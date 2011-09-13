//$Id$
//------------------------------------------------------------------------------
//                           LocatedEvent
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


#include "LocatedEvent.hpp"

LocatedEvent::LocatedEvent() :
   epoch       (0.0),
   boundary    (""),
   type        (""),
   eventValue  (999.999)
{
}

LocatedEvent::~LocatedEvent()
{
}

LocatedEvent::LocatedEvent(const LocatedEvent& le) :
   epoch       (le.epoch),
   boundary    (le.boundary),
   type        (le.type),
   eventValue  (le.eventValue)
{

}

LocatedEvent& LocatedEvent::operator=(const LocatedEvent& le)
{
   if (this != &le)
   {
      epoch      = le.epoch;
      boundary   = le.boundary;
      type       = le.type;
      eventValue = le.eventValue;
   }

   return *this;
}
