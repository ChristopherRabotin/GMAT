//$Header$
//------------------------------------------------------------------------------
//                              AttitudeUtil
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/09/27
//
/**
 * This file provides coversion between different attitude representation.
 * (The code is reused from GSS.)
 */
//------------------------------------------------------------------------------
#ifndef AttitudeUtil_hpp
#define AttitudeUtil_hpp

#include "BaseException.hpp"
#include "RealTypes.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"

namespace GmatAttUtil
{
   Rvector3 ToEulerAngles(Real rotAngle, const Rvector3 &rotAxis,
                          Integer seq1, Integer seq2, Integer seq3);
   Rvector3 ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
                          Integer seq2, Integer seq3);
   Rmatrix33 ToCosineMatrix(const Rvector3 &qVec, Real qScalar);
   Rmatrix33 ToCosineMatrix(Real rotAngle, const Rvector3 &rotAxis);
}

#endif // AttitudeUtil_hpp
