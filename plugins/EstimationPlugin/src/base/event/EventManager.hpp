//$Id: EventManager.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         EventManager
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/09
//
/**
 * The event manager used to monitor events
 */
//------------------------------------------------------------------------------


#ifndef EventManager_hpp
#define EventManager_hpp

#include "estimation_defs.hpp"
#include "TriggerManager.hpp"
#include "Event.hpp"
#include "EstimationRootFinder.hpp"
#include "PropSetup.hpp"


class ESTIMATION_API EventManager : public TriggerManager
{
public:
   EventManager();
   ~EventManager();
   EventManager(const EventManager& em);
   EventManager& operator=(const EventManager& em);

   // Methods not yet needed:
//   void RegisterEvent(Event *newEvent);
//   void ActivateEvent(Event *theEvent, bool makeActive = true);
//   void UnregisterEvent(Event *newEvent);

   virtual void SetObject(GmatBase* obj);
   virtual void ClearObject(GmatBase* obj);
   virtual bool CheckForTrigger();
   virtual Real LocateTrigger();
   virtual TriggerManager* Clone();

   virtual void SetFixedState(Event *thisOne);
   Real FindRoot(Integer whichOne);
   RealArray EvaluateEvent(Integer whichOne);
   const IntegerArray& GetStatus();

protected:
   /// List of events managed by this manager
   StringArray eventNames;
   /// Vector if pointers to the managed events
   ObjectArray events;
   /// A RootFinder used to locate events
   EstimationRootFinder locater;
   /// Current status for each managed event
   IntegerArray eventStatus;
   /// Flags indicating if the matching event is active
   BooleanArray activated;

   void ProcessResults();
   void SaveEventData();
};

#endif // EventManager_hpp
