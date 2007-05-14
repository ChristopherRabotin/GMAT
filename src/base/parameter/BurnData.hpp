//$Header$
//------------------------------------------------------------------------------
//                                  BurnData
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
 * Declares ImpulsiveBurn related data class.
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

   BurnData();
   BurnData(const BurnData &data);
   BurnData& operator= (const BurnData& right);
   virtual ~BurnData();
   
   Real GetBurnReal(Integer item);
   Real GetBurnReal(const std::string &str);

   void SetBurnReal(Integer item, Real rval);
   
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

   
