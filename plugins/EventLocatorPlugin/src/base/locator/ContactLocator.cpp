//$Id: ContactLocator.cpp 1979 2012-01-07 00:34:06Z  $
//------------------------------------------------------------------------------
//                           ContactLocator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Nov 7, 2011
// Updated: 2015  Wendy Shoan / GSFC and Yeerang Lim/KAIST
//
/**
 * Definition of the ContactLocator
 * Updates based on prototype by Joel Parker / GSFC
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
//#define DEBUG_INIT_FINALIZE
//#define DEBUG_CONTACTLOCATOR_INIT

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

const std::string ContactLocator::LT_DIRECTIONS[2] =
{
   "Transmit",
   "Receive",
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

   stationNames.clear();
   stations.clear();
   contactResults.clear();

   // Override default stepSize for the ContactLocator
   stepSize = 10;

   // Set default occulting bodies  2015.09.21 removing Luna as a default (GMT-5070)
//   defaultOccultingBodies.push_back("Luna");

   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage("CREATED new ContactLocator %s<%p>\n",
            instanceName.c_str(), this);
   #endif
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
   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage("DESTRUCTING ContactLocator %s<%p>\n",
            instanceName.c_str(), this);
   #endif
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
   // Observers
   stationNames.clear();
   stations.clear();
   for (Integer jj = 0; jj < cl.stationNames.size(); jj++)
      stationNames.push_back(cl.stationNames.at(jj));
   for (Integer jj = 0; jj < cl.stations.size(); jj++)
      stations.push_back(cl.stations.at(jj));
   // ContactResults
   TakeAction("Clear", "Events");
   ContactResult *toCopy   = NULL;
   ContactResult *newEvent = NULL;
   for (Integer ii = 0; ii < cl.contactResults.size(); ii++)
   {
      toCopy   = cl.contactResults.at(ii);
      newEvent = new ContactResult(*toCopy);
      contactResults.push_back(newEvent);
   }
   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage(
            "CREATED new ContactLocator %s<%p> by copying %s<%p>\n",
            instanceName.c_str(), this, cl.instanceName.c_str(), &cl);
   #endif
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

//      stationNames       = c.stationNames;
      lightTimeDirection = c.lightTimeDirection;

      // Observers
      stationNames.clear();
      stations.clear();
      for (Integer jj = 0; jj < c.stationNames.size(); jj++)
         stationNames.push_back(c.stationNames.at(jj));
      for (Integer jj = 0; jj < c.stations.size(); jj++)
         stations.push_back(c.stations.at(jj));
      // ContactResults
      TakeAction("Clear", "Events");
      ContactResult *toCopy   = NULL;
      ContactResult *newEvent = NULL;
      for (Integer ii = 0; ii < c.contactResults.size(); ii++)
      {
         toCopy   = c.contactResults.at(ii);
         newEvent = new ContactResult(*toCopy);
         contactResults.push_back(newEvent);
      }
   }
   #ifdef DEBUG_INIT_FINALIZE
      MessageInterface::ShowMessage(
            "COPIED to ContactLocator %s<%p> from %s<%p>\n",
            instanceName.c_str(), this, c.instanceName.c_str(), &c);
   #endif

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

//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void ContactLocator::Copy(const GmatBase* orig)
{
   operator=(*((ContactLocator *)(orig)));
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
      #ifdef DEBUG_SET
         MessageInterface::ShowMessage("--- Attempting to add a GS to list of ground stations ...\n");
      #endif
      if (find(stationNames.begin(), stationNames.end(), value) ==
            stationNames.end())
      {
         stationNames.push_back(value);
         #ifdef DEBUG_SET
            MessageInterface::ShowMessage("--- Just added \"%s\" to list of ground stations ...\n",
                  value.c_str());
         #endif
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
      #ifdef DEBUG_SET
         MessageInterface::ShowMessage("--- Attempting to add a GS (index = %d) to list of ground stations ...\n", index);
      #endif
      if (find(stationNames.begin(), stationNames.end(), value) == stationNames.end())
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
      else
      {
         // ignore duplicate station names, for now
         return true;
//         std::string errmsg = "Observer ";
//         errmsg += value + " is already in list for ContactLocator ";
//         errmsg += instanceName + ".  Each observer must be listed only once.\n";
//         throw EventException(errmsg);
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
// UnsignedInt GetPropertyObjectType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieves object type of parameter of given id.
 *
 * @param <id> ID for the parameter.
 *
 * @return parameter ObjectType
 */
