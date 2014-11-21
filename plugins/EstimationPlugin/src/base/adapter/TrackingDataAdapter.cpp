//$Id$
//------------------------------------------------------------------------------
//                           TrackingDataAdapter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 7, 2014
/**
 * Base class for the tracking data adapters
 */
//------------------------------------------------------------------------------

#include "TrackingDataAdapter.hpp"
#include "MeasurementException.hpp"
#include "TextParser.hpp"
#include "MessageInterface.hpp"
#include "PropSetup.hpp"
#include <sstream>

//#define DEBUG_CONSTRUCTION
//#define DEBUG_SET_PARAMETER
//#define DEBUG_INITIALIZATION

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
TrackingDataAdapter::PARAMETER_TEXT[AdapterParamCount - MeasurementModelBaseParamCount] =
{
   "SignalPath",
   "ObservationData",              // made changes by TUAN NGUYEN
   "RampTables",                   // made changes by TUAN NGUYEN
   "MeasurementType",              // made changes by TUAN NGUYEN
   "Bias",                         // made changes by TUAN NGUYEN
   "AddNoise",                     // made changes by TUAN NGUYEN
   "UplinkFrequency",              // made changes by TUAN NGUYEN
   "UplinkBand",                   // made changes by TUAN NGUYEN
};


const Gmat::ParameterType
TrackingDataAdapter::PARAMETER_TYPE[AdapterParamCount - MeasurementModelBaseParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,
   Gmat::OBJECTARRAY_TYPE,          // OBS_DATA          // made changes by TUAN NGUYEN
   Gmat::OBJECTARRAY_TYPE,          // RAMPED_TABLE      // made changes by TUAN NGUYEN
   Gmat::STRING_TYPE,               // MEASUREMENT_TYPE  // made changes by TUAN NGUYEN
   Gmat::RVECTOR_TYPE,              // BIAS              // made changes by TUAN NGUYEN
   Gmat::BOOLEAN_TYPE,              // ADD_NOISE         // made changes by TUAN NGUYEN
   Gmat::REAL_TYPE,                 // UPLINK_FREQUENCY  // made changes by TUAN NGUYEN
   Gmat::INTEGER_TYPE,              // UPLINK_BAND       // made changes by TUAN NGUYEN
};



//------------------------------------------------------------------------------
// TrackingDataAdapter(const std::string &typeStr, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param typeStr The string type of the adapter
 * @param name The name of the adapter
 */
//------------------------------------------------------------------------------
TrackingDataAdapter::TrackingDataAdapter(const std::string &typeStr,
      const std::string &name) :
   MeasurementModelBase (name, typeStr),
   measurementType      (""),              // made changes by TUAN NGUYEN
   calcData             (NULL),
   navLog               (NULL),
   logLevel             (0),
   solarsys             (NULL),
   modelID              (-1),
   modelTypeID          (-1),
   modelType            ("UnknownType"),
   multiplier           (1.0),
   withLighttime        (false),
   thePropagator        (NULL),
   uplinkFreq           (1.0e3),           // made changes by TUAN NGUYEN         // unit: Mhz
   freqBand             (1),               // made changes by TUAN NGUYEN
   obsData              (NULL),            // made changes by TUAN NGUYEN
   addNoise             (false),           // made changes by TUAN NGUYEN
   addBias              (true),            // made changes by TUAN NGUYEN
   rampTB               (NULL),            // made changes by TUAN NGUYEN
   beginIndex           (0),               // made changes by TUAN NGUYEN
   endIndex             (0)                // made changes by TUAN NGUYEN
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingDataAdapter default constructor <%p>\n", this);
#endif
}


//------------------------------------------------------------------------------
// ~TrackingDataAdapter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TrackingDataAdapter::~TrackingDataAdapter()
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingDataAdapter default destructor  <%p>\n", this);
#endif

   if (calcData)                            // made changes by TUAN NGUYEN
      delete calcData;                      // made changes by TUAN NGUYEN

#ifdef DEBUG_CONSTRUCTION
//   MessageInterface::ShowMessage("TrackingDataAdapter default destructor  step 1\n");
#endif
   if (thePropagator)
      delete thePropagator;

#ifdef DEBUG_CONSTRUCTION
//   MessageInterface::ShowMessage("TrackingDataAdapter default destructor  step 2\n");
#endif
   //if (cMeasurement.covariance)             // made changes by TUAN NGUYEN
   //   delete cMeasurement.covariance;       // made changes by TUAN NGUYEN
   cMeasurement.CleanUp();

   for(UnsignedInt i=0; i < participantLists.size(); ++i)
   {
      if (participantLists[i])
      {
         participantLists[i]->clear();
         delete participantLists[i];
      }
   }
   participantLists.clear();
#ifdef DEBUG_CONSTRUCTION
//   MessageInterface::ShowMessage("TrackingDataAdapter default destructor  step 3\n");
#endif

   for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
   {
      theDataDerivatives[i].clear();
   }
   theDataDerivatives.clear();

   refObjects.clear();
#ifdef DEBUG_CONSTRUCTION
//   MessageInterface::ShowMessage("TrackingDataAdapter default destructor  step 4\n");
#endif

}


//------------------------------------------------------------------------------
// TrackingDataAdapter(const TrackingDataAdapter& ma)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ma The adapter copied to make this new one
 */
