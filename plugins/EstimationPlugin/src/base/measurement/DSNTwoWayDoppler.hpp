//$Id: DSNTwoWayDoppler.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         DSNTwoWayDoppler
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS
// Task 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 27, 2010
//
/**
 * Measurement model class for Doppler measurements made by the deep space
 * network
 */
//------------------------------------------------------------------------------


#ifndef DSNTwoWayDoppler_hpp
#define DSNTwoWayDoppler_hpp

#include "estimation_defs.hpp"
#include "AveragedDoppler.hpp"

/**
 * Class defining 2-way Doppler measurements from the Deep Space Network.
 */
class ESTIMATION_API DSNTwoWayDoppler: public AveragedDoppler
{
public:
   DSNTwoWayDoppler(const std::string &withName = "");
   virtual ~DSNTwoWayDoppler();
   DSNTwoWayDoppler(const DSNTwoWayDoppler& dd);
   DSNTwoWayDoppler& operator=(const DSNTwoWayDoppler& dd);

   virtual GmatBase* Clone() const;

   virtual const std::vector<RealArray>& CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id);

protected:
   /// Uplink range rate for the 2 paths
   Real uplinkRangeRate[2];
   /// Downlink range rate for the 2 paths
   Real downlinkRangeRate[2];

   virtual bool Evaluate(bool);
   virtual void SetHardwareDelays(bool loadEvents = true);

   Real GetTurnAroundRatio(Integer freqBand);
};

#endif /* DSNTwoWayDoppler_hpp */
