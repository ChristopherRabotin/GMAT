//$Id$
//------------------------------------------------------------------------------
//                                  Keplerian
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
// Author: Linda Jun (NASA/GSFC)
// Created: 2006/12/28
//
/**
 * Defines KeplerianElement class;  Performs conversions between mean, true and
 * eccentric anomaly.  Converts Cartesian Elements to Keplerian Elements.
 */
//------------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#include "gmatdefs.hpp"
#include "RealUtilities.hpp"     // for PI, TWO_PI, ACos(), Sqrt(), Mod()
#include "GmatConstants.hpp"
#include "Keplerian.hpp"
#include "Rvector.hpp"
#include "UtilityException.hpp"
#include "CoordUtil.hpp"         // for KeplerianToCartesian()
#include "MessageInterface.hpp"

//#define DEBUG_KEPLERIAN 1
//#define DEBUG_KEPLERIAN_TA 2
//#define DEBUG_KEPLERIAN_ECC
//#define DEBUG_ECC_VEC
//#define DEBUG_KEPLERIAN_AOP 1
//#define DEBUG_ANOMALY 1
//#define DEBUG_ECC_VEC 1
//#define DEBUG_ANOMALY_MA

using namespace GmatMathUtil;
using namespace GmatMathConstants;


//---------------------------------
// static data
//---------------------------------
const std::string Keplerian::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Semimajor Axis",
   "Eccentricity",
   "Inclination",
   "RA of Ascending Node",
   "Argument of Periapsis",
   "True Anomally"
};

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// Keplerian::Keplerian() 
//------------------------------------------------------------------------------
Keplerian::Keplerian() 
{
}

//------------------------------------------------------------------------------
// Keplerian::Keplerian(Real sma, Real ecc, Radians inc,
//                      Radians raan, Radians aop, Radians ta)
//------------------------------------------------------------------------------
/*
 * Constructor with values
 *
 * @param  sma   Semimajor axis in km
 * @param  ecc   Eccentricity
 * @param  inc   Inclination in radians
 * @param  raan  Right ascension of ascending node in radians
 * @param  aop   Argument of periapsis in radians
 * @param  ta    True anomaly in radians
 */
//------------------------------------------------------------------------------
Keplerian::Keplerian(Real sma, Real ecc, Radians inc,
                     Radians raan, Radians aop, Radians ta)
   :
   mSemimajorAxis(sma),
   mEccentricity(ecc),
   mInclination(inc),
   mRaOfAscendingNode(raan),
   mArgOfPeriapsis(aop),
   mTrueAnomaly(ta)
{
}


//------------------------------------------------------------------------------
// Keplerian::Keplerian(const Keplerian &keplerian)
//------------------------------------------------------------------------------
/*
 * Copy Constructor
 */
//------------------------------------------------------------------------------
Keplerian::Keplerian(const Keplerian &keplerian)
   :
   mSemimajorAxis(keplerian.mSemimajorAxis),
   mEccentricity(keplerian.mEccentricity),
   mInclination(keplerian.mInclination),
   mRaOfAscendingNode(keplerian.mRaOfAscendingNode),
   mArgOfPeriapsis(keplerian.mArgOfPeriapsis),
   mTrueAnomaly(keplerian.mTrueAnomaly)
{
}


//------------------------------------------------------------------------------
// const Keplerian& Keplerian::operator=(const Keplerian &keplerian)
//------------------------------------------------------------------------------
const Keplerian& Keplerian::operator=(const Keplerian &keplerian)
{
   if (this != &keplerian)
   {
      mSemimajorAxis       = keplerian.mSemimajorAxis;
      mEccentricity        = keplerian.mEccentricity;
      mInclination         = keplerian.mInclination;
      mRaOfAscendingNode   = keplerian.mRaOfAscendingNode;
      mArgOfPeriapsis      = keplerian.mArgOfPeriapsis;
      mTrueAnomaly         = keplerian.mTrueAnomaly;
   }
   return *this;
}


//------------------------------------------------------------------------------
// Keplerian::~Keplerian() 
//------------------------------------------------------------------------------
Keplerian::~Keplerian() 
{
}


//------------------------------------------------------------------------------
// Real Keplerian::GetSemimajorAxis() const
//------------------------------------------------------------------------------
Real Keplerian::GetSemimajorAxis() const
{
   return mSemimajorAxis;
}


//------------------------------------------------------------------------------
// Real Keplerian::GetEccentricity() const
//------------------------------------------------------------------------------
Real Keplerian::GetEccentricity() const
{
   return mEccentricity;
}


//------------------------------------------------------------------------------
// Radians Keplerian::GetInclination() const
//------------------------------------------------------------------------------
Radians Keplerian::GetInclination() const
{
   return mInclination;
}


//------------------------------------------------------------------------------
// Radians Keplerian::GetRAAscendingNode() const
//------------------------------------------------------------------------------
Radians Keplerian::GetRAAscendingNode() const
{
   return mRaOfAscendingNode;
}


