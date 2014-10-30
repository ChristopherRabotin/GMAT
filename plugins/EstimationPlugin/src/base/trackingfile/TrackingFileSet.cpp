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
#include "USNRangeAdapter.hpp"
#include "RangeRateAdapterKps.hpp"
#include "PointRangeRateAdapterKps.hpp"

//#define DEBUG_INITIALIZATION

//------------------------------------------------------------------------------
// Static Data
//------------------------------------------------------------------------------

/// Strings describing the BatchEstimator parameters
const std::string TrackingFileSet::PARAMETER_TEXT[
                             TrackingFileSetParamCount - MeasurementModelBaseParamCount] =      // made changes by TUAN NGUYEN
{
   "AddTrackingConfig",             // TRACKINGCONFIG
   "Filename",                      // FILENAME
   "RampedTable",                   // RAMPED_TABLE               // made changes by TUAN NGUYEN
   "UseLighttime",                  // USELIGHTTIME
   "UseRelativityCorrection",       // USE_RELATIVITY             // made changes by TUAN NGUYEN
   "UseETminusTAI",                 // USE_ETMINUSTAI             // made changes by TUAN NGUYEN
//   "TroposphereModel",              // TROPOSPHERE_MODEL          // made changes by TUAN NGUYEN
//   "IonosphereModel",               // IONOSPHERE_MODEL           // made changes by TUAN NGUYEN
   "RangeNoiseSigma",               // RANGE_NOISESIGMA           // made changes by TUAN NGUYEN
   "RangeErrorModel",               // RANGE_ERRORMODEL           // made changes by TUAN NGUYEN
   "DSNRangeNoiseSigma",            // DSNRANGE_NOISESIGMA        // made changes by TUAN NGUYEN
   "DSNRangeErrorModel",            // DSNRANGE_ERRORMODEL        // made changes by TUAN NGUYEN
   "DopplerNoiseSigma",             // DOPPLER_NOISESIGMA         // made changes by TUAN NGUYEN
   "DopplerErrorModel",             // DOPPLER_ERRORMODEL         // made changes by TUAN NGUYEN
   "RangeModuloConstant",           // RANGE_MODULO               // made changes by TUAN NGUYEN
   "DopplerInterval"          
};

