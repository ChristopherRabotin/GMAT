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
 * Implements Orbit related data class.
 *   VelApoapsis, VelPeriapsis, Apoapsis, Periapsis, OrbitPeriod,
 *   RadApoapsis, RadPeriapais, C3Energy, Energy, Altitude(Geodetic)
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "OrbitData.hpp"
#include "ParameterException.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"
#include "CoordUtil.hpp"         // for friend CartesianToKeplerian()
#include "AngleUtil.hpp"
#include "UtilityException.hpp"
#include "SphericalRADEC.hpp"    // for friend CartesianToSphericalRADEC/AZFPA()
#include "ModKeplerian.hpp"      // for friend KeplerianToModKeplerian()
#include "CelestialBody.hpp"
#include "MessageInterface.hpp"

//#define USE_COORDUTIL_FOR_AOP 1
//#define USE_COORDUTIL_FOR_TA  1

//#define DEBUG_ORBITDATA_INIT 1
//#define DEBUG_ORBITDATA_CONVERT 1
//#define DEBUG_ORBITDATA_RUN 1
//#define DEBUG_CLONE 1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const Real OrbitData::ORBIT_REAL_UNDEFINED = -9876543210.1234;
const Real OrbitData::ORBIT_TOL = 1.0e-6; //1.0e-10;
const Real OrbitData::ORBIT_ZERO_TOL = 1.0e-30;

const std::string
OrbitData::VALID_OBJECT_TYPE_LIST[OrbitDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem",
   "CoordinateSystem",
   "SpacePoint"
}; 


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// OrbitData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
OrbitData::OrbitData()
   : RefData(),
   stateTypeId (-1)
{
   mCartState = Rvector6::RVECTOR6_UNDEFINED;
   mKepState = Rvector6::RVECTOR6_UNDEFINED;
   mModKepState = Rvector6::RVECTOR6_UNDEFINED;
   mSphRaDecState = Rvector6::RVECTOR6_UNDEFINED;
   mSphAzFpaState = Rvector6::RVECTOR6_UNDEFINED;
   mMA = ORBIT_REAL_UNDEFINED;
   mCartEpoch = 0.0;
   mGravConst = 0.0;
   
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mOrigin = NULL;
   mInternalCoordSystem = NULL;
   mOutCoordSystem = NULL;
}


//------------------------------------------------------------------------------
// OrbitData(const OrbitData &data)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <data> the OrbitData object being copied.
 */
//------------------------------------------------------------------------------
OrbitData::OrbitData(const OrbitData &data)
   : RefData(data),
   stateTypeId (data.stateTypeId)
{
   #if DEBUG_CLONE
   MessageInterface::ShowMessage
      ("OrbitData::OrbitData copy constructor called\n");
   #endif
   
   mCartState = data.mCartState;
   mKepState = data.mKepState;
   mModKepState = data.mModKepState;
   mSphRaDecState = data.mSphRaDecState;
   mSphAzFpaState = data.mSphAzFpaState;
   mMA = data.mMA;
   mCartEpoch = data.mCartEpoch;
   mGravConst = data.mGravConst;
   
   mSpacecraft = data.mSpacecraft;
   mSolarSystem = data.mSolarSystem;
   mOrigin = data.mOrigin;
   mInternalCoordSystem = data.mInternalCoordSystem;
   mOutCoordSystem = data.mOutCoordSystem;
}


