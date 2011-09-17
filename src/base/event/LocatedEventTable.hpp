//$Id$
//------------------------------------------------------------------------------
//                           LocatedEventTable
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
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#ifndef LocatedEventTable_hpp
#define LocatedEventTable_hpp

#include "LocatedEvent.hpp"


enum SortStyle
{
   CHRONOLOGICAL = 12000,  // Sorts the event data in time order.
   BY_TYPE,                // Groups the data by event type.
   DURATION_ASCENDING,     // Groups the event data from shortest to longest.
   DURATION_DESCENDING,    // Groups the event data from longest to shortest.
   BY_START,               // Sorts by start epoch
   UNSORTED                // Uses the current buffer ordering
};

/**
 * The table of events located during a run
 */
class GMAT_API LocatedEventTable
{
public:
   LocatedEventTable();
   virtual ~LocatedEventTable();
   LocatedEventTable(const LocatedEventTable& let);
   LocatedEventTable& operator=(const LocatedEventTable& let);

   /// Adds a new event entry to the table of events.
   void AddEvent(LocatedEvent *theEvent);
   /// Adds a new event entry to the table of events.
   void AddEvent(GmatEpoch epoch, std::string boundaryType, std::string eventType);
   /// Returns the longest duration for the detected events of the specified type.
   Real GetMaxSpan(std::string eventType, std::string parties);
   /// Returns the duration of the most recent detected event of the specified type.
   Real GetLastSpan(std::string eventType, std::string parties = "");
   /// Returns the average duration for the detected events of the specified type.
   Real GetAverageSpan(std::string eventType, std::string parties = "");
   /// Sets flags to sort the event data in the specified primary and secondary order when generating the event data file.
   void SortEvents(SortStyle how, SortStyle secondaryStyle);
   /// Writes the event data to an event data file with the specified name.
   bool WriteToFile(std::string filename);
   /// Accessor function that allows for retrieving the event data directly.
   std::vector<LocatedEvent*> *GetEvents();

protected:
   /// The table of located event boundaries
   std::vector<LocatedEvent*>    events;
   /// Main sort style
   SortStyle primarySortStyle;
   /// Secondary sort style
   SortStyle secondarySortStyle;
   /// The report order for the events
   std::vector<UnsignedInt> sortOrder;
   /// Flag indicating stale associations
   bool associationsCurrent;

   void BuildAssociations();
   void SortEvents();
   std::string BuildEventSummary();
};

#endif /* LocatedEventTable_hpp */
