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
   "CoordinateSystem"
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
   Gmat::STRING_TYPE,
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
   Hardware          (Gmat::HARDWARE, "Thruster", nomme),
   coordinateName    ("MJ2000EarthEquator")
   // theCoordinates    (NULL)
{
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
   Hardware          (th)
{
   parameterCount = th.parameterCount;
   
   memcpy(cCoefficients, th.cCoefficients, 14 * sizeof(Real));
   memcpy(kCoefficients, th.kCoefficients, 14 * sizeof(Real));
   
   thrustDirection[0] = th.thrustDirection[0];
   thrustDirection[1] = th.thrustDirection[1];
   thrustDirection[2] = th.thrustDirection[2];
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
      
   memcpy(cCoefficients, th.cCoefficients, 14 * sizeof(Real));
   memcpy(kCoefficients, th.kCoefficients, 14 * sizeof(Real));
   
   thrustDirection[0] = th.thrustDirection[0];
   thrustDirection[1] = th.thrustDirection[1];
   thrustDirection[2] = th.thrustDirection[2];

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
 *
 */
//------------------------------------------------------------------------------
std::string Thruster::GetParameterText(const Integer id) const
{
   if (id >= HardwareParamCount && id < ThrusterParamCount)
      return PARAMETER_TEXT[id - HardwareParamCount];
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
         return cCoefficients[1] = value;
      case C3:
         return cCoefficients[2] = value;
      case C4:
         return cCoefficients[3] = value;
      case C5:
         return cCoefficients[4] = value;
      case C6:
         return cCoefficients[5] = value;
      case C7:
         return cCoefficients[6] = value;
      case C8:
         return cCoefficients[7] = value;
      case C9:
         return cCoefficients[8] = value;
      case C10:
         return cCoefficients[9] = value;
      case C11:
         return cCoefficients[10] = value;
      case C12:
         return cCoefficients[11] = value;
      case C13:
         return cCoefficients[12] = value;
      case C14:
         return cCoefficients[13] = value;
      case K1:
         return kCoefficients[0] = value;
      case K2:
         return kCoefficients[1] = value;
      case K3:
         return kCoefficients[2] = value;
      case K4:
         return kCoefficients[3] = value;
      case K5:
         return kCoefficients[4] = value;
      case K6:
         return kCoefficients[5] = value;
      case K7:
         return kCoefficients[6] = value;
      case K8:
         return kCoefficients[7] = value;
      case K9:
         return kCoefficients[8] = value;
      case K10:
         return kCoefficients[9] = value;
      case K11:
         return kCoefficients[10] = value;
      case K12:
         return kCoefficients[11] = value;
      case K13:
         return kCoefficients[12] = value;
      case K14:
         return kCoefficients[13] = value;
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
const StringArray& Thruster::GetStringArrayParameter(const Integer id) const
{
    if (id == TANK)
       return tankNames;
    
    return Hardware::GetStringArrayParameter(id);
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
