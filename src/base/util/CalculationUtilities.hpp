//$Id:  $
//------------------------------------------------------------------------------
//                              CalculationUtilities
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Created: 2011.08.12
//
/**
 * This namespace provides Calculation Utilities that can be used to compute the
 * GMAT Calculation Objects.
 *
 * NOTE: the input state must be in the coordinate system desired - no
 *       conversions are done here
 */
//------------------------------------------------------------------------------
#ifndef CalculationUtilities_hpp
#define CalculationUtilities_hpp

#include "BaseException.hpp"
#include "GmatConstants.hpp"
#include "Rvector6.hpp"


namespace GmatCalcUtil
{
   Real GMAT_API  CalculateBPlaneData(const std::string &item, const Rvector6 &state, const Real originMu);
   Real GMAT_API  CalculateAngularData(const std::string &item, const Rvector6 &state,
                                       const Real &originMu, const Rvector3 &originToSunUnit);
   Real GMAT_API  CalculateKeplerianData(const std::string &item, const Rvector6 &state, const Real originMu);
   Real GMAT_API  CalculatePlanetData(const std::string &item, const Rvector6 &state,
                                      const Real originRadius, const Real originFlattening, const Real originHourAngle);
}

#endif /* CalculationUtilities_hpp */
