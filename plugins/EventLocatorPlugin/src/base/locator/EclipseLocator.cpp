//$Id: EclipseLocator.cpp 2251 2012-04-03 23:16:37Z djconway@NDC $
//------------------------------------------------------------------------------
//                           EclipseLocator
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
// Created: Sep 2, 2011
//
/**
 * Implementation of the eclipse locator
 */
//------------------------------------------------------------------------------


#include "EclipseLocator.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "EventException.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_TYPELIST
//#define DEBUG_LOCATOR_DESTRUCTOR
//#define DEBUG_EVENT_INITIALIZATION

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string EclipseLocator::PARAMETER_TEXT[
      EclipseLocatorParamCount - EventLocatorParamCount] =
{
      "EclipseTypes",           // ECLIPSE_TYPES
};

const Gmat::ParameterType EclipseLocator::PARAMETER_TYPE[
      EclipseLocatorParamCount - EventLocatorParamCount] =
{
   Gmat::STRINGARRAY_TYPE     // OCCULTERS
};


//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// EclipseLocator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param name The name of the EclipseLocator
 */
//------------------------------------------------------------------------------
EclipseLocator::EclipseLocator(const std::string &name) :
   EventLocator         ("EclipseLocator", name)
{
   objectTypeNames.push_back("EclipseLocator");
   parameterCount = EclipseLocatorParamCount;

   #ifdef DEBUG_LOCATOR_DESTRUCTOR
      MessageInterface::ShowMessage("Creating Eclipse locator %s at <%p>\n",
            name.c_str(), this);
   #endif
}

//------------------------------------------------------------------------------
// ~EclipseLocator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EclipseLocator::~EclipseLocator()
{
   #ifdef DEBUG_LOCATOR_DESTRUCTOR
      MessageInterface::ShowMessage("Deleting EclipseLocator at <%p>\n", this);
   #endif
}

//------------------------------------------------------------------------------
// EclipseLocator(const EclipseLocator & el)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param el The EclipseLocator being copied
 */
//------------------------------------------------------------------------------
EclipseLocator::EclipseLocator(const EclipseLocator & el) :
   EventLocator         (el),
   eclipseTypes         (el.eclipseTypes)
{
   #ifdef DEBUG_LOCATOR_DESTRUCTOR
      MessageInterface::ShowMessage("Creating Eclipse locator %s at <%p> "
            "using the Copy constructor\n", instanceName.c_str(), this);
   #endif

   isInitialized = false;
}

//------------------------------------------------------------------------------
// EclipseLocator& operator=(const EclipseLocator & el)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param el The EclipseLocator providing data for this one
 *
 * @return this instance, configured to match el
 */
//------------------------------------------------------------------------------
EclipseLocator& EclipseLocator::operator=(const EclipseLocator & el)
{
   if (this != &el)
   {
      EventLocator::operator=(el);

      eclipseTypes = el.eclipseTypes;

      isInitialized = false;
   }

   return *this;
}

// Inherited (GmatBase) methods for parameters

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the scripted name for a parameter
 *
 * @param id The parameter's id
 *
 * @return The script string
 */
