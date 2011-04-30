//$Id$
//------------------------------------------------------------------------------
//                               Hardware
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2004/11/08
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class implementation for the Hardware base class.
 */
//------------------------------------------------------------------------------
#include "MessageInterface.hpp"

#include "Hardware.hpp"
#include "HardwareException.hpp"
#include <string.h>


//---------------------------------
// static data
//---------------------------------

/// Labels used for the hardware element parameters.
const std::string
Hardware::PARAMETER_TEXT[HardwareParamCount - GmatBaseParamCount] =
{
   "X_Direction",
   "Y_Direction", 
   "Z_Direction"
};


/// Types of the parameters used by all hardware elements.
const Gmat::ParameterType
Hardware::PARAMETER_TYPE[HardwareParamCount - GmatBaseParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE
};



//------------------------------------------------------------------------------
//  Hardware(Gmat::ObjectType typeId, const std::string &typeStr, 
//           const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Hardware base class constructor.
 *
 * @param typeId Core object type for the component.
 * @param typeStr String label for the actual object type for the component.
 * @param nomme Name of the component.
 */
//------------------------------------------------------------------------------
Hardware::Hardware(Gmat::ObjectType typeId, const std::string &typeStr, 
                   const std::string &nomme) :
   GmatBase(typeId, typeStr, nomme)
{
   objectTypes.push_back(Gmat::HARDWARE);
   objectTypeNames.push_back("Hardware");

   direction[0] = 1.0;
   direction[1] = 0.0;
   direction[2] = 0.0;
   
   secondDirection[0] = 0.0;
   secondDirection[1] = 1.0;
   secondDirection[2] = 0.0;

   location[0] = 0.0;
   location[1] = 0.0;
   location[2] = 0.0;
}


//------------------------------------------------------------------------------
//  ~Hardware()
//------------------------------------------------------------------------------
/**
 * Hardware base class destructor.
 */
//------------------------------------------------------------------------------
Hardware::~Hardware()
{}


//------------------------------------------------------------------------------
//  Hardware(const Hardware& hw)
//------------------------------------------------------------------------------
/**
 * Hardware copy constructor.
 * 
 * @param hw The object being copied.
 */
//------------------------------------------------------------------------------
Hardware::Hardware(const Hardware& hw) :
   GmatBase(hw)
{
   direction[0] = hw.direction[0];
   direction[1] = hw.direction[1];
   direction[2] = hw.direction[2];
   
   secondDirection[0] = hw.secondDirection[0];
   secondDirection[1] = hw.secondDirection[1];
   secondDirection[2] = hw.secondDirection[2];

   location[0] = hw.location[0];
   location[1] = hw.location[1];
   location[2] = hw.location[2];
}


//------------------------------------------------------------------------------
//  Hardware& operator=(const Hardware& hw)
//------------------------------------------------------------------------------
/**
 * Hardware assignment operator.
 * 
 * @param hw The object being copied.
 * 
 * @return this object, with parameters set to the input object's parameters.
 */
//------------------------------------------------------------------------------
Hardware& Hardware::operator=(const Hardware& hw)
{
   if (&hw == this)
      return *this;
   
   GmatBase::operator=(hw);
   
   direction[0] = hw.direction[0];
   direction[1] = hw.direction[1];
   direction[2] = hw.direction[2];
   
   secondDirection[0] = hw.secondDirection[0];
   secondDirection[1] = hw.secondDirection[1];
   secondDirection[2] = hw.secondDirection[2];
   
   location[0] = hw.location[0];
   location[1] = hw.location[1];
   location[2] = hw.location[2];
   
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
std::string Hardware::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < HardwareParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
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
Integer Hardware::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < HardwareParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
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
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Hardware::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < HardwareParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
      
   return GmatBase::GetParameterType(id);
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
 *
 */
//------------------------------------------------------------------------------
std::string Hardware::GetParameterTypeString(const Integer id) const
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
Real Hardware::GetRealParameter(const Integer id) const
{
   switch (id) {
      case DIRECTION_X:
         return direction[0];
         
      case DIRECTION_Y:
         return direction[1];
         
      case DIRECTION_Z:
         return direction[2];
         
      default:
         break;   // Default just drops through
   }
   return GmatBase::GetRealParameter(id);
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
Real Hardware::SetRealParameter(const Integer id, const Real value)
{
   switch (id) {
      case DIRECTION_X:
         return direction[0] = value;
         
      case DIRECTION_Y:
         return direction[1] = value;
         
      case DIRECTION_Z:
//         if (value < 0) {
//            throw HardwareException("Z_Direction must be >= 0"); 
//         }
         return direction[2] = value;
      default:
         break;   // Default just drops through
   }

   return GmatBase::SetRealParameter(id, value);
}


// Required so derived classes work correctly:

//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieve the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 *
 * @return The parameter's value.
 */
//------------------------------------------------------------------------------
Real Hardware::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Set the value for a Real parameter.
 *
 * @param label The script label for the parameter.
 * @param value The new parameter value.
 *
 * @return the parameter value at the end of this call, or throw an exception
 *         if the parameter id is invalid or the parameter type is not Real.
 */
//------------------------------------------------------------------------------
Real Hardware::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}



//------------------------------------------------------------------------
// This function is used to verify a referenced object is properly set up
// or not.
//------------------------------------------------------------------------
/*
bool Hardware::VerifyRefObject(std::string subTypeName, GmatBase* obj)
{
	MessageInterface::ShowMessage("Hardware::VerifyRefObject \n");
	return true;
}
*/
