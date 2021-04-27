//$Id: MeasurementFactory.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         MeasurementFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
// Created: 2009/07/09
//
/**
 * Factory class used to create core measurement objects
 */
//------------------------------------------------------------------------------


#ifndef MeasurementFactory_hpp
#define MeasurementFactory_hpp

#include "geometricmeasurement_defs.hpp"
#include "Factory.hpp"
#include "CoreMeasurement.hpp"


class GEOMETRICMEAS_API MeasurementFactory : public Factory
{
public:
   MeasurementFactory();
   virtual ~MeasurementFactory();
   MeasurementFactory(StringArray createList);
   MeasurementFactory(const MeasurementFactory& fact);
   MeasurementFactory& operator= (const MeasurementFactory& fact);

   GmatBase* CreateObject(const std::string &ofType,
         const std::string &withName = "");
   CoreMeasurement *CreateMeasurement(const std::string &ofType,
         const std::string &withName = "");
};

#endif /* MeasurementFactory_hpp */
