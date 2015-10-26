//$Id$
//------------------------------------------------------------------------------
//                                  BurnData
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/09
//
/**
 * Declares Burn related data class.
 */
//------------------------------------------------------------------------------
#ifndef BurnData_hpp
#define BurnData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "ImpulsiveBurn.hpp"

class GMAT_API BurnData : public RefData
{
public:

   BurnData(const std::string &name = "", const std::string &typeName = "",
            const Gmat::ObjectType paramOwnerType = Gmat::IMPULSIVE_BURN);
   BurnData(const BurnData &data);
   BurnData& operator= (const BurnData& right);
   virtual ~BurnData();
   
   Real GetReal(Integer item);
   void SetReal(Integer item, Real rval);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   const static Real BURN_REAL_UNDEFINED;
   const static Real BURN_TOL;

protected:

   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type);
   
   SolarSystem* GetSolarSystem();
   CoordinateSystem* GetInternalCoordSys();
   
   void SetInternalCoordSys(CoordinateSystem *cs);
   
   Spacecraft *mSpacecraft;
   ImpulsiveBurn *mImpBurn;
   SolarSystem *mSolarSystem;
   SpacePoint *mOrigin;
   CoordinateSystem *mInternalCoordSystem;
   CoordinateSystem *mOutCoordSystem;
   
   CoordinateConverter mCoordConverter;
   
   bool mIsParamCSDep;
   bool firstTimeEpochWarning;
   bool firstTimeHasntFiredWarning;

   enum {ELEMENT1, ELEMENT2, ELEMENT3};
   
   enum
   {
      IMPULSIVE_BURN,
      SOLAR_SYSTEM,
      COORD_SYSTEM,
      SPACECRAFT,
      BurnDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[BurnDataObjectCount];
};
#endif // BurnData_hpp

