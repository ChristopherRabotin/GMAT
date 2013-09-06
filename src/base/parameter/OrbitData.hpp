//$Id$
//------------------------------------------------------------------------------
//                                  OrbitData
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
#include "Rmatrix66.hpp"
#include "Rmatrix33.hpp"
#include "Spacecraft.hpp"
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"

class GMAT_API OrbitData : public RefData
{
public:

   OrbitData(const std::string &name = "",
             const Gmat::ObjectType paramOwnerType = Gmat::SPACECRAFT);
   OrbitData(const OrbitData &data);
   OrbitData& operator= (const OrbitData& right);
   virtual ~OrbitData();
   
   Rvector6 GetCartState();
   Rvector6 GetKepState();
   Rvector6 GetModKepState();
   Rvector6 GetSphRaDecState();
   Rvector6 GetSphAzFpaState();
   Rvector6 GetEquinState();
    // Modified by M.H.
   Rvector6 GetModEquinState();
   Rvector6 GetDelaState();
   Rvector6 GetPlanetodeticState();
   
   void SetReal(Integer item, Real rval);
   void SetRvector6(const Rvector6 &val);
   
   Real GetCartReal(Integer item);
   Real GetCartReal(const std::string &str);
   
   Real GetKepReal(Integer item);
   Real GetKepReal(const std::string &str);
   
   Real GetModKepReal(Integer item);
   Real GetModKepReal(const std::string &str);
   
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
   
   // Modified by M.H.
   Real GetModEquinReal(Integer item);
   Real GetModEquinReal(const std::string &str);

   Real GetDelaReal(Integer item);
   Real GetDelaReal(const std::string &str);

   Real GetPlanetodeticReal(Integer item);
   Real GetPlanetodeticReal(const std::string &str);

   const Rmatrix66& GetStmRmat66(Integer item);
   const Rmatrix33& GetStmRmat33(Integer item);
   
   // The inherited methods from RefData
   virtual bool               ValidateRefObjects(GmatBase *param);
   virtual const std::string* GetValidObjectList() const;

   // More inherited methods from RefData
   virtual std::string        GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type);

   virtual bool               SetRefObjectName(const Gmat::ObjectType type,
                                               const std::string &name);
   virtual GmatBase*          GetRefObject(const Gmat::ObjectType type,
                                           const std::string &name = "");
   virtual bool               SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                           const std::string &name = "");

protected:

   virtual bool              AddRefObject(const Gmat::ObjectType type,
                                          const std::string &name, GmatBase *obj = NULL,
                                          bool replaceName = false);

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
   // Modified by M.H.
   Rvector6 mModEquinState;
   Rvector6 mDelaState;
   Rvector6 mPlanetodeticState;

   Rmatrix66  mSTM;
   Rmatrix33  mSTMSubset;
   
   Real mCartEpoch;
   Real mGravConst;
   Real mFlattening;
   Real mEqRadius;
   
   Integer stateTypeId;
   
   Spacecraft *mSpacecraft;
   SpacePoint *mSpacePoint;
   SolarSystem *mSolarSystem;
   SpacePoint *mOrigin;
   CoordinateSystem *mInternalCS;
   CoordinateSystem *mParameterCS;
   
   bool mIsParamOriginDep;
   bool firstTimeEpochWarning;
   
   // only one CoordinateConverter needed
   static CoordinateConverter mCoordConverter;
   
   // Assign item ids
   // Cartesian items
   enum {CART_X = 0, CART_Y, CART_Z, CART_VX, CART_VY, CART_VZ, CART_STATE, Item1Count};
   
   // Keplerian items
   enum {KEP_SMA = Item1Count, KEP_ECC, KEP_INC, KEP_RAAN, KEP_AOP, KEP_TA, KEP_MA,
         KEP_EA, KEP_HA, KEP_RADN, KEP_STATE, MODKEP_RADAPO, MODKEP_RADPER, Item2Count};
   
   // Spherical RADEC items
   enum {RADEC_RMAG = Item2Count, RADEC_RA, RADEC_DEC, RADEC_VMAG, RADEC_RAV, RADEC_DECV,
         Item3Count};
   
   // Spherical AZIFPA items
   enum {AZIFPA_RMAG = Item3Count, AZIFPA_RA, AZIFPA_DEC, AZIFPA_VMAG, AZIFPA_AZI,
         AZIFPA_FPA, Item4Count};
   
   // Other orbit items
   // @note - Do not add or remove items from this list without updating OrbitData.
   //         These enums are also used in OrbitData for passing parameter names to
   //         CalculationUtilitis::CalculateKeplerianData().
   enum {MM = Item4Count, VEL_APOAPSIS, VEL_PERIAPSIS, ORBIT_PERIOD, C3_ENERGY, ENERGY, Item5Count};
   
   enum {SEMILATUS_RECTUM = Item5Count, HMAG, HX, HY, HZ, BETA_ANGLE, HYPERBOLIC_RLA,
         HYPERBOLIC_DLA, Item6Count};
   
   // Equinoctial items
   enum {EQ_SMA = Item6Count, EQ_H, EQ_K, EQ_P, EQ_Q, EQ_MLONG, Item7Count};
   
   // Orbit STM items
   enum {ORBIT_STM = Item7Count, ORBIT_STM_A, ORBIT_STM_B, ORBIT_STM_C, ORBIT_STM_D,
         Item8Count};
   // Modified by M.H.
   // ModifiedEquinoctial items
   enum {MOD_EQ_P = Item8Count, MOD_EQ_F, MOD_EQ_G, MOD_EQ_H, MOD_EQ_K, MOD_EQ_TLONG, Item9Count};
   
   // Modified by M.H.
   // Delaunay items
   enum {DEL_DELA_SL = Item9Count, DEL_DELA_SG, DEL_DELA_SH, DEL_DELA_L, DEL_DELA_G, DEL_DELA_H, Item10Count};

   // Modified by M.H.
   // Planetodetic items
   enum {PLD_RMAG = Item10Count, PLD_LON, PLD_LAT, PLD_VMAG, PLD_AZI, PLD_HFPA, Item11Count};

   enum
   {
      SPACECRAFT = 0,
      SOLAR_SYSTEM,
      COORD_SYSTEM,
      SPACE_POINT,
      OrbitDataObjectCount
   };
   
   static const std::string VALID_OBJECT_TYPE_LIST[OrbitDataObjectCount];
   static const Real        ORBIT_DATA_TOLERANCE;
   static const std::string VALID_ANGLE_PARAM_NAMES[HYPERBOLIC_DLA - SEMILATUS_RECTUM + 1];
   static const std::string VALID_OTHER_ORBIT_PARAM_NAMES[ENERGY - MM + 1];
};
#endif // OrbitData_hpp

   
