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
#include <fstream>
#include <sstream>
#include <iomanip>
#include "MessageInterface.hpp"

LocatedEventTable::LocatedEventTable()
{
}

LocatedEventTable::~LocatedEventTable()
{
   for (UnsignedInt i = 0; i < events.size(); ++i)
      delete events[i];
}

LocatedEventTable::LocatedEventTable(const LocatedEventTable& let) :
   events         (let.events)
{
}

LocatedEventTable& LocatedEventTable::operator=(const LocatedEventTable& let)
{
   if (this != &let)
   {
      events = let.events;
   }

   return *this;
}

void LocatedEventTable::AddEvent(LocatedEvent *theEvent)
{
   events.push_back(theEvent);
}

/// Adds a new event entry to the table of events.
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
}


/// Returns longest duration for the detected events of the specified type.
Real LocatedEventTable::GetMaxSpan(std::string eventType)
{
   return 0.0;
}


/// Returns duration of the most recent detected event of the specified type.
Real LocatedEventTable::GetLastSpan(std::string eventType)
{
   return 0.0;
}


/// Returns average duration for the detected events of the specified type.
Real LocatedEventTable::GetAverageSpan(std::string eventType)
{
   return 0.0;
}


/// Sets flags to sort the event data in the specified order.
void LocatedEventTable::SortEvents(SortStyle how, SortStyle secondaryStyle)
{

}


/// Writes the event data to an event data file with the specified name.
bool LocatedEventTable::WriteToFile(std::string filename)
{
      MessageInterface::ShowMessage("Writing located event data to the "
            "file %s\n", filename.c_str());

   bool retval = false;

   if (events.size() > 0)
   {
      std::ofstream theFile;
      theFile.open(filename.c_str());
      theFile << "Type        Participants              Duration (sec)   "
                 "UTC Start Time             UTC End Time\n"
                 "--------    ------------              --------------   "
                 "------------------------   ------------------------\n";

      char pass[256];
      GmatEpoch start = 0.0, span;

      for (UnsignedInt i = 0; i < events.size(); ++i)
      {
         if (events[i]->isEntry)
         {
            sprintf(pass, "%-11s %-25s ", events[i]->type.c_str(),
                  events[i]->participants.c_str());
            start = events[i]->epoch;
         }
         else if (start > 0.0)
         {
            span = (events[i]->epoch - start) * 86400.0;
            sprintf(pass, "%s%-12.6lf     %-16.9lf           %-16.9lf\n", pass,
                  span, start, events[i]->epoch);
            theFile << pass;
         }
      }
      theFile.close();
      retval = true;
   }

   return retval;
}

/// Accessor function that allows for retrieving the event data directly.
std::vector<LocatedEvent*> *LocatedEventTable::GetEvents()
{
   return &events;
}
