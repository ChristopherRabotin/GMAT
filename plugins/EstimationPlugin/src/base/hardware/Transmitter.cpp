//$Id: Transmitter.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             Transmitter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/02/22 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the Transmitter class
 */
//------------------------------------------------------------------------------

#include "Transmitter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_SET_REAL_PARA

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Text strings used to script Transmitter properties
const std::string
Transmitter::PARAMETER_TEXT[TransmitterParamCount - RFHardwareParamCount] =
	{
		"FrequencyModel",
		"Frequency",
	};

/// Integer IDs associated with the Transmitter properties
const Gmat::ParameterType
Transmitter::PARAMETER_TYPE[TransmitterParamCount - RFHardwareParamCount] =
	{
		Gmat::STRING_TYPE,
      Gmat::REAL_TYPE,
	};

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Transmitter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name for the transmitter
 */
//------------------------------------------------------------------------------
Transmitter::Transmitter(const std::string &name) :
   RFHardware     ("Transmitter", name),
   frequencyModel ("constant"),
   frequency		(0.0)
{
   objectTypeNames.push_back("Transmitter");
   parameterCount = TransmitterParamCount;
	isTransmitted1 = true;
	signal1 = new Signal();
}

//------------------------------------------------------------------------------
// ~Transmitter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Transmitter::~Transmitter()
{
	if (signal1 != NULL)
		delete signal1;
}


//------------------------------------------------------------------------------
// Transmitter(const Transmitter & trans)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param trans The Transmitter that provides configuration data for this new
 *              one.
 */
//------------------------------------------------------------------------------
Transmitter::Transmitter(const Transmitter & trans) :
	RFHardware        (trans),
   frequencyModel 	(trans.frequencyModel),
   frequency			(trans.frequency)
{
	signal1->SetValue(frequency);
}


//------------------------------------------------------------------------------
// Transmitter& operator =(const Transmitter & trans)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param trans The Transmitter that provides configuration data for this one.
 *
 * @return This Transmitter, set to match trans.
 */
