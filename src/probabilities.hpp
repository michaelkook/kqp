/*
 This file is part of the Kernel Quantum Probability library (KQP).
 
 KQP is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 KQP is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with KQP.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __KQP_PROBABILITIES_H__
#define __KQP_PROBABILITIES_H__

#include "Eigen/Core"

#include "alt_matrix.hpp"
#include "feature_matrix.hpp"
#include "kernel_evd.hpp"

namespace kqp {
    /**
     * Common class shared by (fuzzy) events and densities
     * 
     * <p>
     * The underlying density/subspace is represented by
     * <ul>
     * <li>A set of base vectors provided by {@linkplain FeatureMatrix} X</li>
     * <li>A linear combination matrix Y</li>
     * <li>A diagonal matrix S</li>
     * </ul>
     * 
     * such as <code>A X</code> is usually (i.e.
     * <code>AXX<sup>T</sup>A<sup>T</sup></code> is the identity), and the density
     * <code>rho</code> is expressed as <div>
     * <code>rho = A U S<sup>2</sup> U<sup>T</sup> A<sup>T</sup></code></div>
     * </p>
     * 
     * @date May 2011
     * @author B. Piwowarski <benjamin@bpiwowar.net>
     */
    template <class FMatrix> class KernelOperator {       
    public:
        typedef ftraits<FMatrix> FTraits;
        typedef typename FTraits::Scalar Scalar;
        typedef typename FTraits::Matrix  Matrix;
        typedef typename FTraits::AltMatrix  AltMatrix;
        typedef typename FTraits::RealVector RealVector;
        
        
        /**
         * Creates an object given a Kernel EVD
         * 
         * @param evd The kernel EVD 
         */
        KernelOperator(const KernelEVD<FMatrix>& evd) : orthonormal(true) {
            evd.get_decomposition(mX, mY, mS);
            mS = mS.cwiseSqrt();
        }
        
        
        /**
         * \brief Creates a new kernel operator
         */
        KernelOperator(const FMatrix &mX, const kqp::AltMatrix<Scalar> &mY, const RealVector &mS) : mX(mX), mY(mY), mS(mS) {
        }
        
        
        /**
         * Creates a one dimensional eigen-decomposition representation
         * 
         * @param list The list of vectors
         *           
         * @param copy If the object should be copied
         */
        KernelOperator(const FMatrix &mX) : mX(mX), orthonormal(true) {
            mS = RealVector::Ones(mX.size());
        }
        
        /**
         * Trim the eigenvalue decomposition to a lower rank
         * 
         * @param newRank
         *            The new rank of the subspace
         */
        void trim(size_t newRank);        
        
        /**
         * Get the rank of the operator
         */
        size_t getRank() const {
            if (orthonormal) return mS.size();
        }
        
    protected:
        /**
         * The base vector list
         */
        FMatrix  mX;
        
        /**
         * The combination matrix.
         * 
         * In case of an EVD decomposition, mX mY is orthonormal
         */
        AltMatrix mY;
        
        /**
         * The singular values
         *
         * This matrix is used only if the KernelOperator is in a 
         * EVD decomposed form
         */
        RealVector mS;
        
        //! Is the decomposition othonormal, i.e. is Y^T X^T X Y the identity?
        bool orthonormal;
        
        //! Is the decomposition an observable, i.e. all the non null eigenvalues equal 1?
        bool observable;
        
    };
    
    template <class FMatrix> class Density;
    
    /**
     * A document subspace is defined by the basis vectors (matrix {@linkplain #mU}
     * ). A diagonal matrix ({@linkplain #mS}) defines the weights associated to the
     * basis vectors
     * 
     * @author B. Piwowarski <benjamin@bpiwowar.net>
     * 
     */
    template <class FMatrix> class Event : public KernelOperator<FMatrix>  {
    public:        
        /**
         * Construct a Event from a kernel EVD. See
         * {@linkplain KernelEigenDecomposition#KernelEigenDecomposition(KernelEVD, bool)}
         * @param evd The kernel EVD decomposition
         * @param fuzzy If false (default), the eigenvalues are ignored
         */
        Event(const KernelEVD<FMatrix> &evd, bool fuzzy = false) : KernelOperator<FMatrix>(evd) {            
        }
        
        friend class Density<FMatrix>;
    };
    
    /**
     * A probability density
     * 
     * @author B. Piwowarski <benjamin@bpiwowar.net>
     */
    template <class FMatrix> class Density: public KernelOperator<FMatrix>  {
    public: 
        typedef kqp::ftraits<FMatrix> FTraits;
        typedef typename FTraits::Scalar Scalar;
        typedef typename FTraits::Real Real;
        typedef typename FTraits::Matrix  Matrix;
        typedef typename FTraits::AltMatrix  AltMatrix;
        typedef typename FTraits::ScalarMatrix ScalarMatrix;
        typedef typename FTraits::RealVector RealVector;
        
        /**
         * Creates a new density
         * 
         * @param evd
         * @param deepCopy
         */
        Density(const KernelEVD<FMatrix>& evd) : KernelOperator<FMatrix>(evd) {
        }
        
        Density(const FMatrix &mX, const AltMatrix &mY, const RealVector &mD) : KernelOperator<FMatrix>(mX, mY, mD) {
        }
        
        
        /**
         * Compute the probability of an event
         * 
         * @param subspace
         *            The event
         * @param fuzzyEvent
         *            The event should be considered as "fuzzy" -- i.e. each
         *            dimension is weighted by the corresponding sigma
         * @return The probability
         */
        Real computeProbability(const Event<FMatrix>& subspace) const {
            ScalarMatrix result = getProbabilityMatrix(subspace);
            
            if (result.rows() == 0) return 0;
            
            return result.squared_norm();
        }
        
        /**
         * Pre-computation of a probability. This method is shared by others.
         * 
         * <p>
         * Given the subspace representation \f$ S U \f$ and the density
         * \f$ S^\prime U^\prime \f$, computes \f$ U^\top U^\prime S^\prime \f$ (crisp
         * subspace) or \f$ S U^\top U^\prime S^\prime\f$ (fuzzy subspace). The sum of
         * the squares of the matrix correspond to the probability of observing the
         * subspace given the density.
         * </p>
         * 
         * <p>
         * Each row of the resulting matrix correspond to the probability associated
         * with one of the dimension of the subspace. Similarly, each column is
         * associated to a dimension of the density.
         * </p>
         * 
         * @param subspace The subspace
         * @param fuzzyEvent True if the event should be considered as fuzzy
         * @return A matrix where each row correspond to one dimension of the density, 
         *         and each column to one dimension of the subspace
         */
        ScalarMatrix getProbabilityMatrix(const Event<FMatrix>& subspace,
                                          bool fuzzyEvent) const {
            // Compute Y_s^T (P) Y_d S_d
            ScalarMatrix mP = subspace.mY.transpose() * subspace.mX.computeInnerProducts(this->mX) * this->mY * this->SmS;
            
            // Pre-multiply the result by S_s if using fuzzy subspaces
            if (fuzzyEvent)
                mP = subspace.mS * mP;
            
            return mP;
        }
        
        /**
         * Get the matrix V^T * (U x S) where U is the basis and S is the square
         * root of the eigenvalues. The Froebenius norm of the resulting matrix is
         * the probability of the event defined by v x v^t
         * 
         * @param vector
         *            The vector v
         * @return
         */
        ScalarMatrix getProbabilityMatrix(const FMatrix& fmatrix) const {
            return this->mX.computeInnerProducts(fmatrix) * this->mY * this->mS;
        }
        
        /**
         * @brief Computes the divergence with another density
         *
         * Computes the divergence defined in the paper
         * "Conditional expectation in an operator algebra. IV. Entropy and information" by H. Umegaki (1962),
         * at page 69. The formula is:
         * \f[ J(\rho || \tau) = tr(\rho \log (\rho) - \rho \log(\tau))) \f]
         *
         * Expects both decomposition to be in an orthonormal form
         */
        Real computeDivergence(const Density<FMatrix> &tau, Real epsilon = EPSILON) const {
            // --- Notation
            const Density<FMatrix> &rho = *this;
            
            ScalarMatrix inners;
            kqp::inner(rho.mX, tau.mX, inners);
            inners = (rho.mY.adjoint() * inners * tau.mY * tau.mS.asDiagonal()).eval();

            // --- Compute tr(p log q)
            Scalar plogq = 0;
        
            Index dimension = rho.mX.dimension();
            
			// The background density span the subspace 
			Scalar alpha = 1. / (Scalar)(dimension);
			Scalar alpha_noise = epsilon * alpha;
            
			// Smoothing probability
            if (epsilon >= EPSILON) 
                plogq = log(alpha_noise) * (1. - inners.squaredNorm());
            
			
			// Main computation
			RealVector mD(tau.mS.rows());
			
			for(int j = 0; j < tau.mS.rows(); j++) {
				Scalar sj = tau.mS(j) * tau.mS(j);
				
				Scalar x = -log(((Scalar)1 - epsilon) * sj + alpha_noise);
				if (x < 0)
					if (x > - EPSILON) x = 0;
					else KQP_THROW_EXCEPTION_F(arithmetic_exception, "%g is not greather than 0. Note: S[%d] = %g", %x %j %tau.mS(j,j));
				
				mD(j) = sqrt(x);
			}
			
            
            
			plogq -= (inners * mD.asDiagonal()).squaredNorm();
            
            
            // --- Compute tr(p log p)
            Scalar plogp = 0;
            for (int i = 0; i < rho.mS.rows(); i++) {
                Scalar x = rho.mS(i) * rho.mS(i);
                plogp +=  x * log(x);
            }
            
            return plogp - plogq;        
            
		}
        
        
    };
    
    
    
}

#endif