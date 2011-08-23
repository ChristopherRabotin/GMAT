/*
 * LocatedEventTable.hpp
 *
 *  Created on: Aug 16, 2011
 *      Author: djc
 */

#ifndef LOCATEDEVENTTABLE_HPP_
#define LOCATEDEVENTTABLE_HPP_

#include "LocatedEvent.hpp"


enum SortStyle
{
   CHRONOLOGICAL,       // Sorts the event data in time order.
   BY_TYPE,             // Groups the data by event type.
   DURATION_ASCENDING,  // Groups the event data from shortest to longest.
   DURATION_DESCENDING, // Groups the event data from longest to shortest.
   UNSORTED             // Uses the current buffer ordering
};



class LocatedEventTable
{
public:
   LocatedEventTable();
   virtual ~LocatedEventTable();
   LocatedEventTable(const LocatedEventTable& let);
   LocatedEventTable& operator=(const LocatedEventTable& let);

   /// Adds a new event entry to the table of events.
   void AddEvent(GmatEpoch epoch, std::string boundaryType, std::string eventType);
   /// Returns the longest duration for the detected events of the specified type.
   Real GetMaxSpan(std::string eventType);
   /// Returns the duration of the most recent detected event of the specified type.
   Real GetLastSpan(std::string eventType);
   /// Returns the average duration for the detected events of the specified type.
   Real GetAverageSpan(std::string eventType);
   /// Sets flags to sort the event data in the specified primary and secondary order when generating the event data file.
   void SortEvents(SortStyle how, SortStyle secondaryStyle);
   /// Writes the event data to an event data file with the specified name.
   bool WriteToFile(std::string filename);
   /// Accessor function that allows for retrieving the event data directly.
   std::vector<LocatedEvent*> *GetEvents();

protected:
   /// The table of located event boundaries
   std::vector<LocatedEvent*>    events;
};

#endif /* LOCATEDEVENTTABLE_HPP_ */
