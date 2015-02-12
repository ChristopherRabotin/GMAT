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
//#include "Elevation.hpp"
//#include "LineOfSight.hpp"
#include "MessageInterface.hpp"
#include "GroundstationInterface.hpp"

//#define DEBUG_SET
//#define DEBUG_SETREF

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
}


//------------------------------------------------------------------------------
// ContactLocator& operator=(const ContactLocator &c)
//------------------------------------------------------------------------------
/**
 * Assignemtn operator
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

      // Initialize the member event functions
      retval = true;
   }

   return retval;
}

//------------------------------------------------------------------------------
// void ReportEventData()
//------------------------------------------------------------------------------
/**
 * Writes the event data to file and optionally displays the event data plot.
 */
//------------------------------------------------------------------------------
void ContactLocator::ReportEventData(const std::string &reportNotice)
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

   Integer sz = (Integer) contactResults.size();
   if (sz == 0)
   {
      theReport << "There are no Contact events in the time interval ";
      theReport << fromGregorian << " to " << toGregorian + ".\n";
   }
   else
   {
      // Loop over the ContactResults list
      for (Integer ii = 0; ii < sz; ii++)
      {
         // write headers or whatever needs to be written here, if anything
         // loop over the 'contents' [events list] for each ContactResult
         EventList* evList = contactResults.at(ii);
         Integer sz2   = (Integer) evList->size();
         for (Integer jj = 0; jj < sz2; jj++)
         {
            LocatedEvent *ev = evList->at(jj);
            std::string eventString = ev->GetReportString();
            // do other stuff here
         }
      }
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
void ContactLocator::FindEvents()
{
   // *** FILL THIS IN WITH CALLS TO SPICE AND OTHER CODE ***
   // *** IT MAY BE USEFUL TO LOOK AT SOME OTHER SPICE CODE, IN UTIL,
   // *** TO SEE HOW STRINGS, TIMES, ETC. ARE USED AND PASSED TO CSPICE
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

