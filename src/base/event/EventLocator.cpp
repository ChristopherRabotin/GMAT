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
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#include "EventLocator.hpp"
#include "EventException.hpp"

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
const std::string
EventLocator::PARAMETER_TEXT[EventLocatorParamCount - GmatBaseParamCount] =
{
   "Spacecraft",        // SATNAMES,
   "Tolerance",         // TOLERANCE,
   "Filename"           // EVENT_FILENAME,
};

const Gmat::ParameterType
EventLocator::PARAMETER_TYPE[EventLocatorParamCount - GmatBaseParamCount] =
{
   Gmat::STRINGARRAY_TYPE,       // SATNAMES,
   Gmat::REAL_TYPE,              // TOLERANCE,
   Gmat::STRING_TYPE             // EVENT_FILENAME,
};


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------


EventLocator::EventLocator(const std::string &typeStr,
      const std::string &nomme) :
   GmatBase       (Gmat::EVENT_LOCATOR, typeStr, nomme),
   eventTolerance (1.0e-3),
   eventFile      ("LocatedEvents.txt")
{
   objectTypes.push_back(Gmat::EVENT_LOCATOR);
   objectTypeNames.push_back("EventLocator");
}

EventLocator::~EventLocator()
{
}

EventLocator::EventLocator(const EventLocator& el):
   GmatBase          (el),
   satNames          (el.satNames),
   eventTolerance    (el.eventTolerance),
   eventFile         (el.eventFile)
{

}

EventLocator& EventLocator::operator=(const EventLocator& el)
{
   if (this != &el)
   {
      GmatBase::operator =(el);

      satNames       = el.satNames;
      eventTolerance = el.eventTolerance;
      eventFile      = el.eventFile;
   }

   return *this;
}


std::string EventLocator::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EventLocatorParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}

Integer EventLocator::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < EventLocatorParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
}

Gmat::ParameterType EventLocator::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < EventLocatorParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}

std::string EventLocator::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

bool EventLocator::IsParameterReadOnly(const Integer id) const
{
   return GmatBase::IsParameterReadOnly(id);
}

bool EventLocator::IsParameterReadOnly(const std::string &label) const
{
   return GmatBase::IsParameterReadOnly(label);
}

Real EventLocator::GetRealParameter(const Integer id) const
{
   if (id == TOLERANCE)
      return eventTolerance;

   return GmatBase::GetRealParameter(id);
}

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

   return GmatBase::SetRealParameter(id, value);
}

Real EventLocator::GetRealParameter(const Integer id, const Integer index) const
{
   return GmatBase::GetRealParameter(id, index);
}

Real EventLocator::GetRealParameter(const Integer id, const Integer row,
                                      const Integer col) const
{
   return GmatBase::GetRealParameter(id, row, col);
}

Real EventLocator::SetRealParameter(const Integer id, const Real value,
      const Integer index)
{
   return GmatBase::SetRealParameter(id, value, index);
}

Real EventLocator::SetRealParameter(const Integer id, const Real value,
                                      const Integer row, const Integer col)
{
   return GmatBase::SetRealParameter(id, value, row, col);
}

Real EventLocator::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

Real EventLocator::SetRealParameter(const std::string &label,
                                      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}

Real EventLocator::GetRealParameter(const std::string &label,
                                      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}

Real EventLocator::SetRealParameter(const std::string &label,
                                      const Real value,
                                      const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}

Real EventLocator::GetRealParameter(const std::string &label,
                                      const Integer row,
                                      const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}

Real EventLocator::SetRealParameter(const std::string &label,
                                      const Real value, const Integer row,
                                      const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}

std::string EventLocator::GetStringParameter(const Integer id) const
{
   if (id == EVENT_FILENAME)
      return eventFile;

   return GmatBase::GetStringParameter(id);
}

bool EventLocator::SetStringParameter(const Integer id,
                                        const std::string &value)
{
   if (id == EVENT_FILENAME)
   {
      if (value != "")
      {
         eventFile = value;
         return true;
      }
      return false;
   }

   return GmatBase::SetStringParameter(id, value);
}

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

bool EventLocator::SetStringParameter(const Integer id,
                                        const std::string &value,
                                        const Integer index)
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
         return true;
      }
   }

   return GmatBase::SetStringParameter(id, value, index);
}

const StringArray& EventLocator::GetStringArrayParameter(const Integer id) const
{
   if (id == SATNAMES)
      return satNames;

   return GmatBase::GetStringArrayParameter(id);
}

const StringArray& EventLocator::GetStringArrayParameter(const Integer id,
                                             const Integer index) const
{
   return GmatBase::GetStringArrayParameter(id, index);
}

std::string EventLocator::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

bool EventLocator::SetStringParameter(const std::string &label,
                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

std::string EventLocator::GetStringParameter(const std::string &label,
                                        const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

bool EventLocator::SetStringParameter(const std::string &label,
                                        const std::string &value,
                                        const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

const StringArray& EventLocator::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

const StringArray& EventLocator::GetStringArrayParameter(const std::string &label,
                                             const Integer index) const
{
   return GetStringArrayParameter(GetParameterID(label), index);
}

bool EventLocator::Initialize()
{
   bool retval = false;

   return retval;
}




/// Evaluates the EventFunctions and returns their values and derivatives.
Real *EventLocator::Evaluate()
{
   Real *vals;
   // Loop through the event functions, evaluating each and storing their data
   for (UnsignedInt i = 0; i < eventFunctions.size(); ++i)
   {
      vals = eventFunctions[i]->Evaluate();
   }
   return lastData;
}

/// Adds an event to the LocatedEventTable.
void EventLocator::BufferEvent(Real epoch, std::string type, bool isStart)
{

}

/// Writes the event data to file.
void EventLocator::ReportEventData()
{

}

/// Writes the event data statistics to file.
void EventLocator::ReportEventStatistics()
{

}

/// Retrieves data for a specified event.
Real* EventLocator::GetEventData(std::string type, Integer whichOne)
{
   return lastData;
}

/// Updates the data in the event table, possibly sorting as well
void EventLocator::UpdateEventTable(SortStyle how)
{

}