//------------------------------------------------------------------------------
TrackingDataAdapter::TrackingDataAdapter(const TrackingDataAdapter& ma) :
   MeasurementModelBase (ma),
   measurementType      (ma.measurementType),     // made changes by TUAN NGUYEN
   participantLists     (ma.participantLists),  // made changes by TUAN NGUYEN
   calcData             (NULL),
   navLog               (ma.navLog),
   logLevel             (ma.logLevel),
   solarsys             (ma.solarsys),
   modelID              (ma.modelID),     // Should this be -1?
   modelTypeID          (ma.modelTypeID),
   modelType            (ma.modelType),
   multiplier           (ma.multiplier),
   withLighttime        (ma.withLighttime),
   thePropagator        (NULL),
   uplinkFreq           (ma.uplinkFreq),          // made changes by TUAN NGUYEN
   freqBand             (ma.freqBand),            // made changes by TUAN NGUYEN
   obsData              (ma.obsData),             // made changes by TUAN NGUYEN
   addNoise             (ma.addNoise),            // made changes by TUAN NGUYEN
   addBias              (ma.addBias),             // made changes by TUAN NGUYEN
   rampTB               (ma.rampTB),              // made changes by TUAN NGUYEN
   beginIndex           (ma.beginIndex),          // made changes by TUAN NGUYEN
   endIndex             (ma.endIndex),            // made changes by TUAN NGUYEN
   rampTableNames       (ma.rampTableNames)       // made changes by TUAN NGUYEN
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingDataAdapter copy constructor  from <%p> to <%p>\n", &ma, this);
#endif
   isInitialized = false;
}


//------------------------------------------------------------------------------
// TrackingDataAdapter& operator=(const TrackingDataAdapter& ma)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ma The adapter copied into this one
 *
 * @return This adapter, configured to match ma
 */
//------------------------------------------------------------------------------
TrackingDataAdapter& TrackingDataAdapter::operator=(
      const TrackingDataAdapter& ma)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingDataAdapter operator =  set <%p> = <%p>\n", this, &ma);
