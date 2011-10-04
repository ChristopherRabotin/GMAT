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
 * Implementation of the table of events located by an EventLocator
 */
//------------------------------------------------------------------------------


#include "LocatedEventTable.hpp"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "OwnedPlot.hpp"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>          // For find()


//------------------------------------------------------------------------------
// LocatedEventTable()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
LocatedEventTable::LocatedEventTable() :
   primarySortStyle        (UNSORTED),
   secondarySortStyle      (UNSORTED),
   associationsCurrent     (false),
   thePlot                 (NULL)
{
}

//------------------------------------------------------------------------------
// ~LocatedEventTable()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
LocatedEventTable::~LocatedEventTable()
{
   for (UnsignedInt i = 0; i < events.size(); ++i)
      delete events[i];

   // Here or elsewhere?
//   delete thePlot;
}

//------------------------------------------------------------------------------
// LocatedEventTable(const LocatedEventTable& let) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param let The table being copied here
 */
//------------------------------------------------------------------------------
LocatedEventTable::LocatedEventTable(const LocatedEventTable& let) :
   events                  (let.events),
   primarySortStyle        (let.primarySortStyle),
   secondarySortStyle      (let.secondarySortStyle),
   associationsCurrent     (false),
   thePlot                 (NULL)
{
}

//------------------------------------------------------------------------------
// LocatedEventTable& operator=(const LocatedEventTable& let)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param let The table being copied here
 *
 * @return this LocatedEventTable, populated to match let
 */
//------------------------------------------------------------------------------
LocatedEventTable& LocatedEventTable::operator=(const LocatedEventTable& let)
{
   if (this != &let)
   {
      events              = let.events;
      primarySortStyle    = let.primarySortStyle;
      secondarySortStyle  = let.secondarySortStyle;
      associationsCurrent = false;
      thePlot             = NULL;
      sortOrder.clear();
   }

   return *this;
}

//------------------------------------------------------------------------------
// void AddEvent(LocatedEvent *theEvent)
//------------------------------------------------------------------------------
/**
 * Adds a located event boundary to the table
 *
 * @param theEvent The event boundary
 */
//------------------------------------------------------------------------------
void LocatedEventTable::AddEvent(LocatedEvent *theEvent)
{
   events.push_back(theEvent);
   associationsCurrent = false;
}


//------------------------------------------------------------------------------
// void AddEvent(GmatEpoch epoch, std::string boundaryType,
//       std::string eventType)
//------------------------------------------------------------------------------
/**
 * Adds a new event entry to the table of events.
 *
 * @param epoch The epoch of the event boundary
 * @param boundaryType The type of boundary (entry/exit)
 * @param eventType The type of the event
 *
 * @note This method is deprecated  by the version above
 */
//------------------------------------------------------------------------------
void LocatedEventTable::AddEvent(GmatEpoch epoch, std::string boundaryType,
      std::string eventType)
{
   // Build a LocatedEvent for the supplied data
   LocatedEvent *theEvent = new LocatedEvent;
   theEvent->epoch = epoch;
   theEvent->boundary = boundaryType;
   theEvent->type = eventType;

   // Save it
   events.push_back(theEvent);
   associationsCurrent = false;
}


//------------------------------------------------------------------------------
// Real GetMaxSpan(std::string eventType, std::string parties)
//------------------------------------------------------------------------------
/**
 * Returns longest duration for the detected events of the specified type.
 *
 * @param eventType The type of the event
 * @param parties The participant descriptor for the event
 *
 * @return The duration found.
 */
//------------------------------------------------------------------------------
Real LocatedEventTable::GetMaxSpan(std::string eventType, std::string parties)
{
   Real retval = 0.0;

   if (associationsCurrent == false)
      BuildAssociations();

   for (UnsignedInt i = 0; i < events.size(); ++i)
   {
      if ((events[i]->type == eventType) &&
          (events[i]->participants == parties) &&
          (events[i]->duration > retval))
         retval = events[i]->duration;
   }

   return retval;
}



//------------------------------------------------------------------------------
// Real GetLastSpan(std::string eventType, std::string parties)
//------------------------------------------------------------------------------
/**
 * Returns duration of the most recent detected event of the specified type.
 *
 * @param eventType The type of event being sought
 * @param parties (optional) The participant string for the event being sought
 *                If omitted (or the empty string), only the event type is used
 *
 * @return The duration of the most recent span of the specified type.  Note
 *         that only completed spans are checked -- if an entry exists with no
 *         exit, or an exit with no entry, it is skipped.
 */
