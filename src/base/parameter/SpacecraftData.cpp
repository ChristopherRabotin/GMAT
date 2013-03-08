//$Id$
//------------------------------------------------------------------------------
//                                  SpacecraftData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun
// Created: 2009.03.20
//
/**
 * Implements Spacecraft related data class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SpacecraftData.hpp"
#include "ParameterException.hpp"
#include "StringUtil.hpp"          // ToString()
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SPACECRAFTDATA_INIT
//#define DEBUG_SC_OWNED_OBJ


const std::string
SpacecraftData::VALID_OBJECT_TYPE_LIST[SpacecraftDataObjectCount] =
{
   "Spacecraft"
}; 

const Real SpacecraftData::BALLISTIC_REAL_UNDEFINED = GmatRealConstants::REAL_UNDEFINED_LARGE;

//------------------------------------------------------------------------------
// SpacecraftData()
//------------------------------------------------------------------------------
SpacecraftData::SpacecraftData(const std::string &name)
   : RefData(name)
{
   mSpacecraft = NULL;
}


//------------------------------------------------------------------------------
// SpacecraftData(const SpacecraftData &copy)
//------------------------------------------------------------------------------
SpacecraftData::SpacecraftData(const SpacecraftData &copy)
   : RefData(copy)
{
   mSpacecraft = copy.mSpacecraft;
}


//------------------------------------------------------------------------------
// SpacecraftData& operator= (const SpacecraftData& right)
//------------------------------------------------------------------------------
SpacecraftData& SpacecraftData::operator= (const SpacecraftData& right)
{
   if (this == &right)
      return *this;
   
   RefData::operator=(right);
   
   mSpacecraft = right.mSpacecraft;

   return *this;
}


//------------------------------------------------------------------------------
// ~SpacecraftData()
//------------------------------------------------------------------------------
SpacecraftData::~SpacecraftData()
{
}


//------------------------------------------------------------------------------
// Real GetReal(Integer item)
//------------------------------------------------------------------------------
/**
 * Retrieves Spacecraft or spacecraft owned hardware element by integer id.
 */
//------------------------------------------------------------------------------
Real SpacecraftData::GetReal(Integer item)
{
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   // if there was an error initializing the ref objects, do not try to get the value
   if (mSpacecraft == NULL)
      return BALLISTIC_REAL_UNDEFINED;

   switch (item)
   {
   case DRY_MASS:
      return mSpacecraft->GetRealParameter("DryMass");
   case DRAG_COEFF:
      return mSpacecraft->GetRealParameter("Cd");
   case REFLECT_COEFF:
      return mSpacecraft->GetRealParameter("Cr");
   case DRAG_AREA:
      return mSpacecraft->GetRealParameter("DragArea");
   case SRP_AREA:      
      return mSpacecraft->GetRealParameter("SRPArea");
   case TOTAL_MASS:
      return mSpacecraft->GetRealParameter("TotalMass");
      
   // for Spacecraft owned FuelTank
   case FUEL_MASS:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMass");
   case PRESSURE:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "Pressure");
   case TEMPERATURE:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "Temperature");
   case REF_TEMPERATURE:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "RefTemperature");
   case VOLUME:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "Volume");
   case FUEL_DENSITY:
      return GetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelDensity");
      
   // for Spacecraft owned Thruster
   case DUTY_CYCLE:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "DutyCycle");
   case THRUSTER_SCALE_FACTOR:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "ThrustScaleFactor");
   case GRAVITATIONAL_ACCEL:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "GravitationalAccel");
      
   // Thrust Coefficients
   case C1:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C1");
   case C2:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C2");
   case C3:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C3");
   case C4:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C4");
   case C5:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C5");
   case C6:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C6");
   case C7:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C7");
   case C8:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C8");
   case C9:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C9");
   case C10:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C10");
   case C11:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C11");
   case C12:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C12");
   case C13:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C13");
   case C14:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C14");
   case C15:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C15");
   case C16:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "C16");
      
   // Impulse Coefficients
   case K1:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K1");
   case K2:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K2");
   case K3:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K3");
   case K4:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K4");
   case K5:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K5");
   case K6:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K6");
   case K7:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K7");
   case K8:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K8");
   case K9:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K9");
   case K10:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K10");
   case K11:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K11");
   case K12:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K12");
   case K13:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K13");
   case K14:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K14");
   case K15:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K15");
   case K16:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "K16");
      
   // Thruster ThrustDirections
   case THRUST_DIRECTION1:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection1");
   case THRUST_DIRECTION2:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection2");
   case THRUST_DIRECTION3:
      return GetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection3");
      
   default:
      // otherwise, there is an error   
      throw ParameterException
         ("SpacecraftData::GetReal() Not readable or unknown item id: " +
          GmatStringUtil::ToString(item));
   }
}


