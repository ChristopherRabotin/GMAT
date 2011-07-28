//$Id$
//------------------------------------------------------------------------------
//                                  PlanetData
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
// Created: 2004/12/13
// Modified:
//   2005/6/10 Linda Jun - Moved BetaAngle to AngularParamters.cpp
//
/**
 * Implements class which provides planet related data, such as HourAngle.
 *   MHA, Longitude, Altitude(Geodetic), Latitude(Geodetic), LST
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "PlanetData.hpp"
#include "ParameterException.hpp"
#include "GmatConstants.hpp"
#include "CelestialBody.hpp"
#include "AngleUtil.hpp"          // for PutAngleInDegRange()
#include "Linear.hpp"             // for GmatRealUtil::ToString()
#include "MessageInterface.hpp"

//#define __COMPUTE_LONGITUDE_OLDWAY__

//#define DEBUG_PLANETDATA_INIT
//#define DEBUG_PLANETDATA_RUN
//#define DEBUG_ALTITUDE

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const Real PlanetData::PLANET_REAL_UNDEFINED = -9876543210.1234;

const std::string
PlanetData::VALID_OBJECT_TYPE_LIST[PlanetDataObjectCount] =
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
// PlanetData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
PlanetData::PlanetData()
   : RefData()
{
   mCentralBodyName = "";
   
   mSpacecraft = NULL;
   mSolarSystem = NULL;
   mCentralBody = NULL;
   mOrigin = NULL;
   mInternalCoordSystem = NULL;
   mOutCoordSystem = NULL;
}


//------------------------------------------------------------------------------
// PlanetData(const PlanetData &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <copy> the PlanetData object being copied.
 */
//------------------------------------------------------------------------------
PlanetData::PlanetData(const PlanetData &copy)
   : RefData(copy)
{
   mCentralBodyName = copy.mCentralBodyName;
   mSpacecraft = copy.mSpacecraft;
   mSolarSystem = copy.mSolarSystem;
   mCentralBody = copy.mCentralBody;
   mOrigin = copy.mOrigin;
   mInternalCoordSystem = copy.mInternalCoordSystem;
   mOutCoordSystem = copy.mOutCoordSystem;
}


//------------------------------------------------------------------------------
// PlanetData& operator= (const PlanetData &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
PlanetData& PlanetData::operator= (const PlanetData &right)
{
   if (this == &right)
      return *this;

   RefData::operator=(right);
      
   mCentralBodyName = right.mCentralBodyName;
   mSpacecraft = right.mSpacecraft;
   mSolarSystem = right.mSolarSystem;
   mCentralBody = right.mCentralBody;
   mOrigin = right.mOrigin;
   mInternalCoordSystem = right.mInternalCoordSystem;
   mOutCoordSystem = right.mOutCoordSystem;
   
   return *this;
}


//------------------------------------------------------------------------------
// ~PlanetData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PlanetData::~PlanetData()
{
}


//------------------------------------------------------------------------------
// Real GetPlanetReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves planet related parameters.
 */
