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
//
/**
 * Implements class which provides planet related data, such as HourAngle.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "PlanetData.hpp"
#include "ParameterException.hpp"
#include "CelestialBody.hpp"
#include "AngleUtil.hpp"          //for PutAngleInDegRange()
#include "MessageInterface.hpp"

//#define DEBUG_PLANET_DATA 1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const std::string
PlanetData::VALID_OBJECT_TYPE_LIST[PlanetDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem"
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
   mSpacecraft = NULL;
   mSolarSystem = NULL;
}

//------------------------------------------------------------------------------
// PlanetData(const PlanetData &data)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <data> the PlanetData object being copied.
 */
//------------------------------------------------------------------------------
PlanetData::PlanetData(const PlanetData &data)
   : RefData(data)
{
}

//------------------------------------------------------------------------------
// PlanetData& operator= (const PlanetData& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
PlanetData& PlanetData::operator= (const PlanetData& right)
{
   if (this != &right)
      RefData::operator=(right);

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
#ifdef DEBUG_PLANET_DATA
      MessageInterface::ShowMessage("PlanetData::GetReal() dataType=%s\n",
                                    dataType.c_str());
#endif
      
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();

   //--------------------------------------------------
   // GHA
   //--------------------------------------------------
   if (dataType == "GHA")
   {
      // Get current time
      Real a1mjd = mSpacecraft->GetRealParameter("Epoch");
      
      // Call GetHourAngle() on central body
      Real gha = mCentralBody->GetHourAngle(a1mjd);

#ifdef DEBUG_PLANET_DATA
      MessageInterface::ShowMessage
         ("PlanetData::GetReal() a1mdj=%f, gha=%f\n", a1mjd, gha);
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
      Real raRad = GmatMathUtil::ATan(state[1], state[0]);
      Real raDeg = GmatMathUtil::RadToDeg(raRad, true);
      Real gha = GetReal("GHA");

      // Compute east longitude
      // Longitude is measured positive to east from Greenwich
      // Reference: Valado 3.2.1 Location Parameters
      Real longitude = raDeg - gha;
      longitude = AngleUtil::PutAngleInDegRange(longitude, 0.0, 360.0);

#ifdef DEBUG_PLANET_DATA
      MessageInterface::ShowMessage
         ("PlanetData::GetReal() longitude=%f\n", longitude);
#endif
      
      return longitude;
   }
   //--------------------------------------------------
   // Latitude
   //--------------------------------------------------
   else if (dataType == "Latitude")
   {
      // Compute geocentric latitude (spherical declination)
      Rvector6 state = mSpacecraft->GetCartesianState();
      Real decRad = ATan(state[2], Sqrt(state[0]*state[0] +
                                        state[1]*state[1]));
      Real decDeg = GmatMathUtil::RadToDeg(decRad, true);
      Real latitude = AngleUtil::PutAngleInDegRange(decDeg, 0.0, 360.0);

#ifdef DEBUG_PLANET_DATA
      MessageInterface::ShowMessage
         ("PlanetData::GetReal() latitude=%f\n", latitude);
#endif
      
      return latitude;
   }
   //--------------------------------------------------
   // LST
   //--------------------------------------------------
   else if (dataType == "LST")
   {
      // compute Local Sidereal Time (LST = GMST + Longitude)
      // according to Vallado Eq. 3-41
      Real gmst = GetReal("GHA") * -1.0;
      Real lst = gmst + GetReal("Longitude");
      lst = AngleUtil::PutAngleInDegRange(lst, 0.0, 360.0);

#ifdef DEBUG_PLANET_DATA
      MessageInterface::ShowMessage
         ("PlanetData::GetReal() lst=%f\n", lst);
#endif
      
      return lst;
   }
   //--------------------------------------------------
   // BetaAngle
   //--------------------------------------------------
   else if (dataType == "BetaAngle")
   {
      // compute the beta angle.
      // according to Vallado section 5.3.2 Beta Angle
      // Sin(BetaAngle) = Sin(Epsilon) * Cos(INC) * Sin(SunDec) -
      // Cos(Epsilon) * Sin(INC) * Cos(RAAN) * Sin(SunDec) +
      // Sin(INC) * Sin(RAAN) * Cos(SunDec);

      // compute Epsilon accorging to Vallado section 5.1.1
      // algorithm 29: epsilon = 23.439291(deg) - 0.0130042 * T(tdb)
      // where T_tdb =~ T_ut1

      Real a1mjdReal = mSpacecraft->GetRealParameter("Epoch");
      A1Mjd a1mjd(a1mjdReal);
      Ut1Mjd ut1mjd = a1mjd.ToUt1Mjd();
      Real epsilonRad = (23.439291 - 0.0130042 * ut1mjd) * RAD_PER_DEG;

      // compute Sun declination
      CelestialBody *sun = mSolarSystem->GetBody(SolarSystem::SUN_NAME);
      Rvector6 sunState = sun->GetState(a1mjdReal);
      Real sunDecRad = ATan(sunState[2], Sqrt(sunState[0] * sunState[0] +
                                              sunState[1] * sunState[1]));
      
      // compute beta angle
      Rvector6 kepState = mSpacecraft->GetKeplerianState();
      Real incRad = kepState[2] * RAD_PER_DEG;
      Real raanRad = kepState[3] * RAD_PER_DEG;
      
#ifdef DEBUG_PLANET_DATA
      MessageInterface::ShowMessage
         ("PlanetData::GetReal() sunDecRad=%f, incRad=%f, raanRad=%f\n",
          sunDecRad, incRad, raanRad);
#endif
      
      Real sinBetaAngle = Sin(epsilonRad) * Cos(incRad) * Sin(sunDecRad) -
         Cos(epsilonRad) * Sin(incRad) * Cos(raanRad) * Sin(sunDecRad) +
         Sin(incRad) * Sin(raanRad) * Cos(sunDecRad);
      
      Real betaAngleDeg = ASin(sinBetaAngle) * DEG_PER_RAD;
      betaAngleDeg = AngleUtil::PutAngleInDegRange(betaAngleDeg, 0.0, 360.0);

#ifdef DEBUG_PLANET_DATA
      MessageInterface::ShowMessage
         ("PlanetData::GetReal() betaAngle=%f\n", betaAngleDeg);
#endif
      
      return betaAngleDeg;
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

