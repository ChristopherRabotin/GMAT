//$Header$
//------------------------------------------------------------------------------
//                                RealTypes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/15
//
/**
 * Ddeclarations for real constants and types.
 */
//------------------------------------------------------------------------------
#ifndef RealTypes_hpp
#define RealTypes_hpp

#include "gmatdefs.hpp"
#include <limits>

typedef Real Radians;

namespace GmatRealConst
{

   const Integer SHORT_REAL_RADIX = __glibcpp_f64_radix;
   //loj: Couldn't find coressponding MANT_DIG, do this later
   //const Integer REAL_MANT_DIG;
   const Real    REAL_EPSILON     = __glibcpp_f64_epsilon;
   const Integer REAL_DIG         = __glibcpp_f64_digits;
   const Integer REAL_MIN_EXP     = __glibcpp_f64_min_exponent;
   const Real    REAL_MIN         = __glibcpp_f64_min;
   const Integer REAL_MIN_10_EXP  = __glibcpp_f64_min_exponent10;
   const Integer REAL_MAX_EXP     = __glibcpp_f64_max_exponent;
   const Real    REAL_MAX         = __glibcpp_f64_max;
   const Integer REAL_MAX_10_EXP  = __glibcpp_f64_max_exponent10;
   
   const Real    REAL_TOL         = REAL_EPSILON;
};

#endif // RealTypes_hpp
