//$Id$
//------------------------------------------------------------------------------
//                               ContactEvent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2015.01.08
//
/**
 * Implementation for the ContactEvent base class.
 */
//------------------------------------------------------------------------------

#include "ContactEvent.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

ContactEvent::ContactEvent(Real startEpoch, Real endEpoch) :
   LocatedEvent(startEpoch, endEpoch)
{
}

ContactEvent::~ContactEvent()
{
   // nothing to do here
}

ContactEvent::ContactEvent(const ContactEvent& copy) :
   LocatedEvent(copy)
{
}

ContactEvent& ContactEvent::operator=(const ContactEvent& copy)
{
   if (&copy != this)
   {
      LocatedEvent::operator=(copy);
   }

   return *this;
}


std::string ContactEvent::GetReportString()
{
   // TBD
   return "no contact event data\n";
}

