//$Id: ContactLocator.cpp 1979 2012-01-07 00:34:06Z djconway@NDC $
//------------------------------------------------------------------------------
//                           ContactLocator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
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
#include "Elevation.hpp"
#include "LineOfSight.hpp"

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string ContactLocator::PARAMETER_TEXT[
      ContactLocatorParamCount - EventLocatorParamCount] =
{
   "Stations",                // STATIONS
   "Bodies"                   // BODIES
};

const Gmat::ParameterType ContactLocator::PARAMETER_TYPE[
      ContactLocatorParamCount - EventLocatorParamCount] =
{
      Gmat::STRINGARRAY_TYPE,    // STATIONS
      Gmat::STRINGARRAY_TYPE,    // BODIES
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
   EventLocator         ("ContactLocator", name)
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
   ClearContacts();
}

//------------------------------------------------------------------------------
// ContactLocator(const ContactLocator &cl)
//------------------------------------------------------------------------------
/**
 * Copy construnctor
 *
 * @param cl The original being copied
 */
//------------------------------------------------------------------------------
ContactLocator::ContactLocator(const ContactLocator &cl) :
   EventLocator         (cl),
   stationNames         (cl.stationNames),
   bodyNames            (cl.bodyNames)
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

      stationNames = c.stationNames;
      bodyNames    = c.bodyNames;
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
      const std::string & value)
{
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

   if (id == BODIES)
    {
       if (index < (Integer)bodyNames.size())
          return bodyNames[index];
       else
          throw EventException(
                "Index out of range when trying to access body list for " +
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

   if (id == BODIES)
   {
      if (index < (Integer)bodyNames.size())
      {
         bodyNames[index] = value;
         return true;
      }
      else
      {
         bodyNames.push_back(value);
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

   if (id == BODIES)
      return bodyNames;

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
      case Gmat::CELESTIAL_BODY:
      case Gmat::UNKNOWN_OBJECT:
         for (UnsignedInt i = 0; i < bodyNames.size(); ++i)
         {
            if (bodyNames[i] == oldName)
            {
               bodyNames[i] = newName;
               retval = true;
            }
         }
         break;

      /// @todo: Fill in this method for the other ref objects 

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
      ClearContacts();

      for (UnsignedInt i = 0; i < targets.size(); ++i)
      {
         for (UnsignedInt j = 0; j < stations.size(); ++j)
         {
            if (!stations[j]->IsOfType(Gmat::SPACE_POINT))
               throw EventException("The object " + stations[j]->GetName() +
                     " must be a SpacePoint for Contact Location");
            // Build the contact event collectives
            Contact *theContact = new Contact;
            theContact->SetPrimary(targets[i]);
            theContact->SetStation((SpacePoint*)stations[j]);
            contacts.push_back(theContact);

            // Now create the member EventFunctions
            Elevation *theEl = new Elevation;
            theEl->SetPrimary(targets[i]);
            theEl->SetStation((SpacePoint*)stations[j]);
            theContact->SetEvent(theEl);

            // One LOS event function per body
            for (UnsignedInt k = 0; k < bodies.size(); ++k)
            {
               if (!bodies[k]->IsOfType(Gmat::CELESTIAL_BODY))
                  throw EventException("Obstructing body " +
                        bodies[k]->GetName() + " cannot be used in line-of-"
                        "sight computations; it is not a CelestialBody.");
               LineOfSight *theLos = new LineOfSight;
               theLos->SetPrimary(targets[i]);
               theLos->SetSecondary((SpacePoint*)stations[j]);
               theLos->SetBody((CelestialBody*)bodies[k]);
               theContact->SetEvent(theLos);
            }
         }
      }

      // Initialize the member event functions
      retval = true;
      for (UnsignedInt i = 0; i < contacts.size(); ++i)
         if (contacts[i]->Initialize() == false)
            retval = false;
   }

   return retval;
}

//------------------------------------------------------------------------------
// void ClearContacts()
//------------------------------------------------------------------------------
/**
 * Removes the owned event functions
 */
//------------------------------------------------------------------------------
void ContactLocator::ClearContacts()
{
   for (UnsignedInt i = 0; i < contacts.size(); ++i)
      delete contacts[i];
   contacts.clear();
}
