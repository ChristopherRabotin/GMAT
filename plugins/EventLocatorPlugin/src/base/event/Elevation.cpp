//$Id: Elevation.cpp 1915 2011-11-16 19:09:31Z djconway@NDC $
//------------------------------------------------------------------------------
//                           Elevation
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
// Created: Nov 9, 2011
//
/**
 * Implementation of the elevation event function
 */
//------------------------------------------------------------------------------


#include "Elevation.hpp"
#include "EventException.hpp"
#include "GroundstationInterface.hpp"


//------------------------------------------------------------------------------
// Elevation()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
Elevation::Elevation() :
   EventFunction        ("Elevation"),
   station              (NULL),
   isMasked             (false),
   bfcs                 (NULL),
   mj2kcs               (NULL)
{
}

//------------------------------------------------------------------------------
// ~Elevation()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Elevation::~Elevation()
{
}

//------------------------------------------------------------------------------
// Elevation(const Elevation & el)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param el The Elevation event function providing data for the new one
 */
//------------------------------------------------------------------------------
Elevation::Elevation(const Elevation & el) :
   EventFunction        (el),
   station              (el.station),
   isMasked             (el.isMasked),
   bfcs                 (NULL),
   mj2kcs               (NULL)
{
}


//------------------------------------------------------------------------------
// Elevation& operator=(const Elevation &el)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param el The Elevation event function providing data for this one
 *
 * @return this Elevation event function, configured to match el.
 */
//------------------------------------------------------------------------------
Elevation & Elevation::operator=(const Elevation &el)
{
   if (this != &el)
   {
      EventFunction::operator=(el);

      station  = el.station;
      isMasked = el.isMasked;
      bfcs     = NULL;
      mj2kcs   = NULL;
   }

   return *this;
}


//------------------------------------------------------------------------------
// void SetStation(SpacePoint *s)
//------------------------------------------------------------------------------
/**
 * Sets the station used in elevation computations
 *
 * @param s The station
 */
//------------------------------------------------------------------------------
void Elevation::SetStation(SpacePoint *s)
{
   station = s;
}



//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the event function for computations
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Elevation::Initialize()
{
   if (station == NULL)
      throw EventException("Elevation station not defined in the Elevation "
            "event function");

   bool retval = EventFunction::Initialize();

   if (retval)
   {
      if (station == NULL)
         throw EventException("Elevation event function is missing the "
               "station endpoint");

      if (station->IsOfType(Gmat::GROUND_STATION))
      {
         bfcs = ((GroundstationInterface*)station)->GetBodyFixedCoordinateSystem();
         // Get j2k CS too?
      }
      else
         throw EventException("Elevation stations must be GroundStation "
               "objects in the current implementation.");
   }

   instanceName = primary->GetName();
   instanceName += " - ";
   instanceName += station->GetName();

   return retval;
}


//------------------------------------------------------------------------------
// Real *Evaluate(GmatEpoch atEpoch, Real *forState)
//------------------------------------------------------------------------------
/**
 * Computes the Elevation event function and its derivative
 *
 * @param atEpoch  (Optional) epoch of the computation; if omitted, epoch is
 *                 retrieved from the primary.
 * @param forState (Optional) State data of the computation; if omitted, state
 *                 is retrieved from the primary.
 *
 * @return The epoch, event function value, and event function time derivative
 */
//------------------------------------------------------------------------------
Real *Elevation::Evaluate(GmatEpoch atEpoch, Real *forState)
{
   #ifdef DEBUG_EVENTFUNCTIONS
      MessageInterface::ShowMessage("Evaluating Elevation\n   primary "
            "<%p> - station<%p>\n", primary, station);
   #endif

   // Only calculate if the reference objects are set
   if ((primary != NULL) && (station != NULL))
   {
      if ((mj2kcs == NULL) || (bfcs == NULL))
         throw EventException("Elevation event function coordinate systems "
               "are not set.");

      GmatEpoch now;
      Rvector6 primaryRV, stationRV;

      if (atEpoch == -1.0)
      {
         now = primary->GetEpoch();
         primaryRV = primary->GetMJ2000State(now);
      }
      else
      {
         now = atEpoch;
         primaryRV.Set(forState[0], forState[1], forState[2], forState[3],
               forState[4], forState[5]);
      }

      eventData[0] = now;
      stationRV = station->GetMJ2000State(now);
      Rvector6 theVector = primaryRV - stationRV;
      Rvector6 topoState;

      // tbd: Do we force nutation and ignore translation (the current settings)?
      ccvtr.Convert(atEpoch, theVector, mj2kcs, topoState, bfcs, true, true);
      Real topoR = topoState.GetR().GetMagnitude();
      Real topoV = topoState.GetV().GetMagnitude();

      // Event function is actually not elevation, but a compatible smooth fn
      eventData[1] = GmatMathUtil::Sin(topoState[2] / topoR);

      eventData[2] = GmatMathUtil::Cos(topoState[2] / topoR) *
            (topoState[5] / topoR - topoState[2] * topoV / (topoR * topoR));
   }
   return eventData;
}
