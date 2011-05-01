//$Id: Receiver.cpp 47 2010-03-31$
//------------------------------------------------------------------------------
//                             Receiver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Tuan Dang Nguyen
// Created: 2010/03/31 by Tuan Dang Nguyen (GSFC-NASA)
//
/**
 * Implementation for the Receiver class
 */
//------------------------------------------------------------------------------

#include "Receiver.hpp"
#include "MessageInterface.hpp"

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Text strings used to script Receiver properties
const std::string
Receiver::PARAMETER_TEXT[ReceiverParamCount - RFHardwareParamCount] =
   {
      "FrequencyModel",
      "CenterFrequency",
      "Bandwidth",
   };

/// Integer IDs associated with the Receiver properties
const Gmat::ParameterType
Receiver::PARAMETER_TYPE[ReceiverParamCount - RFHardwareParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::REAL_TYPE,
      Gmat::REAL_TYPE,
   };

//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Receiver(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name for the receiver
 */
//------------------------------------------------------------------------------

Receiver::Receiver(const std::string &name):
   RFHardware      ("Receiver", name),
   frequencyModel  ("constant"),
   centerFrequency (0.0),
   bandwidth       (0.0)
{
   //MessageInterface::ShowMessage("Receiver::Receiver()\n");
   objectTypeNames.push_back("Receiver");
   parameterCount = ReceiverParamCount;
   isTransmitted1 = false;
   signal1 = new Signal();
}

//------------------------------------------------------------------------------
// ~Receiver()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Receiver::~Receiver()
{
   if (signal1 != NULL)
      delete signal1;
}

//------------------------------------------------------------------------------
// Receiver(const Receiver & recei)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param recei The receiver that provides configuration data for this new
 *              one.
 */
//------------------------------------------------------------------------------
Receiver::Receiver(const Receiver& recei):
   RFHardware      (recei),
   frequencyModel  (recei.frequencyModel),
   centerFrequency (recei.centerFrequency),
   bandwidth       (recei.bandwidth)
{
}

//------------------------------------------------------------------------------
// Receiver& operator =(const Receiver & recei)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param recei The Receiver that provides configuration data for this one.
 *
 * @return This Receiver, set to match recei.
 */
