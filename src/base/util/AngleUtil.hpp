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
// Created: 2003/09/16
//
/**
 * Declares AngleUtil which provides various angle computation.
 */
//------------------------------------------------------------------------------
#ifndef AngleUtil_hpp
#define AngleUtil_hpp

#include "gmatdefs.hpp"
#include "Rvector3.hpp"

class AngleUtil
{
public:
   
   static const Real TWO_PI_DEG;
   static const Real PI_DEG;
   
   // static functions
   static Real PutAngleInDegRange(Real angleInDeg, Real minAngleInDeg,
                                  Real maxAngleInDeg);
   static Real PutAngleInRadRange(Real angleInRad, Real minAngleInRad,
                                  Real maxAngleInRad);
   static Real ComputePhaseAngleInDeg(Real axis1, Real axis2);
   static Real ComputePhaseAngleInRad(Real axis1, Real axis2);
   static Real ComputeAngleInDeg(const Rvector3 &vecA, const Rvector3 &vecB,
                                 Real tol = 0.99);
};

#endif // AngleUtil_hpp
