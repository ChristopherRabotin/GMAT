//$Header$
//------------------------------------------------------------------------------
//                                  PlanetData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
#include "CelestialBody.hpp"
#include "AngleUtil.hpp"          //for PutAngleInDegRange()
#include "MessageInterface.hpp"

//#define DEBUG_PLANETDATA_INIT 1
//#define DEBUG_PLANETDATA_RUN 1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const std::string
PlanetData::VALID_OBJECT_TYPE_LIST[PlanetDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem",
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
// Real GetReal(const std::string &dataType)
//------------------------------------------------------------------------------
/**
 * Retrives atmospheric density where spacecraft is at.
 */
//------------------------------------------------------------------------------
Real PlanetData::GetReal(const std::string &dataType)
{
   #ifdef DEBUG_PLANETDATA_RUN
      MessageInterface::ShowMessage("PlanetData::GetReal() dataType=%s\n",
                                    dataType.c_str());
   #endif
      
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();

   //--------------------------------------------------
   // MHA
   //--------------------------------------------------
   if (dataType == "MHA")
   {
      // Get current time
      Real a1mjd = mSpacecraft->GetRealParameter("Epoch");
      
      // Call GetHourAngle() on origin
      Real gha = mOrigin->GetHourAngle(a1mjd);
      
      #ifdef DEBUG_PLANETDATA_RUN
         MessageInterface::ShowMessage
            ("PlanetData::GetReal() a1mdj=%f, origin=%s, gha=%f\n", a1mjd,
             mOrigin->GetName().c_str(), gha);
      #endif
      
      return gha;
   }
   //--------------------------------------------------
   // Longitude
   //--------------------------------------------------
   else if (dataType == "Longitude")
   {
      // Get spacecraft RightAscension
      Rvector6 state = mSpacecraft->GetCartesianState();
      
      if (mOrigin->GetName() != "Earth")
      {
         // Get current time
         Real a1mjd = mSpacecraft->GetRealParameter("Epoch");
         state = state - mOrigin->GetMJ2000State(a1mjd);
      }
      
      Real raRad = GmatMathUtil::ATan(state[1], state[0]);
      Real raDeg = GmatMathUtil::RadToDeg(raRad, true);
      Real gha = GetReal("MHA");
      
      // Compute east longitude
      // Longitude is measured positive to east from Greenwich
      // Reference: Valado 3.2.1 Location Parameters
      Real longitude = raDeg - gha;
      longitude = AngleUtil::PutAngleInDegRange(longitude, 0.0, 360.0);

      #ifdef DEBUG_PLANETDATA_RUN
         MessageInterface::ShowMessage
            ("PlanetData::GetReal() longitude=%f\n", longitude);
      #endif
      
      return longitude;
   }
   //--------------------------------------------------
   // Altitude
   //--------------------------------------------------
   else if (dataType == "Altitude")
   {
      Rvector6 state = mSpacecraft->GetCartesianState();      
      
      if (mOrigin->GetName() != "Earth")
      {
         // Get current time
         Real a1mjd = mSpacecraft->GetRealParameter("Epoch");
         state = state - mOrigin->GetMJ2000State(a1mjd);
      }
      
      // get flattening for the body
      Real flatteningFactor =
         mOrigin->GetRealParameter(mOrigin->GetParameterID("Flattening"));
      Real equatorialRadius =
         mOrigin->GetRealParameter(mOrigin->GetParameterID("EquatorialRadius"));
      
      Real geodesicFactor = equatorialRadius * (1.0 - flatteningFactor);
      
      Real rad = Sqrt(state[0]*state[0] + state[1]*state[1] + state[2]*state[2]);
      
      Real arg = state[2] / rad;
      arg = ((Abs(arg) <= 1.0) ? arg : arg / Abs(arg));
      Real radlat = PI / 2.0 - ACos(arg);
      
      // Convert to geodetic latitude, in degrees
      radlat = flatteningFactor * Sin(2.0 * radlat);
      
      // Geodetic altitude
      Real geoRad =
         geodesicFactor / Sqrt(1.0 - (2.0 - flatteningFactor) * flatteningFactor *
                               Cos(radlat) *  Cos(radlat));
      
      Real geoalt = rad - geoRad;
      
      #ifdef DEBUG_PLANETDATA_RUN
      MessageInterface::ShowMessage
         ("PlanetData::GetReal() radlat=%f, geolat=%f\n", radlat, geoalt);
      #endif
      
      return geoalt;
   }
   //--------------------------------------------------
   // Latitude
   //--------------------------------------------------
   else if (dataType == "Latitude")
   {
      Rvector6 state = mSpacecraft->GetCartesianState();
      
      if (mOrigin->GetName() != "Earth")
      {
         // Get current time
         Real a1mjd = mSpacecraft->GetRealParameter("Epoch");
         state = state - mOrigin->GetMJ2000State(a1mjd);
      }
      
      // get flattening for the body
      Real flatteningFactor =
         mOrigin->GetRealParameter(mOrigin->GetParameterID("Flattening"));
      
      Real rad = Sqrt(state[0]*state[0] + state[1]*state[1] + state[2]*state[2]);
      
      Real arg = state[2] / rad;
      arg = ((Abs(arg) <= 1.0) ? arg : arg / Abs(arg));
      Real radlat = PI / 2.0 - ACos(arg);
      
      // Convert to geodetic latitude, in degrees
      radlat = flatteningFactor * Sin(2.0 * radlat);
      Real geolat = radlat * 180.0 / PI;
      
      #ifdef DEBUG_PLANETDATA_RUN
      MessageInterface::ShowMessage
         ("PlanetData::GetReal() radlat=%f, geolat=%f\n", radlat, geolat);
      #endif
      
      return geolat;
   }
   //--------------------------------------------------
   // LST
   //--------------------------------------------------
   else if (dataType == "LST")
   {
      // compute Local Sidereal Time (LST = GMST + Longitude)
      // according to Vallado Eq. 3-41
      Real gmst = GetReal("MHA");
      Real lst = gmst + GetReal("Longitude");
      lst = AngleUtil::PutAngleInDegRange(lst, 0.0, 360.0);
      
      // convert it to hours (1h = 15 deg according to Vallado 3.5)
      //lst = lst / 15.0;
      
      #ifdef DEBUG_PLANETDATA_RUN
         MessageInterface::ShowMessage
            ("PlanetData::GetReal() lst=%f\n", lst);
      #endif
      
      return lst;
   }
   else
   {
      throw ParameterException("PlanetData::GetReal() Unknown parameter name: " +
                               dataType);
   }
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
      throw ParameterException("PlanetData::GetCartState() Body not found in the "
                               "SolarSystem: " + mCentralBodyName + "\n");
   
   // if dependent body name exist and it is a CelestialBody, set gravity constant
   
   std::string originName =
      FindFirstObjectName(GmatBase::GetObjectType(VALID_OBJECT_TYPE_LIST[SPACE_POINT]));

   if (originName != "")
   {
      #if DEBUG_PLANETDATA_INIT
         MessageInterface::ShowMessage
            ("PlanetData::InitializeRefObjects() getting originName:%s pointer.\n",
             originName.c_str());
      #endif
         
      mOrigin =
         (CelestialBody*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACE_POINT]);
      
      if (!mOrigin)
         throw ParameterException
            ("PlanetData::InitializeRefObjects() Cannot find Origin object: " +
             originName + "\n");

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
   for (int i=0; i<PlanetDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;
}

