//$Id$
//------------------------------------------------------------------------------
//                              RealUtilities
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Created: 1995/07/21 for GSS project
// Modified: 2003/09/15 Linda Jun - Put functions under namespace GmatRealUtil.
//                      Added NearestInt().
//
/**
 * This file provides measurement conversion constatns and Math Utilities that
 * are not provided in the C++ Library or provides call-throughs to the
 * routines of the C++ (C) math.h
 */
//------------------------------------------------------------------------------
#ifndef RealUtilities_hpp
#define RealUtilities_hpp

#include "BaseException.hpp"
#include "GmatConstants.hpp"

struct GMAT_API RealUtilitiesExceptions
{
   class GMAT_API ArgumentError : public BaseException
   {public : ArgumentError(const std::string& message = "")
       : BaseException("", message) {};  };
   
   class GMAT_API IllegalTime   : public BaseException
   {public : IllegalTime(const std::string& message = "")
       : BaseException("", message) {};  };
};

namespace GmatMathUtil
{
   Integer GMAT_API Abs(Integer theNumber);
   Real    GMAT_API Abs(Real theNumber);
   Real    GMAT_API NearestInt(Real theNumber);
   Real    GMAT_API Round(Real theNumber);
   Real    GMAT_API Floor(Real theNumber);
   Real    GMAT_API Fix(Real theNumber);
   Real    GMAT_API Ceiling(Real theNumber);
   Real    GMAT_API Mod(Real left, Real right);
   Real    GMAT_API Rem(Real left, Real right);
   void    GMAT_API Quotient(Real top, Real bottom, Integer &result);
   void    GMAT_API Quotient(Real top, Real bottom, Real &result);
   Real    GMAT_API Min(Real left, Real right);
   Real    GMAT_API Max(Real left, Real right);
   bool    GMAT_API IsPositive(Real theNumber);
   bool    GMAT_API IsNegative(Real theNumber);
   bool    GMAT_API IsNonNegative(Real theNumber);
   bool    GMAT_API IsZero(Real theNumber, Real accuracy=GmatRealConstants::REAL_EPSILON);
   bool    GMAT_API IsEqual(Real left, Real right, 
                    Real accuracy = GmatRealConstants::REAL_EPSILON);
   Integer GMAT_API SignOf(Real theNumber);
   bool    GMAT_API IsOdd(Integer theNumber);
   bool    GMAT_API IsEven(Integer theNumber);
   
   Real    GMAT_API Rad(Real angleInDeg, bool modBy2Pi = false);
   Real    GMAT_API Deg(Real angleInRad, bool modBy360 = false);
   Real    GMAT_API DegToRad(Real deg, bool modBy2Pi = false);
   Real    GMAT_API RadToDeg(Real rad, bool modBy360 = false);
   Real    GMAT_API ArcsecToDeg(Real asec,bool modBy360 = false);
   Real    GMAT_API ArcsecToRad(Real asec,bool modBy2Pi = false);
   
   Real    GMAT_API Sin(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMAT_API SinXOverX(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMAT_API Cos(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMAT_API Tan(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);

   Real    GMAT_API Cosh(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMAT_API Sinh(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMAT_API Tanh(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   
   Real    GMAT_API ASin(Real x, Real tol=GmatRealConstants::REAL_TOL, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMAT_API ACos(Real x, Real tol=GmatRealConstants::REAL_TOL, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMAT_API ATan(Real y, Real x=1.0, Real cycleInRad=GmatMathConstants::TWO_PI);
   
   Real    GMAT_API ATan2(Real y, Real x=1.0, Real cycleInRad=GmatMathConstants::TWO_PI);
   
   Real    GMAT_API ASinh(Real x, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMAT_API ACosh(Real x, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMAT_API ATanh(Real x, Real cycleInRad=GmatMathConstants::TWO_PI);
   
   Real    GMAT_API Ln(Real x);             // Natural (base e) Logarithm of x
   Real    GMAT_API Log(Real x);            // Natural (base e) Logarithm of x
   Real    GMAT_API Log10(Real x);          // Base 10 Logarithm of x
   Real    GMAT_API Log(Real x, Real base); // Base <base> Logarithm of x
   Real    GMAT_API Log(Real x, Integer base);
   
   void    GMAT_API SetSeed(Integer initialSeed1, Integer initialSeed2); 
   void    GMAT_API GetSeed(Integer& initialSeed1, Integer& initialSeed2);
   //loj:void    GMAT_API SetSeedByClock();
   Real    GMAT_API Number(Real lowerBound=0.0, Real upperBound=1.0); 
   Real    GMAT_API GaussianNumber(Real mu= 0.0, Real sigma=1.0); 
   Real    GMAT_API Ran();
   void    GMAT_API SetRanKey(Real k=0.0); 
   //loj:Real    GMAT_API Cot(Real x);
   Real    GMAT_API Cbrt(Real x);
   Real    GMAT_API Sqrt(Real x);
   
   Real    GMAT_API Exp(Real x);            // Raises e  to the x power(e^x)
   Real    GMAT_API Exp10(Real x);          // Raises 10 to the x power(10^x)
   Real    GMAT_API Pow(Real x, Real y);    // Raises x to the y power(x^y)
   Real    GMAT_API Pow(Real x, Integer y); // Raises x to the y power(x^y)
   
   bool    GMAT_API IsNaN(Real x);
   bool    GMAT_API IsInf(Real x);
}

// This inline doesn't work with MSVC++ compiler
#ifndef _MSC_VER
#include <cmath>
//------------------------------------------------------------------------------
// bool IsNaN(Real x)
//------------------------------------------------------------------------------
/**
 * Tests if input value is not a number.
 */
//------------------------------------------------------------------------------
inline bool GmatMathUtil::IsNaN(Real x)
{
   return std::isnan(x);
}

//------------------------------------------------------------------------------
// bool IsInf(Real x)
//------------------------------------------------------------------------------
/**
 * Tests if input value is a infinite number.
 */
//------------------------------------------------------------------------------
inline bool GmatMathUtil::IsInf(Real x)
{
   return std::isinf(x);
}
#endif

#endif // RealUtilities_hpp
