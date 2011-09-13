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


#ifndef LocatedEvent_hpp
#define LocatedEvent_hpp

#include "gmatdefs.hpp"

/**
 * This class is essentially a struct with copy operators, used for managing and
 * storing the event location data.  Since it is a struct in use, all of the
 * attributes are public.
 */
class LocatedEvent
{
public:
   LocatedEvent();
   virtual ~LocatedEvent();
   LocatedEvent(const LocatedEvent& le);
   LocatedEvent& operator=(const LocatedEvent& le);

   /// The epoch of the data element.
   GmatEpoch epoch;
   /// Identifier for the type of datum represented
   std::string boundary;
   /// The type of the event.
   std::string type;
   /// The event function value at the located event
   Real eventValue;
};

#endif /* LocatedEvent_hpp */
