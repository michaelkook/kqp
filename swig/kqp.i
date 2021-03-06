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

%module kqp




%{
    
    #ifndef SWIGPYTHON
    #define DENSE_MATRIX DENSE
    #define DENSE_VECTOR DENSE
    #define CONSTANT_VECTOR CONSTANT
    #define IDENTITY_MATRIX IDENTITY
    #endif

    #include <boost/exception/diagnostic_information.hpp> 
    #include <kqp/cleanup.hpp>
    #include <kqp/space_factory.hpp>
    #include <kqp/logging.hpp>
    #include <kqp/decomposition.hpp>

    #include <kqp/feature_matrix/dense.hpp>
    #include <kqp/feature_matrix/kernel_sum.hpp>
    #include <kqp/feature_matrix/sparse.hpp>
    #include <kqp/feature_matrix/sparse_dense.hpp>
    #include <kqp/feature_matrix/unary_kernel.hpp>
    
    #include <kqp/kernel_evd/dense_direct.hpp>
    #include <kqp/kernel_evd/accumulator.hpp>
    #include <kqp/kernel_evd/incremental.hpp>
    #include <kqp/kernel_evd/divide_and_conquer.hpp>

    #include <kqp/cleaning/unused.hpp>
    #include <kqp/cleaning/qp_approach.hpp>

    #include <kqp/probabilities.hpp>

    namespace kqp {

        namespace _AltMatrix { enum AltMatrixType { DENSE_MATRIX, IDENTITY_MATRIX }; }
        namespace _AltVector { enum AltVectorType { DENSE_VECTOR, CONSTANT_VECTOR }; }
        
        template<typename Scalar>
        struct PrimitiveRef {
          Scalar &value;  
          PrimitiveRef(Scalar &value) : value(value) {}
              
          Scalar get() { return value; }
          void set(Scalar newValue) { value = newValue; }
        };
    }
    
    using Eigen::Dynamic;
    using Eigen::Matrix;
%}

// --- Defines some macros

#define Index long
#define override 
#define DEFINE_KQP_HLOGGER(x)
#define KQP_M_DEBUG(x) x

// Preserve the arguments
#define %kqparg(X...) X

// --- Language dependent includes

%include "std_string.i"
//%include "boost_shared_ptr.i"
%include "boost_shared_ptr.i"
%include "std_vector.i"
%include "std_map.i"
%include "std_pair.i"
%include "exception.i"

#if SWIGJAVA
%include "java_kqp.i"
#endif

#ifdef SWIGPYTHON
%include <pycontainer.swg>
#endif

// --- STL related types

%template(BoolList) std::vector<bool>;
%template(IndexList) std::vector<Index>;
%template(StringList) std::vector<std::string>;

// --- Shared pointers
%shared_ptr(kqp::AbstractSpace)

// ---- Command renaming

%rename operator++ next;
%rename operator!= notEqual;
%rename operator()(Index) get;
%rename operator()(Index, Index) get;
%rename operator[] at;

%ignore kqp::Intervals;
%ignore kqp::IntervalsIterator;
%ignore kqp::Dense::getMatrix;
%ignore kqp::LinearCombination;

// ---- Some basic declarations


namespace kqp {
    
    template<typename Scalar>
    struct PrimitiveRef {
        Scalar get();
        void set(Scalar);
    private:
        PrimitiveRef();
    };

    namespace _AltMatrix { enum AltMatrixType { DENSE_MATRIX, IDENTITY_MATRIX }; }
    namespace _AltVector { enum AltVectorType { DENSE_VECTOR, CONSTANT_VECTOR }; }
    
    template<typename Scalar> struct ScalarDefinitions;
        
    //! A dense vector or a constant vector
     template<typename Scalar> struct AltVector {
         typedef Eigen::Matrix<Scalar,Dynamic,1>  VectorType;
         typedef typename Eigen::Matrix<Scalar,Dynamic,1>::ConstantReturnType ConstantVectorType;
     };


     //! Dense or Identity matrix
     template<typename Scalar> struct AltDense {
         typedef Eigen::Matrix<Scalar,Dynamic,Dynamic> DenseType;
         typedef typename Eigen::MatrixBase< Eigen::Matrix<Scalar,Dynamic,Dynamic> >::IdentityReturnType IdentityType;
     };
         
    template<typename T1,typename T2>
    class AltMatrix {
    };
    
    static std::string demangle(unsigned long pointer) {
        return KQP_DEMANGLE((void*)pointer);
    }

%{
    namespace kqp {
    static std::string demangle(long pointer) {
        return KQP_DEMANGLE(pointer);
    }

    }
%}
}


namespace Eigen {
    template<typename Scalar> struct NumTraits;
    
    template<typename Scalar, int Rows, int Cols>
    class Matrix {
    public:
        Matrix(Index rows, Index cols);
    };
    
    template<typename Scalar, int Major>
    class SparseMatrix {
    public:
        SparseMatrix(Index rows, Index cols);
        class InnerIterator;
    };
    
    
}

// Map (from )
// For the iterator part, see http://stackoverflow.com/questions/9465856/no-iterator-for-java-when-using-swig-with-cs-stdmap
%template(LongLongMap) std::map<Index,Index>;

// Exception handling
%exception {
    try {
        $action
    } 
    
    catch(const kqp::exception &e) {
        SWIG_exception(SWIG_RuntimeError, boost::diagnostic_information(e).c_str());
    }  
       
    catch(const boost::exception &e) {
        SWIG_exception(SWIG_RuntimeError, boost::diagnostic_information(e).c_str());

    }
    catch(const std::exception &e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }    
    catch(...) {
        std::cerr << "Caught an unknown exception!" << std::endl;
        throw;
    }
}

%include <kqp/eigen_identity.hpp>
%include <kqp/logging.hpp>

%define shared_template(NAME, TYPE)
%shared_ptr(TYPE)
%template(NAME) TYPE;
%enddef


%include "kqp_all.i"