//------------------------------------------------------------------------------
// Real SetReal(Integer item, Real val)
//------------------------------------------------------------------------------
/**
 * Sets Spacecraft or spacecraft owned hardware element by integer id.
 */
//------------------------------------------------------------------------------
Real SpacecraftData::SetReal(Integer item, Real val)
{
   if (mSpacecraft == NULL)
      InitializeRefObjects();
   
   // if there was an error initializing the ref objects, do not try to set the value
   if (mSpacecraft == NULL)
      return BALLISTIC_REAL_UNDEFINED;

   switch (item)
   {
   case DRY_MASS:
      return mSpacecraft->SetRealParameter("DryMass", val);
   case DRAG_COEFF:
      return mSpacecraft->SetRealParameter("Cd", val);
   case REFLECT_COEFF:
      return mSpacecraft->SetRealParameter("Cr", val);
   case DRAG_AREA:
      return mSpacecraft->SetRealParameter("DragArea", val);
   case SRP_AREA:      
      return mSpacecraft->SetRealParameter("SRPArea", val);
      
   // for Spacecraft owned FuelTank
   case FUEL_MASS:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelMass", val);
   case PRESSURE:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "Pressure", val);
   case TEMPERATURE:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "Temperature", val);
   case REF_TEMPERATURE:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "RefTemperature", val);
   case VOLUME:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "Volume", val);
   case FUEL_DENSITY:
      return SetOwnedObjectProperty(Gmat::FUEL_TANK, "FuelDensity", val);
      
   // for Spacecraft owned Thruster
   case DUTY_CYCLE:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "DutyCycle", val);
   case THRUSTER_SCALE_FACTOR:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "ThrustScaleFactor", val);
   case GRAVITATIONAL_ACCEL:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "GravitationalAccel", val);
      
   // Thrust Coefficients
   case C1:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C1", val);
   case C2:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C2", val);
   case C3:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C3", val);
   case C4:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C4", val);
   case C5:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C5", val);
   case C6:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C6", val);
   case C7:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C7", val);
   case C8:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C8", val);
   case C9:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C9", val);
   case C10:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C10", val);
   case C11:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C11", val);
   case C12:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C12", val);
   case C13:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C13", val);
   case C14:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C14", val);
   case C15:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C15", val);
   case C16:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "C16", val);
      
   // Impulse Coefficients
   case K1:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K1", val);
   case K2:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K2", val);
   case K3:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K3", val);
   case K4:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K4", val);
   case K5:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K5", val);
   case K6:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K6", val);
   case K7:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K7", val);
   case K8:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K8", val);
   case K9:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K9", val);
   case K10:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K10", val);
   case K11:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K11", val);
   case K12:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K12", val);
   case K13:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K13", val);
   case K14:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K14", val);
   case K15:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K15", val);
   case K16:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "K16", val);
      
   // Thruster ThrustDirections
   case THRUST_DIRECTION1:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection1", val);
   case THRUST_DIRECTION2:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection2", val);
   case THRUST_DIRECTION3:
      return SetOwnedObjectProperty(Gmat::THRUSTER, "ThrustDirection3", val);
      
   default:
      // otherwise, there is an error   
      throw ParameterException
         ("SpacecraftData::SetReal() Not settable or unknown item id: " +
          GmatStringUtil::ToString(item));
   }
}

//-------------------------------------
// Inherited methods from RefData
//-------------------------------------

