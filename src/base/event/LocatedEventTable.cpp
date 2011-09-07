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


#include "LocatedEventTable.hpp"

LocatedEventTable::LocatedEventTable()
{
}

LocatedEventTable::~LocatedEventTable()
{
}

LocatedEventTable::LocatedEventTable(const LocatedEventTable& let)
{

}

LocatedEventTable& LocatedEventTable::operator=(const LocatedEventTable& let)
{
   if (this != &let)
   {

   }

   return *this;
}

/// Adds a new event entry to the table of events.
void LocatedEventTable::AddEvent(GmatEpoch epoch, std::string boundaryType,
      std::string eventType)
{
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
   return false;
}

/// Accessor function that allows for retrieving the event data directly.
std::vector<LocatedEvent*> *LocatedEventTable::GetEvents()
{
   return &events;
}