//------------------------------------------------------------------------------
// OrbitData& operator= (const OrbitData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
OrbitData& OrbitData::operator= (const OrbitData &right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
   
   mCartState = right.mCartState;
   mKepState = right.mKepState;
   mModKepState = right.mModKepState;
   mSphRaDecState = right.mSphRaDecState;
   mSphAzFpaState = right.mSphAzFpaState;
   mMA = right.mMA;
   mCartEpoch = right.mCartEpoch;
   mGravConst = right.mGravConst;
   
   mSpacecraft = right.mSpacecraft;
   mSolarSystem = right.mSolarSystem;
   mOrigin = right.mOrigin;
   mInternalCoordSystem = right.mInternalCoordSystem;
   mOutCoordSystem = right.mOutCoordSystem;
   
   stateTypeId = right.stateTypeId;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~OrbitData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OrbitData::~OrbitData()
{
}


//------------------------------------------------------------------------------
// Rvector6 GetCartState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetCartState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   mCartEpoch = mSpacecraft->GetRealParameter("A1Epoch");
   mCartState.Set(mSpacecraft->GetState().GetState());

   #if DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetCartState() mCartState=\n   %s\n", mCartState.ToString().c_str());
   #endif

   if (mInternalCoordSystem == NULL || mOutCoordSystem == NULL)
   {
      MessageInterface::ShowMessage
         ("OrbitData::GetCartState() Internal CoordSystem or Output CoordSystem is NULL.\n");
      
      throw ParameterException
         ("OrbitData::GetCartState() internal or output CoordinateSystem is NULL.\n");
   }

   if (mOriginDep)
      return mCartState;
   
   // convert to output CoordinateSystem
   if (mInternalCoordSystem->GetName() != mOutCoordSystem->GetName())
   {
      #if DEBUG_ORBITDATA_CONVERT
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() mOutCoordSystem:%s, Axis addr=%d\n",
             mOutCoordSystem->GetName().c_str(),
             mOutCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, ""));
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() <-- Before convert: mCartEpoch=%f\nstate = %s\n", 
             mCartEpoch, mCartState.ToString().c_str());
      #endif
      
      try
      {
         mCoordConverter.Convert(A1Mjd(mCartEpoch), mCartState, mInternalCoordSystem,
                                 mCartState, mOutCoordSystem);
         #if DEBUG_ORBITDATA_CONVERT
            MessageInterface::ShowMessage
               ("OrbitData::GetCartState() --> After  convert: mCartEpoch=%f\n"
                "state = %s\n", mCartEpoch, mCartState.ToString().c_str());
         #endif
      }
      catch (BaseException &e)
      {
         MessageInterface::ShowMessage(e.GetMessage());
      }
   }
   
   return mCartState;
}


//------------------------------------------------------------------------------
// Rvector6 GetKepState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetKepState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();

//    mKepState = mSpacecraft->GetState("Keplerian");
//    MessageInterface::ShowMessage("==>OrbitData::GetKepState() from SC mKepState=%s\n",
//                                  mKepState.ToString().c_str());
   
   //loj: 1/25/06 we want to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Anomaly anomaly;
   mKepState = CartesianToKeplerian(state, mGravConst, anomaly);
   
   //MessageInterface::ShowMessage("==>OrbitData::GetKepState() mKepState=%s\n",
   //                              mKepState.ToString().c_str());
   
   mMA = anomaly.GetMeanAnomaly();

   //loj: 1/25/06 replaced by anomaly.GetMeanAnomaly();
//    //-------------------------------------------------------
//    Real ta = DegToRad(mKepState[TA]);
//    //MessageInterface::ShowMessage("==>OrbitData::GetKepState() ta=%f\n", ta);
//    Real costa = Cos(ta);
//    Real ea = ACos((mKepState[1] + costa)/(1.0 + mKepState[1]*costa));
//    mMA = RadToDeg(ea - mKepState[1] * Sin(ea));
//    if (mKepState[TA] > 180.0)
//       mMA = 360.0 - mMA;
//    //-------------------------------------------------------

   //MessageInterface::ShowMessage("==>OrbitData::GetKepState() mMA=%f\n\n", mMA);
   return mKepState;
}


//------------------------------------------------------------------------------
// Rvector6 GetModKepState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetModKepState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   //mKepState = mSpacecraft->GetState("ModifiedKeplerian");
   
   //loj: 1/25/06 we want to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Anomaly anomaly;
   Rvector6 kepState = CartesianToKeplerian(state, mGravConst, anomaly);
   mModKepState = KeplerianToModKeplerian(kepState);

   mMA = anomaly.GetMeanAnomaly();

   //loj: 1/25/06 replaced by anomaly.GetMeanAnomaly();
//    //-------------------------------------------------------
//    Real ta = DegToRad(mModKepState[5]);
//    Real costa = Cos(ta);
//    Real ea = ACos((mModKepState[1] + costa)/(1.0 + mModKepState[1]*costa));
//    mMA = RadToDeg(ea - mKepState[1] * Sin(ea));
//    if (mModKepState[5] > 180.0)
//       mMA = 360.0 - mMA;
//    //-------------------------------------------------------

   return mModKepState;
}


//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetSphRaDecState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetSphRaDecState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
//    mSphRaDecState = mSpacecraft->GetState("SphericalRADEC");
   
   //loj: 1/25/06 we want to convert to parameter coord system first
   Rvector6 state = GetCartState();
   mSphRaDecState = CartesianToSphericalRADEC(state);
   
//    MessageInterface::ShowMessage
//       ("==>OrbitData::GetKepState() GetCartState() mSphRaDecState=\n   %s\n",
//        mSphRaDecState.ToString().c_str());

   return mSphRaDecState;
}


