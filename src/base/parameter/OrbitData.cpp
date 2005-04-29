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
#include "SphericalRADEC.hpp"    // for friend CartesianToSphericalRADEC/AZFPA()
#include "ModKeplerian.hpp"      // for friend KeplerianToModKeplerian()
#include "CelestialBody.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ORBITDATA_INIT 1
//#define DEBUG_ORBITDATA_CONVERT 1
//#define DEBUG_ORBITDATA_RUN 1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

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
   : RefData()
{
   mCartState = Rvector6::RVECTOR6_UNDEFINED;
   mKepState = Rvector6::RVECTOR6_UNDEFINED;
   mModKepState = Rvector6::RVECTOR6_UNDEFINED;
   mSphRaDecState = Rvector6::RVECTOR6_UNDEFINED;
   mSphAzFpaState = Rvector6::RVECTOR6_UNDEFINED;
   mMA = ORBIT_REAL_UNDEFINED;
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mScOrigin = NULL;
   mOrigin = NULL; //loj: 4/7/05 Added
   mInternalCoordSystem = NULL;
   mOutCoordSystem = NULL;
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
   
   #if DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetCartState() stateType=%s, internalCoordName=%s, outCoordName=%s\n",
       elemType.c_str(), mInternalCoordSystem->GetName().c_str(),
       mOutCoordSystem->GetName().c_str());
   #endif
   
   if (elemType == "Cartesian")
   {
      PropState ps = mSpacecraft->GetState(); // should be cartesian state
      for (int i=0; i<6; i++)
         mCartState[i] = ps[i];
      
      if (mInternalCoordSystem == NULL || mOutCoordSystem == NULL)
      {
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() Internal CoordSystem or Output CoordSystem is NULL.\n");
         
         throw ParameterException
            ("OrbitData::GetCartState() internal or output CoordinateSystem is NULL.\n");
      }
      
      // convert to output CoordinateSystem
      if (mInternalCoordSystem->GetName() != mOutCoordSystem->GetName())
      {
         #if DEBUG_ORBITDATA_CONVERT
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() ===> mOutCoordSystem:%s Axis=%s\n",
             mOutCoordSystem->GetName().c_str(),
             mOutCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, "")->GetName().c_str());
         MessageInterface::ShowMessage
            ("OrbitData::GetCartState() Before convert: mCartEpoch=%f\nstate = %s\n", 
             mCartEpoch, mCartState.ToString().c_str());
         #endif

         try
         {
            mCoordConverter.Convert(A1Mjd(mCartEpoch), mCartState, mInternalCoordSystem,
                                    mCartState, mOutCoordSystem);
            #if DEBUG_ORBITDATA_CONVERT
               MessageInterface::ShowMessage
                  ("OrbitData::GetCartState() --After convert: mCartEpoch=%f\n"
                   "state = %s\n", mCartEpoch, mCartState.ToString().c_str());
             #endif
         }
         catch (BaseException &e)
         {
            MessageInterface::ShowMessage(e.GetMessage());
         }
      }
   }
   else if (elemType == "Keplerian")
   {
      Real grav = mGravConst;
      
      PropState ps = mSpacecraft->GetState(); // should be keplerian state
      Real kepl[6];
      memcpy(kepl, ps.GetState(), 6*sizeof(Real));

      Rvector6 keplState = Rvector6(kepl);

      #if DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage("OrbitData::GetCartState() keplState=%s\n",
                                    keplState.ToString().c_str());
      #endif
      
      mCartState = KeplerianToCartesian(keplState, grav, CoordUtil::TA);
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
      
   Integer id = mSpacecraft->GetParameterID("StateType");
   std::string elemType = mSpacecraft->GetStringParameter(id);
      
   #ifdef DEBUG_ORBITDATA_RUN
   PropState ps = mSpacecraft->GetState(); // should be cartesian state
   std::cout << "OrbitData::GetKepState for spacecraft " << mSpacecraft->GetName() <<"\n";
   std::cout << "   elemType == " << elemType <<"\n";
   for (int i=0; i<6; i++)
   {
      std::cout << "  el[" << i << "] = " << ps[i] << "\n";
   }            
   #endif

   if (elemType == "Keplerian")
   {
      PropState ps = mSpacecraft->GetState(); // should be keplerian state
            
      for (int i=0; i<6; i++)
      {
         #ifdef DEBUG_ORBITDATA_RUN
         std::cout << "  el[" << i << "] = " << ps[i] << "\n";
         #endif
         mKepState[i] = ps[i];
      }            
   }
   else if (elemType == "Cartesian")
   {
      // call GetCartState() since it provides the Coord Sys conversion
      Rvector6 cartState = GetCartState();
      
      Real grav = mGravConst;
      
      #if DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage("OrbitData::GetKepState() cartState=%s\n",
                                    cartState.ToString().c_str());
      #endif
      
      Real ma;
      mKepState = CartesianToKeplerian(cartState, grav, &ma);
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
// Rvector6 GetModKepState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetModKepState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   Integer id = mSpacecraft->GetParameterID("StateType");
   std::string elemType = mSpacecraft->GetStringParameter(id);
   
   #ifdef DEBUG_ORBITDATA_RUN
   PropState ps = mSpacecraft->GetState(); // should be cartesian state
   std::cout << "OrbitData::GetModKepState for spacecraft " << mSpacecraft->GetName() <<"\n";
   std::cout << "   elemType == " << elemType <<"\n";
   for (int i=0; i<6; i++)
   {
      std::cout << "  el[" << i << "] = " << ps[i] << "\n";
   }            
   #endif
   
   if (elemType == "Keplerian")
   {
      PropState ps = mSpacecraft->GetState(); // should be keplerian state
      
      for (int i=0; i<6; i++)
      {
         #ifdef DEBUG_ORBITDATA_RUN
         std::cout << "  el[" << i << "] = " << ps[i] << "\n";
         #endif
         mKepState[i] = ps[i];
      }

      mModKepState = KeplerianToModKeplerian(mKepState);
   }
   else if (elemType == "Cartesian")
   {
      // call GetCartState() since it provides the Coord Sys conversion
      Rvector6 cartState = GetCartState();
      
      Real grav = mGravConst;
      
      #if DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage("OrbitData::GetModKepState() cartState=%s\n",
                                    cartState.ToString().c_str());
      #endif
      
      Real ma;
      mKepState = CartesianToKeplerian(cartState, grav, &ma);
      mModKepState = KeplerianToModKeplerian(mKepState);
      mMA = ma;
   }
   else
   {
      throw ParameterException
         ("OrbitData::GetModKepState() input state types other than Cartesian "
          "and Keplerian are not supported at this time.\n");
   }
   
   return mModKepState;
}


