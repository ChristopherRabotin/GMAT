//$Id$
//------------------------------------------------------------------------------
//                           EventLocator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 6, 2011
//
/**
 * Implementation of the the event locator base class
 */
//------------------------------------------------------------------------------

#include <stdio.h>
#include <iostream>

#include "EventLocator.hpp"
#include "EventException.hpp"
#include "Spacecraft.hpp"
#include "FileManager.hpp"      // for GetPathname()
#include "FileUtil.hpp"
#include "GmatConstants.hpp"
#include "StringUtil.hpp"
#include "TimeSystemConverter.hpp"
#include "GregorianDate.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_DUMPEVENTDATA
//#define DEBUG_EVENTLOCATION
//#define DEBUG_EVENT_INITIALIZATION
//#define DEBUG_OBJECTS
//#define DEBUG_DATE_FORMAT

#ifdef DEBUG_DUMPEVENTDATA
   #include <fstream>
   std::ofstream dumpfile("LocatorData.txt");
#endif

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
EventLocator::PARAMETER_TEXT[EventLocatorParamCount - GmatBaseParamCount] =
{
   "Spacecraft",           // SATNAME
   "Filename",             // EVENT_FILENAME
   "OccultingBodies",      // OCCULTING_BODIES
   "InputEpochFormat",     // INPUT_EPOCH_FORMAT
   "InitialEpoch",         // INITIAL_EPOCH
   "StepSize",             // STEP_SIZE
   "FinalEpoch",           // FINAL_EPOCH
   "UseLightTimeDelay",    // USE_LIGHT_TIME_DELAY
   "UseStellarAberration", // USE_STELLAR_ABERRATION
   "WriteReport",          // WRITE_REPORT
   "UseEntireInterval",    // USE_ENTIRE_INTERVAL
//   "AppendToReport",       // APPEND_TO_REPORT *** this may be an input to the FindEvents command instead
};

const Gmat::ParameterType
EventLocator::PARAMETER_TYPE[EventLocatorParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECT_TYPE,      // SATNAME
   Gmat::FILENAME_TYPE,    // EVENT_FILENAME
   Gmat::OBJECTARRAY_TYPE, // OCCULTING_BODIES
   Gmat::STRING_TYPE,      // INPUT_EPOCH_FORMAT
   Gmat::STRING_TYPE,      // INITIAL_EPOCH
   Gmat::REAL_TYPE,        // STEP_SIZE
   Gmat::STRING_TYPE,      // FINAL_EPOCH
   Gmat::BOOLEAN_TYPE,     // USE_LIGHT_TIME_DELAY
   Gmat::BOOLEAN_TYPE,     // USE_STELLAR_ABERRATION
   Gmat::BOOLEAN_TYPE,     // WRITE_REPORT
   Gmat::BOOLEAN_TYPE,     // USE_ENTIRE_INTERVAL
//   Gmat::BOOLEAN_TYPE,     // APPEND_TO_REPORT *** this may be an input to the FindEvents command instead
};


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// EventLocator(const std::string &typeStr, const std::string &nomme) :
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param typeStr Type name of the locator
 * @param nomme Name of this instance of the locator
 */
//------------------------------------------------------------------------------
EventLocator::EventLocator(const std::string &typeStr,
                           const std::string &nomme) :
   GmatBase                (Gmat::EVENT_LOCATOR, typeStr, nomme),
   filename                (""),
   fileWasWritten          (false),
   useLightTimeDelay       (false),
   useStellarAberration    (false),
   writeReport             (true),
   useEntireInterval       (true),
   appendReport            (false),
   epochFormat             ("UTCGregorian"),
   initialEpoch            ("01 Jan 2000 11:59:28.000"),
   finalEpoch              ("01 Jan 2000 14:59:28.000"),
   stepSize                (60.0),
   initialEp               (0.0),
   finalEp                 (0.0),
   satStartEpoch           (0.0),  // review these
   fromEpoch               (0.0),
   toEpoch                 (0.0),
   numEventsFound          (0),
   satName                 (""),
   sat                     (NULL),
   solarSys                (NULL)
{
   objectTypes.push_back(Gmat::EVENT_LOCATOR);
   objectTypeNames.push_back("EventLocator");

   filename = instanceName + ".txt";  // default filename

   occultingBodyNames.clear();
   occultingBodies.clear();
}


