//$Id: DSNTwoWayRange.hpp 65 2010-03-04 00:10:28Z  $
//------------------------------------------------------------------------------
//                         DSNTwoWayRange
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2010/03/08
//
/**
 * The DSN 2-way range core measurement model.
 */
//------------------------------------------------------------------------------


#ifndef DSNTwoWayRange_hpp
#define DSNTwoWayRange_hpp

#include "estimation_defs.hpp"
#include "TwoWayRange.hpp"


/**
 * Deep Space Network 2-Way Range Measurement Model
 */
class ESTIMATION_API DSNTwoWayRange: public TwoWayRange
{
public:
   DSNTwoWayRange(const std::string nomme = "");
   virtual ~DSNTwoWayRange();
   DSNTwoWayRange(const DSNTwoWayRange& usn);
   DSNTwoWayRange& operator=(const DSNTwoWayRange& usn);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();

   virtual const std::vector<RealArray>& CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id);

protected:
   /// Mapping between station ID and associated frequency
   std::map<std::string,Real>      freqMap;

   /// Inertial Range rate of the target spacecraft
   Real                 targetRangeRate;
   /// Range rate of the uplink
   Real                 uplinkRangeRate;
   /// Range rate of the downlink
   Real                 downlinkRangeRate;

   virtual bool                  Evaluate(bool withEvents = false);
   Real                          GetFrequencyFactor(Real frequency = 0.0);
};

#endif /* DSNTwoWayRange_hpp */
