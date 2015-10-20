//$Id$
//------------------------------------------------------------------------------
//                               ContactEvent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2015.01.08
//
/**
 * Definition for the ContactEvent class.
 */
//------------------------------------------------------------------------------

#ifndef ContactEvent_hpp
#define ContactEvent_hpp

#include "gmatdefs.hpp"
#include "EventLocatorDefs.hpp"
#include "LocatedEvent.hpp"

/**
 * ContactEvent class
 */
class LOCATOR_API ContactEvent : public LocatedEvent
{
public:

   ContactEvent(Real startEpoch, Real endEpoch);
   virtual ~ContactEvent();
   ContactEvent(const ContactEvent& copy);
   ContactEvent& operator=(const ContactEvent& copy);

   /// Return computed quantities

   // Get the report string
   virtual std::string  GetReportString();

protected:
};

#endif // ContactEvent_hpp
