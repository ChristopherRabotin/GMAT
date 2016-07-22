//$Id: StatisticAcceptFilter.cpp 1398 2015-03-03 14:10:37Z tdnguyen $
//------------------------------------------------------------------------------
//                                 StatisticAcceptFilter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2015/03/03
//
/**
 * Implementation for the StatisticAcceptFilter class
 */
//------------------------------------------------------------------------------


#include "StatisticAcceptFilter.hpp"
#include "GmatBase.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include <sstream>


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION
//#define DEBUG_DATA_FILTER
//#define DEBUG_FILTER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string StatisticAcceptFilter::PARAMETER_TEXT[] =
{
   "ThinMode",
   "ThinningFrequency",
};

const Gmat::ParameterType StatisticAcceptFilter::PARAMETER_TYPE[] =
{
   Gmat::STRING_TYPE,			   // THIN_MODE
   Gmat::INTEGER_TYPE,           // THINNING_FREQUENCY
};



//------------------------------------------------------------------------------
// StatisticAcceptFilter(const std::string name)
//------------------------------------------------------------------------------
/**
 * Constructor for StatisticAcceptFilter objects
 *
 * @param name The name of the object
 */
//------------------------------------------------------------------------------
StatisticAcceptFilter::StatisticAcceptFilter(const std::string name) :
   DataFilter        (name),
   thinMode          ("Frequency"),
   thinningFrequency (1)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("StatisticAcceptFilter default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::DATA_FILTER);
   objectTypeNames.push_back("StatisticsAcceptFilter");

   parameterCount = StatisticAcceptFilterParamCount;
}


//------------------------------------------------------------------------------
// ~StatisticAcceptFilter()
//------------------------------------------------------------------------------
/**
 * StatisticAcceptFilter destructor
 */
//------------------------------------------------------------------------------
StatisticAcceptFilter::~StatisticAcceptFilter()
{
}


//------------------------------------------------------------------------------
// StatisticAcceptFilter(const StatisticAcceptFilter& saf)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a StatisticAcceptFilter
 *
 * @param saf        The StatisticAcceptFilter object that provides data for the new one
 */
//------------------------------------------------------------------------------
StatisticAcceptFilter::StatisticAcceptFilter(const StatisticAcceptFilter& saf) :
   DataFilter            (saf),
   thinMode              (saf.thinMode),
   thinningFrequency     (saf.thinningFrequency)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("StatisticAcceptFilter copy constructor from <%s,%p>  to  <%s,%p>\n", saf.GetName().c_str(), &saf, GetName().c_str(), this);
#endif

}


//------------------------------------------------------------------------------
// StatisticAcceptFilter& operator=(const StatisticAcceptFilter& saf)
//------------------------------------------------------------------------------
/**
 * StatisticAcceptFilter assignment operator
 *
 * @param saf    The StatisticAcceptFilter object that provides data for the this one
 *
 * @return This object, configured to match saf
 */
