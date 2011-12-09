//$Id$
//------------------------------------------------------------------------------
//                           EventLocator
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
// Created: Jul 6, 2011
//
/**
 * Implementation of the the event locator base class
 */
//------------------------------------------------------------------------------


#include "EventLocator.hpp"
#include "EventException.hpp"
#include "FileManager.hpp"      // for GetPathname()
#include "MessageInterface.hpp"


//#define DEBUG_DUMPEVENTDATA

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
   "Spacecraft",        // SATNAMES,
   "Tolerance",         // TOLERANCE,
   "Filename",          // EVENT_FILENAME,
   "IsActive",          // IS_ACTIVE
   "ShowPlot",          // SHOW_PLOT
   "Epoch",             // EPOCH (Read only)
   "EventFunction"      // EVENT_FUNCTION (Read only)
};

const Gmat::ParameterType
EventLocator::PARAMETER_TYPE[EventLocatorParamCount - GmatBaseParamCount] =
{
//   Gmat::STRINGARRAY_TYPE,       // SATNAMES,
   Gmat::OBJECTARRAY_TYPE,       // SATNAMES,
   Gmat::REAL_TYPE,              // TOLERANCE,
   Gmat::FILENAME_TYPE,          // EVENT_FILENAME,
   Gmat::BOOLEAN_TYPE,           // IS_ACTIVE
   Gmat::BOOLEAN_TYPE,           // SHOW_PLOT
   Gmat::REAL_TYPE,              // EPOCH (Read only)
   Gmat::RVECTOR_TYPE            // EVENT_FUNCTION (Read only)
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
   GmatBase       (Gmat::EVENT_LOCATOR, typeStr, nomme),
   filename       ("LocatedEvents.txt"),
   efCount        (0),
   lastData       (NULL),
   lastEpochs     (NULL),
   isActive       (true),
   showPlot       (false),
   fileWasWritten (false),
   eventTolerance (1.0e-3),
   solarSys       (NULL)
{
   objectTypes.push_back(Gmat::EVENT_LOCATOR);
   objectTypeNames.push_back("EventLocator");
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
//   MessageInterface::ShowMessage("Deleting event locator %s <%p>\n",
//         instanceName.c_str(), this);

   if (lastData != NULL)
      delete [] lastData;

   if (lastEpochs != NULL)
      delete [] lastEpochs;

   // todo: Delete the member EventFunctions
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
   GmatBase          (el),
   filename          (el.filename),
   efCount           (0),
   lastData          (NULL),
   lastEpochs        (NULL),
   isActive          (el.isActive),
   showPlot          (el.showPlot),
   fileWasWritten    (false),
   satNames          (el.satNames),
   targets           (el.targets),
   eventTolerance    (el.eventTolerance),
   solarSys          (el.solarSys)
{
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

      filename       = el.filename;
      efCount        = 0;
      if (lastData != NULL)
         delete [] lastData;
      if (lastEpochs != NULL)
         delete [] lastEpochs;
      lastData       = NULL;
      lastEpochs     = NULL;
      isActive       = el.isActive;
      showPlot       = el.showPlot;
      fileWasWritten = false;
      satNames       = el.satNames;
      targets        = el.targets;
      eventTolerance = el.eventTolerance;
      solarSys       = el.solarSys;

      eventFunctions.clear();
      maxSpan.clear();
      lastSpan.clear();
      stateIndices.clear();
      associateIndices.clear();
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
   if ((id == IS_ACTIVE) || (id == EPOCH) || (id == EVENT_FUNCTION))
      return true;

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
   if (id == TOLERANCE)
      return eventTolerance;

   if (id == EPOCH)
   {
      if (targets.size() > 0)
         return targets[0]->GetEpoch();
      return -1.0;
   }

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
   if (id == TOLERANCE)
   {
      if (value > 0.0)
         eventTolerance = value;
      else
         throw EventException(
               "Located event tolerance value must be a real number >= 0.0");

      return eventTolerance;
   }

   if (id == EPOCH)
   {
      return 0.0;
   }

   return GmatBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a real parameter from a vector of Reals
 *
 * @param id The ID of the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::GetRealParameter(const Integer id, const Integer index) const
{
   if (id == EVENT_FUNCTION)
   {
      Real retval = 0.0;
      if (index < (Integer)eventFunctions.size())
      {
         Real *data = eventFunctions[index]->Evaluate();
         retval = data[1];
      }
      return retval;       // todo Return the event function value
   }
   return GmatBase::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a real parameter from an array of Reals
 *
 * @param id The ID of the parameter
 * @param row The row index into the array
 * @param col The column index into the array
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::GetRealParameter(const Integer id, const Integer row,
                                      const Integer col) const
{
   return GmatBase::GetRealParameter(id, row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter in a vector of Reals
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::SetRealParameter(const Integer id, const Real value,
      const Integer index)
{
   if (id == EVENT_FUNCTION)
      return 0.0;       // todo Return the event function value

   return GmatBase::SetRealParameter(id, value, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer row,
//       const Integer col)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter in an array of Reals
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 * @param row The row index into the array
 * @param col The column index into the array
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::SetRealParameter(const Integer id, const Real value,
                                      const Integer row, const Integer col)
{
   return GmatBase::SetRealParameter(id, value, row, col);
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
// Real GetRealParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a real parameter from a vector of Reals
 *
 * @param label The script string of the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::GetRealParameter(const std::string &label,
                                      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter in a vector of Reals
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::SetRealParameter(const std::string &label,
                                      const Real value,
                                      const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a real parameter from an array of Reals
 *
 * @param label The script string of the parameter
 * @param row The row index into the array
 * @param col The column index into the array
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::GetRealParameter(const std::string &label,
                                      const Integer row,
                                      const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value,
//       const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * Sets the value for a real parameter in an array of Reals
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 * @param row The row index into the array
 * @param col The column index into the array
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real EventLocator::SetRealParameter(const std::string &label,
                                      const Real value, const Integer row,
                                      const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}

const Rvector& EventLocator::GetRvectorParameter(const Integer id) const
{
   if (id == EVENT_FUNCTION)
      return functionValues;

   return GmatBase::GetRvectorParameter(id);
}

const Rvector& EventLocator::SetRvectorParameter(const Integer id,
      const Rvector &value)
{
   if (id == EVENT_FUNCTION)
   {
      Integer size = (value.GetSize() < functionValues.GetSize() ?
            value.GetSize() : functionValues.GetSize());
      for (Integer i = 0; i < size; ++i)
         functionValues[i] = value[i];
      return functionValues;
   }

   return GmatBase::SetRvectorParameter(id, value);
}

const Rvector& EventLocator::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}

const Rvector& EventLocator::SetRvectorParameter(const std::string &label,
      const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
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
   if (id == EVENT_FILENAME)
      return filename;

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
   if (id == EVENT_FILENAME)
   {
      if (value != "")
      {
         filename = value;
         return true;
      }
      return false;
   }

   if (id == SATNAMES)
   {
      if (find(satNames.begin(), satNames.end(), value) == satNames.end())
      {
         satNames.push_back(value);
         targets.push_back(NULL);
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
   if (id == SATNAMES)
   {
      if (index < (Integer)satNames.size())
         return satNames[index];
      else
         throw EventException(
               "Index out of range when trying to access spacecraft name for " +
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
   if (id == SATNAMES)
   {
      if (index < (Integer)satNames.size())
      {
         satNames[index] = value;
         return true;
      }
      else
      {
         satNames.push_back(value);
         targets.push_back(NULL);
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
   if (id == SATNAMES)
      return satNames;

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
   if (id == IS_ACTIVE)
      return isActive;
   if (id == SHOW_PLOT)
      return showPlot;

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
   if (id == IS_ACTIVE)
   {
      isActive = value;
      return isActive;
   }
   if (id == SHOW_PLOT)
   {
      showPlot = value;
      return showPlot;
   }
   return GmatBase::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a Boolean parameter from a vector of Booleans
 *
 * @param id The ID of the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool EventLocator::GetBooleanParameter(const Integer id,
      const Integer index) const
{
   return GmatBase::GetBooleanParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Boolean parameter in a vector of Booleans
 *
 * @param id The ID of the parameter
 * @param value The new value for the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool EventLocator::SetBooleanParameter(const Integer id, const bool value,
      const Integer index)
{
   return GmatBase::SetBooleanParameter(id, value, index);
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
// bool GetBooleanParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves the value for a Boolean parameter from a vector of Booleans
 *
 * @param label The script string of the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool EventLocator::GetBooleanParameter(const std::string &label,
      const Integer index) const
{
   return GetBooleanParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const std::string &label, const bool value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets the value for a Boolean parameter in a vector of Booleans
 *
 * @param label The script string of the parameter
 * @param value The new value for the parameter
 * @param index The index into the vector
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
bool EventLocator::SetBooleanParameter(const std::string &label,
      const bool value, const Integer index)
{
   return SetBooleanParameter(GetParameterID(label), value, index);
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
      if ((actionData == "Spacecraft") || (actionData == ""))
      {
         satNames.clear();
         targets.clear();
      }
      return true;
   }

   return GmatBase::TakeAction(action, actionData);
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
   if (id == SATNAMES)
   {
      if (find(listedTypes.begin(), listedTypes.end(), Gmat::SPACECRAFT) ==
            listedTypes.end())
         listedTypes.push_back(Gmat::SPACECRAFT);
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
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACEOBJECT)
   {
      // Get ref. objects for requesting type from the parent class
      GmatBase::GetRefObjectNameArray(type);

      // Add ref. objects for requesting type from this class
      refObjectNames.insert(refObjectNames.begin(), satNames.begin(),
            satNames.end());

      #ifdef DEBUG_EVENTLOCATOR_OBJECT
         MessageInterface::ShowMessage
            ("EventLocator::GetRefObjectNameArray() this=<%p>'%s' returning %d "
             "ref. object names\n", this, GetName().c_str(),
             refObjectNames.size());
         for (UnsignedInt i=0; i<refObjectNames.size(); i++)
            MessageInterface::ShowMessage("   '%s'\n",
                  refObjectNames[i].c_str());
      #endif

      return refObjectNames;
   }

   return GmatBase::GetRefObjectNameArray(type);
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
   for (UnsignedInt i = 0; i < satNames.size(); ++i)
   {
      if (satNames[i] == name)
      {
         if (obj->IsOfType(Gmat::SPACEOBJECT))
         {
            targets[i] = (SpaceObject*)obj;
            return true;
         }
         return false;
      }
   }
   return GmatBase::SetRefObject(obj, type, name);
}


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

   StringArray badInits;
   efCount = eventFunctions.size();

   // Loop through the event functions, evaluating each and storing their data
   for (UnsignedInt i = 0; i < efCount; ++i)
   {
      if (eventFunctions[i]->Initialize() == false)
         badInits.push_back(eventFunctions[i]->GetName());
   }

   if (badInits.size() == 0)
      retval = true;
   else
   {
      std::string errorList;
      for (UnsignedInt i = 0; i < badInits.size(); ++i)
         errorList = errorList + "   " + badInits[i] + "\n";
      throw EventException("These event functions failed to initialize:\n" +
            errorList);
   }

   if (lastData != NULL)
      delete [] lastData;
   if (lastEpochs != NULL)
      delete [] lastEpochs;

   earlyBound.clear();
   lateBound.clear();

   if (efCount > 0)
   {
      lastData = new Real[efCount * 3];
      lastEpochs = new GmatEpoch[efCount];
      for (UnsignedInt i = 0; i < efCount; ++i)
         lastEpochs[i] = -1.0;

      if (stateIndices.size() == 0)
         stateIndices.insert(stateIndices.begin(), efCount, -1);
      if (associateIndices.size() == 0)
         associateIndices.insert(associateIndices.begin(), efCount, -1);

      earlyBound.push_back(0.0);
      lateBound.push_back(0.0);
   }

   functionValues.SetSize(efCount);
   for (UnsignedInt i = 0; i < efCount; ++i)
      functionValues[i] = 0.0;

   fileWasWritten = false;

   return retval;
}


//------------------------------------------------------------------------------
// Real GetTolerance()
//------------------------------------------------------------------------------
/**
 * Retrieves the locator's tolerance setting
 *
 * @return The tolerance
 */
//------------------------------------------------------------------------------
Real EventLocator::GetTolerance()
{
   return eventTolerance;
}


//------------------------------------------------------------------------------
// Real *Evaluate()
//------------------------------------------------------------------------------
/**
 * Evaluates the EventFunctions and returns their values and derivatives.
 *
 * #param atEpoch  The epoch of the desired evaluation.  Set to -1 to pull epoch
 *                 off the participants.
 * #param forState The Cartesian state of the desired evaluation.  Set to -1 to
 *                 pull state off the participants.
 *
 * @return The event function data from the evaluation
 */
//------------------------------------------------------------------------------
Real *EventLocator::Evaluate(GmatEpoch atEpoch, Real *forState)
{
   Real *vals;

   #ifdef DEBUG_EVENTLOCATION
      MessageInterface::ShowMessage("Evaluating %d event functions; "
            "locator %s\n", eventFunctions.size(), instanceName.c_str());
   #endif

   UnsignedInt i3;

   // Loop through the event functions, evaluating each and storing their data
   for (UnsignedInt i = 0; i < eventFunctions.size(); ++i)
   {
      i3 = i * 3;
      if (atEpoch == -1.0)
      {
         // Evaluate for event location
         vals = eventFunctions[i]->Evaluate();

         #ifdef DEBUG_DUMPEVENTDATA
            dumpfile.precision(15);
            dumpfile << vals[0] << " " << vals[1] << " " << vals[2] << " ";
         #endif
      }
      else
      {
         // Evaluate for integration
         Real *theState = &(forState[associateIndices[i]]);
         vals = eventFunctions[i]->Evaluate(atEpoch, theState);
      }

      // Load the returned data into lastData
      lastData[  i3  ] = vals[0];
      lastData[i3 + 1] = vals[1];
      lastData[i3 + 2] = vals[2];
   }

   #ifdef DEBUG_DUMPEVENTDATA
      dumpfile << "\n";
   #endif

   return lastData;
}


//------------------------------------------------------------------------------
// UnsignedInt GetFunctionCount()
//------------------------------------------------------------------------------
/**
 * Determine the number of event functions in the locator
 *
 * @return The number of functions
 */
//------------------------------------------------------------------------------
UnsignedInt EventLocator::GetFunctionCount()
{
   return eventFunctions.size();
}


//------------------------------------------------------------------------------
// void BufferEvent(Integer forEventFunction)
//------------------------------------------------------------------------------
/**
 * Saves data for an event boundary in the event table
 *
 * @param forEventFunction Index of the function that is to be saved
 */
//------------------------------------------------------------------------------
void EventLocator::BufferEvent(Integer forEventFunction)
{
   // Build a LocatedEvent structure
   LocatedEvent *theEvent = new LocatedEvent;

   Real *theData = eventFunctions[forEventFunction]->GetData();

   // Only add if it was not the last event bracketed
   if ((theData[0] < earlyBound[forEventFunction]) ||
       (theData[0] > lateBound[forEventFunction]))
   {
      theEvent->epoch = theData[0];
      lastEpochs[forEventFunction] = theEvent->epoch;
      theEvent->eventValue = theData[1];
      theEvent->type = eventFunctions[forEventFunction]->GetTypeName();
      theEvent->participants = eventFunctions[forEventFunction]->GetName();
      theEvent->boundary = eventFunctions[forEventFunction]->GetBoundaryType();
      theEvent->isEntry = eventFunctions[forEventFunction]->IsEventEntry();

      #ifdef DEBUG_EVENTLOCATION
         MessageInterface::ShowMessage("Adding event to event table:\n   "
               "%-20s%-30s%-15s%15.9lf\n", theEvent->type.c_str(),
               theEvent->participants.c_str(), theEvent->boundary.c_str(),
               theEvent->epoch);
      #endif

      eventTable.AddEvent(theEvent);
   }
}


//------------------------------------------------------------------------------
// void EventLocator::BufferEvent(Real epoch, std::string type, bool isStart)
//------------------------------------------------------------------------------
/**
 * Saves data for an event boundary in the event table
 *
 * @param epoch The epoch of the event
 * @param type The event type
 * @param isStart Flag indicating if event is an entry (true) or exit (false)
 *
 * @note: This method is not used in the current code, and not yet implemented
 */
//------------------------------------------------------------------------------
void EventLocator::BufferEvent(Real epoch, std::string type, bool isStart)
{
   throw EventException("BufferEvent(epoch, type, isStart) is not yet "
         "supported");
}


//------------------------------------------------------------------------------
// void ReportEventData()
//------------------------------------------------------------------------------
/**
 * Writes the event data to file and optionally displays the event data plot.
 */
//------------------------------------------------------------------------------
void EventLocator::ReportEventData()
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

   fileWasWritten = eventTable.WriteToFile(fullFileName);
   if (showPlot)
   {
      if (fileWasWritten)
         eventTable.ShowPlot();
      else
         MessageInterface::ShowMessage("No events were found, so the event "
               "plot is not displayed.\n");
   }
}

bool EventLocator::FileWasWritten()
{
   return fileWasWritten;
}


//------------------------------------------------------------------------------
// void ReportEventStatistics()
//------------------------------------------------------------------------------
/**
 * Writes the event data statistics to file.
 */
//------------------------------------------------------------------------------
void EventLocator::ReportEventStatistics()
{
   throw EventException("ReportEventStatistics() is not yet supported");
}


//------------------------------------------------------------------------------
// Real* GetEventData(std::string type, Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Retrieves data for a specified event.
 *
 * @param type The type of the event (Not yet used)
 * @param whichOne Index into the type for the specific event (Not yet used)
 *
 * @return The event data
 */
//------------------------------------------------------------------------------
Real* EventLocator::GetEventData(std::string type, Integer whichOne)
{
   return lastData;
}


//------------------------------------------------------------------------------
// void UpdateEventTable(SortStyle how)
//------------------------------------------------------------------------------
/**
 * Updates the data in the event table, possibly sorting as well
 *
 * @param how The sorting style
 */
//------------------------------------------------------------------------------
void EventLocator::UpdateEventTable(SortStyle how)
{
   throw EventException("UpdateEventTable(how) is not yet supported");
}

//------------------------------------------------------------------------------
// GmatEpoch GetLastEpoch(Integer index)
//------------------------------------------------------------------------------
/**
 * Retrieves the last epoch for a given event
 *
 * @param index Index of the event
 *
 * @return The epoch
 *
 * @note: This method may need refactoring.
 */
//------------------------------------------------------------------------------
GmatEpoch EventLocator::GetLastEpoch(Integer index)
{
   return lastEpochs[index];
}

void EventLocator::SetFoundEventBrackets(Integer index, GmatEpoch early,
      GmatEpoch late)
{
   earlyBound[index] = early;
   lateBound[index]  = late;
}


bool EventLocator::HasAssociatedStateObjects()
{
   return true;
}

std::string EventLocator::GetAssociateName(UnsignedInt val)
{
   std::string retval = "";
   if (val < efCount)
      retval = eventFunctions[val]->GetPrimaryName();
   return retval;
}


//------------------------------------------------------------------------------
// std::string GetTarget(UnsignedInt forFunction)
//------------------------------------------------------------------------------
/**
 * Retrieves the name of the target object
 *
 * @param forFunction Index of the event function being referenced
 *
 * @return The name of the target object
 */
//------------------------------------------------------------------------------
std::string EventLocator::GetTarget(UnsignedInt forFunction)
{
   if (forFunction > efCount)
      throw EventException("Requested event function target is not in the "
            "locator named " + instanceName);

   return eventFunctions[forFunction]->GetPrimaryName();
}

StringArray EventLocator::GetDefaultPropItems()
{
   StringArray propItems;
   propItems.push_back("EventFunction");
   return propItems;
}

Integer EventLocator::SetPropItem(const std::string &propItem)
{
   if (propItem == "EventFunction")
      return Gmat::EVENT_FUNCTION_STATE;

   return Gmat::UNKNOWN_STATE;
}

Integer EventLocator::GetPropItemSize(const Integer item)
{
   if (item == Gmat::EVENT_FUNCTION_STATE)
      return efCount;

   return GmatBase::GetPropItemSize(item);
}

//------------------------------------------------------------------------------
// void SetStateIndices(UnsignedInt forFunction, Integer index,
//       Integer associate)
//------------------------------------------------------------------------------
/**
 * Sets the indices of the parameters in the state vector for an event function
 *
 * @param forFunction Index of the event function being referenced
 * @param index The location of the event function in the state vector
 * @param associate The index of the start of state data for the target object
 */
//------------------------------------------------------------------------------
void EventLocator::SetStateIndices(UnsignedInt forFunction, Integer index,
      Integer associate)
{
   if (forFunction > stateIndices.size())
      throw EventException("Event locator " + instanceName +
            "does not have the requested event function (has it been "
            "initialized?)");

   stateIndices[forFunction]     = index;
   associateIndices[forFunction] = associate;
}