//------------------------------------------------------------------------------
Transmitter& Transmitter::operator =(const Transmitter & trans)
{
   if (this != &trans)
   {
   	frequencyModel = trans.frequencyModel;
   	frequency = trans.frequency;

      RFHardware::operator=(trans);
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this transmitter
 *
 * @return A new Transmitter configured like this one.
 */
//------------------------------------------------------------------------------
GmatBase *Transmitter::Clone() const
{
   return new Transmitter(*this);
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
void Transmitter::Copy(const GmatBase* orig)
{
   operator=(*((Transmitter *)(orig)));
}



//-----------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//-----------------------------------------------------------------------------
/**
 * Method used to find the integer ID associated with the script string for an
 * Transmitter object property.
 *
 * @param str The script string
 *
 * @return The integer ID associated with the string
 */
//-----------------------------------------------------------------------------
Integer Transmitter::GetParameterID(const std::string & str) const
{
   for (Integer i = RFHardwareParamCount; i < TransmitterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - RFHardwareParamCount])
         return i;
   }

   return RFHardware::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Finds the script string associated with a Transmitter object's property,
 * given the integer ID for that property.
 *
 * @param id The property's ID.
 *
 * @return The string associated with the property.
 */
//------------------------------------------------------------------------------
std::string Transmitter::GetParameterText(const Integer id) const
{
   if (id >= RFHardwareParamCount && id < TransmitterParamCount)
      return PARAMETER_TEXT[id - RFHardwareParamCount];
   return RFHardware::GetParameterText(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the Transmitter property associated
 * with the input ID.
 *
 * @param id The property's ID
 *
 * @return A text description of the property's type.
 */
//------------------------------------------------------------------------------
std::string Transmitter::GetParameterTypeString(const Integer id) const
{
   return RFHardware::PARAM_TYPE_STRING[GetParameterType(id)];
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
Gmat::ParameterType Transmitter::GetParameterType(const Integer id) const
{
   if (id >= RFHardwareParamCount && id < TransmitterParamCount)
      return PARAMETER_TYPE[id - RFHardwareParamCount];

   return RFHardware::GetParameterType(id);
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
 * data for the Transmitter properties will need to be specified.
 */
//------------------------------------------------------------------------------
std::string Transmitter::GetParameterUnit(const Integer id) const
{
	switch (id)
	{
		case FREQUENCY_MODEL:
			return "";						// It has no unit
		case FREQUENCY:
			return "MHz";					// Unit of frequency is MHz
		default:
			break;
	}

   return RFHardware::GetParameterUnit(id);
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
bool Transmitter::IsParameterReadOnly(const std::string& label) const
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
bool Transmitter::IsParameterReadOnly(const Integer id) const
{
   return RFHardware::IsParameterReadOnly(id);
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
Real Transmitter::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case FREQUENCY:
         return frequency;
      default:
         break;
   }

   return RFHardware::GetRealParameter(id);
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
Real Transmitter::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_SET_REAL_PARA
	MessageInterface::ShowMessage("Transmitter::SetRealParameter(id = %d, value = %le)  name of transmiiter = '%s'\n",id, value, GetName().c_str());
   #endif

   switch (id)
   {
      case FREQUENCY:
         if (value >= 0.0)
            frequency = value;
         return frequency;

      default:
         break;
   }

   return RFHardware::SetRealParameter(id, value);
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
Real Transmitter::GetRealParameter(const std::string & label) const
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
Real Transmitter::SetRealParameter(const std::string & label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Gets the value of a string property.
 *
 * @param id The id for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
std::string Transmitter::GetStringParameter(const Integer id) const
{
	switch (id)
	{
		case FREQUENCY_MODEL:
			return frequencyModel;
		default:
			break;
	}

	return RFHardware::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string property.
 *
 * @param id The id for the property
 * @param value The new value for the property
 *
 * @return true if the setting success
 */
//------------------------------------------------------------------------------
bool Transmitter::SetStringParameter(const Integer id,
							const std::string &value)
{
	switch (id)
	{
		case FREQUENCY_MODEL:
			frequencyModel = value;
			return true;
		default:
			break;
	}

	return RFHardware::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string property.
 *
 * @param label The scriptable label for the property
 *
 * @return The property's value
 */
//------------------------------------------------------------------------------
std::string Transmitter::GetStringParameter(const std::string &label) const
{
	return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a string property.
 *
 * @param label The scriptable label for the property
 * @param value The new value for the property
 *
 * @return true if the setting success.
 */
//------------------------------------------------------------------------------
bool Transmitter::SetStringParameter(const std::string &label,
							const std::string &value)
{
	return SetStringParameter(GetParameterID(label), value);
}


bool Transmitter::Initialize()
{
	bool reval = false;
	if (RFHardware::Initialize())
	{
		signal1->SetValue(frequency);
		reval = true;
	}

	return reval;
}


//------------------------------------------------------------------------------
// Real GetOutPutFrequency()
//------------------------------------------------------------------------------
/**
 * Get the output frequency of Transmitter.
 *
 * @return the output frequency.
 */
//------------------------------------------------------------------------------
Real Transmitter::GetOutPutFrequency()
{
	// Write code to calculate output frequency here:
	Real outputFreq = frequency; 		// for frequencyModel = "constant"

	return outputFreq;
}


//------------------------------------------------------------------------------
// Real GetDelay(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This function is used to get hardware delay.
 *
 * @return hardware delay
 */
//------------------------------------------------------------------------------
Real Transmitter::GetDelay(Integer whichOne)
{
	if (whichOne == 0)
		return RFHardware::GetDelay();
	else
		throw new GmatBaseException("Delay index is out of bound\n");
}


//------------------------------------------------------------------------------
// bool SetDelay(Real delay, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This function is used to set hardware delay.
 *
 * @return true if it is successfully set, otherwise it throws an exception.
 */
//------------------------------------------------------------------------------
bool Transmitter::SetDelay(Real delay, Integer whichOne)
{
	switch(whichOne)
	{
	   case 0:
	   	hardwareDelay1 = delay;
	   	return true;
	   default:
	   	throw new GmatBaseException("Delay index is out of bound\n");
	}
}



//------------------------------------------------------------------------------
// Integer GetSignalCount()
//------------------------------------------------------------------------------
/**
 * Get the number of signals.
 *
 * @return the number of signals. For a transmitter, number of signal is 1
 */
//------------------------------------------------------------------------------
Integer Transmitter::GetSignalCount()
{
	return 1;
}


//------------------------------------------------------------------------------
// bool IsTransmitted(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Verify a given signal having ability to transmit or not.
 *
 * @param whichOne 	The index specifying a given signal.
 *
 * @return true for ability to transmit, false otherwise.
 */
//------------------------------------------------------------------------------
bool Transmitter::IsTransmitted(Integer whichOne)
{
	return this->isTransmitted1;
}


//------------------------------------------------------------------------------
// Signal* GetSignal(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Get a specified signal.
 *
 * @param whichOne 	The index specifying a given signal.
 *
 * @return 	a signal for a given index.
 */
//------------------------------------------------------------------------------
Signal* Transmitter::GetSignal(Integer whichOne)
{
	return RFHardware::GetSignal();
}


//------------------------------------------------------------------------------
// bool SetSignal(Signal* s, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Set a signal for a given index.
 *
 * @param s 			The signal needed to set to
 * @param whichOne 	The index specifying a given signal.
 *
 * @return 	true if signal is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Transmitter::SetSignal(Signal* s,Integer whichOne)
{
	return RFHardware::SetSignal(s);
}

