//$Id$
//------------------------------------------------------------------------------
//                               Thruster
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Darrel J. Conway
// Created: 2004/11/08
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class implementation for the Spacecraft engines (aka Thrusters).
 */
//------------------------------------------------------------------------------


#include "Thruster.hpp"
#include "ObjectReferencedAxes.hpp"
#include "Spacecraft.hpp"
#include "StringUtil.hpp"
#include "HardwareException.hpp"
#include "MessageInterface.hpp"
#include <sstream>
#include <math.h>          // for pow(real, real)

//#define DEBUG_THRUSTER
//#define DEBUG_THRUSTER_CONSTRUCTOR
//#define DEBUG_THRUSTER_SET
//#define DEBUG_THRUSTER_REF_OBJ
//#define DEBUG_THRUSTER_INIT
//#define DEBUG_THRUSTER_CONVERT
//#define DEBUG_THRUSTER_CONVERT_ROTMAT
//#define DEBUG_BURN_CONVERT_ROTMAT
//#define DEBUG_SET_POWER

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

/// Available local axes labels
StringArray Thruster::localAxesLabels;

/// Labels used for the thruster element parameters.
const std::string
Thruster::PARAMETER_TEXT[ThrusterParamCount - HardwareParamCount] =
{
   "IsFiring",
   "CoordinateSystem",
   "Origin",
   "Axes",
   "DutyCycle",
   "ThrustScaleFactor",
   "DecrementMass",
   "Tank",
   "GravitationalAccel",
};

/// Types of the parameters used by thrusters.
const Gmat::ParameterType
Thruster::PARAMETER_TYPE[ThrusterParamCount - HardwareParamCount] =
{
   Gmat::BOOLEAN_TYPE,     // "IsFiring"
   Gmat::OBJECT_TYPE,      // "CoordinateSystem"
   Gmat::OBJECT_TYPE,      // "Origin"
   Gmat::ENUMERATION_TYPE, // "Axes"
   Gmat::REAL_TYPE,        // "DutyCycle"
   Gmat::REAL_TYPE,        // "ThrustScaleFactor"
   Gmat::BOOLEAN_TYPE,     // "DecrementMass"
   Gmat::OBJECTARRAY_TYPE, // "Tank"
   Gmat::REAL_TYPE,        // "GravitationalAccel"
};


//------------------------------------------------------------------------------
//  Thruster(std::string nomme)
//------------------------------------------------------------------------------
/**
 * Thruster constructor with default VNB Local CoordinateSystem.
 *
 * @param nomme Name of the thruster.
 *
 * @note coordSystem and spacecraft are set through SetRefObject() during
 *       Sandbox initialization. localOrigin and j2000Body are reset when
 *       solarSystem is set. localCoordSystem is created during initialization
 *       or when new spacecraft is set
 */
//------------------------------------------------------------------------------
Thruster::Thruster(const std::string &typeStr, const std::string &nomme) :
   Hardware             (Gmat::THRUSTER, typeStr, nomme),
   solarSystem          (NULL),
   localCoordSystem     (NULL),
   coordSystem          (NULL),
   localOrigin          (NULL),
   j2000Body            (NULL),
   spacecraft           (NULL),
   coordSystemName      ("Local"),
   localOriginName      ("Earth"),
   localAxesName        ("VNB"),
   j2000BodyName        ("Earth"),
   satName              (""),
   power                (0.0),
   gravityAccel         (9.81),
   dutyCycle            (1.0),
   thrustScaleFactor    (1.0),
   pressure             (1500.0),
   temperatureRatio     (1.0),
   thrust               (500.0),
   impulse              (2150.0),
   mDot                 (0.0),
   decrementMass        (false),
   thrusterFiring       (false),
   constantExpressions  (true),
   simpleExpressions    (true),
   usingLocalCoordSys   (true),
   isMJ2000EqAxes       (false),
   isSpacecraftBodyAxes (false)
{
   objectTypes.push_back(Gmat::THRUSTER);
   objectTypeNames.push_back("Thruster");
   parameterCount = ThrusterParamCount;
   blockCommandModeAssignment = false;
   
   inertialDirection[0] = 1.0;
   inertialDirection[1] = 0.0;
   inertialDirection[2] = 0.0;

   // Available local axes labels
   // Since it is static data, clear it first
   localAxesLabels.clear();
   localAxesLabels.push_back("VNB");
   localAxesLabels.push_back("LVLH");
   localAxesLabels.push_back("MJ2000Eq");
   localAxesLabels.push_back("SpacecraftBody");

   // set parameter write order
   for (Integer i=HardwareParamCount; i <= AXES; i++)
      parameterWriteOrder.push_back(i);
   
   parameterWriteOrder.push_back(DIRECTION_X);
   parameterWriteOrder.push_back(DIRECTION_Y);
   parameterWriteOrder.push_back(DIRECTION_Z);
   
   for (Integer i=DUTY_CYCLE; i < ThrusterParamCount; i++)
      parameterWriteOrder.push_back(i);
}


//------------------------------------------------------------------------------
//  ~Thruster()
//------------------------------------------------------------------------------
/**
 * Thruster destructor.
 */
//------------------------------------------------------------------------------
Thruster::~Thruster()
{
   // delete local coordinate system
   if (usingLocalCoordSys && localCoordSystem)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (localCoordSystem, "localCS", "Thruster::~Thruster()",
          "deleting localCoordSystem");
      #endif
      delete localCoordSystem;
      localCoordSystem = NULL;
   }
}


//------------------------------------------------------------------------------
//  Thruster(const Thruster& th)
//------------------------------------------------------------------------------
/**
 * Thruster copy constructor.
 *
 * @param th The object being copied.
 *
 * @note coordSystem and spacecraft are set through SetRefObject() during
 *       Sandbox initialization. localOrigin and j2000Body are reset when
 *       solarSystem is set. localCoordSystem is created during initialization
 *       or when new spacecraft is set
 */