//------------------------------------------------------------------------------
std::string EclipseLocator::GetParameterText(const Integer id) const
{
   if (id >= EventLocatorParamCount && id < EclipseLocatorParamCount)
      return PARAMETER_TEXT[id - EventLocatorParamCount];
   return EventLocator::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a parameter
 *
 * @param str The script string for the parameter
 *
 * @return The parameter's id
 */
//------------------------------------------------------------------------------
Integer EclipseLocator::GetParameterID(const std::string &str) const
{
   for (Integer i = EventLocatorParamCount; i < EclipseLocatorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - EventLocatorParamCount])
         return i;
   }

   return EventLocator::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the type for a parameter
 *
 * @param id The parameter's id
 *
 * @return The parameter type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType EclipseLocator::GetParameterType(const Integer id) const
{
   if (id >= EventLocatorParamCount && id < EclipseLocatorParamCount)
      return PARAMETER_TYPE[id - EventLocatorParamCount];

   return EventLocator::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing a parameter's type
 *
 * @param id The parameter's id
 *
 * @return The type description
 */
//------------------------------------------------------------------------------
std::string EclipseLocator::GetParameterTypeString(const Integer id) const
{
   return EventLocator::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
// const StringArray& GetPropertyEnumStrings(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves eumeration symbols of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return list of enumeration symbols
 */
//---------------------------------------------------------------------------
const StringArray& EclipseLocator::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   switch (id)
   {
   case ECLIPSE_TYPES:
      enumStrings.clear();
      enumStrings.push_back("Umbra");
      enumStrings.push_back("Penumbra");
      enumStrings.push_back("Antumbra");
      return enumStrings;
   default:
      return EventLocator::GetPropertyEnumStrings(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param id The parameter's id
 *
 * @return The parameter data, a string
 */
//------------------------------------------------------------------------------
std::string EclipseLocator::GetStringParameter(const Integer id) const
{
   return EventLocator::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value)
//------------------------------------------------------------------------------
/**
 * Sets the contents of a string parameter
 *
 * @param id The parameter's id
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EclipseLocator::SetStringParameter(const Integer id,
      const std::string &value)
{
   if (id == ECLIPSE_TYPES)
   {
      if ((value != "Umbra") && (value != "Penumbra") && (value != "Antumbra"))
      {
         EventException ee("");
         ee.SetDetails(errorMessageFormat.c_str(),
                       value.c_str(),
                       "EclipseTypes", "1 or more of [Umbra, Penumbra, Antumbra]");
         throw ee;
      }
      if (find(eclipseTypes.begin(), eclipseTypes.end(), value) == eclipseTypes.end())
      {
         eclipseTypes.push_back(value);
      }
      return true;
   }

   return EventLocator::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param id The parameter's id
 * @param index The index into the array
 *
 * @return The parameter string
 */
//------------------------------------------------------------------------------
std::string EclipseLocator::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == ECLIPSE_TYPES)
   {
      if ((index >= 0) && (index < (Integer)eclipseTypes.size()))
         return eclipseTypes.at(index);
      else
         throw EventException(
               "Index out of range when trying to access eclipse type list "
               "for " + instanceName);
   }

   return EventLocator::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a parameter value in a string array
 *
 * @param id The parameter's id
 * @param value The new parameter value
 * @param index The index of the parameter in the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EclipseLocator::SetStringParameter(const Integer id,
      const std::string &value, const Integer index)
{
   if (id == ECLIPSE_TYPES)
   {
      if (index < 0)
      {
         std::string errmsg = "Index for EclipseTypes is out-of-range\n";
         throw EventException(errmsg);
      }
      if ((value != "Umbra") && (value != "Penumbra") && (value != "Antumbra"))
      {
         EventException ee("");
         ee.SetDetails(errorMessageFormat.c_str(),
                       value.c_str(),
                       "EclipseTypes", "1 or more of [Umbra, Penumbra, Antumbra]");
         throw ee;
      }
      if (index < (Integer) eclipseTypes.size())
      {
         eclipseTypes.at(index) = value;
         return true;
      }
      else
      {
         eclipseTypes.push_back(value);
         return true;
      }
   }

   return EventLocator::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// const StringArray & GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param id The parameter's id
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EclipseLocator::GetStringArrayParameter(const Integer id) const
{
   if (id == ECLIPSE_TYPES)
      return eclipseTypes;

   return EventLocator::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter from an array of StringArrays
 *
 * @param id The parameter's id
 * @param index The index into the array
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EclipseLocator::GetStringArrayParameter(const Integer id,
      const Integer index) const
{
   return EventLocator::GetStringArrayParameter(id, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter
 *
 * @param label The parameter's scripted identifier
 *
 * @return The parameter data, a string
 */
//------------------------------------------------------------------------------
std::string EclipseLocator::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label, const std::string & value)
//------------------------------------------------------------------------------
/**
 * Sets the contents of a string parameter
 *
 * @param label The parameter's scripted identifier
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EclipseLocator::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string parameter from a string array
 *
 * @param label The parameter's scripted identifier
 * @param index The index into the array
 *
 * @return The parameter string
 */
//------------------------------------------------------------------------------
std::string EclipseLocator::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label, const std::string & value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a parameter value in a string array
 *
 * @param label The parameter's scripted identifier
 * @param value The new parameter value
 * @param index The index of the parameter in the array
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EclipseLocator::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter
 *
 * @param label The parameter's scripted identifier
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EclipseLocator::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string & label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray parameter from an array of StringArrays
 *
 * @param label The parameter's scripted identifier
 * @param index The index into the array
 *
 * @return The StringArray
 */
//------------------------------------------------------------------------------
const StringArray& EclipseLocator::GetStringArrayParameter(
      const std::string &label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
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
bool EclipseLocator::TakeAction(const std::string &action,
      const std::string &actionData)
{
   if (action == "Clear")
   {
      bool retval = false;

      if ((actionData == "EclipseTypes") || (actionData == ""))
      {
         eclipseTypes.clear();
         retval = true;
      }

      return (EventLocator::TakeAction(action, actionData) || retval);
   }

   return EventLocator::TakeAction(action, actionData);
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
const ObjectTypeArray& EclipseLocator::GetTypesForList(const Integer id)
{
   listedTypes.clear();
   return EventLocator::GetTypesForList(id);
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
const ObjectTypeArray& EclipseLocator::GetTypesForList(const std::string &label)
{
   return GetTypesForList(GetParameterID(label));
}

//------------------------------------------------------------------------------
// GmatBase *Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a replica of this instance
 *
 * @return A pointer to the new instance
 */
//------------------------------------------------------------------------------
GmatBase *EclipseLocator::Clone() const
{
   return new EclipseLocator(*this);
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the locator for use
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EclipseLocator::Initialize()
{
   bool retval = false;

   #ifdef DEBUG_EVENTLOCATION
      MessageInterface::ShowMessage("Initializing %s\n", instanceName.c_str());
   #endif

   if (solarSys)
   {
      sun = (Star*) solarSys->GetBody(SolarSystem::SUN_NAME);
   }

   // NOW initialize the base class
   retval = EventLocator::Initialize();

   return retval;
}

//------------------------------------------------------------------------------
// void ReportEventData()
//------------------------------------------------------------------------------
/**
 * Writes the event data to file and optionally displays the event data plot.
 */
//------------------------------------------------------------------------------
void EclipseLocator::ReportEventData(const std::string &reportNotice)
{
   bool openOK = OpenReportFile();

   if (!openOK)
   {
      // TBD - do we want to throw an exception or just continue without writing?
      return;
   }

   std::string outputFormat = "UTCGregorian";  // will use epochFormat in the future?
   std::string fromGregorian, toGregorian;
   Real        resultMjd;


   TimeConverterUtil::Convert("A1ModJulian", fromEpoch, "",
                              outputFormat, resultMjd, fromGregorian);
   TimeConverterUtil::Convert("A1ModJulian", toEpoch, "",
                              outputFormat, resultMjd, toGregorian);

   if (numEventsFound == 0)
   {
      theReport << "There are no Eclipse events in the time interval ";
      theReport << fromGregorian << " to " << toGregorian + ".\n";
   }
   else
   {
      // *** REPLACE THIS WITH CODE TO WRITE THE REPORT ***
      theReport << "There are no Eclipse events in the time interval ";
      theReport << fromGregorian << " to " << toGregorian + ".\n";
      // **************************************************
   }

   theReport.close();

}

//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void FindEvents(Real fromTime, Real toTime)
//------------------------------------------------------------------------------
/**
 * Find the eclipse events requested in the time range requested.
 *
 */
//------------------------------------------------------------------------------
void EclipseLocator::FindEvents(Real fromTime, Real toTime)
{
   // *** FILL THIS IN WITH CALLS TO SPICE AND OTHER CODE ***
   // *** IT MAY BE USEFUL TO LOOK AT SOME OTHER SPICE CODE, IN UTIL,
   // *** TO SEE HOW STRINGS, TIMES, ETC. ARE USED AND PASSED TO CSPICE
}
