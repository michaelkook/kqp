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
#ifndef __KQP_UNARY_KERNEL_SPACE_H__
#define __KQP_UNARY_KERNEL_SPACE_H__

#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include <kqp/feature_matrix.hpp>
#include <kqp/space_factory.hpp>

namespace kqp {
    template<typename Scalar> 
    class UnaryKernelSpace : public SpaceBase< Scalar > {
    public:
        KQP_SCALAR_TYPEDEFS(Scalar);

        UnaryKernelSpace(const FSpace &base) :  m_base(base) {}

        ~UnaryKernelSpace() {}
        
        virtual FMatrixBasePtr newMatrix() const override { return m_base->newMatrix(); }
        // REMOVE
        // virtual FMatrixBasePtr newMatrix(const FeatureMatrixBase<Scalar> &other) const override { return m_base->newMatrix(other); }        

        virtual const ScalarMatrix &k(const FMatrixBase &mX) const override {            
            // FIXME: Big memory leak here (the feature space should be notified when a feature matrix is deleted) !!!
            // So we suppose we are not multithreaded... and compute it each time.
            // ScalarMatrix &gram = m_gramCache[static_cast<const void*>(&mX)];
            
            static ScalarMatrix gram;
            
            gram.resize(0,0);
            
            if (mX.size() == gram.rows()) return gram;
            
            // We lose space here, could be used otherwise???
            Index current = gram.rows();
            if (current < mX.size()) 
                gram.conservativeResize(mX.size(), mX.size());
            
            Index tofill = mX.size() - current;

            fillGram(gram, tofill, mX);
            
            gram.bottomLeftCorner(tofill, current).noalias() = gram.topRightCorner(current, tofill).adjoint().eval();
            
            return gram;
        }
        
        virtual void load(const pugi::xml_node &node) {
            pugi::xml_node selected;
            for(auto child: node) {
                if (child.type() == pugi::xml_node_type::node_element) {
                    if (selected.empty()) {
                        selected = child;
                    } else { 
                        KQP_THROW_EXCEPTION(exception, "A unary kernel element should have no more than one child"); 
                    }
                }
            }

            if (selected.empty())
                KQP_THROW_EXCEPTION(exception, "A unary kernel element should have one child");
            m_base = boost::dynamic_pointer_cast< SpaceBase<Scalar> >(SpaceFactory::load(selected));
        }

        FSpacePtr base() {
            return m_base;
        }

        FSpaceCPtr base() const {
            return m_base;
        }


    protected:
        /**
         * @brief Fill a part of the Gram matrix.
         *
         * Fill the right columns (tofill) of a Gram matrix
         */
        virtual void fillGram(ScalarMatrix &gram, Index tofill, const FMatrixBase &mX) const = 0;

        //! The base feature space 
        FSpace m_base;

    private:              
        //! Gram matrices
        mutable boost::unordered_map<const void *, ScalarMatrix> m_gramCache;
        
    };


