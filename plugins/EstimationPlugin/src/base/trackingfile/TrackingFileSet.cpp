//$Id$
//------------------------------------------------------------------------------
//                           TrackingFileSet
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
// Created: Mar 11, 2014
/**
 * The class of objects that define GMAT measurement models and tracking data
 */
//------------------------------------------------------------------------------

#include "TrackingFileSet.hpp"
#include "TFSMagicNumbers.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"
#include <sstream>                  // For magic number mapping code
/// Temporarily here; needs to move into a factory
#include "RangeAdapterKm.hpp"
#include "DSNRangeAdapter.hpp"
#include "DopplerAdapter.hpp"
#include "GNDopplerAdapter.hpp"
#include "TDRSDopplerAdapter.hpp"
#include "RangeRateAdapterKps.hpp"
#include "PointRangeRateAdapterKps.hpp"

//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION
//#define DEBUG_INITIALIZE
//#define DEBUG_GET_PARAMETER
//#define DEBUG_BUILD_ADAPTER

//------------------------------------------------------------------------------
// Static Data
//------------------------------------------------------------------------------

/// Strings describing the BatchEstimator parameters
const std::string TrackingFileSet::PARAMETER_TEXT[
                             TrackingFileSetParamCount - MeasurementModelBaseParamCount] =
{
   "AddTrackingConfig",             // TRACKINGCONFIG
   "FileName",                      // FILENAME
   "RampTable",                     // RAMPED_TABLE
   "UseLightTime",                  // USELIGHTTIME
   "UseRelativityCorrection",       // USE_RELATIVITY
   "UseETminusTAI",                 // USE_ETMINUSTAI
   "SimRangeModuloConstant",        // RANGE_MODULO
   "SimDopplerCountInterval",       // DOPPLER_COUNT_INTERVAL
   "SimTDRSServiceAccessList",      // TDRS_SERVICE_ACCESS
   "SimTDRSNode4Frequency",         // TDRS_NODE4_FREQUENCY
   "SimTDRSNode4FrequencyBand",     // TDRS_NODE4_BAND
   "SimTDRSSmarId",                 // TDSR_SMAR_ID
   "SimTDRSDataFlag",               // TDRS_DATA_FLAG
   "DataFilters",                   // DATA_FILTERS
};

/// Types of the BatchEstimator parameters
const Gmat::ParameterType TrackingFileSet::PARAMETER_TYPE[
                             TrackingFileSetParamCount - MeasurementModelBaseParamCount] =
{
   Gmat::STRINGARRAY_TYPE,          // TRACKINGCONFIG
   Gmat::STRINGARRAY_TYPE,          // FILENAME, but it's a list of names...
   Gmat::STRINGARRAY_TYPE,          // RAMPED_TABLE, but it's a list of names...
   Gmat::BOOLEAN_TYPE,              // USELIGHTTIME
   Gmat::BOOLEAN_TYPE,              // USE_RELATIVITY
   Gmat::BOOLEAN_TYPE,              // USE_ETMINUSTAI
   Gmat::REAL_TYPE,                 // RANGE_MODULO
   Gmat::REAL_TYPE,                 // DOPPLER_COUNT_INTERVAL
   Gmat::STRINGARRAY_TYPE,          // TDRS_SERVICE_ACCESS
   Gmat::REAL_TYPE,                 // TDRS_NODE4_FREQUENCY
   Gmat::INTEGER_TYPE,              // TDRS_NODE4_BAND
   Gmat::INTEGER_TYPE,              // TDRS_SMAR_ID
   Gmat::INTEGER_TYPE,              // TDRS_DATA_FLAG
   Gmat::OBJECTARRAY_TYPE,          // DATA_FILLTERS
};


//------------------------------------------------------------------------------
// TrackingFileSet()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the new object
 */
//------------------------------------------------------------------------------
TrackingFileSet::TrackingFileSet(const std::string &name) :
   MeasurementModelBase      (name, "TrackingFileSet"),
   useLighttime              (true),
   solarsystem               (NULL),
   thePropagator             (NULL),
   useRelativityCorrection   (false),
   useETminusTAICorrection   (false),
   rangeModulo               (1.0e18),
   dopplerCountInterval      (1.0),
   tdrsNode4Frequency        (2000.0),              // unit: MHz
   tdrsNode4Band             (1),                   // 0: unspecified, 1: S-band, 2: X-band, 3: K-band
   tdrsSMARID                (0),
   tdrsDataFlag              (0)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingFileSet <%s,%p> default construction \n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::MEASUREMENT_MODEL);
   objectTypeNames.push_back("TrackingFileSet");

   parameterCount = TrackingFileSetParamCount;
}


//------------------------------------------------------------------------------
// ~TrackingFileSet()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TrackingFileSet::~TrackingFileSet()
{
   // remove all created measurements
   for(UnsignedInt i = 0; i < measurements.size(); ++i)
   {
      if (measurements[i])
         delete measurements[i];
   }

   // remove all data filter objects
   for(UnsignedInt i = 0; i < dataFilters.size(); ++i)
   {
      if (dataFilters[i])
         delete dataFilters[i];
   }
   dataFilters.clear();

   // clear tracking configs
   trackingConfigs.clear();

   // clear file names and ramp table names
   filenames.clear();
   rampedTablenames.clear();

   // clear TDRS service access list
   tdrsServiceAccessList.clear();
}


//------------------------------------------------------------------------------
// TrackingFileSet(const TrackingFileSet& tfs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param tfs The TrackingFileSet copied to make a new one
 */
//------------------------------------------------------------------------------
TrackingFileSet::TrackingFileSet(const TrackingFileSet& tfs) :
   MeasurementModelBase      (tfs),
   filenames                 (tfs.filenames),
   rampedTablenames          (tfs.rampedTablenames),
   useLighttime              (tfs.useLighttime),
   solarsystem               (tfs.solarsystem),
   thePropagator             (tfs.thePropagator),
   references                (tfs.references),
   datafiles                 (tfs.datafiles),
   useRelativityCorrection   (tfs.useRelativityCorrection),
   useETminusTAICorrection   (tfs.useETminusTAICorrection),
   rangeModulo               (tfs.rangeModulo),
   dopplerCountInterval      (tfs.dopplerCountInterval),
   tdrsServiceAccessList     (tfs.tdrsServiceAccessList),
   tdrsNode4Frequency        (tfs.tdrsNode4Frequency),
   tdrsNode4Band             (tfs.tdrsNode4Band),
   tdrsSMARID                (tfs.tdrsSMARID),
   tdrsDataFlag              (tfs.tdrsDataFlag),
   dataFilterNames           (tfs.dataFilterNames)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingFileSet <%s,%p> copy construction from <%s,%p>\n", GetName().c_str(), this, tfs.GetName().c_str(), &tfs);
#endif
   
   for (UnsignedInt i = 0; i < tfs.trackingConfigs.size(); ++i)
      trackingConfigs.push_back(tfs.trackingConfigs[i]);

   // Each tracking file set contains a set of filters in which they have their own thinning frequency counters.
   // Therefore, it needs to have seperate set of data filter objects for each tracking file set object. 
   // Note that: If 2 different tracking file set objects share the same set of data filter objects, values 
   // of frequency counters are mixed up by both tracking file set objects. 
   for (UnsignedInt i = 0; i < tfs.dataFilters.size(); ++i)
   {
      if (tfs.dataFilters[i])
         dataFilters.push_back(tfs.dataFilters[i]->Clone());
   }

   isInitialized = false;
}


//------------------------------------------------------------------------------
// TrackingFileSet& operator=(const TrackingFileSet& tfs)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param tfs The TrackingFileSet that provides parameter updates to this one
 *
 * @return This TrackingFileSet, configured to match tfs
 */