//------------------------------------------------------------------------------
Thruster::Thruster(const Thruster& th) :
   Hardware             (th),   
   solarSystem          (th.solarSystem), // copy
   localCoordSystem     (NULL),
   coordSystem          (th.coordSystem), // copy
   localOrigin          (th.localOrigin), // copy
   j2000Body            (th.j2000Body),   // copy
   spacecraft           (NULL),
   coordSystemName      (th.coordSystemName),
   localOriginName      (th.localOriginName),
   localAxesName        (th.localAxesName),
   j2000BodyName        (th.j2000BodyName),
   satName              (th.satName),
   power                (th.power),
   gravityAccel         (th.gravityAccel),
   dutyCycle            (th.dutyCycle),
   thrustScaleFactor    (th.thrustScaleFactor),
   pressure             (th.pressure),
   temperatureRatio     (th.temperatureRatio),
   thrust               (th.thrust),
   impulse              (th.impulse),
   mDot                 (th.mDot),
   decrementMass        (th.decrementMass),
   thrusterFiring       (th.thrusterFiring),
   constantExpressions  (th.constantExpressions),
   simpleExpressions    (th.simpleExpressions),
   usingLocalCoordSys   (th.usingLocalCoordSys),
   isMJ2000EqAxes       (th.isMJ2000EqAxes),
   isSpacecraftBodyAxes (th.isSpacecraftBodyAxes),
   tankNames            (th.tankNames)
{
   #ifdef DEBUG_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("Thruster::Thruster(copy) entered, this = <%p>'%s', copying from <%p>'%s'\n",
       this, GetName().c_str(), &th, th.GetName().c_str());
   #endif
   
   parameterCount = th.parameterCount;
   localAxesLabels = th.localAxesLabels;
   
   inertialDirection[0] = th.inertialDirection[0];
   inertialDirection[1] = th.inertialDirection[1];
   inertialDirection[2] = th.inertialDirection[2];
   
   #ifdef DEBUG_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("   copying tanks. There are %d tank(s) to copy from <%p>\n", th.tanks.size(), &th);
   MessageInterface::ShowMessage
      ("   solarSystem=<%p>, usingLocalCoordSys=%d, coordSystem=<%p>\n",
       solarSystem, usingLocalCoordSys, coordSystem);
   #endif
   
   // copy tanks
   tanks = th.tanks;
   
   isInitialized = false;

   // set parameter write order
   for (Integer i=HardwareParamCount; i <= AXES; i++)
      parameterWriteOrder.push_back(i);

   parameterWriteOrder.push_back(DIRECTION_X);
   parameterWriteOrder.push_back(DIRECTION_Y);
   parameterWriteOrder.push_back(DIRECTION_Z);

   for (Integer i=DUTY_CYCLE; i < ThrusterParamCount; i++)
      parameterWriteOrder.push_back(i);

   #ifdef DEBUG_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage("Thruster::Thruster(copy) exiting\n");
   #endif
}


//------------------------------------------------------------------------------
//  Thruster& operator=(const Thruster& th)
//------------------------------------------------------------------------------
/**
 * Thruster assignment operator.
 *
 * @param th The object being copied.
 *
 * @return this object, with parameters set to the input object's parameters.
 *
 * @note coordSystem and spacecraft are set through SetRefObject() during
 *       Sandbox initialization. localOrigin and j2000Body are reset when
 *       solarSystem is set. localCoordSystem is created during initialization
 *       or when new spacecraft is set
 */
