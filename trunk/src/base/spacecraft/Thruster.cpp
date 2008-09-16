//$Header$
//------------------------------------------------------------------------------
//                               Thruster
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
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
#include <sstream>
#include <math.h>          // for pow(real, real)

// #define DEBUG_THRUSTER

#ifdef DEBUG_THRUSTER
   #include "MessageInterface.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

/// Labels used for the thruster element parameters.
const std::string
Thruster::PARAMETER_TEXT[ThrusterParamCount - HardwareParamCount] =
{
   "Tank",
   "C1",  "C2",  "C3",  "C4",  "C5",  "C6",  "C7", 
   "C8",  "C9", "C10", "C11", "C12", "C13", "C14", 
   "K1",  "K2",  "K3",  "K4",  "K5",  "K6",  "K7", 
   "K8",  "K9", "K10", "K11", "K12", "K13", "K14", 
   "IsFiring",
   "CoordinateSystem",
   "ThrustScaleFactor"
};

/// Types of the parameters used by thrusters.
const Gmat::ParameterType
Thruster::PARAMETER_TYPE[ThrusterParamCount - HardwareParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::REAL_TYPE, Gmat::REAL_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
};


//------------------------------------------------------------------------------
//  Thruster(std::string nomme)
//------------------------------------------------------------------------------
/**
 * Thruster constructor.
 *
 * @param nomme Name of the thruster.
 */
//------------------------------------------------------------------------------
Thruster::Thruster(std::string nomme) :
   Hardware             (Gmat::HARDWARE, "Thruster", nomme),
   coordinateName       ("MJ2000EarthEquator"),
   // theCoordinates       (NULL),
   thrusterFiring       (false),
   thrustScaleFactor    (1.0),
   pressure             (1500.0),
   temperatureRatio     (1.0),
   thrust               (500.0),
   impulse              (2150.0),
   constantExpressions  (true),
   simpleExpressions    (true),
   mDot                 (0.0)
{
   objectTypes.push_back(Gmat::THRUSTER);
   objectTypeNames.push_back("Thruster");
   parameterCount = ThrusterParamCount;

   cCoefficients[0]  = 500.0;
   cCoefficients[10] = 1.0;
   cCoefficients[1]  = cCoefficients[2] =  cCoefficients[3] = 
   cCoefficients[4]  = cCoefficients[5] =  cCoefficients[6] = 
   cCoefficients[7]  = cCoefficients[8] =  cCoefficients[9] = 
   cCoefficients[11] = cCoefficients[12] = cCoefficients[13] = 0.0;

   kCoefficients[0]  = 2150.0;
   kCoefficients[10] = 1.0;
   kCoefficients[1]  = kCoefficients[2]  = kCoefficients[3] = 
   kCoefficients[4]  = kCoefficients[5]  = kCoefficients[6] = 
   kCoefficients[7]  = kCoefficients[8]  = kCoefficients[9] = 
   kCoefficients[11] = kCoefficients[12] = kCoefficients[13] = 0.0;
   
   thrustDirection[0] = 1.0;
   thrustDirection[1] = 0.0;
   thrustDirection[2] = 0.0;
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
}


//------------------------------------------------------------------------------
//  Thruster(const Thruster& th)
//------------------------------------------------------------------------------
/**
 * Thruster copy constructor.
 * 
 * @param th The object being copied.
 */
