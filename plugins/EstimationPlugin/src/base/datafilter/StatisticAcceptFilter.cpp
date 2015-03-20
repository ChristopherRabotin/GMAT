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
   thinMode          ("F"),
   thinningFrequency (1),
   recCount          (0)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("StatisticAcceptFilter default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::DATA_FILTER);
   objectTypeNames.push_back("StatisticAcceptFilter");

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
   thinningFrequency     (saf.thinningFrequency),
   recCount              (0)
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
      recCount          = 0;
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

      recCount = 0;
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
   if (id == THIN_MODE)
      return true;

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


Integer StatisticAcceptFilter::GetIntegerParameter(const Integer id) const
{
   if (id == THINNING_FREQUENCY)
      return thinningFrequency;

   return DataFilter::GetIntegerParameter(id);
}


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


Integer StatisticAcceptFilter::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


Integer StatisticAcceptFilter::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


ObservationData* StatisticAcceptFilter::FilteringData(ObservationData* dataObject, Integer& rejectedReason)
{
#ifdef DEBUG_DATA_FILTER
   MessageInterface::ShowMessage("StatisticAcceptFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) enter\n", dataObject, rejectedReason);
#endif
   Integer reject = 0; 
   if (DataFilter::FilteringData(dataObject, reject) == NULL)
   {
      rejectedReason = reject;
#ifdef DEBUG_DATA_FILTER
   MessageInterface::ShowMessage("StatisticAcceptFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) exit1   return NULL\n", dataObject, rejectedReason);
#endif
      return NULL;
   }

   // 1. Data thin verify:
   if (thinMode == "F")
   {
      //++recCount;                        // record count will be done for all accept filters not for only one 
      if (recCount == (thinningFrequency-1))
      {
         // recCount = 0;
         rejectedReason = 0;
#ifdef DEBUG_DATA_FILTER
   MessageInterface::ShowMessage("StatisticAcceptFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) exit3  return <%p>\n", dataObject, rejectedReason, dataObject);
#endif
         return dataObject;
      }
      else
      {
         rejectedReason = 1;    // 1: reject due to thinning ratio
#ifdef DEBUG_DATA_FILTER
   MessageInterface::ShowMessage("StatisticAcceptFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) exit2  return NULL   recCount = %d   thinningFrequency = %d\n", dataObject, rejectedReason, recCount, thinningFrequency);
#endif
         return NULL;
      }
   }
   else
      throw MeasurementException("Error: " + GetName() + ".ThinMode parameter has an invalid value ('" + thinMode + "'.\n");
}
   

StringArray StatisticAcceptFilter::GetAllAvailableThinModes()
{
   StringArray nameList;
   nameList.push_back("F");

   return nameList;
}


void StatisticAcceptFilter::IncreasingRecordCounter()
{
   ++recCount;
   if (recCount == thinningFrequency)
      recCount = 0;
}