//------------------------------------------------------------------------------
Thruster& Thruster::operator=(const Thruster& th)
{
   if (&th == this)
      return *this;
   
   #ifdef DEBUG_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage("Thruster::operator= entered\n");
   #endif
   
   Hardware::operator=(th);
   
   solarSystem         = th.solarSystem; // copy
   localCoordSystem    = NULL;
   coordSystem         = th.coordSystem; // copy
   localOrigin         = th.localOrigin; // copy
   j2000Body           = th.j2000Body;   // copy
   spacecraft          = NULL;
   coordSystemName     = th.coordSystemName;
   localOriginName     = th.localOriginName;
   localAxesName       = th.localAxesName;
   j2000BodyName       = th.j2000BodyName;
   satName             = th.satName;
   power               = th.power;
   
   gravityAccel        = th.gravityAccel;
   dutyCycle           = th.dutyCycle;
   thrustScaleFactor   = th.thrustScaleFactor;
   pressure            = th.pressure;
   temperatureRatio    = th.temperatureRatio;
   thrust              = th.thrust;
   impulse             = th.impulse;
   mDot                = th.mDot;
   
   inertialDirection[0]  = th.inertialDirection[0];
   inertialDirection[1]  = th.inertialDirection[1];
   inertialDirection[2]  = th.inertialDirection[2];
   
   thrusterFiring      = th.thrusterFiring;
   decrementMass       = th.decrementMass;
   constantExpressions = th.constantExpressions;
   simpleExpressions   = th.simpleExpressions;
   usingLocalCoordSys  = th.usingLocalCoordSys;
   usingLocalCoordSys  = th.usingLocalCoordSys;
   localAxesLabels     = th.localAxesLabels;
   isInitialized       = false;
   
   localAxesLabels     = th.localAxesLabels;
   tankNames           = th.tankNames;
   
   // copy tanks
   tanks               = th.tanks;
   
   #ifdef DEBUG_THRUSTER_CONSTRUCTOR
   MessageInterface::ShowMessage("Thruster::operator= exiting\n");
   #endif
   
   return *this;
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Thruster::GetParameterText(const Integer id) const
{
   if (id >= HardwareParamCount && id < ThrusterParamCount)
      return PARAMETER_TEXT[id - HardwareParamCount];
   
   if (id == DIRECTION_X)
      return "ThrustDirection1";
   if (id == DIRECTION_Y)
      return "ThrustDirection2";
   if (id == DIRECTION_Z)
      return "ThrustDirection3";
   
   return Hardware::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer Thruster::GetParameterID(const std::string &str) const
{
   for (Integer i = HardwareParamCount; i < ThrusterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - HardwareParamCount])
         return i;
   }
   
   if (str == "ThrustDirection1")
      return DIRECTION_X;
   if (str == "ThrustDirection2")
      return DIRECTION_Y;
   if (str == "ThrustDirection3")
      return DIRECTION_Z;
   
   if (str == "X_Direction" || str == "Element1")
   {
      WriteDeprecatedMessage(str, "ThrustDirection1");
      return DIRECTION_X;
   }
   if (str == "Y_Direction" || str == "Element2")
   {
      WriteDeprecatedMessage(str, "ThrustDirection2");
      return DIRECTION_Y;
   }
   if (str == "Z_Direction" || str == "Element3")
   {
      WriteDeprecatedMessage(str, "ThrustDirection3");
      return DIRECTION_Z;
   }
   
   return Hardware::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Thruster::GetParameterType(const Integer id) const
{
   if (id >= HardwareParamCount && id < ThrusterParamCount)
      return PARAMETER_TYPE[id - HardwareParamCount];

   return Hardware::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Thruster::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Thruster::IsParameterReadOnly(const Integer id) const
{
   if (id == THRUSTER_FIRING)
      return true;
   
   if ((id == ORIGIN || id == AXES))
      if (coordSystemName != "Local")
         return true;
   
   return Hardware::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterCommandModeSettable(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Tests to see if an object property can be set in Command mode
 *
 * @param id The ID of the object property
 *
 * @return true if the property can be set in command mode, false if not.
 */
//------------------------------------------------------------------------------
bool Thruster::IsParameterCommandModeSettable(const Integer id) const
{
   if ((id == COORDINATE_SYSTEM) || (id == AXES) ||
       (id == ORIGIN)            || (id == DECREMENT_MASS) ||
       (id == TANK))
      return false;

   // Activate all of the other thruster specific IDs
   if (id >= HardwareParamCount)
      return true;

   // Inherited parameters that can be set:
   if ((id == DIRECTION_X) || (id == DIRECTION_Y) || (id == DIRECTION_Z))
      return true;

   return Hardware::IsParameterCommandModeSettable(id);
}


//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real Thruster::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case DUTY_CYCLE:
         return dutyCycle;
      case THRUST_SCALE_FACTOR:
         return thrustScaleFactor;
      case GRAVITATIONAL_ACCELERATION:
         return gravityAccel;

      default:
         break;   // Default just drops through
   }

   return Hardware::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real Thruster::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_THRUSTER_SET
   MessageInterface::ShowMessage
      ("Thruster::SetRealParameter() '%s' entered, id=%d, value=%f\n",
       GetName().c_str(), id, value);
   #endif
   
   switch (id) 
   {
      // Other parameters
      case DUTY_CYCLE:
         if ((value >= 0.0) && (value <= 1.0))
            dutyCycle = value;
         else
         {
            HardwareException he;
            he.SetDetails(errorMessageFormat.c_str(),
                          GmatStringUtil::ToString(value, GetDataPrecision()).c_str(),
                          "DutyCycle", "0.0 <= Real Number <= 1.0");
            throw he;
         }
         return dutyCycle;
      case THRUST_SCALE_FACTOR:
         if (value >= 0.0)
            thrustScaleFactor = value;
         else
         {
            HardwareException he;
            he.SetDetails(errorMessageFormat.c_str(),
                          GmatStringUtil::ToString(value, GetDataPrecision()).c_str(),
                          "ThrustScaleFactor", "Real Number >= 0.0");
            throw he;
         }
         return thrustScaleFactor;
      case GRAVITATIONAL_ACCELERATION:
         if (value > 0.0)
            gravityAccel = value;
         else
         {
            HardwareException he;
            he.SetDetails(errorMessageFormat.c_str(),
                          GmatStringUtil::ToString(value, GetDataPrecision()).c_str(),
                          "GravitationalAccel", "Real Number > 0.0");
            throw he;
         }
         return gravityAccel;


      default:
         break;   // Default just drops through
   }
   
   return Hardware::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or throw ab=n exception if
 *         there is no string association.
 */
//---------------------------------------------------------------------------
std::string Thruster::GetStringParameter(const Integer id) const
{
   if (id == COORDINATE_SYSTEM)
      return coordSystemName;
   if (id == ORIGIN)
      return localOriginName;
   if (id == AXES)
      return localAxesName;
   
   return Hardware::GetStringParameter(id);
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new string for this parameter.
 *
 * @return true if the string is stored, throw if the string is not stored.
 */
//---------------------------------------------------------------------------
bool Thruster::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_THRUSTER_SET
   MessageInterface::ShowMessage
      ("Thruster::SetStringParameter() '%s' entered, id=%d, value='%s'\n",
       GetName().c_str(), id, value.c_str());
   #endif
   
   switch (id)
   {
   case COORDINATE_SYSTEM:
      coordSystemName = value;
      if (value == "Local")
         usingLocalCoordSys = true;
      else
         usingLocalCoordSys = false;
      return true;
   case ORIGIN:
      localOriginName = value;
      return true;
   case AXES:
      {
         localAxesName = value;
         if (find(localAxesLabels.begin(), localAxesLabels.end(), localAxesName)
             == localAxesLabels.end())
         {
            std::string framelist = localAxesLabels[0];
            for (UnsignedInt n = 1; n < localAxesLabels.size(); ++n)
               framelist += ", " + localAxesLabels[n];
            
            std::string msg =
               "The value of \"" + value + "\" for field \"Axes\""
               " on object \"" + instanceName + "\" is not an allowed value.\n"
               "The allowed values are: [ " + framelist + " ]. ";
            
            throw HardwareException(msg);
         }
         return true;
      }
   case TANK:
      if ((tankNames.size() > 0) && (value != tankNames[0]))
      {
         std::string errmsg;
         errmsg =  "The value of \"";
         errmsg += value;
         errmsg += "\" on Thruster \"";
         errmsg += instanceName;
         errmsg += "\" is not an allowed value.  GMAT does not currently "
               "support more than one tank per thruster; the thruster is "
               "already associated with tank \"";
         errmsg += tankNames[0];
         errmsg += "\".";
         throw HardwareException(errmsg);
      }
      // if not the same name push back
      if (find(tankNames.begin(), tankNames.end(), value) == tankNames.end())
         tankNames.push_back(value);
      return true;
   default:
      return Hardware::SetStringParameter(id, value);
   }
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string Thruster::GetStringParameter(const Integer id,
                                         const Integer index) const
{
   switch (id)
   {
   case TANK:
      {
         if (index >= 0 && index < (Integer)tankNames.size())
            return tankNames[index];
         else
            throw HardwareException
               ("Thruster::GetStringParameter() \"" + instanceName + ", " + 
                GmatStringUtil::ToString(index) +  " is invalid Tank index");
      }
   default:
      return Hardware::GetStringParameter(id, index);
   }
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value,
//                          const Integer index)
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool Thruster::SetStringParameter(const Integer id, const std::string &value,
                                  const Integer index)
{
   switch (id)
   {
   case TANK:
      {
         if (index > 0)
         {
            std::string errmsg;
            errmsg =  "The value of \"";
            errmsg += value;
            errmsg += "\" on Thruster \"";
            errmsg += instanceName;
            errmsg += "\" is not an allowed value.  GMAT does not currently "
                  "support more than one tank per thruster";
            throw HardwareException(errmsg);
         }

         if (index < (Integer)tankNames.size())
            tankNames[index] = value;
         else
            // Only add the tank if it is not in the list already
            if (find(tankNames.begin(), tankNames.end(), value) == tankNames.end()) 
               tankNames.push_back(value);
         
         return true;
      }
   default:
      return Hardware::SetStringParameter(id, value, index);
   }
}


//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Access an array of string data.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& Thruster::GetStringArrayParameter(const Integer id) const
{
   if (id == TANK)
      return tankNames;
   
   return Hardware::GetStringArrayParameter(id);
}


//---------------------------------------------------------------------------
//  bool GetBooleanParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 *
 * @return the boolean value for this parameter, or throw an exception if the
 *         parameter access in invalid.
 */
//---------------------------------------------------------------------------
bool Thruster::GetBooleanParameter(const Integer id) const
{
   if (id == THRUSTER_FIRING)
      return thrusterFiring;
   
   if (id == DECREMENT_MASS)
      return decrementMass;
   
   return Hardware::GetBooleanParameter(id);
}


//---------------------------------------------------------------------------
//  bool SetBooleanParameter(const Integer id, const bool value)
//---------------------------------------------------------------------------
/**
 * Sets the value for a boolean parameter.
 *
 * @param id The integer ID for the parameter.
 * @param value The new value.
 *
 * @return the boolean value for this parameter, or throw an exception if the
 *         parameter is invalid or not boolean.
 */
//---------------------------------------------------------------------------
bool Thruster::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == THRUSTER_FIRING) {
      #ifdef DEBUG_THRUSTER
         MessageInterface::ShowMessage(
            "Setting thruster %s firing mode to %s\n", instanceName.c_str(),
            (value == true ? "true" : "false"));
      #endif
      thrusterFiring = value;
      return thrusterFiring;
   }
   
   if (id == DECREMENT_MASS)
   {
      decrementMass = value;
      return decrementMass;
   }
   
   return Hardware::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
//  const StringArray& GetPropertyEnumStrings(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Access an array of enumerated string data.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a 
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& Thruster::GetPropertyEnumStrings(const Integer id) const
{
   if (id == AXES)
      return localAxesLabels;
   
   return GmatBase::GetPropertyEnumStrings(id);
}


//------------------------------------------------------------------------------
//  const StringArray& GetPropertyEnumStrings(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Access an array of enumerated string data.
 *
 * @param <label> The parameter name.
 *
 * @return The requested StringArray
 */
//------------------------------------------------------------------------------
const StringArray& Thruster::GetPropertyEnumStrings(const std::string &label) const
{
   return GetPropertyEnumStrings(GetParameterID(label));
}

//---------------------------------------------------------------------------
// Gmat::ObjectType GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
Gmat::ObjectType Thruster::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case TANK:
      return Gmat::FUEL_TANK;
   case COORDINATE_SYSTEM:
      return Gmat::COORDINATE_SYSTEM;
   case ORIGIN:
      return Gmat::CELESTIAL_BODY;
   default:
      return Hardware::GetPropertyObjectType(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
std::string Thruster::GetRefObjectName(const Gmat::ObjectType type) const
{
   #ifdef DEBUG_THRUSTER_REF_OBJ
   MessageInterface::ShowMessage
      ("Thruster::GetRefObjectName() <%p>'%s' entered, type=%d\n", this,
       GetName().c_str(), type);
   #endif
   
   std::string refObjName;
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      std::string refObjName;
      if (!usingLocalCoordSys)
         refObjName = coordSystemName;
      
      #ifdef DEBUG_THRUSTER_REF_OBJ
      MessageInterface::ShowMessage
         ("Thruster::GetRefObjectName() <%p>'%s' returning '%s'\n", this,
          GetName().c_str(), refObjName.c_str());
      #endif
      
      return refObjName;
   }
   
   return Hardware::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Thruster::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::COORDINATE_SYSTEM);
   refObjectTypes.push_back(Gmat::CELESTIAL_BODY);
   refObjectTypes.push_back(Gmat::SPACECRAFT);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// virtual const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
const StringArray& Thruster::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   #ifdef DEBUG_THRUSTER_REF_OBJ
   MessageInterface::ShowMessage
      ("Thruster::GetRefObjectNameArray() <%p>'%s' entered, type=%d\n",
       this, GetName().c_str(), type);
   #endif
   
   refObjectNames.clear();
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::COORDINATE_SYSTEM)
   {
      if (!usingLocalCoordSys)
         refObjectNames.push_back(coordSystemName);
   }
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::CELESTIAL_BODY)
   {
      if (usingLocalCoordSys)
      {
         refObjectNames.push_back(j2000BodyName);
         if (localOriginName != j2000BodyName)
            refObjectNames.push_back(localOriginName);
      }
   }
   
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACECRAFT)
   {
      if (satName != "")
         refObjectNames.push_back(satName);
   }
   
   #ifdef DEBUG_THRUSTER_REF_OBJ
   MessageInterface::ShowMessage
      ("Thruster::GetRefObjectNameArray(), refObjectNames.size()=%d\n",
       refObjectNames.size());
   for (UnsignedInt i=0; i<refObjectNames.size(); i++)
      MessageInterface::ShowMessage("   '%s'\n", refObjectNames[i].c_str());
   #endif
   
   return refObjectNames;
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Thruster::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Thruster::RenameRefObject() type=%s, oldName=%s, newName=%s\n",
       GetObjectTypeString(type).c_str(), oldName.c_str(), newName.c_str());
   #endif
   
   if (type != Gmat::HARDWARE && type != Gmat::COORDINATE_SYSTEM)
      return true;
   
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (coordSystemName == oldName)
         coordSystemName = newName;
   }

   if (type == Gmat::HARDWARE)
   {
      for (UnsignedInt i=0; i<tankNames.size(); i++)
      {
         if (tankNames[i] == oldName)
         {
            tankNames[i] = newName;
            break;
         }
      }
   }
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets referenced objects.
 *
 * @param obj The object.
 * @param type Type of the object.
 * @param name Name of the object.
 * 
 * @return true if the ref object was set, false if not.
 */