//------------------------------------------------------------------------------
// Radians Keplerian:: GetArgumentOfPeriapsis() const
//------------------------------------------------------------------------------
Radians Keplerian:: GetArgumentOfPeriapsis() const
{
   return mArgOfPeriapsis;
}


//------------------------------------------------------------------------------
// Radians Keplerian::GetMeanAnomaly() const
//------------------------------------------------------------------------------
Radians Keplerian::GetMeanAnomaly() const
{
   return 0.0;
}


//------------------------------------------------------------------------------
// Radians Keplerian::GetTrueAnomaly() const
//------------------------------------------------------------------------------
Radians Keplerian::GetTrueAnomaly() const
{
   return mTrueAnomaly;
}


//------------------------------------------------------------------------------
// void Keplerian::SetSemimajorAxis(Real sma)
//------------------------------------------------------------------------------
void Keplerian::SetSemimajorAxis(Real a)
{
   mSemimajorAxis = a;
}


//------------------------------------------------------------------------------
// void Keplerian::SetEccentricity(Real e)
//------------------------------------------------------------------------------
void Keplerian::SetEccentricity(Real e)
{
   mEccentricity = e;
}


//------------------------------------------------------------------------------
// void Keplerian::SetInclination(Radians i)
//------------------------------------------------------------------------------
void Keplerian::SetInclination(Radians i)
{
   mInclination = i;
}


//------------------------------------------------------------------------------
// void Keplerian::SetRAAscendingNode(Radians raan)
//------------------------------------------------------------------------------
void Keplerian::SetRAAscendingNode(Radians raan)
{
   mRaOfAscendingNode = raan;
}


//------------------------------------------------------------------------------
// void Keplerian::SetArgumentOfPeriapsis(Radians aop)
//------------------------------------------------------------------------------
void Keplerian::SetArgumentOfPeriapsis(Radians aop)
{
   mArgOfPeriapsis = aop;
}


//------------------------------------------------------------------------------
// void Keplerian::SetMeanAnomaly(Radians ma)
//------------------------------------------------------------------------------
void Keplerian::SetMeanAnomaly(Radians ma)
{
   mMeanAnomaly = ma;
}


//------------------------------------------------------------------------------
// void Keplerian::SetTrueAnomaly(Radians ta)
//------------------------------------------------------------------------------
void Keplerian::SetTrueAnomaly(Radians ta)
{
   mTrueAnomaly = ta;
}


//------------------------------------------------------------------------------
// void Keplerian::Setall(Real a, Real e, Radians i, Radians raan,
//                                Radians aop, Radians ma)
//------------------------------------------------------------------------------
void Keplerian::SetAll(Real a, Real e, Radians i, Radians raan,
                       Radians aop, Radians ma)
{
   mSemimajorAxis = a;
   mEccentricity = e;
   mInclination = i;
   mRaOfAscendingNode = raan;
   mArgOfPeriapsis = aop;
   mTrueAnomaly = ma;
}


//------------------------------------------------------------------------------
// Integer GetNumData() const
//------------------------------------------------------------------------------
Integer Keplerian::GetNumData() const
{
   return NUM_DATA;
}


//------------------------------------------------------------------------------
// const std::string* GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* Keplerian::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}


//------------------------------------------------------------------------------
// std::string* ToValueStrings()
//------------------------------------------------------------------------------
std::string* Keplerian::ToValueStrings()
{
   std::stringstream ss("");

   ss << mSemimajorAxis;
   stringValues[0] = ss.str();
   
   ss.str("");
   ss << mEccentricity;
   stringValues[1] = ss.str();
   
   ss.str("");
   ss << mInclination;
   stringValues[2] = ss.str();
   
   ss.str("");
   ss << mRaOfAscendingNode;
   stringValues[3] = ss.str();
   
   ss.str("");
   ss << mArgOfPeriapsis;
   stringValues[4] = ss.str();
   
   ss.str("");
   ss << mTrueAnomaly;
   stringValues[5] = ss.str();

   return stringValues;
}


//---------------------------------
// static functions
//---------------------------------

//------------------------------------------------------------------------------
// static Rvector6 KeplerianToCartesian(Real mu, const Rvector6 &state,
//                                      const std::string &anomalyType = "TA")
//------------------------------------------------------------------------------
Rvector6 Keplerian::KeplerianToCartesian(Real mu, const Rvector6 &state,
                                         const std::string &anomalyType)
{
   Anomaly anomaly;
   anomaly.Set(state[0], state[1], state[5], anomalyType);
   return CoordUtil::KeplerianToCartesian(state, mu, anomaly);
}


