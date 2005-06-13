//$Header$
//------------------------------------------------------------------------------
//                                  OrbitData
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
#ifndef OrbitData_hpp
#define OrbitData_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "RefData.hpp"
#include "Rvector6.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"

class GMAT_API OrbitData : public RefData
{
public:

   OrbitData();
   OrbitData(const OrbitData &data);
   OrbitData& operator= (const OrbitData& right);
   virtual ~OrbitData();
   
   Rvector6 GetCartState();
   Rvector6 GetKepState();
   Rvector6 GetModKepState();
   Rvector6 GetSphRaDecState();
   Rvector6 GetSphAzFpaState();
   
   Real GetCartReal(const std::string &str);
   Real GetKepReal(const std::string &str);
   Real GetOtherKepReal(const std::string &str);
   Real GetSphRaDecReal(const std::string &str);
   Real GetSphAzFpaReal(const std::string &str);
   Real GetAngularReal(const std::string &str);
   Real GetOtherAngleReal(const std::string &str);
   
   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
   
   const static Real ORBIT_REAL_UNDEFINED = -9876543210.1234;
   const static Real ORBIT_TOL = 1.0e-10;

protected:

   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type);
   
   SolarSystem* GetSolarSystem();
   CoordinateSystem* GetInternalCoordSys();
   
   void SetInternalCoordSys(CoordinateSystem *cs);
   Rvector6 GetRelativeCartState(SpacePoint *origin);
   Real GetPositionMagnitude(SpacePoint *origin);
   
   Rvector6 mCartState;
   Rvector6 mKepState;
   Rvector6 mModKepState;
   Rvector6 mSphRaDecState;
   Rvector6 mSphAzFpaState;
   
   Real mMA;
   Real mCartEpoch;
   Real mGravConst;
   
   Spacecraft *mSpacecraft;
   SolarSystem *mSolarSystem;
   SpacePoint *mScOrigin;
   SpacePoint *mOrigin;
   CoordinateSystem *mInternalCoordSystem;
   CoordinateSystem *mOutCoordSystem;
   
   CoordinateConverter mCoordConverter;
   
   enum {PX, PY, PZ, VX, VY, VZ};
   enum {SMA, ECC, INC, RAAN, AOP, TA};
   enum {RD_RMAG, RD_RRA, RD_RDEC, RD_VMAG, RD_RAV, RD_DECV};
   enum {AF_RMAG, AF_RRA, AF_RDEC, AF_VMAG, AF_AZI, AF_FPA};
   
   enum
   {
      SPACECRAFT = 0,
      SOLAR_SYSTEM,
      COORD_SYSTEM,
      SPACE_POINT,
      OrbitDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[OrbitDataObjectCount];
};
#endif // OrbitData_hpp

   