//------------------------------------------------------------------------------
// virtual const std::string* GetValidObjectList() const
//------------------------------------------------------------------------------
const std::string* SpacecraftData::GetValidObjectList() const
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
bool SpacecraftData::ValidateRefObjects(GmatBase *param)
{
   int objCount = 0;
   for (int i=0; i<SpacecraftDataObjectCount; i++)
   {
      if (HasObjectType(VALID_OBJECT_TYPE_LIST[i]))
         objCount++;
   }
   
   if (objCount == SpacecraftDataObjectCount)
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
void SpacecraftData::InitializeRefObjects()
{
   #ifdef DEBUG_SPACECRAFTDATA_INIT
   MessageInterface::ShowMessage
      ("SpacecraftData::InitializeRefObjects() '%s' entered.\n", mActualParamName.c_str());
   #endif
   
   mSpacecraft = (Spacecraft*)FindFirstObject(VALID_OBJECT_TYPE_LIST[SPACECRAFT]);
   
   if (mSpacecraft == NULL)
   {
      // Just write a message since Parameters in GmatFunction may not have ref. object
      // set until execution
      #ifdef DEBUG_SPACECRAFTDATA_INIT
      MessageInterface::ShowMessage
         ("SpacecraftData::InitializeRefObjects() Cannot find Spacecraft object.\n");
      #endif
      
      //throw ParameterException
      //   ("SpacecraftData::InitializeRefObjects() Cannot find Spacecraft object.\n");
   }
   
   #ifdef DEBUG_SPACECRAFTDATA_INIT
   MessageInterface::ShowMessage
      ("SpacecraftData::InitializeRefObjects() '%s' leaving, mSpacecraft=<%p>'%s'\n",
       mActualParamName.c_str(), mSpacecraft, mSpacecraft->GetName().c_str());
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
bool SpacecraftData::IsValidObjectType(Gmat::ObjectType type)
{
   for (int i=0; i<SpacecraftDataObjectCount; i++)
   {
      if (GmatBase::GetObjectTypeString(type) == VALID_OBJECT_TYPE_LIST[i])
         return true;
   }
   
   return false;

}


//------------------------------------------------------------------------------
// Real GetOwnedObjectProperty(Gmat::ObjectType objType, const std::string &propName)
//------------------------------------------------------------------------------
Real SpacecraftData::GetOwnedObjectProperty(Gmat::ObjectType objType,
                                            const std::string &propName)
{
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(mActualParamName, type, owner, dep);
   
   #ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
      ("SpacecraftData::GetOwnedObjectProperty() '%s' entered, objType=%d, propName='%s'\n   "
       "type='%s', owner='%s', dep='%s'\n", mActualParamName.c_str(), objType, propName.c_str(),
       type.c_str(), owner.c_str(), dep.c_str());
   #endif
   
   GmatBase *ownedObj = mSpacecraft->GetRefObject(objType, dep);
   
   #ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
      ("   ownedObj=<%p><%s>'%s'\n", ownedObj, ownedObj ? ownedObj->GetTypeName().c_str() : "NULL",
       ownedObj ? ownedObj->GetName().c_str() : "NULL");
   #endif
   
   if (ownedObj == NULL)
   {
      Integer runMode = GmatGlobal::Instance()->GetRunMode();
      std::string msg = "Cannot evaluate Parameter \"" + mActualParamName + "\"; " +
         GmatBase::GetObjectTypeString(objType) + " named \"" + dep + "\" is not "
         "attached to Spacecraft \"" + mSpacecraft->GetName() + "\"";
      
      if (runMode == GmatGlobal::TESTING || runMode == GmatGlobal::TESTING_NO_PLOTS)
         MessageInterface::ShowMessage("In SpacecraftData::GetOwnedObjectProperty() " + msg + "\n");
      
      ParameterException pe(msg);
      throw pe;
   }
   else
   {
      Real result = ownedObj->GetRealParameter(propName);
      
      #ifdef DEBUG_SC_OWNED_OBJ
      MessageInterface::ShowMessage
         ("SpacecraftData::GetOwnedObjectProperty() returning %f\n", result);
      #endif
      return result;
   }
}


//------------------------------------------------------------------------------
// Real SetOwnedObjectProperty(Gmat::ObjectType objType, const std::string &propName,
//                             Real val)
//------------------------------------------------------------------------------
Real SpacecraftData::SetOwnedObjectProperty(Gmat::ObjectType objType,
                                            const std::string &propName,
                                            Real val)
{
   std::string type, owner, dep;
   GmatStringUtil::ParseParameter(mActualParamName, type, owner, dep);
   
   #ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
      ("SpacecraftData::SetOwnedObjectProperty() '%s' entered, objType=%d, "
       "propName='%s', val=%f, type='%s', owner='%s', dep='%s',\n",
       mActualParamName.c_str(), objType, propName.c_str(), val,
       type.c_str(), owner.c_str(), dep.c_str());
   #endif
   
   GmatBase *ownedObj = mSpacecraft->GetRefObject(objType, dep);
   
   #ifdef DEBUG_SC_OWNED_OBJ
   MessageInterface::ShowMessage
      ("   ownedObj=<%p><%s>'%s'\n", ownedObj, ownedObj ? ownedObj->GetTypeName().c_str() : "NULL",
       ownedObj ? ownedObj->GetName().c_str() : "NULL");
   #endif
   
   if (ownedObj == NULL)
   {
      ParameterException pe;
      pe.SetDetails("SpacecraftData::SetOwnedObjectProperty() %s \"%s\" is not "
                    "attached to Spacecraft \"%s\"",
                    GmatBase::GetObjectTypeString(objType).c_str(), dep.c_str(),
                    mSpacecraft->GetName().c_str());
      throw pe;
   }
   else
   {
      Real result = ownedObj->SetRealParameter(propName, val);
      
      #ifdef DEBUG_SC_OWNED_OBJ
      MessageInterface::ShowMessage
         ("SpacecraftData::SetOwnedObjectProperty() '%s' returning %f\n",
          mActualParamName.c_str(), result);
      #endif
      return result;
   }
}

