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
#include "MessageInterface.hpp"
#include "EventException.hpp"


//#define DEBUG_ORIGIN

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
   eventData         (NULL),
   dataSize          (3),
   primary           (NULL),
   origin            (NULL),
   boundaryType      ("Undetermined"),
   isStart           (false)
{
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
   if (eventData)
      delete [] eventData;
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
   eventData         (NULL),
   dataSize          (ef.dataSize),
   primary           (ef.primary),
   origin            (ef.origin),
   boundaryType      (ef.boundaryType),
   isStart           (ef.isStart)
{
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

      // Only rebuild the data buffer if needed
      if (dataSize != ef.dataSize)
      {
         dataSize = ef.dataSize;
         if (eventData != NULL)
         {
            delete [] eventData;
            eventData = NULL;
         }
      }
      if ((dataSize > 0) && (eventData == NULL) && (ef.eventData != NULL))
         eventData = new Real[dataSize];

      // Copy in the most recent event data
      if (ef.eventData != NULL)
      {
         for (UnsignedInt i = 0; i < dataSize; ++i)
            eventData[i] = ef.eventData[i];
      }

      primary      = ef.primary;
      origin       = ef.origin;
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


//------------------------------------------------------------------------------
// std::string GetPrimaryName()
//------------------------------------------------------------------------------
/**
 * Retrieves the name of the primary body (e.g. the target spacecraft)
 *
 * @return The name of the primary
 */
//------------------------------------------------------------------------------
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
// bool SetOrigin(CelestialBody *bod)
//------------------------------------------------------------------------------
/**
 * Sets the coordinate system origin body pointer
 *
 * @param bod The body of the state origin data
 *
 * @return true if an origin was set, false if the origin is NULL on return
 */
//------------------------------------------------------------------------------
bool EventFunction::SetOrigin(SpacePoint *sp)
{
   origin = sp;
   #ifdef DEBUG_ORIGIN
      MessageInterface::ShowMessage("Origin: %s\n", origin->GetName().c_str());
   #endif
   return (origin != NULL);
}


//------------------------------------------------------------------------------
// bool Initialize()
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

   if (origin == NULL)
      throw EventException("Unable to initialize the " + typeName +
            " EventFunction; the origin is not set.");

   // (Re)allocate the data array
   if (eventData != NULL)
      delete [] eventData;
   eventData = new Real[dataSize];

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
