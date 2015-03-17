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
#include <sstream>                  // For magic number mapping code
/// Temporarily here; needs to move into a factory
#include "RangeAdapterKm.hpp"
#include "DSNRangeAdapter.hpp"
#include "DopplerAdapter.hpp"
#include "RangeRateAdapterKps.hpp"
#include "PointRangeRateAdapterKps.hpp"

//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION
//#define DEBUG_GET_PARAMETER

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
   dopplerCountInterval      (1.0)
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

   // cleat file names and ramped table names
   filenames.clear();
   rampedTablenames.clear();
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
         throw MeasurementException("Error: "+GetName()+".RangeModuloConstant has an invalid value. It has to be a positive number\n");

      rangeModulo = value;
      return rangeModulo;
   }

   if (id == DOPPLER_COUNT_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: "+GetName()+".DopplerCountInterval has an invalid value. It has to be a positive number\n");

      dopplerCountInterval = value;
      return dopplerCountInterval;
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
      MessageInterface::ShowMessage("TrackingFileSet::SetStringParameter(id = %d, "
            "value = '%s') called\n", id, value.c_str());
   #endif

   if (id == FILENAME)
   {
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
      if (find(rampedTablenames.begin(), rampedTablenames.end(), value) == rampedTablenames.end())
      {
         rampedTablenames.push_back(value);
         return true;
      }
      else
         throw MeasurementException("Error: ramped table name is replicated ('" + value + "')\n");

   }

   if (id == DATA_FILTERS)
   {
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
               "a ramped table file name");
   }

   if (id == DATA_FILTERS)
   {
      if (((Integer)dataFilterNames.size() > index) && (index >= 0))
         return dataFilterNames[index];
      else
         throw MeasurementException("Index out of bounds when trying to access "
               "a data filter name");
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
bool TrackingFileSet::SetStringParameter(const Integer id,
      const std::string& value1, const Integer index)                                                       // made changes by TUAN NGUYEN
{
   std::string value = value1;                                                                              // made changes by TUAN NGUYEN
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("TrackingFileSet::SetStringParameter(%d, "
            "'%s', %d) called\n", id, value.c_str(), index);
   #endif

   if (id == TRACKINGCONFIG)
   {
      if ((value.size() > 1) && (value.c_str()[0] == '{') && (value.c_str()[value.size()-1] == '}'))        // made changes by TUAN NGUYEN
      {
         // Processing a tracking config:
         value = value.substr(1,value.size()-2);                                                            // made changes by TUAN NGUYEN
         std::string term;                                                                                  // made changes by TUAN NGUYEN
         Integer pos = value.find_first_of(',');                                                            // made changes by TUAN NGUYEN
         Integer newIndex = 0;                                                                              // made changes by TUAN NGUYEN
         bool retVal;                                                                                       // made changes by TUAN NGUYEN

         while (pos != std::string::npos)                                                                   // made changes by TUAN NGUYEN
         {                                                                                                  // made changes by TUAN NGUYEN
            term = value.substr(0, pos);                                                                    // made changes by TUAN NGUYEN
            retVal = TrackingFileSet::SetStringParameter(id, term, newIndex);                               // made changes by TUAN NGUYEN
            if (retVal == false)                                                                            // made changes by TUAN NGUYEN
               return false;                                                                                // made changes by TUAN NGUYEN

            value = value.substr(pos+1);                                                                    // made changes by TUAN NGUYEN
            pos = value.find_first_of(',');                                                                 // made changes by TUAN NGUYEN
            ++newIndex;                                                                                     // made changes by TUAN NGUYEN
         }                                                                                                  // made changes by TUAN NGUYEN

         return TrackingFileSet::SetStringParameter(id, value, newIndex);                                   // made changes by TUAN NGUYEN
      }                                                                                                     // made changes by TUAN NGUYEN

      // MeasurementDefinition theDef;                                                                      // made changes by TUAN NGUYEN
      if (index == 0)
      {
         // Starting a new definition
         MeasurementDefinition newDef;
         trackingConfigs.push_back(newDef);
      }
      Integer defIndex = trackingConfigs.size() - 1;

      std::string rawName = value;
      TFSMagicNumbers *mn = TFSMagicNumbers::Instance();
      StringArray knownTypes = mn->GetKnownTypes();

      bool newStrand = false;
      const char* valarray = value.c_str();
      if ((valarray[0]=='{') || (trackingConfigs[defIndex].strands.size()==0))
      {
         newStrand = true;
         rawName = value.substr(1);
      }

      if (newStrand)
      {
         StringArray trackList;
         trackingConfigs[defIndex].strands.push_back(trackList);
      }
      Integer strandIndex = trackingConfigs[defIndex].strands.size() - 1;

      // Strip off trailing '}', and leading and trailing white space
      UnsignedInt loc = rawName.find('}');
      if (loc != std::string::npos)
         rawName = rawName.substr(0,loc);
      valarray = rawName.c_str();
      loc = 0;
      while ((valarray[loc] == ' ') || (valarray[loc] == '\t'))
         ++loc;
      Integer eloc = rawName.length() - 1;
      while ((valarray[eloc] == ' ') || (valarray[eloc] == '\t'))
         --eloc;
      rawName = rawName.substr(loc, eloc-loc+1);

      if (find(knownTypes.begin(), knownTypes.end(), rawName) == knownTypes.end())
         trackingConfigs[defIndex].strands[strandIndex].push_back(rawName);
      else
         trackingConfigs[defIndex].types.push_back(rawName);

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
      if (((Integer)filenames.size() > index) && (index >= 0))
         filenames[index] = value;
      else if ((Integer)filenames.size() == index)
      {
         filenames.push_back(value);
      }
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
      if (((Integer)rampedTablenames.size() > index) && (index >= 0))
         rampedTablenames[index] = value;
      else if ((Integer)rampedTablenames.size() == index)
      {
         rampedTablenames.push_back(value);
      }
      else
         throw MeasurementException("Index out of bounds when trying to "
               "set a ramped table file name");

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
            dataFilters.push_back(obj->Clone());
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
         retval = true;
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
   #ifdef DEBUG_INITIALIZATION
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
      if (trackingConfigs.size() == 0)
         throw MeasurementException("No TrackingConfig is defined for " + GetName() + " object.\n");

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

            // Pass in the signal paths
            std::string theStrand;
            for (UnsignedInt k=0; k < trackingConfigs[i].strands[0].size(); ++k)
            {
               if (k > 0)
                  theStrand += ", ";
               theStrand += trackingConfigs[i].strands[0][k];
            }

            tda->SetStringParameter("SignalPath", theStrand);

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

         // Set range modulo constant for DSNRange
         if (measurements[i]->GetStringParameter("MeasurementType") == "DSNRange")
         {
            measurements[i]->SetRealParameter("RangeModuloConstant", rangeModulo);
         }

         // Set doppler count interval for Doppler
         if (measurements[i]->GetStringParameter("MeasurementType") == "Doppler")
         {
            measurements[i]->SetRealParameter("DopplerCountInterval", dopplerCountInterval);
         }
         // Set range modulo constant for RangeRate
         if (measurements[i]->GetStringParameter("MeasurementType") == "RangeRate")
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
   
   #ifdef DEBUG_INITIALIZATION
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
// made changes by TUAN NGUYEN
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
   
   if (type == "Range")
   {
      retval = new RangeAdapterKm(adapterName);
      if (retval)
      {
         retval->UsesLightTime(useLighttime);
         retval->SetStringParameter("MeasurementType", type);
      }
   }
   else if (type == "DSNRange")
   {
      retval = new DSNRangeAdapter(adapterName);
      if (retval)
      {
         retval->UsesLightTime(useLighttime);
         retval->SetStringParameter("MeasurementType", type);
      }
   }
   else if (type == "Doppler")
   {
      retval = new DopplerAdapter(adapterName);
      if (retval)
      {
         ((DopplerAdapter*)retval)->adapterS = (RangeAdapterKm*)BuildAdapter(strand, "Range", configIndex); 
         retval->UsesLightTime(useLighttime);
         retval->SetStringParameter("MeasurementType", type);
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
   }

   return retval;
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