//------------------------------------------------------------------------------
TrackingFileSet& TrackingFileSet::operator=(const TrackingFileSet& tfs)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingFileSet <%s,%p> operator =  from <%s,%p>\n", GetName().c_str(), this, tfs.GetName().c_str(), &tfs);
#endif

   if (this != &tfs)
   {
      MeasurementModelBase::operator=(tfs);

      for (UnsignedInt i = 0; i < measurements.size(); ++i)
      {
         if (measurements[i])
            delete measurements[i];
      }
      measurements.clear();

      trackingConfigs.clear();

      for (UnsignedInt i = 0; i < tfs.trackingConfigs.size(); ++i)
         trackingConfigs.push_back(tfs.trackingConfigs[i]);

      filenames               = tfs.filenames;
      rampedTablenames        = tfs.rampedTablenames;
      useLighttime            = tfs.useLighttime;
      solarsystem             = tfs.solarsystem;
      thePropagator           = tfs.thePropagator;
      references              = tfs.references;
      datafiles               = tfs.datafiles;
      useRelativityCorrection = tfs.useRelativityCorrection;
      useETminusTAICorrection = tfs.useETminusTAICorrection;
      rangeModulo             = tfs.rangeModulo;
      dopplerCountInterval    = tfs.dopplerCountInterval;
      tdrsServiceAccessList   = tfs.tdrsServiceAccessList;
      tdrsNode4Frequency      = tfs.tdrsNode4Frequency;
      tdrsNode4Band           = tfs.tdrsNode4Band;
      tdrsSMARID              = tfs.tdrsSMARID;
      tdrsDataFlag            = tfs.tdrsDataFlag;
      dataFilterNames         = tfs.dataFilterNames;

      // Remove all dataFilters
      for(UnsignedInt i = 0; i < dataFilters.size(); ++i)
      {
         if (dataFilters[i])
            delete dataFilters[i];
      }
      dataFilters.clear();

      // Create clones of data filters
      for(UnsignedInt i = 0; i < tfs.dataFilters.size(); ++i)
      {
         if (tfs.dataFilters[i])
            dataFilters.push_back(tfs.dataFilters[i]->Clone());
      }

      isInitialized = false;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Makes a replica of this TrackingFileSet
 *
 * @return The replica
 */
//------------------------------------------------------------------------------
GmatBase* TrackingFileSet::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("TrackingFileSet<%s,%p>::Clone() \n", GetName().c_str(), this);
#endif

   return new TrackingFileSet(*this);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id ID for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetParameterText(const Integer id) const
{
   if (id >= MeasurementModelBaseParamCount && id < TrackingFileSetParamCount)
      return PARAMETER_TEXT[id - MeasurementModelBaseParamCount];
   return MeasurementModelBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter unit, given the input parameter ID.
 *
 * @param id ID for the requested parameter text.
 *
 * @return parameter unit for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetParameterUnit(const Integer id) const
{
   if (id == RANGE_MODULO)
      return "RU";
   if (id == DOPPLER_COUNT_INTERVAL)
      return "sec";

   return MeasurementModelBase::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
//  Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str String for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer TrackingFileSet::GetParameterID(const std::string& str) const
{
   for (Integer i = MeasurementModelBaseParamCount; i < TrackingFileSetParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - MeasurementModelBaseParamCount])
         return i;
   }

   return MeasurementModelBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType TrackingFileSet::GetParameterType(const Integer id) const
{
   if (id >= MeasurementModelBaseParamCount && id < TrackingFileSetParamCount)
      return PARAMETER_TYPE[id - MeasurementModelBaseParamCount];

   return MeasurementModelBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetParameterTypeString(const Integer id) const
{
   return PARAM_TYPE_STRING[GetParameterType(id)];
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
Integer TrackingFileSet::GetIntegerParameter(const Integer id) const
{
   if (id == TDRS_NODE4_BAND)
      return tdrsNode4Band;
   if (id == TDRS_SMAR_ID)
      return tdrsSMARID;
   if (id == TDRS_DATA_FLAG)
      return tdrsDataFlag;

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
Integer TrackingFileSet::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == TDRS_NODE4_BAND)
   {
      if (value < 0)
         throw MeasurementException("Error: Parameter "+GetName()+"."+GetParameterText(id)+" has invalid value. Its value has to be a non negative integer\n");

      tdrsNode4Band = value;
      return tdrsNode4Band;
   }

   if (id == TDRS_SMAR_ID)
   {
      if (value < 0)
         throw MeasurementException("Error: Parameter "+GetName()+"."+GetParameterText(id)+" has invalid value. Its value has to be a non negative integer\n");

      tdrsSMARID = value;
      return tdrsSMARID;
   }

   if (id == TDRS_DATA_FLAG)
   {
      if ((value != 0)&&(value != 1))
         throw MeasurementException("Error: Parameter "+GetName()+"."+GetParameterText(id)+" has invalid value. Its value has to be 0 or 1.\n");

      tdrsDataFlag = value;
      return tdrsSMARID;
   }

   return MeasurementModelBase::SetIntegerParameter(id, value);
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
Integer TrackingFileSet::GetIntegerParameter(const std::string &label) const
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
Integer TrackingFileSet::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameters used in the noise modeling for the Measurement
 *
 * @param id The ID for the parameter that is retrieved
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real TrackingFileSet::GetRealParameter(const Integer id) const
{
   if (id == RANGE_MODULO)
      return rangeModulo;
   if (id == DOPPLER_COUNT_INTERVAL)
      return dopplerCountInterval;
   if (id == TDRS_NODE4_FREQUENCY)
      return tdrsNode4Frequency;

   return MeasurementModelBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the parameters used in the noise modeling for the Measurement
 *
 * @param id The ID for the parameter that is to be set
 * @param value The new value for the parameter
 *
 * @return The parameter value.  The return value is the new value if it was
 *         changed, or the value prior to the call if the new value was not
 *         accepted.
 */
//------------------------------------------------------------------------------
Real TrackingFileSet::SetRealParameter(const Integer id, const Real value)
{
   if (id == RANGE_MODULO)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: "+GetName()+"."+GetParameterText(id)+" has an invalid value. It has to be a positive number\n");

      rangeModulo = value;
      return rangeModulo;
   }

   if (id == DOPPLER_COUNT_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: "+GetName()+"."+GetParameterText(id)+" has an invalid value. It has to be a positive number\n");

      dopplerCountInterval = value;
      return dopplerCountInterval;
   }

   if (id == TDRS_NODE4_FREQUENCY)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: "+GetName()+"."+GetParameterText(id)+" has an invalid value. It has to be a positive number\n");

      tdrsNode4Frequency = value;
      return tdrsNode4Frequency;
   }

   return MeasurementModelBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameters used in the noise modeling for the Measurement
 *
 * @param label The text label for the parameter that is retrieved
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real TrackingFileSet::GetRealParameter(const std::string & label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string & label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the parameters used in the noise modeling for the Measurement
 *
 * @param label The text label for the parameter that is to be set
 * @param value The new value for the parameter
 *
 * @return The parameter value.  The return value is the new value if it was
 *         changed, or the value prior to the call if the new value wwas not
 *         accepted.
 */
//------------------------------------------------------------------------------
Real TrackingFileSet::SetRealParameter(const std::string & label,
      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return The string.
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetStringParameter(const Integer id) const
{
   return MeasurementModelBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter value
 *
 * @param id The ID of the parameter
 * @param value The value
 *
 * @return true is the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetStringParameter(const Integer id,
      const std::string& value)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("TrackingFileSet<%s,%p>::SetStringParameter(id = %d, "
            "value = '%s') called\n", GetName().c_str(), this, id, value.c_str());
   #endif

   if (id == TRACKINGCONFIG)
   {
      throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".AddTrackingConfig parameter has a syntax error.\n");
   }

   if (id == FILENAME)
   {
      // if it is an empty list, then clear file name list
      if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         filenames.clear();
         return true;
      }

      // check for a valid file name
      Integer err = 0;
      if (!GmatStringUtil::IsValidFullFileName(value, err))
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".FileName parameter is an invalid file name.\n");

      // add name to the list
      if (find(filenames.begin(), filenames.end(), value) == filenames.end())
      {
         filenames.push_back(value);
         return true;
      }
      else
         throw MeasurementException("Error: File name is replicated ('" + value + "')\n");
   }

   if (id == RAMPED_TABLENAME)
   {
      // if it is an empty list, then clear ramp table name list
      if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         rampedTablenames.clear();
         return true;
      }

      // check for a valid file name
      Integer err = 0;
      if (!GmatStringUtil::IsValidFullFileName(value, err))
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".RampTable parameter is an invalid file name.\n");

      // add name to the list
      if (find(rampedTablenames.begin(), rampedTablenames.end(), value) == rampedTablenames.end())
      {
         rampedTablenames.push_back(value);
         return true;
      }
      else
         throw MeasurementException("Error: ramp table name is replicated ('" + value + "')\n");

   }

   if (id == DATA_FILTERS)
   {
      // if it is an empty list, then set clear the list
      if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         dataFilterNames.clear();
         return true;
      }
      else if (!GmatStringUtil::IsValidIdentity(value))
         throw MeasurementException("Error: '" + value + " set to " + GetName() + ".DataFilters parameter is an invalid object name.\n");

      if (find(dataFilterNames.begin(), dataFilterNames.end(), value) == dataFilterNames.end())
      {
         dataFilterNames.push_back(value);
         return true;
      }
      else
         throw MeasurementException("Error: name of data filter is replicated ('" + value + "')\n");      
   }

   return MeasurementModelBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter value from a StringArray.
 *
 * @param id ID for the requested parameter
 * @param index The index into the array
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == TRACKINGCONFIG)
   {
      if (((Integer)trackingConfigs.size() > index) && (index >= 0))
         return trackingConfigs[index].GetDefinitionString();
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a tracking data configuration");
   }

   if (id == FILENAME)
   {
      if (((Integer)filenames.size() > index) && (index >= 0))
         return filenames[index];
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a tracking data file name");
   }

   if (id == RAMPED_TABLENAME)
   {
      if (((Integer)rampedTablenames.size() > index) && (index >= 0))
         return rampedTablenames[index];
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a ramp table file name");
   }

   if (id == DATA_FILTERS)
   {
      if (((Integer)dataFilterNames.size() > index) && (index >= 0))
         return dataFilterNames[index];
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a data filter name");
   }

   if (id == TDRS_SERVICE_ACCESS)
   {
      if (((Integer)tdrsServiceAccessList.size() > index) && (index >= 0))
         return tdrsServiceAccessList[index];
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a service access list");
   }

   return MeasurementModelBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter value in a StringArray.
 *
 * @param id ID for the requested parameter
 * @param value The new value
 * @param index The index into the array
 *
 * @return The string.
 */
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/**
* This function is used to add a particpant name to the list of signal path's
* particicpants list.
*
* @param participantName      Name of a partcipant along the signal path
*
* @return                     true if a valid participant name, false otherwise
*/
//------------------------------------------------------------------------------
bool TrackingFileSet::AddToSignalPath(std::string participantName, Integer configIndex, Integer strandIndex)
{
   // validate participant name
   if (GmatStringUtil::IsValidIdentity(participantName) == false)
      throw MeasurementException("Error: Invalid participant name '" + participantName + "' was set to " + GetName() + ".AddTrackingConfig parameter.\n");
   
   if (strandIndex > 0)
      throw MeasurementException("Error: Syntax error when setting value to " + GetName() + ".AddTrackingConfig parameter. In current version, GMAT does not allow two or more participant lists in tracking configuration.\n");

   trackingConfigs[configIndex].strands[strandIndex].push_back(participantName);

   return true;
}