/// Types of the BatchEstimator parameters
const Gmat::ParameterType TrackingFileSet::PARAMETER_TYPE[
                             TrackingFileSetParamCount - MeasurementModelBaseParamCount] =      // made changes by TUAN NGUYEN
{
   Gmat::STRINGARRAY_TYPE,          // TRACKINGCONFIG
   Gmat::STRINGARRAY_TYPE,          // FILENAME, but it's a list of names...
   Gmat::STRINGARRAY_TYPE,          // RAMPED_TABLE, but it's a list of names...
   Gmat::BOOLEAN_TYPE,              // USELIGHTTIME
   Gmat::BOOLEAN_TYPE,              // USE_RELATIVITY            // made changes by TUAN NGUYEN
   Gmat::BOOLEAN_TYPE,              // USE_ETMINUSTAI            // made changes by TUAN NGUYEN
//   Gmat::STRING_TYPE,               // TROPOSPHERE_MODEL         // made changes by TUAN NGUYEN
//   Gmat::STRING_TYPE,               // IONOSPHERE_MODEL          // made changes by TUAN NGUYEN
   Gmat::REAL_TYPE,                 // RANGE_NOISESIGMA          // made changes by TUAN NGUYEN
   Gmat::STRING_TYPE,               // RANGE_ERROR_MODEL         // made changes by TUAN NGUYEN
   Gmat::REAL_TYPE,                 // DSNRANGE_NOISESIGMA       // made changes by TUAN NGUYEN
   Gmat::STRING_TYPE,               // DSNRANGE_ERROR_MODEL      // made changes by TUAN NGUYEN
   Gmat::REAL_TYPE,                 // DOPPLER_NOISESIGMA        // made changes by TUAN NGUYEN
   Gmat::STRING_TYPE,               // DOPPLER_ERROR_MODEL       // made changes by TUAN NGUYEN
   Gmat::REAL_TYPE,                 // RANGE_MODULO              // made changes by TUAN NGUYEN
   Gmat::REAL_TYPE                  // Doppler Interval
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
   MeasurementModelBase (name, "TrackingFileSet"),
   useLighttime         (false),    // Default to true once implemented
   solarsystem          (NULL),
   thePropagator        (NULL),
   useRelativityCorrection   (false),            // made changes by TUAN NGUYEN
   useETminusTAICorrection   (false),            // made changes by TUAN NGUYEN
   rangeNoiseSigma           (1.0),              // made changes by TUAN NGUYEN
   rangeErrorModel           ("RandomConstant"), // made changes by TUAN NGUYEN
   dsnrangeNoiseSigma        (1.0),              // made changes by TUAN NGUYEN
   dsnrangeErrorModel        ("RandomConstant"), // made changes by TUAN NGUYEN
   dopplerNoiseSigma         (1.0),              // made changes by TUAN NGUYEN
   dopplerErrorModel         ("RandomConstant"), // made changes by TUAN NGUYEN
   rangeModulo               (1.0e18)           // made changes by TUAN NGUYEN
//   troposphereModel          ("None"),           // made changes by TUAN NGUYEN
//   ionosphereModel           ("None")            // made changes by TUAN NGUYEN
   dopplerInterval           (1.0)           
{
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

   // clear tracking configs
   trackingConfigs.clear();
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
   rampedTablenames          (tfs.rampedTablenames),          // made changes by TUAN NGUYEN
   useLighttime              (tfs.useLighttime),
   solarsystem               (tfs.solarsystem),
   thePropagator             (tfs.thePropagator),
   references                (tfs.references),
   datafiles                 (tfs.datafiles),
   useRelativityCorrection   (tfs.useRelativityCorrection),   // made changes by TUAN NGUYEN
   useETminusTAICorrection   (tfs.useETminusTAICorrection),   // made changes by TUAN NGUYEN
   rangeNoiseSigma           (tfs.rangeNoiseSigma),           // made changes by TUAN NGUYEN
   rangeErrorModel           (tfs.rangeErrorModel),           // made changes by TUAN NGUYEN
   dsnrangeNoiseSigma        (tfs.dsnrangeNoiseSigma),        // made changes by TUAN NGUYEN
   dsnrangeErrorModel        (tfs.dsnrangeErrorModel),        // made changes by TUAN NGUYEN
   dopplerNoiseSigma         (tfs.dopplerNoiseSigma),         // made changes by TUAN NGUYEN
   dopplerErrorModel         (tfs.dopplerErrorModel),         // made changes by TUAN NGUYEN
   rangeModulo               (tfs.rangeModulo)               // made changes by TUAN NGUYEN
//   troposphereModel          (tfs.troposphereModel),          // made changes by TUAN NGUYEN
//   ionosphereModel           (tfs.ionosphereModel)            // made changes by TUAN NGUYEN
   rangeModulo               (tfs.rangeModulo),               // made changes by TUAN NGUYEN
   dopplerInterval           (tfs.dopplerInterval)          
{
   for (UnsignedInt i = 0; i < tfs.trackingConfigs.size(); ++i)
      trackingConfigs.push_back(tfs.trackingConfigs[i]);
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
   if (this != &tfs)
   {
      MeasurementModelBase::operator=(tfs);

      for (UnsignedInt i = 0; i < measurements.size(); ++i)    // made changes by TUAN NGUYEN
      {                                                        // made changes by TUAN NGUYEN
         if (measurements[i])                                  // made changes by TUAN NGUYEN
            delete measurements[i];                            // made changes by TUAN NGUYEN
      }                                                        // made changes by TUAN NGUYEN
      measurements.clear();                                    // made changes by TUAN NGUYEN

      trackingConfigs.clear();

      for (UnsignedInt i = 0; i < tfs.trackingConfigs.size(); ++i)
         trackingConfigs.push_back(tfs.trackingConfigs[i]);

      filenames               = tfs.filenames;
      rampedTablenames        = tfs.rampedTablenames;          // made changes by TUAN NGUYEN
      useLighttime            = tfs.useLighttime;
      solarsystem             = tfs.solarsystem;
      thePropagator           = tfs.thePropagator;
      references              = tfs.references;
      datafiles               = tfs.datafiles;
      useRelativityCorrection = tfs.useRelativityCorrection;   // made changes by TUAN NGUYEN
      useETminusTAICorrection = tfs.useETminusTAICorrection;   // made changes by TUAN NGUYEN
      rangeNoiseSigma         = tfs.rangeNoiseSigma;           // made changes by TUAN NGUYEN
      rangeErrorModel         = tfs.rangeErrorModel;           // made changes by TUAN NGUYEN
      dsnrangeNoiseSigma      = tfs.dsnrangeNoiseSigma;        // made changes by TUAN NGUYEN
      dsnrangeErrorModel      = tfs.dsnrangeErrorModel;        // made changes by TUAN NGUYEN
      dopplerNoiseSigma       = tfs.dopplerNoiseSigma;         // made changes by TUAN NGUYEN
      dopplerErrorModel       = tfs.dopplerErrorModel;         // made changes by TUAN NGUYEN
      rangeModulo             = tfs.rangeModulo;               // made changes by TUAN NGUYEN
//      troposphereModel        = tfs.troposphereModel;          // made changes by TUAN NGUYEN
//      ionosphereModel         = tfs.ionosphereModel;           // made changes by TUAN NGUYEN
      dopplerInterval             = tfs.dopplerInterval;         

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
   if (id == RANGE_NOISESIGMA)
      return rangeNoiseSigma;

   if (id == DSNRANGE_NOISESIGMA)
      return dsnrangeNoiseSigma;

   if (id == DOPPLER_NOISESIGMA)
      return dopplerNoiseSigma;

   if (id == RANGE_MODULO)
      return rangeModulo;

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
   if (id == RANGE_NOISESIGMA)
   {
      if (value <= 0.0)
      {
         std::stringstream ss;
         ss << "Error: value of " << GetName() << ".RangeNoiseSigma (" << value << ") is invalid. It value should be a positive number.\n";
         throw MeasurementException(ss.str());
      }
      rangeNoiseSigma = value;
      return rangeNoiseSigma;
   }

   if (id == DSNRANGE_NOISESIGMA)
   {
      if (value <= 0.0)
      {
         std::stringstream ss;
         ss << "Error: value of " << GetName() << ".DSNRangeNoiseSigma (" << value << ") is invalid. It value should be a positive number.\n";
         throw MeasurementException(ss.str());
      }
      dsnrangeNoiseSigma = value;
      return dsnrangeNoiseSigma;
   }

   if (id == DOPPLER_NOISESIGMA)
   {
      if (value <= 0.0)
      {
         std::stringstream ss;
         ss << "Error: value of " << GetName() << ".DopplerNoiseSigma (" << value << ") is invalid. It value should be a positive number.\n";
         throw MeasurementException(ss.str());
      }
      dopplerNoiseSigma = value;
      return dopplerNoiseSigma;
   }

   if (id == RANGE_MODULO)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: "+GetName()+".RangeModuloConstant has an invalid value. It has to be a positive number\n");

      rangeModulo = value;
      return rangeModulo;
   }
   if (id == DOPPLER_INTERVAL)
   {
      if (value <= 0.0)
         throw MeasurementException("Error: "+GetName()+".DopplerInterval has an invalid value. It has to be a positive number\n");

      dopplerInterval = value;
      return dopplerInterval;
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
   //if (id == TROPOSPHERE_MODEL)
   //   return troposphereModel;

   //if (id == IONOSPHERE_MODEL)
   //   return ionosphereModel;

   if (id == RANGE_ERRORMODEL)
      return rangeErrorModel;

   if (id == DSNRANGE_ERRORMODEL)
      return dsnrangeErrorModel;

   if (id == DOPPLER_ERRORMODEL)
      return dopplerErrorModel;

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
   //if (id == TROPOSPHERE_MODEL)
   //{
   //   troposphereModel = value;
   //   return true;
   //}

   //if (id == IONOSPHERE_MODEL)
   //{
   //   ionosphereModel = value;
   //   return true;
   //}

   if (id == FILENAME)
   {
      if (find(filenames.begin(), filenames.end(), value) == filenames.end())
      {
         filenames.push_back(value);
         return true;
      }
      return false;
   }

   if (id == RAMPED_TABLENAME)                                                                          // made changes by TUAN NGUYEN
   {                                                                                                    // made changes by TUAN NGUYEN
      if (find(rampedTablenames.begin(), rampedTablenames.end(), value) == rampedTablenames.end())      // made changes by TUAN NGUYEN
      {                                                                                                 // made changes by TUAN NGUYEN
         rampedTablenames.push_back(value);                                                             // made changes by TUAN NGUYEN
         return true;                                                                                   // made changes by TUAN NGUYEN
      }                                                                                                 // made changes by TUAN NGUYEN
      return false;                                                                                     // made changes by TUAN NGUYEN
   }                                                                                                    // made changes by TUAN NGUYEN

   if (id == RANGE_ERRORMODEL)
   {
      if (value != "RandomConstant")
         throw MeasurementException("Error: '"+ value + "' is invalid value for " + GetName() + ".RangeErrorModel\n");
      rangeErrorModel = value;
      return true;
   }

   if (id == DSNRANGE_ERRORMODEL)
   {
      if (value != "RandomConstant")
         throw MeasurementException("Error: '"+ value + "' is invalid value for " + GetName() + ".DSNRangeErrorModel\n");
      dsnrangeErrorModel = value;
      return true;
   }

   if (id == DOPPLER_ERRORMODEL)
   {
      if (value != "RandomConstant")
         throw MeasurementException("Error: '" + value + "' is invalid value for " + GetName() + ".DopplerErrorModel\n");
      dopplerErrorModel = value;
      return true;
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

   if (id == RAMPED_TABLENAME)                                                     // made changes by TUAN NGUYEN
   {                                                                               // made changes by TUAN NGUYEN
      if (((Integer)rampedTablenames.size() > index) && (index >= 0))              // made changes by TUAN NGUYEN
         return rampedTablenames[index];                                           // made changes by TUAN NGUYEN
      else                                                                         // made changes by TUAN NGUYEN
         throw MeasurementException("Index out of bounds when trying to access "   // made changes by TUAN NGUYEN
               "a ramped table file name");                                        // made changes by TUAN NGUYEN
   }                                                                               // made changes by TUAN NGUYEN

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
      const std::string& value, const Integer index)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("TrackingFileSet::SetStringParameter(%d, "
            "'%s', %d) called\n", id, value.c_str(), index);
   #endif

   if (id == TRACKINGCONFIG)
   {
      MeasurementDefinition theDef;
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
            MessageInterface::ShowMessage("   %s\n", trackingConfigs[i].c_str());
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

   if (id == RAMPED_TABLENAME)                                                       // made changes by TUAN NGUYEN
   {                                                                                 // made changes by TUAN NGUYEN
      if (((Integer)rampedTablenames.size() > index) && (index >= 0))                // made changes by TUAN NGUYEN
         rampedTablenames[index] = value;                                            // made changes by TUAN NGUYEN
      else if ((Integer)rampedTablenames.size() == index)                            // made changes by TUAN NGUYEN
      {                                                                              // made changes by TUAN NGUYEN
         rampedTablenames.push_back(value);                                          // made changes by TUAN NGUYEN
      }                                                                              // made changes by TUAN NGUYEN
      else                                                                           // made changes by TUAN NGUYEN
         throw MeasurementException("Index out of bounds when trying to "            // made changes by TUAN NGUYEN
               "set a ramped table file name");                                      // made changes by TUAN NGUYEN

      #ifdef DEBUG_INITIALIZATION                                                    // made changes by TUAN NGUYEN
         MessageInterface::ShowMessage("%d members in config:\n",                    // made changes by TUAN NGUYEN
               rampedTablenames.size());                                             // made changes by TUAN NGUYEN
         for (UnsignedInt i = 0; i < rampedTablenames.size(); ++i)                   // made changes by TUAN NGUYEN
            MessageInterface::ShowMessage("   %s\n", rampedTablenames[i].c_str());   // made changes by TUAN NGUYEN
      #endif                                                                         // made changes by TUAN NGUYEN
      return true;                                                                   // made changes by TUAN NGUYEN
   }                                                                                 // made changes by TUAN NGUYEN

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
   static StringArray tconfigs;
   if (id == TRACKINGCONFIG)
   {
      tconfigs.clear();
      for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
//         tconfigs.push_back(trackingConfigs[i].GetDefinitionString());
         for (UnsignedInt j = 0; j < trackingConfigs[i].strands.size(); ++j)
            for (UnsignedInt k=0; k < trackingConfigs[i].strands[j].size(); ++k)
               if (find(tconfigs.begin(), tconfigs.end(),
                     trackingConfigs[i].strands[j][k]) == tconfigs.end())
                  tconfigs.push_back(trackingConfigs[i].strands[j][k]);
      return tconfigs;
   }

   if (id == FILENAME)
      return filenames;

   if (id == RAMPED_TABLENAME)                        // made changes by TUAN NGUYEN
      return rampedTablenames;                        // made changes by TUAN NGUYEN

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
      MessageInterface::ShowMessage("Called TrackingFileSet::"
            "GetRefObjectNameArray(%d)\n", type);
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
   bool retval = false;

   if ((type == Gmat::DATA_FILE) || (type == Gmat::UNKNOWN_OBJECT))
   {
      for (UnsignedInt i = 0; i < filenames.size(); ++i)
      {
         if (filenames[i] == oldName)
         {
            filenames[i] = newName;
            retval = true;
            break;
         }
      }

      //for (UnsignedInt i = 0; i < rampedTablenames.size(); ++i)            // made changes by TUAN NGUYEN
      //{                                                                    // made changes by TUAN NGUYEN
      //   if (rampedTablenames[i] == oldName)                               // made changes by TUAN NGUYEN
      //   {                                                                 // made changes by TUAN NGUYEN
      //      rampedTablenames[i] = newName;                                 // made changes by TUAN NGUYEN
      //      retval = true;                                                 // made changes by TUAN NGUYEN
      //      break;                                                         // made changes by TUAN NGUYEN
      //   }                                                                 // made changes by TUAN NGUYEN
      //}                                                                    // made changes by TUAN NGUYEN
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
                  retval = true;
               }
            }
         }
      }
   }

   return retval;
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
      MessageInterface::ShowMessage("Entered TrackingFileSet::Initialize()\n");
   #endif

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

      // Build the adapters
      for (UnsignedInt i = 0; i < trackingConfigs.size(); ++i)
      {
         for (UnsignedInt j = 0; j < trackingConfigs[i].types.size(); ++j)
         {
            if (trackingConfigs[i].strands.size() != 1)
               throw MeasurementException("Multiple strands and empty strands "
                     "are not yet implemented");

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

         // Set measurement corrections to TrackingDataAdapter                     // made changes by TUAN NGUYEN
         if (useRelativityCorrection)                                              // made changes by TUAN NGUYEN
            measurements[i]->SetCorrection("Moyer","Relativity");                  // made changes by TUAN NGUYEN
         if (useETminusTAICorrection)                                              // made changes by TUAN NGUYEN
            measurements[i]->SetCorrection("Moyer","ET-TAI");                      // made changes by TUAN NGUYEN
         //if (troposphereModel != "")                                               // made changes by TUAN NGUYEN
         //   measurements[i]->SetCorrection(troposphereModel,"TroposphereModel");   // made changes by TUAN NGUYEN
         //if (ionosphereModel != "")                                                // made changes by TUAN NGUYEN
         //   measurements[i]->SetCorrection(ionosphereModel,"IonosphereModel");     // made changes by TUAN NGUYEN

         // Set ramped frequency tables to TrackingDataAdapter                            // made changes by TUAN NGUYEN
         for (UnsignedInt k = 0; k < rampedTablenames.size(); ++k)                        // made changes by TUAN NGUYEN
         {
            measurements[i]->SetStringParameter("RampTables", rampedTablenames[k], k);    // made changes by TUAN NGUYEN
         }

         // Set noise sigma and error model to TrackingDataAdapter
         measurements[i]->SetRealParameter("NoiseSigma", rangeNoiseSigma, 0);
         measurements[i]->SetRealParameter("NoiseSigma", dopplerNoiseSigma, 1);
         measurements[i]->SetRealParameter("NoiseSigma", dsnrangeNoiseSigma, 2);
         measurements[i]->SetStringParameter("ErrorModel", rangeErrorModel, 0);
         measurements[i]->SetStringParameter("ErrorModel", dopplerErrorModel, 1);
         measurements[i]->SetStringParameter("ErrorModel", dsnrangeErrorModel, 2);

         // Set range modulo constant for DSNRange
         if (measurements[i]->GetStringParameter("MeasurementType") == "DSNRange")
         {
            measurements[i]->SetRealParameter("RangeModuloConstant", rangeModulo);
         }
         // Set range modulo constant for RangeRate
         if (measurements[i]->GetStringParameter("MeasurementType") == "RangeRate")
         {
            measurements[i]->SetRealParameter("DopplerInterval", dopplerInterval);
         }

         // Initialize TrackingDataAdapter
         retval = retval && measurements[i]->Initialize();
      }

      isInitialized = true;
   }

   return retval;
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
      //designator.str() = "";                                          // made changes by TUAN NGUYEN
      // Search for a referent object with the same name of node strand[i]
      UnsignedInt j = 0;                                                // made changes by TUAN NGUYEN
      for (; j < references.size(); ++j)                                // made changes by TUAN NGUYEN
      {                                                                 // made changes by TUAN NGUYEN
         if (references[j]->GetName() == strand[i])                     // made changes by TUAN NGUYEN
            break;                                                      // made changes by TUAN NGUYEN
      }                                                                 // made changes by TUAN NGUYEN

      // If found then add value to designators map
      if (j < references.size())                                        // made changes by TUAN NGUYEN
      {                                                                 // made changes by TUAN NGUYEN
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
      }                                                                 // made changes by TUAN NGUYEN
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
   ss << instanceName << "_Cfig" << configIndex+1 << "_" << type;
   std::string adapterName = ss.str();          // tracking adapter name contains TrackingFileSet name following tracking configuration index and type 
   
   if (type == "Range")
   {
      retval = new RangeAdapterKm(adapterName);
      if (retval)
      {
         retval->UsesLightTime(useLighttime);
         retval->SetStringParameter("MeasurementType", type);         // made changes by TUAN NGUYEN
      }
   }
   else if (type == "DSNRange")                                       // made changes by TUAN NGUYEN
   {                                                                  // made changes by TUAN NGUYEN
      retval = new DSNRangeAdapter(adapterName);                      // made changes by TUAN NGUYEN
      if (retval)                                                     // made changes by TUAN NGUYEN
      {
         retval->UsesLightTime(useLighttime);                         // made changes by TUAN NGUYEN
         retval->SetStringParameter("MeasurementType", type);         // made changes by TUAN NGUYEN
      }
   }                                                                  // made changes by TUAN NGUYEN
   else if (type == "Doppler")                                        // made changes by TUAN NGUYEN
   {                                                                  // made changes by TUAN NGUYEN
      retval = new DopplerAdapter(adapterName);                       // made changes by TUAN NGUYEN
      if (retval)                                                     // made changes by TUAN NGUYEN
      {
         retval->UsesLightTime(useLighttime);                         // made changes by TUAN NGUYEN
         retval->SetStringParameter("MeasurementType", type);         // made changes by TUAN NGUYEN
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
            "not implemented in this version of EstimationPlugin.\n");    // made changes by TUAN NGUYEN

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
