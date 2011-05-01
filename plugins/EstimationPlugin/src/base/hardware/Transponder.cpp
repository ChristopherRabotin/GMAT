//$Id: Transponder.cpp 2010-04-02$
//------------------------------------------------------------------------------
//                             Transponder
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Tuan Dang Nguyen
// Created: 2010/04/02 by Tuan Dang Nguyen (GSFC-NASA)
//
/**
 * Implementation for the Transponder class
 */
//------------------------------------------------------------------------------

#include "Transponder.hpp"

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Text strings used to script Receiver properties
const std::string
Transponder::PARAMETER_TEXT[TransponderParamCount - RFHardwareParamCount] =
	{
		"InputFrequencyModel",
		"InputCenterFrequency",
		"InputBandwidth",
		"OutputFrequencyModel",
		"TurnAroundRatio",
	};

/// Integer IDs associated with the Receiver properties
const Gmat::ParameterType
Transponder::PARAMETER_TYPE[TransponderParamCount - RFHardwareParamCount] =
	{
		Gmat::STRING_TYPE,
      Gmat::REAL_TYPE,
      Gmat::REAL_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
	};

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Transponder(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name for the transponder
 */
//------------------------------------------------------------------------------
Transponder::Transponder(const std::string &name):
	RFHardware				("Transponder", name),
	inputFrequencyModel	("CenterAndBandwidth"),
	inputCenterFrequency	(0.0),
	inputBandwidth			(0.0),
	outputFrequencyModel	("TurnAroundRatio"),
	turnAroundRatio		("240/221")
{
   objectTypeNames.push_back("Transponder");
   parameterCount = TransponderParamCount;
	isTransmitted1 = false;
	isTransmitted2 = true;

	signal1 = new Signal();
	signal2 = new Signal();
}

//------------------------------------------------------------------------------
// ~Transponder()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Transponder::~Transponder()
{
	if (signal1 != NULL)
		delete signal1;
	if (signal2 != NULL)
		delete signal2;
}

//------------------------------------------------------------------------------
// Transponder(const Transponder & trans)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param trans The transponder that provides configuration data for this new
 *              one.
 */
//------------------------------------------------------------------------------
Transponder::Transponder(const Transponder& trans):
   RFHardware				(trans),
   inputFrequencyModel 	(trans.inputFrequencyModel),
   inputCenterFrequency (trans.inputCenterFrequency),
   inputBandwidth			(trans.inputBandwidth),
   outputFrequencyModel	(trans.outputFrequencyModel),
   turnAroundRatio		(trans.turnAroundRatio)
{
}

//------------------------------------------------------------------------------
// Transponder& operator =(const Transponder & trans)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param trans The Transponder that provides configuration data for this one.
 *
 * @return This Transponder, set to match trans.
 */
//------------------------------------------------------------------------------
Transponder& Transponder::operator=(const Transponder& trans)
{
   if (this != &trans)
   {
   	inputFrequencyModel = trans.inputFrequencyModel;
   	inputCenterFrequency = trans.inputCenterFrequency;
   	inputBandwidth = trans.inputBandwidth;
   	outputFrequencyModel = trans.outputFrequencyModel;
   	turnAroundRatio = trans.turnAroundRatio;

      RFHardware::operator=(trans);
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this transponder
 *
 * @return A new Transponder configured like this one.
 */
//------------------------------------------------------------------------------
GmatBase* Transponder::Clone() const
{
	return new Transponder(*this);
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
void Transponder::Copy(const GmatBase* orig)
{
   operator=(*((Transponder *)(orig)));
}



//-----------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//-----------------------------------------------------------------------------
/**
 * Method used to find the integer ID associated with the script string for an
 * Transponder object property.
 *
 * @param str The script string
 *
 * @return The integer ID associated with the string
 */
//-----------------------------------------------------------------------------
Integer Transponder::GetParameterID(const std::string & str) const
{
   for (Integer i = RFHardwareParamCount; i < TransponderParamCount; i++)
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
 * Finds the script string associated with a Transponder object's property,
 * given the integer ID for that property.
 *
 * @param id The property's ID.
 *
 * @return The string associated with the property.
 */
//------------------------------------------------------------------------------
std::string Transponder::GetParameterText(const Integer id) const
{
   if (id >= RFHardwareParamCount && id < TransponderParamCount)
      return PARAMETER_TEXT[id - RFHardwareParamCount];
   return RFHardware::GetParameterText(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the Trsnaponder property associated
 * with the input ID.
 *
 * @param id The property's ID
 *
 * @return A text description of the property's type.
 */
//------------------------------------------------------------------------------
std::string Transponder::GetParameterTypeString(const Integer id) const
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
Gmat::ParameterType Transponder::GetParameterType(const Integer id) const
{
   if (id >= RFHardwareParamCount && id < TransponderParamCount)
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
 * data for the Transponder properties will need to be specified.
 */
//------------------------------------------------------------------------------
std::string Transponder::GetParameterUnit(const Integer id) const
{
	switch(id)
	{
		case INPUT_FREQUENCY_MODEL:
		case TURN_AROUND_RATIO:
		case OUTPUT_FREQUENCY_MODEL:
			return "";						// They have no unit

		case INPUT_CENTER_FREQUENCY:
		case INPUT_BANDWIDTH:
			return "MHz";					// They have MHz unit

		default:
			break;
	}

   return RFHardware::GetParameterUnit(id);
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
Real Transponder::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case INPUT_CENTER_FREQUENCY:
         return inputCenterFrequency;
      case INPUT_BANDWIDTH:
      	return inputBandwidth;
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
Real Transponder::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
      case INPUT_CENTER_FREQUENCY:
         if (value >= 0.0)
            inputCenterFrequency = value;
         return inputCenterFrequency;
      case INPUT_BANDWIDTH:
         if (value >= 0.0)
            inputBandwidth = value;
         return inputBandwidth;
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
Real Transponder::GetRealParameter(const std::string & label) const
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
Real Transponder::SetRealParameter(const std::string & label, const Real value)
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
std::string Transponder::GetStringParameter(const Integer id) const
{
	switch (id)
	{
		case INPUT_FREQUENCY_MODEL:
			return inputFrequencyModel;
		case OUTPUT_FREQUENCY_MODEL:
			return outputFrequencyModel;
		case TURN_AROUND_RATIO:
			return turnAroundRatio;

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
bool Transponder::SetStringParameter(const Integer id,
							const std::string &value)
{
	switch (id)
	{
		case INPUT_FREQUENCY_MODEL:
			inputFrequencyModel = value;
			return true;
		case OUTPUT_FREQUENCY_MODEL:
			outputFrequencyModel = value;
			return true;
		case TURN_AROUND_RATIO:
			turnAroundRatio = value;
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
std::string Transponder::GetStringParameter(const std::string &label) const
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
bool Transponder::SetStringParameter(const std::string &label,
							const std::string &value)
{
	return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * This function is used to initialize its parameters or referenced objects.
 */
//------------------------------------------------------------------------------
bool Transponder::Initialize()
{
	return RFHardware::Initialize();
}


//------------------------------------------------------------------------------
// Real GetOutPutFrequency()
//------------------------------------------------------------------------------
/**
 * Get the output frequency.
 *
 * @return the output frequency.
 */
//------------------------------------------------------------------------------
/*
Real Transponder::GetOutPutFrequency()
{
	Real outputFreq = 0.0;
	// Code to get output frequency:

	return outputFreq;
}
*/


//------------------------------------------------------------------------------
// Real GetDelay(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Get the sensor delay for a given signal.
 *
 * @param whichOne 	The index specifying a given signal. 1 for input signal,
 * 						2 for output signal
 *
 * @return the input or output frequency.
 */
//------------------------------------------------------------------------------
Real Transponder::GetDelay(Integer whichOne)
{
	return RFHardware::GetDelay(whichOne);
}


//------------------------------------------------------------------------------
// bool SetDelay(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Set the sensor delay for a given signal.
 *
 * @param delay	   Hardware delay
 * @param whichOne 	The index specifying a given signal. 1 for input signal,
 * 						2 for output signal
 *
 * @return true if it is successfully set, false otherwise .
 */
//------------------------------------------------------------------------------
bool Transponder::SetDelay(Real delay, Integer whichOne)
{
	return RFHardware::SetDelay(delay, whichOne);
}


//------------------------------------------------------------------------------
// bool IsFeasible(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Verify a given signal is feasible or not.
 *
 * @param whichOne 	The index specifying a given signal. 1 for input signal,
 * 						2 for output signal
 *
 * @return 	true if it is feasible, false otherwise.
 */
//------------------------------------------------------------------------------
bool Transponder::IsFeasible(Integer whichOne)
{
	// It need to check for frequencyModel:

	Real high_freq = inputCenterFrequency + inputBandwidth/2;
	Real low_freq = inputCenterFrequency - inputBandwidth/2;

	// for Transponder, signal1 is received and signal2 is transmitted
	Signal* s = GetSignal(0);			// Get signal1
	if ((low_freq <= s->GetValue())&&(s->GetValue() <= high_freq))
		return true;
	else
		return false;
}


//------------------------------------------------------------------------------
// Integer GetSignalCount()
//------------------------------------------------------------------------------
/**
 * Get the number of signals handle by Transponder.
 *
 * @return the number of signals.
 */
//------------------------------------------------------------------------------
Integer Transponder::GetSignalCount()
{
	return 2;
}


//------------------------------------------------------------------------------
// bool IsTransmitted(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Verify a given signal having ability to transmit or not.
 *
 * @param whichOne 	The index specifying a given signal. 1 for input signal,
 * 						2 for output signal
 *
 * @return true for ability to transmit, false otherwise.
 */
//------------------------------------------------------------------------------
bool Transponder::IsTransmitted(Integer whichOne)
{
	switch(whichOne)
	{
	case 1:
		return isTransmitted1;
	case 2:
		return isTransmitted2;
	default:
		throw new GmatBaseException("Index is out of bound\n");
	}
}


//------------------------------------------------------------------------------
// Signal* GetSignal(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Get a specified signal.
 *
 * @param whichOne 	The index specifying a given signal. 1 for input signal,
 * 						2 for output signal
 *
 * @return 	a signal for a given index.
 */
//------------------------------------------------------------------------------
Signal* Transponder::GetSignal(Integer whichOne)
{
	return RFHardware::GetSignal(whichOne);
}


//------------------------------------------------------------------------------
// bool SetSignal(Signal* s, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Set a signal for a given index.
 *
 * @param s 			The signal needed to set to
 * @param whichOne 	The index specifying a given signal. 1 for input signal,
 * 						2 for output signal
 *
 * @return 	true if signal is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Transponder::SetSignal(Signal* s,Integer whichOne)
{
	bool retval = false;
	if (RFHardware::SetSignal(s, whichOne))
	{
		if (whichOne == 0)
		{
			Real output_freq = GetTurnAroundRatio()*s->GetValue();
			RFHardware::GetSignal(1)->SetValue(output_freq);
		}
		retval = true;
	}

	return retval;
}


//-------------------------------------------------------------------------
// Real GetTurnAroundRatio()
//-------------------------------------------------------------------------
/**
 * This function converts turnAroundRatio string to a real number and
 * return it result.
 */
//-------------------------------------------------------------------------
Real Transponder::GetTurnAroundRatio()
{
	Real ratio;
	Integer loc = turnAroundRatio.find('/');
	if (loc >= 0)
	{
		Integer len = turnAroundRatio.length();
		std::string num_s = turnAroundRatio.substr(0, loc);
		std::string denom_s = turnAroundRatio.substr(loc+1, len-loc-1);

		ratio = atof(num_s.c_str())/atof(denom_s.c_str());
	}
	else
		ratio = atof(turnAroundRatio.c_str());

	return ratio;
}
