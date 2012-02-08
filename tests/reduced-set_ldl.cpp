#include "kqp.hpp"

#include "reduced_set/ldl_approach.hpp"
#include "feature_matrix/dense.hpp"
#include "tests_utils.hpp"

DEFINE_LOGGER(logger, "kqp.test.reduced-set.ldl")

namespace kqp {
    
    int test_reduced_set_ldl(std::deque<std::string> &args) {
        // --- Simple test
        
        
        
        // --- Random test

        // Parameters
        Index dim = 10;
        Index n = 3;
        
        // Gets a rank-n matrix and a full rank matrix
        Eigen::MatrixXd _mF = generateMatrix<double>(dim, n);        
        Eigen::MatrixXd _mY = generateMatrix<double>(dim, dim);                        
        
        // Copy
        Eigen::MatrixXd mY = _mY;
        DenseMatrix<double> mF(_mF);
        
        kqp::removePreImagesWithLDL(mF, mY);
        
        Eigen::MatrixXd m1 = mF.get_matrix() * mY;
        Eigen::MatrixXd m2 = _mF * _mY;
        double error = (m1 - m2).norm();
        
        Index delta = (_mY.rows() - mY.rows());
        KQP_LOG_INFO_F(logger, "Error is %g and row difference is %d", %error %delta);

        return mF.size() == n && (error < EPSILON * delta);
    }
}