//------------------------------------------------------------------------------
Real LocatedEventTable::GetLastSpan(std::string eventType, std::string parties)
{
   Real retval = 0.0;

   if (associationsCurrent == false)
      BuildAssociations();

   GmatEpoch ep = 0.0;
   Integer index = -1;
   bool checkMe;

   for (UnsignedInt i = 0; i < events.size(); ++i)
   {
      checkMe = false;
      if ((events[i]->type == eventType) &&
          (events[i]->partner != NULL))
      {
         if (parties != "")
         {
            if (events[i]->participants == parties)
               checkMe = true;
         }
         else
            checkMe = true;

         if (checkMe && (events[i]->epoch > ep))
            index = i;
      }
   }

   if (index >= 0)
      retval = events[index]->duration;

   return retval;
}



//------------------------------------------------------------------------------
// Real GetAverageSpan(std::string eventType, std::string parties)
//------------------------------------------------------------------------------
/**
 * Returns average duration for the detected events of the specified type.
 *
 * @param eventType The type of event being sought
 * @param parties (optional) The participant string for the event being sought
 *                If omitted (or the empty string), only the event type is used
 *
 * @return The average duration of the most spans of the specified type.  Note
 *         that only completed spans are checked -- if an entry exists with no
 *         exit, or an exit with no entry, it is skipped.
 */
//------------------------------------------------------------------------------
Real LocatedEventTable::GetAverageSpan(std::string eventType,
      std::string parties)
{
   Real retval = 0.0;

   if (associationsCurrent == false)
      BuildAssociations();

   Real total = 0.0;
   Integer count = 0;

   for (UnsignedInt i = 0; i < events.size(); ++i)
   {
      if ((events[i]->type == eventType) &&
          (events[i]->partner != NULL))
      {
         if (parties != "")
         {
            if (events[i]->participants == parties)
            {
               total += events[i]->duration;
               ++count;
            }
         }
         else
         {
            total += events[i]->duration;
            ++count;
         }
      }
   }

   if (count > 0)
      retval = total / count;

   return retval;
}



//------------------------------------------------------------------------------
// void SortEvents(SortStyle how, SortStyle secondaryStyle)
//------------------------------------------------------------------------------
/**
 * Sets flags to sort the event data in the specified order.
 *
 * @param how The primary sorting style
 * @param secondaryStyle The secondary style
 */
//------------------------------------------------------------------------------
void LocatedEventTable::SortEvents(SortStyle how, SortStyle secondaryStyle)
{
   primarySortStyle = how;
   secondarySortStyle = secondaryStyle;
}



//------------------------------------------------------------------------------
// bool WriteToFile(std::string filename)
//------------------------------------------------------------------------------
/**
 * Writes the event data to an event data file with the specified name.
 *
 * @param filename The name of the output file
 *
 * @return true if the file was written, false if not
 */