    //! Gaussian Kernel \f$k'(x,y) = \exp(\frac{2 Re(k(x,y)) -  k(x) - k(y) \vert}{\sigma^2})\f$
    template<typename Scalar> class GaussianSpace : public UnaryKernelSpace<Scalar> {
    public:
        KQP_SCALAR_TYPEDEFS(Scalar);
#ifndef SWIG
        using UnaryKernelSpace<Scalar>::m_base;
        using UnaryKernelSpace<Scalar>::k;
#endif
        GaussianSpace(Real sigma, const FSpace &base) : UnaryKernelSpace<Scalar>(base), m_sigma(sigma) {}
        GaussianSpace() :  UnaryKernelSpace<Scalar>(FSpace()), m_sigma(1) {}

        virtual FSpacePtr copy() const override { return FSpacePtr(new GaussianSpace<Scalar>(m_sigma, m_base)); }        

        virtual Index dimension() const override { return -1; }
        virtual bool _canLinearlyCombine() const override { return false; }

        
        virtual ScalarMatrix k(const FMatrixBase &mX1, const ScalarAltMatrix &mY1, const RealAltVector &mD1, 
                               const FMatrixBase &mX2, const ScalarAltMatrix &mY2, const RealAltVector &mD2) const override {
            return mD1.asDiagonal() * mY1.adjoint() 
                    * this->f(m_base->k(mX1, mX2), m_base->k(mX1).diagonal(), m_base->k(mX2).diagonal())
                    * mY2 * mD2.asDiagonal();
        }
        

        virtual void update(KernelValues<Scalar> &values) const override {
            KQP_LOG_ASSERT(main_logger, values.children.size() == 1, "Expected only one child in values for gaussian kernel while updating partials");
            auto &child = values.children[0];
            m_base->update(child);
            Scalar sigma_2 = m_sigma * m_sigma;
            Scalar re_inner = Eigen::internal::real(child._inner);
            values._inner =  Eigen::internal::exp((2. * re_inner - child._innerX - child._innerY) / sigma_2);
            values._innerX = 1;
            values._innerY = 1;
        }

        virtual void updatePartials(Real alpha, std::vector<Real> &partials, int offset, const KernelValues<Scalar> &values, int mode) const {
            KQP_LOG_ASSERT(main_logger, values.children.size() == 1, "Expected only one child in values for gaussian kernel while updating partials");
            const auto &child  = values.children[0];
            Scalar exp_v = 1;
            Scalar sigma_2 = m_sigma * m_sigma;
            if (mode == 0) {
                Scalar re_inner = Eigen::internal::real(child.inner(0));
                Scalar v = (2. * re_inner - child.innerX(0) - child.innerY(0)) / sigma_2;
                exp_v = Eigen::internal::exp(v);
                partials[offset] +=  alpha * -2. / m_sigma * v * Eigen::internal::exp(v);
            }


            Scalar beta = alpha * (exp_v / sigma_2);
            m_base->updatePartials(2 * beta, partials, offset+1, child, 0);
            m_base->updatePartials(- beta, partials, offset+1, child, -1);
            m_base->updatePartials(- beta, partials, offset+1, child, 1);
        }

        virtual void updatePartials(Real alpha, std::vector<Real> & partials, int offset, const FMatrixBase &mX, const FMatrixBase &mY) const {
            Scalar inner = m_base->k(mX, mY)(0,0), innerX = m_base->k(mX)(0,0), innerY = m_base->k(mY)(0,0);

            Scalar sigma_2 = m_sigma * m_sigma;
            Scalar re_inner = Eigen::internal::real(inner);
            Scalar v = (2. * re_inner - innerX + innerY) / sigma_2;
            Scalar exp_v = Eigen::internal::exp(v);
            partials[offset] +=  alpha * -2. / m_sigma * v * Eigen::internal::exp(v);

            Scalar beta = alpha * (exp_v / sigma_2);
            m_base->updatePartials(2 * beta, partials, offset+1, mX, mY);
            m_base->updatePartials(- beta, partials, offset+1, mX, mX);
            m_base->updatePartials(- beta, partials, offset+1, mY, mY);
        }

        virtual int numberOfParameters() const {
            return 1 + m_base->numberOfParameters();
        }

        virtual void getParameters(std::vector<Real> & parameters, int offset) const {
            parameters[offset] = m_sigma;
            m_base->getParameters(parameters, offset + 1);
        }

        virtual void setParameters(const std::vector<Real> & parameters, int offset)  {
            m_sigma = parameters[offset];
            if (m_sigma < 0) m_sigma = -m_sigma;
            if (m_sigma < EPSILON) m_sigma = kqp::EPSILON;
            m_base->setParameters(parameters, offset + 1);
        }

        static const std::string &name() { static std::string NAME("gaussian"); return NAME; }

        virtual void load(const pugi::xml_node &node) override {
            m_sigma = boost::lexical_cast<decltype(m_sigma)>(node.attribute("sigma").value());
            UnaryKernelSpace<Scalar>::load(node);
        }

        virtual void save(pugi::xml_node &node) const override {
            pugi::xml_node self = node.append_child(name().c_str());
            self.append_attribute("sigma") = boost::lexical_cast<std::string>(m_sigma).c_str();
            if (m_base)
                m_base->save(self);
        }
    protected:
        virtual void fillGram(ScalarMatrix &gram, Index tofill, const FMatrixBase &mX) const override {
            // Compute the remaining inner products
            const ScalarMatrix &baseGram = m_base->k(mX);
            gram.rightCols(tofill) = this->f(baseGram.rightCols(tofill), baseGram.diagonal(), baseGram.diagonal().tail(tofill));            
        }
        

    private:


        template<typename Derived, typename DerivedRow, typename DerivedCol>
        inline ScalarMatrix f(const Eigen::MatrixBase<Derived> &k, 
                                  const Eigen::MatrixBase<DerivedRow>& rowNorms, 
                                  const Eigen::MatrixBase<DerivedCol>& colNorms) const { 
            return (-(rowNorms.derived().rowwise().replicate(k.cols()) + colNorms.derived().adjoint().colwise().replicate(k.rows()) - 2 * k.derived().real()) / (m_sigma*m_sigma)).array().exp();
        }
      
        Real m_sigma;
    };
    
