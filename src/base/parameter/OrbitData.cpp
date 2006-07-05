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
 *   RadApoapsis, RadPeriapais, C3Energy, Energy
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
#include "Equinoctial.hpp"
#include "CelestialBody.hpp"
#include "MessageInterface.hpp"

#define __USE_COORDUTIL__

//#define __USE_COORDUTIL_FOR_AOP__
//#define __USE_COORDUTIL_FOR_TA__

//#define __TRY_COORD_CONVERSION__  // causes crash as of 7/5/06, dhunter

#define __USE_NEW_TA_CODE__   // new code gives results much closer to STK
#define __USE_NEW_ECC_CODE__  // new code gives results slightly closer to STK
#define __USE_NEW_SMA_CODE__  // new code gives no change in results
#define __USE_NEW_INC_CODE__
#define __USE_NEW_AOP_CODE__
#define __USE_NEW_VELAPO_CODE__
#define __USE_NEW_VELPER_CODE__
#define __USE_NEW_RAAN_CODE__   // new code gives virtually no change in results

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
   //MessageInterface::ShowMessage("==> OrbitData::~OrbitData()\n");
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
   
   // if origin dependent parameter, the relative position/velocity is computed in
   // the parameter calculation, so just return prop state.
   if (mOriginDep)
      return mCartState;
   
   if (mInternalCoordSystem == NULL || mOutCoordSystem == NULL)
   {
      MessageInterface::ShowMessage
         ("OrbitData::GetCartState() Internal CoordSystem or Output CoordSystem is NULL.\n");
      
      throw ParameterException
         ("OrbitData::GetCartState() internal or output CoordinateSystem is NULL.\n");
   }
   
   //-----------------------------------------------------------------
   // convert to output CoordinateSystem
   //-----------------------------------------------------------------
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

   //MessageInterface::ShowMessage("==>OrbitData::GetKepState() from SC mKepState=%s\n",
   //                               mKepState.ToString().c_str());
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Anomaly anomaly;
   mKepState = CartesianToKeplerian(state, mGravConst, anomaly);
   
   //MessageInterface::ShowMessage("==>OrbitData::GetKepState() mKepState=%s\n",
   //                              mKepState.ToString().c_str());
   
   mMA = anomaly.GetMeanAnomaly();

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
      
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Anomaly anomaly;
   Rvector6 kepState = CartesianToKeplerian(state, mGravConst, anomaly);
   mModKepState = KeplerianToModKeplerian(kepState);

   mMA = anomaly.GetMeanAnomaly();
   
   return mModKepState;
}


//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetSphRaDecState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetSphRaDecState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   mSphRaDecState = CartesianToSphericalRADEC(state);
   
   //MessageInterface::ShowMessage
   //   ("==>OrbitData::GetKepState() GetCartState() mSphRaDecState=\n   %s\n",
   //    mSphRaDecState.ToString().c_str());

   return mSphRaDecState;
}


//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetSphAzFpaState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetSphAzFpaState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   mSphAzFpaState = CartesianToSphericalAZFPA(state);
   
   return mSphAzFpaState;
}

