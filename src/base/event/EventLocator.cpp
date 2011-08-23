/*
 * EventLocator.cpp
 *
 *  Created on: Aug 16, 2011
 *      Author: djc
 */

#include "EventLocator.hpp"

EventLocator::EventLocator(const std::string &typeStr,
      const std::string &nomme) :
   GmatBase       (Gmat::EVENT_LOCATOR, typeStr, nomme)
{
   // TODO Auto-generated constructor stub

}

EventLocator::~EventLocator()
{
   // TODO Auto-generated destructor stub
}

EventLocator::EventLocator(const EventLocator& el):
   GmatBase          (el)
{

}

EventLocator& EventLocator::operator=(const EventLocator& el)
{
   if (this != &el)
   {

   }

   return *this;
}

Real *EventLocator::Evaluate()
{
   Real *vals;
   // Loop through the event functions, evaluating each and storing their data
   for (UnsignedInt i = 0; i < eventFunctions.size(); ++i)
   {
      vals = eventFunctions[i]->Evaluate();
   }
   return eventData;
}

/// Adds an event to the LocatedEventTable.
void EventLocator::BufferEvent(Real epoch, std::string type, bool isStart)
{

}

/// Writes the event data to file.
void EventLocator::ReportEventData()
{

}

/// EventLocator::Writes the event data statistics to file.
void EventLocator::ReportEventStatistics()
{

}

/// Retrieves data for a specified event.
Real EventLocator::GetEventData(std::string type, Integer whichOne)
{
   Real retval = 99999.99999;

   return retval;
}

/// Updates the data in the event table, possibly sorting as well
void EventLocator::UpdateEventTable(SortStyle how)
{

}
