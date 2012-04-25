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
#ifndef __KQP_SPARSE_DENSE_FEATURE_MATRIX_H__
#define __KQP_SPARSE_DENSE_FEATURE_MATRIX_H__

#include <boost/function_output_iterator.hpp>

#include <kqp/subset.hpp>
#include <kqp/feature_matrix.hpp>
#include <Eigen/Sparse>


namespace kqp {
    
    /**
     * @brief A feature matrix where vectors are in a dense subspace (in the canonical basis).
     *
     * This class makes the hypothesis that vectors have only a few non null components (compared to the dimensionality of the space), 
     * and that those components are mostly the same.
     *
     * In practice, the matrix is a map from a row index to a vector (along with a count of the number of zeros),
     * where each vector has a size less or equal to the number of columns of the sparse matrix.
     * @ingroup FeatureMatrix
     */
    template <typename _Scalar> 
    class SparseDenseMatrix : public FeatureMatrix< SparseDenseMatrix<_Scalar> > {
    public:
        KQP_FMATRIX_COMMON_DEFS(SparseDenseMatrix<_Scalar>);
        typedef std::map<Index,Index> RowMap;

        //! Dimension of the space
        Index m_dimension;
        
        
        SparseDenseMatrix() :  m_dimension(0) {}
        SparseDenseMatrix(Index dimension) :  m_dimension(dimension) {}
        
#ifndef SWIG
        SparseDenseMatrix(RowMap &&map, ScalarMatrix &&matrix) : m_map(std::move(map)), m_matrix(std::move(matrix)) {
            
        }
#endif

        //! Creates from a sparse matrix
        SparseDenseMatrix(const Eigen::SparseMatrix<_Scalar, Eigen::ColMajor> &mat, double threshold = EPSILON) : m_dimension(mat.rows()) {
            // --- Compute which rows we need
            
            
            // Computing the column norms
            RealVector norms(mat.cols());
            for (Index k=0; k<mat.cols(); ++k)  { // Loop on cols
                // FIXME: use norm() when Eigen fixed
                // norms[k] = mat.innerVector(k).norm();
                for (typename Eigen::SparseMatrix<_Scalar, Eigen::ColMajor>::InnerIterator it(mat,k); it; ++it) { // Loop on rows 
                    norms[k] += Eigen::internal::abs2(it.value());
                }
            }
            
            norms = norms.cwiseSqrt();
            
            // Computing selected rows map
            for (int k=0; k<mat.outerSize(); ++k) { // Loop on cols
                for (typename Eigen::SparseMatrix<_Scalar, Eigen::ColMajor>::InnerIterator it(mat,k); it; ++it) { // Loop on rows
                    if (std::abs(it.value()) / norms[it.col()] > threshold) {
                        if (m_map.find(it.row()) == m_map.end()) {
                            m_map[it.row()] = m_map.size();                            
                        }
                    }
                }
            }
            
            // --- Copying
            m_matrix.resize(m_map.size(), mat.cols());
            m_matrix.setZero();
            
            for (int k=0; k<mat.outerSize(); ++k) { // Loop on cols
                for (typename Eigen::SparseMatrix<_Scalar, Eigen::ColMajor>::InnerIterator it(mat,k); it; ++it) { // Loop on rows
                    m_matrix(m_map[it.row()], it.col()) = it.value();
                }
            }
            
        }

        
        //! Creates from a sparse matrix
        SparseDenseMatrix(const Eigen::SparseMatrix<_Scalar, Eigen::RowMajor> &mat, double threshold = EPSILON) : m_dimension(mat.rows()) {
            // --- Compute which rows we need
            
                      
            // Computing the column norms
            RealVector norms(mat.cols());
            norms.setZero();
            for (int k=0; k<mat.outerSize(); ++k)  // Loop on rows
                for (typename Eigen::SparseMatrix<_Scalar, Eigen::RowMajor>::InnerIterator it(mat,k); it; ++it) // Loop on cols
                    norms[it.col()] += Eigen::internal::abs2(it.value());

            // Computing selected rows
            norms = norms.cwiseSqrt();
            for (int k=0; k<mat.outerSize(); ++k) {
                bool any = false;
                for (typename Eigen::SparseMatrix<_Scalar, Eigen::RowMajor>::InnerIterator it(mat,k); it && !any; ++it) {
                    any |= std::abs(it.value()) / norms[it.col()] > threshold;
                }
                if (any) 
                    m_map[k] = m_map.size();
            }

            // --- Copying
            m_matrix.resize(m_map.size(), mat.cols());
            m_matrix.setZero();
            
            for(auto i = m_map.begin(); i != m_map.end(); i++) {
                for (typename Eigen::SparseMatrix<_Scalar, Eigen::RowMajor>::InnerIterator it(mat,i->first); it; ++it) 
                    m_matrix(i->second, it.col()) = it.value();

            }
            
        }
        
