/*
 * EventLocator.hpp
 *
 *  Created on: Aug 16, 2011
 *      Author: djc
 */

#ifndef EVENTLOCATOR_HPP_
#define EVENTLOCATOR_HPP_

#include "GmatBase.hpp"
#include "LocatedEventTable.hpp"
#include "EventFunction.hpp"


class EventLocator: public GmatBase
{
public:
   EventLocator(const std::string &typeStr, const std::string &nomme = "");
   virtual ~EventLocator();
   EventLocator(const EventLocator& el);
   EventLocator& operator=(const EventLocator& el);

   /// Evaluates contained EventFunctions and returns values and derivatives.
   virtual Real *Evaluate();
   /// Adds an event to the LocatedEventTable.
   void BufferEvent(Real epoch, std::string type, bool isStart);
   /// Writes the event data to file.
   void ReportEventData();
   /// Writes the event data statistics to file.
   void ReportEventStatistics();
   /// Retrieves data for a specified event.
   Real GetEventData(std::string type, Integer whichOne = 0);
   /// Updates the data in the event table, possibly sorting as well
   void UpdateEventTable(SortStyle how);

protected:

   std::vector<EventFunction*> eventFunctions;
   std::vector<Real> maxSpan;
   std::vector<Real>lastSpan;
   LocatedEventTable eventTable;
   std::string filename;
   Real *eventData;
};

#endif /* EVENTLOCATOR_HPP_ */
