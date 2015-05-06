//$Id: ContactLocator.cpp 1979 2012-01-07 00:34:06Z djconway@NDC $
//------------------------------------------------------------------------------
//                           ContactLocator
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
// Created: Nov 7, 2011
//
/**
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#include "ContactLocator.hpp"
#include "EventException.hpp"
#include "MessageInterface.hpp"
#include "GroundstationInterface.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"
#include "EventException.hpp"
#include "EphemManager.hpp"
#include "StringUtil.hpp"
#include "ContactEvent.hpp"

//#define DEBUG_SET
//#define DEBUG_SETREF
//#define DEBUG_CONTACT_LOCATOR_WRITE
//#define DEBUG_CONTACT_EVENTS

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string ContactLocator::PARAMETER_TEXT[
      ContactLocatorParamCount - EventLocatorParamCount] =
{
   "Observers",               // STATIONS
   "LightTimeDirection",
};

const Gmat::ParameterType ContactLocator::PARAMETER_TYPE[
      ContactLocatorParamCount - EventLocatorParamCount] =
{
   Gmat::OBJECTARRAY_TYPE,    // STATIONS
   Gmat::ENUMERATION_TYPE,
};


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ContactLocator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ContactLocator::ContactLocator(const std::string &name) :
   EventLocator         ("ContactLocator", name),
   lightTimeDirection   ("Transmit")
{
   objectTypeNames.push_back("ContactLocator");
   parameterCount = ContactLocatorParamCount;
}

//------------------------------------------------------------------------------
// ~ContactLocator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ContactLocator::~ContactLocator()
{
   TakeAction("Clear");
}

//------------------------------------------------------------------------------
// ContactLocator(const ContactLocator &cl)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param cl The original being copied
 */
//------------------------------------------------------------------------------
ContactLocator::ContactLocator(const ContactLocator &cl) :
   EventLocator         (cl),
   stationNames         (cl.stationNames),
   lightTimeDirection   (cl.lightTimeDirection)
{
   TakeAction("Clear");
   for (Integer ii = 0; ii < cl.contactResults.size(); ii++)
      contactResults.push_back(cl.contactResults.at(ii));
}


//------------------------------------------------------------------------------
// ContactLocator& operator=(const ContactLocator &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param c The ContactLocator providing parameters for this one
 *
 * @return This ContactLocator
 */
