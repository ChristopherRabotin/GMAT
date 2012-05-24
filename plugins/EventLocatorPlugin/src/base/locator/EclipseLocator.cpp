//$Id: EclipseLocator.cpp 2251 2012-04-03 23:16:37Z djconway@NDC $
//------------------------------------------------------------------------------
//                           EclipseLocator
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
   "OccultingBodies"          // OCCULTERS
};

const Gmat::ParameterType EclipseLocator::PARAMETER_TYPE[
      EclipseLocatorParamCount - EventLocatorParamCount] =
{
//   Gmat::STRINGARRAY_TYPE     // OCCULTERS
   Gmat::OBJECTARRAY_TYPE     // OCCULTERS
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

   for (UnsignedInt i = 0; i < umbras.size(); ++i)
      delete umbras[i];
   for (UnsignedInt i = 0; i < penumbras.size(); ++i)
      delete penumbras[i];
   for (UnsignedInt i = 0; i < antumbras.size(); ++i)
      delete antumbras[i];
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
   occulters            (el.occulters)
{
   #ifdef DEBUG_LOCATOR_DESTRUCTOR
      MessageInterface::ShowMessage("Creating Eclipse locator %s at <%p> "
            "using the Copy constructor\n", instanceName.c_str(), this);
   #endif

   for (UnsignedInt i = 0; i < umbras.size(); ++i)
      delete umbras[i];
   for (UnsignedInt i = 0; i < penumbras.size(); ++i)
      delete penumbras[i];
   for (UnsignedInt i = 0; i < antumbras.size(); ++i)
      delete antumbras[i];

   umbras.clear();
   penumbras.clear();
   antumbras.clear();

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

      occulters = el.occulters;

      for (UnsignedInt i = 0; i < umbras.size(); ++i)
         delete umbras[i];
      umbras.clear();
      for (UnsignedInt i = 0; i < penumbras.size(); ++i)
         delete penumbras[i];
      penumbras.clear();
      for (UnsignedInt i = 0; i < antumbras.size(); ++i)
         delete antumbras[i];
      antumbras.clear();

      isInitialized = false;
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
GmatBase *EclipseLocator::Clone() const
{
   return new EclipseLocator(*this);
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
   if (id == OCCULTERS)
   {
      if (find(satNames.begin(), satNames.end(), value) == satNames.end())
      {
         if (value != "Sun")
            occulters.push_back(value);
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
   if (id == OCCULTERS)
   {
      if (index < (Integer)occulters.size())
         return occulters[index];
      else
         throw EventException(
               "Index out of range when trying to access occulting body list "
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
   if (id == OCCULTERS)
   {
      if (index < (Integer)occulters.size())
      {
         occulters[index] = value;
         return true;
      }
      else
      {
         occulters.push_back(value);
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
   if (id == OCCULTERS)
      return occulters;

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

      if ((actionData == "OccultingBodies") || (actionData == ""))
      {
         occulters.clear();
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

   if (id == OCCULTERS)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::CELESTIAL_BODY) ==
            listedTypes.end())
      {
         #ifdef DEBUG_TYPELIST
            MessageInterface::ShowMessage("Adding CelestialBody (%d) to the "
               "list for Occulters\n", Gmat::CELESTIAL_BODY);
         #endif
         listedTypes.push_back(Gmat::CELESTIAL_BODY);
      }
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
const ObjectTypeArray& EclipseLocator::GetTypesForList(const std::string &label)
{
   return GetTypesForList(GetParameterID(label));
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
bool EclipseLocator::RenameRefObject(const Gmat::ObjectType type, 
   const std::string &oldName, const std::string &newName)
{
   bool retval = false;

   switch (type)
   {
      case Gmat::CELESTIAL_BODY:
      case Gmat::UNKNOWN_OBJECT:
         for (UnsignedInt i = 0; i < occulters.size(); ++i)
         {
            if (occulters[i] == oldName)
            {
               occulters[i] = newName;
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
bool EclipseLocator::Initialize()
{
   bool retval = false;

   #ifdef DEBUG_EVENTLOCATION
      MessageInterface::ShowMessage("Initializing %s\n", instanceName.c_str());
   #endif

   // Clear old event function instances
   for (UnsignedInt i = 0; i < umbras.size(); ++i)
      delete umbras[i];
   umbras.clear();
   for (UnsignedInt i = 0; i < penumbras.size(); ++i)
      delete penumbras[i];
   penumbras.clear();
   for (UnsignedInt i = 0; i < antumbras.size(); ++i)
      delete antumbras[i];
   antumbras.clear();

   eventFunctions.clear();

   if (solarSys)
   {
      CelestialBody *sun = solarSys->GetBody(SolarSystem::SUN_NAME);
      // This needs to be extended to provide separate functions for each
      // occulter.  Current code is just to get the data flow right

      // Build the event functions: Umbra, Penumbra, Antumbra for each
      // target-occulter combination
      for (UnsignedInt j = 0; j < occulters.size(); ++j)
      {
         CelestialBody *cb = solarSys->GetBody(occulters[j]);
         for (UnsignedInt i = 0; i < targets.size(); ++i)
         {
            SpacePoint *origin = targets[i]-> GetOrigin();

            Penumbra *penumbra = new Penumbra;
            penumbra->SetSol(sun);
            penumbra->SetBody(cb);
            penumbra->SetOrigin(origin);
            penumbra->SetPrimary(targets[i]);

            penumbras.push_back(penumbra);
            eventFunctions.push_back(penumbra);

            Umbra *umbra = new Umbra;
            umbra->SetSol(sun);
            umbra->SetBody(cb);
            umbra->SetOrigin(origin);
            umbra->SetPrimary(targets[i]);

            umbras.push_back(umbra);
            eventFunctions.push_back(umbra);

            Antumbra *antumbra = new Antumbra;
            antumbra->SetSol(sun);
            antumbra->SetBody(cb);
            antumbra->SetOrigin(origin);
            antumbra->SetPrimary(targets[i]);

            antumbras.push_back(antumbra);
            eventFunctions.push_back(antumbra);
         }
      }
   }

   // NOW initialize the base class
   retval = EventLocator::Initialize();

   return retval;
}
