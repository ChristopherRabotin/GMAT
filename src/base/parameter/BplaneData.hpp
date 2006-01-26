//$Header$
//------------------------------------------------------------------------------
//                                  BplaneData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/01/09
//
/**
 * Declares Orbit related data class.
 */
//------------------------------------------------------------------------------
#ifndef BplaneData_hpp
#define BplaneData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"

class GMAT_API BplaneData : public RefData
{
public:
   
   BplaneData();
   BplaneData(const BplaneData &data);
   BplaneData& operator= (const BplaneData& right);
   virtual ~BplaneData();
   
   Real GetBplaneReal(Integer item);
   Real GetBplaneReal(const std::string &str);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
protected:

   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type);
   
   SolarSystem* GetSolarSystem();
   CoordinateSystem* GetInternalCoordSys();
   
   void SetInternalCoordSys(CoordinateSystem *cs);
   Rvector6 GetCartState();
   
   Rvector6 mCartState;
   
   Real mCartEpoch;
   Real mGravConst;
   
   Spacecraft *mSpacecraft;
   SolarSystem *mSolarSystem;
   SpacePoint *mOrigin;
   CoordinateSystem *mInternalCoordSystem;
   CoordinateSystem *mOutCoordSystem;
   
   CoordinateConverter mCoordConverter;

   enum {B_DOT_T, B_DOT_R};
   
   enum
   {
      SPACECRAFT = 0,
      SOLAR_SYSTEM,
      COORD_SYSTEM,
      SPACE_POINT,
      BplaneDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[BplaneDataObjectCount];
};
#endif // BplaneData_hpp

   