//------------------------------------------------------------------------------
// bool ParseTrackingConfig(const std::string value, Integer configIndex)
//------------------------------------------------------------------------------
/**
* This function is used to parse a strand {part1,...,partn} or a tracking config 
* {{part1,..., partn}, datatype1, ..., datatypem}
*
* @param value          a string containing a strand or a tracking config
* @param configIndex    index of this tracking config
* @param start          flag indicates the start of a configuration
*
* @return               true if no syntax error, false otherwise
*/
//------------------------------------------------------------------------------
bool TrackingFileSet::ParseTrackingConfig(std::string value, Integer& configIndex, bool& start)
{
   // case 1: parse a strand {part1, ..., partn}
   // case 2: parse a tracking config syntax: {{part1, ..., partn}, ..., {part1, ..., partn}, datatype1, ..., datatypem}

   //MessageInterface::ShowMessage("TrackingFileSet::ParseTrackingConfig('%s', %d)\n", value.c_str(), configIndex);

   bool retval = true;

   // Get all known measurement types
   StringArray knownTypes;                                                                     // GMT-5701
   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();                                     // GMT-5701
   //if ((runmode != GmatGlobal::TESTING) && (runmode != GmatGlobal::TESTING_NO_PLOTS))        // GMT-5701
   if (runmode != GmatGlobal::TESTING)                                                         // GMT-5701
   {                                                                                           // GMT-5701
      knownTypes.push_back("DSNRange");                                                        // GMT-5701
      knownTypes.push_back("Doppler");                                                         // GMT-5701
   }                                                                                           // GMT-5701
   else                                                                                        // GMT-5701
   {                                                                                           // GMT-5701
      TFSMagicNumbers *mn = TFSMagicNumbers::Instance();                                       // GMT-5701
      knownTypes = mn->GetKnownTypes();                                                        // GMT-5701
   }                                                                                           // GMT-5701

   // Remove opened and closed curly brackets at the begin and end of string
   std::string value1 = GmatStringUtil::Trim(value.substr(1, value.size() - 2));

   // Specify the syntax is a strand or a tracking config
   if (value1.at(0) == '{')
   {
      // case 2: parse a tracking config syntax: {part1, ..., partn}, ..., {part1, ..., partn}, datatype1, ..., datatypem

      std::string::size_type pos;
      // 1. Parse tracking config and get a list of strands
      while (value1.at(0) == '{')
      {
         pos = value1.find_first_of('}');
         if (pos == value1.npos)
            throw MeasurementException("Error: strand '" + value1 + "' missed a closed curly bracket.\n");
         else
         {
            // Get list of strands
            std::string strand = value1.substr(0, pos + 1);

            //MessageInterface::ShowMessage("case 2: strand = <%s>\n", strand.c_str());
            // Add a slot to strands array in order to store a new strand 
            StringArray trackList;
            trackingConfigs[configIndex].strands.push_back(trackList);
            Integer strandIndex = trackingConfigs[configIndex].strands.size() - 1;

            // Parse the strand
            retval = ParseStrand(strand, configIndex, strandIndex);
            if (retval == false)
               return retval;

            // Get the remain unparsed string
            value1 = GmatStringUtil::Trim(value1.substr(pos+1));
            if (value1.at(0) != ',')
               throw MeasurementException("Error: syntax error - missing ',' after '" + strand + "'.\n");
            value1 = GmatStringUtil::Trim(value1.substr(1));
         }
      }

      // 2. Parse tracking config and get a list of data types: datatype1, ..., datatypem
      //MessageInterface::ShowMessage("case 2: data types = <%s>\n", value1.c_str());

      std::string datatype;
      pos = value1.find_first_of(',');
      while (pos != value1.npos)
      {
         // Get data type
         datatype = value1.substr(0, pos);

         // Validate and add data type to data type array
         if (find(knownTypes.begin(), knownTypes.end(), datatype) == knownTypes.end())
         {
            std::stringstream ss;
            ss << "Error: In current version, GMAT does not have measurement type '" + datatype + "'.\n This is a list of all available types:\n";
            for (Integer i = 0; i < knownTypes.size(); ++i)
            {
               if (i < knownTypes.size() - 1)
                  ss << " " << knownTypes[i] << ",";
               else
                  ss << " " << knownTypes[i] << ".\n";
            }
            throw MeasurementException(ss.str());
         }
         else
         {
            if ((!trackingConfigs[configIndex].types.empty()) &&
               (find(trackingConfigs[configIndex].types.begin(), trackingConfigs[configIndex].types.end(), datatype) != trackingConfigs[configIndex].types.end()))
               throw MeasurementException("Error: Duplicate measurement type in tracking configuration in " + GetName() + ".AddTrackingConfig parameter.\n");
            else
            {
               trackingConfigs[configIndex].types.push_back(datatype);
            }
         }

         // Get the remain of unparsed string
         value1 = GmatStringUtil::Trim(value1.substr(pos + 1));
         pos = value1.find_first_of(',');
      }

      // Add the last data type to data type array
      datatype = value1;
      if (find(knownTypes.begin(), knownTypes.end(), datatype) == knownTypes.end())
      {
         std::stringstream ss;
         ss << "Error: In current version, GMAT does not have measurement type '" + datatype + "'.\n This is a list of all available types:\n";
         for (Integer i = 0; i < knownTypes.size(); ++i)
         {
            if (i < knownTypes.size() - 1)
               ss << " " << knownTypes[i] << ",";
            else
               ss << " " << knownTypes[i] << ".\n";
         }
         throw MeasurementException(ss.str());
      }
      else
      {
         if ((!trackingConfigs[configIndex].types.empty()) &&
            (find(trackingConfigs[configIndex].types.begin(), trackingConfigs[configIndex].types.end(), datatype) != trackingConfigs[configIndex].types.end()))
            throw MeasurementException("Error: Duplicate measurement type in tracking configuration in " + GetName() + ".AddTrackingConfig parameter.\n");
         else
         {
            trackingConfigs[configIndex].types.push_back(datatype);
         }
      }

      // start a new tracking config
      start = true;
   }
   else
   {
      //case 1: parse a strand syntax: part1, ..., partn
      // Add a slot to strands array in order to store a new strand 
      StringArray trackList;
      trackingConfigs[configIndex].strands.push_back(trackList);
      Integer strandIndex = trackingConfigs[configIndex].strands.size() - 1;

      // Parse the strand
      return ParseStrand(value, configIndex, strandIndex);
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool ParsingStrand(const std::string value)
//------------------------------------------------------------------------------
/**
* This function is used to parse strand with syntax {part1,..., partn}
*
* @param value    a string containing a list of partcipants a long signal path
*
* @return         true if no syntax error, false otherwise
*/
//------------------------------------------------------------------------------
bool TrackingFileSet::ParseStrand(std::string value, Integer configIndex, Integer strandIndex)
{
   // Remove open and close curly brackets at the begin and end of string
   value = value.substr(1, value.size() - 2);

   // processing each participant in participants list
   std::string participantName;
   size_t pos = value.find_first_of(',');
   while (pos != std::string::npos)
   {
      participantName = GmatStringUtil::Trim(value.substr(0, pos));
      if (AddToSignalPath(participantName, configIndex, strandIndex) == false)
         return false;

      value = value.substr(pos + 1);
      pos = value.find_first_of(',');
   }

   participantName = GmatStringUtil::Trim(value);
   return AddToSignalPath(participantName, configIndex, strandIndex);
}


bool TrackingFileSet::SetStringParameter(const Integer id,
      const std::string& value1, const Integer index)
{
   std::string value = value1;
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("TrackingFileSet<%s,%p>::SetStringParameter(%d, "
            "'%s', %d) called\n", GetName().c_str(), this, id, value.c_str(), index);
   #endif

   if (id == TRACKINGCONFIG)
   {
      // Nothing was set to tracking configuration when an empty list was set to AddTrackingConfig parameter
      if (index == -1)
         return true;

      if (value == "")
         throw MeasurementException("Error:: Syntax error when GMAT sets value to " + GetName() + ".AddTrackingConfig parameter.\n");


      // Get all known measurement types
      StringArray knownTypes;                                                                     // GMT-5701
      Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();                                     // GMT-5701
      //if ((runmode != GmatGlobal::TESTING) && (runmode != GmatGlobal::TESTING_NO_PLOTS))        // GMT-5701
      if (runmode != GmatGlobal::TESTING)                                                         // GMT-5701
      {                                                                                           // GMT-5701
         knownTypes.push_back("DSNRange");                                                        // GMT-5701
         knownTypes.push_back("Doppler");                                                         // GMT-5701
      }                                                                                           // GMT-5701
      else                                                                                        // GMT-5701
      {                                                                                           // GMT-5701
         TFSMagicNumbers *mn = TFSMagicNumbers::Instance();                                       // GMT-5701
         knownTypes = mn->GetKnownTypes();                                                        // GMT-5701
      }                                                                                           // GMT-5701


      // Create a new tracking config
      if (index == 0)                    // it starts a tracking configuration when index = 0
      {
         // reset open bracket count and new strand flag
         openBracketCount = 0;
         start = true;
      }

      if (start)
      {
         // Starting a new definition
         MeasurementDefinition newDef;
         trackingConfigs.push_back(newDef);
         start = false;
      }
      Integer defIndex = trackingConfigs.size() - 1;

      if ((value.size() > 1) && (value.c_str()[0] == '{') && (value.c_str()[value.size() - 1] == '}'))
      {
         // parsing syntax for: 
         // case1: {part1,..., partn}
         // case2: {{part1,..., partn}, datatype1,...,datatypem}
         if (openBracketCount != 0)
            throw MeasurementException("Error:: Syntax error when GMAT sets value to " + GetName() + ".AddTrackingConfig parameter.\n");

         return ParseTrackingConfig(value, defIndex, start);
      }
      else
      {
         // parsing the follwoing cases:
         // case 3: '{<part>'
         // case 4: '<part>}'
         // case 5: '<part>'
         // case 6: '<typename>'

         std::string rawName = GmatStringUtil::Trim(value);
         if (rawName.size() == 0)
            throw MeasurementException("Error:: Syntax error when GMAT sets value to " + GetName() + ".AddTrackingConfig parameter.\n");

         // case 3: '{<part>'
         if (rawName.at(0) == '{')
         {
            if (openBracketCount != 0)
               throw MeasurementException("Error:: Syntax error when GMAT sets value to " + GetName() + ".AddTrackingConfig parameter.\n");

            ++openBracketCount;
            rawName = GmatStringUtil::Trim(rawName.substr(1));                          // trim open curly bracket and all white space
            
            StringArray trackList;
            trackingConfigs[defIndex].strands.push_back(trackList);
            Integer strandIndex = trackingConfigs[defIndex].strands.size() - 1;
            AddToSignalPath(rawName, defIndex, strandIndex);

            return true;
         }


         // case 4: '<part>}'
         if (rawName.at(rawName.size() - 1) == '}')
         {
            if (openBracketCount == 0)
               throw MeasurementException("Error:: Syntax error when GMAT sets value to " + GetName() + ".AddTrackingConfig parameter.\n");

            --openBracketCount;
            rawName = GmatStringUtil::Trim(rawName.substr(0, rawName.size() - 1));
            Integer strandIndex = trackingConfigs[defIndex].strands.size() - 1;
            AddToSignalPath(rawName, defIndex, strandIndex);

            return true;
         }

         // case 5: '<part>'
         if (openBracketCount > 0)
         {
            Integer strandIndex = trackingConfigs[defIndex].strands.size() - 1;
            AddToSignalPath(rawName, defIndex, strandIndex);
            
            return true;
         }

         // case 6:
         if (find(knownTypes.begin(), knownTypes.end(), rawName) == knownTypes.end())
         {
            std::stringstream ss;
            ss << "Error: In current version, GMAT does not have measurement type '" + rawName + "'.\n This is a list of all available types:\n";
            for (Integer i = 0; i < knownTypes.size(); ++i)
            {
               if (i < knownTypes.size() - 1)
                  ss << " " << knownTypes[i] << ",";
               else
                  ss << " " << knownTypes[i] << ".\n";
            }
            throw MeasurementException(ss.str());
         }
         else
         {
            if ((!trackingConfigs[defIndex].types.empty()) &&
                (find(trackingConfigs[defIndex].types.begin(), trackingConfigs[defIndex].types.end(), rawName) != trackingConfigs[defIndex].types.end()))
               throw MeasurementException("Error: Duplicate measurement type in tracking configuration in " + GetName() + ".AddTrackingConfig parameter.\n");
            else
            {
               trackingConfigs[defIndex].types.push_back(rawName);
               return true;
            }
         }
      }


      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%d members in config:\n",
               trackingConfigs.size());
         for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
            MessageInterface::ShowMessage("   %s\n", trackingConfigs[i].GetDefinitionString().c_str());
      #endif
      return true;
   }

   if (id == FILENAME)
   {
      // throw an error message when it is an empty list
      if (index == -1)
         throw MeasurementException("Error: No file name was set to " + GetName() + ".Filenames parameter.\n");

      if ((!filenames.empty())&&
          (find (filenames.begin(), filenames.end(), value) != filenames.end()))
         throw MeasurementException("Error: replication of file name ('" + value + "').\n");

      if (((Integer)filenames.size() > index) && (index >= 0))
         filenames[index] = value;
      else if ((Integer)filenames.size() == index)
         filenames.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
               "set a tracking data file name");

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%d members in config:\n",
               filenames.size());
         for (UnsignedInt i = 0; i < filenames.size(); ++i)
            MessageInterface::ShowMessage("   %s\n", filenames[i].c_str());
      #endif
      return true;
   }

   if (id == RAMPED_TABLENAME)
   {
      // an empty list is set to RampTables parameter when index == -1
      if (index == -1)
      {
         rampedTablenames.clear();
         return true;
      }

      if ((!rampedTablenames.empty())&&
          (find(rampedTablenames.begin(), rampedTablenames.end(), value) != rampedTablenames.end()))
         throw MeasurementException("Error: replication of ramp table name ('" + value + "').\n");

      if (((Integer)rampedTablenames.size() > index) && (index >= 0))
         rampedTablenames[index] = value;
      else if ((Integer)rampedTablenames.size() == index)
         rampedTablenames.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
               "set a ramp table file name");

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%d members in config:\n",
               rampedTablenames.size());
         for (UnsignedInt i = 0; i < rampedTablenames.size(); ++i)
            MessageInterface::ShowMessage("   %s\n", rampedTablenames[i].c_str());
      #endif
      return true;
   }

   if (id == DATA_FILTERS)
   {
      // return true when it is an empty list
      if (index == -1)
      {
         dataFilterNames.clear();
         return true;
      }

      if ((!dataFilterNames.empty())&&
          (find(dataFilterNames.begin(), dataFilterNames.end(), value) != dataFilterNames.end()))
         throw MeasurementException("Error: replication of data filter name ('" + value + "').\n");
      
      if (((Integer)dataFilterNames.size() > index) && (index >= 0))
         dataFilterNames[index] = value;
      else if ((Integer)dataFilterNames.size() == index)
         dataFilterNames.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
               "set a data filter name");

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%d members in DataFilters:\n",
               dataFilterNames.size());
         for (UnsignedInt i = 0; i < dataFilterNames.size(); ++i)
            MessageInterface::ShowMessage("   %s\n", dataFilterNames[i].c_str());
      #endif
      return true;
   }

   if (id == TDRS_SERVICE_ACCESS)
   {
      // an empty list is set to TDARSServiceAccess parameter when index == -1
      if (index == -1)
      {
         tdrsServiceAccessList.clear();
         return true;
      }

      if ((!tdrsServiceAccessList.empty())&&
          (find(tdrsServiceAccessList.begin(), tdrsServiceAccessList.end(), value) != tdrsServiceAccessList.end()))
         throw MeasurementException("Error: replication of service access name ('" + value + "').\n");
      
      if (((Integer)tdrsServiceAccessList.size() > index) && (index >= 0))
         tdrsServiceAccessList[index] = value;
      else if ((Integer)tdrsServiceAccessList.size() == index)
         tdrsServiceAccessList.push_back(value);
      else
         throw MeasurementException("Index out of bounds when trying to "
               "set a service access name");

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("%d names in TDRS service access list:\n",
               tdrsServiceAccessList.size());
         for (UnsignedInt i = 0; i < tdrsServiceAccessList.size(); ++i)
            MessageInterface::ShowMessage("   %s\n", tdrsServiceAccessList[i].c_str());
      #endif
      return true;
   }

   return MeasurementModelBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array
 *
 * @param id The ID of the array
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetStringArrayParameter(
      const Integer id) const
{
#ifdef DEBUG_GET_PARAMETER
   MessageInterface::ShowMessage("TrackingFileSet::GetStringArrayParameter(id = %d).\n", id);
#endif

   static StringArray tconfigs;

   if (id == TRACKINGCONFIG)
   {
      tconfigs.clear();
      for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
         tconfigs.push_back(trackingConfigs[i].GetDefinitionString());
      return tconfigs;
   }

   if (id == FILENAME)
      return filenames;

   if (id == RAMPED_TABLENAME)
      return rampedTablenames;

   if (id == TDRS_SERVICE_ACCESS)
      return tdrsServiceAccessList;

   if (id == DATA_FILTERS)
      return dataFilterNames;
   
   return MeasurementModelBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array from a vector of string arrays
 *
 * @param id The ID for the string array vector
 * @param index The string array in the vector that is wanted
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   /// todo: Build decomposed reference stuff if we need external access to it
   if (id == TRACKINGCONFIG)
      if ((index >= 0) && (index < (Integer)trackingConfigs.size()))
         return trackingConfigs[index].types;

   return MeasurementModelBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter value, given the input parameter ID.
 *
 * @param label Scripted string for the requested parameter.
 *
 * @return The string.
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetStringParameter(const std::string& label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter value
 *
 * @param label Scripted string for the requested parameter.
 * @param value The value
 *
 * @return true is the parameter was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetStringParameter(const std::string& label,
      const std::string& value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string& label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter value from a StringArray.
 *
 * @param label Scripted string for the requested parameter.
 * @param index The index into the array
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetStringParameter(const std::string& label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string& label, const std::string& value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string parameter value in a StringArray.
 *
 * @param label Scripted string for the requested parameter.
 * @param value The new value
 * @param index The index into the array
 *
 * @return The string.
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetStringParameter(const std::string& label,
      const std::string& value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array
 *
 * @param label Scripted string for the requested parameter.
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetStringArrayParameter(
      const std::string& label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string array from a vector of string arrays
 *
 * @param label Scripted string for the requested parameter.
 * @param index The string array in the vector that is wanted
 *
 * @return The string array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetStringArrayParameter(
      const std::string& label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::GetBooleanParameter(const Integer id) const
{
   if (id == USE_RELATIVITY)
      return useRelativityCorrection;

   if (id == USE_ETMINUSTAI)
      return useETminusTAICorrection;

   if (id == USELIGHTTIME)
      return useLighttime;

   return MeasurementModelBase::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter
 *
 * @param id ID for the parameter
 * @param value The new value
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == USE_RELATIVITY)
   {
      useRelativityCorrection = value;
      return useRelativityCorrection;
   }

   if (id == USE_ETMINUSTAI)
   {
      useETminusTAICorrection = value;
      return useETminusTAICorrection;
   }

   if (id == USELIGHTTIME)
   {
      useLighttime = value;
      return useLighttime;
   }

   return MeasurementModelBase::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter from an array of Boolean values
 *
 * @param id The ID for the parameter
 * @param index The index in the Boolean array
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::GetBooleanParameter(const Integer id,
      const Integer index) const
{
   return MeasurementModelBase::GetBooleanParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter from an array of Boolean values
 *
 * @param id The ID for the parameter
 * @param value The new value
 * @param index The index in the Boolean array
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetBooleanParameter(const Integer id, const bool value,
      const Integer index)
{
   return MeasurementModelBase::SetBooleanParameter(id, value, index);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter from an array of Boolean values
 *
 * @param label Script label for the parameter
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::GetBooleanParameter(const std::string& label) const
{
   return GetBooleanParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string& label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter
 *
 * @param label Script label for the parameter
 * @param value The new value
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetBooleanParameter(const std::string& label,
      const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string& label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a Boolean parameter from an array of Boolean values
 *
 * @param label Script label for the parameter
 * @param index The index in the Boolean array
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::GetBooleanParameter(const std::string& label,
      const Integer index) const
{
   return GetBooleanParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string& label, const bool value,
//      const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a Boolean parameter from an array of Boolean values
 *
 * @param label Script label for the parameter
 * @param value The new value
 * @param index The index in the Boolean array
 *
 * @return The parameter setting
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetBooleanParameter(const std::string& label,
      const bool value, const Integer index)
{
   return SetBooleanParameter(GetParameterID(label), value, index);
}


//---------------------------------------------------------------------------
// Gmat::ObjectType GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param id ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
Gmat::ObjectType TrackingFileSet::GetPropertyObjectType(const Integer id) const
{
   if (id == DATA_FILTERS)
      return Gmat::DATA_FILTER;

   return MeasurementModelBase::GetPropertyObjectType(id);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of a single reference object
 *
 * @param type The type of the desired object
 *
 * @return The name of the object
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::GetRefObjectName(const Gmat::ObjectType type) const
{
   return MeasurementModelBase::GetRefObjectName(type);
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves a list of types the object expects
 *
 * @return The list of types
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& TrackingFileSet::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   /// @todo: remove when they are autogenerated
   refObjectTypes.push_back(Gmat::DATA_FILE);
   refObjectTypes.push_back(Gmat::DATA_FILTER);

   return refObjectTypes;
}

//------------------------------------------------------------------------------
// const StringArray& TrackingFileSet::GetRefObjectNameArray(
//       const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves the names of reference objects
 *
 * @param type The object type, or Gmat::UNKNOWN_OBJECT for all ref objects
 *
 * @return The list of reference object names
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Called TrackingFileSet<%s,%p>::"
            "GetRefObjectNameArray(%d)\n", GetName().c_str(), this, type);
   #endif

   refObjectNames.clear();

   // @todo Make more robust by dropping the types rather than assuming only
   //       one at the end.  Basically, get the bracket parsing working
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::SPACE_POINT))
   {
      for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
         for (UnsignedInt j = 0; j < trackingConfigs[i].strands.size(); ++j)
            for (UnsignedInt k=0; k < trackingConfigs[i].strands[j].size(); ++k)
         {
            if (find(refObjectNames.begin(), refObjectNames.end(),
                  trackingConfigs[i].strands[j][k]) == refObjectNames.end())
               refObjectNames.push_back(trackingConfigs[i].strands[j][k]);
         }
   }

   // @todo This loop statement is temporary code.  Remove when datafile objects
   // are autogenerated.  The current code sets DataFile object names in the
   // filenames array
   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::DATA_FILE))
   {
      for (UnsignedInt i = 0; i <  filenames.size(); ++i)
         refObjectNames.push_back(filenames[i]);
   }

   if ((type == Gmat::UNKNOWN_OBJECT) || (type == Gmat::DATA_FILTER))
   {
      for (UnsignedInt i = 0; i <  dataFilterNames.size(); ++i)
         refObjectNames.push_back(dataFilterNames[i]);
   }

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Reference objects:  ");
      for (UnsignedInt i = 0; i < refObjectNames.size(); ++i)
         MessageInterface::ShowMessage("'%s'  ", refObjectNames[i].c_str());
      MessageInterface::ShowMessage("\n");
   #endif

   return refObjectNames;
}

//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets names for reference objects
 *
 * @param type The type of object being named
 * @param name The object name
 *
 * @return true if set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetRefObjectName(const Gmat::ObjectType type,
      const std::string& name)
{
   bool retval = false;
   return (retval ? retval:MeasurementModelBase::SetRefObjectName(type, name));
}

//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string& oldName,
//      const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Renames object references based on a rename in the user interface
 *
 * @param type The type of the object that is renamed
 * @param oldName The old name of the object
 * @param newName The new name
 *
 * @return true if a name was changed, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Called TrackingFileSet<%s,%p>::"
            "RenameRefObject(type = %d, oldName = '%s', newName = '%s')\n", GetName().c_str(), this, type, oldName.c_str(), newName.c_str());
   #endif

   if ((type == Gmat::DATA_FILE) || (type == Gmat::UNKNOWN_OBJECT))
   {
      for (UnsignedInt i = 0; i < filenames.size(); ++i)
      {
         if (filenames[i] == oldName)
         {
            filenames[i] = newName;
            return true;
         }
      }
   }

   if ((type == Gmat::SPACE_POINT) || (type == Gmat::UNKNOWN_OBJECT))
   {
      for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
      {
         for (UnsignedInt j = 0; j < trackingConfigs[i].strands.size(); ++j)
         {
            for (UnsignedInt k = 0; k < trackingConfigs[i].strands[j].size(); ++k)
            {
               if (trackingConfigs[i].strands[j][k] == oldName)
               {
                  trackingConfigs[i].strands[j][k] = newName;
                  return true;
               }
            }
         }
      }
   }

   if ((type == Gmat::DATA_FILTER) || (type == Gmat::UNKNOWN_OBJECT))
   {
      for (UnsignedInt i = 0; i < dataFilterNames.size(); ++i)
      {
         if (dataFilterNames[i] == oldName)
         {
            dataFilterNames[i] = newName;
            return true;
         }
      }
   }

   return MeasurementModelBase::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string& name)
//------------------------------------------------------------------------------
/**
 * Retrieves a reference object, given its type and name
 *
 * @param type The object type
 * @param name The object name
 *
 * @return The object pointer, or NULL if it is not set
 */
//------------------------------------------------------------------------------
GmatBase* TrackingFileSet::GetRefObject(const Gmat::ObjectType type,
      const std::string& name)
{
   if ((type == Gmat::DATA_FILTER)||(type == Gmat::UNKNOWN_OBJECT))
   {
      for(UnsignedInt i = 0; i < dataFilters.size(); ++i)
      {
         if (dataFilters[i]->GetName() == name)
            return dataFilters[i];
      }
   }

   return MeasurementModelBase::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string& name,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves a reference object from an array of objects
 *
 * @param type The object type
 * @param name The object name
 * @param index The index of the object in the object array
 *
 * @return The object pointer, or NULL if it is not set
 */
//------------------------------------------------------------------------------
GmatBase* TrackingFileSet::GetRefObject(const Gmat::ObjectType type,
      const std::string& name, const Integer index)
{
   if (type == Gmat::SPACE_POINT)
   {
      if ((index < (Integer)references.size()) && (index >= 0))
         if (references[index]->GetName() == name)
            return references[index];
   }

   return MeasurementModelBase::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets a reference object pointer
 *
 * @param obj The object
 * @param type The type of the object
 * @param name The name of the object
 *
 * @return true if a pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
      const std::string& name)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Called TrackingFileSet<%s,%p>::"
            "SetRefObject(obj = %p, type = %d, name = '%s')\n", GetName().c_str(), this, obj, type, name.c_str());
   #endif


   bool retval = false;

   if (obj->IsOfType(Gmat::DATA_FILE))
   {
      if (find(datafiles.begin(), datafiles.end(), obj) == datafiles.end())
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Adding datafile %s\n", name.c_str());
         #endif
         datafiles.push_back((DataFile*)obj);
         return true;
      }
   }
   else if (obj->IsOfType(Gmat::DATA_FILTER))
   {
      bool found = false;
      for(UnsignedInt i = 0; i < dataFilters.size(); ++i)
      {
         if (obj->GetName() == dataFilters[i]->GetName())
         {
            found = true;
            break;
         }
      }

      if (!found)
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Adding data filter %s\n", name.c_str());
         #endif
            dataFilters.push_back(obj->Clone());                                     // It needs to clone in order to avoid error when deleting dataFilters in destructor
            return true;
      }
   }
   else
   {
      if (find(references.begin(), references.end(), obj) == references.end())
      {
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("Adding node %s\n", name.c_str());
         #endif
         references.push_back(obj);
         return true;
      }
   }

   return (retval || MeasurementModelBase::SetRefObject(obj, type, name));
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//       const std::string& name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a reference object pointer in an object array
 *
 * @param obj The object
 * @param type The type of the object
 * @param name The name of the object
 * @param index The array index for the object
 *
 * @return true if a pointer was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
      const std::string& name, const Integer index)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Called TrackingFileSet<%s,%p>::"
            "SetRefObject(obj = %p, type = %d, name = '%s', index = %d)\n", GetName().c_str(), this, obj, type, name.c_str(), index);
   #endif

   bool retval = false;

   if (find(references.begin(), references.end(), obj) == references.end())
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Adding node %s\n", name.c_str());
      #endif
      references.push_back(obj);
      retval = true;
   }

   return (retval || MeasurementModelBase::SetRefObject(obj,type,name,index));
}