        /**
         * @brief Construct from a dense matrix
         *
         * Discard rows where entries are neglectable (see the class documentation).
         *
         * @param mat The matrix to copy
         * @param threshold The threshold for a value to be neglectable
         */
        SparseDenseMatrix(const ScalarMatrix &mat, double threshold = EPSILON) : m_dimension(mat.rows()) {
            // Compute which rows we need
            Matrix<Real, 1, Dynamic> norms = mat.cwiseAbs2().colwise().sum();
            
            for(Index i = 0; i < mat.rows(); i++) {
                if ((std::abs(mat.row(i).array()) / norms.array() > threshold).any()) 
                    m_map[i] = m_map.size();
            }
            
            // Copy the selected rows
            m_matrix.resize(m_map.size(), mat.cols());
            if ((size_t)mat.rows() == m_map.size())
                m_matrix = mat;
            else 
                for(auto i = m_map.begin(); i != m_map.end(); i++) 
                    m_matrix.row(i->second) = mat.row(i->first);
            
        }
        
        
        //! Converts to a dense matrix (mostly used for debug)
        ScalarMatrix toDense() const {
            ScalarMatrix mat(m_dimension, size());
            mat.setZero();
            
            for(auto i = m_map.begin(); i != m_map.end(); i++)
                mat.row(i->first) = m_matrix.row(i->second);
            
            return mat;
        }
        
        //! Get the dense dimension
        Index denseDimension() const {
            return m_matrix.rows();
        }
        
        /** @brief Cleanup the near zero entries
         *
         * An entry of a vector is considered as zero if its ratio to the maximum magnitude component of the vectors
         * is above a given threshold. If a full row is considered as below zero, it is removed.
         *
         * @param threshold The threshold above which a value is considered as zero
         */
        void cleanup(Real threshold = EPSILON) {
            Matrix<Real, 1, Dynamic> norms = m_matrix.cwiseAbs2().colwise().sum();

            std::vector<bool> selected(m_matrix.rows(), true);
            for(auto i = m_map.begin(); i != m_map.end(); ) { 
                auto j = i;
                i++;
                if ((std::abs(m_matrix.row(j->second).array()) / norms.array() <= threshold).all()) {
                    selected[j->second] = false;
                    m_map.erase(j);
                }
            }
            
            Index newSize = 0;
            std::vector<Index> ix(m_matrix.rows(), 0);
            for(Index i = 0; i < m_matrix.rows(); i++) {
                ix[i] = newSize;
                if (selected[i]) newSize++;
            }
            for(auto i = m_map.begin(); i != m_map.end(); i++) 
                i->second = ix[i->second];
            
            
            select_rows(selected, m_matrix, m_matrix);            
            assert(m_matrix.rows() == newSize);
        }
    protected:
        
        // --- Base methods 
        inline Index _size() const { 
            return m_matrix.cols();
        }
        
        Index _dimension() const {
            return m_dimension;
        }
        
        struct Insert {
            RowMap &m_map;
            void operator()(const RowMap::const_reference &x) const {
                m_map[x.first] = m_map.size();
            }
        };
        
