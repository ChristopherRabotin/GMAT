//$Id$
//------------------------------------------------------------------------------
//                               ContactResult
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy C. Shoan
// Created: 2015.04.24
//
/**
 * Definition for the ContactResult class.  This class is a holder of
 * ContactEvents.
 */
//------------------------------------------------------------------------------

#ifndef ContactResult_hpp
#define ContactResult_hpp

#include "gmatdefs.hpp"
#include "ContactEvent.hpp"

/**
 * ContactResult class
 */
class LOCATOR_API ContactResult : public LocatedEvent
{
public:

   ContactResult();
   virtual ~ContactResult();
   ContactResult(const ContactResult& copy);
   ContactResult& operator=(const ContactResult& copy);

   // Add events to the list
   virtual bool         AddEvent(ContactEvent* newEvent);
   //virtual bool         SetEvent(Integer atIndex, ContactEvent *toEvent, bool deleteExisting = false);
   virtual Integer      NumberOfEvents();
   /// Set the no-events string
   virtual void         SetNoEvents(const std::string &noEv);
   /// Set the name of the observer
   virtual void         SetObserverName(const std::string &itsName);

   /// Return computed quantities
   virtual Real         GetDuration();

//   /// Set index
//   void                 SetIndex(Integer i);

   /// Return a specific Event
   ContactEvent*        GetEvent(Integer atIndex);

   // Get the report string
   virtual std::string  GetReportString();

   // Clear the event list
   virtual bool         TakeAction(const std::string &action,
                                   const std::string &actionData = "");

protected:
   /// The eclipse events
   std::vector<ContactEvent*> theEvents;
   /// Number of events
   Integer                    numEvents;
   /// The name of the observer
   std::string                observerName;
   /// No-events string
   std::string                noEvents;
};

#endif // ContactResult_hpp
