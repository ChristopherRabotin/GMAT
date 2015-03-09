//$Id$
//------------------------------------------------------------------------------
//                           LocatedEvent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Sep 2, 2011
// Heavily Modified: Wendy Shoan/GSFC  2015.01.09
//
/**
 * Implementation for the LocatedEvent base class.
 */
//------------------------------------------------------------------------------

#include "LocatedEvent.hpp"
#include "GmatConstants.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

LocatedEvent::LocatedEvent() :
   start     (0.0),
   end       (0.0),
   duration  (0.0)
{
}

LocatedEvent::LocatedEvent(Real startEpoch, Real endEpoch) :
   start     (startEpoch),
   end       (endEpoch),
   duration  (0.0)
{
}

LocatedEvent::~LocatedEvent()
{
   // nothing to do here
}

LocatedEvent::LocatedEvent(const LocatedEvent& copy) :
   start        (copy.start),
   end          (copy.end),
   duration     (copy.duration)
{
}

LocatedEvent& LocatedEvent::operator=(const LocatedEvent& copy)
{
   if (&copy != this)
   {
      start     = copy.start;
      end       = copy.end;
      duration  = copy.duration;
   }

   return *this;
}


Real LocatedEvent::GetDuration()
{
   duration = (end - start) * GmatTimeConstants::SECS_PER_DAY;
   return duration;
}

Real LocatedEvent::GetStart()
{
   return start;
}

Real LocatedEvent::GetEnd()
{
   return end;
}

//std::string LocatedEvent::GetReportString()
//{
//   // Abstract for now
//}

void LocatedEvent::SetStart(Real epoch1)
{
   start = epoch1;
}

void LocatedEvent::SetEnd(Real epoch2)
{
   end = epoch2;
}