        void _add(const Self &other, const std::vector<bool> *which = NULL)  {
            if (m_dimension != other.m_dimension)
                KQP_THROW_EXCEPTION_F(illegal_argument_exception, "Cannot add vectors of different sizes (%d vs %d)", %m_dimension %other.m_dimension);
            
            // Compute which vectors to add
            std::vector<Index> ix;
            Index toAdd = 0;
            if (which) {
                for(size_t i = 0; i < which->size(); i++)
                    if ((*which)[i]) ix.push_back(i);
            } else toAdd = other._size();
            
            if (toAdd == 0) return;
            
            std::set_difference(other.m_map.begin(), other.m_map.end(), m_map.begin(), m_map.end(), 
                                boost::make_function_output_iterator(Insert({m_map})));
            
            
            // Add 
            Index offset = m_matrix.cols();
            m_matrix.conservativeResize(m_map.size(), m_matrix.cols() + toAdd);
            for(auto i = other.m_map.begin(); i != other.m_map.end(); i++) {
                Index otherRow = i->second;
                Index selfRow = m_map[i->first];
                
                if (which) {
                    for(size_t k = 0; k < ix.size(); k++)
                        m_matrix(selfRow, offset + k) = other.m_matrix(otherRow, ix[k]);
                } else
                    m_matrix.row(selfRow).tail(toAdd) = other.m_matrix.row(otherRow);
            }
                        
        }
        
        
        const ScalarMatrix &_inner() const {
            if (size() == 0) return m_gramMatrix;
            
            // We lose space here, could be used otherwise???
            Index current = m_gramMatrix.rows();
            if (current < size()) 
                m_gramMatrix.conservativeResize(size(), size());
            
            Index tofill = size() - current;
            
            // Compute the remaining inner products
            m_gramMatrix.bottomRightCorner(tofill, tofill).noalias() = m_matrix.rightCols(tofill).adjoint() * m_matrix.rightCols(tofill);
            m_gramMatrix.topRightCorner(current, tofill).noalias() = m_matrix.leftCols(current).adjoint() * m_matrix.rightCols(tofill);
            m_gramMatrix.bottomLeftCorner(tofill, current) = m_gramMatrix.topRightCorner(current, tofill).adjoint().eval();
            
            return m_gramMatrix;
        }
        
        
        //! Computes the inner product with another matrix
        template<class DerivedMatrix>
        void _inner(const Self &other, const Eigen::MatrixBase<DerivedMatrix> &_result) const {
            DerivedMatrix &result = const_cast<DerivedMatrix&>(_result.derived());
            result.setZero();
            
            struct Collector {
                const ScalarMatrix &mat1, &mat2;
                const RowMap &map1, &map2;
                DerivedMatrix &mat;
                
                void operator()(const RowMap::const_reference & x) const {
                    Index ix1 = this->map1.find(x.first)->second;
                    Index ix2 = this->map2.find(x.first)->second;
                    this->mat += this->mat1.row(ix1).transpose() * this->mat2.row(ix2);
                }
            } collector({m_matrix, other.m_matrix, m_map, other.m_map, result});

            std::set_intersection(m_map.begin(), m_map.end(), other.m_map.begin(), other.m_map.end(), 
                                  boost::make_function_output_iterator(collector));
            
        }
        
        
        // Computes alpha * X * A + beta * Y * B (X = *this)
        Self _linear_combination(const ScalarAltMatrix &mA, Scalar alpha, const Self *mY, const ScalarAltMatrix *mB, Scalar beta) const {
            // Simple case: we don't have to add another matrix
            if (!mY) 
                return Self(RowMap(m_map), alpha * m_matrix * mA);
            
            // Add the keys
            RowMap newMap;
            std::set_union(mY->m_map.begin(), mY->m_map.end(), m_map.begin(), m_map.end(), 
                           boost::make_function_output_iterator(Insert({newMap})));


            // Perform the linear combination
            ScalarMatrix mat(newMap.size(), mA.cols());
            mat.setZero();
            
            for(auto i = m_map.begin(), end = m_map.end(); i != end; i++) 
                mat.row(newMap[i->first]) = alpha * m_matrix.row(i->second) * mA;

            for(auto i = mY->m_map.begin(), end = mY->m_map.end(); i != end; i++) 
                mat.row(newMap[i->first]) += beta * mY->m_matrix.row(i->second) * *mB;
            
            // Move and cleanup before returning
            Self sdMat(std::move(newMap), std::move(mat));
            sdMat.cleanup(EPSILON);
            return sdMat;
        }
        
        
        
        void _subset(const std::vector<bool>::const_iterator &begin, const std::vector<bool>::const_iterator &end, Self &into) const {
            select_columns(begin, end, m_matrix, into.m_matrix);
            
            into.m_gramMatrix.resize(0,0);
            if (&into != this) {
                into.m_dimension = m_dimension;
                into.m_map = m_map;
            }
        }
        
    private:
        //! Cache of the gram matrix
        mutable ScalarMatrix m_gramMatrix;
        
        //! A map from row to row
        std::map<Index, Index> m_map;

        //! The dense matrix
        ScalarMatrix m_matrix;

    };
    
    
    // The scalar for dense feature matrices
    template <typename _Scalar> struct FeatureMatrixTypes<SparseDenseMatrix<_Scalar> > {
        typedef _Scalar Scalar;
        enum {
            can_linearly_combine = 1
        };
    };
    
    
    
# // Extern templates
# define KQP_SCALAR_GEN(scalar) \
  extern template class SparseDenseMatrix<scalar>;
# include <kqp/for_all_scalar_gen>
    
} // end namespace kqp

#endif