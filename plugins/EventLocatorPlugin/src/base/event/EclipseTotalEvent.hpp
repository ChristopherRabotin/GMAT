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
// Created: 2015.01.09
//
/**
 * Definition for the EclipseTotalEvent class.  This class is a holder of
 * EclipseEvents.
 */
//------------------------------------------------------------------------------

#ifndef EclipseTotalEvent_hpp
#define EclipseTotalEvent_hpp

#include "gmatdefs.hpp"
#include "LocatedEvent.hpp"
#include "EclipseEvent.hpp"

/**
 * EclipseTotalEvent class
 */
class LOCATOR_API EclipseTotalEvent : public LocatedEvent
{
public:

   EclipseTotalEvent();
   virtual ~EclipseTotalEvent();
   EclipseTotalEvent(const EclipseTotalEvent& copy);
   EclipseTotalEvent& operator=(const EclipseTotalEvent& copy);

   // Add events to the list
   virtual bool         AddEvent(EclipseEvent* newEvent);
   virtual bool         SetEvent(Integer atIndex, EclipseEvent *toEvent, bool deleteExisting = false);
   virtual Integer      NumberOfEvents();

   /// Return computed quantities
   virtual Real         GetDuration();

   /// Set index
   void                 SetIndex(Integer i);

   /// Return a specific Event
   EclipseEvent*        GetEvent(Integer atIndex);

   // Get the report string
   virtual std::string  GetReportString();

   // Clear the event list
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData);

protected:
   /// The eclipse events
   std::vector<EclipseEvent*> theEvents;
   /// Number of events
   Integer                    numEvents;
   /// Index of the total event
   Integer                    theIndex;
};

#endif // EclipseTotalEvent_hpp