//---------------------------------------------------------------------------
UnsignedInt ContactLocator::GetPropertyObjectType(const Integer id) const
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
      for (Integer ii = 0; ii < 2; ii++)
         enumStrings.push_back(LT_DIRECTIONS[ii]);

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
// GmatBase* GetRefObject(const UnsignedInt type, const std::string &name,
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
GmatBase* ContactLocator::GetRefObject(const UnsignedInt type,
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
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
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
bool ContactLocator::SetRefObject(GmatBase *obj, const UnsignedInt type,
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
            "--- DIDN'T match anthing!!! so calling parent ...\n");
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
const StringArray& ContactLocator::GetRefObjectNameArray(const UnsignedInt type)
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
// bool RenameRefObject(const UnsignedInt type,
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
bool ContactLocator::RenameRefObject(const UnsignedInt type,
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
            "Entering EclipseLocator::TakeAction (%s<%p>) with action = %s and actionData = %s\n",
            instanceName.c_str(), this, action.c_str(), actionData.c_str());
   #endif
   if (action == "Clear")
   {
      bool retval = false;

      if ((actionData == "Observers") || (actionData == ""))
      {
         stationNames.clear();
         stations.clear();
         retval = true;
      }
      else if ((actionData == "Events") || (actionData == ""))
      {
         #ifdef DEBUG_ECLIPSE_ACTION
            MessageInterface::ShowMessage(
                  "In ContactLocator::TakeAction, about to clear %d events\n",
                  (Integer) contactResults.size());
         #endif
         for (Integer ii = 0; ii < contactResults.size(); ii++)
         {
            contactResults.at(ii)->TakeAction("Clear", "Events");
            delete contactResults.at(ii);
         }
         contactResults.clear();
         retval = true;
      }

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
   #ifdef DEBUG_CONTACTLOCATOR_INIT
      MessageInterface::ShowMessage("In CL::Init for %s\n", instanceName.c_str());
   #endif

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
//      for (UnsignedInt i = 0; i < occultingBodies.size(); ++i)
//      {
         for (UnsignedInt j = 0; j < stations.size(); ++j)
         {
            if (!stations[j]->IsOfType(Gmat::SPACE_POINT))
               throw EventException("The object " + stations[j]->GetName() +
                     " must be a SpacePoint for Contact Location");
         }
//      }

      if (runMode != "Disabled")
      {
         // Set up the ground stations so that we can do Contact Location
         for (UnsignedInt ii= 0; ii < stations.size(); ii++)
         {
            GroundstationInterface *gsi = (GroundstationInterface*) stations.at(ii);
            if (!gsi->InitializeForContactLocation(true))  // use false for testing resulting files
            {
               std::string errmsg = "Error writing SPK or FK kernel for Ground Station ";
               errmsg            += stationNames.at(ii) + " used by ContactLocator ";
               errmsg            += instanceName + "\n";
               throw EventException(errmsg);
            }
         }
      }

      // Initialize the member event functions
      retval = true;
   }

   #ifdef DEBUG_CONTACTLOCATOR_INIT
      MessageInterface::ShowMessage("In CL::Init  about to set locatingString\n");
   #endif
   SetLocatingString("ContactLocator");

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
   std::string noEvents = GetNoEventsString("contact");

   #ifdef DEBUG_CONTACT_LOCATOR_WRITE
      MessageInterface::ShowMessage("attempting to write out %d events\n",
            (Integer) sz);
   #endif
   Integer numIndividual = 0;

   if (sz ==0)
   {
      theReport << "\n" << noEvents << "\n";
   }
   // Loop over the total events list
   for (Integer ii = 0; ii < sz; ii++)
   {
      ContactResult* ev = contactResults.at(ii);
      numIndividual     = ev->NumberOfEvents();
      ev->SetNoEvents(noEvents);

      std::string eventString = ev->GetReportString();
      theReport << eventString << "\n";

      theReport << "\nNumber of events : " << numIndividual << "\n\n\n";
   }

//   for (unsigned int jj = 0; jj < sz; jj++)
//      numIndividual += contactResults.at(jj)->NumberOfEvents();

//   theReport << "\nNumber of events : " << numIndividual << "\n\n\n";

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
      MessageInterface::ShowMessage("Stations (%d) are:\n", stations.size());
      for (Integer ii = 0; ii < stations.size(); ii++)
         MessageInterface::ShowMessage("    %d     %s<%p>\n",
               ii, (stations.at(ii)->GetName()).c_str(), stations.at(ii));
   #endif


   // Set up data for the calls to CSPICE

   // @YRL
   std::string      theObsrvr = ""; // we will loop over observers for this
   // up to this line
//   std::string      theFront  = "";  // we will loop over occultingBodies
//   std::string      theFShape = "ELLIPSOID";
//   std::string      theFFrame = ""; // we will loop over occultingBodies for this
//   std::string      theBack   = "SUN";
//   std::string      theBShape = "ELLIPSOID";
//   std::string      theBFrame = "IAU_SUN";
   std::string      theAbCorr = GetAbcorrString();
   #ifdef DEBUG_CONTACT_EVENTS
      MessageInterface::ShowMessage("---- theAbCorr  = %s\n", theAbCorr.c_str());
   #endif

   Integer        numContacts = 0;
   RealArray      starts;
   RealArray      ends;

   // Need to set findStart and findStop somewhere in here!!!!

   // Clear old events
   TakeAction("Clear", "Events");
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
      std::string obsFrame = stations.at(j)->GetStringParameter("SpiceFrameId");

      Real  minElAngle  = stations.at(j)->GetRealParameter("MinimumElevationAngle");

      // The ground station's central body should not be an occulting body
      StringArray bodiesToUse;
      std::string currentBody;
      std::string centralBody = stations.at(j)->GetStringParameter(
                                stations.at(j)->GetParameterID("CentralBody"));
      for (unsigned int ii = 0; ii < occultingBodyNames.size(); ii++)
      {
         currentBody = occultingBodyNames.at(ii);
         if (currentBody == centralBody)
         {
//            if (!centralBodyWarningWritten)
//            {
               MessageInterface::ShowMessage(
                     "*** WARNING *** Body %s is the central body for "
                     "GroundStation %s and so will not be considered an occulting body "
                     "for contact location.\n", centralBody.c_str(),
                     (stations.at(j)->GetName()).c_str());
//               centralBodyWarningWritten = true;
//            }
         }
         else
         {
            bodiesToUse.push_back(currentBody);
         }
      }


      #ifdef DEBUG_CONTACT_EVENTS
         MessageInterface::ShowMessage("Calling GetContactIntervals with: \n");
         MessageInterface::ShowMessage("   theObsrvr         = %s(%s)\n",
               (stations.at(j))->GetName().c_str(), theObsrvr.c_str());
         MessageInterface::ShowMessage("   occultingBodies   = \n");
          for (Integer ii = 0; ii < occultingBodyNames.size(); ii++)
             MessageInterface::ShowMessage("      %d     %s\n", ii, occultingBodyNames.at(ii).c_str());
          MessageInterface::ShowMessage("   bodiesToUse   = \n");
           for (Integer ii = 0; ii < bodiesToUse.size(); ii++)
              MessageInterface::ShowMessage("      %d     %s\n", ii, bodiesToUse.at(ii).c_str());
         MessageInterface::ShowMessage("   theAbCorr         = %s\n", theAbCorr.c_str());
         MessageInterface::ShowMessage("   initialEp         = %12.10f\n", initialEp);
         MessageInterface::ShowMessage("   finalEp           = %12.10f\n", finalEp);
         MessageInterface::ShowMessage("   useEntireInterval = %s\n", (useEntireInterval? "true" : "false"));
         MessageInterface::ShowMessage("   stepSize          = %12.10f\n", stepSize);
      #endif
      bool transmit = (GmatStringUtil::ToUpper(lightTimeDirection) == "TRANSMIT");
      em -> GetContactIntervals(theObsrvr, minElAngle, obsFrame, bodiesToUse, theAbCorr,
            initialEp, finalEp, useEntireInterval, useLightTimeDelay, transmit, stepSize, numContacts,
            starts, ends);
      #ifdef DEBUG_CONTACT_EVENTS
         MessageInterface::ShowMessage("After GetContactIntervals: \n");
         MessageInterface::ShowMessage("   numContacts       = %d\n", numContacts);
      #endif
      if (numContacts > 0)
      {
         // Insert the events into the array
         for (Integer kk = 0; kk < numContacts; kk++ )
         {
            Real s1 = starts.at(kk);
            Real e1 = ends.at(kk);
            ContactEvent *newEvent = new ContactEvent(s1, e1);
            evList->AddEvent(newEvent);
         }
      }
      // One result array for each station whether or not there are events
      contactResults.push_back(evList);
   }

   #ifdef DEBUG_CONTACT_EVENTS
      MessageInterface::ShowMessage("ContactLocator::FindEvents leaving ... \n");
   #endif
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

