//$Id: TrackingData.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             TrackingData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/02/19 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the TrackingData class
 */
//------------------------------------------------------------------------------


#include "TrackingData.hpp"
#include "MeasurementException.hpp"

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------

/// Script labels for the tracking data properties
const std::string
TrackingData::PARAMETER_TEXT[TrackingDataParamCount - GmatBaseParamCount] =
   {
      "Type",
      "Participants"
   };

/// Tracking data property types
const Gmat::ParameterType
TrackingData::PARAMETER_TYPE[TrackingDataParamCount - GmatBaseParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRINGARRAY_TYPE,
   };


//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TrackingData(const std::string &name) :
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the new tracking system
 */
//------------------------------------------------------------------------------
TrackingData::TrackingData(const std::string &name) :
   GmatBase             (Gmat::TRACKING_DATA, "TrackingData", name),
   trackingType         ("")
{
   objectTypes.push_back(Gmat::TRACKING_DATA);
   objectTypeNames.push_back("TrackingData");
}


//------------------------------------------------------------------------------
// ~TrackingData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TrackingData::~TrackingData()
{
   // TODO Auto-generated destructor stub
}


//------------------------------------------------------------------------------
// TrackingData(const TrackingData & td)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param td The TrackingData object that gets copied here
 */
//------------------------------------------------------------------------------
TrackingData::TrackingData(const TrackingData & td) :
   GmatBase             (td),
   trackingType         (td.trackingType),
   participantNames     (td.participantNames)
{
}


//------------------------------------------------------------------------------
// TrackingData& operator=(const TrackingData & td)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param td The TrackingData object that gets provides data for this one
 *
 * @return This TrackingData object, configured to match td
 */
