//$Id: USNTwoWayRange.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         USNTwoWayRange
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/18
//
/**
 * The USN 2-way range core measurement model.
 */
//------------------------------------------------------------------------------


#ifndef USNTwoWayRange_hpp
#define USNTwoWayRange_hpp

#include "estimation_defs.hpp"
#include "TwoWayRange.hpp"


/**
 * Universal Space Network 2-Way Range Measurement Model
 */
class ESTIMATION_API USNTwoWayRange: public TwoWayRange
{
public:
   USNTwoWayRange(const std::string nomme = "");
   virtual ~USNTwoWayRange();
   USNTwoWayRange(const USNTwoWayRange& usn);
   USNTwoWayRange& operator=(const USNTwoWayRange& usn);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();

   virtual const std::vector<RealArray>& CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id);

protected:
   /// Inertial Range rate of the target spacecraft
   Real                 targetRangeRate;
   /// Range rate of the uplink
   Real                 uplinkRangeRate;
   /// Range rate of the downlink
   Real                 downlinkRangeRate;

   virtual bool         Evaluate(bool withEvents = false);
};

#endif /* USNTwoWayRange_hpp */
