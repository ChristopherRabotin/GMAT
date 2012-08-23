//$Id$
//------------------------------------------------------------------------------
//                           EventModel
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
// Created: Sep 7, 2011
//
/**
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#ifndef EventModel_hpp
#define EventModel_hpp

#include "PhysicalModel.hpp"

class EventLocator;

/**
 * This PhysicalModel connects EventFunctions to Integrators.
 *
 * The EventModel class is a container class that takes a collection of
 * EventLocator objects, accesses the contained EventFunctions, and generates
 * derivative data for each EventFunction for use by GMAT's propagation
 * subsystem.  The EventLocator objects are collected into a vector in the
 * Sandbox and passed into the ODEModel that manages the EventModel.  The 
 * ODEModel passes that vector into the EventModel during initialization.
 */
class EventModel: public PhysicalModel
{
public:
   EventModel(const std::string &nomme = "");
   virtual ~EventModel();
   EventModel(const EventModel& em);
   EventModel& operator=(const EventModel& em);
   virtual GmatBase* Clone() const;

   virtual void SetEventLocators(std::vector<EventLocator*> *els);
   virtual bool Initialize();

//   // Support for extra derivative calcs - the EventFunction contributions here
//   virtual const IntegerArray& GetSupportedDerivativeIds();
//   virtual const StringArray&  GetSupportedDerivativeNames();

   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index,
                         Integer quantity);

   virtual const StringArray& GetRefObjectNameArray(
         const Gmat::ObjectType type);
   virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
         const std::string &name = "");
   virtual bool RenameRefObject(const Gmat::ObjectType type,
         const std::string &oldName, const std::string &newName);


   virtual bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1,
         const Integer id = -1);

   DEFAULT_TO_NO_CLONES

private:
   /// The list of active EventLocators
   std::vector<EventLocator*> *events;
   /// Event functions counts for the locators
   std::vector<Integer> functionCounts;
   /// Start indices for event functions embedded in the event locator vector
   std::vector<Integer> eventStarts;
   /// Associated state data for event calculations
   std::vector<Integer> eventAssociates;
   /// Event start index in the state vector
   Integer eventStartIndex;
};

#endif /* EventModel_hpp */