//------------------------------------------------------------------------------
Real PlanetData::GetPlanetReal(Integer item)
{
   #ifdef DEBUG_PLANETDATA_RUN
   MessageInterface::ShowMessage("PlanetData::GetPlanetReal() item=%d\n", item);
   #endif
   
   if (item < LATITUDE || item > LST_ID)
      throw ParameterException
         ("PlanetData::GetPlanetReal() Unknown parameter ID: " +
          GmatRealUtil::ToString(item));
   
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();
   
   switch (item)
   {
   case MHA_ID:
      {
         // Get current time
         Real a1mjd = mSpacecraft->GetEpoch();
         
         // Call GetHourAngle() on origin
         Real mha = mOrigin->GetHourAngle(a1mjd);
         
         #ifdef DEBUG_PLANETDATA_RUN
         MessageInterface::ShowMessage
            ("PlanetData::GetPlanetReal() a1mdj=%f, origin=%s, mha=%f\n", a1mjd,
             mOrigin->GetName().c_str(), mha);
         #endif
      
         return mha;
      }
   case LONGITUDE:
      {
         #ifdef __COMPUTE_LONGITUDE_OLDWAY__
         //==============================================================
         
         // Get spacecraft RightAscension
         Rvector6 state = mSpacecraft->GetState().GetState();
         Real a1mjd = mSpacecraft->GetEpoch();
         
         if (mOrigin->GetName() != "Earth")
         {
            // Get current state
            state = state - mOrigin->GetMJ2000State(a1mjd);
         }
         
         Real raDeg = GmatMathUtil::ATan(state[1], state[0]) * DEG_PER_RAD;
         Real mha = mOrigin->GetHourAngle(a1mjd);
         
         // Compute east longitude
         // Longitude is measured positive to east from prime meridian.
         // (Greenwich for Earth)
         // Reference: Valado 3.2.1 Location Parameters
         Real longitude = raDeg - mha;
         longitude = AngleUtil::PutAngleInDegRange(longitude, -180.0, 180.0);
         
         #ifdef DEBUG_PLANETDATA_RUN
         MessageInterface::ShowMessage
            ("PlanetData::GetPlanetReal(%d) raDeg=%f, mha=%f, longitude=%f\n",
             item, raDeg, mha, longitude);
         #endif
         
         #else
         //==============================================================
         // 1)  Get state in the body fixed system of the central body
         //     defined by sc.centralbody.Longitude. so, if the central
         //     body is Mars, we convert the state to MarsFixed,
         //     if the central body is Venus, we convert to Venus fixed.
         //     This gives us X_f, Y_f, and Z_f,  the Cartesian coordinates
         //     in the fixed system.
         // 2)  Longitude = atan2( Y_f, X_f )
         
         Real epoch = mSpacecraft->GetEpoch();
         Rvector6 state = mSpacecraft->GetState().GetState();
         Rvector6 outState;
         mCoordConverter.Convert(A1Mjd(epoch), state, mInternalCoordSystem,
                                 outState, mOutCoordSystem);
         
         Real longitude = ATan(outState[1], outState[0]) * GmatMathConstants::DEG_PER_RAD;
         longitude = AngleUtil::PutAngleInDegRange(longitude, -180.0, 180.0);

         #ifdef DEBUG_LONGITUDE
         MessageInterface::ShowMessage
            ("PlanetData::GetPlanetReal(%d) longitude=%f\n\n", item, longitude);
         #endif
         
         #endif
         //==============================================================
      
         return longitude;
      }
   case LATITUDE:
   case ALTITUDE:
      {
         Rvector6 intState = mSpacecraft->GetState().GetState();
         
         #ifdef DEBUG_ALTITUDE
         MessageInterface::ShowMessage
            ("PlanetData::GetPlanetReal(%d) state=%s\n", item,
                  intState.ToString().c_str());
         #endif
         
         Real epoch = mSpacecraft->GetRealParameter("A1Epoch");
         Rvector6 instate = mSpacecraft->GetState().GetState();
         Rvector6 state;
         mCoordConverter.Convert(A1Mjd(epoch), instate, mInternalCoordSystem,
                                 state, mOutCoordSystem);
         
         // get flattening for the body
         Real flatteningFactor =
            mOrigin->GetRealParameter(mOrigin->GetParameterID("Flattening"));
         
         // Reworked to match Vallado algorithm 12 (Vallado, 2nd ed, p. 177)
         
         // Note -- using cmath here because I know it better -- may want to change
         // to GmatMath
         Real equatorialRadius =
            mOrigin->GetRealParameter(mOrigin->GetParameterID("EquatorialRadius"));
         
         Real rxy = sqrt(state[0]*state[0] + state[1]*state[1]);
         Real geolat = atan2(state[2], rxy);
         Real delta = 1.0;
         Real geodeticTolerance = 1.0e-7;    // Better than 0.0001 degrees
         Real ecc2 = 2.0 * flatteningFactor - flatteningFactor*flatteningFactor;
         
         Real cFactor, oldlat, sinlat;
         while (delta > geodeticTolerance)
         {
            oldlat = geolat;
            sinlat = sin(oldlat);
            cFactor = equatorialRadius / sqrt(1.0 - ecc2 * sinlat * sinlat);
            geolat = atan2(state[2] + cFactor*ecc2*sinlat, rxy);
            delta = fabs(geolat - oldlat);
         }
         
         #ifdef DEBUG_PLANETDATA_RUN
         MessageInterface::ShowMessage
            ("PlanetData::GetPlanetReal() geolat=%lf rad = %lf deg\n", geolat,
             geolat * 180.0 / PI);
         #endif
         
         if (item == LATITUDE)
         {
            //return geolat * 180.0 / PI;
            // put latitude between -90 and 90
            geolat = geolat * 180.0 / GmatMathConstants::PI;
            geolat = AngleUtil::PutAngleInDegRange(geolat, -90.0, 90.0);
            return geolat;
         }
         else  // item == ALTITUDE
         {
            sinlat = sin(geolat);
            cFactor = equatorialRadius / sqrt(1.0 - ecc2 * sinlat * sinlat);
            return rxy / cos(geolat) - cFactor;
         }
      }
   case LST_ID:
      {
         // compute Local Sidereal Time (LST = GMST + Longitude)
         // according to Vallado Eq. 3-41
         Real gmst = GetPlanetReal(MHA_ID);
         Real lst = gmst + GetPlanetReal(LONGITUDE);
         lst = AngleUtil::PutAngleInDegRange(lst, 0.0, 360.0);
         
         #ifdef DEBUG_PLANETDATA_RUN
         MessageInterface::ShowMessage("PlanetData::GetPlanetReal() lst=%f\n", lst);
         #endif
         
         return lst;
      }
   default:
      throw ParameterException("PlanetData::GetPlanetReal() Unknown parameter id: " +
                               GmatRealUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real GetPlanetReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives planet related parameters.
 */
//------------------------------------------------------------------------------
Real PlanetData::GetPlanetReal(const std::string &str)
{
   if (str == "Latitude")
      return GetPlanetReal(LATITUDE);
   else if (str == "Longitude")
      return GetPlanetReal(LONGITUDE);
   else if (str == "Altitude")
      return GetPlanetReal(ALTITUDE);
   else if (str == "MHA")
      return GetPlanetReal(MHA_ID);
   else if (str == "LST")
      return GetPlanetReal(LST_ID);
   else
   {
      throw ParameterException
         ("PlanetData::GetPlanetReal Unknown parameter name: " + str);
   }
}


//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/*
 * @param <cs> internal coordinate system what parameter data is representing.
 */
//------------------------------------------------------------------------------ 
void PlanetData::SetInternalCoordSystem(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}


//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* PlanetData::GetValidObjectList() const
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
bool PlanetData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<PlanetDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == PlanetDataObjectCount)
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void PlanetData::InitializeRefObjects()
{
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);

   if (mSpacecraft == NULL)
      throw ParameterException
         ("PlanetData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set.\n");
   
   mSolarSystem = (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   if (mSolarSystem == NULL)
      throw ParameterException
         ("PlanetData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   mCentralBody = mSolarSystem->GetBody(mCentralBodyName);
   
   if (!mCentralBody)
      throw ParameterException("PlanetData::InitializeRefObjects() Body not found in the "
                               "SolarSystem: " + mCentralBodyName + "\n");
   
   if (mInternalCoordSystem == NULL)
      throw ParameterException
         ("PlanetData::InitializeRefObjects() Cannot find internal "
          "CoordinateSystem object\n");
   #ifdef DEBUG_PLANETDATA_INIT
      MessageInterface::ShowMessage
         ("PlanetData::InitializeRefObjects() getting output CS pointer.\n");
   #endif
   
   mOutCoordSystem =
      (CoordinateSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[COORD_SYSTEM]);
   
   if (mOutCoordSystem == NULL)
      throw ParameterException
         ("PlanetData::InitializeRefObjects() Cannot find output "
          "CoordinateSystem object\n");
   
   // if dependent body name exist and it is a CelestialBody, set gravity constant
   
   std::string originName =
      FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[SPACE_POINT]));
   
   if (originName != "")
   {
      #ifdef DEBUG_PLANETDATA_INIT
         MessageInterface::ShowMessage
            ("PlanetData::InitializeRefObjects() getting originName:%s pointer.\n",
             originName.c_str());
      #endif
         
      mOrigin =
         (CelestialBody*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACE_POINT]);
      
      if (!mOrigin)
      {
         throw ParameterException
            ("PlanetData::InitializeRefObjects() Cannot find Origin object: " +
             originName + "\n");
      }

   }
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
bool PlanetData::IsValidObjectType(Gmat::ObjectType type)
{
#ifdef DEBUG_PLANETDATA_INIT
   MessageInterface::ShowMessage
      ("PlanetData::IsValidObjectType() CHECKING type %s.\n",(GmatBase::GetObjectTypeString(type).c_str()));
#endif
   for (int i=0; i<PlanetDataObjectCount; i++)
   {
#ifdef DEBUG_PLANETDATA_INIT
   MessageInterface::ShowMessage
      ("PlanetData::IsValidObjectType() has type %s.\n",(VALID_OBJECT_TYPE_LIST[i]).c_str());
#endif
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
      // Special case for CelestialBody origin
      if ((VALID_OBJECT_TYPE_LIST[i] == "SpacePoint") && (GmatBase::GetObjectTypeString(type) == "CelestialBody"))
         return true;
   }
   
   return false;
}

