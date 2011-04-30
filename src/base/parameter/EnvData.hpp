//$Id$
//------------------------------------------------------------------------------
//                                  EnvData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/12/07
//
/**
 * Declares class which provides environmental data, such as atmospheric density,
 * solar flux, etc.
 */
//------------------------------------------------------------------------------
#ifndef EnvData_hpp
#define EnvData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"

class GMAT_API EnvData : public RefData
{
public:
   
   EnvData();
   EnvData(const EnvData &data);
   EnvData& operator= (const EnvData& right);
   virtual ~EnvData();
   
   Real GetEnvReal(const std::string &str);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   const static Real ENV_REAL_UNDEFINED;// = -9876543210.1234;
   
protected:
   
   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type);
   
   Spacecraft *mSpacecraft;
   SolarSystem *mSolarSystem;
   SpacePoint *mOrigin;
   
   enum
   {
      SPACECRAFT = 0,
      SOLAR_SYSTEM,
      SPACE_POINT, //loj: 4/11/05 Changed CELESTIAL_BODY to SPACE_POINT
      EnvDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[EnvDataObjectCount];
};
#endif // EnvData_hpp

   