//------------------------------------------------------------------------------
// ~EventLocator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EventLocator::~EventLocator()
{
   #ifdef DEBUG_EVENT_INITIALIZATION
      MessageInterface::ShowMessage("Deleting event locator %s <%p>\n",
            instanceName.c_str(), this);
   #endif

   if (!occultingBodyNames.empty())
      occultingBodyNames.clear();
   if (!occultingBodies.empty())
      occultingBodies.clear();
}


//------------------------------------------------------------------------------
// EventLocator(const EventLocator& el)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param el The locator being copied here
 */
//------------------------------------------------------------------------------
EventLocator::EventLocator(const EventLocator& el) :
   GmatBase                (el),
   filename                (el.filename),
   fileWasWritten          (false),
   useLightTimeDelay       (el.useLightTimeDelay),
   useStellarAberration    (el.useStellarAberration),
   writeReport             (el.writeReport), // or true?
   useEntireInterval       (el.useEntireInterval),
   appendReport            (el.appendReport),
   epochFormat             (el.epochFormat),
   initialEpoch            (el.initialEpoch),
   finalEpoch              (el.finalEpoch),
   stepSize                (el.stepSize),
   initialEp               (el.initialEp),
   finalEp                 (el.finalEp),
   satStartEpoch           (el.satStartEpoch),
   fromEpoch               (el.fromEpoch),
   toEpoch                 (el.toEpoch),
   numEventsFound          (el.numEventsFound),   // or 0?
   satName                 (el.satName),
   sat                     (NULL),
   solarSys                (el.solarSys)
{
   occultingBodyNames.clear();
   occultingBodies.clear();

   UnsignedInt sz = occultingBodyNames.size();
   for (UnsignedInt ii = 0; ii < sz; ii++)
      occultingBodyNames.push_back(el.occultingBodyNames.at(ii));

   isInitialized    = false;
}


//------------------------------------------------------------------------------
// EventLocator& EventLocator::operator=(const EventLocator& el)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param el The locator being copied here
 *
 * @return this instance, set to match el.
 */
//------------------------------------------------------------------------------
EventLocator& EventLocator::operator=(const EventLocator& el)
{
   if (this != &el)
   {
      GmatBase::operator =(el);

      filename             = el.filename;
      fileWasWritten       = el.fileWasWritten;

      useLightTimeDelay    = el.useLightTimeDelay;
      useStellarAberration = el.useStellarAberration;
      writeReport          = el.writeReport;
      useEntireInterval    = el.useEntireInterval;
      appendReport         = el.appendReport;
      epochFormat          = el.epochFormat;
      initialEpoch         = el.initialEpoch;
      finalEpoch           = el.finalEpoch;
      stepSize             = el.stepSize;
      initialEp            = el.initialEp;
      finalEp              = el.finalEp;
      satStartEpoch        = el.satStartEpoch;
      fromEpoch            = el.fromEpoch;
      toEpoch              = el.toEpoch;
      numEventsFound       = el.numEventsFound;
      satName              = el.satName;
      sat                  = NULL;
      solarSys             = el.solarSys;

      occultingBodyNames.clear();
      occultingBodies.clear();

      UnsignedInt sz = occultingBodyNames.size();
      for (UnsignedInt ii = 0; ii < sz; ii++)
         occultingBodyNames.push_back(el.occultingBodyNames.at(ii));

      sz = occultingBodies.size();
      for (UnsignedInt ii = 0; ii < sz; ii++)
         occultingBodies.push_back(el.occultingBodies.at(ii));
   }

   return *this;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text description of the object parameter with input id
 *
 * @param id The ID for the parameter
 *
 * @return The string used to script the parameter
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EventLocatorParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter ID associated with a string
 *
 * @param str The string associated with the ID
 *
 * @return The ID
 */
//------------------------------------------------------------------------------
Integer EventLocator::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < EventLocatorParamCount; i++)
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
 * Retrieves the type of a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType EventLocator::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EventLocatorParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a text description of the type of a parameter
 *
 * @param id The ID for the parameter
 *
 * @return The parameter's type description
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Determines if the parameter is read only or writable when saved
 *
 * @param id The ID for the parameter
 *
 * @return true if the parameter is not writable, false if it is
 */
