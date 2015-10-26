//$Id$
//------------------------------------------------------------------------------
//                                  SpacecraftData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun
// Created: 2009.03.20
//
/**
 * Declares Spacecraft Spacecraft related data class.
 */
//------------------------------------------------------------------------------
#ifndef SpacecraftData_hpp
#define SpacecraftData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"

class GMAT_API SpacecraftData : public RefData
{
public:

   SpacecraftData(const std::string &name = "");
   SpacecraftData(const SpacecraftData &data);
   SpacecraftData& operator= (const SpacecraftData& right);
   virtual ~SpacecraftData();
   
   Real GetReal(Integer item);
   Real SetReal(Integer item, Real val);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   const static Real BALLISTIC_REAL_UNDEFINED;
   
protected:

   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type);
   
   Spacecraft *mSpacecraft;
   
   enum 
   {
      DRY_MASS,
      DRAG_COEFF,
      REFLECT_COEFF,
      DRAG_AREA,
      SRP_AREA,
      TOTAL_MASS,
      
      // for Spacecraft owned FuelTank
      FUEL_MASS,
      PRESSURE,
      TEMPERATURE,
      VOLUME,
      FUEL_DENSITY,
      REF_TEMPERATURE,
      
      // for Spacecraft owned Thruster
      DUTY_CYCLE,
      THRUSTER_SCALE_FACTOR,
      GRAVITATIONAL_ACCEL,
      C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, C16,
      K1, K2, K3, K4, K5, K6, K7, K8, K9, K10, K11, K12, K13, K14, K15, K16,
      THRUST_DIRECTION1, THRUST_DIRECTION2, THRUST_DIRECTION3,

      // for Spacecraft-owned PowerSystem
      TOTAL_POWER_AVAILABLE,
      REQUIRED_BUS_POWER,
      THRUST_POWER_AVAILABLE,
   };
   
   enum
   {
      SPACECRAFT = 0,
      SpacecraftDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[SpacecraftDataObjectCount];
   
private:
   
   Real GetOwnedObjectProperty(Gmat::ObjectType objType, const std::string &name);
   Real SetOwnedObjectProperty(Gmat::ObjectType objType, const std::string &name,
                               Real val);
   
};

#endif /*SpacecraftData_hpp*/
