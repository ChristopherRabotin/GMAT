//$Header$
//------------------------------------------------------------------------------
//                                  EnvData
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/12/07
//
/**
 * Implements class which provides environmental data, such as atmospheric density,
 * solar flux, etc.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "EnvData.hpp"
#include "ParameterException.hpp"
#include "CelestialBody.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_ENVDATA 1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

const std::string
EnvData::VALID_OBJECT_TYPE_LIST[EnvDataObjectCount] =
{
   "Spacecraft",
   "SolarSystem"
}; 

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// EnvData()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
EnvData::EnvData()
   : RefData()
{
   mSpacecraft = NULL;
   mSolarSystem = NULL;
}

//------------------------------------------------------------------------------
// EnvData(const EnvData &data)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param <data> the EnvData object being copied.
 */
//------------------------------------------------------------------------------
EnvData::EnvData(const EnvData &data)
   : RefData(data)
{
}

//------------------------------------------------------------------------------
// EnvData& operator= (const EnvData& right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param <right> the object being copied.
 *
 * @return reference to this object
 */
//------------------------------------------------------------------------------
EnvData& EnvData::operator= (const EnvData& right)
{
   if (this != &right)
      RefData::operator=(right);

   return *this;
}

//------------------------------------------------------------------------------
// ~EnvData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EnvData::~EnvData()
{
}

//------------------------------------------------------------------------------
// Real GetEnvReal(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Retrives atmospheric density where spacecraft is at.
 */
//------------------------------------------------------------------------------
Real EnvData::GetEnvReal(const std::string &str)
{
   //-------------------------------------------------------
   // 1. Get Spacecraft's central body (It's done in InitializeRefObjects())
   // 2. Get current time (use parameter or sc pointer?)
   // 3. Get spacecraft's position (use parameter?)
   // 4. Call GetDensity() on central body
   //-------------------------------------------------------
   
   if (mSpacecraft == NULL || mSolarSystem == NULL)
      InitializeRefObjects();

   if (str == "AtmosDensity")
   {
      Real a1mjd = mSpacecraft->GetRealParameter("Epoch");
      Rvector6 cartState = mSpacecraft->GetCartesianState();
      Real state[6];
      for (int i=0; i<6; i++)
         state[i] = cartState[i];
      
      Real density = 0.0;
      
      // Call GetDensity() on central body
      mCentralBody->GetDensity(state, &density, a1mjd, 1);

#ifdef DEBUG_ENVDATA
      MessageInterface::ShowMessage("EnvData::GetEnvReal() density=%g\n", density);
#endif
      
      return density;
   }
   else
   {
      throw ParameterException("EnvData::GetEnvReal() Unknown parameter name: " +
                               str);
   }
}

//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* EnvData::GetValidObjectList() const
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
bool EnvData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<EnvDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }

   if (objCount == EnvDataObjectCount)
      return true;
   else
      return false;
}

//------------------------------------------------------------------------------
// virtual void InitializeRefObjects()
//------------------------------------------------------------------------------
void EnvData::InitializeRefObjects()
{
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);

   if (mSpacecraft == NULL)
      throw ParameterException
         ("EnvData::InitializeRefObjects() Cannot find Spacecraft object.\n"
          "Make sure Spacecraft is set to any unnamed parameters\n");
   
   mSolarSystem = (SolarSystem*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SOLAR_SYSTEM]);
   if (mSolarSystem == NULL)
      throw ParameterException
         ("EnvData::InitializeRefObjects() Cannot find SolarSystem object\n");
   
   Integer id = mSpacecraft->GetParameterID("ReferenceBody");
   std::string bodyName = mSpacecraft->GetStringParameter(id);
   mCentralBody = mSolarSystem->GetBody(bodyName);
   
   if (!mCentralBody)
      throw ParameterException("EnvData::GetCartState() Body not found in the "
                               "SolarSystem: " + bodyName + "\n");

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
bool EnvData::IsValidObjectType(Gmat::ObjectType type)
{
   for (int i=0; i<EnvDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}

