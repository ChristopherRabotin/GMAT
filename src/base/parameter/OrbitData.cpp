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
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "OrbitData.hpp"
#include "ParameterException.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"
#include "CoordUtil.hpp"         // for Cartesian to Keplerian conversion
#include "AngleUtil.hpp"
#include "UtilityException.hpp"
#include "SphericalTwo.hpp"      // for friend ToSphericalTwo()
#include "CelestialBody.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ORBITDATA 1
//#define DEBUG_ORBITDATA_DETAILS 1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const std::string
OrbitData::VALID_OBJECT_TYPE_LIST[OrbitDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem"
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
   : RefData()
{
   mCartState = Rvector6::RVECTOR6_UNDEFINED;
   mKepState = Rvector6::RVECTOR6_UNDEFINED;
   mSphState = Rvector6::RVECTOR6_UNDEFINED;
   mMA = ORBIT_REAL_UNDEFINED;
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mCartEpoch = 0.0;
   mGravConst = 0.0;
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
   : RefData(data)
{
}

//------------------------------------------------------------------------------
// OrbitData& operator= (const OrbitData& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
OrbitData& OrbitData::operator= (const OrbitData& right)
{
   if (this != &right)
      RefData::operator=(right);

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
   
   Integer id = mSpacecraft->GetParameterID("StateType"); 
   std::string elemType = mSpacecraft->GetStringParameter(id);
   mCartEpoch = mSpacecraft->GetRealParameter("Epoch");
   
#if DEBUG_ORBITDATA
   MessageInterface::ShowMessage
      ("OrbitData::GetCartState() stateType=%s\n", elemType.c_str());
#endif
   
   if (elemType == "Cartesian")
   {
      PropState statePtr = mSpacecraft->GetState(); // should be cartesian state
      for (int i=0; i<6; i++)
         mCartState[i] = statePtr[i];

#if DEBUG_ORBITDATA
      MessageInterface::ShowMessage("OrbitData::GetCartState() scEpoch=%f, mCartEpoch=%f\n"
                                    "state = %f, %f, %f, %f, %f, %f\n",
                                    mSpacecraft->GetRealParameter("Epoch"), mCartEpoch,
                                    mCartState[0], mCartState[1], mCartState[2],
                                    mCartState[3], mCartState[4], mCartState[5]);
#endif
   }
   else if (elemType == "Keplerian")
   {
      Real grav = mGravConst;
      
      PropState statePtr = mSpacecraft->GetState(); // should be keplerian state
      Real kepl[6];
      memcpy(kepl, statePtr.GetState(), 6*sizeof(Real));

      Rvector6 keplState = Rvector6(kepl);
      Rvector6 cartState;

#if DEBUG_ORBITDATA
      MessageInterface::ShowMessage("OrbitData::GetCartState() keplState=%s\n",
                                    keplState.ToString().c_str());
#endif
      
      cartState = KeplerianToCartesian(keplState, grav, CoordUtil::TA);
      mCartState = cartState;
   }
   else
   {
      throw ParameterException
         ("OrbitData::GetCartState() input state types other than Cartesian "
          "and Keplerian are not supported at this time." + elemType + "\n");
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
   
   //loj: 4/19/04 system crashes when I use the new method.
   //Rvector6 mKepState = mSpacecraft->GetKeplerianState();

   Integer id = mSpacecraft->GetParameterID("StateType");
   std::string elemType = mSpacecraft->GetStringParameter(id);

   PropState statePtr = mSpacecraft->GetState(); // should be keplerian state
      
#ifdef DEBUG_ORBITDATA_DETAILS
   PropState statePtr = mSpacecraft->GetState(); // should be keplerian state
   std::cout << "OrbitData::GetKepState for spacecraft " << mSpacecraft->GetName() <<"\n";
   std::cout << "   elemType == " << elemType <<"\n";
   for (int i=0; i<6; i++)
   {
      std::cout << "  el[" << i << "] = " << statePtr[i] << "\n";
   }            
#endif

   if (elemType == "Keplerian")
   {
      PropState statePtr = mSpacecraft->GetState(); // should be keplerian state
            
      for (int i=0; i<6; i++)
      {
#ifdef DEBUG_ORBITDATA_DETAILS
         std::cout << "  el[" << i << "] = " << statePtr[i] << "\n";
#endif
         mKepState[i] = statePtr[i];
      }            
   }
   else if (elemType == "Cartesian")
   {
      PropState statePtr = mSpacecraft->GetState(); // should be cartesian state
      Real cart[6];
      memcpy(cart, statePtr.GetState(), 6*sizeof(Real));
                
      Real grav = mGravConst;
      Rvector6 cartState = Rvector6(cart);
      Rvector6 keplState;

#if DEBUG_ORBITDATA
      MessageInterface::ShowMessage("OrbitData::GetKepState() cartState=%s\n",
                                    cartState.ToString().c_str());
#endif
      
      Real ma;
      keplState = CartesianToKeplerian(cartState, grav, &ma);

      mKepState = keplState;
      mMA = ma;
   }
   else
   {
      throw ParameterException
         ("OrbitData::GetKepState() input state types other than Cartesian "
          "and Keplerian are not supported at this time.\n");
   }
   
   return mKepState;
}

//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetSphState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetSphState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   Integer id = mSpacecraft->GetParameterID("StateType");
   std::string elemType = mSpacecraft->GetStringParameter(id);

   if (elemType == "Cartesian")
   {      
      Rvector6 cartState = mSpacecraft->GetCartesianState(); 

#if DEBUG_ORBITDATA
      MessageInterface::ShowMessage
         ("OrbitData::GetSphState() cartState=%s\n",
          cartState.ToString().c_str());
#endif
      
      // update SphericalTwo state
      SphericalTwo sph2 = ToSphericalTwo(Cartesian(cartState));

#if DEBUG_ORBITDATA
      MessageInterface::ShowMessage
         ("OrbitData::GetSphState() sph2=%s\n",
          sph2.ToString().c_str());
#endif
      
      mSphState[0] = sph2.GetPositionMagnitude();
      mSphState[1] = sph2.GetRightAscension();
      mSphState[2] = sph2.GetDeclination();
      mSphState[3] = sph2.GetVelocityMagnitude();
      mSphState[4] = sph2.GetVelocityRA();
      mSphState[5] = sph2.GetVelocityDeclination();
   }
   else
   {
      throw ParameterException
         ("OrbitData::GetSphState() input state types other than Cartesian "
          "are not supported at this time\n");
   }

   return mSphState;
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
// Real GetKepReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Keplerian element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetKepReal(const std::string &str)
{
   Rvector6 state = GetKepState();

   if (str == "KepSMA")
      return mKepState[SMA];
   else if (str == "KepEcc")
      return mKepState[ECC];
   else if (str == "KepInc")
      return mKepState[INC];
   else if (str == "KepRAAN")
      return mKepState[RAAN];
   else if (str == "KepRADN")
      return AngleUtil::PutAngleInDegRange(mKepState[RAAN] + 180, 0.0, 360.0);
   else if (str == "KepAOP")
      return mKepState[AOP];
   else if (str == "KepTA")
      return mKepState[TA];
   else if (str == "KepMA")
      return mMA;
   else
      throw ParameterException("OrbitData::GetCartReal() Unknown parameter name: " +
                               str);
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

   Real sma = GetKepReal("KepSMA");
   Real ecc = GetKepReal("KepEcc");
   
   Real grav = mGravConst;
   Real E, R;
        
   if (str == "KepMM")
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
   else if (str == "VelApoapsis")
   {
      E = -grav / (2.0 * sma);
      R = sma * (1.0 + ecc);
      return Sqrt (2.0 * (E + grav/R));
   }
   else if (str == "VelPeriapsis")
   {
      E = -grav / (2.0 * sma);
      R = sma * (1.0 - ecc);
      return Sqrt (2.0 * (E + grav/R));
   }
   else if (str == "OrbitPeriod")
   {
      return GmatMathUtil::TWO_PI * Sqrt((sma * sma * sma)/ grav);
   }
   else if (str == "RadApoapsis")
   {
      return sma * (1.0 + ecc);
   }
   else if (str == "RadPeriapsis")
   {
      return sma * (1.0 - ecc);
   }
   else if (str == "C3Energy")
   {
      return -grav / sma;
   }
   else if (str == "Energy")
   {
      return -grav / (2.0 * sma);
   }
   else
   {
      throw ParameterException
         ("OrbitData::GetOtherKepReal() Unknown parameter name: " + str);
   }
}

//------------------------------------------------------------------------------
// Real GetSphReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Spherical element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetSphReal(const std::string &str)
{
   Rvector6 state = GetSphState();

#if DEBUG_ORBITDATA
   MessageInterface::ShowMessage
      ("OrbitData::GetSphReal() str=%s state=%s\n",
       str.c_str(), state.ToString().c_str());
#endif
   
   if (str == "SphRMag")
      return mSphState[RMAG];
   else if (str == "SphRA")
      return mSphState[RRA];
   else if (str == "SphDec")
      return mSphState[RDEC];
   else if (str == "SphVMag")
      return mSphState[VMAG];
   else if (str == "SphRAV")
      return mSphState[RAV];
   else if (str == "SphDecV")
      return mSphState[DECV];
   else if (str == "Altitude") //loj: 11/5/04 added
   {
      //loj: Do I need to use other radius?
      return mSphState[RMAG] - mCentralBody->GetEquatorialRadius();
   }
   else
   {
      throw ParameterException("OrbitData::GetSphReal() Unknown parameter name: " +
                               str);
   }
}

//------------------------------------------------------------------------------
// Real GetAngularReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives angular related element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetAngularReal(const std::string &str)
{    
   Rvector6 state = GetCartState();
   Rvector3 pos = Rvector3(state[0], state[1], state[2]);
   Rvector3 vel = Rvector3(state[3], state[4], state[5]);
              
   Rvector3 hVec3 = Cross(pos, vel);
   Real h = Sqrt(hVec3 * hVec3);
        
   Real grav = mGravConst;
   
   if (str == "SemilatusRectum")
   {
      if (h < ORBIT_TOL)
         return 0.0;
      else
         return (h / grav) * h;      // B M W; eq. 1.6-1
   }
   else if (str == "HMAG") //loj: 12/8/04 - added HMAG, HX, HY, HZ
   {
      return h; 
   }
   else if (str == "HX")
   {
      return hVec3[0];
   }
   else if (str == "HY")
   {
      return hVec3[1];
   }
   else if (str == "HZ")
   {
      return hVec3[2];
   }
   else
   {
      throw ParameterException("OrbitData::GetAngularReal() Unknown parameter name: " +
                               str);
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

//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void OrbitData::InitializeRefObjects()
{
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);

   if (mSpacecraft == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set to any unnamed parameters\n");
   
   mSolarSystem = (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   if (mSolarSystem == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   Integer id = mSpacecraft->GetParameterID("ReferenceBody");
   std::string bodyName = mSpacecraft->GetStringParameter(id);
   mCentralBody = mSolarSystem->GetBody(bodyName);
   
   if (!mCentralBody)
      throw ParameterException("OrbitData::GetCartState() Body not found in the "
                               "SolarSystem: " + bodyName + "\n");

   //assume gravitational constant doesn't change through the mission run
   mGravConst = mCentralBody->GetGravitationalConstant();
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

