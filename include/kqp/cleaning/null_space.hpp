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

#ifndef __KQP_REDUCED_SET_NULL_SPACE_H__
#define __KQP_REDUCED_SET_NULL_SPACE_H__

#include <algorithm> // sort

#include <kqp/kqp.hpp>
#include <Eigen/LU> // LU decomposition

#include <kqp/feature_matrix.hpp>
#include <kqp/subset.hpp>
#include <kqp/cleaning/unused.hpp>

namespace kqp {
#   include <kqp/define_header_logger.hpp>
    DEFINE_KQP_HLOGGER("kqp.cleaning.null_space");

#ifndef SWIG
    template <class ComparableArray, typename Index = int>
    struct IndirectSort {
        const ComparableArray &array;
        IndirectSort(const ComparableArray &array) : array(array) {}
        bool operator() (int i,int j) { return (array[i] < array[j]);}
    };
    
    template<typename Scalar>
    struct ReducedSetNullSpace {
        KQP_SCALAR_TYPEDEFS(Scalar);
        
        /**
         * @brief Removes pre-images with the null space method
         * 
         * Removes pre-images using the null space method.
         * We have \f$ X Z  = 0\f$ and want to find \f$X^\prime\f$ and \f$A\f$ such that
         * \f$ X = (\begin{array}{cc} X^\prime & X^\prime A \end{array} )
         * 
         * @param mF (\b in) the feature matrix \f$X\f$ (\b out) the reduced feature matrix \f$X^\prime\f$
         * @param kernel (in) the null space basis \f$Z\f$ for the matrix \ref mF (out) the matrix \f$A\f$ such that \f$ X^\prime A = X^{\prime\prime} \f$
         * @param mP (out) Permutation matrix so that \f$X P = (X^\prime X^{\prime\prime})\f$
         * @param weights give an order to the different pre-images
         * @param delta
         */
        static FMatrix remove(const FMatrix &mF, ScalarMatrix &kernel, Eigen::PermutationMatrix<Dynamic, Dynamic, Index>& mP, const RealVector &weights, double delta = 1e-4) {
            typedef typename Eigen::PermutationMatrix<Dynamic, Dynamic, Index> Permutation;

            // --- Check if we have something to do
            if (mF->size() == 0)
                return mF;
            
            // --- Look up at the indices to remove
            
            // Get the pre-images available for removal (one per vector of the null space, i.e. by columns in kernel)
            // Summarize the result per pre-image (number of vectors where it appears)
            // Eigen::Matrix<int,Dynamic,1> num_vectors = (kernel.array().abs() > kernel.array().abs().colwise().maxCoeff().replicate(kernel.rows(), 1) * delta).template cast<int>().rowwise().sum();
            
            std::vector<Index> to_remove;
            for(Index i = 0; i < /*num_vectors*/kernel.rows(); i++)
                    to_remove.push_back(i);

            // Sort        
            const Index pre_images_count = kernel.rows();
            const std::size_t remove_size = kernel.cols();
            const Index keep_size = mF->size() - remove_size;
            
            mP = Permutation(mF->size());
            mP.setIdentity();
            
            std::sort(to_remove.begin(), to_remove.end(), IndirectSort<RealVector>(weights));
            std::vector<bool> selection(mF->size(), true);
            std::vector<bool> used(to_remove.size(), false);
            
            ScalarVector v;
            
            // --- Remove the vectors one by one (Pivoted Gauss)
            Index last = mF->size() - 1;

            Index remaining = remove_size;

            while (remaining > 0) {
                // Select the pre-image to remove along with the null space vector
                // that will be used
                // TODO: find a better way

                Eigen::Matrix<Real,Dynamic,1> colnorms = kernel.colwise().norm();

                size_t i = 0;
                Index j = -1;
                Real max = 0;
                Real threshold;
                for(size_t index = 0; index < to_remove.size(); index++) {
                    // remove the ith pre-image
                    i = to_remove[index];
                    if (!selection[i]) continue;
                    
                    // Searching for the highest magnitude
                    j = -1;
                    // ... but we want it above a given threshold
                    max = 0;
                    for(Index k = 0; k < kernel.cols(); k++) {
                        Real x = kernel.array().abs()(i, k);
                        if (!used[k] && x > max && x > delta * colnorms(k)) {
                            threshold = delta * colnorms(k); // just for debug
                            max = x;
                            j = k;
                        }
                    }
                    if (j >= 0) break;
                }
                if (j == -1)
                    KQP_THROW_EXCEPTION_F(assertion_exception,
                     "Could not find a way to remove the a pre-image with null space (%d/%d pre-images). %d remaining.",
                     %kernel.cols() %kernel.rows() %remaining);
                KQP_HLOG_DEBUG_F("Selected pre-image %d with basis vector %d [%g > %g; norm=%g]", %i %j %max %threshold %colnorms(j));

                remaining--;
                used[j] = true;
                
                // Update permutation by putting this vector at the end
                selection[i] = false;            
                mP.indices()(i) = j + keep_size;
                last -= 1;
                
                // Update the matrix
                Scalar kij = kernel(i,j);
                KQP_HLOG_DEBUG_F("Normalizing column %d [norm %g] with the inverse of %g", %j %kernel.col(j).norm() %kij)
                v = kernel.col(j) / kij;
                kernel.col(j) /= -kij;
                
                assert(!kqp::isNaN(kernel.col(j).squaredNorm()));
                
                kernel(i,j) = 0;
                
                kernel = ((Eigen::Identity<Scalar>(pre_images_count, pre_images_count) 
                           - v * ScalarVector::Unit(pre_images_count, i).adjoint()) * kernel).eval();
            }
            
            
            // --- Remove the vectors from mF and set the permutation matrix
            
            // Remove unuseful vectors
            
            select_rows(selection.begin(), selection.end(), kernel, kernel);
            
            // Complete the permutation matrix
            Index count = 0;
            for(size_t index = 0; index < selection.size(); index++) 
                if (selection[index]) {
                    mP.indices()(index) = count++;
                }
            
            return mF->subset(selection.begin(), selection.end());
        }
        