//------------------------------------------------------------------------------
Thruster::Thruster(const Thruster& th) :
   Hardware             (th),
   tankNames            (th.tankNames),
   coordinateName       (th.coordinateName),
   // theCoordinates       (th.theCoordinates), // Assumes CS's are globally accessed
   thrusterFiring       (th.thrusterFiring),
   thrustScaleFactor    (th.thrustScaleFactor),
   pressure             (th.pressure),
   temperatureRatio     (th.temperatureRatio),
   thrust               (th.thrust),
   impulse              (th.impulse),
   constantExpressions  (th.constantExpressions),
   simpleExpressions    (th.simpleExpressions),
   mDot                 (th.mDot)
{
   parameterCount = th.parameterCount;
   
   memcpy(cCoefficients, th.cCoefficients, 14 * sizeof(Real));
   memcpy(kCoefficients, th.kCoefficients, 14 * sizeof(Real));
   
   thrustDirection[0] = th.thrustDirection[0];
   thrustDirection[1] = th.thrustDirection[1];
   thrustDirection[2] = th.thrustDirection[2];
   
   tanks.clear();
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
 */
//------------------------------------------------------------------------------
Thruster& Thruster::operator=(const Thruster& th)
{
   if (&th == this)
      return *this;

   Hardware::operator=(th);

   memcpy(cCoefficients, th.cCoefficients, 14 * sizeof(Real));
   memcpy(kCoefficients, th.kCoefficients, 14 * sizeof(Real));
   
   thrustDirection[0] = th.thrustDirection[0];
   thrustDirection[1] = th.thrustDirection[1];
   thrustDirection[2] = th.thrustDirection[2];

   tankNames = th.tankNames;
   tanks.clear();
   coordinateName = th.coordinateName;

   // theCoordinates = th.theCoordinates; // Assumes CS's are globally accessed
   thrusterFiring = th.thrusterFiring;
   thrustScaleFactor = th.thrustScaleFactor;
   pressure = th.pressure;
   temperatureRatio = th.temperatureRatio;
   thrust = th.thrust;
   impulse = th.impulse;
   constantExpressions = th.constantExpressions;
   simpleExpressions = th.simpleExpressions;
   mDot = th.mDot;

   return *this;
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
 * @return true if the name was set, false if not.
 */
//------------------------------------------------------------------------------
bool Thruster::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                            const std::string &name)
{
   if (obj->GetTypeName() == "FuelTank") {
      #ifdef DEBUG_THRUSTER
         MessageInterface::ShowMessage("Setting tank \"%s\" on thruster \"%s\"\n",
                                       name.c_str(), instanceName.c_str());
      #endif
      
      if (find(tanks.begin(), tanks.end(), obj) == tanks.end())
         tanks.push_back((FuelTank*)obj);

      #ifdef DEBUG_THRUSTER
         // Peek at the mass flow data
         bool temp = thrusterFiring;
         thrusterFiring = true;
         CalculateMassFlow();
         thrusterFiring = temp;
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
//   if (type == Gmat::HARDWARE)
//      return tanks;
      
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
      return "Element1";
   if (id == DIRECTION_Y)
      return "Element2";
   if (id == DIRECTION_Z)
      return "Element3";
   
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
   
   if (str == "Element1")
      return DIRECTION_X;
   if (str == "Element2")
      return DIRECTION_Y;
   if (str == "Element3")
      return DIRECTION_Z;
   
   if (str == "X_Direction")
   { 
      MessageInterface::ShowMessage("\"X_Direction\" thruster orientations "
         "are deprecated and will be removed from a future build; please use "
         "\"Element1\" instead.\n");
      return DIRECTION_X;
   }
   if (str == "Y_Direction")
   { 
      MessageInterface::ShowMessage("\"Y_Direction\" thruster orientations "
         "are deprecated and will be removed from a future build; please use "
         "\"Element2\" instead.\n");
      return DIRECTION_Y;
   }
   if (str == "Z_Direction")
   { 
      MessageInterface::ShowMessage("\"Z_Direction\" thruster orientations "
         "are deprecated and will be removed from a future build; please use "
         "\"Element3\" instead.\n");
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
   //Temporary change until we decide some tank/thruster issues.
   // if ((id == THRUSTER_FIRING) || (id == TANK))
   if ((id == THRUSTER_FIRING))
      return true;

   return Hardware::IsParameterReadOnly(id);
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
   switch (id) {
      case C1:
         return cCoefficients[0];
      case C2:
         return cCoefficients[1];
      case C3:
         return cCoefficients[2];
      case C4:
         return cCoefficients[3];
      case C5:
         return cCoefficients[4];
      case C6:
         return cCoefficients[5];
      case C7:
         return cCoefficients[6];
      case C8:
         return cCoefficients[7];
      case C9:
         return cCoefficients[8];
      case C10:
         return cCoefficients[9];
      case C11:
         return cCoefficients[10];
      case C12:
         return cCoefficients[11];
      case C13:
         return cCoefficients[12];
      case C14:
         return cCoefficients[13];

      case K1:
         return kCoefficients[0];
      case K2:
         return kCoefficients[1];
      case K3:
         return kCoefficients[2];
      case K4:
         return kCoefficients[3];
      case K5:
         return kCoefficients[4];
      case K6:
         return kCoefficients[5];
      case K7:
         return kCoefficients[6];
      case K8:
         return kCoefficients[7];
      case K9:
         return kCoefficients[8];
      case K10:
         return kCoefficients[9];
      case K11:
         return kCoefficients[10];
      case K12:
         return kCoefficients[11];
      case K13:
         return kCoefficients[12];
      case K14:
         return kCoefficients[13];
      case THRUST_SCALE_FACTOR:
         return thrustScaleFactor;
         
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
   switch (id) {
      case C1:
         return cCoefficients[0] = value;
      case C2:
         if (value != 0.0)
            constantExpressions = false;
         return cCoefficients[1] = value;
      case C3:
         if (value != 0.0)
            constantExpressions = false;
         return cCoefficients[2] = value;
      case C4:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[3] = value;
      case C5:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[4] = value;
      case C6:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[5] = value;
      case C7:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[6] = value;
      case C8:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[7] = value;
      case C9:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[8] = value;
      case C10:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[9] = value;
      case C11:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[10] = value;
      case C12:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return cCoefficients[11] = value;
      case C13:
         return cCoefficients[12] = value;
      case C14:
         return cCoefficients[13] = value;
      case K1:
         return kCoefficients[0] = value;
      case K2:
         if (value != 0.0)
            constantExpressions = false;
         return kCoefficients[1] = value;
      case K3:
         if (value != 0.0)
            constantExpressions = false;
         return kCoefficients[2] = value;
      case K4:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[3] = value;
      case K5:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[4] = value;
      case K6:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[5] = value;
      case K7:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[6] = value;
      case K8:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[7] = value;
      case K9:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[8] = value;
      case K10:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[9] = value;
      case K11:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[10] = value;
      case K12:
         if (value != 0.0) {
            constantExpressions = false;
            simpleExpressions = false;
         }
         return kCoefficients[11] = value;
      case K13:
         return kCoefficients[12] = value;
      case K14:
         return kCoefficients[13] = value;
      case THRUST_SCALE_FACTOR:
//         if (value > 0.0)
//            thrustScaleFactor = value;
//         return thrustScaleFactor;
         
         if (value >= 0.0)
            thrustScaleFactor = value;
         else
         {
            std::stringstream buffer;
            buffer << value;
            throw HardwareException(
               "The value of \"" + buffer.str() + "\" for field \"Thrust Scale Factor\""
               " on object \"" + instanceName + "\" is not an allowed value.\n"
               "The allowed values are: [ Real Number >= 0.0 ]. ");
         }
         return thrustScaleFactor;
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
std::string Thruster::GetStringParameter(const Integer id) const
{
   if (id == COORDINATE_SYSTEM)
      return coordinateName;
      
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
bool Thruster::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == COORDINATE_SYSTEM) {
      coordinateName = value;
      return true;
   }

   if (id == TANK) {
      tankNames.push_back(value);
      return true;
   }

   return Hardware::SetStringParameter(id, value);
}


// std::string Thruster::GetStringParameter(const Integer id,
//                                          const Integer index) const
// {
// }

// bool Thruster::SetStringParameter(const Integer id, const std::string &value,
//                                   const Integer index)
// {
// }


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

   return Hardware::SetBooleanParameter(id, value);
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


//---------------------------------------------------------------------------
//  GmatBase* Clone() const
//---------------------------------------------------------------------------
/**
 * Provides a clone of this object by calling the copy constructor.
 *
 * @return A GmatBase pointer to the cloned thruster.
 */
//---------------------------------------------------------------------------
GmatBase* Thruster::Clone() const
{
   return new Thruster(*this);
}


//---------------------------------------------------------------------------
//  void Copy(GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 *
 * @return A GmatBase pointer to the cloned thruster.
 */
//---------------------------------------------------------------------------
void Thruster::Copy(const GmatBase* orig)
{
   operator=(*((Thruster *)(orig)));
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
   
   if (type != Gmat::HARDWARE)
      return true;
   
   for (UnsignedInt i=0; i<tankNames.size(); i++)
   {
      if (tankNames[i] == oldName)
      {
         tankNames[i] = newName;
         break;
      }
   }
   
   return true;
}


//---------------------------------------------------------------------------
//  bool CalculateThrustAndIsp()
//---------------------------------------------------------------------------
/**
 * Evaluates the thrust and specific impulse polynomials.
 *
 * GMAT uses polynomial expressions for the thrust and specific impulse
 * imparted to the spacecraft by thrusters attached to the spacecraft.
 * Both thrust and specific impulse are expressed as functions of pressure
 * and temperature. The pressure and temperature are values obtained
 * from fuel tanks containing the fuel. All measurements in GMAT are
 * expressed in metric units. The thrust, in Newtons, applied by a spacecraft
 * engine is given by
 * 
   \f[
    F_{T}(P,T) = \left\{ C_{1}+C_{2}P+C_{3}P^{2}+C_{4}P^{C_{5}}+
                   C_{6}P^{C_{7}}+C_{8}P^{C_{9}}+
                   C_{10}C_{11}^{C_{12}P}\right\} \left(\frac{T}{T_{ref}}
                   \right)^{1+C_{13}+C_{14}P}\f]
 *
 * Pressures are expressed in kilopascals, and temperatures in degrees
 * centigrade. The coefficients C1 - C14 are set by the user. Each coefficient
 * is expressed in units commiserate with the final expression in Newtons;
 * for example, C1 is expressed in Newtons, C2 in Newtons per kilopascal,
 * and so forth.
 * 
 * Specific Impulse, measured in m/s (or, equivalently, Newton Seconds/kilogram)
 * is expressed using a similar equation:
 * 
   \f[
    I_{sp}(P,T) = \left\{ K_{1}+K_{2}P+K_{3}P^{2}+K_{4}P^{K_{5}}+
                    K_{6}P^{K_{7}}+K_{8}P^{K_{9}}+K_{10}K_{11}^{K_{12}P}\right\} 
         \left(\frac{T}{T_{ref}}\right)^{1+K_{13}+K_{14}P}\f]
 * 
 * @return true on successful evaluation.
 */
//---------------------------------------------------------------------------
bool Thruster::CalculateThrustAndIsp()
{
   if (!thrusterFiring) {
      thrust  = 0.0;
      impulse = 0.0;
   }
   else {
      if (tanks.empty())
         throw HardwareException("Thruster \"" + instanceName + 
                                    "\" does not have a fuel tank");
      
      // Require that the tanks all be at the same pressure and temperature
      Integer pressID = tanks[0]->GetParameterID("Pressure");
      Integer tempID = tanks[0]->GetParameterID("Temperature");
      Integer refTempID = tanks[0]->GetParameterID("RefTemperature");
      
      pressure = tanks[0]->GetRealParameter(pressID);
      temperatureRatio = tanks[0]->GetRealParameter(tempID) / 
                         tanks[0]->GetRealParameter(refTempID);
      
      thrust = cCoefficients[0];
      impulse = kCoefficients[0];
      
      if (!constantExpressions) {
   
         thrust  += pressure*(cCoefficients[1] + pressure*cCoefficients[2]);
         impulse += pressure*(kCoefficients[1] + pressure*kCoefficients[2]);
      
         // For efficiency, if thrust and Isp are simple, don't bother evaluating 
         // higher order terms
         if (!simpleExpressions) {
            thrust  += cCoefficients[3] * pow(pressure, cCoefficients[4]) +
                       cCoefficients[5] * pow(pressure, cCoefficients[6]) +
                       cCoefficients[7] * pow(pressure, cCoefficients[8]) +
                       cCoefficients[9] * pow(cCoefficients[10], 
                                              pressure * cCoefficients[11]);
                       
            impulse += kCoefficients[3] * pow(pressure, kCoefficients[4]) +
                       kCoefficients[5] * pow(pressure, kCoefficients[6]) +
                       kCoefficients[7] * pow(pressure, kCoefficients[8]) +
                       kCoefficients[9] * pow(kCoefficients[10], 
                                              pressure * kCoefficients[11]);
         }
      }
   
      thrust  *= pow(temperatureRatio, (1.0 + cCoefficients[12] + 
                     pressure*cCoefficients[13])) * thrustScaleFactor;
      impulse *= pow(temperatureRatio, (1.0 + kCoefficients[12] + 
                     pressure*kCoefficients[13]));
   }
   
   return true;
}


//---------------------------------------------------------------------------
//  Real CalculateMassFlow()
//---------------------------------------------------------------------------
/**
 * Evaluates the time rate of change of mass due to a thruster firing.
 * 
 *  \f[\frac{dm}{dt} = \frac{F_T}{I_{sp}} \f]
 * 
 * @return the mass flow rate from this thruster, used in integration.
 */
//---------------------------------------------------------------------------
Real Thruster::CalculateMassFlow()
{
   if (!thrusterFiring)
      return 0.0;
   else {     
      if (tanks.empty())
         throw HardwareException("Thruster \"" + instanceName + 
                                    "\" does not have a fuel tank");

      // For now, always calculate T and I_sp
      //if (!constantExpressions) {
         if (!CalculateThrustAndIsp())
            throw HardwareException("Thruster \"" + instanceName + 
                                    "\" could not calculate dm/dt");
         if (impulse == 0.0)
            throw HardwareException("Thruster \"" + instanceName + 
                                    "\" has specific impulse == 0.0");
         mDot = (thrust/thrustScaleFactor) / impulse;
      //}
   }

   #ifdef DEBUG_THRUSTER
      MessageInterface::ShowMessage(
            "   Thrust = %15lf, Isp = %15lf, MassFlow = %15lf T/Isp =  %lf\n", 
            thrust, impulse, mDot, thrust/impulse);
   #endif
   
   return mDot;
}
