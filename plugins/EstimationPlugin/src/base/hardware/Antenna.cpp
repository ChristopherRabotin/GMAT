//$Id: Antenna.cpp 2010-03-31 $
//------------------------------------------------------------------------------
//                             Antenna
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Tuan Dang Nguyen
// Created: 2010/03/31 by Tuan Dang Nguyen
//
/**
 * Implementation for the Antenna class
 */
//------------------------------------------------------------------------------

#include "Antenna.hpp"

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Text strings used to script RFHardware properties
const std::string
Antenna::PARAMETER_TEXT[AntennaParamCount - HardwareParamCount] =
   {
      "Delay",
      "PhaseCenterLocation1",
      "PhaseCenterLocation2",
      "PhaseCenterLocation3",
   };

/// Integer IDs associated with the RFHardware properties
const Gmat::ParameterType
Antenna::PARAMETER_TYPE[AntennaParamCount - HardwareParamCount] =
   {
      Gmat::REAL_TYPE,
      Gmat::INTEGER_TYPE,
      Gmat::INTEGER_TYPE,
      Gmat::INTEGER_TYPE,
   };

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Antenna(const std::string &type, const std::string &name)
//-----------------------------------------------------------------------------
/**
 * Default constructor.
 *
 * @param type The type of Antenna subclass that is constructed.
 * @param name The name of the Antenna object.
 */
//-----------------------------------------------------------------------------
Antenna::Antenna(const std::string &type, const std::string &name) :
   Hardware             (Gmat::HARDWARE, type, name),
   antennaDelay         (1e-6),
   phaseCenterLocation1 (0),
   phaseCenterLocation2 (0),
   phaseCenterLocation3 (0)
{
   // TODO Auto-generated constructor stub
   objectTypes.push_back(Gmat::ANTENNA);
   objectTypeNames.push_back("Antenna");
   parameterCount = AntennaParamCount;
}

//-----------------------------------------------------------------------------
// ~Antenna()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
Antenna::~Antenna()
{
        // TODO Auto-generated destructor stub
}

//-----------------------------------------------------------------------------
// Antenna(const Antenna & ant) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ant The Antenna element that is copied into this new instance.
 */
//-----------------------------------------------------------------------------
Antenna::Antenna(const Antenna & ant) :
   Hardware                (ant),
   antennaDelay            (ant.antennaDelay),
   phaseCenterLocation1    (ant.phaseCenterLocation1),
   phaseCenterLocation2    (ant.phaseCenterLocation2),
   phaseCenterLocation3    (ant.phaseCenterLocation3)
{
}


//-----------------------------------------------------------------------------
// Antenna& operator=(const Antenna& ant)
//-----------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param ant The Antenna element that provides the configuration for this
 * instance.
 *
 * @return This object, configured to look like ant.
 */
//-----------------------------------------------------------------------------
Antenna & Antenna::operator=(const Antenna & ant)
{
   if (this != &ant)
   {
      Hardware::operator=(ant);

      antennaDelay 			= ant.antennaDelay;
      phaseCenterLocation1 = ant.phaseCenterLocation1;
      phaseCenterLocation2 = ant.phaseCenterLocation2;
      phaseCenterLocation3 = ant.phaseCenterLocation3;
   }

   return *this;
}

GmatBase* Antenna::Clone() const
{
   return new Antenna(*this);
}

//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void Antenna::Copy(const GmatBase* orig)
{
   operator=(*((Antenna *)(orig)));
}


//-----------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//-----------------------------------------------------------------------------
/**
 * Method used to find the integer ID associated with the script string for an
 * Antenna object property.
 *
 * @param str The script string
 *
 * @return The integer ID associated with the string
 */
//-----------------------------------------------------------------------------
Integer Antenna::GetParameterID(const std::string & str) const
{
   for (Integer i = HardwareParamCount; i < AntennaParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - HardwareParamCount])
         return i;
   }

   return Hardware::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Finds the script string associated with an Antenna object's property,
 * given the integer ID for that property.
 *
 * @param id The property's ID.
 *
 * @return The string associated with the property.
 */
