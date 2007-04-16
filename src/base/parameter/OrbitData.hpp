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
   Rvector6 GetEquinState();

   Real GetCartReal(Integer item);
   Real GetCartReal(const std::string &str);
   
   Real GetKepReal(Integer item);
   Real GetKepReal(const std::string &str);
   
   Real GetOtherKepReal(Integer item);
   Real GetOtherKepReal(const std::string &str);
   
   Real GetSphRaDecReal(Integer item);
   Real GetSphRaDecReal(const std::string &str);
   
   Real GetSphAzFpaReal(Integer item);
   Real GetSphAzFpaReal(const std::string &str);
   
   Real GetAngularReal(Integer item);
   Real GetAngularReal(const std::string &str);
   
   Real GetOtherAngleReal(Integer item);
   Real GetOtherAngleReal(const std::string &str);

   Real GetEquinReal(Integer item);
   Real GetEquinReal(const std::string &str);

   // The inherited methods from RefData
   virtual bool ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;
      
protected:

   bool mOriginDep;
   SolarSystem* GetSolarSystem();
   CoordinateSystem* GetInternalCoordSys();
      
   void SetInternalCoordSys(CoordinateSystem *cs);
   Rvector6 GetRelativeCartState(SpacePoint *origin);
   Real GetPositionMagnitude(SpacePoint *origin);
   
   // The inherited methods from RefData
   virtual void InitializeRefObjects();
   virtual bool IsValidObjectType(Gmat::ObjectType type);
   
   Rvector6 mCartState;
   Rvector6 mKepState;
   Rvector6 mModKepState;
   Rvector6 mSphRaDecState;
   Rvector6 mSphAzFpaState;
   
   Real mCartEpoch;
   Real mGravConst;
   
   Integer stateTypeId;
   
   Spacecraft *mSpacecraft;
   SolarSystem *mSolarSystem;
   SpacePoint *mOrigin;
   CoordinateSystem *mInternalCoordSystem;
   CoordinateSystem *mOutCoordSystem;
   
   // only one CoordinateConverter needed
   static CoordinateConverter mCoordConverter;
   
   enum {PX, PY, PZ, VX, VY, VZ};
   enum {SMA, ECC, INC, RAAN, AOP, TA, MA, EA, HA, RADN};
   enum {RD_RMAG, RD_RRA, RD_RDEC, RD_VMAG, RD_RAV, RD_DECV};
   enum {AF_RMAG, AF_RRA, AF_RDEC, AF_VMAG, AF_AZI, AF_FPA};
   enum {MM, VEL_APOAPSIS, VEL_PERIAPSIS, ORBIT_PERIOD, RAD_APOAPSIS,
         RAD_PERIAPSIS, C3_ENERGY, ENERGY};
   enum {SEMILATUS_RECTUM, HMAG, HX, HY, HZ, BETA_ANGLE};
   enum {EQ_SMA, EY, EX, NY, NX, MLONG};
   
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

   
