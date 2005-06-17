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
// Created: 2005/06/16
//
/**
 * Implements B-Plane related data class.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "BplaneData.hpp"
#include "ParameterException.hpp"
#include "Rvector3.hpp"
#include "RealUtilities.hpp"   // for Sqrt()
#include "UtilityException.hpp"
#include "CelestialBody.hpp"
#include "Linear.hpp"           // for GmatRealUtil::ToString()
#include "MessageInterface.hpp"

//#define DEBUG_BPLANE_DATA_INIT 1
//#define DEBUG_BPLANE_DATA_CONVERT 1
//#define DEBUG_BPLANE_DATA_RUN 1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const std::string
BplaneData::VALID_OBJECT_TYPE_LIST[BplaneDataObjectCount] =
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
// BplaneData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
BplaneData::BplaneData()
   : RefData()
{
   mCartState = Rvector6::RVECTOR6_UNDEFINED;
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mScOrigin = NULL;
   mOrigin = NULL;
   mInternalCoordSystem = NULL;
   mOutCoordSystem = NULL;
   mCartEpoch = 0.0;
   mGravConst = 0.0;
}


//------------------------------------------------------------------------------
// BplaneData(const BplaneData &data)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <data> the BplaneData object being copied.
 */
//------------------------------------------------------------------------------
BplaneData::BplaneData(const BplaneData &data)
   : RefData(data)
{
}


//------------------------------------------------------------------------------
// BplaneData& operator= (const BplaneData& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
BplaneData& BplaneData::operator= (const BplaneData& right)
{
   if (this != &right)
      RefData::operator=(right);

   return *this;
}


//------------------------------------------------------------------------------
// ~BplaneData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
BplaneData::~BplaneData()
{
}


//------------------------------------------------------------------------------
// Real GetBplaneReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Computes B-Plane related paramters.
 *
 * @note - Implements GMAT math Spec 2.11 B-Plane Coordinates
 */