//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetSphRaDecState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetSphRaDecState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   Integer id = mSpacecraft->GetParameterID("StateType");
   std::string elemType = mSpacecraft->GetStringParameter(id);

   if (elemType == "Cartesian")
   {
      //Call GetCartState() since it provides the Coord System conversion
      Rvector6 cartState = GetCartState();
      
      #if DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage
         ("OrbitData::GetSphRaDecState() cartState=%s\n",
          cartState.ToString().c_str());
      #endif
      
      // update SphericalRADEC state
      mSphRaDecState = CartesianToSphericalRADEC(cartState);

      #if DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage
         ("OrbitData::GetSphRaDecState() mSphRaDecState=%s\n",
          mSphRaDecState.ToString().c_str());
      #endif
      
   }
   else
   {
      throw ParameterException
         ("OrbitData::GetSphRaDecState() input state types other than Cartesian "
          "are not supported at this time\n");
   }

   return mSphRaDecState;
}


//------------------------------------------------------------------------------
// Rvector6 OrbitData::GetSphAzFpaState()
//------------------------------------------------------------------------------
Rvector6 OrbitData::GetSphAzFpaState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   Integer id = mSpacecraft->GetParameterID("StateType");
   std::string elemType = mSpacecraft->GetStringParameter(id);

   if (elemType == "Cartesian")
   {
      // call GetCartState() since it provides the Coord. System conversion
      Rvector6 cartState = GetCartState();
      
      #if DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage
         ("OrbitData::GetSphAzFpaState() cartState=%s\n",
          cartState.ToString().c_str());
      #endif
      
      // update SphericalAZFPA state
      mSphAzFpaState = CartesianToSphericalAZFPA(cartState);
      
      #if DEBUG_ORBITDATA_RUN
      MessageInterface::ShowMessage
         ("OrbitData::GetSphAzFpaState() mSphAzFpaState=%s\n",
          mSphAzFpaState.ToString().c_str());
      #endif
      
   }
   else
   {
      throw ParameterException
         ("OrbitData::GetSphAzFpaState() input state types other than Cartesian "
          "are not supported at this time\n");
   }

   return mSphAzFpaState;
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

   //loj: 4/6/05 for efficiency, just call GetKepState()
   //Real sma = GetKepReal("KepSMA");
   //Real ecc = GetKepReal("KepEcc");
   Rvector6 state = GetKepState();
   Real sma = state[SMA];
   Real ecc = state[ECC];
   
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
// Real GetSphRaDecReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives Spherical element.
 */