//------------------------------------------------------------------------------
// Real CartesianToSMA(Real mu, const Rvector3 &pos,
//                     const Rvector3 &vel)
//------------------------------------------------------------------------------
Real Keplerian::CartesianToSMA(Real mu, const Rvector3 &pos,
                               const Rvector3 &vel)
{
   #if DEBUG_KEPLERIAN_SMA
   MessageInterface::ShowMessage("CartesianToSMA() ");
   #endif
   
   Real rMag = pos.GetMagnitude(); // ||r||
   Real vMag = vel.GetMagnitude(); // ||v||
   
   Real zeta = 0.5*(vMag*vMag) - mu/rMag;
   

   // check if the orbit is near parabolic
   Real ecc = CartesianToECC(mu, pos, vel);
   if ((Abs(1.0 - ecc)) <= GmatOrbitConstants::KEP_ECC_TOL)
   {
     
      throw UtilityException
         ("Error in conversion from Cartesian to Keplerian state: "
          "The state results in an orbit that is nearly parabolic.\n");
   }
   
   Real sma = -mu/(2*zeta);

   // Check for a singular conic section
   if (Abs(sma*(1 - ecc) < .001))
   {
      throw UtilityException
         ("Error in conversion from Cartesian to Keplerian state: "
          "The state results in a singular conic section with radius of periapsis less than 1 m.\n");
   }

      
   #if DEBUG_KEPLERIAN_SMA
   MessageInterface::ShowMessage("returning %f\n", sma);
   #endif
   
   return sma;
}


//------------------------------------------------------------------------------
// static Rvector3 CartesianToEccVector(Real mu, const Rvector3 &pos,
//                                      const Rvector3 &vel)
//------------------------------------------------------------------------------
Rvector3 Keplerian::CartesianToEccVector(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel)
{
   Real rMag = pos.GetMagnitude();
   Real vMag = vel.GetMagnitude();

   Rvector3 eVec = ((vMag*vMag - mu/rMag)*pos - (pos*vel)*vel) / mu;

   #ifdef DEBUG_ECC_VEC
   MessageInterface::ShowMessage
      ("CartesianToEccVector() returning\n   %s\n", eVec.ToString().c_str());
   #endif
   
   return eVec;
}


//------------------------------------------------------------------------------
// static Real CartesianToECC(Real mu, const Rvector3 &pos,
//                            const Rvector3 &vel)
//------------------------------------------------------------------------------
Real Keplerian::CartesianToECC(Real mu, const Rvector3 &pos,
                               const Rvector3 &vel)
{
   #ifdef DEBUG_KEPLERIAN_ECC
   MessageInterface::ShowMessage("CartesianToECC() ");
   #endif
   
   Rvector3 eVec = CartesianToEccVector(mu, pos, vel);   
   Real eMag = eVec.GetMagnitude(); // ||e||
   
   #ifdef DEBUG_KEPLERIAN_ECC
   MessageInterface::ShowMessage("returning %f\n", eMag);
   #endif
   
   return eMag;
}


//------------------------------------------------------------------------------
// static Real CartesianToINC(Real mu, const Rvector3 &pos,
//                            const Rvector3 &vel)
//------------------------------------------------------------------------------
Real Keplerian::CartesianToINC(Real mu, const Rvector3 &pos,
                               const Rvector3 &vel, bool inRadian)
{
   #if DEBUG_KEPLERIAN_INC
   MessageInterface::ShowMessage("CartesianToINC() ");
   #endif
   
   // check if the orbit is near parabolic
   Real eMag = CartesianToECC(mu, pos, vel);
   if ((Abs(1.0 - eMag)) <= GmatOrbitConstants::KEP_ZERO_TOL)
   {
      
      throw UtilityException
      ("Error in conversion from Cartesian to Keplerian state: "
       "The state results in an orbit that is nearly parabolic.\n");
   }
   
   Rvector3 hVec = Cross(pos, vel);
   Real hMag = hVec.GetMagnitude();
   
   //Real inc = ACos(hVec[2] / hMag);
   Real inc = ACos((hVec[2] / hMag), GmatOrbitConstants::KEP_TOL);
   if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Keplerian state: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   } 
   
   #if DEBUG_KEPLERIAN_INC
   MessageInterface::ShowMessage("returning %f\n", inc);
   #endif
      
   if (inRadian)
      return inc;
   else
      return inc * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// static Rvector3 CartesianToDirOfLineOfNode(const Rvector3 &pos, const Rvector3 &vel)
//------------------------------------------------------------------------------
Rvector3 Keplerian::CartesianToDirOfLineOfNode(const Rvector3 &pos, const Rvector3 &vel)
{
   Rvector3 hVec = Cross(pos, vel);
   Rvector3 nVec = Cross(Rvector3(0, 0, 1), hVec);
   return nVec;
}


