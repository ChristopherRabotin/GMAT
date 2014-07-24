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

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
TrackingDataAdapter::PARAMETER_TEXT[AdapterParamCount - MeasurementModelBaseParamCount] =
{
   "SignalPath",
};


const Gmat::ParameterType
TrackingDataAdapter::PARAMETER_TYPE[AdapterParamCount - MeasurementModelBaseParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,
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
   calcData             (NULL),
   navLog               (NULL),
   logLevel             (0),
   solarsys             (NULL),
   modelID              (-1),
   modelTypeID          (-1),
   modelType            ("UnknownType"),
   multiplier           (1.0),
   withLighttime        (false),
   thePropagator        (NULL)
{
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
   if (thePropagator)
      delete thePropagator;
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
   participantLists     (ma.participantLists),
   calcData             (NULL),
   navLog               (ma.navLog),
   logLevel             (ma.logLevel),
   solarsys             (ma.solarsys),
   modelID              (ma.modelID),     // Should this be -1?
   modelTypeID          (ma.modelTypeID),
   modelType            (ma.modelType),
   multiplier           (ma.multiplier),
   withLighttime        (ma.withLighttime),
   thePropagator        (NULL)
{
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
   if (this != &ma)
   {
      MeasurementModelBase::operator=(ma);

      navLog             = ma.navLog;
      logLevel           = ma.logLevel;
      solarsys           = ma.solarsys;
      modelID            = ma.modelID;          // Should this be -1?
      modelTypeID        = ma.modelTypeID;
      modelType          = ma.modelType;
      multiplier         = ma.multiplier;
      withLighttime      = ma.withLighttime;

      calcData = (MeasureModel*)ma.calcData->Clone();

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
   //// Make sure the measurement model has been set
   //if (!calcData)
   //   throw MeasurementException("Unable to set parameter data on the "
   //         "measurement because the associated model has not yet been set.");

   if (id == SIGNAL_PATH)
   {
      StringArray *partList = DecomposePathString(value);
      if (partList->size() > 0)
      {
         participantLists.push_back(partList);

        // Make sure the measurement model has been set
        if (!calcData)
           throw MeasurementException("Unable to set parameter data on the "
            "measurement because the associated model has not yet been set.");

         // Pass the ordered participant lists to the measurement model
         Integer whichOne = participantLists.size() - 1;
         for (UnsignedInt i = 0; i < partList->size(); ++i)
            calcData->SetStringParameter("SignalPath", partList->at(i),
                  whichOne);

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
      MessageInterface::ShowMessage("Setting propagator to %p in "
            "TrackingDataAdapter\n", ps);
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
      MessageInterface::ShowMessage("Initializing a Tracking Data Adapter\n");
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
