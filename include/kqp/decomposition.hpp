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
#ifndef _KQP_DECOMPOSITION_H
#define _KQP_DECOMPOSITION_H

#include <kqp/feature_matrix.hpp>

namespace kqp {
    
    //! An "EVD" decomposition
    template<typename FMatrix>
    struct Decomposition {
        KQP_FMATRIX_TYPES(FMatrix);        
        
        //! The feature matrix
        FMatrix mX;
        
        //! The linear combination matrix
        ScalarAltMatrix mY;
        
        //! The diagonal matrix
        RealAltVector mD;
        
        //! If this is a real decomposition
        bool orthonormal;
        
        //! Default constructor (sets orthonormal to true)
        Decomposition() : orthonormal(true) {}

        //! Full constructor
        Decomposition(const FMatrix &mX, const ScalarAltMatrix &mY, const RealAltVector &mD, bool orthonormal) 
            : mX(mX), mY(mY), mD(mD), orthonormal(orthonormal) {}
};   
    

    
}
#endif