//------------------------------------------------------------------------------
// static Real CartesianToRAAN(Real mu, const Rvector3 &pos,
//                             const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
Real Keplerian::CartesianToRAAN(Real mu, const Rvector3 &pos,
                                const Rvector3 &vel, bool inRadian)
{
   #if DEBUG_KEPLERIAN_RAAN
   MessageInterface::ShowMessage("CartesianToRAAN() ");
   #endif
   
   Real ecc = CartesianToECC(mu, pos, vel);
   Real inc = CartesianToINC(mu, pos, vel, true);
   if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Keplerian state: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   } 
   Real raan = 0.0;
   
   // Case 1:  Non-circular, Inclined Orbit
   if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      //raan = ACos(nVec[0] / nMag);
      raan = ACos((nVec[0] / nMag), GmatOrbitConstants::KEP_TOL);
      
      // Fix quadrant
      if (nVec[1] < 0.0)
         raan = TWO_PI - raan;   
   }
   // Case 2: Non-circular, Equatorial Orbit
   else if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      raan = 0.0;
   }
   // Case 3: Circular, Inclined Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      //raan = ACos(nVec[0] / nMag);
      raan = ACos((nVec[0] / nMag), GmatOrbitConstants::KEP_TOL);
      
      // Fix quadrant
      if (nVec[1] < 0.0)
         raan = TWO_PI - raan;   
   }
   // Case 4: Circular, Equatorial Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      raan = 0.0;
   }
   
   #if DEBUG_KEPLERIAN_RAAN
   MessageInterface::ShowMessage("returning %f\n", raan);
   #endif
   
   // Convert 2pi to 0
   raan = Mod(raan, TWO_PI);
   
   if (inRadian)
      return raan;
   else
      return raan * DEG_PER_RAD;
   
}


//------------------------------------------------------------------------------
// static Real CartesianToAOP(Real mu, const Rvector3 &pos,
//                            const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
Real Keplerian::CartesianToAOP(Real mu, const Rvector3 &pos,
                               const Rvector3 &vel, bool inRadian)
{
   #if DEBUG_KEPLERIAN_AOP
   MessageInterface::ShowMessage("CartesianToAOP() ");
   #endif
   
   Rvector3 eVec = CartesianToEccVector(mu, pos, vel);
   Real inc = CartesianToINC(mu, pos, vel, true);
   if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Keplerian state: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   } 
   Real ecc = eVec.GetMagnitude();
   Real aop = 0.0;
   
   // Case 1:  Non-circular, Inclined Orbit
   if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      //aop = ACos((nVec*eVec) / (nMag*ecc));
      aop = ACos(((nVec*eVec) / (nMag*ecc)), GmatOrbitConstants::KEP_TOL);
      
      // Fix quadrant
      if (eVec[2] < 0.0)
         aop = TWO_PI - aop;
   }
   // Case 2: Non-circular, Equatorial Orbit
   else if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      //aop = ACos(eVec[0] / ecc);
      aop = ACos((eVec[0] / ecc), GmatOrbitConstants::KEP_TOL);
      
      // Fix quadrant
      if (eVec[1] < 0.0)
         aop = TWO_PI - aop;
   }
   // Case 3: Circular, Inclined Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      aop = 0.0;
   }
   // Case 4: Circular, Equatorial Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      aop = 0.0;
   }
   
   #if DEBUG_KEPLERIAN_AOP
   MessageInterface::ShowMessage("returning %f\n", aop);
   #endif
   
   // Convert 2pi to 0
   aop = Mod(aop, TWO_PI);
   
   if (inRadian)
      return aop;
   else
      return aop * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// static Real CartesianToTA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
