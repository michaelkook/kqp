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
#ifndef _KQP_CLEANUP_H_
#define _KQP_CLEANUP_H_

#include <boost/shared_ptr.hpp>

#include <kqp/decomposition.hpp>
#include <kqp/rank_selector.hpp>
#include <kqp/subset.hpp>

namespace kqp {

#   include <kqp/define_header_logger.hpp>
DEFINE_KQP_HLOGGER("kqp.cleaner");

    template<typename Scalar> class Cleaner {
    public:
        virtual ~Cleaner() {}
        
        //! Cleanup
        virtual void cleanup(Decomposition<Scalar> &) const {}        
    };
    
    
    /** @brief A series of cleaners.
    * 
    * @ingroup cleaning
    */
    template<typename Scalar>
    class CleanerList : public Cleaner<Scalar> {
    public:
        typedef boost::shared_ptr< Cleaner<Scalar> > Ptr;
        typedef boost::shared_ptr< const Cleaner<Scalar> > CPtr;
        
        virtual void cleanup(Decomposition<Scalar> &d) const override {
            KQP_HLOG_DEBUG_F("Before cleaning: pre-images=%d, rank=%d", %d.mX->size() %d.mD.rows());
            for(auto i = list.begin(); i != list.end(); ++i) {
                (*i)->cleanup(d);

                KQP_HLOG_DEBUG_F("After cleaner %s: pre-images=%d, rank=%d", %KQP_DEMANGLE(**i) %d.mX->size() %d.mD.rows());

                // Sanity check
                if (!d.check())
                    KQP_THROW_EXCEPTION_F(assertion_exception, "Decomposition in an invalid state (%d, %dx%d, %d) after cleaner %s", 
                        %d.mX->size() %d.mY.rows() %d.mY.cols() %d.mD.rows() % KQP_DEMANGLE(**i));
            }
        }
        
        void add(const Ptr &item) {
            list.push_back(item);
        } 
        
    private:
        std::vector< Ptr > list;
    };
    
    /** @brief Rank cleaner
    * 
    * @ingroup cleaning
    */
    template<typename Scalar>
    class CleanerRank : public Cleaner<Scalar> {
    public:
        KQP_SCALAR_TYPEDEFS(Scalar);
        
        //! Set the rank selector
        CleanerRank(const boost::shared_ptr< const Selector<Real> > &selector): selector(selector) {
        }
        
        virtual void cleanup(Decomposition<Scalar> &d) const override {
            DecompositionList<Real> list(d.mD);
            this->selector->selection(list);
            
            // Remove corresponding entries
            select_rows(list.getSelected(), d.mD, d.mD);
            
            // Case where mY is the identity matrix
            if (d.mY.getTypeId() == typeid(typename AltDense<Scalar>::IdentityType)) {
                d.mX = d.mX->subset(list.getSelected());
                d.mY.conservativeResize(list.getRank(), list.getRank());
            } else {
                select_columns(list.getSelected(), d.mY, d.mY);
            }
            
        }
    private:
        //! Eigen value selector
        boost::shared_ptr< const Selector<Real> > selector;
    };
    

}

#define KQP_CLEANUP_H_GEN(extern, type)  \
    extern template class kqp::Cleaner<type>; \
    extern template class kqp::CleanerList<type>; \
    extern template class kqp::CleanerRank<type>; 

#define KQP_SCALAR_GEN(type)  KQP_CLEANUP_H_GEN(extern, type)

#include <kqp/for_all_scalar_gen.h.inc>
#undef KQP_SCALAR_GEN

#endif


