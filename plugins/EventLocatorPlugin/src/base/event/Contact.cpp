//$Id: Contact.cpp 1915 2011-11-16 19:09:31Z djconway@NDC $
//------------------------------------------------------------------------------
//                           Contact
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
// Created: Nov 7, 2011
//
/**
 * Definition of the ...
 */
//------------------------------------------------------------------------------


#include "Contact.hpp"
#include "SpacePoint.hpp"
#include "EventException.hpp"


//------------------------------------------------------------------------------
// Contact()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
Contact::Contact() :
   EventFunction        ("Contact"),
   station              (NULL),
   elevation            (NULL)
{
}


//------------------------------------------------------------------------------
// ~Contact()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Contact::~Contact()
{
   // The Contact event owns internal elevation & line of sight event functions
   ClearEventFunctions();
}


//------------------------------------------------------------------------------
// Contact(const Contact &c)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param c The Contact being copied
 */
//------------------------------------------------------------------------------
Contact::Contact(const Contact &c) :
   EventFunction        (c),
   station              (c.station),
   elevation            (NULL)
{
}


//------------------------------------------------------------------------------
// Contact& operator=(const Contact &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param c The Contact providing new parameters for this one
 *
 * @return this Contact, configured to match c
 */
//------------------------------------------------------------------------------
Contact& Contact::operator=(const Contact &c)
{
   if (this != &c)
   {
      EventFunction::operator =(c);

      station  = c.station;
      ClearEventFunctions();
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetStation(SpacePoint *s)
//------------------------------------------------------------------------------
/**
 * Sets the station pointer for this Contact
 *
 * @param s The station
 */
//------------------------------------------------------------------------------
void Contact::SetStation(SpacePoint *s)
{
   station = s;
}

//------------------------------------------------------------------------------
// bool HasEvent(EventFunction *ef)
//------------------------------------------------------------------------------
/**
 * Checks to see if the input event function is in this Contact container
 *
 * @param ef The EventFunction whose owner is sought
 *
 * @return true if the event function is a member, false if not.
 */
//------------------------------------------------------------------------------
bool Contact::HasEvent(EventFunction *ef)
{
   bool retval = false;

   if (elevation == ef)
      retval = true;

   if (find(los.begin(), los.end(), ef) != los.end())
      retval = true;

   return retval;
}

//------------------------------------------------------------------------------
// void SetEvent(EventFunction *ef)
//------------------------------------------------------------------------------
/**
 * Passes in an owned event function.
 *
 * The EventLocator managing this Contact event creates the EventFunctions it
 * needs, and then passes each one into the Contact container.  Once received,
 * the Contact owns the EventFunction and manages it, calling it for
 * computations and destroying it when no longer needed.
 *
 * @param ef The event function
 */
//------------------------------------------------------------------------------
void Contact::SetEvent(EventFunction *ef)
{
   if (ef->GetTypeName() == "Elevation")
      elevation = (Elevation*)ef;

   else if (ef->GetTypeName() == "LineOfSight")
   {
      if (find(los.begin(), los.end(), ef) == los.end())
         los.push_back((LineOfSight*)ef);
   }

   else throw EventException("Contact events do not work with " +
         ef->GetTypeName() + " event functions.");
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the Contact for use
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Contact::Initialize()
{
   // Size the output data buffer to handle all of the event data
   dataSize = los.size() + 1;
   bool retval = EventFunction::Initialize();

   if (retval == true)
   {
      if (station == NULL)
         throw EventException("Unable to initialize the " + typeName +
               " EventFunction; the contact station is not set.");

      if (elevation == NULL)
         throw EventException("Unable to initialize the contact EventFunction; "
               "the elevation member event function is not set.");

      // Initialize the member event functions
      if (elevation->Initialize() == false)
            return false;
      for (UnsignedInt i = 0; i < los.size(); ++i)
         if (los[i]->Initialize() == false)
            return false;

      instanceName = primary->GetName();
      instanceName += " - ";
      instanceName += station->GetName();
   }

   return retval;
}


//------------------------------------------------------------------------------
// Real* Evaluate(GmatEpoch atEpoch, Real *forState)
//------------------------------------------------------------------------------
/**
 * Evaluates the Contact event
 *
 * For Contact events, evaluation consists of evaluating the owned event
 * functions and filling in the event data accordingly.
 *
 * @param atEpoch The epoch passed into each EventFunction
 * @param forState The state passed into each EventFunction
 *
 * @return The eventData vector
 */
//------------------------------------------------------------------------------
Real* Contact::Evaluate(GmatEpoch atEpoch, Real *forState)
{
   // Only calculate if the reference objects are set
   if ((primary != NULL) && (station != NULL))
   {
      if (elevation == NULL)
         throw EventException("Cannot evaluate contact; elevation event "
               "function is NULL");

      Real *values = elevation->Evaluate(atEpoch, forState);
      eventData[0] = values[0];
      eventData[1] = values[1];
      eventData[2] = values[2];

      for (UnsignedInt i = 0; i < los.size(); ++i)
      {
         UnsignedInt i3 = (i + 1) * 3;

         values = los[i]->Evaluate(atEpoch, forState);
         eventData[ i3 ] = values[0];
         eventData[i3+1] = values[1];
         eventData[i3+2] = values[2];
      }
   }

   return eventData;
}


//------------------------------------------------------------------------------
// void ClearEventFunctions()
//------------------------------------------------------------------------------
/**
 * Removes the EventFunctions from the Contact container, freeing memory
 */
//------------------------------------------------------------------------------
void Contact::ClearEventFunctions()
{
   if (elevation)
      delete elevation;
   elevation = NULL;

   for (UnsignedInt i = 0; i < los.size(); ++i)
      delete los[i];
   los.clear();
}
