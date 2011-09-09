//$Id$
//------------------------------------------------------------------------------
//                           EventFunction
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
// Created: Jul 6, 2011
//
/**
 * Definition of the ...
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "EventFunction.hpp"
#include "EventException.hpp"

EventFunction::EventFunction(const std::string &typeStr) :
   typeName          (typeStr),
   instanceName      ("Base EventFunction (Name not set)"),
   primary           (NULL)
{
   eventData[0] = 0.0;
   eventData[1] = 999.999;
   eventData[2] = 999.999;
}

EventFunction::~EventFunction()
{
}

EventFunction::EventFunction(const EventFunction& ef) :
   typeName          (ef.typeName),
   instanceName      (ef.instanceName),
   primary           (ef.primary)
{
   eventData[0] = ef.eventData[0];
   eventData[1] = ef.eventData[1];
   eventData[2] = ef.eventData[2];
}

EventFunction& EventFunction::operator=(const EventFunction& ef)
{
   if (this != &ef)
   {
      typeName     = ef.typeName;
      instanceName = ef.instanceName;
      eventData[0] = ef.eventData[0];
      eventData[1] = ef.eventData[1];
      eventData[2] = ef.eventData[2];
      primary      = ef.primary;
   }

   return *this;
}

std::string EventFunction::GetTypeName()
{
   return typeName;
}

std::string EventFunction::GetName()
{
   return instanceName;
}

bool EventFunction::SetPrimary(SpaceObject *so)
{
   primary = so;
   return (so != NULL);
}



bool EventFunction::Initialize()
{
   if (primary == NULL)
      throw EventException("Unable to initialize the " + typeName +
            " EventFunction; the primary is not set.");

   return true;
}


