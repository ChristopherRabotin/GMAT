//$Header$
//------------------------------------------------------------------------------
//                              RealUtilities
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: M. Weippert, Chu-chi Li, E. Corderman, W. Shoan
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
#include "RealTypes.hpp"

struct RealUtilitiesExceptions
{
    class ArgumentError : public BaseException
          {public : ArgumentError(const std::string& message =
          "Real Utilities error : argument error.")
          : BaseException(message) {};  };
    class IllegalTime   : public BaseException
          {public : IllegalTime(const std::string& message =
          "Real Utilities error : illegal time.")
          : BaseException(message) {};  };
};

namespace GmatMathUtil
{
   //Math constants
   static const Real PI = 3.14159265358979323846264338327950288419716939937511;
   static const Real TWO_PI = 6.28318530717958647692528676655900576839433879875022;
   static const Real PI_OVER_TWO = 1.57079632679489661923132169163975144209858469968756;
   static const Real E = 2.71828182845904523536028747135266249775724709369996;
   
   //Angle conversion
   static const Real RAD_PER_DEG =
      3.14159265358979323846264338327950288419716939937511 / 180.0;
   static const Real DEG_PER_RAD=
      180.0 / 3.14159265358979323846264338327950288419716939937511;
   
   //Mass (kilogram)
   static const Real LBM_TO_KG = 0.45359237;
   static const Real SLUG_TO_KG = 14.59390294;

   //Length (metre)
   static const Real INCH_TO_M = 0.0254;
   static const Real FOOT_TO_M = 0.3048;
   static const Real STATUTE_MILE_TO_M = 1609.344;
   static const Real M_TO_KM = 0.001;
   static const Real KM_TO_M = 1000.0;

   Integer Abs(Integer theNumber);
   Real    Abs(Real theNumber);
   Real    NearestInt(Real theNumber);
   Integer Round(Real theNumber);
   Integer Floor(Real theNumber);
   Integer Ceiling(Real theNumber);
   Real    Mod (Real left, Real right);
   Real    Rem (Real left, Real right);
   void    Quotient (Real top, Real bottom, Integer &result);
   void    Quotient (Real top, Real bottom, Real &result);
   Real    Min (Real left, Real right);
   Real    Max (Real left, Real right);
   bool    IsPositive(Real theNumber);
   bool    IsNegative(Real theNumber);
   bool    IsNonNegative(Real theNumber);
   bool    IsZero (Real theNumber, Real accuracy=GmatRealConst::REAL_EPSILON);
   bool    IsEqual (Real left, Real right, 
                    Real accuracy = GmatRealConst::REAL_EPSILON);
   Integer SignOf (Real theNumber); 
   Real    Rad(Real angleInDeg, bool modBy2Pi = false);
   Real    Deg(Real angleInRad, bool modBy360 = false);
   Real    Sin (Real angleInRad, Real cycleInRad=TWO_PI); 
   //loj:Real    SinXOverX (Real angleInRad, Real cycleInRad=TWO_PI); 
   Real    Cos (Real angleInRad, Real cycleInRad=TWO_PI); 
   Real    Tan (Real angleInRad, Real cycleInRad=TWO_PI); 
   Real    ASin (Real x, Real cycleInRad=TWO_PI); 
   Real    ACos (Real x, Real cycleInRad=TWO_PI); 
   Real    ATan (Real y, Real x=1.0, Real cycleInRad=TWO_PI);  
   Real    Log(Real x,Real base=10.0); 
   Real    Log (Real x, Integer base);
   void    SetSeed(Integer initialSeed1, Integer initialSeed2); 
   void    GetSeed(Integer& initialSeed1, Integer& initialSeed2);
   //loj:void    SetSeedByClock();
   Real    Number (Real lowerBound=0.0, Real upperBound=1.0); 
   Real    GaussianNumber(Real mu= 0.0, Real sigma=1.0); 
   Real    Ran();
   void    SetRanKey(Real k=0.0); 
   //loj:Real    Cot(Real x);
   Real    Cbrt(Real x);
   Real    Sqrt(Real x);
   Real    Pow(Real x, Real y);
}
#endif // RealUtilities_hpp
