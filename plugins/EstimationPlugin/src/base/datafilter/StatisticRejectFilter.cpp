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
//#define DEBUG_DATA_FILTER

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
   objectTypeNames.push_back("StatisticRejectFilter");

   parameterCount = StatisticRejectFilterParamCount;
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
// StatisticRejectFilter(const StatisticRejectFilter& saf)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a StatisticRejectFilter
 *
 * @param saf        The StatisticRejectFilter object that provides data for the new one
 */
//------------------------------------------------------------------------------
StatisticRejectFilter::StatisticRejectFilter(const StatisticRejectFilter& saf) :
   DataFilter            (saf)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("StatisticRejectFilter copy constructor from <%s,%p>  to  <%s,%p>\n", saf.GetName().c_str(), &saf, GetName().c_str(), this);
#endif

}


//------------------------------------------------------------------------------
// StatisticRejectFilter& operator=(const StatisticRejectFilter& saf)
//------------------------------------------------------------------------------
/**
 * StatisticRejectFilter assignment operator
 *
 * @param saf    The StatisticRejectFilter object that provides data for the this one
 *
 * @return This object, configured to match saf
 */
//------------------------------------------------------------------------------
StatisticRejectFilter& StatisticRejectFilter::operator=(const StatisticRejectFilter& saf)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("StatisticRejectFilter operator = <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &saf)
   {
      DataFilter::operator=(saf);

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


ObservationData* StatisticRejectFilter::FilteringData(ObservationData* dataObject, Integer& rejectedReason)
{
#ifdef DEBUG_DATA_FILTER
   MessageInterface::ShowMessage("StatisticRejectFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) enter\n", dataObject, rejectedReason);
#endif
   Integer reject = 0;
   if (DataFilter::FilteringData(dataObject, reject) == NULL)
   {
      rejectedReason = reject;
#ifdef DEBUG_DATA_FILTER
   MessageInterface::ShowMessage("StatisticRejectFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) enter\n", dataObject, rejectedReason);
#endif
      return dataObject;

      //@todo: Add code to filter additional data here
   }

   rejectedReason = reject;
#ifdef DEBUG_DATA_FILTER
   MessageInterface::ShowMessage("StatisticRejectFilter::FilteringData(dataObject = <%p>, rejectedReason = %d) enter\n", dataObject, rejectedReason);
#endif
   return NULL;
}
   

