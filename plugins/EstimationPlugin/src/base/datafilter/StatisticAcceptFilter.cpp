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
#include <sstream>


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string StatisticAcceptFilter::PARAMETER_TEXT[] =
{
   "Filenames",
   "DataThin",
};

const Gmat::ParameterType StatisticAcceptFilter::PARAMETER_TYPE[] =
{
   Gmat::STRINGARRAY_TYPE,			// FILE_NAMES
   Gmat::STRING_TYPE,			   // DATATHIN
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
   dataThin          ("F1")
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
   fileNames             (saf.fileNames),
   dataThin              (saf.dataThin)
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

      fileNames    = saf.fileNames;
      dataThin     = saf.dataThin;
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
// bool Finalize()
//------------------------------------------------------------------------------
/**
 * Code that executes after a run completes
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool StatisticAcceptFilter::Finalize()
{
   bool retval = false;

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
   if (id == DATA_THIN)
      return dataThin;

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
   if (id == DATA_THIN)
   {
      dataThin = value;
      return true;
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
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from a StringArray object
 *
 * @param id      The ID of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string StatisticAcceptFilter::GetStringParameter(const Integer id, const Integer index) const
{
   if (id == FILENAMES)
   {
      if ((index < 0)||(index >= (Integer)fileNames.size()))
      {
         std::stringstream ss;
         ss << "Error: file name's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return fileNames[index];
   }

   return DataFilter::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value to a property of StringArray type
 *
 * @param id      The ID of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return true if the setting successes and false otherwise
 */
//------------------------------------------------------------------------------
bool StatisticAcceptFilter::SetStringParameter(const Integer id, const std::string &value,
                                           const Integer index)
{
   if (id == FILENAMES)
   {
      if ((0 <= index)&&(index < (Integer)fileNames.size()))
      {
         fileNames[index] = value;
         return true;
      }
      else
      {
         if (index == fileNames.size())
         {
            fileNames.push_back(value);
            return true;
         }
         else
         {
            std::stringstream ss;
            ss << "Error: file name's index (" << index << ") is out of bound.\n"; 
            throw GmatBaseException(ss.str());
         }
      }
   }

   return DataFilter::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from a StringArray object
 *
 * @param labe    The name of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string StatisticAcceptFilter::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value to a property of StringArray type
 *
 * @param label   The name of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return true if the setting successes and false otherwise
 */
//------------------------------------------------------------------------------
bool StatisticAcceptFilter::SetStringParameter(const std::string &label, const std::string &value,
                                           const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const std::string GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray property
 *
 * @param id The ID of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
const StringArray& StatisticAcceptFilter::GetStringArrayParameter(const Integer id) const
{
   if (id == FILENAMES)
      return fileNames;

   return DataFilter::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const std::string GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray property
 *
 * @param label   The name of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
const StringArray& StatisticAcceptFilter::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


#include "ObservationData.hpp"
GmatBase* StatisticAcceptFilter::FilteringData(GmatBase* dataObject)
{
   if (dataObject == NULL)
      return dataObject;

   ObservationData* measData = (ObservationData*)dataObject;

   
   // File verify:

   // Observated objects verify:
   bool pass1 = false;
   for(UnsignedInt i = 0; i < observers.size(); ++i)
   {
      for(UnsignedInt j = 1; j < measData->participantIDs.size(); ++j)
      {
         if (observers[i] == measData->participantIDs[j])
         {
            pass1 = true;
            break;
         }
      }

      if (pass1)
         break;
   }

   // Trackers verify:

   // Strands verify:
   // Time interval verify:


   // Data thin verify:

   return dataObject;
}
   

   