//------------------------------------------------------------------------------
Real OrbitData::GetSphRaDecReal(const std::string &str)
{
   Rvector6 state = GetSphRaDecState();

   #if DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetSphRaDecReal() str=%s state=%s\n",
       str.c_str(), state.ToString().c_str());
   #endif
   
   if (str == "SphRMag")
   {
      // if orgin is the same as central body, just return default
      if (mOrigin->GetName() == mScOrigin->GetName())
         return mSphRaDecState[RD_RMAG];
      else
         return GetPositionMagnitude(mOrigin);

   }
   else if (str == "SphRA")
      return mSphRaDecState[RD_RRA];
   else if (str == "SphDec")
      return mSphRaDecState[RD_RDEC];
   else if (str == "SphVMag")
      return mSphRaDecState[RD_VMAG];
   else if (str == "SphRAV")
      return mSphRaDecState[RD_RAV];
   else if (str == "SphDecV")
      return mSphRaDecState[RD_DECV];
   else if (str == "Altitude")
   {
      // if orgin is the same as central body, just return default
      if (mOrigin->GetName() == mScOrigin->GetName())
      {
         return mSphRaDecState[RD_RMAG] -
            ((CelestialBody*)mScOrigin)->GetEquatorialRadius();
      }
      else
      {
         Real rmag = GetPositionMagnitude(mOrigin);
         return rmag - ((CelestialBody*)mOrigin)->GetEquatorialRadius();
      }
   }
   else
   {
      throw ParameterException
         ("OrbitData::GetSphRaDecReal() Unknown parameter name: " + str);
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
   Rvector6 state = GetSphAzFpaState();

   #if DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetSphAzFpaReal() str=%s state=%s\n",
       str.c_str(), state.ToString().c_str());
   #endif
   
   if (str == "SphRMag")
      return mSphAzFpaState[AF_RMAG];
   else if (str == "SphRA")
      return mSphAzFpaState[AF_RRA];
   else if (str == "SphDec")
      return mSphAzFpaState[AF_RDEC];
   else if (str == "SphVMag")
      return mSphAzFpaState[AF_VMAG];
   else if (str == "SphAzi")
      return mSphAzFpaState[AF_AZI];
   else if (str == "SphFPA")
      return mSphAzFpaState[AF_FPA];
   else
   {
      throw ParameterException("OrbitData::GetSphAzFpaReal() Unknown parameter name: " +
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

   #if DEBUG_ORBITDATA_RUN
   MessageInterface::ShowMessage
      ("OrbitData::GetAngularReal() str=%s state=%s\n",
       str.c_str(), state.ToString().c_str());
   #endif
   
   Rvector3 pos;
   Rvector3 vel;

   pos = Rvector3(state[0], state[1], state[2]);
   vel = Rvector3(state[3], state[4], state[5]);
   
   Rvector3 hVec3 = Cross(pos, vel);
   Real h = Sqrt(hVec3 * hVec3);
   
   Real grav = mGravConst;
   
   if (str == "SemilatusRectum")
   {
      if (mOrigin->GetName() != mScOrigin->GetName())
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
   else if (str == "HMAG")
   {
      if (mOrigin->GetName() != mScOrigin->GetName())
      {
         state = GetRelativeCartState(mOrigin);
         pos = Rvector3(state[0], state[1], state[2]);
         vel = Rvector3(state[3], state[4], state[5]);
         hVec3 = Cross(pos, vel);
         h = Sqrt(hVec3 * hVec3);
      }
      
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

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void OrbitData::InitializeRefObjects()
{
   #if DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage
      ("OrbitData::InitializeRefObjects() entered.\n");
   #endif
   
   mSpacecraft =
      (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);

   if (mSpacecraft == NULL)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set to any unnamed parameters\n");

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

   //loj: 4/28/05 old code
   //Integer id = mSpacecraft->GetParameterID("CoordinateSystem");
   //std::string bodyName = mSpacecraft->GetStringParameter(id);
   //mScOrigin = mSolarSystem->GetBody(bodyName);
   
   // get spacecraft CoordinateSystem
   std::string csName = mSpacecraft->GetRefObjectName(Gmat::COORDINATE_SYSTEM);   
   CoordinateSystem *cs = (CoordinateSystem*)mSpacecraft->
      GetRefObject(Gmat::COORDINATE_SYSTEM, csName);

   if (!cs)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() spacecraft CoordinateSystem not "
          "found: " + csName + "\n");
   
   mScOrigin = cs->GetOrigin();
   
   if (!mScOrigin)
      throw ParameterException
         ("OrbitData::InitializeRefObjects() spacecraft origin not found: " +
          cs->GetOriginName() + "\n");
   
   // get gravity constant if spacecraft origin is CelestialBody
   if (mScOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      mGravConst = ((CelestialBody*)mScOrigin)->GetGravitationalConstant();

   //-----------------------------------------------------------------
   // if dependent body name exist and it is a CelestialBody,
   // set new gravity constant
   //-----------------------------------------------------------------
   std::string originName =
      FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[SPACE_POINT]));

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
      if (mOrigin->GetType() == Gmat::CELESTIAL_BODY)
         mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
   }
   else
   {
      mOrigin = mScOrigin;
   }
   
   #if DEBUG_ORBITDATA_INIT
   MessageInterface::ShowMessage
      ("OrbitData::InitializeRefObjects() mScOrignName=%s, mOriginName=%s\n",
       mScOrigin->GetName().c_str(), mOrigin->GetName().c_str());
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


//loj: 4/7/05 Added
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
   //Rvector6 originState = origin->GetState(mCartEpoch);
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
