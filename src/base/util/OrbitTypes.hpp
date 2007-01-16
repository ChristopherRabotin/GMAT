//$Header$
//------------------------------------------------------------------------------
//                              OrbitTypes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun (NASA/GSFV)
// Created: 2006/12/28
//
/**
 * This file provides constants used in orbital element conversion between
 * Cartesion, Keplerian, ModKeplerian, etc.
 */
//------------------------------------------------------------------------------
#ifndef OrbitTypes_hpp
#define OrbitTypes_hpp

#include "BaseException.hpp"
#include "RealTypes.hpp"


namespace GmatOrbit
{
   // Undefined
   const static Real ORBIT_REAL_UNDEFINED = -9876543210.1234;

   // Tolerance
   const static Real KEP_TOL = 1.0e-11;
   const static Real KEP_ANOMALY_TOL = 1.0e-12;
   const static Real KEP_ZERO_TOL = 1.0e-30;
   
}
#endif // OrbitTypes_hpp
