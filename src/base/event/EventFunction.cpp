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
 * Base class defining interfaces for event functions.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "EventFunction.hpp"
#include "EventException.hpp"

//------------------------------------------------------------------------------
// EventFunction(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EventFunction::EventFunction(const std::string &typeStr) :
   typeName          (typeStr),
   instanceName      ("Base EventFunction (Name not set)"),
   primary           (NULL),
   boundaryType      ("Undetermined"),
   isStart           (false)
{
   eventData[0] = 0.0;
   eventData[1] = 999.999;
   eventData[2] = 999.999;
}


//------------------------------------------------------------------------------
// ~EventFunction()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EventFunction::~EventFunction()
{
}


//------------------------------------------------------------------------------
// EventFunction(const EventFunction& ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ef The event function that is copied here
 */
//------------------------------------------------------------------------------
EventFunction::EventFunction(const EventFunction& ef) :
   typeName          (ef.typeName),
   instanceName      (ef.instanceName),
   primary           (ef.primary),
   boundaryType      (ef.boundaryType),
   isStart           (ef.isStart)
{
   eventData[0] = ef.eventData[0];
   eventData[1] = ef.eventData[1];
   eventData[2] = ef.eventData[2];
}


//------------------------------------------------------------------------------
// EventFunction& operator=(const EventFunction& ef)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ef The event function that is copied here
 *
 * @return this instance, set to match ef.
 */
//------------------------------------------------------------------------------
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
      boundaryType = ef.boundaryType;
      isStart      = ef.isStart;
   }

   return *this;
}


//------------------------------------------------------------------------------
// std::string GetTypeName()
//------------------------------------------------------------------------------
/**
 * Retrieves the type name assigned to the event function.
 *
 * This string is used to identify the type of event being computed.  Examples
 * include Umbra, Contact, Penumbra, and Antumbra.
 *
 * @return String name for the event type
 */
//------------------------------------------------------------------------------
std::string EventFunction::GetTypeName()
{
   return typeName;
}


//------------------------------------------------------------------------------
// std::string GetName()
//------------------------------------------------------------------------------
/**
 * Retrieves the instance specific string describing the event.
 *
 * @return The name associated with the event function instance.
 *
 * @note This method is not currently used.
 */
//------------------------------------------------------------------------------
std::string EventFunction::GetName()
{
   return instanceName;
}

std::string EventFunction::GetPrimaryName()
{
   std::string retval = "";

   if (primary != NULL)
      retval = primary->GetName();

   return retval;
}


//------------------------------------------------------------------------------
// bool SetPrimary(SpaceObject *so)
//------------------------------------------------------------------------------
/**
 * Sets the primary (target) object for the event function.
 *
 * The primary for event functions is the body -- usually a spacecraft -- that
 * is being evaluated for status of the event.
 *
 * @param so The object being evaluated
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool EventFunction::SetPrimary(SpaceObject *so)
{
   primary = so;
   return (so != NULL);
}


//------------------------------------------------------------------------------
// bool EventFunction::Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the event function for use
 *
 * @return true if initialization succeeds, false if it fails
 */
//------------------------------------------------------------------------------
bool EventFunction::Initialize()
{
   if (primary == NULL)
      throw EventException("Unable to initialize the " + typeName +
            " EventFunction; the primary is not set.");

   return true;
}


//------------------------------------------------------------------------------
// Real* GetData()
//------------------------------------------------------------------------------
/**
 * Retrieves the most recently calculated event data
 *
 * @return The data, in a Real array sized to match the size of the event
 *         function data (typically 3 elements: epoch, value, and derivative)
 */
//------------------------------------------------------------------------------
Real* EventFunction::GetData()
{
   return eventData;
}


//------------------------------------------------------------------------------
// std::string EventFunction::GetBoundaryType()
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing the type of boundary encountered
 *
 * @return The description, typically "Entry", "Exit", "Rise", or "Set"
 */
//------------------------------------------------------------------------------
std::string EventFunction::GetBoundaryType()
{
   // Default: slope positive at entry
   if (eventData[2] > 0.0)
      boundaryType = "Entry";
   else
      boundaryType = "Exit";

   return boundaryType;
}


//------------------------------------------------------------------------------
// bool EventFunction::IsEventEntry()
//------------------------------------------------------------------------------
/**
 * Retrieves a flag indicating if the event boundary is an entry or an exit
 *
 * @return true if the boundary is an entry location, false if an exit.
 */
//------------------------------------------------------------------------------
bool EventFunction::IsEventEntry()
{
   return (eventData[2] > 0.0 ? true : false);
}