Real Keplerian::CartesianToTA(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_KEPLERIAN_TA
   MessageInterface::ShowMessage
      ("CartesianToTA() mu=%f, inRadian=%d\n   pos =%s\n   vel =%s\n", mu, inRadian,
       pos.ToString().c_str(), vel.ToString().c_str());
   #endif
   
   Rvector3 eVec = CartesianToEccVector(mu, pos, vel);
   Real inc = CartesianToINC(mu, pos, vel, true);
   if (inc >= PI - GmatOrbitConstants::KEP_TOL)
   {
      throw UtilityException
         ("Error in conversion to Keplerian state: "
          "GMAT does not currently support orbits with inclination of 180 degrees.\n");
   } 
   Real ecc = eVec.GetMagnitude();
   Real rMag = pos.GetMagnitude();
   Real ta = 0.0;
   
   #if DEBUG_KEPLERIAN_TA > 1
   MessageInterface::ShowMessage
      ("   eVec=%s,\n   inc=%f, ecc=%f, rMag=%f, ta=%f\n",
       eVec.ToString().c_str(), inc, ecc, rMag, ta);
   #endif
   
   // Case 1:  Non-circular, Inclined Orbit
   if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {      
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 1:  Non-circular, Inclined Orbit\n");
      #endif
      
      Real temp = (eVec*pos) / (ecc*rMag);
      ta = ACos(temp, GmatOrbitConstants::KEP_TOL);
      
      #if DEBUG_KEPLERIAN_TA > 1
      MessageInterface::ShowMessage("   ACos(%+.16f) = %+.16f\n", temp, ACos(temp, GmatOrbitConstants::KEP_TOL));      
      MessageInterface::ShowMessage("   ACos(%+.16f) = %+.16f\n", temp, ta);
      #endif
      
      // Fix quadrant
      if ((pos*vel) < 0.0)
      {
         ta = TWO_PI - ta;
         #if DEBUG_KEPLERIAN_TA > 1
         MessageInterface::ShowMessage("   after quadrant fix, ta=%f\n", ta);
         #endif
      }
   }
   // Case 2: Non-circular, Equatorial Orbit
   else if ((ecc >= GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 2: Non-circular, Equatorial Orbit\n");
      #endif
      
      //ta = ACos((eVec*pos) / (ecc*rMag));
      ta = ACos(((eVec*pos) / (ecc*rMag)), GmatOrbitConstants::KEP_TOL);
      
      // Fix quadrant
      if ((pos*vel) < 0.0)
         ta = TWO_PI - ta;
   }
   // Case 3: Circular, Inclined Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc >= GmatOrbitConstants::KEP_TOL))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 3: Circular, Inclined Orbit\n");
      #endif
      
      Rvector3 nVec =  CartesianToDirOfLineOfNode(pos, vel);
      Real nMag = nVec.GetMagnitude();
      //ta = ACos((nVec*pos) / (nMag*rMag));
      ta = ACos(((nVec*pos) / (nMag*rMag)), GmatOrbitConstants::KEP_TOL);
      
      // Fix quadrant
      if ((pos[2]) < 0.0)
         ta = TWO_PI - ta;
   }
   // Case 4: Circular, Equatorial Orbit
   else if ((ecc < GmatOrbitConstants::KEP_TOL) && (inc < GmatOrbitConstants::KEP_TOL))
   {
      #ifdef DEBUG_KEPLERIAN_TA
      MessageInterface::ShowMessage("   Case 4: Circular, Equatorial Orbit\n");
      #endif
      
      //ta = ACos(pos[0] / rMag);
      ta = ACos((pos[0] / rMag), GmatOrbitConstants::KEP_TOL);
      
      // Fix quadrant
      if ((pos[1]) < 0.0)
         ta = TWO_PI - ta;
   }
   
   // Convert 2pi to 0
   ta = Mod(ta, TWO_PI);
   
   #ifdef DEBUG_KEPLERIAN_TA
   MessageInterface::ShowMessage("CartesianToTA() returning %f\n", ta);
   #endif
   
   if (inRadian)
      return ta;
   else
      return ta * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// static Real CartesianToEA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
Real Keplerian::CartesianToEA(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel, bool inRadian)
{
   #if DEBUG_KEPLERIAN_EA
   MessageInterface::ShowMessage("CartesianToEA() ");
   #endif
   
   Real ta = CartesianToTA(mu, pos, vel, true);
   Real ecc = CartesianToECC(mu, pos, vel);
   Real ea = TrueToEccentricAnomaly(ta, ecc);
   
   #if DEBUG_KEPLERIAN_EA
   MessageInterface::ShowMessage("returning %f\n", ea);
   #endif
   
   if (inRadian)
      return ea;
   else
      return ea * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// static Real CartesianToHA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
Real Keplerian::CartesianToHA(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel, bool inRadian)
{
   #if DEBUG_KEPLERIAN_HA
   MessageInterface::ShowMessage("CartesianToHA() ");
   #endif
   
   Real ta = CartesianToTA(mu, pos, vel, true);
   Real ecc = CartesianToECC(mu, pos, vel);
   Real ha = TrueToHyperbolicAnomaly(ta, ecc);
   
   #if DEBUG_KEPLERIAN_HA
   MessageInterface::ShowMessage("returning %f\n", ha);
   #endif
   
   if (inRadian)
      return ha;
   else
      return ha * DEG_PER_RAD;
   
}


//------------------------------------------------------------------------------
// static Real CartesianToMA(Real mu, const Rvector3 &pos,
//                           const Rvector3 &vel, bool inRadian = false)
//------------------------------------------------------------------------------
Real Keplerian::CartesianToMA(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel, bool inRadian)
{
   #ifdef DEBUG_ANOMALY_MA
   MessageInterface::ShowMessage("CartesianToMA() ");
   #endif
   
   Real ta = CartesianToTA(mu, pos, vel, true);
   Real ecc = CartesianToECC(mu, pos, vel);   
   Real ma = TrueToMeanAnomaly(ta, ecc);
   
   #ifdef DEBUG_ANOMALY_MA
   MessageInterface::ShowMessage("returning %f\n", ma);
   #endif
   
   if (inRadian)
      return ma;
   else
      return ma * DEG_PER_RAD;
}


//------------------------------------------------------------------------------
// static Rvector6 CartesianToAngularMomentum(Real mu, const Rvector3 &pos,
//                                            const Rvector3 &vel)
//------------------------------------------------------------------------------
/**
 * CartesianTos angular momentum, its magnitude, and other related data.
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
Rvector6 Keplerian::CartesianToAngularMomentum(Real mu, const Rvector3 &pos,
                                               const Rvector3 &vel)
{   
   #if DEBUG_KEPLERIAN_AM
   MessageInterface::ShowMessage("CartesianToAngularMomentum() ");
   #endif
   
   Real vMag = pos.GetMagnitude();
   Real vMagSq = vMag*vMag;
   
   Rvector3 hVec = Cross(pos, vel);
   Real hMag = Sqrt(hVec * hVec);
   Real orbParam = (hMag*hMag) / mu;
   hVec.Normalize();
   
   Rvector6 h(hVec[0], hVec[1], hVec[2], hMag, vMagSq, orbParam);
   
   #if DEBUG_KEPLERIAN_AM
   MessageInterface::ShowMessage("returning %s\n", h.ToString().c_str());
   #endif
   
   return h;
}


//------------------------------------------------------------------------------
// static Rvector6 CartesianToKeplerian(Real mu, const Rvector3 &pos,
//                                      const Rvector3 &vel,
//                                      Anomaly::AnomalyType anomalyType)
//------------------------------------------------------------------------------
Rvector6 Keplerian::CartesianToKeplerian(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel,
                                         Anomaly::AnomalyType anomalyType)
{
   #if DEBUG_KEPLERIAN
   MessageInterface::ShowMessage("CartesianToKeplerian() ");
   #endif
   
//   Real sma = CartesianToSMA(mu, pos, vel);
//   Real ecc = CartesianToECC(mu, pos, vel);
//   Real inc = CartesianToINC(mu, pos, vel);
//   Real raan = CartesianToRAAN(mu, pos, vel);
//   Real aop = CartesianToAOP(mu, pos, vel);
//   Real ta = CartesianToTA(mu, pos, vel);
//
//   Real anomaly = ta;
//
//   if (anomalyType != Anomaly::TA)
//   {
//      Anomaly temp;
//      temp.Set(sma, ecc, ta, Anomaly::TA);
//      anomaly = temp.GetValue(anomalyType);
//   }

//   Rvector6 kep(sma, ecc, inc, raan, aop, anomaly);
   Real tfp, ma;
   Real p[3], v[3];
   for (unsigned int ii = 0; ii < 3; ii++)
   {
      p[ii] = pos[ii];
      v[ii] = vel[ii];
   }

   Real     kepOut[6];
   Integer retval = CoordUtil::ComputeCartToKepl(mu, p, v, &tfp, kepOut, &ma);
   if (retval != 0)
   {
      ; // an error?  warning message?
   }
   
   Real anomaly = kepOut[5];
   Real sma = kepOut[0];
   Real ecc = kepOut[1];
   Real ta  = kepOut[5];
   
   if (anomalyType != Anomaly::TA)
   {
      Anomaly temp;
      temp.Set(sma, ecc, ta, Anomaly::TA);
      anomaly = temp.GetValue(anomalyType);
   }
   Rvector6 kep(sma, ecc, kepOut[2], kepOut[3], kepOut[4], anomaly);

   #if DEBUG_KEPLERIAN
   MessageInterface::ShowMessage("returning %s\n", kep.ToString().c_str());
   #endif
   
   return kep;
}


//------------------------------------------------------------------------------
// static Rvector6 CartesianToKeplerian(Real mu, const Rvector3 &pos,
//                                      const Rvector3 &vel,
//                                      const std::string &anomalyType = "TA")
//------------------------------------------------------------------------------
Rvector6 Keplerian::CartesianToKeplerian(Real mu, const Rvector3 &pos,
                                         const Rvector3 &vel,
                                         const std::string &anomalyType)
{
   Anomaly::AnomalyType type = Anomaly::GetAnomalyType(anomalyType);
   return CartesianToKeplerian(mu, pos, vel, type);
}


//------------------------------------------------------------------------------
// static Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state
//                                      Anomaly::AnomalyType anomalyType)
//------------------------------------------------------------------------------
Rvector6 Keplerian::CartesianToKeplerian(Real mu, const Rvector6 &state,
                                         Anomaly::AnomalyType anomalyType)
{
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   return CartesianToKeplerian(mu, pos, vel, anomalyType);
}


//------------------------------------------------------------------------------
// static Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state
//                                      const std::string &anomalyType = "TA")
//------------------------------------------------------------------------------
Rvector6 Keplerian::CartesianToKeplerian(Real mu, const Rvector6 &state,
                                         const std::string &anomalyType)
{
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   return CartesianToKeplerian(mu, pos, vel, anomalyType);
}


//------------------------------------------------------------------------------
// static Radians TrueToMeanAnomaly(Radians ta, Real ecc)
//------------------------------------------------------------------------------
/*
 * Computes mean anomaly from true anomaly
 */
//------------------------------------------------------------------------------
Radians Keplerian::TrueToMeanAnomaly(Radians ta, Real ecc)
{
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("TrueToMeanAnomaly() ta=%f, ecc=%f\n", ta, ecc);
   #endif
   
   Real ma = 0.0;
   
   if (ecc < (1.0 - GmatOrbitConstants::KEP_ANOMALY_TOL))
   {
      Real ea = TrueToEccentricAnomaly(ta, ecc);
      ma = ea - ecc * Sin(ea);
   }
   else if (ecc > (1.0 + GmatOrbitConstants::KEP_ANOMALY_TOL))
   {
      Real ha = TrueToHyperbolicAnomaly(ta, ecc);
      ma = ecc * Sinh(ha) - ha;
   }
   else
   {
      std::string warn = 
         "Warning: Orbit is near parabolic in mean anomaly calculation.  ";
      warn += "Setting MA = 0\n";
      MessageInterface::PopupMessage(Gmat::WARNING_, warn);
      ma = 0.0;
   }
   
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage("TrueToMeanAnomaly() returning %f\n", ma);
   #endif
   
   if (ma < 0.0)
      ma = ma + TWO_PI;
   
   return ma;
}


//------------------------------------------------------------------------------
// static Radians TrueToEccentricAnomaly(Radians ta, Real ecc)
//------------------------------------------------------------------------------
Radians Keplerian::TrueToEccentricAnomaly(Radians ta, Real ecc)
{
   #if DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("TrueToEccentricAnomaly() ta=%f, ecc=%f\n", ta, ecc);
   #endif
   
   Real ea = 0.0;
   
   if (ecc <= (1.0 - GmatOrbitConstants::KEP_ANOMALY_TOL))
   {
      Real cosTa = Cos(ta);
      Real eccCosTa = ecc * cosTa;
      Real sinEa = (Sqrt(1.0 - ecc*ecc) * Sin(ta)) / (1.0 + eccCosTa);
      Real cosEa = (ecc + cosTa) / (1.0 + eccCosTa);
      ea = ATan2(sinEa, cosEa);
   }
   
   if (ea < 0.0)
      ea = ea + TWO_PI;
   
   #if DEBUG_ANOMALY
   MessageInterface::ShowMessage("TrueToEccentricAnomaly() returning %f\n", ea);
   #endif
   
   return ea;
}


//------------------------------------------------------------------------------
// static Radians TrueToHyperbolicAnomaly(Radians ta, Real ecc)
//------------------------------------------------------------------------------
Radians Keplerian::TrueToHyperbolicAnomaly(Radians ta, Real ecc)
{
   #if DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("TrueToHyperbolicAnomaly() ta=%f, ecc=%f\n", ta, ecc);
   #endif
   
   Real ha = 0.0;
   
   if (ecc >= (1.0 + GmatOrbitConstants::KEP_ANOMALY_TOL))
   {
      Real cosTa = Cos(ta);
      Real eccCosTa = ecc * cosTa;
      Real sinhHa = (Sin(ta) * Sqrt(ecc*ecc - 1.0)) / (1.0 + eccCosTa);
      Real coshHa = (ecc + cosTa) / (1.0 + eccCosTa);
      ha = ATanh(sinhHa / coshHa);
   }
   
   // Per discussions April 23, 2007, HA falls between -180 and 180 deg, so the
   // following lines are not needed:
   //   if (ha < 0.0)
   //      ha = ha + TWO_PI;
   
   #if DEBUG_ANOMALY
   MessageInterface::ShowMessage("TrueToHyperbolicAnomaly() returning %f\n", ha);
   #endif
   
   return ha;
}


//------------------------------------------------------------------------------
// static Real MeanToTrueAnomaly(Real maInDeg, Real ecc, Real tol = 1.0e-08)
//------------------------------------------------------------------------------
Real Keplerian::MeanToTrueAnomaly(Real maInDeg, Real ecc, Real tol)
{
   #if DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("MeanToTrueAnomaly() maInDeg=%f, ecc=%f\n", maInDeg, ecc);
   #endif
   
   Real ta;
   Integer iter;
   Integer ret;
   
   ret = ComputeMeanToTrueAnomaly(maInDeg, ecc, tol, &ta, &iter);
   
   if (ret == 0)
   {
      #if DEBUG_ANOMALY
      MessageInterface::ShowMessage("MeanToTrueAnomaly() returning %f\n", ta);
      #endif
      
      return ta;
   }
   
   throw UtilityException("MeanToTrueAnomaly() Error converting "
                          " Mean Anomaly to True Anomaly\n");
}


//------------------------------------------------------------------------------
// static Integer ComputeMeanToTrueAnomaly(Real maInDeg, Real ecc, Real tol,
//                                         Real *ta, Integer *iter)
//------------------------------------------------------------------------------
/*
 * @param <ecc>  input eccentricity
 * @param <maInDeg>   input elliptical, hyperbolic or mean anomaly in degrees
 * @param <tol>  input tolerance for accuracy
 * @param <ta>   output true anomaly in degrees
 * @param <iter> output number of iterations
 */
//------------------------------------------------------------------------------
Integer Keplerian::ComputeMeanToTrueAnomaly(Real maInDeg, Real ecc, Real tol,
                                            Real *ta, Integer *iter)
{
   Real temp, temp2;
   Real rm,   e,     e1,  e2,  c,  f,  f1,   f2,    g;
   Real ztol = 1.0e-30;
   int done;
   
   rm = DegToRad(Mod(maInDeg, 360.0));
   *iter = 0;
   
   if (ecc <= 1.0)
   {
      //---------------------------------------------------------
      // elliptical orbit
      //---------------------------------------------------------
      
      e2 = rm + ecc * Sin(rm);
      done = 0;

      while (!done)
      {
         *iter = *iter + 1;
         temp = 1.0 - ecc * Cos(e2);

         if (Abs(temp) < ztol)
            return (3);

         e1 = e2 - (e2 - ecc * Sin(e2) - rm)/temp;

         if (Abs(e2-e1) < tol)
         {
            done = 1;
            e2 = e1;
         }

         if (!done)
         {
            *iter = *iter + 1;
            temp = 1.0 - ecc * Cos(e1);

            if (Abs(temp) < ztol)
               return (4);

            e2 = e1 - (e1 - ecc * Sin(e1) - rm)/temp;

            if( Abs(e1-e2) < tol)
               done = 1;
         }
      }

      e = e2;

      if (e < 0.0)
         e = e + TWO_PI;

      c = Abs(RadToDeg(e) - 180.0);
      
      if (c >= 1.0e-08) 
      {
         temp  = 1.0 - ecc;
         
         if (Abs(temp)< ztol)
            return (5);
         
         temp2 = (1.0 + ecc)/temp;
         
         if (temp2 <0.0)
            return (6);
         
         f = Sqrt(temp2);
         g = Tan(e/2.0);
         *ta = 2.0 * RadToDeg(ATan(f*g));
         
      }
      else
      {
         *ta = RadToDeg(e);
      }
      
      if( *ta < 0.0)
         *ta = *ta + 360.0;
      
   }
   else
   {
      //---------------------------------------------------------
      // hyperbolic orbit
      //---------------------------------------------------------
      
      if (rm > PI)
         rm = rm - TWO_PI;
      
      f2 = ecc * Sinh(rm) - rm;
      done = 0;
      
      while (!done)
      {
         *iter = *iter + 1;
         temp = ecc * Cosh(f2) - 1.0;
         
         if (Abs(temp) < ztol)
            return (7);
         
         f1 = f2 - (ecc * Sinh(f2) - f2 - rm) / temp;
         
         if (Abs(f2-f1) < tol)
         {
            done = 1;
            f2 = f1;
         } 
         
         if (!done)
         {
            *iter = *iter + 1; 
            temp = ecc * Cosh(f1) - 1.0;

            if (Abs(temp) < ztol)
               return (8);
            
            f2 = f1 - (ecc * Sinh(f1) - f1 - rm) / temp;
            
            if ( Abs(f1-f2) < tol)
               done = 1;
         }
         
         if (*iter > 1000)
         {
            throw UtilityException
               ("ComputeMeanToTrueAnomaly() "
                "Caught in infinite loop numerical argument "
                "out of domain for sinh() and cosh()\n");
         }
      }
      
      f = f2;
      temp = ecc - 1.0;
      
      if (Abs(temp) < ztol)
         return (9);
      
      temp2 = (ecc + 1.0) / temp;
      
      if (temp2 <0.0)
         return (10);
      
      e = Sqrt(temp2);
      g = Tanh(f/2.0);
      *ta = 2.0 * RadToDeg(ATan(e*g));
      
      if (*ta < 0.0)
         *ta = *ta + 360.0;
   }
   
   return (0);
   
} // end ComputeMeanToTrueAnomaly()


//------------------------------------------------------------------------------
// friend std::ostream& operator<<(std::ostream& output, Keplerian& k)
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& output, Keplerian& k)
{
   Rvector v(6, k.mSemimajorAxis, k.mEccentricity,
             k.mInclination,   k.mRaOfAscendingNode,
             k.mArgOfPeriapsis, k.mTrueAnomaly);

   output << v << std::endl;

   return output;
}


//------------------------------------------------------------------------------
// friend std::istream& operator>>(std::istream& input, Keplerian& k)
//------------------------------------------------------------------------------
std::istream& operator>>(std::istream& input, Keplerian& k)
{
   Rvector ke(6, 0.0);

   input >> ke;
   k.mSemimajorAxis      = ke[0];
   k.mEccentricity       = ke[1];
   k.mInclination        = ke[2];
   k.mRaOfAscendingNode  = ke[3];
   k.mArgOfPeriapsis     = ke[4];
   k.mTrueAnomaly        = ke[5];

   return input;
}