//------------------------------------------------------------------------------
TrackingData& TrackingData::operator=(const TrackingData & td)
{
   if (this != &td)
   {
      GmatBase::operator=(td);

      trackingType     = td.trackingType;
      participantNames = td.participantNames;
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this TrackingData object
 *
 * @return The new TrackingData object that looks just like this one
 */
//------------------------------------------------------------------------------
GmatBase* TrackingData::Clone() const
{
   return new TrackingData(*this);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a scriptable property of this object
 *
 * @param str The script string used for the property
 *
 * @return The property's ID
 */
//------------------------------------------------------------------------------
Integer TrackingData::GetParameterID(const std::string & str) const
{
   for (Integer i = GmatBaseParamCount; i < TrackingDataParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Finds and reports the type of a scriptable property
 *
 * @param id The ID of the property
 *
 * @return The enumerated type of the property
 */
//------------------------------------------------------------------------------
Gmat::ParameterType TrackingData::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < TrackingDataParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Returns a string describing the type of the property with thd input ID
 *
 * @param id The ID of the property
 *
 * @return The property's type
 */
//------------------------------------------------------------------------------
std::string TrackingData::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script the property with the input ID
 *
 * @param id The ID of the property
 *
 * @return The text string
 */
//------------------------------------------------------------------------------
std::string TrackingData::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < TrackingDataParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units used for a property
 *
 * @param id The ID of the property
 *
 * @return The string describing the units of the property
 *
 * @note GMAT does not yet support units
 */
//------------------------------------------------------------------------------
std::string TrackingData::GetParameterUnit(const Integer id) const
{
   return GmatBase::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string based property
 *
 * @param id The property's ID
 *
 * @return The property
 */
//------------------------------------------------------------------------------
std::string TrackingData::GetStringParameter(const Integer id) const
{
   if (id == TYPE)
      return trackingType;

   return GmatBase::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value)
//------------------------------------------------------------------------------
/**
 * Sets a string based property
 *
 * @param id The property's ID
 * @param value The value of the property
 *
 * @return true if the property was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingData::SetStringParameter(const Integer id,
         const std::string& value)
{
   switch (id)
   {
      case TYPE:
         trackingType = value;
         return true;

      case PARTICIPANTS:
         if (find(participantNames.begin(), participantNames.end(), value) ==
                  participantNames.end())
            participantNames.push_back(value);

         return true;

      default:
         break;
   }


   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from a StringArray
 *
 * @param id The property ID
 * @param index The index of the specific string requested
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string TrackingData::GetStringParameter(const Integer id,
         const Integer index) const
{
   switch (id)
   {
      case PARTICIPANTS:
         if ((index >= 0) && ((Integer)participantNames.size() > index))
            return participantNames[index];
         break;

      default:
         break;
   }

   return GmatBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value,
//          const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string property in a StringArray
 *
 * @param id The property ID
 * @param value The value of the entry
 * @param index The index of the specific string being set or changed
 *
 * @return true if the property was changes, false if not
 */
//------------------------------------------------------------------------------
bool TrackingData::SetStringParameter(const Integer id,
         const std::string & value, const Integer index)
{
   switch (id)
   {
      case PARTICIPANTS:
         if (index >= 0)
         {
            // Only add if it is not in the list, regardless of index
            if (find(participantNames.begin(), participantNames.end(),
                     value) == participantNames.end())
            {
               if (index == (Integer)participantNames.size())
                  participantNames.push_back(value);
               else
               {
                  if (index > (Integer)participantNames.size())
                     throw MeasurementException("Attempting to add tracking "
                              "data participant outside of the allowed range "
                              "of the data array");
                  participantNames[index] = value;
               }
               return true;
            }

            // Make the addition idempotent
            if (index < (Integer)participantNames.size())
               if (participantNames[index] == value)
                  return true;
         }
         break;

      default:
         break;
   }

   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of properties contained in a StringArray
 *
 * @param id The ID of the StringArray
 *
 * @return The array
 */
//------------------------------------------------------------------------------
const StringArray& TrackingData::GetStringArrayParameter(const Integer id) const
{
   switch (id)
   {
      case PARTICIPANTS:
         return participantNames;

      default:
         break;
   }

   return GmatBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//          const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of properties contained in an array of StringArrays
 *
 * @param id The ID of the StringArray
 * @param index The index of the StringArray in the containing array
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& TrackingData::GetStringArrayParameter(const Integer id,
         const Integer index) const
{
   return GmatBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from the object
 *
 * @param label Script string used to identify the property
 *
 * @return The property
 */
//------------------------------------------------------------------------------
std::string TrackingData::GetStringParameter(const std::string & label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// std::string SetStringParameter(const std::string & label,
//          const std::string & value)
//------------------------------------------------------------------------------
/**
 * Sets a string property in a StringArray from the object
 *
 * @param label Script string used to identify the property
 * @param value The value of the property
 *
 * @return true if the property was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingData::SetStringParameter(const std::string & label,
         const std::string & value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label,
//          const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property in a StringArray from the object
 *
 * @param label Script string used to identify the property
 * @param index The index of the property in the StringArray
 *
 * @return The property
 */
//------------------------------------------------------------------------------
std::string TrackingData::GetStringParameter(const std::string & label,
         const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// std::string SetStringParameter(const std::string & label,
//          const std::string & value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string property in a StringArray from the object
 *
 * @param label Script string used to identify the property
 * @param value The value of the property in the StringArray
 * @param index The index of the string in the array
 *
 * @return true if the property was set, false if not
 */
//------------------------------------------------------------------------------
bool TrackingData::SetStringParameter(const std::string & label,
         const std::string & value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string& label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of properties contained in a StringArray
 *
 * @param label Script string used to identify the property
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray & TrackingData::GetStringArrayParameter(
         const std::string & label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string & label,
//          const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a list of properties contained in an array of StringArrays
 *
 * @param label Script string used to identify the property
 * @param index The index of the StringArray in the containing array
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& TrackingData::GetStringArrayParameter(
         const std::string & label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}

bool TrackingData::Initialize()
{
   bool retval = false;

   return retval;
}




