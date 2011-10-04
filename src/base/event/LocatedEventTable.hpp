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


class OwnedPlot;


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

   void AddEvent(LocatedEvent *theEvent);
   void AddEvent(GmatEpoch epoch, std::string boundaryType, std::string eventType);
   Real GetMaxSpan(std::string eventType, std::string parties);
   Real GetLastSpan(std::string eventType, std::string parties = "");
   Real GetAverageSpan(std::string eventType, std::string parties = "");
   void SortEvents(SortStyle how, SortStyle secondaryStyle);
   std::vector<LocatedEvent*> *GetEvents();

   bool WriteToFile(std::string filename);
   void ShowPlot();
   void BuildPlot(const std::string &plotName);

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
   /// List of the types of events
   StringArray eventTypesWithNames;
   /// Plot of the event data
   OwnedPlot *thePlot;
   /// X Data for plotting
   std::map<std::string,RealArray> xData;
   /// Y Data for plotting
   std::map<std::string,RealArray> yData;

   void BuildAssociations();
   void SortEvents();
   std::string BuildEventSummary();
   void CollectData(const std::string &forCurve, RealArray &xv, RealArray &yv);
};

#endif /* LocatedEventTable_hpp */
