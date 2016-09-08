//$Id: StatisticRejectFilter.cpp 1398 2015-03-03 14:10:37Z tdnguyen $
//------------------------------------------------------------------------------
//                                 StatisticRejectFilter
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
 * Implementation for the StatisticRejectFilter class
 */
//------------------------------------------------------------------------------


#include "StatisticRejectFilter.hpp"
#include "GmatBase.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include <sstream>


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION
//#define DEBUG_FILTER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//const std::string StatisticRejectFilter::PARAMETER_TEXT[] =
//{
//};
//
//const Gmat::ParameterType StatisticRejectFilter::PARAMETER_TYPE[] =
//{
//};



//------------------------------------------------------------------------------
// StatisticRejectFilter(const std::string name)
//------------------------------------------------------------------------------
/**
 * Constructor for StatisticRejectFilter objects
 *
 * @param name The name of the object
 */
//------------------------------------------------------------------------------
StatisticRejectFilter::StatisticRejectFilter(const std::string name) :
   DataFilter        (name)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("StatisticRejectFilter default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::DATA_FILTER);
   objectTypeNames.push_back("StatisticsRejectFilter");

   parameterCount = StatisticRejectFilterParamCount;

   //finalEpoch = initialEpoch;
   //epochEnd   = epochStart;
}


//------------------------------------------------------------------------------
// ~StatisticRejectFilter()
//------------------------------------------------------------------------------
/**
 * StatisticRejectFilter destructor
 */
//------------------------------------------------------------------------------
StatisticRejectFilter::~StatisticRejectFilter()
{
}


//------------------------------------------------------------------------------
// StatisticRejectFilter(const StatisticRejectFilter& srf)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a StatisticRejectFilter
 *
 * @param srf        The StatisticRejectFilter object that provides data for the new one
 */
//------------------------------------------------------------------------------
StatisticRejectFilter::StatisticRejectFilter(const StatisticRejectFilter& srf) :
   DataFilter            (srf)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("StatisticRejectFilter copy constructor from <%s,%p>  to  <%s,%p>\n", srf.GetName().c_str(), &srf, GetName().c_str(), this);
#endif

}


//------------------------------------------------------------------------------
// StatisticRejectFilter& operator=(const StatisticRejectFilter& srf)
//------------------------------------------------------------------------------
/**
 * StatisticRejectFilter assignment operator
 *
 * @param srf    The StatisticRejectFilter object that provides data for the this one
 *
 * @return This object, configured to match saf
 */