//------------------------------------------------------------------------------
bool LocatedEventTable::WriteToFile(std::string filename)
{
   bool retval = false;

   if (events.size() > 0)
   {
      std::ofstream theFile;
      theFile.open(filename.c_str());
      // Write the header labels
      theFile << "Type        Participants              Duration (sec)   "
                 "UTC Start Time             UTC End Time\n"
                 "--------    ------------              --------------   "
                 "------------------------   ------------------------\n";

      char pass[256];
      GmatEpoch start = 0.0;

      SortEvents();
      LocatedEvent *current, *partner;

      for (UnsignedInt i = 0; i < sortOrder.size(); ++i)
      {
         pass[0] = '\0';
         current = events[sortOrder[i]];
         partner = current->partner;

         // Used to build the UTC strings
         std::string instr, startstr, endstr;
         Real outval;

         if (current->isEntry)
         {
            sprintf(pass, "%-11s %-25s ", current->type.c_str(),
                  current->participants.c_str());
            start = current->epoch;
            TimeConverterUtil::Convert("A1ModJulian", start, instr,
                  "UTCGregorian", outval, startstr, 1);

            if (partner != NULL)
            {
               TimeConverterUtil::Convert("A1ModJulian", partner->epoch, instr,
                     "UTCGregorian", outval, endstr, 1);

               sprintf(pass, "%s%-12.6lf     %24s   %24s\n", pass,
                     current->duration, startstr.c_str(), endstr.c_str());
            }
            else
            {
               sprintf(pass, "%sUndefined        %24s   --------------------"
                     "----\n", pass, startstr.c_str());
            }
         }
         else
         {
            sprintf(pass, "%-11s %-25s ", current->type.c_str(),
                  current->participants.c_str());
            start = current->epoch;
            TimeConverterUtil::Convert("A1ModJulian", start, instr,
                  "UTCGregorian", outval, endstr, 1);
            sprintf(pass, "%sUndefined        ------------------------   "
                  "%24s\n", pass, endstr.c_str());
         }

         theFile << pass;
      }

      std::string summary = BuildEventSummary();
      theFile << summary;

      theFile.close();
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// std::vector<LocatedEvent*> *GetEvents()
//------------------------------------------------------------------------------
/**
 * Accessor function that allows for retrieving the event data directly.
 *
 * @return The vector of LocatedEvent data.
 */
//------------------------------------------------------------------------------
std::vector<LocatedEvent*> *LocatedEventTable::GetEvents()
{
   return &events;
}


//------------------------------------------------------------------------------
// void BuildAssociations()
//------------------------------------------------------------------------------
/**
 * Connects start and end events together
 *
 * @note: The current implementation is moderately crude.  It assumes the events
 *        vector is in chronological order, and that any start event followed by
 *        an end event match as long as the type and participant list match.  It
 *        does not account for toggle-off conditions, nor does it handle either
 *        backwards propagation nor loops that reset the epoch.  These are all
 *        affects that must be handled with a code update.
 */
//------------------------------------------------------------------------------
void LocatedEventTable::BuildAssociations()
{
   // Clear the partnership links
   for (UnsignedInt i = 0; i < events.size(); ++i)
   {
      events[i]->partner = NULL;
      events[i]->duration = 0.0;
   }

   xData.clear();
   yData.clear();

   // Build the links
   for (UnsignedInt i = 0; i < events.size(); ++i)
   {
      events[i]->dataName = events[i]->type + "-" + events[i]->participants;
      if (events[i]->isEntry)
      {
         Integer mate = -1;
         GmatEpoch currentEpoch = events[i]->epoch;
         GmatEpoch mateEpoch = 9e99;

         for (UnsignedInt j = i+1; j < events.size(); ++j)
         {
            if ((!(events[j]->isEntry)) &&
                (events[i]->type == events[j]->type) &&
                (events[i]->participants == events[j]->participants))
            {
               if ((events[j]->epoch < mateEpoch) &&
                   (events[j]->epoch > currentEpoch))
               {
                  mateEpoch = events[j]->epoch;
                  mate = (Integer)j;
               }
            }
         }
         if (mate > 0)
         {
            events[i]->partner = events[mate];
            events[mate]->partner = events[i];
            Real duration = (events[mate]->epoch - events[i]->epoch) *
                  GmatTimeConstants::SECS_PER_DAY;
            events[i]->duration = duration;
            events[mate]->duration = duration;
         }
      }
   }

   associationsCurrent = true;

   #ifdef DEBUG_EVENTLOCATION
      for (UnsignedInt i = 0; i < events.size(); ++i)
      {
         if (events[i]->partner != NULL)
            MessageInterface::ShowMessage("%s %s %16.10lf %s %s %s %16.10lf\n",
                  events[i]->type.c_str(), events[i]->participants.c_str(),
                  events[i]->epoch,
                  (events[i]->isEntry ? "starts for" : "closes for"),
                  events[i]->partner->type.c_str(),
                  events[i]->partner->participants.c_str(),
                  events[i]->partner->epoch
                  );
         else
            MessageInterface::ShowMessage("%s %s %16.10lf %s no partner\n",
                  events[i]->type.c_str(), events[i]->participants.c_str(),
                  events[i]->epoch,
                  (events[i]->isEntry ? "starts for" : "closes for")
                  );
      }
   #endif
}

//------------------------------------------------------------------------------
// void SortEvents()
//------------------------------------------------------------------------------
/**
 * Builds event associations and then sorts the events
 *
 * @note Sorting is not yet implemented; ordering is just the order of the
 *       events vector
 */
//------------------------------------------------------------------------------
void LocatedEventTable::SortEvents()
{
   sortOrder.clear();

   // Link up the partners
   BuildAssociations();

   // Build the report order -- for now, just use the order of the events vector
   for (UnsignedInt i = 0; i < events.size(); ++i)
   {
      if ((events[i]->isEntry) || (events[i]->partner == NULL))
      {
         sortOrder.push_back(i);
      }
   }
}


//------------------------------------------------------------------------------
// std::string BuildEventSummary()
//------------------------------------------------------------------------------
/**
 * Builds the summary string
 *
 * @return The summary string, written at the end of the event report file
 */
//------------------------------------------------------------------------------
std::string LocatedEventTable::BuildEventSummary()
{
   std::string summary = "\nEvent Report Summary\n--------------------\n";

   StringArray eventTypes;
   StringArray eventNames;
//   std::map<std::string, std::string> typeNameMap;
   std::string name;

   eventTypesWithNames.clear();

   // Build the list of types
   for (UnsignedInt i = 0; i < events.size(); ++i)
   {
      if (find(eventTypes.begin(), eventTypes.end(), events[i]->type) ==
            eventTypes.end())
         eventTypes.push_back(events[i]->type);
   }

   Integer count;
   char data[256], sub[128];
   std::string parties;
   Real span;

   // Find maxima, type by type
   for (UnsignedInt i = 0; i < eventTypes.size(); ++i)
   {
      eventNames.clear();
      span = 0.0;
      sprintf(sub, "  Max %s Duration", eventTypes[i].c_str());
      for (UnsignedInt j = 0; j < events.size(); ++j)
      {
         if ((events[j]->isEntry) && (events[j]->partner != NULL) &&
             (find(eventNames.begin(), eventNames.end(),
              events[j]->participants) == eventNames.end()))
         {
            parties = events[j]->participants;
            eventNames.push_back(parties);
            span = GetMaxSpan(eventTypes[i], parties);
            if (span > 0.0)
            {
               eventTypesWithNames.push_back(events[j]->dataName);
               sprintf(data, "%-34s: %12.3lf s (%s)\n", sub, span,
                     parties.c_str());
               summary += data;
               sprintf(sub, " ");
            }
         }
      }
   }

   summary += "\n";

   // Count up the events
   for (UnsignedInt i = 0; i < eventTypes.size(); ++i)
   {
      count = 0;
      for (UnsignedInt j = 0; j < events.size(); ++j)
      {
         if ((events[j]->isEntry) && (events[j]->partner != NULL) &&
             (events[j]->type == eventTypes[i]))
            ++count;
      }
      sprintf(sub, "  Number of %s Events", eventTypes[i].c_str());
      sprintf(data, "%-34s: %d\n", sub, count);
      summary += data;
   }

   return summary;
}


void LocatedEventTable::ShowPlot()
{
   BuildPlot("Event Data");
}

//------------------------------------------------------------------------------
// void BuildPlot(const std::string &plotName,
//       const StringArray &measurementNames)
//------------------------------------------------------------------------------
/**
 * Creates an OwnedPlot instance that is used for plotting residuals
 *
 * @param plotName The name of the plot.  This name needs to ne unique in the
 *                 Sandbox
 * @param measurementNames The names of the measurement models that are sources
 *                         for the residuals being plotted
 */
//------------------------------------------------------------------------------
void LocatedEventTable::BuildPlot(const std::string &plotName)
{
   // Commented out for stability

   thePlot = new OwnedPlot(plotName);

   thePlot->SetStringParameter("PlotTitle", plotName);
   thePlot->SetBooleanParameter("UseLines", false);
   thePlot->SetBooleanParameter("UseHiLow", false);
   // Turn on automatic marker colors
   thePlot->SetIntegerParameter(thePlot->GetParameterID("DefaultColor"), 0);

   eventTypesWithNames.clear();
   for (UnsignedInt i = 0; i < events.size(); ++i)
      if (find(eventTypesWithNames.begin(), eventTypesWithNames.end(),
            events[i]->dataName) == eventTypesWithNames.end())
         eventTypesWithNames.push_back(events[i]->dataName);

   for (UnsignedInt i = 0; i < eventTypesWithNames.size(); ++i)
   {
      std::string curveName = eventTypesWithNames[i];
      thePlot->SetStringParameter("Add", curveName);
   }

   thePlot->Initialize();

   for (UnsignedInt i = 0; i < eventTypesWithNames.size(); ++i)
   {
      // Load up the data
      RealArray xd, yd;
      CollectData(eventTypesWithNames[i], xd, yd); 
      
      if (xd.size() > 0)
         thePlot->SetCurveData(i, &xd, &yd);
   }
}


//------------------------------------------------------------------------------
// void CollectData(const std::string &forCurve, RealArray &xv, RealArray &yv)
//------------------------------------------------------------------------------
/**
 * Collects data for an event plot curve
 *
 * @param forCurve The label associated with teh curve that receives the data
 * @param xv The container for the X data
 * @param yv The container for the Y data
 */
//------------------------------------------------------------------------------
void LocatedEventTable::CollectData(const std::string &forCurve, RealArray &xv,
      RealArray &yv)
{
   xv.clear();
   yv.clear();

   for (UnsignedInt i = 0; i < events.size(); ++i)
   {
      if ((events[i]->dataName == forCurve) && (events[i]->partner != NULL))
      {
         if (events[i]->isEntry)
         {
            xv.push_back(events[i]->epoch);
            yv.push_back(events[i]->duration);
         }
      }
   }
}