//------------------------------------------------------------------------------
StatisticAcceptFilter& StatisticAcceptFilter::operator=(const StatisticAcceptFilter& saf)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("StatisticAcceptFilter operator = <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &saf)
   {
      DataFilter::operator=(saf);

      thinMode          = saf.thinMode;
      thinningFrequency = saf.thinningFrequency;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone method for StatisticAcceptFilter
 *
 * @return A clone of this object.
 */
//------------------------------------------------------------------------------
GmatBase* StatisticAcceptFilter::Clone() const
{
   return new StatisticAcceptFilter(*this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Code fired in the Sandbox when the Sandbox initializes objects prior to a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool StatisticAcceptFilter::Initialize()
{
#ifdef DEBUG_INITIALIZATION
	MessageInterface::ShowMessage("StatisticAcceptFilter<%s,%p>::Initialize()   entered\n", GetName().c_str(), this);
#endif

   bool retval = false;
   if (DataFilter::Initialize())
   {
      //@todo: Initialize code is here

      isInitialized = retval;
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("StatisticAcceptFilter<%s,%p>::Initialize()   exit\n", GetName().c_str(), this);
#endif
   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script a StatisticAcceptFilter property
 *
 * @param id The ID of the property
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string StatisticAcceptFilter::GetParameterText(const Integer id) const
{
   if (id >= DataFilterParamCount && id < StatisticAcceptFilterParamCount)
      return PARAMETER_TEXT[id - DataFilterParamCount];

   return DataFilter::GetParameterText(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units used for a property
 *
 * @param id The ID of the property
 *
 * @return The text string specifying the property's units
 */
//------------------------------------------------------------------------------
std::string StatisticAcceptFilter::GetParameterUnit(const Integer id) const
{
   // @Todo: It needs to add code to specify unit used for each parameter

   return DataFilter::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID associated with a scripted property string
 *
 * @param str The scripted string used for the property
 *
 * @return The associated ID
 */
//------------------------------------------------------------------------------
Integer StatisticAcceptFilter::GetParameterID(const std::string &str) const
{
   for (Integer i = DataFilterParamCount; i < StatisticAcceptFilterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - DataFilterParamCount])
      {
         if (IsParameterReadOnly(i))
            throw MeasurementException("Error: " + str + "parameter was not defined in StatisticsAcceptFilter.\n");
         return i;
      }
   }

   return DataFilter::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter type for a property
 *
 * @param id The ID of the property
 *
 * @return The ParameterType of the property
 */
//------------------------------------------------------------------------------
Gmat::ParameterType StatisticAcceptFilter::GetParameterType(const Integer id) const
{
   if (id >= DataFilterParamCount && id < StatisticAcceptFilterParamCount)
      return PARAMETER_TYPE[id - DataFilterParamCount];

   return DataFilter::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing the type of a property
 *
 * @param id The ID of the property
 *
 * @return The text description of the property type
 */
//------------------------------------------------------------------------------
std::string StatisticAcceptFilter::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


bool StatisticAcceptFilter::IsParameterReadOnly(const Integer id) const
{
   return DataFilter::IsParameterReadOnly(id);
}


bool StatisticAcceptFilter::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property
 *
 * @param id The ID of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string StatisticAcceptFilter::GetStringParameter(const Integer id) const
{
   if (id == THIN_MODE)
      return thinMode;

   return DataFilter::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string property
 *
 * @param id The ID of the property
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool StatisticAcceptFilter::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == THIN_MODE)
   {
      StringArray nameList = GetAllAvailableThinModes();
      if (find(nameList.begin(), nameList.end(), value) != nameList.end())
      {
         thinMode = value;
         return true;
      }
      else
         throw MeasurementException("Error: Value '" + value + "' set to " + GetName() + ".ThinMode is invalid.\n");  
   }

   if (id == FILENAMES)
   {
      if (value == "All")
      {
         if (find(observers.begin(), observers.end(), "From_AddTrackingConfig") != observers.end())
            throw MeasurementException("Error: Both 'All' and 'From_AddTrackingConfig' can not set to " + GetName() + ".FileNames simultanously.\n");  
      }
      else if (value == "From_AddTrackingConfig")
      {
         if (find(observers.begin(), observers.end(), "All") != observers.end())
            throw MeasurementException("Error: Both 'All' and 'From_AddTrackingConfig' can not set to " + GetName() + ".FileNames simultanously.\n");  
      }
   }

   return DataFilter::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property of a ErrorModel
 *
 * @param label The text description of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string StatisticAcceptFilter::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string property
 *
 * @param label The text description of the property
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool StatisticAcceptFilter::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* Retrieves an integer parameter
*
* @param id   Parameter ID
*
* @return     The parameter value
*/
//------------------------------------------------------------------------------
Integer StatisticAcceptFilter::GetIntegerParameter(const Integer id) const
{
   if (id == THINNING_FREQUENCY)
      return thinningFrequency;

   return DataFilter::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
* Set value to an integer parameter
*
* @param id     Parameter ID
* @param value  Value which is set to the parameter
*
* @return     The parameter value
*/
//------------------------------------------------------------------------------
Integer StatisticAcceptFilter::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == THINNING_FREQUENCY)
   {
      if (value > 0)
      {
         thinningFrequency = value;
         return thinningFrequency;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: An invalid value (" << value << ") is set to " << GetName() << ".ThinningFrequency parameter.\n";
         throw MeasurementException(ss.str());
      }
   }

   return DataFilter::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Retrieves an integer parameter
*
* @param label   Name of the parameter
*
* @return        The parameter value
*/
//------------------------------------------------------------------------------
Integer StatisticAcceptFilter::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
* Set value to an integer parameter
*
* @param label     Parameter's name
* @param value     Value which is set to the parameter
*
* @return          The parameter value
*/
//------------------------------------------------------------------------------
Integer StatisticAcceptFilter::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SetTrackingConfigs(StringArray tkconfigs)
//------------------------------------------------------------------------------
/**
* Set value to an integer parameter
*
* @param tkconfigs     A list of tracking configuration
*
* @return              true if the setting is successfull
*/
//------------------------------------------------------------------------------
bool StatisticAcceptFilter::SetTrackingConfigs(StringArray tkconfigs)
{
   tkConfigList = tkconfigs;
   return true;
}


//------------------------------------------------------------------------------
// ObservationData* FilteringData(ObservationData* dataObject, Integer& rejectedReason)
//------------------------------------------------------------------------------
/**
* This function is used to filter data
*
* @param dataObject        An input data record
* @param rejectedReason    Reason the record is rejected 
*
* @return                  NULL if the input data record is rejected, 
*                          the input data record otherwise 
*/
//------------------------------------------------------------------------------
ObservationData* StatisticAcceptFilter::FilteringData(ObservationData* dataObject, Integer& rejectedReason)
{
#ifdef DEBUG_DATA_FILTER
   MessageInterface::ShowMessage("StatisticAcceptFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) enter\n", dataObject, rejectedReason);
#endif
   // 0. FileNames verify: data is rejected when file name list is empty
   bool pass0 = true;
   rejectedReason = 0;   // 0: no rejected due to accept all spacecrafts
   if (fileNames.empty())
   {
      pass0 = false;
      rejectedReason = 8;   // 8: rejected due to file name list is empty
   }

   if (!pass0)
   {
      #ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("StatisticAcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit6 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
      #endif
      return NULL;             // return NULL when it does not pass the test. The value of rejectedReason has to be 8 
   }

   // Filter data using tracking configs
   bool pass5 = true;
   rejectedReason = 0;
   if (find(fileNames.begin(), fileNames.end(), "From_AddTrackingConfig") != fileNames.end())
   {
      // Filter data based on tracking configs when tracking configs are not generated automatically
      if (tkConfigList.size() != 0)
      {
         bool pass5 = false;
         rejectedReason = 9;         // 9: reject due to tracking configuration 
         for(UnsignedInt i = 0; i < tkConfigList.size(); ++i)
         {
            if (dataObject->GetTrackingConfig() == tkConfigList[i])
            {
               pass5 = true;
               rejectedReason = 0;
               break;
            }
         }
         if (!pass5)
         {
            #ifdef DEBUG_FILTER
               MessageInterface::ShowMessage("StatisticAcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit7 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
            #endif
            return NULL;
         }
      }
   }
   else
   {
      if (!HasFile(dataObject))
      {
         #ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("StatisticAcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit6.1 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
         #endif

         rejectedReason = 8;
         return NULL;
      }
   }

   // 1. Observated objects verify: It will be passed the test when observation data contains one spacecraft in "observers" array
   if (!HasObserver(dataObject))
   {
      rejectedReason = 6;          // 6: rejected due to spacecraft is not found
      #ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("StatisticAcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit1 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
      #endif
      return NULL;                 // return NULL when it does not have any observer listing in DataFilter object. The value of rejectedReason has to be 6 
   }


   // 2. Trackers verify: It will be passed the test when observation data contains one ground station in "trackers" array
   if (!HasTracker(dataObject))
   {
      rejectedReason = 5;        // 5: rejected due to ground station is not found
      #ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("StatisticAcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit2 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
      #endif
      return NULL;             // return NULL when it does not have any tracker listing in DataFilter object. The value of rejectedReason has to be 5 
   }

   // 3. Measurement type verify: It will be passed the test when data type of observation data is found in "dataTypes" array
   if (!HasDataType(dataObject))
   {
      rejectedReason = 7;        // 7: rejected due to data type of observation data is not found
      #ifdef DEBUG_FILTER
         MessageInterface::ShowMessage("StatisticAcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit3 return NULL\n", GetName().c_str(), this, dataObject, rejectedReason);
      #endif
      return NULL;             // return NULL when it does not have any data type listing in DataFilter object. The value of rejectedReason has to be 7
   }

   // Strands verify:
   // 4. Time interval verify:
   if (!IsInTimeWindow(dataObject))
   {
      rejectedReason = 2;      // 2: rejected due to time span
      #ifdef DEBUG_FILTER
         GmatEpoch currentEpoch = TimeConverterUtil::Convert(dataObject->epoch, dataObject->epochSystem, TimeConverterUtil::A1MJD);
         MessageInterface::ShowMessage(" currentEpoch = %.12lf    epochStart = %.12lf     epochEnd = %.12lf\n", currentEpoch, epochStart, epochEnd);
         MessageInterface::ShowMessage("StatisticAcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit4 return NULL  time out of range\n", GetName().c_str(), this, dataObject, rejectedReason);
      #endif
      return NULL;             // return NULL when measurement epoch is not in time window. The value of rejectedReason has to be 2 
   }

   // 5. Data thin verify:
   if (!IsThin(dataObject))
   {
      rejectedReason = 1;    // 1: reject due to thinning ratio
      #ifdef DEBUG_FILTER
         //MessageInterface::ShowMessage("StatisticAcceptFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) exit5  return NULL   recCount = %d   thinningFrequency = %d\n", dataObject, rejectedReason, recCount, thinningFrequency);
         MessageInterface::ShowMessage("StatisticAcceptFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) exit5  return NULL   thinningFrequency = %d\n", dataObject, rejectedReason, thinningFrequency);
      #endif
      return NULL;
   }


   #ifdef DEBUG_FILTER
      MessageInterface::ShowMessage("StatisticAcceptFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit0 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
   #endif
   return dataObject;
}
   

//------------------------------------------------------------------------------
// StringArray GetAllAvailableThinModes()
//------------------------------------------------------------------------------
/**
* Retrieves a list of all available thin modes
*
* @return        A list of all available thin modes
*/
//------------------------------------------------------------------------------
StringArray StatisticAcceptFilter::GetAllAvailableThinModes()
{
   StringArray nameList;
   nameList.push_back("Frequency");           // data thinning is applied based on record count
   nameList.push_back("Time");                // data thinning is applied based on time range

   return nameList;
}


//------------------------------------------------------------------------------
// bool IsThin(ObservationData* dataObject)
//------------------------------------------------------------------------------
/**
* Verify a data record to be satisfied thinning criteria
*
* @return        true if it is satisfied, false otherwise
*/
//------------------------------------------------------------------------------
bool StatisticAcceptFilter::IsThin(ObservationData* dataObject)
{
   bool isAccepted = false;
   // 1. Get tracking config from observation data 
   std::string trackingConfig = dataObject->GetTrackingConfig();

   if (thinMode == "Frequency")
   {
      // 2. If tracking config is not in recCountMap, set record count to 0 for that tracking config
      bool found = false;
      for (std::map<std::string, Integer>::iterator i = recCountMap.begin(); i != recCountMap.end(); ++i)
      {
         if (i->first == trackingConfig)
         {
            found = true;
            break;
         }
      }
      if (!found)
         recCountMap[trackingConfig] = 0;

      // 3. Book keeping record count for this tracking config 
      if (recCountMap[trackingConfig] == (thinningFrequency - 1))
      {
         isAccepted = true;
         recCountMap[trackingConfig] = 0;
      }
      else
      {
         ++recCountMap[trackingConfig];
      }
   }
   else if (thinMode == "Time")
   {
      // 2. If tracking config is not in startTimeWindowMap, set start time window to epochStart for that tracking config
      bool found = false;
      for (std::map<std::string,GmatEpoch>::iterator i = startTimeWindowMap.begin(); i != startTimeWindowMap.end(); ++i)
      {
         if (i->first == trackingConfig)
         {
            found = true;
            break;
         }
      }
      if (!found)
         startTimeWindowMap[trackingConfig] = epochStart;

      if (dataObject->epoch > startTimeWindowMap[trackingConfig])
      {
         isAccepted = true;
         Real step = GmatMathUtil::Floor((dataObject->epoch - startTimeWindowMap[trackingConfig]) * GmatTimeConstants::SECS_PER_DAY / thinningFrequency);
         startTimeWindowMap[trackingConfig] = startTimeWindowMap[trackingConfig] + (step + 1)*(thinningFrequency / GmatTimeConstants::SECS_PER_DAY);
      }
   }
   else
      throw MeasurementException("Error: " + GetName() + ".ThinMode parameter has an invalid value ('" + thinMode + "'.\n");

   return isAccepted;
}
