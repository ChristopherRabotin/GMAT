//$Header$
//------------------------------------------------------------------------------
//                             AngleUtil
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/04/19
// Modified:
//   2004/04/19 Linda Jun - Modified Swnigby code to reuse for GMAT project
//
/**
 * Declares conversion functions between Cartesian and Keplerian elements.
 */
//------------------------------------------------------------------------------
#ifndef AngleUtil_hpp
#define AngleUtil_hpp

#include "gmatdefs.hpp"

class AngleUtil
{
public:
   
   static const Real TWO_PI_DEG = 360.0;
   static const Real PI_DEG = 180.0;
   
   // static functions
   static Real PutAngleInDegRange(Real angleInDeg, Real minAngleInDeg,
                                  Real maxAngleInDeg);
   static Real PutAngleInRadRange(Real angleInRad, Real minAngleInRad,
                                  Real maxAngleInRad);
   static Real ComputePhaseAngleInDeg(Real axis1, Real axis2);
   static Real ComputePhaseAngleInRad(Real axis1, Real axis2);
   
};

#endif // AngleUtil_hpp