//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetSphAzFpaState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetSphAzFpaState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   //mSphAzFpaState = mSpacecraft->GetState("SphericalAZFPA");

   //loj: 1/25/06 we want to convert to parameter coord system first
   Rvector6 state = GetCartState();
   mSphAzFpaState = CartesianToSphericalAZFPA(state);
   
   return mSphAzFpaState;
}


//------------------------------------------------------------------------------
// Real GetCartReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Cartesian element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetCartReal(Integer item)
{
   Rvector6 state = GetCartState();
   
   if (item >= PX && item <= VZ)
      return mCartState[item];
   else
      throw ParameterException("OrbitData::GetCartReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
}


//------------------------------------------------------------------------------
// Real GetCartReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Cartesian element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetCartReal(const std::string &str)
{
   Rvector6 state = GetCartState();

   if (str == "Epoch")
      return mCartEpoch;
   else if (str == "CartX")
      return mCartState[PX];
   else if (str == "CartY")
      return mCartState[PY];
   else if (str == "CartZ")
      return mCartState[PZ];
   else if (str == "CartVx")
      return mCartState[VX];
   else if (str == "CartVy")
      return mCartState[VY];
   else if (str == "CartVz")
      return mCartState[VZ];
   else
      throw ParameterException("OrbitData::GetCartReal() Unknown parameter name: " +
                               str);
    
}


//------------------------------------------------------------------------------
// Real GetKepReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Keplerian element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetKepReal(Integer item)
{
   Rvector6 state = GetCartState();

   if (mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < ORBIT_ZERO_TOL)
      throw ParameterException
         ("OrbitData::GetKepReal(" + GmatRealUtil::ToString(item) +
          ") position vector is zero. pos: " + pos.ToString() + " vel: " +
          vel.ToString());
   
   switch (item)
   {
   case SMA:
      return GetSemiMajorAxis(pos, vel);
   case ECC:
      return GetEccentricity(pos, vel);
   case INC:
      {
         Rvector6 hVec = GetAngularMomentum(pos, vel);
         Real incDeg = ACos(hVec[2]) * DEG_PER_RAD;
         return incDeg;
      }
   case TA:
   case MA:
      {
         #if USE_COORDUTIL_FOR_TA
      
         Rvector6 state = GetKepState();

         if (item == TA)
            return mKepState[TA];
         else
            return mMA;
      
         #else
      
         // I don't know how much efficient it will be just computing TA here
         // than computing keplerian state and return TA.
      
         // find cos ta and sind ta
         Real sma = GetSemiMajorAxis(pos, vel);
         Rvector6 hVec = GetAngularMomentum(pos, vel);
         Real hMag = hVec[3];
         Real orbParam = hVec[5];
         Real ecc = Sqrt(Abs(1.0 - orbParam/sma));
         Real inc = ACos(hVec[2]);
         Real cosTa = 0.0;
         Real sinTa = 0.0;
      
         if (ecc >= ORBIT_TOL)
         {
            // for elliptic orbit
            Real er = ecc * rMag;
            cosTa = (orbParam - rMag) / er;
            //Spec 2.21 does not give correct results
            //sinTa = (pos*vel) / (mGravConst * er); // GMAT Spec 2.11
            sinTa = (pos*vel)*hMag / (mGravConst * er); // Swingby CoordUtil
         }
         else
         {
            // for circular orbit
            Real hzx = hVec[2] * hVec[0];
            Real hzy = hVec[2] * hVec[1];
            Real hxySq = hVec[0]*hVec[0] + hVec[1]*hVec[1];
         
            if (inc >= ORBIT_TOL)
            {
               // for circular inclined case
               Rvector3 nVec(-hzx, -hzy, hxySq);
               Real nMag = nVec.GetMagnitude();
               cosTa = (-hzx*pos[0] - hzy*pos[1]) / (rMag * hxySq);
               sinTa = (-hzx*pos[0] - hzy*pos[1] + hxySq*pos[2]) / (rMag*nMag);
            }
            else
            {
               // for circular equatorial case
               cosTa = pos[0] / rMag;
               sinTa = pos[1] / rMag;
            }
         }
      
         Real taDeg = ATan(sinTa, cosTa) * DEG_PER_RAD;
      
         if (taDeg < 0.0)
            taDeg = taDeg + 360.0;

         if (item == TA)
         {
            //MessageInterface::ShowMessage("==>OrbitData::GetKepReal() taDeg=%f\n\n", taDeg);
            return taDeg;
         }
         else
         {
            Real ta = DegToRad(taDeg);
            Real costa = Cos(ta);
            Real ea = ACos((ecc + costa)/(1.0 + ecc*costa));
            mMA = RadToDeg(ea - ecc * Sin(ea));
            if (taDeg > 180.0)
               mMA = 360.0 - mMA;
         
            //MessageInterface::ShowMessage("==>OrbitData::GetKepReal() taDeg=%f\n", taDeg);
            //MessageInterface::ShowMessage("==>OrbitData::GetKepReal() mMA=%f\n\n", mMA);
            return mMA;
         }
      
         #endif // USE_COORDUTIL_FOR_TA
      }
   case RAAN:
      return GetRAofAN(pos, vel);
   case RADN:
      {
         Real raanDeg = GetRAofAN(pos, vel);
         return AngleUtil::PutAngleInDegRange(raanDeg + 180, 0.0, 360.0);
      }
   case AOP:
      {
         #if USE_COORDUTIL_FOR_AOP
      
         Rvector6 state = GetKepState();
         return mKepState[AOP];
      
         #else
      
         // I don't know how much efficient it will be just computing AOP here
         // than computing keplerian state and return AOP.
      
         Real sma = GetSemiMajorAxis(pos, vel);
         Rvector6 hVec = GetAngularMomentum(pos, vel);
         Real vMagSq = hVec[4];
         Real orbParam = hVec[5];
         Real ecc = Sqrt(Abs(1.0 - orbParam/sma));
         Real inc = ACos(hVec[2]);
         Real aopDeg;
      
         if (ecc < ORBIT_TOL)
         {
            aopDeg = 0.0;
         }
         else
         {
            Real hzx = hVec[2] * hVec[0];
            Real hzy = hVec[2] * hVec[1];
            Real hxySq = hVec[0]*hVec[0] + hVec[1]*hVec[1];
            Real hxySqrt = Sqrt(hxySq);
            Rvector3 nVec(-hzx, -hzy, hxySq);
            Rvector3 xVec = (vMagSq - mGravConst/rMag)*pos - (pos*vel)*vel;
            Real hyxX = -hVec[1]*xVec[0] + hVec[0]*xVec[1]; //Swingby CoordUtil
            Real xMag = xVec.GetMagnitude();
         
            //if (Abs(inc) >= ORBIT_TOL)
            if (ecc >= ORBIT_TOL && Abs(inc) >= ORBIT_TOL)
            {
               //Spec 2.23 does not give correct results
               //Real nMag = nVec.GetMagnitude();
               //aopDeg = ACos((nVec*xVec) / (nMag*xMag)) * DEG_PER_RAD; //Spec 2.23)
            
               aopDeg = ACos(hyxX / (hxySqrt*xMag)) * DEG_PER_RAD; //Swingby CoordUtil
            
               if (xVec[2] < 0.0)
                  aopDeg = -aopDeg;
            }
            else
            {
               xVec.Normalize();
               aopDeg = ATan(xVec[1], xVec[0]) * DEG_PER_RAD;
            }
         }
      
         if (aopDeg < 0.0)
            aopDeg = aopDeg + 360.0;
      
         return aopDeg;
      
         #endif
      }

   default:
      throw ParameterException("OrbitData::GetKepReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetKepReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Keplerian element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetKepReal(const std::string &str)
{
   if (str == "KepSMA")
      return GetKepReal(SMA);
   else if (str == "KepEcc")
      return GetKepReal(ECC);
   else if (str == "KepInc")
      return GetKepReal(INC);
   else if (str == "KepTA")
      return GetKepReal(TA);
   else if (str == "KepMA")
      return GetKepReal(MA);
   else if (str == "KepRAAN")
      return GetKepReal(RAAN);
   else if (str == "KepRADN")
      return GetKepReal(RADN);
   else if (str == "KepAOP")
      return GetKepReal(AOP);
   else
      throw ParameterException("OrbitData::GetKepReal() Unknown parameter name: " +
                               str);
}


//------------------------------------------------------------------------------
// Real GetOtherKepReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives other Keplerian element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetOtherKepReal(Integer item)
{
   Rvector6 state = GetCartState();
   
   if (mOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   
   Real sma = GetSemiMajorAxis(pos, vel);   
   Real ecc = GetEccentricity(pos, vel);
   
   Real grav = mGravConst;
   Real E, R;

   switch (item)
   {
   case MM:
      {
         Real mm;
         if (ecc < (1.0 - ORBIT_TOL))          // Ellipse
         {
            mm = Sqrt((grav / sma ) / (sma * sma));
         }
         else
         {
            if (ecc > (1.0 + ORBIT_TOL))       // Hyperbola (See B M W; eq's 4.2-19 to 4.2-21)
               mm = sqrt(-(grav / sma)/(sma * sma));
            else                               // Parabola (See B M W 4.2-17 and 4.2-18)
               mm = 2.0 * sqrt(grav);
         }
         return mm;
      }
   case VEL_APOAPSIS:
      {
         E = -grav / (2.0 * sma);
         R = sma * (1.0 + ecc);
         return Sqrt (2.0 * (E + grav/R));
      }
   case VEL_PERIAPSIS:
      {
         E = -grav / (2.0 * sma);
         R = sma * (1.0 - ecc);
         return Sqrt (2.0 * (E + grav/R));
      }
   case ORBIT_PERIOD:
      return GmatMathUtil::TWO_PI * Sqrt((sma * sma * sma)/ grav);
   case RAD_APOAPSIS:
      return sma * (1.0 + ecc);
   case RAD_PERIAPSIS:
      return sma * (1.0 - ecc);
   case C3_ENERGY:
      return -grav / sma;
   case ENERGY:
      return -grav / (2.0 * sma);
   default:
      throw ParameterException
         ("OrbitData::GetOtherKepReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetOtherKepReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives other Keplerian element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetOtherKepReal(const std::string &str)
{
   if (str == "KepMM")
      return GetOtherKepReal(MM);
   else if (str == "VelApoapsis")
      return GetOtherKepReal(VEL_APOAPSIS);
   else if (str == "VelPeriapsis")
      return GetOtherKepReal(VEL_PERIAPSIS);
   else if (str == "OrbitPeriod")
      return GetOtherKepReal(ORBIT_PERIOD);
   else if (str == "RadApoapsis")
      return GetOtherKepReal(RAD_APOAPSIS);
   else if (str == "RadPeriapsis")
      return GetOtherKepReal(RAD_PERIAPSIS);
   else if (str == "C3Energy")
      return GetOtherKepReal(C3_ENERGY);
   else if (str == "Energy")
      return GetOtherKepReal(ENERGY);
   else
   {
      throw ParameterException
         ("OrbitData::GetOtherKepReal() Unknown parameter name: " + str);
   }
}


//------------------------------------------------------------------------------
// Real GetSphRaDecReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Spherical element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetSphRaDecReal(Integer item)
{
   Rvector6 state = GetSphRaDecState();

   #if DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetSphRaDecReal() str=%s state=%s\n",
       str.c_str(), state.ToString().c_str());
   #endif
   
   switch (item)
   {
   case RD_RMAG:
      {
         // if orgin is "Earth" just return default
         if (mOrigin->GetName() == "Earth")
            return mSphRaDecState[RD_RMAG];
         else
            return GetPositionMagnitude(mOrigin);
      }
   case RD_RRA:
   case RD_RDEC:
   case RD_VMAG:
   case RD_RAV:
   case RD_DECV:
      return mSphRaDecState[item];
   default:
      throw ParameterException
         ("OrbitData::GetSphRaDecReal() Unknown parameter name: " +
          GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetSphRaDecReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Spherical element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetSphRaDecReal(const std::string &str)
{
   if (str == "SphRMag")
      return GetSphRaDecReal(RD_RMAG);
   else if (str == "SphRA")
      return GetSphRaDecReal(RD_RRA);
   else if (str == "SphDec")
      return GetSphRaDecReal(RD_RDEC);
   else if (str == "SphVMag")
      return GetSphRaDecReal(RD_VMAG);
   else if (str == "SphRAV")
      return GetSphRaDecReal(RD_RAV);
   else if (str == "SphDecV")
      return GetSphRaDecReal(RD_DECV);
   else
   {
      throw ParameterException
         ("OrbitData::GetSphRaDecReal() Unknown parameter name: " + str);
   }
}


//------------------------------------------------------------------------------
// Real GetSphAzFpaReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Spherical element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetSphAzFpaReal(Integer item)
{
   Rvector6 state = GetSphAzFpaState();

   #if DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetSphAzFpaReal() str=%s state=%s\n",
       str.c_str(), state.ToString().c_str());
   #endif

   if (item >= AF_RMAG && item <= AF_FPA)
      return mSphAzFpaState[item];
   else
   {
      throw ParameterException("OrbitData::GetSphAzFpaReal() Unknown parameter ID: " +
                               GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetSphAzFpaReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Spherical element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetSphAzFpaReal(const std::string &str)
{
   if (str == "SphRMag")
      return GetSphAzFpaReal(AF_RMAG);
   else if (str == "SphRA")
      return GetSphAzFpaReal(AF_RRA);
   else if (str == "SphDec")
      return GetSphAzFpaReal(AF_RDEC);
   else if (str == "SphVMag")
      return GetSphAzFpaReal(AF_VMAG);
   else if (str == "SphAzi")
      return GetSphAzFpaReal(AF_AZI);
   else if (str == "SphFPA")
      return GetSphAzFpaReal(AF_FPA);
   else
   {
      throw ParameterException("OrbitData::GetSphAzFpaReal() Unknown parameter name: " +
                               str);
   }
}


//------------------------------------------------------------------------------
// Real GetAngularReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Computes angular related parameter.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetAngularReal(Integer item)
{
   Rvector6 state = GetCartState();

   #if DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetAngularReal() item=%d state=%s\n",
       item, state.ToString().c_str());
   #endif
   
   Rvector3 pos;
   Rvector3 vel;

   pos = Rvector3(state[0], state[1], state[2]);
   vel = Rvector3(state[3], state[4], state[5]);
   
   Rvector3 hVec3 = Cross(pos, vel);
   Real h = Sqrt(hVec3 * hVec3);
   
   Real grav = mGravConst;

   switch (item)
   {
   case SEMILATUS_RECTUM:
      {
         if (mOrigin->GetName() != "Earth")
         {
            state = GetRelativeCartState(mOrigin);
            pos = Rvector3(state[0], state[1], state[2]);
            vel = Rvector3(state[3], state[4], state[5]);
            hVec3 = Cross(pos, vel);
            h = Sqrt(hVec3 * hVec3);
         }
      
         if (h < ORBIT_TOL)
            return 0.0;
         else
            return (h / grav) * h;      // B M W; eq. 1.6-1
      }
   case HMAG:
      {
         if (mOrigin->GetName() != "Earth")
         {
            state = GetRelativeCartState(mOrigin);
            pos = Rvector3(state[0], state[1], state[2]);
            vel = Rvector3(state[3], state[4], state[5]);
            hVec3 = Cross(pos, vel);
            h = Sqrt(hVec3 * hVec3);
         }
      
         return h; 
      }
   case HX:
      return hVec3[0];
   case HY:
      return hVec3[1];
   case HZ:
      return hVec3[2];
   default:
      throw ParameterException
         ("OrbitData::GetAngularReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetAngularReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Computes angular related parameter.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetAngularReal(const std::string &str)
{
   if (str == "SemilatusRectum")
      return GetAngularReal(SEMILATUS_RECTUM);
   else if (str == "HMAG")
      return GetAngularReal(HMAG);
   else if (str == "HX")
      return GetAngularReal(HX);
   else if (str == "HY")
      return GetAngularReal(HY);
   else if (str == "HZ")
      return GetAngularReal(HZ);
   else
   {
      throw ParameterException
         ("OrbitData::GetAngularReal() Unknown parameter name: " + str);
   }
}


//------------------------------------------------------------------------------
// Real GetOtherAngleReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Computes other angle related parameters.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetOtherAngleReal(Integer item)
{
   Rvector6 state = GetCartState();

   switch (item)
   {
   case BETA_ANGLE:
      {
         if (mOrigin->GetName() != "Earth")
            state = GetRelativeCartState(mOrigin);
      
         // compute orbit normal unit vector
         Rvector3 pos = Rvector3(state[0], state[1], state[2]);
         Rvector3 vel = Rvector3(state[3], state[4], state[5]);
         Rvector3 hVec3 = Cross(pos, vel);
         hVec3.Normalize();
      
         // compute sun unit vector from the origin
         Rvector3 sunPos = (mSolarSystem->GetBody(SolarSystem::SUN_NAME))->
            GetMJ2000Position(mCartEpoch);
         Rvector3 originPos = mOrigin->GetMJ2000Position(mCartEpoch);
         Rvector3 originToSun = sunPos - originPos;
         originToSun.Normalize();
      
         Real betaAngle = ACos(hVec3*originToSun) * DEG_PER_RAD;
         return betaAngle;
      }
   default:
      throw ParameterException
         ("OrbitData::GetOtherAngleReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetOtherAngleReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Computes other angle related parameters.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetOtherAngleReal(const std::string &str)
{
   if (str == "BetaAngle")
      return GetOtherAngleReal(BETA_ANGLE);
   else
   {
      throw ParameterException
         ("OrbitData::GetOtherAngleReal() Unknown parameter name: " + str);
   }
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* OrbitData::GetValidObjectList() const
{
   return VALID_OBJECT_TYPE_LIST;
}


//------------------------------------------------------------------------------
// bool ValidateRefObjects(GmatBase *param)
//------------------------------------------------------------------------------
/**
 * Validates reference objects for given parameter
 */
//------------------------------------------------------------------------------
bool OrbitData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<OrbitDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == OrbitDataObjectCount)
      return true;
   else
      return false;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// Rvector6 GetAngularMomentum(const Rvector3 &pos, const Rvector3 &vel)
//------------------------------------------------------------------------------
/**
 * Computes angular momentum, its magnitude, and other related data.
 *
 * @param <pos> input position vector
 * @param <vel> input velocity vector
 *
 * @return [0] Angular momentum x unit vector
 *         [1] Angular momentum y unit vector
 *         [2] Angular momentum z unit vector
 *         [3] Angular momentum magnitude
 *         [4] velocity magnitude squared
 *         [5] Orbit parameter
 */
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetAngularMomentum(const Rvector3 &pos, const Rvector3 &vel)
{
   Rvector3 r = pos;
   Rvector3 v = vel;
   
   Real vMag = v.GetMagnitude();
   Real vMagSq = vMag*vMag;
   
   Rvector3 hVec = Cross(pos, vel);
   Real hMag = Sqrt(hVec * hVec);
   Real orbParam = (hMag*hMag) / mGravConst;
   hVec.Normalize();
   
   Rvector6 h(hVec[0], hVec[1], hVec[2], hMag, vMagSq, orbParam);
   return h;
}


//------------------------------------------------------------------------------
// Real GetSemiMajorAxis(const Rvector3 &pos, const Rvector3 &vel)
//------------------------------------------------------------------------------
Real OrbitData::GetSemiMajorAxis(const Rvector3 &pos, const Rvector3 &vel)
{   
   Rvector3 r = pos;
   Rvector3 v = vel;
   
   Real rMag = r.GetMagnitude();
   Real vMag = v.GetMagnitude();
   Real vMagSq = vMag*vMag;
   Real denom = (2.0 - (rMag*vMagSq)/mGravConst);
   
   if (Abs(denom) < ORBIT_ZERO_TOL)
      throw ParameterException
         ("OrbitData::GetSemiMajorAxis() divide-by-zero occurred. pos: " +
          r.ToString() + " vel: " + v.ToString());
   
   Real sma = rMag / denom;

   #if DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetSemiMajorAxis() mOrigin=%s, mGravConst=%f\n   r=%s, "
       "v=%s, sma=%f\n", mOrigin->GetName().c_str(), mGravConst,
       r.ToString().c_str(), v.ToString().c_str(), sma);
   #endif
   
   return sma;
}


//------------------------------------------------------------------------------
// Real GetEccentricity(const Rvector3 &pos, const Rvector3 &vel)
//------------------------------------------------------------------------------
Real OrbitData::GetEccentricity(const Rvector3 &pos, const Rvector3 &vel)
{
   Real sma = GetSemiMajorAxis(pos, vel);
   Rvector6 hVec = GetAngularMomentum(pos, vel);
   Real orbParam = hVec[5];
   Real ecc = Sqrt(Abs(1.0 - orbParam/sma));
   return ecc;
}


//------------------------------------------------------------------------------
// Real GetRAofAN(const Rvector3 &pos, const Rvector3 &vel)
//------------------------------------------------------------------------------
Real OrbitData::GetRAofAN(const Rvector3 &pos, const Rvector3 &vel)
{
   Rvector6 hVec = GetAngularMomentum(pos, vel);
   Real inc = ACos(hVec[2]);
   Real raanDeg;
      
   if (Abs(inc) < ORBIT_TOL)
      raanDeg = 0.0;
   else
      raanDeg = ATan(hVec[0], -hVec[1]) * DEG_PER_RAD;

   if (raanDeg < 0.0)
      raanDeg = raanDeg + 360.0;
      
   return raanDeg;
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystem()
//------------------------------------------------------------------------------
SolarSystem* OrbitData::GetSolarSystem()
{
   return mSolarSystem;
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordSys()
//------------------------------------------------------------------------------
CoordinateSystem* OrbitData::GetInternalCoordSys()
{
   return mInternalCoordSystem;
}


//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * @param <cs> internal coordinate system what parameter data is representing.
 */
//------------------------------------------------------------------------------ 
void OrbitData::SetInternalCoordSys(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}


//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetRelativeCartState(SpacePoint *origin)
//------------------------------------------------------------------------------
/**
 * Computes spacecraft cartesian state from the given origin.
 *
 * @param <origin> origin pointer
 *
 * @return spacecraft state from the given origin.
 */
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetRelativeCartState(SpacePoint *origin)
{
   // get spacecraft state
   Rvector6 scState = GetCartState();

   // get origin state
   Rvector6 originState = origin->GetMJ2000State(mCartEpoch);

   #if DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage
         ("OrbitData::GetRelativeCartState() origin=%s, state=%s\n",
          origin->GetName().c_str(), originState.ToString().c_str());
   #endif
      
   // return relative state
   return scState - originState;
}


//------------------------------------------------------------------------------
// Real OrbitData::GetPositionMagnitude(SpacePoint *origin)
//------------------------------------------------------------------------------
/**
 * Computes position magnitude from the given origin.
 *
 * @param <origin> origin pointer
 *
 * @return position magnitude from the given origin.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetPositionMagnitude(SpacePoint *origin)
{
   // get spacecraft position
   Rvector6 scState = GetCartState();
   Rvector3 scPos = Rvector3(scState[0], scState[1], scState[2]);

   // get origin position
   Rvector6 originState = origin->GetMJ2000State(mCartEpoch);
   Rvector3 originPos = Rvector3(originState[0], originState[1], originState[2]);

   // get relative position magnitude
   Rvector3 relPos = scPos - originPos;
   
   #if DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage
         ("OrbitData::GetPositionMagnitude() scPos=%s, originPos=%s, relPos=%s\n",
          scPos.ToString().c_str(), originPos.ToString().c_str(),
          relPos.ToString().c_str());
   #endif
   
   return relPos.GetMagnitude();
}


// The inherited methods from RefData
//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void OrbitData::InitializeRefObjects()
{
   //#if DEBUG_ORBITDATA_INIT
   //MessageInterface::ShowMessage("OrbitData::InitializeRefObjects() entered.\n");
   //#endif
   
   mSpacecraft =
      (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   
   if (mSpacecraft == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find Spacecraft object: " +
          GetRefObjectName(Gmat::SPACECRAFT) + ".\n" +
          "Make sure Spacecraft is set to any internal parameters.\n");
   
   if (stateTypeId == -1)
      stateTypeId = mSpacecraft->GetParameterID("StateType");
   
   mSolarSystem =
      (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   
   if (mSolarSystem == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   if (mInternalCoordSystem == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find internal "
          "CoordinateSystem object\n");
   
   mOutCoordSystem =
      (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   
   if (mOutCoordSystem == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find output "
          "CoordinateSystem object\n");
   
   // get spacecraft CoordinateSystem
   std::string csName = mSpacecraft->GetRefObjectName(Gmat::COORDINATE_SYSTEM);   
   CoordinateSystem *cs = (CoordinateSystem*)mSpacecraft->
      GetRefObject(Gmat::COORDINATE_SYSTEM, csName);

   if (!cs)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() spacecraft CoordinateSystem not "
          "found: " + csName + "\n");

   // Set origin to out coord system origin
   mOrigin = mOutCoordSystem->GetOrigin();
   
   if (!mOrigin)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() origin not found: " +
          cs->GetOriginName() + "\n");
   
   // get gravity constant if out coord system origin is CelestialBody
   if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
   
   //-----------------------------------------------------------------
   // if dependent body name exist and it is a CelestialBody,
   // set new gravity constant
   //-----------------------------------------------------------------
   std::string originName =
      FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[SPACE_POINT]));

   mOriginDep = false;
   
   if (originName != "")
   {
      #if DEBUG_ORBITDATA_INIT
      MessageInterface::ShowMessage
         ("OrbitData::InitializeRefObjects() getting originName:%s pointer.\n",
          originName.c_str());
      #endif
      
      mOrigin =
         (SpacePoint*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACE_POINT]);

      if (!mOrigin)
         throw ParameterException
            ("OrbitData::InitializeRefObjects() Cannot find Origin object: " +
             originName + "\n");
      
      // override gravity constant if origin is CelestialBody
      if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
         mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();

      mOriginDep = true;
   }
   
   #if DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage
      ("OrbitData::InitializeRefObjects() mOrigin.Name=%s, mGravConst=%f, "
       "mOriginDep=%d\n",  mOrigin->GetName().c_str(), mGravConst, mOriginDep);
   #endif
}


//------------------------------------------------------------------------------
// virtual bool IsValidObjectType(Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Checks reference object type.
 *
 * @return return true if object is valid object, false otherwise
 */
//------------------------------------------------------------------------------
bool OrbitData::IsValidObjectType(Gmat::ObjectType type)
{
   for (int i=0; i<OrbitDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}