        /**
         * @brief Removes unuseful pre-images 
         *
         * 1. Removes unused pre-images 
         * 2. Computes a \f$LDL^\dagger\f$ decomposition of the Gram matrix to find redundant pre-images
         * 3. Removes newly unused pre-images
         */
        static void run(const FSpace &fs, const FMatrixPtr &mF, ScalarAltMatrix &mY, Real epsilon = Eigen::NumTraits<Real>::epsilon()) {
            
            // Removes unused pre-images
            CleanerUnused<Scalar>::run(mF, mY);
            
            // Dimension of the problem
            Index N = mY.rows();
            assert(N == mF->size());
            
            // LDL decomposition (stores the L^T matrix)
            Eigen::FullPivLU<ScalarMatrix> lu_decomposition(fs->k(mF));

            // Set the thresholds according to the same heuristic than Eigen
            lu_decomposition.setThreshold(epsilon * lu_decomposition.matrixLU().diagonalSize());
            
            // Stop if full rank
            Index rank = lu_decomposition.rank();
            KQP_HLOG_DEBUG_F("Rank of LU decomposition is %d/%d [epsilon=%g]", %rank %N %epsilon);
            if (rank == N) 
                return;
            
            // Remove pre-images using the kernel
            ScalarMatrix kernel = lu_decomposition.kernel();
            RealVector weights = mY.rowwise().squaredNorm().array() * fs->k(mF).diagonal().array().abs();
            Eigen::PermutationMatrix<Dynamic, Dynamic, Index> mP;
            *mF = std::move(*remove(mF, kernel, mP, weights));
            
            // Y <- (Id A) P Y
            ScalarMatrix mY2(mY);
            mY2= (mP * mY2).topRows(mF->size()) + kernel * (mP * mY2).bottomRows(mY.rows() - mF->size());
            mY.swap(mY2);
            
            // Removes unused pre-images
            CleanerUnused<Scalar>::run(mF, mY);
        }
        
        static void run(const FSpacePtr &fs, const FMatrixPtr &mF, ScalarMatrix &mY) {
            ScalarAltMatrix _mY;
            _mY.swap(mY);
            run(fs, mF, _mY);
            _mY.swap(mY);
        }
        
    };

#endif

    template<typename Scalar> class CleanerNullSpace: public Cleaner<Scalar> {
    public:
        typedef typename Eigen::NumTraits<Scalar>::Real Real;
        CleanerNullSpace(Real epsilon = Eigen::NumTraits<Scalar>::epsilon()) : m_epsilon(epsilon) {

        }
        virtual void cleanup(Decomposition<Scalar> &d) const {
           ReducedSetNullSpace<Scalar>::run(d.fs, d.mX, d.mY, m_epsilon); 
        }        
    private:
        Real m_epsilon;
    };
    
    
# ifndef SWIG
# define KQP_SCALAR_GEN(Scalar) extern template struct ReducedSetNullSpace<Scalar>;
# include <kqp/for_all_scalar_gen.h.inc>
# endif 
}

#endif