//------------------------------------------------------------------------------
Real BplaneData::GetBplaneReal(const std::string &str)
{
   if (mOutCoordSystem == NULL || mOrigin == NULL)
      InitializeRefObjects();
   
   // If the selected coordinate system does not have a celestial body as
   // its origin, then throw an exception
   if (mOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      mGravConst = ((CelestialBody*)mOrigin)->GetGravitationalConstant();
   else
      throw ParameterException
         ("BplaneData::GetBplaneReal() Cannot compute B-Plane "
          "parameter for CoordinateSystem that has non-celestial body as origin.  "
          "CoordinateSystem: " + mOutCoordSystem->GetName() + "  Origin: " +
          mOrigin->GetName());
   
   Rvector6 state = GetCartState();
   
   #if DEBUG_BPLANE_DATA_RUN
   MessageInterface::ShowMessage
      ("BplaneData::GetBplaneReal() str = %s, mGravConst = %f\n     state = %s\n",
       str.c_str(), mGravConst, state.ToString().c_str());
   #endif
   
   Rvector3 pos(state[0], state[1], state[2]);
   Rvector3 vel(state[3], state[4], state[5]);
   
   Real rMag = pos.GetMagnitude();
   Real vMag = vel.GetMagnitude();
   
   // Compute eccentricity related information   
   Rvector3 eVec =
      ((vMag*vMag - mGravConst/rMag) * pos - (pos*vel)*vel) / mGravConst;
   
   Real eMag = eVec.GetMagnitude();
   
   // if eMag <= 1, then the method fails, orbit should be hyperbolic
   if (eMag <= 1.0)
      throw ParameterException
         ("BplaneData::GetBplaneReal() eccentricity magnitude is <= 1.0. "
          "eMag: " + GmatRealUtil::ToString(eMag));
   
   eVec.Normalize();
   
   // Compute the angular momentum and orbit normal vectors
   Rvector3 hVec = Cross(pos, vel);
   Real hMag = hVec.GetMagnitude();
   hVec.Normalize();
   Rvector3 nVec = Cross(hVec, eVec);
   
   #if DEBUG_BPLANE_DATA_RUN
   MessageInterface::ShowMessage
      ("BplaneData::GetBplaneReal()\n     eVec = %s\n     hVec = %s\n     nVec = %s\n",
       eVec.ToString().c_str(), hVec.ToString().c_str(), nVec.ToString().c_str());
   #endif
      
   // Compute semiminor axis, b
   Real b = (hMag*hMag) / (mGravConst * Sqrt(eMag*eMag - 1.0));
   
   // Compute incoming asymptote
   Real oneOverEmag = 1.0/eMag;
   Real temp = Sqrt(1.0 - oneOverEmag*oneOverEmag);
   Rvector3 sVec = (eVec/eMag) + (temp*nVec);
   
   // Compute the B-vector
   Rvector3 bVec = b * (temp * eVec - oneOverEmag*nVec);
   
   #if DEBUG_BPLANE_DATA_RUN
   MessageInterface::ShowMessage
      ("BplaneData::GetBplaneReal() b = %f\n     sVec = %s\n     bVec = %s\n",
       b, sVec.ToString().c_str(), bVec.ToString().c_str());
   #endif
   
   // Compute T and R vector
   Rvector3 sVec1(sVec[1], -sVec[0], 0.0);
   Rvector3 tVec = sVec1 / Sqrt(sVec[0]*sVec[0] + sVec[1]*sVec[1]);
   Rvector3 rVec = Cross(sVec, tVec);

   #if DEBUG_BPLANE_DATA_RUN
   MessageInterface::ShowMessage
      ("BplaneData::GetBplaneReal()\n     sVec1 = %s\n     tVec = %s\n     rVec = %s\n",
       sVec1.ToString().c_str(), tVec.ToString().c_str(), rVec.ToString().c_str());
   #endif
   
   if (str == "BdotT")
      return bVec * tVec;
   else if (str == "BdotR")
      return bVec * rVec;
   else
   {
      throw ParameterException
         ("BplaneData::GetBplaneReal() Unknown parameter name: " + str);
   }
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* BplaneData::GetValidObjectList() const
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
bool BplaneData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<BplaneDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == BplaneDataObjectCount)
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
void BplaneData::InitializeRefObjects()
{
   #if DEBUG_BPLANE_DATA_INIT
   MessageInterface::ShowMessage
      ("BplaneData::InitializeRefObjects() entered.\n");
   #endif
   
   mSpacecraft =
      (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);

   if (mSpacecraft == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set to any unnamed parameters\n");
   
   mSolarSystem =
      (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   
   if (mSolarSystem == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   if (mInternalCoordSystem == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find internal "
          "CoordinateSystem object\n");
   
   mOutCoordSystem =
      (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   
   if (mOutCoordSystem == NULL)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() Cannot find output "
          "CoordinateSystem object\n");
   
   // get spacecraft CoordinateSystem
   std::string csName = mSpacecraft->GetRefObjectName(Gmat::COORDINATE_SYSTEM);   
   CoordinateSystem *cs = (CoordinateSystem*)mSpacecraft->
      GetRefObject(Gmat::COORDINATE_SYSTEM, csName);
   
   if (!cs)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() spacecraft CoordinateSystem not "
          "found: " + csName + "\n");
   
   mScOrigin = cs->GetOrigin();
   
   if (!mScOrigin)
      throw ParameterException
         ("BplaneData::InitializeRefObjects() spacecraft origin not found: " +
          cs->GetOriginName() + "\n");
   
   // get gravity constant if spacecraft origin is CelestialBody
   if (mScOrigin->IsOfType(Gmat::CELESTIAL_BODY))
      mGravConst = ((CelestialBody*)mScOrigin)->GetGravitationalConstant();

   mOrigin = mOutCoordSystem->GetOrigin();
         
   #if DEBUG_BPLANE_DATA_INIT
   MessageInterface::ShowMessage
      ("BplaneData::InitializeRefObjects() mScOrignName=%s, mOriginName=%s\n",
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
bool BplaneData::IsValidObjectType(Gmat::ObjectType type)
{
   for (int i=0; i<BplaneDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystem()
//------------------------------------------------------------------------------
SolarSystem* BplaneData::GetSolarSystem()
{
   return mSolarSystem;
}


//------------------------------------------------------------------------------
// CoordinateSystem* GetInternalCoordSys()
//------------------------------------------------------------------------------
CoordinateSystem* BplaneData::GetInternalCoordSys()
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
void BplaneData::SetInternalCoordSys(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}


//------------------------------------------------------------------------------
// Rvector6 GetCartState()
//------------------------------------------------------------------------------
Rvector6 BplaneData::GetCartState()
{
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   Integer id = mSpacecraft->GetParameterID("StateType"); 
   std::string elemType = mSpacecraft->GetStringParameter(id);
   mCartEpoch = mSpacecraft->GetRealParameter("Epoch");
   
   #if DEBUG_BPLANE_DATA_RUN
   MessageInterface::ShowMessage
      ("BplaneData::GetCartState() stateType=%s, internalCoordName=%s\n"
       "     outCoordName=%s\n", elemType.c_str(),
       mInternalCoordSystem->GetName().c_str(),
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
            ("BplaneData::GetCartState() Internal CoordSystem or Output "
             "CoordSystem is NULL.\n");
         
         throw ParameterException
            ("BplaneData::GetCartState() internal or output CoordinateSystem "
             "is NULL.\n");
      }
      
      // convert to output CoordinateSystem
      if (mInternalCoordSystem->GetName() != mOutCoordSystem->GetName())
      {
         #if DEBUG_BPLANE_DATA_CONVERT
         MessageInterface::ShowMessage
            ("BplaneData::GetCartState() ===> mOutCoordSystem:%s Axis=%s\n",
             mOutCoordSystem->GetName().c_str(),
             mOutCoordSystem->GetRefObject(Gmat::AXIS_SYSTEM, "")->GetName().c_str());
         MessageInterface::ShowMessage
            ("BplaneData::GetCartState() Before convert: mCartEpoch=%f\n"
             "state = %s\n", mCartEpoch, mCartState.ToString().c_str());
         #endif
         
         try
         {
            mCoordConverter.Convert(A1Mjd(mCartEpoch), mCartState,
                                    mInternalCoordSystem,
                                    mCartState, mOutCoordSystem);
            
            #if DEBUG_BPLANE_DATA_CONVERT
               MessageInterface::ShowMessage
                  ("BplaneData::GetCartState() --After convert: mCartEpoch=%f\n"
                   "     state = %s\n", mCartEpoch, mCartState.ToString().c_str());
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

      #if DEBUG_BPLANE_DATA_RUN
      MessageInterface::ShowMessage("BplaneData::GetCartState() keplState=%s\n",
                                    keplState.ToString().c_str());
      #endif
      
      mCartState = KeplerianToCartesian(keplState, grav, CoordUtil::TA);
   }
   else
   {
      throw ParameterException
         ("BplaneData::GetCartState() input state types other than Cartesian "
          "and Keplerian are not supported at this time." + elemType + "\n");
   }
   
   return mCartState;
}


