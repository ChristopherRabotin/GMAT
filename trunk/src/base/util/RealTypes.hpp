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
 * Declarations for real constants and types.
 */
//------------------------------------------------------------------------------
#ifndef RealTypes_hpp
#define RealTypes_hpp

#include "gmatdefs.hpp"
#include <limits>

typedef Real Radians;

namespace GmatRealConst
{
   const Real    REAL_TOL = 1.0e-16; // Changed from 1.0e-15
   
   #ifdef __glibcpp_f64_radix
      const Integer SHORT_REAL_RADIX = __glibcpp_f64_radix;
   #else
      const Integer SHORT_REAL_RADIX = 2;
   #endif
      
   //loj: Couldn't find coressponding MANT_DIG, do this later
   //const Integer REAL_MANT_DIG;
   #ifdef __glibcpp_f64_epsilon
      const Real    REAL_EPSILON     = __glibcpp_f64_epsilon;
   #else
      const Real    REAL_EPSILON     = 2.2204460492503131e-16;
   #endif

   #ifdef __glibcpp_f64_digits
      const Integer REAL_DIG         = __glibcpp_f64_digits;
   #else
      const Integer REAL_DIG         = 53;
   #endif

   #ifdef __glibcpp_f64_min_exponent
      const Integer REAL_MIN_EXP     = __glibcpp_f64_min_exponent;
   #else
      const Integer REAL_MIN_EXP     = -1021;
   #endif

   #ifdef __glibcpp_f64_min
      const Real    REAL_MIN         = __glibcpp_f64_min;
   #else
      const Real    REAL_MIN         = 2.2250738585072014e-308;
   #endif

   #ifdef __glibcpp_f64_min_exponent10
      const Integer REAL_MIN_10_EXP  = __glibcpp_f64_min_exponent10;
   #else
      const Integer REAL_MIN_10_EXP  = -307;
   #endif

   #ifdef __glibcpp_f64_max_exponent
      const Integer REAL_MAX_EXP     = __glibcpp_f64_max_exponent;
   #else
      const Integer REAL_MAX_EXP     = 1024;
   #endif

   #ifdef __glibcpp_f64_max
      const Real    REAL_MAX         = __glibcpp_f64_max;
   #else
      const Real    REAL_MAX         = 1.7976931348623157e+308;
   #endif

   #ifdef __glibcpp_f64_max_exponent10
      const Integer REAL_MAX_10_EXP  = __glibcpp_f64_max_exponent10;
   #else
      const Integer REAL_MAX_10_EXP  = 308;
   #endif
      
};

#endif // RealTypes_hpp
