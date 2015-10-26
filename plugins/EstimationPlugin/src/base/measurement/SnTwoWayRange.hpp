//$Id: SnTwoWayRange.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         SnTwoWayRange
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
 * The space network 2-way range measurement model
 */
//------------------------------------------------------------------------------


#ifndef SnTwoWayRange_hpp
#define SnTwoWayRange_hpp

#include "estimation_defs.hpp"
#include "TwoWayRange.hpp"

/**
 * This class implements the Space Network range measurement calculations.
 *
 * The class is not yet implemented.
 */
class ESTIMATION_API SnTwoWayRange: public TwoWayRange
{
public:
   SnTwoWayRange(const std::string &nomme = "");
   virtual ~SnTwoWayRange();
   SnTwoWayRange(const SnTwoWayRange& sn);
   SnTwoWayRange& operator=(const SnTwoWayRange& sn);

   virtual GmatBase* Clone() const;

   virtual const std::vector<RealArray>& CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id);


protected:
   virtual bool                    Evaluate(bool withEvents = false);

};

#endif /* SnTwoWayRange_hpp */
