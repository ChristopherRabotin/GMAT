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


#include "EventModel.hpp"
#include "EventLocator.hpp"
#include "MessageInterface.hpp"


EventModel::EventModel(const std::string &nomme) :
   PhysicalModel        (Gmat::PHYSICAL_MODEL, "EventModel", nomme)
{
}

EventModel::~EventModel()
{
}

EventModel::EventModel(const EventModel & em) :
   PhysicalModel        (em),
   events               (em.events),
   functionCounts       (em.functionCounts),
   eventStarts          (em.eventStarts),
   eventAssociates      (em.eventAssociates)
{
}

EventModel& EventModel::operator=(const EventModel & em)
{
   if (this != &em)
   {
      PhysicalModel::operator=(em);

      events          = em.events;
      functionCounts  = em.functionCounts;
      eventStarts     = em.eventStarts;
      eventAssociates = em.eventAssociates;
   }

   return *this;
}


GmatBase* EventModel::Clone() const
{
   return new EventModel(*this);
}


void EventModel::SetEventLocators(std::vector<EventLocator*> *els)
{
   events = els;
}

bool EventModel::Initialize()
{
   bool retval = PhysicalModel::Initialize();

   functionCounts.clear();
   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      Integer fc = events->at(i)->GetFunctionCount();
      functionCounts.push_back(fc);
      for (Integer j = 0; j < fc; ++j)
      {
         MessageInterface::ShowMessage("Calling SetStateIndices(%d, %d, "
               "%d)\n", j, eventStarts[i] + j,
               theState->GetAssociateIndex(eventStarts[i]+j));
         events->at(i)->SetStateIndices(j, eventStarts[i] + j,
               theState->GetAssociateIndex(eventStarts[i]+j));
      }
   }

   return retval;
}

bool EventModel::GetDerivatives(Real *state, Real dt, Integer order, const Integer id)
{
   bool retval = false;

   #ifdef DEBUG_EXE
      MessageInterface::ShowMessage("EventModel::GetDerivatives called\n%d "
            "events\n%d functionCounts\n", events->size(), functionCounts.size());
   #endif

   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      GmatEpoch now = theState->GetEpoch() + dt/GmatTimeConstants::SECS_PER_DAY;
      Real *data = events->at(i)->Evaluate(now, state);
      for (Integer j = 0; j < functionCounts[i]; ++j)
      {
//         Integer associate = theState->GetAssociateIndex(eventStarts[i]+j);
         deriv[eventStarts[i]+j] = 0.0; //data[j*3+2];
      }
   }

   retval = true;

   #ifdef DEBUG_EXE
      MessageInterface::ShowMessage("Deriv = [");
      for (Integer i = 0; i < eventStarts[0]+functionCounts[0]; ++i)
         MessageInterface::ShowMessage(" %le ", deriv[i]);
      MessageInterface::ShowMessage("]\n");
   #endif

   return retval;
}

bool EventModel::SupportsDerivative(Gmat::StateElementId id)
{
   if (id == Gmat::EVENT_FUNCTION_STATE)
      return true;
   return false;
}

bool EventModel::SetStart(Gmat::StateElementId id, Integer index,
      Integer quantity)
{
   bool retval = false;

   if (id == Gmat::EVENT_FUNCTION_STATE)
   {
      eventStarts.push_back(index);
      retval = true;
   }

   return retval;
}

const StringArray& EventModel::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   refObjectNames.clear();
   PhysicalModel::GetRefObjectNameArray(type);

   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      StringArray temp = events->at(i)->GetRefObjectNameArray(type);
      for (UnsignedInt j = 0; j < temp.size(); ++j)
      {
         if (find(refObjectNames.begin(), refObjectNames.end(), temp[j]) ==
               refObjectNames.end())
            refObjectNames.push_back(temp[j]);
      }
   }

   return refObjectNames;
}


bool EventModel::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string &name)
{
   bool retval = false;

   for (UnsignedInt i = 0; i < events->size(); ++i)
   {
      if (events->at(i)->SetRefObject(obj, type, name))
         retval = true;
   }

   return retval;
}