//------------------------------------------------------------------------------
bool Thruster::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                            const std::string &name)
{
   #ifdef DEBUG_THRUSTER_REF_OBJ
   MessageInterface::ShowMessage
      ("Thruster::SetRefObject() <%p>'%s' entered, obj=<%p>, type=%d, name='%s'\n",
       this, GetName().c_str(), obj, type, name.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   if (type == Gmat::COORDINATE_SYSTEM && obj->GetType() == Gmat::COORDINATE_SYSTEM)
   {
      if (coordSystemName == name)
      {
         coordSystem = (CoordinateSystem*)obj;
         #ifdef DEBUG_THRUSTER_REF_OBJ
         MessageInterface::ShowMessage
            ("Thruster::SetRefObject() leaving, the CoordinateSystem set to <%p>\n",
             coordSystem);
         #endif
      }
      return true;
   }
   
   if (type == Gmat::CELESTIAL_BODY && obj->GetType() == Gmat::CELESTIAL_BODY &&
       localOriginName == name)
   {
      localOrigin = (CelestialBody*)obj;
      return true;
   }
   
   if (type == Gmat::CELESTIAL_BODY && obj->GetType() == Gmat::CELESTIAL_BODY &&
       j2000BodyName == name)
   {
      j2000Body = (CelestialBody*)obj;
      return true;
   }
   
   if (obj->GetType() == Gmat::SPACECRAFT)
   {
      return SetSpacecraft((Spacecraft*)obj);
   }
   
//   if (obj->GetTypeName() == "FuelTank")
   if (obj->IsOfType("FuelTank"))
   {
      #ifdef DEBUG_THRUSTER_REF_OBJ
         MessageInterface::ShowMessage("   Setting tank \"%s\" on thruster \"%s\"\n",
                                       name.c_str(), instanceName.c_str());
      #endif
      
      if (tanks.empty())
      {
         tanks.push_back((FuelTank*)obj);
         #ifdef DEBUG_THRUSTER_REF_OBJ
         MessageInterface::ShowMessage
            ("   <%p>'%s' added to tank list, now size is %d\n",
             obj, obj->GetName().c_str(), tanks.size());
         #endif
      }
      else if (find(tanks.begin(), tanks.end(), obj) == tanks.end())
      {
         #ifdef DEBUG_THRUSTER_REF_OBJ
         MessageInterface::ShowMessage
            ("   ------ the tank pointer of name %s was NOT found in the array (of size %d)\n",
                  name.c_str(), (Integer) tanks.size());
         #endif
         // Replace old tank with new one. We don't want to delete the
         // old tank here since Spacecraft owns it (tank is not cloned in the
         // copy constructor)
         for (UnsignedInt i=0; i<tanks.size(); i++)
         {
            #ifdef DEBUG_THRUSTER_REF_OBJ
            if (!tanks[i])
               MessageInterface::ShowMessage("--- tank pointer is : NULL\n");
            else
               MessageInterface::ShowMessage("--- tank pointer is : %p\n", tanks[i]);
            std::string theType = PARAM_TYPE_STRING[tanks[i]->GetType()];
            MessageInterface::ShowMessage
               ("   ---- tanks %d type is %s\n", (Integer) i, theType.c_str());
            #endif
            if (tanks[i]->IsOfType(Gmat::FUEL_TANK))
            {
               #ifdef DEBUG_THRUSTER_REF_OBJ
               std::string theType = PARAM_TYPE_STRING[tanks[i]->GetType()];
               MessageInterface::ShowMessage
                  ("   ---- tanks %d name is %s, name = %s\n",
                        tanks[i]->GetName().c_str(), name.c_str());
               #endif
               if (tanks[i]->GetName() == name)
               {
                  #ifdef DEBUG_THRUSTER_REF_OBJ
                  MessageInterface::ShowMessage
                     ("   <%p>'%s' will replace the old tank <%p>'%s'\n", obj,
                      obj->GetName().c_str(), tanks[i], tanks[i]->GetName().c_str());
                  #endif
                  tanks[i] = (FuelTank*)obj;
               }
            }
         }
      }
      
      #ifdef DEBUG_THRUSTER
         // Peek at the mass flow data
         bool temp = thrusterFiring;
         thrusterFiring = true;
         CalculateMassFlow();
         thrusterFiring = temp;
      #endif
      
      #ifdef DEBUG_THRUSTER_REF_OBJ
      MessageInterface::ShowMessage
         ("Thruster::SetRefObject() <%p>'%s' returning true\n", this, GetName().c_str());
      #endif
      return true;
   }
   
   return Hardware::SetRefObject(obj, type, name);
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers by type.
 *
 * @param type The type of objects requested
 *
 * @return Reference to the array.
 */
//---------------------------------------------------------------------------
ObjectArray& Thruster::GetRefObjectArray(const Gmat::ObjectType type)
{
   if (type == Gmat::HARDWARE)
   {
      tempArray.clear();
      for (UnsignedInt i = 0; i < tanks.size(); ++i)
         tempArray.push_back(tanks[i]);
      return tempArray;
   }

   return Hardware::GetRefObjectArray(type);
}


//---------------------------------------------------------------------------
//  ObjectArray& GetRefObjectArray(const std::string& typeString)
//---------------------------------------------------------------------------
/**
 * Obtains an array of GmatBase pointers based on a string (e.g. the typename).
 *
 * @param typeString The string used to find the objects requested.
 *
 * @return Reference to the array.  This default method returns an empty vector.
 */
//---------------------------------------------------------------------------
ObjectArray& Thruster::GetRefObjectArray(const std::string& typeString)
{
//   if ((typeString == "FuelTank") || (typeString == "Tanks"))
//      return tanks;

   return Hardware::GetRefObjectArray(typeString);
}


//---------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//---------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * Thrusters use this method to clear the pointers and names of tanks used for
 * a burn, prior to reassigning the tanks.
 *
 * @param <action> The string descriptor for the requested action.
 * @param <actionData> Optional data used for the action.
 *
 * @return true if the action was performed, false if not.
 */
//---------------------------------------------------------------------------
bool Thruster::TakeAction(const std::string &action,
                          const std::string &actionData)
{
   if (action == "ClearTanks") {
//      if (thrusterFiring)
//         throw HardwareException("Thruster " + instanceName +
//            " is attempting to remove fuel tank access during a finite burn");

      tankNames.clear();
      tanks.clear();

      return true;
   }

   return Hardware::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
//  void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the internal solar system pointer for objects that have one.
 *
 * @param ss   The solar system.
 */
//------------------------------------------------------------------------------
void Thruster::SetSolarSystem(SolarSystem *ss)
{
   #ifdef DEBUG_THRUSTER_REF_OBJ
   MessageInterface::ShowMessage
      ("Thruster::SetSolarSystem() ss=<%p> '%s'\n", ss, ss->GetName().c_str());
   #endif
   
   if (solarSystem != ss)
   {
      solarSystem = ss;
      
      if (usingLocalCoordSys)
      {
         localOrigin = solarSystem->GetBody(localOriginName);
         j2000Body = solarSystem->GetBody(j2000BodyName);

         if (localOrigin == NULL)
         {
            HardwareException he;
            he.SetDetails(errorMessageFormat.c_str(),
                  localOriginName.c_str(), "Origin",
                  "an object with physical location in space");
            throw he;
         }
      }
   }
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Sets up the bodies used in the thrust calculations.
 */
//------------------------------------------------------------------------------
bool Thruster::Initialize()
{
   #ifdef DEBUG_THRUSTER_INIT
   MessageInterface::ShowMessage
      ("Thruster::Initialize() <%p>'%s' entered, spacecraft=<%p>, "
       "usingLocalCoordSys=%d, coordSystem=<%p>, localCoordSystem=<%p>\n",
       this, GetName().c_str(), spacecraft, usingLocalCoordSys,
       coordSystem, localCoordSystem);
   #endif
   
   bool retval = Hardware::Initialize();
   if (!retval)
      return false;
   
   if (!usingLocalCoordSys)
   {
      if (coordSystem == NULL)
         throw HardwareException
            ("Thruster::Initialize() the Coordinate System \"" + coordSystemName +
             "\" has not been set.");
      
      // If spacecraft is not set this time, just return true
      // This instance of Thruster may be just cloned one from the Sandbox
      if (spacecraft == NULL)
         return true;
   }
   else
   {
      if (solarSystem == NULL)
         throw HardwareException
            ("Unable to initialize the Thruster object \"" + 
             instanceName + "\"; " + "\"SolarSystem\" was not set.");
      
      if (!j2000Body)
         throw HardwareException
            ("Unable to initialize the thruster object \"" + 
             instanceName + "\"; \"" + j2000BodyName + "\" was not set.");
      
      if (!localOrigin)
         throw HardwareException
            ("Unable to initialize the thruster object \"" + 
             instanceName + "\"; \"" + localOriginName + "\" \" was not set.");

      #ifdef DEBUG_THRUSTER_INIT
         MessageInterface::ShowMessage("Initializing thruster local system at "
            "body %s with J2000 body %s\n", localOrigin->GetName().c_str(), 
            j2000Body->GetName().c_str());      
      #endif

      // delete old local coordinate system
      if (localCoordSystem != NULL)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (localCoordSystem, "localCS", "Thruster::Initialize()",
             "deleting localCoordSystem");
         #endif
         delete localCoordSystem;
         localCoordSystem = NULL;
      }
      
      // If spacecraft is available, create new local coordinate system
      if (spacecraft != NULL)
         localCoordSystem = CreateLocalCoordinateSystem();
      
      // localCoordSystem is still NULL, just return
      if (localCoordSystem == NULL)
      {
         #ifdef DEBUG_THRUSTER_INIT
         MessageInterface::ShowMessage
            ("Thruster::Initialize() <%p>'%s' returning false, localCoordSystem is NULL\n",
             this, GetName().c_str());
         #endif
         return false;
      }

      #ifdef DEBUG_THRUSTER_CONVERT
         MessageInterface::ShowMessage
            ("Local thruster coord system is\n%s\n",localCoordSystem->
                  GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      #endif

   }
   
   // Convert direction to inertial coord system
   Real epoch = spacecraft->GetRealParameter("A1Epoch");
   ConvertDirectionToInertial(direction, inertialDirection, epoch);
   
   #ifdef DEBUG_THRUSTER_INIT
   MessageInterface::ShowMessage
      ("   Inertial thrust direction:  %18le  %18le  %18le\n",
       inertialDirection[0], inertialDirection[1], inertialDirection[2]);
   MessageInterface::ShowMessage
      ("   %s tank mass computation\n", decrementMass ? "Continue with " : "Skipping");
   #endif
   
   #ifdef DEBUG_THRUSTER_INIT
   MessageInterface::ShowMessage
      ("Thruster::Initialize() <%p>'%s' returning %s\n", this, GetName().c_str(),
       retval ? "true" : "false");
   #endif
   
   return retval;
}

//------------------------------------------------------------------------------
//  bool SetPower(Real allocatedPower)
//------------------------------------------------------------------------------
/**
 * Sets the allocated power level for the thruster.  This method does not
 * do much in this base class; it must be overridden in a class that
 * needs more functionality (e.g. ElectricThruster class).
 */
//------------------------------------------------------------------------------
bool Thruster::SetPower(Real allocatedPower)
{
#ifdef DEBUG_SET_POWER
   MessageInterface::ShowMessage(">>>>>>>>>> Setting power for thruster %s to %12.10\n",
         instanceName.c_str(), allocatedPower);
#endif
   power = allocatedPower;
   return true;
}


//------------------------------------------------------------------------------
//  bool SetSpacecraft(Spacecraft *sat)
//------------------------------------------------------------------------------
/**
 * Accessor method to pass in the spacecraft pointer. This method is usually
 * called during the Sandbox initialization when building Spacecraft owned
 * objects such as tanks and thrusters. The spacecraft passes itself to this
 * class instance using Thruster::SetRefObject() in Spacecraft::SetRefObject().
 *
 * This method creates new local coordinate system if current spacecraft is not
 * the same as old spacecraft.
 * 
 * @param <sat> the Spacecraft
 */
//------------------------------------------------------------------------------
bool Thruster::SetSpacecraft(Spacecraft *sat)
{
   if (sat == NULL)
      return false;
   
   #ifdef DEBUG_THRUSTER_REF_OBJ
   MessageInterface::ShowMessage
      ("Thruster::SetSpacecraft() sat=<%p>'%s', usingLocalCoordSys=%d, "
       "localCoordSystem=<%p>\n", sat, sat->GetName().c_str(), usingLocalCoordSys,
       localCoordSystem);
   #endif
   
   // If spacecraft is different
   if (spacecraft != sat)
   {
      spacecraft = sat;
      satName = spacecraft->GetName();
      
      // create new local coordinate system
      if (usingLocalCoordSys)
      {
         if (localCoordSystem)
         {
            #ifdef DEBUG_MEMORY
            MemoryTracker::Instance()->Remove
               (localCoordSystem, "localCS", "Thruster::SetSpacecraft()",
                "deleting localCoordSystem");
            #endif
            delete localCoordSystem;
            localCoordSystem = NULL;
         }
         localCoordSystem = CreateLocalCoordinateSystem();
         
         #ifdef DEBUG_THRUSTER_REF_OBJ
         MessageInterface::ShowMessage
            ("   new localCoordSystem <%p> created\n", localCoordSystem);
         #endif
      }
   }
   
   #ifdef DEBUG_THRUSTER_REF_OBJ
   MessageInterface::ShowMessage("Thruster::SetSpacecraft() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// CoordinateSystem* CreateLocalCoordinateSystem()
//------------------------------------------------------------------------------
CoordinateSystem* Thruster::CreateLocalCoordinateSystem()
{
   #ifdef DEBUG_THRUSTER_INIT
   MessageInterface::ShowMessage
      ("Thruster::CreateLocalCoordinateSystem() <%p>'%s' entered, usingLocalCoordSys=%d, "
       "spacecraft=<%p>\n", this, GetName().c_str(), usingLocalCoordSys, spacecraft);
   #endif
   
   CoordinateSystem *localCS = NULL;
   
   // If coordinate system being used is local, then create
   if (usingLocalCoordSys)
   {
      if (solarSystem == NULL)
      {
         // Since SolarSystem may be set later, just return NULL for now
         #ifdef DEBUG_THRUSTER_INIT
         MessageInterface::ShowMessage
            ("Thruster::CreateLocalCoordinateSystem() SolarSystem is not set so, "
             "returning NULL\n");
         #endif
         return NULL;
         
         //throw HardwareException
         //   ("Unable to initialize the Thruster object " + 
         //    instanceName + " " + "\"SolarSystem\" was not set for the thruster.");
      }
      
      if (spacecraft == NULL)
      {
         // Since spacecraft may be set later, just return NULL for now
         #ifdef DEBUG_THRUSTER_INIT
         MessageInterface::ShowMessage
            ("Thruster::CreateLocalCoordinateSystem() spacecraft is not set so, "
             "returning NULL\n");
         #endif
         return NULL;
         
         //throw HardwareException("Unable to initialize the Thruster object " + 
         //   instanceName + " " + satName + " was not set for the thruster.");
      }
            
      // Call CoordinateSystem static method to create a local coordinate system
      localOrigin = solarSystem->GetBody(localOriginName);
      localCS = CoordinateSystem::CreateLocalCoordinateSystem
         ("Local", localAxesName, spacecraft, localOrigin, spacecraft,
          j2000Body, solarSystem);

      if (localCS == NULL)
         return NULL;
      
      #ifdef DEBUG_THRUSTER_INIT
      MessageInterface::ShowMessage("Created local coordinate system:\n%s\n",
         localCS->GetGeneratingString(Gmat::NO_COMMENTS, "   ").c_str());
      #endif

      if (localAxesName == "MJ2000Eq")
         isMJ2000EqAxes = true;
      else if (localAxesName == "SpacecraftBody")
         isSpacecraftBodyAxes = true;
      
   }
   else
   {
      // If not using local cooordinate system, then it is using configured CS and
      // it should have been set by this time
      if (coordSystem == NULL)
      {
         throw HardwareException
            ("Unable to initialize the Thruster object " + 
             instanceName + " " + coordSystemName + " was not set for the thruster.");
      }
      localCS = coordSystem;
   }
   
   #ifdef DEBUG_THRUSTER_INIT
   MessageInterface::ShowMessage
      ("Thruster::CreateLocalCoordinateSystem() <%p>'%s' returning <%p>\n",
       this, GetName().c_str(), localCS);
   #endif
   
   return localCS;
}


//------------------------------------------------------------------------------
// void ConvertDirectionToInertial(Real *dir, Real *dirInertial, Real epoch)
//------------------------------------------------------------------------------
/*
 * Converts thruster direction to inertial frame
 *
 * @param dir  Thruster direction in thruster frame
 * @param dirInertial  Thruster direction in inertial frame
 * @param epoch  Epoch to be used for conversion
 */
//------------------------------------------------------------------------------
void Thruster::ConvertDirectionToInertial(Real *dir, Real *dirInertial, Real epoch)
{
   #ifdef DEBUG_THRUSTER_CONVERT
   MessageInterface::ShowMessage
      ("Thruster::ConvertDirectionToInertial() <%p>'%s' entered, epoch=%.15f\n   "
       "dir=%.15f %.15f %.15f\n", this, GetName().c_str(), epoch, dir[0], dir[1], dir[2]);
   MessageInterface::ShowMessage
      ("   usingLocalCoordSys=%s, coordSystemName='%s', coordSystem=<%p>, "
       "localCoordSystem=<%p>\n", (usingLocalCoordSys?"true":"false"),
       coordSystemName.c_str(), coordSystem, localCoordSystem);
   #endif
   
   if (usingLocalCoordSys && localCoordSystem == NULL)
   {      
      // try creating localCoordSystem
      //localCoordSystem = CreateLocalCoordinateSystem();
      // try Initialize again (LOJ: 2009.07.28)
      Initialize();
      if (!localCoordSystem)
         throw HardwareException
            ("Unable to convert thrust direction to Inertial, the local Coordinate "
             "System has not been created.");
   }
   else if (!usingLocalCoordSys && coordSystem == NULL)
   {
      throw HardwareException
         ("Unable to convert thrust direction to Inertial, the Coordinate "
          "System \"" + coordSystemName + "\" has not been set.");      
   }
   
   Real inDir[6], outDir[6];
   for (Integer i=0; i<3; i++)
      inDir[i] = dir[i];
   for (Integer i=3; i<6; i++)
      inDir[i] = 0.0;
   
   // if not using local CS, use ref CoordinateSystem
   if (!usingLocalCoordSys)
   {     
//      // Now rotate to MJ2000Eq axes, we don't want to translate so
//      // set coincident to true
//      coordSystem->ToMJ2000Eq(epoch, inDir, outDir, true);
      // Now rotate to base system axes, we don't want to translate so
      // set coincident to true
      coordSystem->ToBaseSystem(epoch, inDir, outDir, true); // @todo - need ToMJ2000Eq here?
      
      #ifdef DEBUG_BURN_CONVERT_ROTMAT
      Rmatrix33 rotMat = coordSystem->GetLastRotationMatrix();
      MessageInterface::ShowMessage
         ("rotMat=\n%s\n", rotMat.ToString(16, 20).c_str());
      #endif
      
      dirInertial[0] = outDir[0];
      dirInertial[1] = outDir[1];
      dirInertial[2] = outDir[2];
   }
   else
   {
      // if MJ2000Eq axes rotation matrix is always identity matrix
      if (isMJ2000EqAxes)
      {

         dirInertial[0] = dir[0];
         dirInertial[1] = dir[1];
         dirInertial[2] = dir[2];
      }
      else if (isSpacecraftBodyAxes)
      {
         Rvector3 inDir(dir[0], dir[1], dir[2]);
         Rvector3 outDir;
         // Get attitude matrix from Spacecraft and transpose since
         // attitude matrix from spacecraft gives rotation matrix from
         // inertial to body
         Rmatrix33 inertialToBody = spacecraft->GetAttitude(epoch);
         Rmatrix33 rotMat = inertialToBody.Transpose();
         outDir = inDir * rotMat;
         for (Integer i=0; i<3; i++)
            dirInertial[i] = outDir[i];
      }
      else
      {         
//         // Now rotate to MJ2000Eq axes
//         localCoordSystem->ToMJ2000Eq(epoch, inDir, outDir, true);
         // Now rotate to base system axes
         #ifdef DEBUG_THRUSTER_CONVERT
            MessageInterface::ShowMessage("Converting thruster burn direction "
                  "using local CS:\n%s\n", localCoordSystem->
                  GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         #endif


         localCoordSystem->ToBaseSystem(epoch, inDir, outDir, true);  // @todo - do we need ToMJ2000Eq here?
         
         dirInertial[0] = outDir[0];
         dirInertial[1] = outDir[1];
         dirInertial[2] = outDir[2];
      }
   }
   
   #ifdef DEBUG_THRUSTER_CONVERT
   MessageInterface::ShowMessage
      ("Thruster::ConvertDirectionToInertial() returning\n   "
       "  direction = %.15f %.15f %.15f\n   "
       "dirInertial = %.15f %.15f %.15f\n",
       direction[0], direction[1], direction[2],
       dirInertial[0], dirInertial[1], dirInertial[2]);
   #endif
}


//---------------------------------------------------------------------------
// void ComputeInertialDirection(Real epoch)
//---------------------------------------------------------------------------
void Thruster::ComputeInertialDirection(Real epoch)
{
   ConvertDirectionToInertial(direction, inertialDirection, epoch);
}


//------------------------------------------------------------------------------
// void WriteDeprecatedMessage(const std::string &oldProp,
//                             const std::string &newProp) const
//------------------------------------------------------------------------------
void Thruster::WriteDeprecatedMessage(const std::string &oldProp,
                                      const std::string &newProp) const
{
   // Write only one message per session
   static bool writeXDirection = true;
   static bool writeYDirection = true;
   static bool writeZDirection = true;
   static bool writeElement1 = true;
   static bool writeElement2 = true;
   static bool writeElement3 = true;
   
   if ((oldProp == "X_Direction" && writeXDirection) ||
       (oldProp == "Y_Direction" && writeYDirection) ||
       (oldProp == "Z_Direction" && writeZDirection) ||
       (oldProp == "Element1" && writeElement1) ||
       (oldProp == "Element2" && writeElement2) ||
       (oldProp == "Element3" && writeElement3))
   {
      MessageInterface::ShowMessage
         ("*** WARNING *** \"" + oldProp + "\" of Thruster orientation is "
          "deprecated and will be removed from a future build; please use \"" +
          newProp + "\" instead.\n");
   }
   
   if (oldProp == "X_Direction")
      writeXDirection = false;
   else if (oldProp == "Y_Direction")
      writeYDirection = false;
   else if (oldProp == "Z_Direction")
      writeYDirection = false;
   else if (oldProp == "Element1")
      writeElement1 = false;
   else if (oldProp == "Element2")
      writeElement2 = false;
   else if (oldProp == "Element3")
      writeElement3 = false;
   
}

