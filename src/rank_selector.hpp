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

#ifndef __KQP_RANK_SELECTOR_H__
#define __KQP_RANK_SELECTOR_H__

#include <Eigen/Core>
#include "kqp.hpp"

namespace kqp {
    /**
     * A list of eigenvalues that can be edited
     */
    class EigenList {
    public:
        virtual ~EigenList();
        
        /**
         * Select an eigenvalue
         */
        virtual double get(size_t i) const = 0;
        
        /**
         * Remove this eigenvalue from the selection
         */
        virtual void remove(size_t i) = 0;
        
        /**
         * The original number of eigenvalues
         */
        virtual size_t size() const = 0;
        
        /**
         * The current number of selected
         */
        virtual size_t getRank() const = 0;
        
        /**
         * Check if an eigenvalue is currently selected or not
         */
        virtual bool isSelected(size_t i) const = 0;
    };
    
    template<typename Scalar>
    class DecompositionList : public EigenList {
    public:
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> Matrix;
        typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> Vector;
        
        
        DecompositionList(const Vector &eigenvalues) 
        : eigenvalues(eigenvalues), selected(eigenvalues.size(), true), rank(eigenvalues.size()) {
            
        }
        
        /**
         * Select an eigenvalue
         */
        virtual double get(size_t i) const {
            return eigenvalues[i];
        }
        
        /**
         * Remove this eigenvalue from the selection
         */
        virtual void remove(size_t i) {
            if (selected[i]) {
                selected[i] = false;
                rank--;
            }
        }
        
        /**
         * The original number of eigenvalues
         */
        virtual size_t size() const {
            return eigenvalues.size();
        }
        
        /**
         * The current number of selected
         */
        virtual size_t getRank() const {
            return rank;
        }
        
        /**
         * Check if an eigenvalue is currently selected or not
         */
        virtual bool isSelected(size_t i) const { 
            return selected[i];
        }
        
        const std::vector<bool> &getSelected() const {
            return selected;
        }
        
    private:
        
        Vector eigenvalues;
        std::vector<bool> selected;
        size_t rank;
    };
    
        
    /**
     * Gets an eigenlist and removes whatever eigenvalues it does not like
     */
    class Selector {
    public:
        /**
         * @param eigenValues
         *            The ordered list of eigenvalues
         */
        virtual void selection(EigenList& eigenValues) const = 0;
        
    };
    
    /**
     * Chain selection
     */
    class ChainSelector : public Selector {
    public:
        ChainSelector();
        void add(const ChainSelector &);
        virtual void selection(EigenList& eigenValues) const;
    };
    
    /**
     * Minimum relative eigenvalue
     */
    template<typename Scalar>
    class MinimumSelector {
    public:
        MinimumSelector();
        virtual void selection(EigenList& eigenValues) const;
    };
}

#endif