//------------------------------------------------------------------------------
StatisticRejectFilter& StatisticRejectFilter::operator=(const StatisticRejectFilter& srf)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("StatisticRejectFilter operator = <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &srf)
   {
      DataFilter::operator=(srf);

      //@todo: set value for parameters here
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone method for StatisticRejectFilter
 *
 * @return A clone of this object.
 */
//------------------------------------------------------------------------------
GmatBase* StatisticRejectFilter::Clone() const
{
   return new StatisticRejectFilter(*this);
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
bool StatisticRejectFilter::Initialize()
{
#ifdef DEBUG_INITIALIZATION
	MessageInterface::ShowMessage("StatisticRejectFilter<%s,%p>::Initialize()   entered\n", GetName().c_str(), this);
#endif

   bool retval = false;
   if (DataFilter::Initialize())
   {
      //@todo: Initialize code is here

      isInitialized = retval;
   }

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("StatisticRejectFilter<%s,%p>::Initialize()   exit\n", GetName().c_str(), this);
#endif
   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script a StatisticRejectFilter property
 *
 * @param id The ID of the property
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string StatisticRejectFilter::GetParameterText(const Integer id) const
{
   if (id >= DataFilterParamCount && id < StatisticRejectFilterParamCount)
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
std::string StatisticRejectFilter::GetParameterUnit(const Integer id) const
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
Integer StatisticRejectFilter::GetParameterID(const std::string &str) const
{
   for (Integer i = DataFilterParamCount; i < StatisticRejectFilterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - DataFilterParamCount])
         return i;
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
Gmat::ParameterType StatisticRejectFilter::GetParameterType(const Integer id) const
{
   if (id >= DataFilterParamCount && id < StatisticRejectFilterParamCount)
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
std::string StatisticRejectFilter::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
bool StatisticRejectFilter::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == FILENAMES)
   {
      if (value == "From_AddTrackingConfig")
         throw MeasurementException("Error: 'From_AddTrackingConfig' is an invalid value for " + GetName() + ".FileNames parameter.\n");  
   }

   return DataFilter::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
* Sets an element of a string array property
*
* @param id The ID of the property
* @param value The new value
* @param index The index of a given element in a string array
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool StatisticRejectFilter::SetStringParameter(const Integer id, 
   const std::string &value, const Integer index)
{
   if (id == FILENAMES)
   {
      if (value == "From_AddTrackingConfig")
         throw MeasurementException("Error: 'From_AddTrackingConfig' is an invalid value for " + GetName() + ".FileNames parameter.\n");
   }

   return DataFilter::SetStringParameter(id, value, index);
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
bool StatisticRejectFilter::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
* Sets an element of a string array property
*
* @param label The text description of the property
* @param value The new value
* @param index The index of a given element in a string array
*
* @return true on success, false on failure
*/
//------------------------------------------------------------------------------
bool StatisticRejectFilter::SetStringParameter(const std::string &label, 
   const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


bool StatisticRejectFilter::SetTrackingConfigs(StringArray tkconfigs)
{
   tkConfigList = tkconfigs;
   return true;
}


ObservationData* StatisticRejectFilter::FilteringData(ObservationData* dataObject, Integer& rejectedReason)
{
#ifdef DEBUG_FILTER
   MessageInterface::ShowMessage("StatisticRejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d)  enter\n", GetName().c_str(), this, dataObject, rejectedReason);
#endif
   
   rejectedReason = 0;             // no reject

   // 0. File name verify: It will be passed the test when observation data does not contain any file name in "FileNames" array
   if (!HasFile(dataObject))
   {
#ifdef DEBUG_FILTER
      MessageInterface::ShowMessage("StatisticRejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit11 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
#endif
      return dataObject;             // return dataObject when name of the file specified in dataObject does not match to any file in FileNames list. The value of rejectedReason has to be 0 
   }


   // 1. Observated objects verify: It will be passed the test when observation data does not contain any spacecraft in "observers" array
   if (!HasObserver(dataObject))
   {
      #ifdef DEBUG_FILTER
          MessageInterface::ShowMessage("StatisticRejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit1 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
      #endif
      return dataObject;             // return dataObject when it does not have any spacecraft matching to observers list. The value of rejectedReason has to be 0 
   }

   // 2. Trackers verify: It will be passed the test when observation data contains one ground station in "trackers" array
   if (!HasTracker(dataObject))
   {
      #ifdef DEBUG_FILTER
          MessageInterface::ShowMessage("StatisticRejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit2 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
      #endif
      return dataObject;             // return dataObject when it does not have any spacecraft matching to observers list. The value of rejectedReason has to be 0 
   }

   

   // 3. Measurement type verify: It will be passed the test when data type of observation data is found in "dataTypes" array
   if (!HasDataType(dataObject))
   {
      #ifdef DEBUG_FILTER
          MessageInterface::ShowMessage("StatisticRejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit3 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
      #endif
      return dataObject;             // return dataObject when it does not have any spacecraft matching to observers list. The value of rejectedReason has to be 0 
   }

   // Strands verify:

   // Time interval verify:
   if (!IsInTimeWindow(dataObject))
   {
      #ifdef DEBUG_FILTER
          MessageInterface::ShowMessage("StatisticRejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit4 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
      #endif
      return dataObject;             // return dataObject when it does not have any spacecraft matching to observers list. The value of rejectedReason has to be 0 
   }

   rejectedReason = 100;             // reject due to reject filter
   dataObject = NULL;
   #ifdef DEBUG_FILTER
      MessageInterface::ShowMessage("StatisticRejectFilter<%s,%p>::FilteringData(dataObject = <%p>, rejectedReason = %d) exit0 return <%p>\n", GetName().c_str(), this, dataObject, rejectedReason, dataObject);
   #endif

   return dataObject;
}
   