    //! Polynomial Kernel \f$k'(x,y) = (k(x,y) + D)^p\f$
    template<typename Scalar> class PolynomialSpace  : public UnaryKernelSpace<Scalar> {
    public:
        KQP_SCALAR_TYPEDEFS(Scalar);
#ifndef SWIG
        using UnaryKernelSpace<Scalar>::m_base;
        using UnaryKernelSpace<Scalar>::k;
#endif
        virtual FSpacePtr copy() const override { return FSpacePtr(new PolynomialSpace<Scalar>(m_bias, m_degree, m_base)); }        

        PolynomialSpace(Real bias, int degree, const FSpace &base) : UnaryKernelSpace<Scalar>(base), m_bias(bias), m_degree(degree) {}
        PolynomialSpace() : UnaryKernelSpace<Scalar>(FSpace()), m_bias(0), m_degree(1) {}

        virtual Index dimension() const override { return -1; }
        virtual bool _canLinearlyCombine() const override { return false; }

        virtual ScalarMatrix k(const FMatrixBase &mX1, const ScalarAltMatrix &mY1, const RealAltVector &mD1, 
                               const FMatrixBase &mX2, const ScalarAltMatrix &mY2, const RealAltVector &mD2) const override {
            return mD1.asDiagonal() * mY1.adjoint() * this->f(m_base->k(mX1,mX2)) * mY2 * mD2.asDiagonal();
        }

        virtual void updatePartials(Real alpha, std::vector<Real> & partials, int offset, const FMatrixBase &mX, const FMatrixBase &mY) const override {
            Scalar inner = m_base->k(mX,mY)(0,0);
            Scalar v = (Scalar)m_degree * Eigen::internal::pow(inner + m_bias, (Scalar)m_degree - 1);
            partials[offset] += alpha * inner * v;
            m_base->updatePartials(alpha * v, partials, offset+1, mX, mY);
        }

        virtual void updatePartials(Real alpha, std::vector<Real> &partials, int offset, const KernelValues<Scalar> &values, int mode) const override {
            KQP_LOG_ASSERT(main_logger, values.children.size() == 1, "Expected only one child in values for polynomial kernel while updating partials");
            Scalar v = (Scalar)m_degree * Eigen::internal::pow(values.children[0].inner(mode) + m_bias, (Scalar)m_degree - 1);
            partials[offset] += alpha * values.inner(mode) * v;

            m_base->updatePartials(alpha * v, partials, offset+1, values.children[0], mode);
        }

        virtual void update(KernelValues<Scalar> &values) const override {
            KQP_LOG_ASSERT(main_logger, values.children.size() == 1, "Expected only one child in values for polynomial kernel while updating partials");
            auto &child = values.children[0];
            m_base->update(child);
            values._inner = Eigen::internal::pow(child._inner + m_bias, (Scalar)m_degree);
            values._innerX = Eigen::internal::pow(child._innerX + m_bias, (Scalar)m_degree);            
            values._innerY = Eigen::internal::pow(child._innerY + m_bias, (Scalar)m_degree);
        }

        virtual int numberOfParameters() const {
            return 1 + m_base->numberOfParameters();
        }

        virtual void getParameters(std::vector<Real> & parameters, int offset) const {
            parameters[offset] = m_bias;
        }

        virtual void setParameters(const std::vector<Real> & parameters, int offset)  {
            m_bias = parameters[offset];
        }


        static const std::string &name() { static std::string NAME("polynomial"); return NAME; }
        
        virtual void load(const pugi::xml_node &node) {
            m_bias = boost::lexical_cast<decltype(m_bias)>(node.attribute("bias").value());
            m_degree = boost::lexical_cast<decltype(m_degree)>(node.attribute("degree").value());
            UnaryKernelSpace<Scalar>::load(node);
        }

        virtual void save(pugi::xml_node &node) const override {
            pugi::xml_node self = node.append_child(name().c_str());
            self.append_attribute("degree") = boost::lexical_cast<std::string>(m_degree).c_str();
            self.append_attribute("bias") = boost::lexical_cast<std::string>(m_bias).c_str();
            if (m_base)
                m_base->save(self);
        }

    protected:
        virtual void fillGram(ScalarMatrix &gram, Index tofill, const FMatrixBase &mX) const override {           
            gram.rightCols(tofill) = this->f(m_base->k(mX).rightCols(tofill));
        }

        
    private:
        template<typename Derived>
        inline ScalarMatrix f(const Eigen::MatrixBase<Derived> &k) const { 
            return (k.derived() + ScalarMatrix::Constant(k.rows(), k.cols(), m_bias)).array().pow(m_degree);
        }
      
        Real m_bias;
        int m_degree;
    };
    
}

#define KQP_SCALAR_GEN(scalar) \
    extern template class kqp::GaussianSpace<scalar>; \
    extern template class kqp::PolynomialSpace<scalar>;
#include <kqp/for_all_scalar_gen.h.inc>
#undef KQP_SCALAR_GEN

#endif