//------------------------------------------------------------------------------
ContactLocator& ContactLocator::operator=(const ContactLocator &c)
{
   if (this != &c)
   {
      EventLocator::operator=(c);

      stationNames       = c.stationNames;
      lightTimeDirection = c.lightTimeDirection;

      TakeAction("Clear");
      for (Integer ii = 0; ii < c.contactResults.size(); ii++)
         contactResults.push_back(c.contactResults.at(ii));

   }

   return *this;
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
GmatBase *ContactLocator::Clone() const
{
   return new ContactLocator(*this);
}


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
std::string ContactLocator::GetParameterText(const Integer id) const
{
   if (id >= EventLocatorParamCount && id < ContactLocatorParamCount)
      return PARAMETER_TEXT[id - EventLocatorParamCount];

   if (id == SATNAME)  return "Target";

   return EventLocator::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string & str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a parameter
 *
 * @param str The script string for the parameter
 *
 * @return The parameter's id
 */
//------------------------------------------------------------------------------
Integer ContactLocator::GetParameterID(const std::string & str) const
{
   for (Integer i = EventLocatorParamCount; i < ContactLocatorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - EventLocatorParamCount])
         return i;
   }
   if (str == "Target")
      return SATNAME;
   else if (str == "Spacecraft")
   {
      std::string errmsg = "\"Spacecraft\" not a valid field for a Contact Locator.  ";
      errmsg += "Please use \"Target\".\n";
      throw EventException(errmsg);
   }
   else
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
Gmat::ParameterType ContactLocator::GetParameterType(const Integer id) const
{
   if (id >= EventLocatorParamCount && id < ContactLocatorParamCount)
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
std::string ContactLocator::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
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
std::string ContactLocator::GetStringParameter(const Integer id) const
{
   if (id == LIGHT_TIME_DIRECTION)
      return lightTimeDirection;

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
bool ContactLocator::SetStringParameter(const Integer id,
      const std::string &value)
{
   #ifdef DEBUG_SET
      MessageInterface::ShowMessage("In SetStringParameter with id = %d, value = %s\n",
            id, value.c_str());
   #endif
   if (id == LIGHT_TIME_DIRECTION)
   {
      std::string lightTimeDirectionList = "Transmit, Receive";
      if ((value == "Transmit") || (value == "Receive"))
      {
         lightTimeDirection = value;
         return true;
      }
      else
      {
         std::string errmsg =
            "The value of \"" + value + "\" for field \"LightTimeDirection\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [ " + lightTimeDirectionList + " ]. ";
         #ifdef DEBUG_SET
            MessageInterface::ShowMessage("ERROR message is: \"%s\"\n", errmsg.c_str()); // *******
         #endif
         EventException ee;
         ee.SetDetails(errmsg);
         throw ee;
      }
   }
   if (id == STATIONS)
   {
      if (find(stationNames.begin(), stationNames.end(), value) ==
            stationNames.end())
         stationNames.push_back(value);
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
std::string ContactLocator::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (id == STATIONS)
    {
       if (index < (Integer)stationNames.size())
          return stationNames[index];
       else
          throw EventException(
                "Index out of range when trying to access station list for " +
                instanceName);
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
bool ContactLocator::SetStringParameter(const Integer id,
      const std::string & value, const Integer index)
{
   if (id == STATIONS)
   {
      if (index < (Integer)stationNames.size())
      {
         stationNames[index] = value;
         return true;
      }
      else
      {
         stationNames.push_back(value);
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
const StringArray & ContactLocator::GetStringArrayParameter(
      const Integer id) const
{
   if (id == STATIONS)
      return stationNames;

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
const StringArray& ContactLocator::GetStringArrayParameter(const Integer id,
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
std::string ContactLocator::GetStringParameter(const std::string & label) const
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
bool ContactLocator::SetStringParameter(const std::string & label,
      const std::string & value)
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
std::string ContactLocator::GetStringParameter(const std::string &label,
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
bool ContactLocator::SetStringParameter(const std::string & label,
      const std::string & value, const Integer index)
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
const StringArray & ContactLocator::GetStringArrayParameter(
      const std::string & label) const
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
const StringArray & ContactLocator::GetStringArrayParameter(
      const std::string & label, const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
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
Gmat::ObjectType ContactLocator::GetPropertyObjectType(const Integer id) const
{
   switch (id)
   {
   case STATIONS:
      return Gmat::GROUND_STATION;
   default:
      return EventLocator::GetPropertyObjectType(id);
   }
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
const StringArray& ContactLocator::GetPropertyEnumStrings(const Integer id) const
{
   static StringArray enumStrings;
   switch (id)
   {
   case LIGHT_TIME_DIRECTION:
      enumStrings.clear();
      enumStrings.push_back("Transmit");
      enumStrings.push_back("Receive");
      return enumStrings;
   default:
      return EventLocator::GetPropertyEnumStrings(id);
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
const ObjectTypeArray& ContactLocator::GetTypesForList(const Integer id)
{
   listedTypes.clear();  // ??
   if (id == STATIONS)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::GROUND_STATION) ==
            listedTypes.end())
         listedTypes.push_back(Gmat::GROUND_STATION);
      return listedTypes;
   }

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
const ObjectTypeArray& ContactLocator::GetTypesForList(const std::string &label)
{
   return GetTypesForList(GetParameterID(label));
}


//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name,
//                        const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns a pointer to a reference object contained in a vector of
 * objects in the BodyFixedPoint class.
 *
 * @param type type of the reference object requested
 * @param name name of the reference object requested
 * @param index index for the particular object requested.
 *
 * @return pointer to the reference object requested.
 */
//------------------------------------------------------------------------------
GmatBase* ContactLocator::GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index)
{
   switch (type)
   {
      case Gmat::GROUND_STATION:
         for (UnsignedInt ii = 0; ii < stationNames.size(); ii++)
         {
            if (name == stationNames.at(ii))
            {
               return stations.at(ii);
            }
         }
         break;

      default:
         break;
   }
   return EventLocator::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a pointer to a reference object in a vector of objects in
 * the BodyFixedPoint class.
 *
 * @param obj The reference object.
 * @param type type of the reference object requested
 * @param name name of the reference object requested
 *
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool ContactLocator::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                  const std::string &name)
{
   #ifdef DEBUG_SETREF
      MessageInterface::ShowMessage("CL::SetRefObject, obj = %s, type = %d (%s), name= %s\n",
            (obj? "NOT NULL" : "NULL"), (Integer) type, OBJECT_TYPE_STRING[type - Gmat::SPACECRAFT].c_str(),
            name.c_str());
      MessageInterface::ShowMessage("station names are:\n");
      for (Integer ii = 0; ii < stationNames.size(); ii++)
         MessageInterface::ShowMessage("   %s\n", stationNames.at(ii).c_str());
   #endif
   switch (type)
   {
      case Gmat::GROUND_STATION:
         for (UnsignedInt ii = 0; ii < stationNames.size(); ii++)
         {
            #ifdef DEBUG_SETREF
               MessageInterface::ShowMessage(
                     "Is of type GROUND_STATION, checking name %s ...\n",
                     stationNames.at(ii).c_str());
            #endif
            if (name == stationNames.at(ii))
//            if (obj->GetName() == stationNames.at(ii))
            {
               #ifdef DEBUG_SETREF
                  MessageInterface::ShowMessage(
                        "it matched!!! so setting it ...\n");
               #endif
               stations.push_back(obj);
//               stations.at(ii) = (GroundstationInterface*) obj;
               return true;
            }
         }
         break;

      default:
         break;
   }
   #ifdef DEBUG_SETREF
      MessageInterface::ShowMessage(
            "--- DIDN'T match anthing!!! so caling parent ...\n");
   #endif
   // Call parent class to add objects to bodyList
   return EventLocator::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool ContactLocator::HasRefObjectTypeArray()
{
   return true;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
const StringArray& ContactLocator::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   #ifdef DEBUG_BF_REF
      MessageInterface::ShowMessage("In BFP::GetRefObjectNameArray, requesting type %d (%s)\n",
            (Integer) type, (GmatBase::OBJECT_TYPE_STRING[type]).c_str());
   #endif

   refObjectNames = EventLocator::GetRefObjectNameArray(type);

  if ((type == Gmat::GROUND_STATION) || (type == Gmat::UNKNOWN_OBJECT))
  {
     refObjectNames.insert(refObjectNames.begin(), stationNames.begin(),
           stationNames.end());
  }

   return refObjectNames;
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 *
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& ContactLocator::GetRefObjectTypeArray()
{
   refObjectTypes = EventLocator::GetRefObjectTypeArray();
   refObjectTypes.push_back(Gmat::GROUND_STATION);
   return refObjectTypes;
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
bool ContactLocator::RenameRefObject(const Gmat::ObjectType type, 
   const std::string &oldName, const std::string &newName)
{
   bool retval = false;

   switch (type)
   {
      case Gmat::GROUND_STATION:
      case Gmat::UNKNOWN_OBJECT:
         for (UnsignedInt i = 0; i < stationNames.size(); ++i)
         {
            if (stationNames[i] == oldName)
            {
               stationNames[i] = newName;
               retval = true;
            }
         }
         break;
      default:
         ;        // Intentional drop-through
   }
   
   retval = (retval || EventLocator::RenameRefObject(type, oldName, newName));

   return retval;
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
bool ContactLocator::TakeAction(const std::string &action,
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

      for (Integer ii = 0; ii < contactResults.size(); ii++)
      {
         contactResults.at(ii)->TakeAction("Clear", "Events");
         delete contactResults.at(ii);
      }
      contactResults.clear();
      retval = true;

      return (EventLocator::TakeAction(action, actionData) || retval);
   }

   return EventLocator::TakeAction(action, actionData);
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
bool ContactLocator::Initialize()
{
   bool retval = false;

   if (EventLocator::Initialize())
   {
      if (stationNames.size() != stations.size())
      {
         std::string errmsg = "Error setting observers on ContactLocator ";
         errmsg += instanceName + "\n";
         throw EventException(errmsg);
      }
      for (UnsignedInt ii= 0; ii < stations.size(); ii++)
      {
         if (stations.at(ii) == NULL)
         {
            std::string errmsg = "Error setting observers on ContactLocator ";
            errmsg += instanceName + "\n";
            throw EventException(errmsg);
         }
      }
      for (UnsignedInt i = 0; i < occultingBodies.size(); ++i)
      {
         for (UnsignedInt j = 0; j < stations.size(); ++j)
         {
            if (!stations[j]->IsOfType(Gmat::SPACE_POINT))
               throw EventException("The object " + stations[j]->GetName() +
                     " must be a SpacePoint for Contact Location");
         }
      }

      // Set up the ground stations so that we can do Contact Location
      for (UnsignedInt ii= 0; ii < stations.size(); ii++)
      {
         GroundstationInterface *gsi = (GroundstationInterface*) stations.at(ii);
         if (!gsi->InitializeForContactLocation(false))
         {
            std::string errmsg = "Error writing SPK or FK kernel for Ground Station ";
            errmsg            += stationNames.at(ii) + " used by ContactLocator ";
            errmsg            += instanceName + "\n";
            throw EventException(errmsg);
         }
      }

      // Initialize the member event functions
      retval = true;
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool ReportEventData(const std::string &reportNotice = "")
//------------------------------------------------------------------------------
/**
 * Writes the event data to file and optionally displays the event data plot.
 */
//------------------------------------------------------------------------------
bool ContactLocator::ReportEventData(const std::string &reportNotice)
{
   #ifdef DEBUG_CONTACT_LOCATOR_WRITE
      MessageInterface::ShowMessage("ContactLocator::ReportEventData ... \n");
   #endif

   bool openOK = OpenReportFile(false);

   if (!openOK)
   {
      // TBD - do we want to throw an exception or just continue without writing?
      return false;
   }

   std::string    itsName   = sat->GetName();
   theReport << "Target: " << itsName << "\n\n";

   Integer     sz       = (Integer) contactResults.size();
   std::string noEvents = GetNoEventsString("Contact");

//   if (sz == 0)
//   {
//      theReport << GetNoEventsString() << "\n";
//   }
//   else
//   {
      #ifdef DEBUG_CONTACT_LOCATOR_WRITE
         MessageInterface::ShowMessage("attempting to write out %d events\n",
               (Integer) sz);
      #endif
      Integer numIndividual = 0;

      // Loop over the total events list
      for (Integer ii = 0; ii < sz; ii++)
      {
         ContactResult* ev = contactResults.at(ii);
         ev->SetNoEvents(noEvents);
//         Integer numEv     = ev->NumberOfEvents();
//         if (numEv > 0)
//         {
//            std::string itsName = stations.at(ii)->GetName();
//            theReport << "Observer: " << itsName << "\n";
//            theReport << "Start Time (UTC)            Stop Time (UTC)";
//            theReport << "               Duration (s)         ";
//            theReport << "Maximum Elevation (deg)\n";

            std::string eventString = ev->GetReportString();
            theReport << eventString << "\n";
//         }
//         else
//         {
//            theReport << GetNoEventsString() << "\n";
//         }
      }

      for (unsigned int jj = 0; jj < sz; jj++)
         numIndividual += contactResults.at(jj)->NumberOfEvents();

      theReport << "\nNumber of events : " << numIndividual << "\n";
//   }

   theReport.close();
   return true;
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
void ContactLocator::FindEvents()
{
   #ifdef DEBUG_CONTACT_EVENTS
      MessageInterface::ShowMessage("Entering ContactLocator::FindEvents ...\n");
   #endif

   EphemManager   *em       = sat->GetEphemManager();
   if (!em)
   {
      std::string errmsg = "ERROR - no EphemManager available for spacecraft ";
      errmsg += sat->GetName() + "!!\n";
      throw EventException(errmsg);
   }

   scNow = sat->GetEpoch();
   em->GetCoverageStartAndStop(initialEp, finalEp, useEntireInterval, true,
                               findStart, findStop);
   #ifdef DEBUG_CONTACT_EVENTS
      MessageInterface::ShowMessage("---- findStart (from ephemManager)  = %12.10f\n", findStart);
      MessageInterface::ShowMessage("---- findStop (from ephemManager)   = %12.10f\n", findStop );
   #endif
   if (GmatMathUtil::IsEqual(findStart,0.0) && GmatMathUtil::IsEqual(findStop,0.0))
   {
      // ... in case there were no files to read from, we'll just use the
      // beginning and current spacecraft times
      findStart = scStart;
      findStop  = scNow;
   }
   #ifdef DEBUG_CONTACT_EVENTS
      MessageInterface::ShowMessage("---- findStart  = %12.10f\n", findStart);
      MessageInterface::ShowMessage("---- findStop   = %12.10f\n", findStop );
   #endif

   // Set up data for the calls to CSPICE

   // @YRL
   std::string      theObsrvr = ""; // we will loop over observers for this
   // up to this line
   std::string      theFront  = "";  // we will loop over occultingBodies
   std::string      theFShape = "ELLIPSOID";
   std::string      theFFrame = ""; // we will loop over occultingBodies for this
   std::string      theBack   = "SUN";
   std::string      theBShape = "ELLIPSOID";
   std::string      theBFrame = "IAU_SUN";
   std::string      theAbCorr = GetAbcorrString();
   #ifdef DEBUG_CONTACT_EVENTS
      MessageInterface::ShowMessage("---- theAbCorr  = %s\n", theAbCorr.c_str());
   #endif

   Integer        numContacts = 0;
   RealArray      starts;
   RealArray      ends;

   // Need to set findStart and findStop somewhere in here!!!!

   // @YRL
   for (Integer j = 0; j < stations.size(); j++ )
   {
      // We want a ContactResult for each station whether or not there are events
      ContactResult *evList = new ContactResult();
      evList->SetObserverName(stations.at(j)->GetName());

      starts.clear();
      ends.clear();
      Integer obsNaifId = stations.at(j)->GetIntegerParameter(
                          stations.at(j)->GetParameterID("NAIFId"));
      theObsrvr = GmatStringUtil::ToString(obsNaifId);
      std::string obsFrame = stations.at(j)->GetStringParameter("SpiceFrameName");

      Real  minElAngle  = stations.at(j)->GetRealParameter("MinimumElevationAngle");

      #ifdef DEBUG_CONTACT_EVENTS
         MessageInterface::ShowMessage("Calling GetContactIntervals with: \n");
         MessageInterface::ShowMessage("   theObsrvr         = %s\n", theObsrvr.c_str());
         MessageInterface::ShowMessage("   occultingBodies   = \n");
         for (Integer ii = 0; ii < occultingBodyNames.size(); ii++)
            MessageInterface::ShowMessage("      %d     %s\n", ii, occultingBodyNames.at(ii).c_str());
         MessageInterface::ShowMessage("   theAbCorr         = %s\n", theAbCorr.c_str());
         MessageInterface::ShowMessage("   initialEp         = %12.10f\n", initialEp);
         MessageInterface::ShowMessage("   finalEp           = %12.10f\n", finalEp);
         MessageInterface::ShowMessage("   useEntireInterval = %s\n", (useEntireInterval? "true" : "false"));
         MessageInterface::ShowMessage("   stepSize          = %12.10f\n", stepSize);
      #endif
      bool transmit = (GmatStringUtil::ToUpper(lightTimeDirection) == "TRANSMIT");
      em -> GetContactIntervals(theObsrvr, minElAngle, obsFrame, occultingBodyNames, theAbCorr,
            initialEp, finalEp, useEntireInterval, useLightTimeDelay, transmit, stepSize, numContacts,
            starts, ends);
      #ifdef DEBUG_CONTACT_EVENTS
         MessageInterface::ShowMessage("After GetContactIntervals: \n");
         MessageInterface::ShowMessage("   numContacts       = %d\n", numContacts);
      #endif
      if (numContacts > 0)
      {
//         ContactResult *evList = new ContactResult();
         // Insert the events into the array
         for (Integer kk = 0; kk < numContacts; kk++ )
         {
            Real s1 = starts.at(kk);
            Real e1 = ends.at(kk);
            ContactEvent *newEvent = new ContactEvent(s1, e1);
            evList->AddEvent(newEvent);
         }
//         // One result array for each station
//         contactResults.push_back(evList);
      }
      // One result array for each station whether or not there are events
      contactResults.push_back(evList);
   }

   // @YRL, upto this line
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
std::string ContactLocator::GetAbcorrString()
{
   std::string correction =  EventLocator::GetAbcorrString();
   if (useLightTimeDelay && (lightTimeDirection == "Transmit"))
   {
      correction = 'X' + correction;
   }
   return correction;
}

