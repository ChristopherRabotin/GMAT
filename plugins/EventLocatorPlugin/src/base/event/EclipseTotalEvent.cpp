//$Id$
//------------------------------------------------------------------------------
//                               EclipseTotalEvent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2015.01.12
//
/**
 * Implementation for the EclipseTotalEvent base class.
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <iomanip>
#include "EclipseTotalEvent.hpp"
#include "EventException.hpp"
#include "StringUtil.hpp"
#include "GmatConstants.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

EclipseTotalEvent::EclipseTotalEvent() :
   LocatedEvent  (),
   numEvents     (0),
   theIndex      (-1)
{
}

EclipseTotalEvent::~EclipseTotalEvent()
{
   // nothing to do here
}

EclipseTotalEvent::EclipseTotalEvent(const EclipseTotalEvent& copy) :
   LocatedEvent  (copy),
   theIndex      (copy.theIndex)
{
   TakeAction("Clear");
}

EclipseTotalEvent& EclipseTotalEvent::operator=(const EclipseTotalEvent& copy)
{
   if (&copy != this)
   {
      LocatedEvent::operator=(copy);
      theIndex      = copy.theIndex;
   }

   return *this;
}

bool EclipseTotalEvent::AddEvent(EclipseEvent* newEvent)
{
   // #todo check here first to make sure the event is not
   // already in the list
   theEvents.push_back(newEvent);
   numEvents++;
   return true;
}

bool EclipseTotalEvent::SetEvent(Integer atIndex, EclipseEvent *toEvent, bool deleteExisting)
{
   if ((atIndex < 0) || (atIndex >= numEvents))
      throw EventException("Index out-of-range for EclipseTotalEvent.\n");

   if (theEvents.at(atIndex) != toEvent)
   {
      // don't want to do this deletion in most cases
      if (deleteExisting) delete theEvents.at(atIndex);
      theEvents.at(atIndex) = toEvent;
   }
   return true;
}

Integer EclipseTotalEvent::NumberOfEvents()
{
   return theEvents.size();
}

Real EclipseTotalEvent::GetDuration()
{
   // recompute start and end here from the events in the list <<<< TBD
   // for now, events are ordered as they are put into the list and
   // start and end are set by FindEvents
   duration = (end - start) * GmatTimeConstants::SECS_PER_DAY;
   return duration;
}


void EclipseTotalEvent::SetIndex(Integer i)
{
   theIndex = i;
}

EclipseEvent* EclipseTotalEvent::GetEvent(Integer atIndex)
{
   if ((atIndex < 0) || (atIndex >= numEvents))
      throw EventException("Index out-of-range for EclispeTotalEvent.\n");

   return theEvents.at(atIndex);
}

std::string EclipseTotalEvent::GetReportString()
{
   std::stringstream totalString("");

   Integer sz = theEvents.size();
   // Loop over the total events list
   for (Integer ii = 0; ii < sz; ii++)
   {
      EclipseEvent* ev = theEvents.at(ii);
      std::string eventString = ev->GetReportString();
      Real        totalD      = GetDuration();
      totalString <<  eventString << "    ";
      totalString.setf(std::ios::left);
      totalString.width(5);
      totalString << (theIndex + 1)   << "       " <<
                     GmatStringUtil::BuildNumber(totalD, false, 14) << "\n";
   }
   return totalString.str();
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
bool EclipseTotalEvent::TakeAction(const std::string &action,
                                   const std::string &actionData)
{
   if (action == "Clear")
   {
      for (Integer ii = 0; ii < theEvents.size(); ii++)
      {
         delete theEvents.at(ii);
      }
      theEvents.clear();
   }
   return true;
}