#endif

   if (this != &ma)
   {
      MeasurementModelBase::operator=(ma);

      measurementType    = ma.measurementType;    // made changes by TUAN NGUYEN
      navLog             = ma.navLog;
      logLevel           = ma.logLevel;
      solarsys           = ma.solarsys;
      modelID            = ma.modelID;          // Should this be -1?
      modelTypeID        = ma.modelTypeID;
      modelType          = ma.modelType;
      multiplier         = ma.multiplier;
      withLighttime      = ma.withLighttime;

      uplinkFreq         = ma.uplinkFreq;        // made changes by TUAN NGUYEN
      freqBand           = ma.freqBand;          // made changes by TUAN NGUYEN
      obsData            = ma.obsData;           // made changes by TUAN NGUYEN
      addNoise           = ma.addNoise;          // made changes by TUAN NGUYEN
      addBias            = ma.addBias;           // made changes by TUAN NGUYEN
      rampTB             = ma.rampTB;            // made changes by TUAN NGUYEN
      beginIndex         = ma.beginIndex;        // made changes by TUAN NGUYEN
      endIndex           = ma.endIndex;            // made changes by TUAN NGUYEN
      rampTableNames     = ma.rampTableNames;    // made changes by TUAN NGUYEN

      if (calcData)                                        // made changes by TUAN NGUYEN
      {                                                    // made changes by TUAN NGUYEN
         delete calcData;                                  // made changes by TUAN NGUYEN
         calcData = NULL;                                  // made changes by TUAN NGUYEN
      }                                                    // made changes by TUAN NGUYEN
      if (ma.calcData)                                     // made changes by TUAN NGUYEN
         calcData = (MeasureModel*)ma.calcData->Clone();   // made changes by TUAN NGUYEN

      refObjects.clear();
      isInitialized = false;
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 *
 * @param ss The pointer
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetSolarSystem(SolarSystem *ss)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetSolarSystem('%s')\n", this, ss->GetName().c_str()); 
#endif

   solarsys = ss;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns the script name for the parameter
 *
 * @param id The id of the parameter
 *
 * @return The script name
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetParameterText(const Integer id) const
{
   if (id >= MeasurementModelBaseParamCount && id < AdapterParamCount)
      return PARAMETER_TEXT[id - MeasurementModelBaseParamCount];
   return MeasurementModelBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string& str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a scriptable parameter
 *
 * @param str The string used for the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::GetParameterID(const std::string& str) const
{
   for (Integer i = MeasurementModelBaseParamCount; i < AdapterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - MeasurementModelBaseParamCount])
         return i;
   }
   return MeasurementModelBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a specified parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType TrackingDataAdapter::GetParameterType(
      const Integer id) const
{
   if (id >= MeasurementModelBaseParamCount && id < AdapterParamCount)
      return PARAMETER_TYPE[id - MeasurementModelBaseParamCount];

   return MeasurementModelBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description for a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The description string
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetParameterTypeString(const Integer id) const
{
   return MeasurementModelBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an integer parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::GetIntegerParameter(const Integer id) const
{
   if (id == UPLINK_BAND)
      return freqBand;

   return MeasurementModelBase::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// bool SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an integer parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::SetIntegerParameter(const Integer id, const Integer value)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetIntegerParameter('%s', %d)\n", this, GetParameterText(id).c_str(), value);
#endif

   if (id == UPLINK_BAND)
   {
      if (value <= 0)
         throw MeasurementException("Error: uplink frequency band has invalid value\n");

      freqBand = value;
      return freqBand;
   }

   return MeasurementModelBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an integer parameter
 *
 * @param label The name for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Sets the value for an integer parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::GetRealParameter(const Integer id) const
{
   if (id == UPLINK_FREQUENCY)
      return uplinkFreq;                        // unit: Mhz

   return MeasurementModelBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter (unit: Mhz for uplink frequency)
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::SetRealParameter(const Integer id, const Real value)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetRealParameter('%s', %lf)\n", this, GetParameterText(id).c_str(), value);
#endif

   if (id == UPLINK_FREQUENCY)
   {
      if (value < 0)
         throw MeasurementException("Error: uplink frequency has a negative value\n");

      uplinkFreq = value;             // unit: Mhz
      return uplinkFreq;
   }

   return MeasurementModelBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of an element in a real parameter
 *
 * @param id      The ID for the parameter
 * @param index   The index of an element in a real array
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::GetRealParameter(const Integer id, const Integer index) const
{
   return MeasurementModelBase::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const Integer id, const Real value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for an element in a real array parameter
 *
 * @param id      The ID for the parameter
 * @param value   The value for the parameter
 * @oaram index   The index of an element in a real array
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::SetRealParameter(const Integer id, const Real value, const Integer index)
{
   return MeasurementModelBase::SetRealParameter(id, value, index);
}

   
//------------------------------------------------------------------------------
// std::string GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a real parameter
 *
 * @param label The name for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param label The name for the parameter
 * @param value The value for the parameter
 *
 * @return setting value
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method calls the base class method.  It is provided for overload
 * compatibility.  See the base class description for a full description.
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::GetRealParameter(const std::string & label,
      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a Real parameter in an array.
 *
 * @param label The text label for the parameter that is to be set
 * @param value The new value for the parameter
 * @param index The index of the parameter
 *
 * @return The parameter value.
 */
//------------------------------------------------------------------------------
Real TrackingDataAdapter::SetRealParameter(const std::string & label,
      const Real value, const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


bool TrackingDataAdapter::GetBooleanParameter(const Integer id) const
{
   if (id == ADD_NOISE)
      return addNoise;

   return MeasurementModelBase::GetBooleanParameter(id);
}


bool TrackingDataAdapter::SetBooleanParameter(const Integer id, const bool value)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetBooleanParameter('%s', %s)\n", this, GetParameterText(id).c_str(), (value?"true":"false"));
#endif

   if (id == ADD_NOISE)
   {
      addNoise = value;
      return value;
   }

   return MeasurementModelBase::SetBooleanParameter(id, value);
}


bool TrackingDataAdapter::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


bool TrackingDataAdapter::SetBooleanParameter(const std::string &label, const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string parameter
 *
 * @param id The ID for the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetStringParameter(const Integer id) const
{
   if (id == MEASUREMENT_TYPE)
      return measurementType;

   return MeasurementModelBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param id The ID for the parameter
 * @param value The value for the parameter
 *
 * @return true if the paramter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetStringParameter(const Integer id,
      const std::string& value)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetStringParameter('%s', '%s')\n", this, GetParameterText(id).c_str(), value.c_str());
#endif


   if (id == MEASUREMENT_TYPE)
   {
      measurementType = value;
      return true;
   }

   if (id == SIGNAL_PATH)
   {
      StringArray *partList = DecomposePathString(value);
      if (partList->size() > 0)
      {
         participantLists.push_back(partList);

         // This code was moved to Initialize() function 
         // Make sure the measurement model has been set                             // made changes by TUAN NGUYEN
         if (!calcData)                                                              // made changes by TUAN NGUYEN
            throw MeasurementException("Unable to set parameter data on the "        // made changes by TUAN NGUYEN
            "measurement because the associated model has not yet been set.");       // made changes by TUAN NGUYEN

         // Pass the ordered participant lists to the measurement model              // made changes by TUAN NGUYEN
         Integer whichOne = participantLists.size() - 1;                             // made changes by TUAN NGUYEN
         for (UnsignedInt i = 0; i < partList->size(); ++i)                          // made changes by TUAN NGUYEN
            calcData->SetStringParameter("SignalPath", partList->at(i),              // made changes by TUAN NGUYEN
                  whichOne);                                                         // made changes by TUAN NGUYEN

         return true;
      }

      // The list is empty
      delete partList;
      return false;
   }

   return MeasurementModelBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param id The ID for the parameter
 * @param index The index into the array
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == RAMPTABLES)
   {
      if ((0 <= index)&&(index < rampTableNames.size()))
         return rampTableNames[index];
      else
      {
         std::stringstream ss;
         ss << "Error: index (" << index << ") is out of bound (" << rampTableNames.size() << ")\n";
         throw MeasurementException(ss.str());
      }
   }

   return MeasurementModelBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array of strings
 *
 * @param id The ID for the parameter
 * @param value The new value for the parameter
 * @index The desired location of the parameter in the array
 *
 * @return true if the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetStringParameter(const Integer id,
      const std::string& value, const Integer index)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetStringParameter('%s', '%s', index = %d)\n", this, GetParameterText(id).c_str(), value.c_str(), index);
#endif

   if (id == RAMPTABLES)
   {
      if (value == "")
         throw MeasurementException("Error: Name of ramped frequency table is an empty string\n");

      if ((0 <= index)&&(index < rampTableNames.size()))
         rampTableNames[index] = value;
      else
         rampTableNames.push_back(value);
      return true;
   }

   return MeasurementModelBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array parameter
 *
 * @param id The ID for the parameter
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingDataAdapter::GetStringArrayParameter(
      const Integer id) const
{
   if (id == RAMPTABLES)                                  // made changes by TUAN NGUYEN
      return rampTableNames;                              // made changes by TUAN NGUYEN

   return MeasurementModelBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retireves a string array from an array of string arrays
 *
 * @param id The ID for the parameter
 * @param index The index into the array of arrays
 *
 * @return The requested string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingDataAdapter::GetStringArrayParameter(
      const Integer id, const Integer index) const
{
   if (id == SIGNAL_PATH)
   {
      if ((index >= 0) && ((Integer)participantLists.size() > index))
         return *(participantLists[index]);
      throw MeasurementException("Index out of bounds when accessing the "
            "signal path");
   }

   return MeasurementModelBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value of a string parameter
 *
 * @param label The scriptable name of the parameter
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetStringParameter(
      const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The scriptable name of the parameter
 * @param value The value for the parameter
 *
 * @return true if the paramter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param label The scriptable name of the parameter
 * @param index The index into the array
 *
 * @return The value of the parameter
 */
//------------------------------------------------------------------------------
std::string TrackingDataAdapter::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter in an array of strings
 *
 * @param label The scriptable name of the parameter
 * @param value The new value for the parameter
 * @index The desired location of the parameter in the array
 *
 * @return true if the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array parameter
 *
 * @param label The scriptable name of the parameter
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingDataAdapter::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retireves a string array from an array of string arrays
 *
 * @param label The scriptable name of the parameter
 * @param index The index into the array of arrays
 *
 * @return The requested string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingDataAdapter::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}



//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string& oldName,
//       const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Adjusts names for objects referenced by this one when they are renamed
 *
 * @param type The type of the object that is renamed
 * @param oldName The name of the object before it was changed
 * @param newName The new name for the object
 *
 * @return true if a reference name was changed, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TrackingDataAdapter<%p>::RenameRefObject(type = %d, oldName = '%s', newName = %s)\n", this, type, oldName.c_str(), newName.c_str());
#endif

   bool retval = false;

   // Build the list
   if (calcData)
   {
      retval = calcData->RenameRefObject(type, oldName, newName);
   }

   return retval;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of the reference objects used in the model
 *
 * @param type The type of object requested (not used)
 *
 * @return The list of names of reference objects
 */
//------------------------------------------------------------------------------
const StringArray& TrackingDataAdapter::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   refObjectNames.clear();

   // Build the list
   if (calcData)
   {
      refObjectNames = calcData->GetRefObjectNameArray(type);
   }

   return refObjectNames;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets pointers to the model's reference objects
 *
 * @param obj The object pointer
 * @param type The type of the object (not used)
 * @param name The name of the object  (not used)
 *
 * @return true if the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetRefObject(GmatBase* obj,
      const Gmat::ObjectType type, const std::string& name)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetRefObject(obj = <%p>, type = %d, name = '%s')\n", this, obj, type, name.c_str());
#endif

   bool retval = false;

   if (calcData)
   {
      retval = calcData->SetRefObject(obj, type, name);
   }

   if (find(refObjects.begin(), refObjects.end(), obj) == refObjects.end())
   {
      refObjects.push_back(obj);
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//       const std::string& name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the pointers for the reference object
 *
 * @param obj The object pointer
 * @param type The type of the object (not used)
 * @param name The name of the object  (not used)
 * @param index Index for the object's location
 *
 * @return true if the pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetRefObject(GmatBase* obj,
      const Gmat::ObjectType type, const std::string& name, const Integer index)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetRefObject(obj = <%p>, type = %d, name = '%s', index  %d)\n", this, obj, type, name.c_str(), index);
#endif

   bool retval = false;

   if (calcData)
   {
      retval = calcData->SetRefObject(obj, type, name, index);
   }

   if ((index >= 0) && (index < (Integer)refObjects.size()))
   {
      refObjects[index] = obj;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetMeasurement(MeasureModel* meas)
//------------------------------------------------------------------------------
/**
 * Sets the measurement model pointer
 *
 * @param meas The pointer
 *
 * @return true if set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetMeasurement(MeasureModel* meas)
{
#ifdef DEBUG_SET_PARAMETER
   MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetMeasurement(meas = <%p>)\n", this, meas);
#endif

   bool retval = false;

   if (meas)
   {
      calcData = meas;
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// const MeasurementData& GetMeasurement()
//------------------------------------------------------------------------------
/**
 * Retrieves computed measurement data
 *
 * @return The measurement data
 */
//------------------------------------------------------------------------------
const MeasurementData& TrackingDataAdapter::GetMeasurement()
{
   if (!calcData)
      throw MeasurementException("Measurement data was requested for " +
            instanceName + " before the measurement was set");

   return cMeasurement;
}


//------------------------------------------------------------------------------
// bool WriteMeasurements()
//------------------------------------------------------------------------------
/**
 * Method to write the measurement data
 *
 * @todo: Implement this method
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::WriteMeasurements()
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// bool WriteMeasurement(Integer id)
//------------------------------------------------------------------------------
/**
 * Method to write a single measurement
 *
 * @param id ID for the measurement that is written
 *
 * @todo: Implement this method
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::WriteMeasurement(Integer id)
{
   bool retval = false;

   return retval;
}

//------------------------------------------------------------------------------
// Integer HasParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
 * Checks to see if adapter has covariance data for a specified parameter ID
 *
 * @param paramID The parameter ID
 *
 * @return Size of the covariance data that is available
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::HasParameterCovariances(Integer parameterId)
{
   return 0;
}


//------------------------------------------------------------------------------
// Integer GetEventCount()
//------------------------------------------------------------------------------
/**
 * Returns the number of light time solution events in the measurement
 *
 * @return The event count
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::GetEventCount()
{
   return 0;
}


//------------------------------------------------------------------------------
// void SetPropagator(PropSetup* ps)
//------------------------------------------------------------------------------
/**
 * Passes a propagator to the adapter for use in light time iterations.  The
 * propagator is cloned so that propagation of single spacecraft can be
 * performed.
 *
 * @param ps The PropSetup that is being set
 *
 * @todo The current call takes a single propagator.  Once the estimation system
 *       supports multiple propagators, this should be changed to a vector of
 *       PropSetup objects.
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetPropagator(PropSetup* ps)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("TrackingDataAdapter<%p>::SetPropagator(ps = <%p>)\n", this, ps);
   #endif
   
   if (thePropagator != NULL)
      delete thePropagator;
   
   if (ps != NULL)
   {
      thePropagator = (PropSetup*)(ps->Clone());

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("   TDA propagator is the clone %p\n",
               thePropagator);
      #endif

         calcData->SetPropagator(thePropagator);
   }
   else
      thePropagator = NULL;
}


// made changes by TUAN NGUYEN
MeasureModel* TrackingDataAdapter::GetMeasurementModel() 
{
   return calcData;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the adapter for use
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Start Initializing a Tracking Data Adapter <%p>\n", this);
   #endif

   bool retval = false;

   if (MeasurementModelBase::Initialize())
   {
      if (calcData == NULL)
         throw MeasurementException("TrackingDataAdapter Initialization failed "
               "because the measurement model is not yet set");

      if (thePropagator)
         calcData->SetPropagator(thePropagator);

      calcData->SetSolarSystem(solarsys);
      calcData->SetProgressReporter(navLog);
      calcData->UsesLightTime(withLighttime);

      retval = calcData->Initialize();
   }

   cMeasurement.type = modelTypeID;
   cMeasurement.typeName = modelType;
   cMeasurement.uniqueID = modelID;

   if (cMeasurement.covariance == NULL)
      cMeasurement.covariance = new Covariance;

   // Default to a 1x1 identity covariance
   cMeasurement.covariance->SetDimension(1);
   (*(cMeasurement.covariance))(0,0) = 1.0;

   if (navLog != NULL)
      logLevel = navLog->GetLogLevel("Adapter");
   else
      logLevel = 32767;

   if (!retval && navLog && (logLevel == 0))
      navLog->WriteData("Error initializing the measurement adapter " +
            instanceName);

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("End Initializing a Tracking Data Adapter <%p>\n", this);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// void SetCorrection(const std::string correctionName,
//       const std::string correctionType)
//------------------------------------------------------------------------------
/**
 * Passes a correction name into the owned CoreMeasurement
 *
 * @param correctionName The name of the correction
 * @param correctionType The type of correction
 *
 * @todo This method isn't yet hooked up, and might change as corrections are
 *       incorporated into the system
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetCorrection(const std::string& correctionName,
      const std::string& correctionType)
{
   calcData->SetCorrection(correctionName, correctionType);                  // made changes by TUAN NGUYEN
}


//------------------------------------------------------------------------------
// bool SetProgressReporter(ProgressReporter* reporter)
//------------------------------------------------------------------------------
/**
 * Sets the reporter for the Adapter
 *
 * @param reporter The reporter that should be used
 *
 * @return true if set, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingDataAdapter::SetProgressReporter(ProgressReporter* reporter)
{
   bool retval = false;

   if (reporter)
   {
      navLog = reporter;
      logLevel = reporter->GetLogLevel("Adapter");
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// void SetModelID(const Integer newID)
//------------------------------------------------------------------------------
/**
 * Sets the model's unique ID number
 *
 * @param newID the model ID used in the current run
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetModelID(const Integer newID)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting ModelID to %d\n", newID);
   #endif

   modelID = newID;
   cMeasurement.uniqueID = modelID;
}


//------------------------------------------------------------------------------
// Integer GetModelID()
//------------------------------------------------------------------------------
/**
 * Retrieves the unique model ID for the current run
 *
 * @return The ID
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::GetModelID()
{
   return modelID;
}


//------------------------------------------------------------------------------
// Integer GetModelTypeID()
//------------------------------------------------------------------------------
/**
 * Returns the model's magic number
 *
 * @return The type ID, used to speed lookups and to identify model subtypes
 */
//------------------------------------------------------------------------------
Integer TrackingDataAdapter::GetModelTypeID()
{
   return modelTypeID;
}

//------------------------------------------------------------------------------
// void SetModelTypeID(const Integer theID, const std::string type, Real mult)
//------------------------------------------------------------------------------
/**
 * Sets fields used for all of teh Adapters of a given type and subtype
 *
 * @param theID The measuremetn's magic number used internally and in .gmd files
 * @param type The typ descriptor for the model
 * @param mult A multiplicative factor that some models use (defaults to 1.0)
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::SetModelTypeID(const Integer theID,
      const std::string type, Real mult)
{
   modelTypeID = theID;
   modelType = type;
   multiplier = mult;
}

//------------------------------------------------------------------------------
// void UsesLightTime(const bool tf)
//------------------------------------------------------------------------------
/**
 * Method used to set or clear the light time solution flag
 *
 * @param tf true to find light time solutions, false to omit them
 */
//------------------------------------------------------------------------------
void TrackingDataAdapter::UsesLightTime(const bool tf)
{
   withLighttime = tf;
}

//------------------------------------------------------------------------------
// StringArray* DecomposePathString(const std::string &value)
//------------------------------------------------------------------------------
/**
 * Breaks apart a signal path string into a participant list
 *
 * @param value The string
 *
 * @return The ordered vector of participant names
 */
//------------------------------------------------------------------------------
StringArray* TrackingDataAdapter::DecomposePathString(const std::string &value)
{
   TextParser theParser;
   StringArray *partList = new StringArray;

   (*partList)  = theParser.SeparateBy(value, ", ");

   if (navLog)
   {
      if (logLevel == 0)
      {
         std::stringstream msg;
         msg << "Path string " << value << " has members:\n";
         for (UnsignedInt i = 0; i < partList->size(); ++i)
            msg << "   " + partList->at(i) + "\n";

         navLog->WriteData(msg.str());
      }
   }

   return partList;
}


// made changes by TUAN NGUYEN
void TrackingDataAdapter::SetMultiplierFactor(Real mult)
{
   multiplier = mult;
}


// made changes by TUAN NGUYEN
Real TrackingDataAdapter::GetMultiplierFactor()
{
   return multiplier;
}


void TrackingDataAdapter::ComputeMeasurementBias(const std::string biasName)
{
   // Get signal data
   std::vector<SignalData*> data = calcData->GetSignalData();
   UnsignedInt measurementSize = data.size();

   // Calculate bias for each measurement (signal path)
   measurementBias.clear();
   Real val, bias;
   for (UnsignedInt i = 0; i < measurementSize; ++i)
   {
      //bias = 0.0;
      //SignalData* sd = data[i];
      //while (sd != NULL)
      //{
      //   // Get bias of transmit participant
      //   try
      //   {
      //      val = sd->tNode->GetRealParameter(biasName);
      //   }
      //   catch(...)
      //   {
      //      val = 0.0;               // if cannot get value of NoiseSigma, set it to 0.0; 
      //   }
      //   bias = bias + val;          // bias of a signal path equals sum of both participants' bias

      //   // Get bias of receive participant
      //   try
      //   {
      //      val = sd->rNode->GetRealParameter(biasName);
      //   }
      //   catch(...)
      //   {
      //      val = 0.0;               // if cannot get value of NoiseSigma, set it to 0.0; 
      //   }
      //   bias = bias + val;          // bias of a signal path equals sum of both participants' bias

      //   // Move the next signal leg
      //   sd = sd->next;
      //}

      // Get first and last signal data
      SignalData* first = data[i];
      SignalData* last = data[i];
      while (last->next != NULL)
         last = last->next;

      bias = 0.0;
      if ((first->tNode->IsOfType(Gmat::GROUND_STATION)) && (last->rNode->IsOfType(Gmat::GROUND_STATION) == false))
         bias = first->tNode->GetRealParameter(biasName);
      else if (last->rNode->IsOfType(Gmat::GROUND_STATION))
         bias = last->rNode->GetRealParameter(biasName);

      measurementBias.push_back(bias);
   }

   // Clean up memmory
   data.clear();
}


void TrackingDataAdapter::ComputeMeasurementNoiseSigma(const std::string noiseSigmaName)
{
   // Get signal data
   std::vector<SignalData*> data = calcData->GetSignalData();
   UnsignedInt measurementSize = data.size();

   // Calculate noise sigma for each signal path
   noiseSigma.clear();
   Real val, noise;
   for (UnsignedInt i = 0; i < measurementSize; ++i)
   {
      //noise = 0.0;
      //SignalData* sd = data[i];
      //while (sd != NULL)
      //{
      //   // Get noise sigma of transmit participant
      //   try
      //   {
      //      val = sd->tNode->GetRealParameter(noiseSigmaName);
      //   }
      //   catch(...)
      //   {
      //      val = 0.0;         // if cannot get value of NoiseSigma, set it to 0.0; 
      //   }
      //   noise = noise + val;

      //   // Get noise sigma of receive participant
      //   try
      //   {
      //      val = sd->rNode->GetRealParameter(noiseSigmaName);
      //   }
      //   catch(...)
      //   {
      //      val = 0.0;         // if cannot get value of NoiseSigma, set it to 0.0; 
      //   }
      //   noise = noise + val;

      //   // Move to next signal leg
      //   sd = sd->next;
      //}

      // Get first and last signal data
      SignalData* first = data[i];
      SignalData* last = data[i];
      while (last->next != NULL)
         last = last->next;

      noise = 0.0;
      if ((first->tNode->IsOfType(Gmat::GROUND_STATION)) && (last->rNode->IsOfType(Gmat::GROUND_STATION) == false))
         noise = first->tNode->GetRealParameter(noiseSigmaName);
      else if (last->rNode->IsOfType(Gmat::GROUND_STATION))
         noise = last->rNode->GetRealParameter(noiseSigmaName);


      noiseSigma.push_back(noise);
   }// for i

   // Clean up memmory
   data.clear();
}


void TrackingDataAdapter::ComputeMeasurementErrorCovarianceMatrix()
{
   // Get signal data
   std::vector<SignalData*> data = calcData->GetSignalData();
   UnsignedInt measurementSize = data.size();

   // Calculate measurement error covariance matrix
   measErrorCovariance.SetDimension(measurementSize);
   for (UnsignedInt i = 0; i < measurementSize; ++i)
   {
      for (Integer j = 0; j < measurementSize; ++j)
      {
         if (i != j)
            measErrorCovariance(i,j) = 0.0;
         else
         {
            if (noiseSigma[i] != 0.0)
               measErrorCovariance(i,j) =  noiseSigma[i] * noiseSigma[i];
            else
               measErrorCovariance(i,j) =  1.0;          // if no noise setting, set it to 1.0
         }
      }
   }// for i

   // Clean up memmory
   data.clear();
}



#include "SignalBase.hpp"
void TrackingDataAdapter::BeginEndIndexesOfRampTable(Integer & err)
{
   // 1. Get search key
   std::vector<SignalBase*> paths = calcData->GetSignalPaths();
   SignalBase* leg = paths[0];
   SignalData* sd = leg->GetSignalDataObject();

   std::string gsName, scName, gsID, scID, searchkey;
   if (sd->tNode->IsOfType(Gmat::GROUND_STATION))
   {
      gsName = sd->tNode->GetName();
      gsID   = sd->tNode->GetStringParameter("Id");
      scName = sd->rNode->GetName();
      scID   = sd->rNode->GetStringParameter("Id");
   }
   else
   {
      gsName = sd->rNode->GetName();
      gsID   = sd->rNode->GetStringParameter("Id");
      scName = sd->tNode->GetName();
      scID   = sd->tNode->GetStringParameter("Id");
   }
   searchkey = gsID + " " + scID + " ";

   // 2. Search for the beginning index
   beginIndex = 0;
   for(; beginIndex < (*rampTB).size(); ++beginIndex)
   {
      if (searchkey == (*rampTB)[beginIndex].indexkey.substr(0,searchkey.size()))
         break;
   }

   // 3. Search for the ending index
   endIndex = beginIndex; 
   for(; endIndex < (*rampTB).size(); ++endIndex)
   {
      if (searchkey != (*rampTB)[endIndex].indexkey.substr(0,searchkey.size()))
         break;
   }

   // 4. Verify number of data records
   if ((endIndex - beginIndex) < 2)
   {
      err = 3;
      std::stringstream ss;
      ss << "Error: Ramp table has " << (endIndex - beginIndex) << " frequency data records for uplink signal from "<< gsName << " to " << scName << ". It needs at least 2 records\n";
      throw MeasurementException(ss.str());
   }
}




Real TrackingDataAdapter::IntegralRampedFrequency(Real t1, Real delta_t, Integer& err)
{
#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage("Enter PhysicalMeasurement::IntegralRampedFrequency()\n");
#endif

   err = 0;
   if (delta_t < 0)
   {
      err = 1;
      throw MeasurementException("Error: Elapse time has to be a non negative number\n");
   }

   if (rampTB == NULL)
   {
      err = 2;
      throw MeasurementException("Error: No ramp table available for measurement calculation\n");
   }
   
   if ((*rampTB).size() < 2)
   {
      err = 3;
      std::stringstream ss;
      ss << "Error: Ramp table has " << (*rampTB).size() << " data records. It needs at least 2 records\n";
      throw MeasurementException(ss.str());
   }
   
   // Get the beginning index and the ending index for frequency data records for this measurement model 
   BeginEndIndexesOfRampTable(err);

   Real t0 = t1 - delta_t/GmatTimeConstants::SECS_PER_DAY; 
   //Real time_min = (*rampTB)[0].epoch;
   //Real time_max = (*rampTB)[(*rampTB).size() -1 ].epoch;
   Real time_min = (*rampTB)[beginIndex].epoch;
   Real time_max = (*rampTB)[endIndex-1].epoch;


#ifdef RAMP_TABLE_EXPANDABLE
   Real correct_val = 0;
   if (t1 < time_min)
   {
      // t0 and t1 < time_min
      //return delta_t*(*rampTB)[0].rampFrequency;
      return delta_t*(*rampTB)[beginIndex].rampFrequency;
   }
   else if (t1 > time_max)
   {
      if (t0 < time_min)
      {
         // t0 < time_min < time_max < t1
         //correct_val = (*rampTB)[0].rampFrequency * (time_min-t0)*GmatTimeConstants::SECS_PER_DAY;
         correct_val = (*rampTB)[beginIndex].rampFrequency * (time_min-t0)*GmatTimeConstants::SECS_PER_DAY;
         t0 = time_min;
      }
      else if (t0 > time_max)
      {
         // t0 and t1 > time_max
         //return delta_t*(*rampTB)[(*rampTB).size()-1].rampFrequency;
         return delta_t*(*rampTB)[endIndex-1].rampFrequency;
      }
      else
      {
         // time_min <= t0 <= time_max < t1
         //correct_val = (*rampTB)[(*rampTB).size() -1].rampFrequency * (t1-time_max)*GmatTimeConstants::SECS_PER_DAY;
         correct_val = (*rampTB)[endIndex -1].rampFrequency * (t1-time_max)*GmatTimeConstants::SECS_PER_DAY;
         t1 = time_max;
      }
   }
   else
   {
      if (t0 < time_min)
      {
         // t0 < time_min <= t1 <= time_max
         //correct_val = (*rampTB)[0].rampFrequency * (time_min-t0)*GmatTimeConstants::SECS_PER_DAY;
         correct_val = (*rampTB)[beginIndex].rampFrequency * (time_min-t0)*GmatTimeConstants::SECS_PER_DAY;
         t0 = time_min;
      }
   }
#endif

   if ((t1 < time_min)||(t1 > time_max))
   {
      char s[200];
      sprintf(&s[0], "Error: End epoch t3R = %.12lf is out of range [%.12lf , %.12lf] of ramp table\n", t1, time_min, time_max);
      std::string st(&s[0]);
      err = 4;
      throw MeasurementException(st);
   }

   
   if ((t0 < time_min)||(t0 > time_max))
   {
      char s[200];
      sprintf(&s[0], "Error: Start epoch t1T = %.12lf is out of range [%.12lf , %.12lf] of ramp table\n", t0, time_min, time_max);
      std::string st(&s[0]);
      err = 5;
      throw MeasurementException(st);
   }

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage(" Start epoch t1 = %.15lf A1Mjd\n", t0);
   MessageInterface::ShowMessage(" End epoch t3   = %.15lf A1Mjd\n", t1);
   MessageInterface::ShowMessage(" elapse time   = %.15lf s\n", delta_t);
#endif
   // search for end interval:
   //UnsignedInt end_interval = 0;
   //for (UnsignedInt i = 1; i < (*rampTB).size(); ++i)
   //{
   //   if (t1 < (*rampTB)[i].epoch)
   //   {
   //      end_interval = i-1;      
   //      break;
   //   }
   //}
   UnsignedInt end_interval = beginIndex;
   for (UnsignedInt i = beginIndex+1; i < endIndex; ++i)
   {
      if (t1 < (*rampTB)[i].epoch)
      {
         end_interval = i-1;      
         break;
      }
   }

   Real basedFreq = (*rampTB)[end_interval].rampFrequency; 

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage("\n End interval: i = %d    epoch = %.12lf A1Mjd    frequency = %.12lf    ramp rate = %.12lf\n", end_interval, (*rampTB)[end_interval].epoch, (*rampTB)[end_interval].rampFrequency, (*rampTB)[end_interval].rampRate);
   MessageInterface::ShowMessage("               i = %d    epoch = %.12lf A1Mjd    frequency = %.12lf    ramp rate = %.12lf\n\n", end_interval+1, (*rampTB)[end_interval+1].epoch, (*rampTB)[end_interval+1].rampFrequency, (*rampTB)[end_interval+1].rampRate);
   MessageInterface::ShowMessage(" Based frequency = %.15le\n", basedFreq);
#endif

   // search for end interval:
   Real f0, f1, f_dot;
   Real value1;
   Real interval_len;

   Real value = 0.0;
   Real dt = delta_t;
   Integer i = end_interval;
   while (dt > 0)
   {
      f_dot = (*rampTB)[i].rampRate;

      // Specify lenght of the current interval
      if (i == end_interval)
         interval_len = (t1 - (*rampTB)[i].epoch)*GmatTimeConstants::SECS_PER_DAY;
      else
         interval_len = ((*rampTB)[i+1].epoch - (*rampTB)[i].epoch)*GmatTimeConstants::SECS_PER_DAY;

      f0 = (*rampTB)[i].rampFrequency;
      if (dt < interval_len)
      {
         f0 = f0 + f_dot*(interval_len - dt);
         interval_len = dt;
      }
      // Specify frequency at the end of the current interval
      f1 = f0 + f_dot*interval_len;

      // Take integral for the current interval
      value1 = ((f0 + f1)/2 - basedFreq) * interval_len;
      value  = value + value1;

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
      MessageInterface::ShowMessage("interval i = %d:    value1 = %.12lf    f0 = %.12lf   f1 = %.12lf     f_ave = %.12lfHz   width = %.12lfs \n", i, value1, f0, f1, (f0+f1)/2, interval_len);
      MessageInterface::ShowMessage("interval i = %d: epoch = %.12lf     band = %d    ramp type = %d   ramp freq = %.12le    ramp rate = %.12le\n", i,
      (*rampTB)[i].epoch,  (*rampTB)[i].uplinkBand, (*rampTB)[i].rampType, (*rampTB)[i].rampFrequency, (*rampTB)[i].rampRate);
#endif

      // Specify dt 
      dt = dt - interval_len;

      i--;
   }
   Real rel_val = value;
   value = value + basedFreq*delta_t;

#ifdef DEBUG_INTEGRAL_RAMPED_FREQUENCY
   MessageInterface::ShowMessage(" value = %.15lf     relative value = %.15lf    based value = %.15lf\n", value, rel_val, basedFreq*delta_t);
   MessageInterface::ShowMessage("Exit PhysicalMeasurement::IntegralRampedFrequency()\n");
#endif

#ifdef RAMP_TABLE_EXPANDABLE
   return value + correct_val;
#else
   return value;
#endif

}

