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
// Author: Rodger Abel
// Created: 1995/07/18 for GSS project.
// Modified:
//   2003/09/16 Linda Jun - Reused for GMAT project
//
/**
 * Defines ephemeris exceptions and constants and conversions between
 * Cartesian and Keplerian elements.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "AngleUtil.hpp"
#include "RealUtilities.hpp"     // for Mod(), ATan()

using namespace GmatMathUtil;

//---------------------------------
// public static functions
//---------------------------------

//------------------------------------------------------------------------------
// static Real AngleUtil::PutAngleInDegRange(Real angleInDeg, Real minAngleInDeg,
//                                           Real maxAngleInDeg)
//------------------------------------------------------------------------------
/*
 * @param <angleInDeg> input angle (deg)
 * @param <minAngleInDeg> input minimum angle (deg)
 * @param <maxAngleInDeg> input maximum angle (deg)
 *
 * @return angle in degrees in min/max range
 */
//------------------------------------------------------------------------------
Real AngleUtil::PutAngleInDegRange(Real angleInDeg, Real minAngleInDeg,
                                   Real maxAngleInDeg)
{
   Real angle = Mod(angleInDeg, TWO_PI_DEG);
   
   if (angle < minAngleInDeg)
      angle += TWO_PI_DEG;
   
   else if (angle > maxAngleInDeg)
      angle -= TWO_PI_DEG;
   
   return angle;
}

//------------------------------------------------------------------------------
// static Real AngleUtil::PutAngleInRadRange(Real angleInRad, Real minAngleInRad,
//                                           Real maxAngleInRad)
//------------------------------------------------------------------------------
/*
 * @param <angleInRad> input angle (rad)
 * @param <minAngleInRad> input minimum angle (rad)
 * @param <maxAngleInRad> input maximum angle (rad)
 *
 * @return angle in radians in min/max range
 */
//------------------------------------------------------------------------------
Real AngleUtil::PutAngleInRadRange(Real angleInRad, Real minAngleInRad,
                                   Real maxAngleInRad)
{
   Real angle = Mod(angleInRad, TWO_PI);
   
   if (angle < minAngleInRad)
      angle += TWO_PI;
   
   else if (angle > maxAngleInRad)
      angle -= TWO_PI;
   
   return angle;
}

//------------------------------------------------------------------------------
// static Real ComputePhaseAngleInDeg(Real axis1, Real axis2)
//------------------------------------------------------------------------------
/*
 * @param <axis1> first axis for phase angle computation
 * @param <axis2> second axis for phase angle computation
 *
 * @return phase angle in degrees between axis1 and axis2
 */
//------------------------------------------------------------------------------
Real AngleUtil::ComputePhaseAngleInDeg(Real axis1, Real axis2)
{
   Real phi;

   phi = ATan(axis1, axis2);

   if(phi < 0.0)
      phi += TWO_PI;

   phi = RadToDeg(phi);
   
   return phi;
}

//------------------------------------------------------------------------------
// static Real ComputePhaseAngleInRad(Real axis1, Real axis2)
//------------------------------------------------------------------------------
/*
 * @param <axis1> first axis for phase angle computation
 * @param <axis2> second axis for phase angle computation
 *
 * @return phase angle in radians between axis1 and axis2
 */
//------------------------------------------------------------------------------
Real AngleUtil::ComputePhaseAngleInRad(Real axis1, Real axis2)
{
   Real phi;

   phi = ATan(axis1, axis2);

   if(phi < 0.0)
      phi += TWO_PI;
   
   return phi;
}
