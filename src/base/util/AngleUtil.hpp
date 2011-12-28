//$Id$
//------------------------------------------------------------------------------
//                             AngleUtil
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

class GMAT_API AngleUtil
{
public:
   
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