//------------------------------------------------------------------------------
Receiver& Receiver::operator=(const Receiver& recei)
{
   if (this != &recei)
   {
      frequencyModel = recei.frequencyModel;
      centerFrequency = recei.centerFrequency;
      bandwidth = recei.bandwidth;
      
      RFHardware::operator=(recei);
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this receiver
 *
 * @return A new Receiver configured like this one.
 */
//------------------------------------------------------------------------------
GmatBase* Receiver::Clone() const
{
   return new Receiver(*this);
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
void Receiver::Copy(const GmatBase* orig)
{
   operator=(*((Receiver *)(orig)));
}


//-----------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//-----------------------------------------------------------------------------
/**
 * Method used to find the integer ID associated with the script string for an
 * Receiver object property.
 *
 * @param str The script string
 *
 * @return The integer ID associated with the string
 */
//-----------------------------------------------------------------------------
Integer Receiver::GetParameterID(const std::string & str) const
{
   for (Integer i = RFHardwareParamCount; i < ReceiverParamCount; i++)
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
 * Finds the script string associated with a Receiver object's property,
 * given the integer ID for that property.
 *
 * @param id The property's ID.
 *
 * @return The string associated with the property.
 */
//------------------------------------------------------------------------------
std::string Receiver::GetParameterText(const Integer id) const
{
   if (id >= RFHardwareParamCount && id < ReceiverParamCount)
      return PARAMETER_TEXT[id - RFHardwareParamCount];
   return RFHardware::GetParameterText(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the Receiver property associated
 * with the input ID.
 *
 * @param id The property's ID
 *
 * @return A text description of the property's type.
 */
//------------------------------------------------------------------------------
std::string Receiver::GetParameterTypeString(const Integer id) const
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
Gmat::ParameterType Receiver::GetParameterType(const Integer id) const
{
   if (id >= RFHardwareParamCount && id < ReceiverParamCount)
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
 * data for the Receiver properties will need to be specified.
 */
//------------------------------------------------------------------------------
std::string Receiver::GetParameterUnit(const Integer id) const
{
   switch(id)
   {
   case FREQUENCY_MODEL:
      return "";                              // It has no unit
   case CENTER_FREQUENCY:
   case BANDWIDTH:
      return "MHz";                           // The units of center frequency and bandwidth are MHz
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
bool Receiver::IsParameterReadOnly(const std::string& label) const
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
 * as "read only" by a return value of true from this method are not placed in
 * the text shown on the screen or in the file.
 *
 * Users can script the "read only" parameters, but do so at their own risk.
 *
 * @param id The integer ID of the property
 *
 * @return true if the property is read only, false if not.
 */
//------------------------------------------------------------------------------
bool Receiver::IsParameterReadOnly(const Integer id) const
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
Real Receiver::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case CENTER_FREQUENCY:
         return centerFrequency;
      case BANDWIDTH:
        return bandwidth;
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
Real Receiver::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
      case CENTER_FREQUENCY:
         if (value >= 0.0)
            centerFrequency = value;
         return centerFrequency;
      case BANDWIDTH:
         if (value >= 0.0)
            bandwidth = value;
         return bandwidth;
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
Real Receiver::GetRealParameter(const std::string & label) const
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
Real Receiver::SetRealParameter(const std::string & label, const Real value)
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
std::string Receiver::GetStringParameter(const Integer id) const
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
bool Receiver::SetStringParameter(const Integer id,
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
std::string Receiver::GetStringParameter(const std::string &label) const
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
bool Receiver::SetStringParameter(const std::string &label,
                                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


bool Receiver::Initialize()
{
   return RFHardware::Initialize();
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
Real Receiver::GetDelay(Integer whichOne)
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
bool Receiver::SetDelay(Real delay, Integer whichOne)
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
// bool IsFeasible(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Verify a given signal is feasible or not.
 *
 * @param whichOne      The index specifying a given signal.
 *                                              The signal is used by receiver is signal1
 *
 * @return      true if it is feasible, false otherwise.
 */
//------------------------------------------------------------------------------
bool Receiver::IsFeasible(Integer whichOne)
{
   // It needs to check for frequencyModel:
   
   Real high_freq = centerFrequency + bandwidth/2;
   Real low_freq = centerFrequency - bandwidth/2;
   Signal* s = RFHardware::GetSignal();
   
   if ((low_freq <= s->GetValue())&&(s->GetValue() <= high_freq))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// Integer GetSignalCount()
//------------------------------------------------------------------------------
/**
 * Get the number of signals.
 *
 * @return the number of signals. For a receiver, number of signal is 1
 */
//------------------------------------------------------------------------------
Integer Receiver::GetSignalCount()
{
   return 1;
}


//------------------------------------------------------------------------------
// bool IsTransmitted(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Verify a given signal having ability to transmit or not.
 *
 * @param whichOne      The index specifying a given signal.
 *
 * @return true for ability to transmit, false otherwise.
 */
//------------------------------------------------------------------------------
bool Receiver::IsTransmitted(Integer whichOne)
{
   return this->isTransmitted1;
}


//------------------------------------------------------------------------------
// Signal* GetSignal(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Get a specified signal.
 *
 * @param whichOne  The index specifying a given signal.
 *
 * @return          a signal for a given index.
 */
//------------------------------------------------------------------------------
Signal* Receiver::GetSignal(Integer whichOne)
{
   return RFHardware::GetSignal();
}


//------------------------------------------------------------------------------
// bool SetSignal(Signal* s, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Set a signal for a given index.
 *
 * @param s         The signal needed to set to
 * @param whichOne  The index specifying a given signal.
 *
 * @return          true if signal is set, false otherwise.
 */
//------------------------------------------------------------------------------
bool Receiver::SetSignal(Signal* s,Integer whichOne)
{
   return RFHardware::SetSignal(s);
}