//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves an array of reference objects
 *
 * @param type The type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray& TrackingFileSet::GetRefObjectArray(const Gmat::ObjectType type)
{
   static ObjectArray objectList;
   objectList.clear();

   if ((type == Gmat::DATA_FILTER)||(type == Gmat::UNKNOWN_OBJECT))
   {
      for(UnsignedInt i = 0; i < dataFilters.size(); ++i)
         objectList.push_back(dataFilters[i]);
      return objectList;
   }

   return MeasurementModelBase::GetRefObjectArray(type);
}

//------------------------------------------------------------------------------
// ObjectArray& GetRefObjectArray(const std::string& typeString)
//------------------------------------------------------------------------------
/**
 * Retrieves an array of reference objects
 *
 * @param type The label for the type of object requested
 *
 * @return The array of objects
 */
//------------------------------------------------------------------------------
ObjectArray& TrackingFileSet::GetRefObjectArray(const std::string& typeString)
{
   return GetRefObjectArray(GetObjectType(typeString));
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem* ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system for the measurements
 *
 * @param ss The solar system
 */
//------------------------------------------------------------------------------
void TrackingFileSet::SetSolarSystem(SolarSystem* ss)
{
   solarsystem = ss;
}


//------------------------------------------------------------------------------
// void SetPropagator(PropSetup* ps)
//------------------------------------------------------------------------------
/**
 * Sets the propagator pointer used for light time computations
 *
 * @param ps The propagator
 *
 * @todo: Generalize for multiple propagators once that support is implemented
 */
//------------------------------------------------------------------------------
void TrackingFileSet::SetPropagator(PropSetup* ps)
{
   MessageInterface::ShowMessage("Setting propagator in the tracking file set "
         "%s to <%p>\n", instanceName.c_str(), ps);
   thePropagator = ps;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the tracking file set for use
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TrackingFileSet::Initialize()
{
   #ifdef DEBUG_INITIALIZE
      MessageInterface::ShowMessage("Entered TrackingFileSet::Initialize() <%s,%p>\n", GetName().c_str(), this);
   #endif
   
   if (this->IsInitialized())
      return true;

   bool retval = false;
   if (MeasurementModelBase::Initialize())
   {
      // Count the adapters needed
      TFSMagicNumbers *mn = TFSMagicNumbers::Instance();
      StringArray knownTypes = mn->GetKnownTypes();

      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("Tracking Configuration:\n");
         for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
         {
            MessageInterface::ShowMessage("  %3d: ", i);
            MessageInterface::ShowMessage("%s\n",
                  trackingConfigs[i].GetDefinitionString().c_str());
         }
      #endif

      // Check for observation data file 
      if (filenames.size() == 0)
         throw MeasurementException("No observation data file is set to " + GetName() + " object.\n");

      // Note: No ramp table is accepted when frequency is got from transmiter
//      // Check for ramp table 
//      if (rampedTablenames.size() == 0)
//         throw MeasurementException("No ramp table is set to " + GetName() + " object.\n");

      // Build the adapters
      //if (trackingConfigs.size() == 0)
      //   throw MeasurementException("No TrackingConfig is defined for " + GetName() + " object.\n");

      for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
      {
         // Check to make sure only one strand defined in TrackingConfig
         if (trackingConfigs[i].strands.size() != 1)
            throw MeasurementException("Multiple strands and empty strands "
                     "are not yet implemented");
         // Check to make sure at least one measurement type defined in TrackingConfig
         if (trackingConfigs[i].types.size() == 0)
            throw MeasurementException("No measurement type is defined in " + GetName() + ".AddTrackingConfig.\n");

         for (UnsignedInt j = 0; j < trackingConfigs[i].types.size(); ++j)
         {
            TrackingDataAdapter *tda =
                  BuildAdapter(trackingConfigs[i].strands[0],
                        trackingConfigs[i].types[j], i);
            
            if (tda == NULL)
            {
               throw MeasurementException("Unable to build the " +
                     trackingConfigs[i].types[j] + " measurement");
            }

            // This section was done in BuildAdapter
            //// Pass in the signal paths
            //std::string theStrand;
            //for (UnsignedInt k=0; k < trackingConfigs[i].strands[0].size(); ++k)
            //{
            //   if (k > 0)
            //      theStrand += ", ";
            //   theStrand += trackingConfigs[i].strands[0][k];
            //}

            //tda->SetStringParameter("SignalPath", theStrand);

            // Push tracking data apadter to measurement model list
            measurements.push_back(tda);
         }
      }

      // Pass in ref objects
      for (UnsignedInt i = 0; i < measurements.size(); ++i)
      {
         StringArray refObjects = measurements[i]->GetRefObjectNameArray(
               Gmat::UNKNOWN_OBJECT);

         for (UnsignedInt j = 0; j < refObjects.size(); ++j)
         {
            GmatBase *obj = NULL;
            for (UnsignedInt k = 0; k < references.size(); ++k)
            {
               if (references[k]->GetName() == refObjects[j])
               {
                  obj = references[k];
                  break;
               }
            }
            if (obj)
            {
               if (measurements[i]->SetRefObject(obj, obj->GetType(),
                     obj->GetName()) == false)
                  MessageInterface::ShowMessage("The reference object %s was "
                        "requested but not set in a measurement referenced "
                        "by %s\n", obj->GetName().c_str(),
                        instanceName.c_str());
            }
            else
            {
               throw MeasurementException("The reference object " +
                     refObjects[j] + " was requested but not available in the "
                     "tracking file set " + instanceName);
            }
         }
      }

      retval = true;

      // Initialize the Adapters
      for (UnsignedInt i = 0; i < measurements.size(); ++i)
      {
         measurements[i]->SetSolarSystem(solarsystem);
         if (thePropagator)
            measurements[i]->SetPropagator(thePropagator);

         // Set measurement corrections to TrackingDataAdapter
         if (useRelativityCorrection)
            measurements[i]->SetCorrection("Moyer","Relativity");
         if (useETminusTAICorrection)
            measurements[i]->SetCorrection("Moyer","ET-TAI");

         // Set ramped frequency tables to TrackingDataAdapter
         for (UnsignedInt k = 0; k < rampedTablenames.size(); ++k)
         {
            measurements[i]->SetStringParameter("RampTables", rampedTablenames[k], k);
         }

         std::string measType = measurements[i]->GetStringParameter("MeasurementType");
         // Set range modulo constant for DSNRange
         if (measType == "DSNRange")
         {
            measurements[i]->SetRealParameter("RangeModuloConstant", rangeModulo);
         }

         // Set service access, node 4 frequency and band, SMAR id for TDRSDoppler
         if (measType == "TDRSDoppler_HZ")
         {
            // set the list of service access to TDRSDopplerAdapter object
            for (UnsignedInt accIndex = 0; accIndex < tdrsServiceAccessList.size(); ++accIndex) 
               measurements[i]->SetStringParameter("ServiceAccess", tdrsServiceAccessList[accIndex], accIndex);

            // set node 4 frequency and frequency band to TDRSDopplerAdapter object
            measurements[i]->SetRealParameter("Node4Frequency", tdrsNode4Frequency);
            measurements[i]->SetIntegerParameter("Node4Band", tdrsNode4Band);

            // set SMARD Id and TDRS data flag to TDRSDopplerAdapter object
            measurements[i]->SetIntegerParameter("SmarId", tdrsSMARID);
            measurements[i]->SetIntegerParameter("DataFlag", tdrsDataFlag);
         }

         // Set doppler count interval for Doppler
         if ((measType == "Doppler")||(measType == "Doppler_RangeRate")||(measType == "TDRSDoppler_HZ"))
         {
            measurements[i]->SetRealParameter("DopplerCountInterval", dopplerCountInterval);
         }
         // Set DopplerInterval for RangeRate
         if (measType == "RangeRate")
         {
            //measurements[i]->SetRealParameter("DopplerInterval", dopplerInterval);
            measurements[i]->SetRealParameter("DopplerInterval", dopplerCountInterval);      // unit: second
         }

         // Initialize TrackingDataAdapter
         retval = retval && measurements[i]->Initialize();
      }
      
      // Initialize data filters
      for (UnsignedInt i = 0; i < dataFilters.size(); ++i)
         dataFilters[i]->Initialize();

      isInitialized = true;
   }
   
   #ifdef DEBUG_INITIALIZE
      MessageInterface::ShowMessage("dataFilters.size() = %d\n", dataFilters.size());
      for (UnsignedInt i = 0; i < dataFilters.size(); ++i)
         MessageInterface::ShowMessage("filter %d: name = '%s'   pointer = <%p>\n", i, dataFilters[i]->GetName().c_str(), dataFilters[i]);

      MessageInterface::ShowMessage("Exit TrackingFileSet::Initialize() <%s,%p>\n", GetName().c_str(), this);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// const STringArray& GetParticipants() const
//------------------------------------------------------------------------------
/**
 * Retrieves the participants specified in the tracking file set
 *
 * @return  a string array containing names of all participants listed in tracking file set
 */
//------------------------------------------------------------------------------
const StringArray& TrackingFileSet::GetParticipants() const
{                                                                                         
#ifdef DEBUG_GET_PARTICIPANT
   MessageInterface::ShowMessage("TrackingFileSet::GetParticipants().\n");
#endif

   static StringArray tconfigs;
   tconfigs.clear();
   //StringArray tconfigs;

   for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
      for (UnsignedInt j = 0; j < trackingConfigs[i].strands.size(); ++j)
         for (UnsignedInt k=0; k < trackingConfigs[i].strands[j].size(); ++k)
            if (find(tconfigs.begin(), tconfigs.end(),
                  trackingConfigs[i].strands[j][k]) == tconfigs.end())
               tconfigs.push_back(trackingConfigs[i].strands[j][k]);

   return tconfigs;
}


//------------------------------------------------------------------------------
// std::vector<TrackingDataAdapter*> *GetAdapters()
//------------------------------------------------------------------------------
/**
 * Retrieves the tracking data adapters specified in the tracking file set
 *
 * @return The adapters, in an std::vector
 */
//------------------------------------------------------------------------------
std::vector<TrackingDataAdapter*> *TrackingFileSet::GetAdapters()
{
   return &measurements;
}


//------------------------------------------------------------------------------
// TrackingDataAdapter* BuildAdapter(const StringArray& strand,
//       const std::string& type)
//------------------------------------------------------------------------------
/**
 * Builds a tracking data adapter for the tracking file set
 *
 * @param strand The ordered list of nodes in the strands used in the adapter
 * @param type The type of adapter needed
 *
 * @return The adapter
 *
 * @todo This implementation is not the final code.  The adapter should be built
 *       in a Factory so that users can add new models without changing core nav
 *       code.  In addition, it is currently coded for only one strand.
 */
//------------------------------------------------------------------------------
TrackingDataAdapter* TrackingFileSet::BuildAdapter(const StringArray& strand,
      const std::string& type, Integer configIndex)
{
#ifdef DEBUG_BUILD_ADAPTER
   MessageInterface::ShowMessage("TrackingFileSet::BuildAdapter(, type = <%s>, configIndex = %d)   enter\n", type.c_str(), configIndex);
#endif

   TrackingDataAdapter *retval = NULL;
   std::vector<StringArray> nodelist;
   StringArray currentStrand;
   std::map<std::string,std::string> designators;

   std::stringstream designator;
   // Counts for spacecraft and stations identified in GEODYN format
   Integer sCount = 0, tCount = 0;

   /// @todo: Check to see if this code works for multi-strand measurements.
   ///        Original code assumed a single strand, so it may need modification
   ///        here and wrapping in an outer loop.  Hence the indentation.
   
   // 1. Set value for designators map:
   for (UnsignedInt i = 0; i < strand.size(); ++i)
   {
      //designator.str() = "";
      // Search for a referent object with the same name of node strand[i]
      UnsignedInt j = 0;
      for (; j < references.size(); ++j)
      {
         if (references[j]->GetName() == strand[i])
            break;
      }

      // If found then add value to designators map
      if (j < references.size())
      {
         Gmat::ObjectType pType = references[j]->GetType();

         switch(pType)
         {
            case Gmat::SPACECRAFT:
               if (designators.find(strand[i]) == designators.end())
               {
                  ++sCount;
                  designator.str("");
                  designator << "S" << sCount;
                  designators[strand[i]] = designator.str();
               }
               break;

            case Gmat::GROUND_STATION:
               if (designators.find(strand[i]) == designators.end())
               {
                  ++tCount;
                  designator.str("");
                  designator << "T" << tCount;
                  designators[strand[i]] = designator.str();
               }
               break;

            default:
               throw MeasurementException("Object type not recognized in "
                           "the tracking file set strand mapping code");
               break;  // No effect, but it avoids a warning message
         }
      }
   }
   
   // And now build the node list
   /// @todo: Check as described above.
   // 2. Add all spacecraft and station IDs to currentStrand
   currentStrand.clear();
   for (UnsignedInt i = 0; i < strand.size(); ++i)
   {
      currentStrand.push_back(designators[strand[i]]);
   }
   nodelist.push_back(currentStrand);

   
   /// @todo Move this into a Factory so that plugin adapters work
   // 3. Create TrackingDataAdapter for a given measurement type
   std::stringstream ss;
   //ss << instanceName << "_Cfig" << configIndex+1 << "_" << type;
   ss << instanceName << "_{";
   for (UnsignedInt i = 0; i < strand.size(); ++i)
   {
      ss << strand[i];
      if (i < strand.size()-1)
         ss << ",";
   }
   ss << "}_" << type;
   std::string adapterName = ss.str();          // tracking adapter name contains TrackingFileSet name following tracking configuration index and type 
   //MessageInterface::ShowMessage("Adapter name: <%s>\n", adapterName.c_str());
   
   if (type == "Range_KM")                                    // It is GN (or USN) Range
   {
      retval = new RangeAdapterKm(adapterName);
      if (retval)
      {
         retval->UsesLightTime(useLighttime);
         retval->SetStringParameter("MeasurementType", type);
      }
   }
   else if (type == "DSNRange")                               // It is DSN Range
   {
      retval = new DSNRangeAdapter(adapterName);
      if (retval)
      {
         retval->UsesLightTime(useLighttime);
         retval->SetStringParameter("MeasurementType", type);
      }
   }
   else if (type == "Doppler")                               // It is DSN Doppler
   {
      retval = new DopplerAdapter(adapterName);
      if (retval)
      {
         ((DopplerAdapter*)retval)->adapterS = (RangeAdapterKm*)BuildAdapter(strand, "Range_KM", configIndex);
         retval->UsesLightTime(useLighttime);
         retval->SetStringParameter("MeasurementType", type);
      }
   }                                                                  
   else if (type == "Doppler_RangeRate")
   {
      retval = new GNDopplerAdapter(adapterName);
      if (retval)
      {
         ((GNDopplerAdapter*)retval)->adapterS = (RangeAdapterKm*)BuildAdapter(strand, "Range_KM", configIndex);
         retval->UsesLightTime(useLighttime);
         retval->SetStringParameter("MeasurementType", type);
      }
   }
   else if (type == "TDRSDoppler_HZ")                    // It is 2-ways TDRS Doppler
   {
      retval = new TDRSDopplerAdapter(adapterName);
      if (retval)
      {
         retval->UsesLightTime(useLighttime);
         retval->SetStringParameter("MeasurementType", type);

         // Add code to specify short strand here
         StringArray shortStrand;
         if (strand.size() != 5)
            throw MeasurementException("Error: Signal path does not contain 5 participants.\n");
         shortStrand.push_back(strand[0]);
         shortStrand.push_back(strand[3]);
         shortStrand.push_back(strand[4]);

         ((TDRSDopplerAdapter*)retval)->adapterES = (RangeAdapterKm*)BuildAdapter(shortStrand, "Range_KM", configIndex);
         ((TDRSDopplerAdapter*)retval)->adapterSL = (RangeAdapterKm*)BuildAdapter(strand, "Range_KM", configIndex);
         ((TDRSDopplerAdapter*)retval)->adapterSS = (RangeAdapterKm*)BuildAdapter(shortStrand, "Range_KM", configIndex);
      }
   }
   else if (type == "RangeRate")
   {                                                                  
      retval = new RangeRateAdapterKps(instanceName + type);              
      if (retval)                                                     
      {
         retval->UsesLightTime(useLighttime);                         
         retval->SetStringParameter("MeasurementType", type);       
      }
   }
   else if (type == "PointRangeRate")
   {
      retval = new PointRangeRateAdapterKps(instanceName + type);
      if (retval)
      {
         retval->UsesLightTime(useLighttime);
         retval->SetStringParameter("MeasurementType", type);
      }
   }
   else                                                               
      throw MeasurementException("Error: '"+ type +"' measurement type was "
            "not implemented in this version of EstimationPlugin.\n");
   
   if (retval)
   {
      TFSMagicNumbers *mn = TFSMagicNumbers::Instance();
      //MeasureModel *mm = new MeasureModel(instanceName + type + "Measurement");
      MeasureModel *mm = new MeasureModel(adapterName + "Measurement");
      retval->SetMeasurement(mm);
      Integer magicNumber = mn->GetMagicNumber(nodelist, type);
      Real multiplier = mn->GetMNMultiplier(magicNumber);
      retval->SetModelTypeID(magicNumber, type, multiplier);

      // Pass in the signal paths
      std::string theStrand;
      for (UnsignedInt k=0; k < strand.size(); ++k)
      {
         if (k > 0)
            theStrand += ", ";
         theStrand += strand[k];
      }
      retval->SetStringParameter("SignalPath", theStrand);
      //MessageInterface::ShowMessage("Add signal path <%s> to %s adapter\n", theStrand.c_str(), retval->GetName().c_str());
   }

#ifdef DEBUG_BUILD_ADAPTER
   MessageInterface::ShowMessage("TrackingFileSet::BuildAdapter(, type = <%s>, configIndex = %d)   exit\n", type.c_str(), configIndex);
#endif

   return retval;
}


bool TrackingFileSet::GenerateTrackingConfigs(std::vector<StringArray> strandsList, StringArray typesList)
{
   if (strandsList.empty())
      return true;

   // Validate measurement type in typesList                                                                      // GMT-5701
   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();                                                        // GMT-5701
   //if ((runmode != GmatGlobal::TESTING) && (runmode != GmatGlobal::TESTING_NO_PLOTS))                           // GMT-5701
   if (runmode != GmatGlobal::TESTING)                                                                            // GMT-5701
   {                                                                                                              // GMT-5701
      for (Integer i = 0; i < typesList.size(); ++i)                                                              // GMT-5701
      {                                                                                                           // GMT-5701
         if ((typesList[i] != "DSNRange") && (typesList[i] != "Doppler"))                                         // GMT-5701
            throw MeasurementException("Error: Observation's data type '" + typesList[i] +                        // GMT-5701
            "' is not allowed in the current GMAT version. It only allows 'DSNRange' and 'Doppler' observation.\n");         // GMT-5701
      }                                                                                                           // GMT-5701
   }                                                                                                              // GMT-5701

   // Generate a list of tracking configs
   MessageInterface::ShowMessage("Total of %d tracking configurations are generated for tracking file set %s:\n", strandsList.size(), GetName().c_str());
   for(UnsignedInt i = 0; i < strandsList.size(); ++i)
   {
      MeasurementDefinition md;
      md.SetDefinitionString(strandsList[i], typesList[i]);
      trackingConfigs.push_back(md);
      trackingConfigs[i].GetDefinitionString();
      MessageInterface::ShowMessage("   Tracking config %d: %s\n", i, md.GetDefinitionString().c_str());
   }

   isInitialized = false;
   
   return Initialize();
}


// Internal class definition

//------------------------------------------------------------------------------
// TrackingFileSet::MeasurementDefinition::MeasurementDefinition()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
TrackingFileSet::MeasurementDefinition::MeasurementDefinition()
{
}


//------------------------------------------------------------------------------
// MeasurementDefinition::~MeasurementDefinition()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TrackingFileSet::MeasurementDefinition::~MeasurementDefinition()
{
   for (UnsignedInt i = 0; i < strands.size(); ++i)
      strands[i].clear();
   strands.clear();
   types.clear();
}

//------------------------------------------------------------------------------
// MeasurementDefinition::MeasurementDefinition(const MeasurementDefinition& md)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param md The definition copied to this one
 */
//------------------------------------------------------------------------------
TrackingFileSet::MeasurementDefinition::MeasurementDefinition(
      const MeasurementDefinition& md) :
            types       (md.types)
{
   for (UnsignedInt i = 0; i < md.strands.size(); ++i)
   {
      StringArray strand = md.strands[i];
      strands.push_back(strand);
   }
}


//------------------------------------------------------------------------------
// MeasurementDefinition& MeasurementDefinition::operator=(
//       const MeasurementDefinition& md)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param md The definition copied into this one
 *
 * @return This MeasurementDefinition, set to match md
 */
//------------------------------------------------------------------------------
TrackingFileSet::MeasurementDefinition&
      TrackingFileSet::MeasurementDefinition::operator=(
            const MeasurementDefinition& md)
{
   if (this != &md)
   {
      types = md.types;

      strands.clear();
      for (UnsignedInt i = 0; i < md.strands.size(); ++i)
      {
         StringArray strand = md.strands[i];
         strands.push_back(strand);
      }
   }

   return *this;
}


bool TrackingFileSet::MeasurementDefinition::SetDefinitionString(StringArray strand, std::string measType)
{
   strands.push_back(strand);
   types.push_back(measType);

   return true;
}


//------------------------------------------------------------------------------
// std::string MeasurementDefinition::GetDefinitionString() const
//------------------------------------------------------------------------------
/**
 * Generates the string describing a measurement definition
 *
 * @return The string, as it could appear in script
 */
//------------------------------------------------------------------------------
std::string TrackingFileSet::MeasurementDefinition::GetDefinitionString() const
{
   std::string configstring = "{";
   for (UnsignedInt i = 0; i < strands.size(); ++i)
   {
      if (i > 0)
         configstring += ",";
      configstring += "{";
      for (UnsignedInt j = 0; j < strands[i].size(); ++j)
      {
         if (j > 0)
            configstring += ",";
         configstring += strands[i][j];
      }
      configstring += "}";
   }
   for (UnsignedInt i = 0; i < types.size(); ++i)
   {
      configstring += ",";
      configstring += types[i];
   }
   configstring += "}";
   return configstring;
}