//------------------------------------------------------------------------------
bool EventLocator::IsParameterReadOnly(const Integer id) const
{
//   if (id == APPEND_TO_REPORT)
//      return true;

   return GmatBase::IsParameterReadOnly(id);
}


//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Determines if the parameter is read only or writable when saved
 *
 * @param label The string description of the parameter
 *
 * @return true if the parameter is not writable, false if it is
 */
//------------------------------------------------------------------------------
bool EventLocator::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a real parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::GetRealParameter(const Integer id) const
{
   if (id == STEP_SIZE)
      return stepSize;

   return GmatBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::SetRealParameter(const Integer id, const Real value)
{
   if (id == STEP_SIZE)
   {
      if (value > 0.0)
      {
         stepSize = value;
         return stepSize;
      }
      else
      {
         EventException ee("");
         ee.SetDetails(errorMessageFormat.c_str(),
               GmatStringUtil::ToString(value, GetDataPrecision()).c_str(),
               "StepSize", "Real number > 0");
         throw ee;
      }
   }

   return GmatBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a real parameter
 *
 * @param label The script string of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::SetRealParameter(const std::string &label,
                                      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a string parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetStringParameter(const Integer id) const
{
   if (id == SATNAME)
      return satName;
   if (id == EVENT_FILENAME)
      return filename;
   if (id == INPUT_EPOCH_FORMAT)
      return epochFormat;
   if (id == INITIAL_EPOCH)
      return initialEpoch;
   if (id == FINAL_EPOCH)
      return finalEpoch;

   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 *
 * @return true if the string was set, false if not
 */
//------------------------------------------------------------------------------
bool EventLocator::SetStringParameter(const Integer id,
      const std::string &value)
{
   if (id == SATNAME)
   {
      if (value != "")
      {
         satName = value;
         return true;
      }
      return false;
   }
   if (id == EVENT_FILENAME)
   {
      if (value != "")
      {
         filename = value;
         return true;
      }
      return false;
   }
   if (id == INPUT_EPOCH_FORMAT)
   {
      if (TimeConverterUtil::IsValidTimeSystem(value))
      {
         epochFormat = value;
         return true;
      }
      else
      {
         EventException ee("");
         ee.SetDetails(errorMessageFormat.c_str(),
                       value.c_str(),
                       "EpochFormat", "Valid Time Format");
         throw ee;
      }
   }
   if ((id == INITIAL_EPOCH) || (id == FINAL_EPOCH))
   {
      SetEpoch(value, id);
      return true;
   }
   if (id == OCCULTING_BODIES)
   {
      if (value != satName)
      {
         if (value != "Sun")
         {
            if (find(occultingBodyNames.begin(), occultingBodyNames.end(),
                  value) == occultingBodyNames.end())
            occultingBodyNames.push_back(value);
         }
         else
         {
            MessageInterface::ShowMessage("The Sun was set as an occulting "
            "body, but Stars cannot occult their own light.  The Sun is being "
            "excluded from the list of occulters.\n");
            return false;
         }
      }
      return true;
   }

   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a string parameter from a vector of strings
 *
 * @param id The ID of the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetStringParameter(const Integer id,
      const Integer index) const
{

   if (id == OCCULTING_BODIES)
   {
      if ((index >= 0) && (index < (Integer) occultingBodyNames.size()))
         return occultingBodyNames.at(index);
      else
         throw EventException(
               "Index out of range when trying to access occulting body name for " +
               instanceName);
   }

   return GmatBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter in a vector of strings
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 * @param index The index into the vector
 *
 * @return true if the string was set, false if not
 */
//------------------------------------------------------------------------------
bool EventLocator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   if (id == OCCULTING_BODIES)
   {
      if (index < (Integer) occultingBodyNames.size())
      {
         occultingBodyNames.at(index) = value;
         return true;
      }
      else
      {
         occultingBodyNames.push_back(value);
         return true;
      }
   }

   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a StringArray parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
const StringArray& EventLocator::GetStringArrayParameter(const Integer id) const
{
   if (id == OCCULTING_BODIES)
      return occultingBodyNames;

   return GmatBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a StringArray parameter from a vector of StringArrays
 *
 * @param id The ID of the parameter
 * @param index The index of the StringArray in the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
const StringArray& EventLocator::GetStringArrayParameter(const Integer id,
                                             const Integer index) const
{
   return GmatBase::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a string parameter
 *
 * @param label The script string of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 *
 * @return true if the string was set, false if not
 */
//------------------------------------------------------------------------------
bool EventLocator::SetStringParameter(const std::string &label,
                                      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a string parameter from a vector of strings
 *
 * @param label The script string of the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a string parameter in a vector of strings
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 * @param index The index into the vector
 *
 * @return true if the string was set, false if not
 */
//------------------------------------------------------------------------------
bool EventLocator::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a StringArray parameter
 *
 * @param label The script string of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
const StringArray& EventLocator::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a StringArray parameter from a vector of StringArrays
 *
 * @param label The script string of the parameter
 * @param index The index of the StringArray in the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
const StringArray& EventLocator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a Boolean parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool EventLocator::GetBooleanParameter(const Integer id) const
{
   if (id == USE_LIGHT_TIME_DELAY)
      return useLightTimeDelay;
   if (id == USE_STELLAR_ABERRATION)
      return useStellarAberration;
   if (id == WRITE_REPORT)
      return writeReport;
   if (id == USE_ENTIRE_INTERVAL)
      return useEntireInterval;
//   if (id == APPEND_TO_REPORT)
//      return appendReport;

   return GmatBase::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Boolean parameter
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool EventLocator::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == USE_LIGHT_TIME_DELAY)
   {
      useLightTimeDelay = value;
      return true;
   }
   if (id == USE_STELLAR_ABERRATION)
   {
      useStellarAberration = value;
      return true;
   }
   if (id == WRITE_REPORT)
   {
      writeReport = value;
      return true;
   }
   if (id == USE_ENTIRE_INTERVAL)
   {
      useEntireInterval = value;
      return true;
   }
//   if (id == APPEND_TO_REPORT)
//   {
//      appendReport = value;
//      return true;
//   }

   return GmatBase::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a Boolean parameter
 *
 * @param label The script string of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool EventLocator::GetBooleanParameter(const std::string &label) const
{
   return GetBooleanParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Boolean parameter
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool EventLocator::SetBooleanParameter(const std::string &label,
      const bool value)
{
   return SetBooleanParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Performs a custom action on the object.
 *
 * Event locators use this method to clear arrays in the locator.
 *
 * @param action The string specifying the action to be taken
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EventLocator::TakeAction(const std::string &action,
      const std::string &actionData)
{
   if (action == "Clear")
   {
      if ((actionData == "OccultingBodies") || (actionData == ""))
      {
         occultingBodyNames.clear();
         occultingBodies.clear();
      }
      return true;
   }

   return GmatBase::TakeAction(action, actionData);
}


//---------------------------------------------------------------------------
// Gmat::ObjectType GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
Gmat::ObjectType EventLocator::GetPropertyObjectType(const Integer id) const
{
   #ifdef DEBUG_OBJECTS
      MessageInterface::ShowMessage(
            "EventLocator:: Asking now for the type of object %d\n", id);
   #endif
   switch (id)
   {
   case SATNAME:
      return Gmat::SPACECRAFT;
   case OCCULTING_BODIES:
      return Gmat::CELESTIAL_BODY;
   default:
      return GmatBase::GetPropertyObjectType(id);
   }
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetTypesForList(const Integer id)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of types that need to be shown on a GUI for a parameter
 *
 * @param id The parameter ID
 *
 * @return The list of types
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& EventLocator::GetTypesForList(const Integer id)
{
   listedTypes.clear();  // ??
   if (id == SATNAME)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::SPACECRAFT) ==
            listedTypes.end())
         listedTypes.push_back(Gmat::SPACECRAFT);
      return listedTypes;
   }
   if (id == OCCULTING_BODIES)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::CELESTIAL_BODY) ==
            listedTypes.end())
         listedTypes.push_back(Gmat::CELESTIAL_BODY);
      return listedTypes;
   }

   return GmatBase::GetTypesForList(id);
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetTypesForList(const std::string &label)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of types that need to be shown on a GUI for a parameter
 *
 * @param label The parameter's identifying string
 *
 * @return The list of types
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& EventLocator::GetTypesForList(const std::string &label)
{
   return GetTypesForList(GetParameterID(label));
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 *
 * @param ss The solar system
 */
//------------------------------------------------------------------------------
void EventLocator::SetSolarSystem(SolarSystem *ss)
{
   solarSys = ss;
}

//------------------------------------------------------------------------------
//  void SetEpoch(std::string ep)
//------------------------------------------------------------------------------
/**
 * Set the epoch.
 *
 * @param <ep> The new epoch.
 *
 * @return the epoch as an A1MJD Real
 */
//------------------------------------------------------------------------------
void EventLocator::SetEpoch(const std::string &ep, Integer id)
{
   #ifdef DEBUG_DATE_FORMAT
   MessageInterface::ShowMessage
      ("EventLocator::SetEpoch() Setting epoch (%d) for spacecraft %s to \"%s\"\n",
       id, instanceName.c_str(), ep.c_str());
   #endif

   std::string epochString = "";
   Real        epochAsReal = 0.0;
   std::string timeSystem;
   std::string timeFormat;
   TimeConverterUtil::GetTimeSystemAndFormat(epochFormat, timeSystem, timeFormat);
   if (timeFormat == "ModJulian") // numeric - save and output without quotes
      epochString = GmatStringUtil::RemoveEnclosingString(ep, "'");
   else // "Gregorian" - not numeric - save and output with quotes
   {
      if (!GmatStringUtil::IsEnclosedWith(ep, "'"))
         epochString = GmatStringUtil::AddEnclosingString(ep, "'");
      else
         epochString = ep;
   }

   #ifdef DEBUG_DATE_FORMAT
   MessageInterface::ShowMessage
      ("EventLocator::SetEpoch() Calling EpochToReal with %s\n",
            epochString.c_str());
   #endif
   Real epochIn = -999.999;
   std::string noQuotes = GmatStringUtil::RemoveEnclosingString(epochString, "'");
   std::string outStr;
   TimeConverterUtil::Convert(epochFormat, epochIn, noQuotes,
                             "A1ModJulian", epochAsReal, outStr);
   if (id == INITIAL_EPOCH)
   {
      initialEpoch = epochString;
      initialEp    = epochAsReal;
   }
   else  // FINAL_EPOCH
   {
      finalEpoch   = epochString;
      finalEp      = epochAsReal;
   }
   #ifdef DEBUG_DATE_FORMAT
   MessageInterface::ShowMessage
      ("EventLocator::SetEpoch() Setting epoch (A1Mjd) to %12.15f\n",
      epochAsReal);
   #endif
}

//------------------------------------------------------------------------------
// std::string GetEpochString(const std::string whichOne = "INITIAL")
//------------------------------------------------------------------------------
std::string EventLocator::GetEpochString(const std::string whichOne)
{
   Real outMjd = -999.999;
   std::string outStr;
   std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?

   if (whichOne == "INITIAL")
   {
      TimeConverterUtil::Convert("A1ModJulian", initialEp, "",
                                 outputFormat, outMjd, outStr);
   }
   else if (whichOne == "FINAL")
   {
      TimeConverterUtil::Convert("A1ModJulian", finalEp, "",
                                 outputFormat, outMjd, outStr);
   }
   else
      return "";

   return outStr;
}

void EventLocator::SetAppend(bool appendIt)
{
   appendReport = appendIt;
}

//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Retrieves the name of the reference object
 *
 * @param type The type of the requested object name; UNKNOWN_OBJECT for any
 *
 * @return Object name
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetRefObjectName(const Gmat::ObjectType type) const
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACECRAFT)
   {
      return satName;
   }
   return GmatBase::GetRefObjectName(type);
}

//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of reference objects
 *
 * @param type The type of the requested objects; UNKNOWN_OBJECT for all
 *
 * @return A StringArray of object names
 */
//------------------------------------------------------------------------------
const StringArray& EventLocator::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   refObjectNames.clear();
   // Get ref. objects for requesting type from the parent class
   GmatBase::GetRefObjectNameArray(type);

   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACECRAFT)
   {
      refObjectNames.push_back(satName);

      #ifdef DEBUG_EVENTLOCATOR_OBJECT
         MessageInterface::ShowMessage
            ("EventLocator::GetRefObjectNameArray() this=<%p>'%s' returning %d "
             "ref. object names\n", this, GetName().c_str(),
             refObjectNames.size());
         for (UnsignedInt i=0; i<refObjectNames.size(); i++)
            MessageInterface::ShowMessage("   '%s'\n",
                  refObjectNames[i].c_str());
      #endif
   }
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::CELESTIAL_BODY)
   {
      // Add ref. objects for requesting type from this class
      refObjectNames.insert(refObjectNames.begin(), occultingBodyNames.begin(),
            occultingBodyNames.end());

      #ifdef DEBUG_EVENTLOCATOR_OBJECT
         MessageInterface::ShowMessage
            ("EventLocator::GetRefObjectNameArray() this=<%p>'%s' returning %d "
             "ref. object names\n", this, GetName().c_str(),
             refObjectNames.size());
         for (UnsignedInt i=0; i<refObjectNames.size(); i++)
            MessageInterface::ShowMessage("   '%s'\n",
                  refObjectNames[i].c_str());
      #endif
   }

   return refObjectNames;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets a reference object on this object
 *
 * @param obj The reference object
 * @param type The type of the object
 * @param name The name of the object
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EventLocator::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                const std::string &name)
{
   if ((satName == name) && (obj->IsOfType(Gmat::SPACECRAFT)))
   {
      sat = (Spacecraft*) obj;
      return true;
   }
   for (UnsignedInt i = 0; i < occultingBodyNames.size(); ++i)
   {
      if (occultingBodyNames.at(i) == name)
      {
         if (obj->IsOfType(Gmat::CELESTIAL_BODY))
         {
            occultingBodies.at(i) = (CelestialBody*) obj;
            return true;
         }
         return false;
      }
   }
   return GmatBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, 
//       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Interface used to support user renames of object references.
 *
 * @param type reference object type.
 * @param oldName object name to be renamed.
 * @param newName new object name.
 *
 * @return true if object name changed, false if not.
 */
//------------------------------------------------------------------------------
bool EventLocator::RenameRefObject(const Gmat::ObjectType type, 
   const std::string &oldName, const std::string &newName)
{
   bool retval = false;

   switch (type)
   {
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION:
      case Gmat::SPACEOBJECT:
      case Gmat::UNKNOWN_OBJECT:
         if (satName == oldName)
         {
            satName = newName;
            retval = true;
         }
         break;
      case Gmat::CELESTIAL_BODY:
         for (UnsignedInt ii = 0; ii < occultingBodyNames.size(); ii++)
         {
            if (occultingBodyNames.at(ii) == oldName)
            {
               occultingBodyNames.at(ii) = newName;
               retval = true;
            }
         }
         break;

      default:
         ;        // Intentional drop-through
   }
   
   return retval;
}


////------------------------------------------------------------------------------
//// Integer GetOwnedObjectCount()
////------------------------------------------------------------------------------
///**
// * Retrieves the number of owned objects in the event locator
// *
// * @return The owned object count
// */
////------------------------------------------------------------------------------
//Integer EventLocator::GetOwnedObjectCount()
//{
//   return 1 + eventTable.GetOwnedObjectCount();
//}
//
//
////------------------------------------------------------------------------------
//// GmatBase* GetOwnedObject(Integer whichOne)
////------------------------------------------------------------------------------
///**
// * Retrieves an owned objects by index
// *
// * @param whichOne The index of the owned object
// *
// * @return The owned object
// */
////------------------------------------------------------------------------------
//GmatBase* EventLocator::GetOwnedObject(Integer whichOne)
//{
//   GmatBase *retval = NULL;
//   if (whichOne == 0)
//      retval = &eventTable;
//   else
//      retval = eventTable.GetOwnedObject(whichOne - 1);
//
//   return retval;
//}
//

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the locator for use
 *
 * @return true if initialization succeeds, false on failure
 */
//------------------------------------------------------------------------------
bool EventLocator::Initialize()
{
   bool retval = false;

   #ifdef DEBUG_EVENT_INITIALIZATION
      MessageInterface::ShowMessage("Initializing event locator at <%p>\n", this);
      MessageInterface::ShowMessage("    Initial epoch is \"%s\"\n", initialEpoch.c_str());
      MessageInterface::ShowMessage("    Final   epoch is \"%s\"\n", finalEpoch.c_str());
   #endif

      // Make sure the epochs are set and Real epochs computed
   SetEpoch(initialEpoch, INITIAL_EPOCH);
   SetEpoch(finalEpoch,   FINAL_EPOCH);

   // Validate inputs here
   if (initialEp > finalEp)
   {
      std::string errmsg = "Initial Epoch must be earlier than Final Epoch for EventLocator ";
      errmsg += instanceName + ".\n";
      throw EventException(errmsg);
   }

   fromEpoch = initialEp;
   toEpoch   = finalEp;

   if (!sat)
   {
      std::string errmsg = "Spacecraft has not been set for EventLocator ";
      errmsg += instanceName + ".\n";
      throw EventException(errmsg);
   }
   if (!sat->IsOfType("Spacecraft"))
   {
      std::string errmsg = "Target must be of type Spacecraft for EventLocator ";
      errmsg += instanceName + ".\n";
      throw EventException(errmsg);
   }
   if (filename == "")
   {
      std::string errmsg = "No filename specified for EventLocator ";
      errmsg += instanceName + ".\n";
      throw EventException(errmsg);
   }

   satStartEpoch = sat->GetEpoch(); // or do I need to get this at some other time??

   /// Get the occulting bodies from the solar system??? OR put them on the ref object list ??  TBD
   // @todo <<<<<<<<<<  Currently have them in the ref object list

   // For now, call this method here to load the planetary PCKs
   solarSys->LoadPCKs();

   // Tell the spacecraft to start recording its data
   sat->RecordEphemerisData();


   fileWasWritten = false;
   isInitialized = true;

   return retval;
}


//------------------------------------------------------------------------------
// void ReportEventData()
//------------------------------------------------------------------------------
/**
 * Writes the event data to file and optionally displays the event data plot.
 * NOTE: this may be able to be an abstract method, when it is implemented in
 * all of the derived classes (unless there are commonalities that can
 * be handled here) *******
 */
//------------------------------------------------------------------------------
void EventLocator::ReportEventData(const std::string &reportNotice)
{
   bool openOK = OpenReportFile();

   if (!openOK)
   {
      // TBD - do we want to throw an exception or just continue without writing?
      return;
   }

   // Write the data here
   theReport << "Here is the report for the EventLocator ";
   theReport << instanceName << "!!!!!!\n";

   theReport.close();

//   fileWasWritten = eventTable.WriteToFile(fullFileName, reportNotice);
//   if (showPlot)
//   {
//      if (fileWasWritten)
//         eventTable.ShowPlot();
//      else
//         MessageInterface::ShowMessage("No events were found, so the event "
//               "plot is not displayed.\n");
//   }

}

//------------------------------------------------------------------------------
// void LocateEvents(const std::string &reportNotice = "")
//------------------------------------------------------------------------------
/**
 * Locates events in the selected time range
 *
 * @param appendToFile flag specifying whether to append the data to the
 *                     file (true) or erase the current file and start
 *                     over (false)
 * @note  this method assumes that the appendReport flag has been set
 *        before this call (using SetAppend method)
 *
 */
//------------------------------------------------------------------------------
void EventLocator::LocateEvents(const std::string &reportNotice)
{
   if (useEntireInterval)
   {
      fromEpoch = satStartEpoch;
      toEpoch   = sat->GetEpoch();
   }
   else
   {
      fromEpoch = initialEp;
      toEpoch   = finalEp;
   }
   // Locate events here and store them as you have decided to do so
   FindEvents(fromEpoch, toEpoch);

   // Write the report
   // appendReport should already have been set by a call to SetAppend
   if (writeReport)
   {
      ReportEventData(reportNotice);
      fileWasWritten = true;  // or set this in ReportEventData in the derived class?
   }
}

//------------------------------------------------------------------------------
// bool FileWasWritten()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating if the EventLocation data was written to a file
 *
 * @return The fileWasWritten flag.
 */
//------------------------------------------------------------------------------
bool EventLocator::FileWasWritten()
{
   return fileWasWritten;
}


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Real EpochToReal(const std::string &ep)
//------------------------------------------------------------------------------
Real EventLocator::EpochToReal(const std::string &ep)
{
   Real fromMjd = -999.999;
   Real outMjd = -999.999;
   std::string outStr;

   // remove enclosing quotes for the validation and conversion
   std::string epNoQuote = GmatStringUtil::RemoveEnclosingString(ep, "'");

   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage
         ("EventLocator::EpochToReal() Converting from %s to A1ModJulian\n", epochFormat.c_str());
   #endif

   if (epochFormat.find("Gregorian") != std::string::npos)
   {
      if (!GregorianDate::IsValid(epNoQuote))
      {
         std::string errmsg = "EventLocator error: epoch ";
         errmsg += ep + " is not a valid Gregorian date.\n";
         throw EventException(errmsg);
      }
   }

   TimeConverterUtil::Convert(epochFormat, fromMjd, epNoQuote, "A1ModJulian", outMjd,
                              outStr);
   #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage
         ("EventLocator::EpochToReal() Done converting from %s to A1ModJulian\n", epochFormat.c_str());
   #endif

   if (outMjd == -999.999)
   {
      #ifdef DEBUG_DATE_FORMAT
      MessageInterface::ShowMessage("EventLocator::EpochToReal() oops!  outMjd = -999.999!!\n");
      #endif
   }
   return outMjd;
}

//------------------------------------------------------------------------------
//  bool OpenReportFile(bool renameOld = true)
//------------------------------------------------------------------------------
bool EventLocator::OpenReportFile(bool renameOld)
{
   std::string fullFileName;

   if ((filename.find('/', 0) == std::string::npos) &&
       (filename.find('\\', 0) == std::string::npos))
   {
      FileManager *fm = FileManager::Instance();
      std::string outPath = fm->GetAbsPathname(FileManager::OUTPUT_PATH);

      // Check for terminating '/' and add if needed
      Integer len = outPath.length();
      if ((outPath.c_str()[len-1] != '/') && (outPath.c_str()[len-1] != '\\'))
         outPath = outPath + "/";

      fullFileName = outPath + filename;
   }
   else
      fullFileName = filename;

   std::string withoutExt = "";
   // Get the file extension (with prepended '.')
   std::string fileExt = GmatFileUtil::ParseFileExtension(fullFileName, true);
   if (fileExt == "")
   {
      withoutExt = fullFileName;
   }
   else
   {
      std::string::size_type lastDot = fullFileName.find_last_of(".");
      withoutExt = fullFileName.substr(0, lastDot);
   }

   if (appendReport)
      theReport.open(fullFileName.c_str(), std::fstream::app);
   else
   {
      // Rename any old existing files here if necessary
      if (renameOld)
      {
         if (GmatFileUtil::DoesFileExist(fullFileName))
         {
            // Get the FileManager pointer
            FileManager *fm = FileManager::Instance();
            Integer     fileCounter = 0;
            bool        done        = false;
            std::stringstream fileRename("");
            Integer     retCode = 0;
            while (!done)
            {
               fileRename.str("");
               fileRename << withoutExt << "__" << fileCounter << fileExt;
               if (fm->RenameFile(fullFileName, fileRename.str(), retCode))
               {
                  done = true;
               }
               else
               {
                  if (retCode == 0) // if no error from system, but not allowed to overwrite
                  {
      //               if (fileCounter < MAX_FILE_RENAMES) // no MAX for now
                        fileCounter++;
      //               else
      //               {
      //                  reset_c(); // reset failure flag in SPICE
      //                  std::string errmsg = "Error renaming existing Event Report file  \"";
      //                  errmsg += fullFileName + "\".  Maximum number of renames exceeded.\n";
      //                  throw UtilityException(errmsg);
      //               }
                  }
                  else
                  {
      //               reset_c(); // reset failure flag in SPICE
                     std::string errmsg =
                           "Unknown system error occurred when attempting to "
                           "rename existing Event Report file \"";
                     errmsg += fullFileName + "\".\n";
                     throw EventException(errmsg);
                  }
               }

            }
         }
      }
      else // if not renaming, if it already exists, delete it
      {
         if (GmatFileUtil::DoesFileExist(fullFileName))
            remove(fullFileName.c_str());
      }
      theReport.open(fullFileName.c_str(), std::fstream::out);
   }

   if (!theReport.is_open())
   {
      std::string errmsg = "Error creating or opening report file ";
      errmsg += fullFileName + ", for object \"";
      errmsg += instanceName + "\".  No event data will be added to the file.\n";
      MessageInterface::PopupMessage(Gmat::WARNING_, errmsg);
      return false;
   }
   return true;
}

//------------------------------------------------------------------------------
// std::string GetAbcorrString()
//------------------------------------------------------------------------------
/**
 * Returns the aberration correction for use in CSPICE calls
 *
 * @return string representing the selected aberration corrections
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetAbcorrString()
{
   std::string correction =  "NONE";
   if (useLightTimeDelay)
   {
      correction = "CN";
      if (useStellarAberration)
      {
         correction = correction + "+S";
      }
   }
   return correction;
}
