//$Header$
//------------------------------------------------------------------------------
//                                  PlanetData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/12/13
//
/**
 * Declares class which provides planet related data, such as HourAngle.
 */
//------------------------------------------------------------------------------
#ifndef PlanetData_hpp
#define PlanetData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"

class GMAT_API PlanetData : public RefData
{
public:
   
   PlanetData();
   PlanetData(const PlanetData &data);
   PlanetData& operator= (const PlanetData& right);
   virtual ~PlanetData();
   
   Real GetReal(const std::string &dataType);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   const static Real PLANET_REAL_UNDEFINED = -9876543210.1234;
   
protected:
   
   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type);

   std::string mCentralBodyName;
   
   Spacecraft *mSpacecraft;
   SolarSystem *mSolarSystem;
   CelestialBody *mCentralBody;
   CelestialBody *mOrigin;
   
   enum
   {
      SPACECRAFT = 0,
      SOLAR_SYSTEM,
      CELESTIAL_BODY, //loj: 4/7/05 Added
      PlanetDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[PlanetDataObjectCount];
};
#endif // PlanetData_hpp

   
