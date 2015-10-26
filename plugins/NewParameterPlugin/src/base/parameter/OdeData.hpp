//$Id$
//------------------------------------------------------------------------------
//                                  OdeData
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
// contract, Task Order 28.
//
// Author: Darrel Conway, Thinking Systems, Inc
// Created: 2013/05/24  (Based on EnvData)
//
/**
 * Declares class which provides data from a force model, such as acceleration,
 * atmospheric density, current or average F10.7 value, relativistic correction,
 * etc.
 */
//------------------------------------------------------------------------------
#ifndef OdeData_hpp
#define OdeData_hpp

#include "newparameter_defs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"
#include "ODEModel.hpp"          // In case force lookup is needed
#include "PhysicalModel.hpp"     // Allow Parameters that use specific forces


class NEW_PARAMETER_API OdeData : public RefData
{
public:
   
   OdeData();
   OdeData(const OdeData &data);
   OdeData& operator= (const OdeData& right);
   virtual ~OdeData();
   
   Real GetOdeReal(const std::string &str);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   const static Real ODE_REAL_UNDEFINED;// = -9876543210.1234;
   
protected:
   
   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type);
   
   Spacecraft                    *mSpacecraft;
   SolarSystem                   *mSolarSystem;
   PhysicalModel                 *mModel;
   std::vector<PhysicalModel *>  *transients;
   
   enum
   {
      SPACECRAFT = 0,
      SOLAR_SYSTEM,
      PHYSICAL_MODEL,
      OdeDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[OdeDataObjectCount];
};
#endif // OdeData_hpp