//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetEquinState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetEquinState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   // Call GetCartState() to convert to parameter coord system first
   Rvector6 state = GetCartState();
   Rvector6 mEquinState = CartesianToEquinoctial(state, mGravConst);
   
   return mEquinState;
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

   if (mOriginDep && mOrigin->GetName() != "Earth")
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
         #ifndef __USE_NEW_INC_CODE__
         Rvector6 hVec = GetAngularMomentum(pos, vel);
         Real incDeg = ACos(hVec[2]) * DEG_PER_RAD;
         return incDeg;
         
         #else
         
         Rvector3 posOut = pos;
         Rvector3 velOut = vel; // vectors to be used in computation
         #ifdef __TRY_COORD_CONVERSION__
         // Convert pos and vel to coord-sys F, if necessary
         // CAUSES CRASH
         if (mInternalCoordSystem != mOutCoordSystem) {
            mCoordConverter.Convert(A1Mjd(mCartEpoch), pos, mInternalCoordSystem, posOut, mOutCoordSystem);
            mCoordConverter.Convert(A1Mjd(mCartEpoch), vel, mInternalCoordSystem, velOut, mOutCoordSystem);
         }
         #endif
         
         Rvector3 h = Cross(posOut, velOut);
         Real hMag = h.GetMagnitude();
   
         Real i = ACos(h.Get(2) / hMag);
         Real iDeg = i * DEG_PER_RAD;
         return iDeg;
         #endif
      }
   case TA:
   case MA:
      {
         #ifdef __USE_COORDUTIL__
      
         Rvector6 state = GetKepState();

         if (item == TA)
            return mKepState[TA];
         else
            return mMA;
      
         #else
      
         // I don't know how much efficient it will be just computing TA here
         // than computing keplerian state and return TA.

         #ifndef __USE_NEW_TA_CODE__
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

         #else
         
         Rvector3 posOut = pos;
         Rvector3 velOut = vel; // vectors to be used in computation
         #ifdef __TRY_COORD_CONVERSION__
         // Convert pos and vel to coord-sys F, if necessary
         // CAUSES CRASH
         if (mInternalCoordSystem != mOutCoordSystem) {
            mCoordConverter.Convert(A1Mjd(mCartEpoch), pos, mInternalCoordSystem, posOut, mOutCoordSystem);
            mCoordConverter.Convert(A1Mjd(mCartEpoch), vel, mInternalCoordSystem, velOut, mOutCoordSystem);
         }
         #endif

         Rvector3 h = Cross(posOut, velOut);
         Real hMag = h.GetMagnitude();
         
         Rvector3 n = Cross(Rvector3(0,0,1), h);
         Real nMag = n.GetMagnitude();
         
         Real r = posOut.GetMagnitude();
         Real v = velOut.GetMagnitude();
         
         Rvector3 eVec = (1/mGravConst)*((v*v - mGravConst/r)*posOut - (posOut*velOut)*velOut);
         Real ecc = eVec.GetMagnitude();
         
         Real i = ACos(h.Get(2) / hMag);
         
         Real trueAnom;
         if (ecc >= 1E-11) {
                trueAnom = ACos((eVec*posOut)/(ecc*r));
                if (posOut*velOut < 0)
                   trueAnom = TWO_PI - trueAnom;
         }
         else {
                if (i >= 1E-11) {
                   trueAnom = ACos((n*posOut)/(nMag*r));
                   if (posOut.Get(2) < 0)
                      trueAnom = TWO_PI - trueAnom;
                }
                else {
                   trueAnom = ACos(posOut.Get(0) / r);
                   if (posOut.Get(1) < 0)
                      trueAnom = TWO_PI - trueAnom;
                }
         }
         Real taDeg = trueAnom * DEG_PER_RAD;
      
         if (taDeg < 0.0)
            taDeg = taDeg + 360.0;

         #endif // __USE_NEW_TA_CODE__
         
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
      
         #endif // __USE_COORDUTIL__
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
         #ifdef __USE_COORDUTIL_FOR_AOP__
      
         Rvector6 state = GetKepState();
         return mKepState[AOP];
      
         #else
      
         // I don't know how much efficient it will be just computing AOP here
         // than computing keplerian state and return AOP.

         #ifndef __USE_NEW_AOP_CODE__
      
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
            Rvector3 nVec(-hzx, -hzy, hxySq);
            Rvector3 xVec = (vMagSq - mGravConst/rMag)*pos - (pos*vel)*vel;
            Real xMag = xVec.GetMagnitude();
            
            //if (Abs(inc) >= ORBIT_TOL)
            if (ecc >= ORBIT_TOL && Abs(inc) >= ORBIT_TOL)
            {
               // Spec 2.23 (It does not give correct results)
               //Real nMag = nVec.GetMagnitude();
               //aopDeg = ACos((nVec*xVec) / (nMag*xMag)) * DEG_PER_RAD; 
               
               // Swingby CoordUtil
               Real hyxX = -hVec[1]*xVec[0] + hVec[0]*xVec[1]; 
               Real hxySqrt = Sqrt(hxySq);
               aopDeg = ACos(hyxX / (hxySqrt*xMag)) * DEG_PER_RAD;
               
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
      
         #else
         
         Real r = pos.GetMagnitude();
         Real v = vel.GetMagnitude(); 
         
         Rvector3 eVec = (1/mGravConst)*((v*v - mGravConst/r)*pos - (pos*vel)*vel);
         Real ecc = eVec.GetMagnitude();
         
         if (ecc < Exp10(-11))
                return 0;
                
         Rvector3 hVec = Cross(pos, vel);
         Real h= hVec.GetMagnitude();
         
         Real i = ACos(hVec.Get(2) / h);
         
         Real aop;
         if (i < Exp10(-11)) {
                aop = ACos(eVec.Get(0) / ecc);
                if (eVec.Get(1) < Exp10(-11))
                   aop = TWO_PI - aop;
         }
         else {
                Rvector3 n = Cross(Rvector3(0,0,1), hVec);
                aop = ACos((n*eVec)/(n.GetMagnitude() * ecc));
                if (eVec.Get(2) < Exp10(-11))
                   aop = TWO_PI - aop;
         }
         
         Real aopDeg = aop * DEG_PER_RAD;
         if (aopDeg < 0.0)
            aopDeg = aopDeg + 360.0;

         return aopDeg;
      
         #endif // __USE_NEW_AOP_CODE
         #endif // __USE_COORDUTIL_FOR_AOP__
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
         #ifndef __USE_NEW_VELAPO_CODE__
         Real E, R;
         E = -grav / (2.0 * sma);
         R = sma * (1.0 + ecc);
         return Sqrt (2.0 * (E + grav/R));

         #else
         Real vA;
         if (1-ecc < 1E-12)
            vA = 0;
         else
            vA = Sqrt( (mGravConst/sma)*((1-ecc)/(1+ecc)) );
         return vA;
         #endif
      }
   case VEL_PERIAPSIS:
      {
         #ifndef __USE_NEW_VELPER_CODE__
         Real E, R;
         E = -grav / (2.0 * sma);
         R = sma * (1.0 - ecc);
         return Sqrt (2.0 * (E + grav/R));
         #else
         Real vP;
         vP = Sqrt( (mGravConst/sma)*((1+ecc)/(1-ecc)) );
         return vP;
         #endif
      }
   case ORBIT_PERIOD:
      if (sma < 0) return 0;
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


//------------------------------------------------------------------------------
// Real GetEquiReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrives Equinoctial element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetEquinReal(Integer item)
{
   Rvector6 state = GetCartState();

   if (mOriginDep && mOrigin->GetName() != "Earth")
   {
      state = state - mOrigin->GetMJ2000State(mCartEpoch);
   }
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);   
   Real rMag = pos.GetMagnitude();
   
   if (rMag < ORBIT_ZERO_TOL)
      throw ParameterException
         ("OrbitData::GetEquiReal(" + GmatRealUtil::ToString(item) +
          ") position vector is zero. pos: " + pos.ToString() + " vel: " +
          vel.ToString());
   
   switch (item)
   {
   case EQ_SMA:
      return GetSemiMajorAxis(pos, vel);
   case EY:
      {
        Rvector6 equiState = GetEquinState();
         return equiState[1];
      }
   case EX:
      {
        Rvector6 equiState = GetEquinState();
         return equiState[2];
      }
   case NY:
      {
        Rvector6 equiState = GetEquinState();
         return equiState[3];
      }
   case NX:
      {
        Rvector6 equiState = GetEquinState();
         return equiState[4];
      }
   case MLONG:
      {
        Rvector6 equiState = GetEquinState();
         return equiState[5];
      }

   default:
      throw ParameterException("OrbitData::GetKepReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetEquiReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Equinoctial element
 */
//------------------------------------------------------------------------------
Real OrbitData::GetEquinReal(const std::string &str)
{
   Rvector6 state = GetCartState();

   if (str == "EquiSma")
      return GetEquinReal(EQ_SMA);
   else if (str == "EquiEy")
      return GetEquinReal(EY);
   else if (str == "EquiEx")
      return GetEquinReal(EX);
   else if (str == "EquiNy")
      return GetEquinReal(NY);
   else if (str == "EquiNx")
      return GetEquinReal(NX);
   else if (str == "EquiMlong")
      return GetEquinReal(MLONG);
   else
      throw ParameterException("OrbitData::GetEquiReal() Unknown parameter name: " +
                               str);
    
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
   
   Real rMag = r.GetMagnitude(); // ||r||
   Real vMag = v.GetMagnitude(); // ||v||

   #ifndef __USE_NEW_SMA_CODE__
   
   Real vMagSq = vMag*vMag;
   Real denom = (2.0 - (rMag*vMagSq)/mGravConst);
   
   if (Abs(denom) < ORBIT_ZERO_TOL)
      throw ParameterException
         ("OrbitData::GetSemiMajorAxis() divide-by-zero occurred. pos: " +
          r.ToString() + " vel: " + v.ToString());
   
   Real sma = rMag / denom;
   
   #else

   Real zeta = 0.5*(vMag*vMag) - mGravConst/rMag;
   
   Real sma = -mGravConst/(2*zeta);

   #endif
   
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
   #ifndef __USE_NEW_ECC_CODE__
   Real sma = GetSemiMajorAxis(pos, vel);
   Rvector6 hVec = GetAngularMomentum(pos, vel);
   Real orbParam = hVec[5];
   Real ecc = Sqrt(Abs(1.0 - orbParam/sma));
   return ecc;
   
   #else
   
   #ifndef __USE_COORDUTIL__
   Real r = pos.GetMagnitude();
   Real v = vel.GetMagnitude();
   
   Rvector3 e;  // eccentricity vector
   e = (1/mGravConst)*((v*v - mGravConst/r)*pos - (pos*vel)*vel);
   
   Real eMag = e.GetMagnitude(); // ||e||
   
   return eMag;
   #else
   Rvector6 state = GetKepState();

   return state[ECC];
   
   #endif  // __USE_COORDUTIL__
   #endif  // __USE_NEW_ECC_CODE
}


//------------------------------------------------------------------------------
// Real GetRightAscensionOfVelocity(const Rvector3 &vel)
//------------------------------------------------------------------------------
Real OrbitData::GetRightAscensionOfVelocity(const Rvector3 &vel)
{
   // Convert velocity vector to coord-system F, if necessary
   Rvector3 velOut; // vector to be used in computation
   if (mInternalCoordSystem != mOutCoordSystem) {
          mCoordConverter.Convert(A1Mjd(mCartEpoch), vel, mInternalCoordSystem, velOut, mOutCoordSystem);
   }
   else {
          velOut = vel;
   }
   
   Real rav = ATan(velOut.Get(1), velOut.Get(0));
        
   return rav;
}


//------------------------------------------------------------------------------
// Real GetRAofAN(const Rvector3 &pos, const Rvector3 &vel)
//------------------------------------------------------------------------------
Real OrbitData::GetRAofAN(const Rvector3 &pos, const Rvector3 &vel)
{
   #ifndef __USE_NEW_RAAN_CODE__
  
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
   
   #else
   
   Rvector3 posOut = pos;
   Rvector3 velOut = vel; // vectors to be used in computation
   #ifdef __TRY_COORD_CONVERSION__
   // Convert pos and vel to coord-sys F, if necessary
   // CAUSES CRASH
   if (mInternalCoordSystem != mOutCoordSystem) {
      mCoordConverter.Convert(A1Mjd(mCartEpoch), pos, mInternalCoordSystem, posOut, mOutCoordSystem);
      mCoordConverter.Convert(A1Mjd(mCartEpoch), vel, mInternalCoordSystem, velOut, mOutCoordSystem);
   }
   #endif

   Rvector3 h = Cross(posOut, velOut);
   Real hMag = h.GetMagnitude();
   
   Rvector3 n = Cross(Rvector3(0, 0, 1), h);
   Real nMag = n.GetMagnitude();
   
   Real i = ACos(h.Get(2) / hMag);  // inclination in radians
   
   Real omega;
   if (i >= 1E-11) {
      omega = ACos(n.Get(0) / nMag);
      if (n.Get(1) < 0)
         omega = TWO_PI - omega;
   }
   else
      omega = 0;
   
   return omega*DEG_PER_RAD;
   #endif // __USE_NEW_RAAN_CODE__
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
// virtual void InitializeRefObjects(
//------------------------------------------------------------------------------
void OrbitData::InitializeRefObjects()
{
   #if DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage("OrbitData::InitializeRefObjects() entered.\n");
   #endif
   
   mSpacecraft =
      (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   
   if (mSpacecraft == NULL)
   {
      #if DEBUG_ORBITDATA_INIT
      MessageInterface::ShowMessage
         ("OrbitData::InitializeRefObjects() Cannot find spacecraft: " +
          GetRefObjectName(Gmat::SPACECRAFT) + ".\n" +
          "Make sure Spacecraft is set to any internal parameters.\n");
      #endif
      
      throw ParameterException
         ("Cannot find spacecraft: " + GetRefObjectName(Gmat::SPACECRAFT));
   }
   
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
   
//    mOutCoordSystem =
//       (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   
//    if (mOutCoordSystem == NULL)
//    {
//       #if DEBUG_ORBITDATA_INIT
//       MessageInterface::ShowMessage
//          ("OrbitData::InitializeRefObjects() Cannot find output "
//           "CoordinateSystem object\n");
//       #endif
      
//       throw ParameterException
//          ("OrbitData::InitializeRefObjects() Cannot find coordinate system.\n");
//    }

   
//    // Set origin to out coordinate system origin for CoordinateSystem
//    // dependent parameter
//    mOrigin = mOutCoordSystem->GetOrigin();
   
//    if (!mOrigin)
//    {
//       #if DEBUG_ORBITDATA_INIT
//       MessageInterface::ShowMessage
//          ("OrbitData::InitializeRefObjects() origin not found: " +
//           mOutCoordSystem->GetOriginName() + "\n");
//       #endif
      
//       throw ParameterException
//          ("Coordinate system origin: " + mOutCoordSystem->GetOriginName() +
//           " not found.");
//    }
   
//    // get gravity constant if out coord system origin is CelestialBody
//    if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
//       mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
   
   //-----------------------------------------------------------------
   // if dependent body name exist and it is a CelestialBody,
   // it is origin dependent parameter, set new gravity constant.
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
         throw InvalidDependencyException
            (" is a central body dependent parameter.");

      // override gravity constant if origin is CelestialBody
      if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
         mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();

      mOriginDep = true;
   }
   //-----------------------------------------------------------------
   // It is CoordinateSystem dependent parameter.
   // Set Origin to CoordinateSystem origin and mu to origin mu.
   //-----------------------------------------------------------------
   else
   {
      mOutCoordSystem =
         (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   
      if (mOutCoordSystem == NULL)
      {
         #if DEBUG_ORBITDATA_INIT
         MessageInterface::ShowMessage
            ("OrbitData::InitializeRefObjects() Cannot find output "
             "CoordinateSystem object\n");
         #endif
         
         throw ParameterException
            ("OrbitData::InitializeRefObjects() Cannot find coordinate system.\n");
      }
      
      
      // Set origin to out coordinate system origin for CoordinateSystem
      // dependent parameter
      mOrigin = mOutCoordSystem->GetOrigin();
      
      if (!mOrigin)
      {
         #if DEBUG_ORBITDATA_INIT
         MessageInterface::ShowMessage
            ("OrbitData::InitializeRefObjects() origin not found: " +
             mOutCoordSystem->GetOriginName() + "\n");
         #endif
      
         throw ParameterException
            ("Coordinate system origin: " + mOutCoordSystem->GetOriginName() +
             " not found.");
      }
      
      // get gravity constant if out coord system origin is CelestialBody
      if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
         mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
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


