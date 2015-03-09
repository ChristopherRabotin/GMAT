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
#include "SpiceInterface.hpp"
#include "EphemManager.hpp"
#include "EclipseEvent.hpp"
#include "StringUtil.hpp"


//#define DEBUG_TYPELIST
//#define DEBUG_LOCATOR_DESTRUCTOR
//#define DEBUG_EVENT_INITIALIZATION
//#define DEBUG_ECLIPSE_EVENTS
//#define DEBUG_ECLIPSE_ACTION

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
   EventLocator         ("EclipseLocator", name),
   findStart            (0.0),
   findStop             (0.0),
   maxIndex             (-1),
   maxDuration          (-1.0)
{
   objectTypeNames.push_back("EclipseLocator");
   parameterCount = EclipseLocatorParamCount;

   #ifdef DEBUG_ECLIPSE_EVENTS
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
   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Deleting EclipseLocator at <%p>\n", this);
   #endif
   TakeAction("Clear", "Events");
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
   eclipseTypes         (el.eclipseTypes),
   sun                  (NULL),
   findStart            (el.findStart),
   findStop             (el.findStop),
   maxIndex             (el.maxIndex),
   maxDuration          (el.maxDuration)
{
   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Creating Eclipse locator %s at <%p> "
            "using the Copy constructor\n", instanceName.c_str(), this);
   #endif
   TakeAction("Clear", "Events");
   for (Integer ii = 0; ii < el.theEvents.size(); ii++)
      theEvents.push_back(el.theEvents.at(ii));

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
      sun          = NULL;
      findStart    = el.findStart;
      findStop     = el.findStop;
      maxIndex     = el.maxIndex;
      maxDuration  = el.maxDuration;

      TakeAction("Clear", "Events");
      for (Integer ii = 0; ii < el.theEvents.size(); ii++)
         theEvents.push_back(el.theEvents.at(ii));

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
   #ifdef DEBUG_ECLIPSE_ACTION
      MessageInterface::ShowMessage(
            "Entering EclipseLocator::TakeAction with action = %s and actionData = %s\n",
            action.c_str(), actionData.c_str());
   #endif
   if (action == "Clear")
   {
      bool retval = false;

      if ((actionData == "EclipseTypes") || (actionData == ""))
      {
         eclipseTypes.clear();
         retval = true;
      }
      else if ((actionData == "Events") || (actionData == ""))
      {
      #ifdef DEBUG_ECLIPSE_ACTION
         MessageInterface::ShowMessage(
               "In EclipseLocator::TakeAction, about to clear %d events\n",
               (Integer) theEvents.size());
      #endif
         for (Integer ii = 0; ii < theEvents.size(); ii++)
         {
            theEvents.at(ii)->TakeAction("Clear", "Events");
            delete theEvents.at(ii);
         }
         theEvents.clear();
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

   #ifdef DEBUG_ECLIPSE_EVENTS
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


   TimeConverterUtil::Convert("A1ModJulian", findStart, "",
                              outputFormat, resultMjd, fromGregorian);
   TimeConverterUtil::Convert("A1ModJulian", findStop, "",
                              outputFormat, resultMjd, toGregorian);

   Integer sz = (Integer) theEvents.size();
   if (sz == 0)
   {
      theReport << "There are no Eclipse events in the time interval ";
      theReport << fromGregorian << " to " << toGregorian + ".\n";
   }
   else
   {
      Integer        itsNaifId = sat->GetIntegerParameter("NAIFId");
      theReport << "Spacecraft: " << itsNaifId << "\n\n";

//      theReport << "Start Time (UTC)            Stop Time (UTC)             Type        ";
//      theReport << "Occ Body          Duration (s)      Total Duration (s)   Event Number\n";
      theReport << "Start Time (UTC)            Stop Time (UTC)               Duration (s)    ";
      theReport << "Occ Body        Type        Event Number  Total Duration (s)\n";

      // Loop over the total events list
      for (Integer ii = 0; ii < sz; ii++)
      {
         EclipseTotalEvent* ev = theEvents.at(ii);
         std::string eventString = ev->GetReportString();
         theReport << eventString << "\n";
      }
      Integer numIndividual = 0;
      for (unsigned int jj = 0; jj < sz; jj++)
         numIndividual += theEvents.at(jj)->NumberOfEvents();

      theReport << "\nNumber of individual events : " << numIndividual << "\n";
      theReport << "Number of total events      : "   << sz            << "\n";
      theReport << "Maximum duration (s)        : "   << maxDuration   << "\n";
      theReport << "Maximum duration at the "         <<
                   GmatStringUtil::ToOrdinal(maxIndex + 1) << " eclipse.\n";
   }

   theReport.close();
}

//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void FindEvents()
//------------------------------------------------------------------------------
/**
 * Find the eclipse events requested in the time range requested.
 *
 */
//------------------------------------------------------------------------------
void EclipseLocator::FindEvents()
{
   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Entering EclipseLocator::FindEvents with these occultingBodies:\n");
//      for (Integer ii = 0; ii < occultingBodyNames.size(); ii++)
//         MessageInterface::ShowMessage("   %s\n", occultingBodyNames.at(ii).c_str());
      if (!occultingBodies.empty())
      {
         for (Integer ii = 0; ii < occultingBodies.size(); ii++)
            MessageInterface::ShowMessage("   %s\n", (occultingBodies.at(ii))->GetName().c_str());
      }
      else
      {
         MessageInterface::ShowMessage("No occulting bodies set!!!!\n");
      }
      MessageInterface::ShowMessage("and these eclipseTypes:\n");
      for (Integer ii = 0; ii < eclipseTypes.size(); ii++)
         MessageInterface::ShowMessage("   %s\n", eclipseTypes.at(ii).c_str());
   #endif
   EphemManager   *em       = sat->GetEphemManager();
   Integer        itsNaifId = sat->GetIntegerParameter("NAIFId");

   SpiceInterface *spice    = new SpiceInterface();
   em->ProvideEphemerisData();
   em->StopRecording();

   // coverage interval(s) in the loaded kernels
   SPICEDOUBLE_CELL(coverage, 2000);
   scard_c(0, &coverage);   // reset (empty) the coverage cell

   // window we want to search
   SPICEDOUBLE_CELL(window, 2000);
   scard_c(0, &window);   // reset (empty) the coverage cell

   SpiceDouble  start0, end0, startN, endN;
   SpiceInt     numInt     = 0;

   em->GetCoverageWindow(&coverage);

   if (useEntireInterval)
   {
      copy_c(&coverage, &window);
   }
   else // create a window only over the specified time range
   {
      // create a window of the specified time span
      SpiceDouble s = spice->A1ToSpiceTime(initialEp);
      SpiceDouble e = spice->A1ToSpiceTime(finalEp);
      SPICEDOUBLE_CELL(timespan, 2000);
      scard_c(0, &window);   // reset (empty) the coverage cell
      // Get the intersection of the timespan window and the coverage window
      wninsd_c(s, e, &timespan);
      wnintd_c(&coverage, &timespan, &window);
   }
   numInt     = wncard_c(&window);
   wnfetd_c(&window, 0, &start0, &end0);
   wnfetd_c(&window, (numInt-1), &startN, &endN);
   findStart  = spice->SpiceTimeToA1(start0);
   findStop   = spice->SpiceTimeToA1(endN);

   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Number of intervals = %d\n", (Integer) numInt);
   #endif

   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage(" time of first interval start is %12.10f\n", (Real) spice->SpiceTimeToA1(start0));
      MessageInterface::ShowMessage(" time of first interval end is %12.10f  \n", (Real) spice->SpiceTimeToA1(end0));
      MessageInterface::ShowMessage(" time of last interval start is %12.10f \n", (Real) spice->SpiceTimeToA1(startN));
      MessageInterface::ShowMessage(" time of last interval end is %12.10f   \n", (Real) spice->SpiceTimeToA1(endN));
   #endif

   // Set up data for the calls to CSPICE

//   SpiceInt MAXWIN = 1000;
   std::string      theFront  = "";  // we will loop over occultingBodies
   std::string      theFShape = "ELLIPSOID";
   std::string      theFFrame = ""; // we will loop over occultingBodies for this
   std::string      theBack   = "SUN";
   std::string      theBShape = "ELLIPSOID";
   std::string      theBFrame = "IAU_SUN";
   std::string      theAbCorr = GetAbcorrString();
   std::string      theNAIFId = GmatStringUtil::ToString(itsNaifId);

   // CSPICE data
   ConstSpiceChar   *occType  = NULL;
   ConstSpiceChar   *front    = NULL;
   ConstSpiceChar   *fshape   = NULL;
   ConstSpiceChar   *fframe   = NULL;
   ConstSpiceChar   *back     = NULL;
   ConstSpiceChar   *bshape   = NULL;
   ConstSpiceChar   *bframe   = NULL;
   ConstSpiceChar   *abcorr   = NULL;
   ConstSpiceChar   *obsrvr   = NULL;
   SpiceDouble      step      = stepSize;

   SPICEDOUBLE_CELL(result, 2000);
   scard_c(0, &result);   // reset (empty) the coverage cell

   fshape = theFShape.c_str();
   back   = theBack.c_str();
   bshape = theBShape.c_str();
   bframe = theBFrame.c_str();
   abcorr = theAbCorr.c_str();
   obsrvr = theNAIFId.c_str();

   EclipseTotalEvent  *rawList = new EclipseTotalEvent();

   for (Integer ii = 0; ii < occultingBodies.size(); ii++)
   {
//      std::string full    = "FULL";
//      std::string partial = "PARTIAL";
//      std::string annular = "ANNULAR";

      theFront  = GmatStringUtil::ToUpper(occultingBodyNames.at(ii));
      if (theFront == "LUNA")
         theFront = "MOON";
      CelestialBody *body = occultingBodies.at(ii);
      theFFrame = body->GetStringParameter(body->GetParameterID("SpiceFrameName"));

      front     = theFront.c_str();
      fframe    = theFFrame.c_str();

      for (Integer jj = 0; jj < eclipseTypes.size(); jj++)
      {
         if (eclipseTypes.at(jj) == "Umbra")
         {
//            occType = full.c_str();
            occType = SPICE_GF_FULL;
         }
         else if (eclipseTypes.at(jj) == "Penumbra")
         {
            occType = SPICE_GF_PARTL;
//            occType = partial.c_str();
         }
         else // Antumbra
         {
            occType = SPICE_GF_ANNULR;
//            occType = annular.c_str();
         }
         #ifdef DEBUG_ECLIPSE_EVENTS
            MessageInterface::ShowMessage("Inputs to gfoclt_c:\n");
            MessageInterface::ShowMessage("  occType = %s\n", eclipseTypes.at(jj).c_str());
            MessageInterface::ShowMessage("  front   = %s\n", occultingBodyNames.at(ii).c_str());
            MessageInterface::ShowMessage("  fshape  = %s\n", theFShape.c_str());
            MessageInterface::ShowMessage("  fframe  = %s\n", theFFrame.c_str());
            MessageInterface::ShowMessage("  back    = %s\n", theBack.c_str());
            MessageInterface::ShowMessage("  bshape  = %s\n", theBShape.c_str());
            MessageInterface::ShowMessage("  bframe  = %s\n", theBFrame.c_str());
            MessageInterface::ShowMessage("  abcorr  = %s\n", theAbCorr.c_str());
            MessageInterface::ShowMessage("  obsrvr  = %s\n", theNAIFId.c_str());
         #endif
         gfoclt_c(occType, front, fshape, fframe, back, bshape, bframe, abcorr,
                  obsrvr, step, &window, &result);

//         // ****
//            spice->TryBogusCall();
//         // ****
         if (failed_c())
         {
            ConstSpiceChar option[] = "LONG";
            SpiceInt       numChar  = MAX_LONG_MESSAGE_VALUE;
            SpiceChar      err[MAX_LONG_MESSAGE_VALUE];
            getmsg_c(option, numChar, err);
            std::string errStr(err);
            std::string errmsg = "Error calling gfoclt_c!!!  ";
            errmsg += "Message received from CSPICE is: ";
            errmsg += errStr + "\n";
            MessageInterface::ShowMessage("----- error message = %s\n",
                  errmsg.c_str());
            reset_c();
            throw EventException(errmsg);
         }
         #ifdef DEBUG_ECLIPSE_EVENTS
            MessageInterface::ShowMessage("After gfoclt_c .....\n");
         #endif
         Integer numEclipse = wncard_c(&result);
         #ifdef DEBUG_ECLIPSE_EVENTS
            MessageInterface::ShowMessage("After gfoclt_c:\n");
            MessageInterface::ShowMessage("  numEclipse = %d\n", numEclipse);
         #endif
         // Create an event from the result
         for (Integer kk = 0; kk < numEclipse; kk++)
         {
            SpiceDouble s, e;
            wnfetd_c(&result, kk, &s, &e);
            Real s1 = spice->SpiceTimeToA1(s);
            Real e1 = spice->SpiceTimeToA1(e);
            EclipseEvent *newEvent = new EclipseEvent(s1, e1, eclipseTypes.at(jj), theFront);
            rawList->AddEvent(newEvent);
         }
      }
   }
   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("rawList has been set up ...n");
   #endif

   Integer numEventsInTotal = rawList->NumberOfEvents();
   if (numEventsInTotal == 0)
   {
      delete rawList;
      return;
   }

   // Clear old events
   TakeAction("Clear", "Events");
   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Events have been cleared ... numEventsInTotal = %d\n",
            numEventsInTotal);
   #endif

   // Rearrange the events into the proper order
   EclipseEvent *a, *b;
   // @todo Check this algorithm for correctness and performance
   for (Integer yy = 0; yy < numEventsInTotal; yy++)
   {
      for (Integer zz = 0; zz < numEventsInTotal; zz++)
      {
         a = rawList->GetEvent(yy);
         b = rawList->GetEvent(zz);
         if (a->GetStart() < b->GetStart())
         {
            // switch these
            rawList->SetEvent(yy, b);
            rawList->SetEvent(zz, a);
         }
      }
   }
   a = NULL;
   b = NULL;
   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("Events have been ordered\n");
   #endif

   Integer currentIndex = 0;
   EclipseTotalEvent *currentTotal = NULL;
   EclipseEvent* currentEvent = rawList->GetEvent(0);

   EclipseTotalEvent *aTotal = new EclipseTotalEvent();
   aTotal->AddEvent(currentEvent);
   aTotal->SetStart(currentEvent->GetStart());
   aTotal->SetEnd(currentEvent->GetEnd());
   aTotal->SetIndex(currentIndex);
   theEvents.push_back(aTotal); // first one on the list, at position 0
   #ifdef DEBUG_ECLIPSE_EVENTS
      MessageInterface::ShowMessage("First total event has been set up\n");
   #endif

   for (Integer qq = 1; qq < numEventsInTotal; qq++)
   {
      currentEvent = rawList->GetEvent(qq);
      Real itsStart = currentEvent->GetStart();
      Real itsEnd   = currentEvent->GetEnd();
      if (itsStart > (theEvents.at(currentIndex))->GetEnd())
      {
         currentIndex++;
         currentTotal = new EclipseTotalEvent();
         currentTotal->AddEvent(currentEvent);
         currentTotal->SetStart(itsStart);
         currentTotal->SetEnd(itsEnd);
         currentTotal->SetIndex(currentIndex);
         theEvents.push_back(currentTotal);
      }
      else
      {
         if (itsEnd > (theEvents.at(currentIndex))->GetEnd())
         {
            theEvents.at(currentIndex)->SetEnd(itsEnd);
         }
         theEvents.at(currentIndex)->AddEvent(currentEvent);
      }
   }

   // Compute the maximum duration of the events
   maxIndex    = -1;
   maxDuration = -1.0;
   for (Integer rr = 0; rr < theEvents.size(); rr++)
   {
      Real itsTotalDuration = theEvents.at(rr)->GetDuration();
      if (itsTotalDuration > maxDuration)
      {
         maxDuration = itsTotalDuration;
         maxIndex = rr;
      }
   }
   delete rawList;
}