//------------------------------------------------------------------------------
std::string Antenna::GetParameterText(const Integer id) const
{
   if (id >= HardwareParamCount && id < AntennaParamCount)
      return PARAMETER_TEXT[id - HardwareParamCount];
   return Hardware::GetParameterText(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the Antenna property associated
 * with the input ID.
 *
 * @param id The property's ID
 *
 * @return A text description of the property's type.
 */
//------------------------------------------------------------------------------
std::string Antenna::GetParameterTypeString(const Integer id) const
{
   return Hardware::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the enumerated parameter type for a specified property
 *
 * @param id The property's ID
 *
 * @return The ParameterType of the object.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Antenna::GetParameterType(const Integer id) const
{
   if (id >= HardwareParamCount && id < AntennaParamCount)
      return PARAMETER_TYPE[id - HardwareParamCount];

   return Hardware::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units for a property
 *
 * @param id The property's ID
 *
 * @return The units for the property
 *
 * @note: This method is not yet used in GMAT.  Once the code uses it, the unit
 * data for the Antenna properties will need to be specified.
 */
//------------------------------------------------------------------------------
std::string Antenna::GetParameterUnit(const Integer id) const
{
	switch (id)
	{
		case ANTENNA_DELAY:
			return "s"; 					// Unit of antenna delay is (s) second
		case PHASE_CENTER_LOCATION1:
		case PHASE_CENTER_LOCATION2:
		case PHASE_CENTER_LOCATION3:
			return "";						// It has no unit
		default:
			break;
	}

   return Hardware::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Specifies whether a property is an expected user scripted parameter.
 *
 * This method is used when an object is serialized to determine if the
 * corresponding property should be included in the serialization.  Typical use
 * is when an object is written to the screen or to a file; objects identified
 * as "read only" by a return value of true from this method are but places in
 * the text shown on the screen or in the file.
 *
 * Users can script the "read only" parameters, but do so at their own risk.
 *
 * @param label The scriptable label of the property
 *
 * @return true if the property is read only, false if not.
 */
//------------------------------------------------------------------------------
bool Antenna::IsParameterReadOnly(const std::string& label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Specifies whether a property is an expected user scripted parameter.
 *
 * This method is used when an object is serialized to determine if the
 * corresponding property should be included in the serialization.  Typical use
 * is when an object is written to the screen or to a file; objects identified
 * as "read only" by a return value of true from this method are but places in
 * the text shown on the screen or in the file.
 *
 * Users can script the "read only" parameters, but do so at their own risk.
 *
 * @param id The integer ID of the property
 *
 * @return true if the property is read only, false if not.
 */
//------------------------------------------------------------------------------
bool Antenna::IsParameterReadOnly(const Integer id) const
{
   if ((id == DIRECTION_X) || (id == DIRECTION_Y) || (id == DIRECTION_Z))
      return true;
   return Hardware::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an integer property.
 *
 * @param id The integer ID of the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Integer Antenna::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
      case PHASE_CENTER_LOCATION1:
         return phaseCenterLocation1;

      case PHASE_CENTER_LOCATION2:
         return phaseCenterLocation2;

      case PHASE_CENTER_LOCATION3:
         return phaseCenterLocation3;

      default:
         break;
   }

   return Hardware::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetRealParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value of an integer property.
 *
 * @param id The integer ID of the property
 * @param value The new value for the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Integer Antenna::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
      case PHASE_CENTER_LOCATION1:
            phaseCenterLocation1 = value;
         return phaseCenterLocation1;

      case PHASE_CENTER_LOCATION2:
            phaseCenterLocation2 = value;
         return phaseCenterLocation2;

      case PHASE_CENTER_LOCATION3:
            phaseCenterLocation3 = value;
         return phaseCenterLocation3;

      default:
         break;
   }

   return Hardware::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an integer property.
 *
 * @param label The scriptable label for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Integer Antenna::GetIntegerParameter(const std::string & label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string & label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value of an integer property.
 *
 * @param label The scriptable label for the property
 * @param value The new value for the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Integer Antenna::SetIntegerParameter(const std::string & label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param id The integer ID of the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Real Antenna::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case ANTENNA_DELAY:
         return antennaDelay;

      default:
         break;
   }

   return Hardware::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param id The integer ID of the property
 * @param value The new value for the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Real Antenna::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
      case ANTENNA_DELAY:
         if (value >= 0.0)
            antennaDelay = value;
         return antennaDelay;

      default:
         break;
   }

   return Hardware::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real property.
 *
 * @param label The scriptable label for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
Real Antenna::GetRealParameter(const std::string & label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a real property.
 *
 * @param label The scriptable label for the property
 * @param value The new value for the property
 *
 * @return The property's value at the end of the call
 */
//------------------------------------------------------------------------------
Real Antenna::SetRealParameter(const std::string & label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

