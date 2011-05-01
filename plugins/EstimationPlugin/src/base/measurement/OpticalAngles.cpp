//$Id$
//------------------------------------------------------------------------------
//                             OpticalAngles
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: Mar 23, 2010 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the OpticalAngles class
 */
//------------------------------------------------------------------------------


#include "OpticalAngles.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"


//------------------------------------------------------------------------------
// OpticalAngles(const std::string &type, const std::string &nomme):
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The scripted type for this measurment
 * @param nomme The measurement name
 */
//------------------------------------------------------------------------------
OpticalAngles::OpticalAngles(const std::string &type, const std::string &nomme):
   PhysicalMeasurement        (type, nomme),
   tR                         (GmatTimeConstants::MJD_OF_J2000),
   tT                         (GmatTimeConstants::MJD_OF_J2000),
   receiveDelay               (0.0),
   transitTime                (0.0)
{
   objectTypeNames.push_back("OpticalAngles");
   lightPath.SetName("OpticalLightPath");
   eventCount = 1;
}


//------------------------------------------------------------------------------
// ~OpticalAngles()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OpticalAngles::~OpticalAngles()
{
}


//------------------------------------------------------------------------------
// OpticalAngles::OpticalAngles(const OpticalAngles & oa) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param oa The instance copied to this new one
 */
//------------------------------------------------------------------------------
OpticalAngles::OpticalAngles(const OpticalAngles & oa) :
   PhysicalMeasurement        (oa),
   tR                         (oa.tR),
   tT                         (oa.tT),
   receiveDelay               (oa.receiveDelay),
   transitTime                (oa.transitTime),
   lightPath                  (oa.lightPath)
{
}


OpticalAngles & OpticalAngles::operator=(const OpticalAngles & oa)
{
   if (this != &oa)
   {
      PhysicalMeasurement::operator=(oa);

      tR           = oa.tR;
      tT           = oa.tT;
      receiveDelay = oa.receiveDelay;
      transitTime  = oa.transitTime;
      lightPath    = oa.lightPath;
   }

   return *this;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string &name)
//------------------------------------------------------------------------------
/**
 * Method used to pass participants and other reference objects to this instance
 *
 * @param obj Pointer to the object
 * @param type The type of obj
 * @param name The name of obj
 *
 * @return true if the object was processed
 */
//------------------------------------------------------------------------------
bool OpticalAngles::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string & name)
{
   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      // It's a participant, so add it to the events
      lightPath.AddParticipant((SpacePoint *)obj);

      // Current code has spacecraft groundstation as its model
      // todo: Generalize to support two spacecraft interconnect
      if (!obj->IsOfType(Gmat::SPACEOBJECT))
         lightPath.FixState(obj, false);
   }

   return PhysicalMeasurement::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// bool TwoWayRange::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//       const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * Method used to pass reference objects stored in an array to this instance
 *
 * @param obj Pointer to the object
 * @param type The type of obj
 * @param name The name of obj
 * @param index Array index for the object
 *
 * @return true if the object was processed
 */
//------------------------------------------------------------------------------
bool OpticalAngles::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string & name, const Integer index)
{
   return PhysicalMeasurement::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method used to prepare the object for a run
 *
 * @return true if initialization succeeded, false on failure
 */
//------------------------------------------------------------------------------
bool OpticalAngles::Initialize()
{
   bool retval = false;

   if (PhysicalMeasurement::Initialize())
   {
      if (participants.size() < 2)
         MessageInterface::ShowMessage("Optical angle calculations require 2 "
               "participants; cannot initialize\n");
      else
      {
         // For now, require specific order for the participants
         // todo: Allow arbitrary participant ordering
         if ((participants[0]->IsOfType(Gmat::SPACE_POINT)) &&
             (participants[1]->IsOfType(Gmat::SPACECRAFT)))
         {
            satEpochID = participants[1]->GetParameterID("A1Epoch");

            for (UnsignedInt i = 0; i < participants.size(); ++i)
               currentMeasurement.participantIDs[i] =
                     participants[i]->GetStringParameter("Id");

            retval = true;
         }
         else
         {
            MessageInterface::ShowMessage("Participant mismatch in optical "
                  "angle measurement: Current code requires one Spacecraft and "
                  "one other SpacePoint participant; cannot initialize\n");
         }

         lightPath.SetFixedTimestep(-receiveDelay);
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Event* GetEvent(UnsignedInt whichOne)
//------------------------------------------------------------------------------
/**
 * Retrieves the light transit events
 *
 * @param whichOne The number of the event that is being retrieved
 *
 * @return A pointer to the event
 */
//------------------------------------------------------------------------------
Event *OpticalAngles::GetEvent(UnsignedInt whichOne)
{
   Event *retval = NULL;

   if (whichOne == 0)
      retval = &lightPath;

   return retval;
}


//------------------------------------------------------------------------------
// bool SetEventData(Event *locatedEvent)
//------------------------------------------------------------------------------
/**
 * Passes data used by an event into the event
 *
 * This method is used to pass data from one event to another for events that
 * depend on each other.  It is provided in the optical angle measurement in
 * case it is needed at a later date.
 *
 * @param locatedEvent The event that generated the data
 *
 * @return true if the data was processed or ignored, false if not
 */
//------------------------------------------------------------------------------
bool OpticalAngles::SetEventData(Event *locatedEvent)
{
   bool retval = false;

   if (locatedEvent == &lightPath)
   {
      // declare success!
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// void InitializeMeasurement()
//------------------------------------------------------------------------------
/**
 * Prepares the measurement for use in simulation or estimation
 *
 * This method calls the base class InitializeMeasurement() method, and then
 * manages the coordinate systems used in the lightPath light time correction
 * event.
 */
//------------------------------------------------------------------------------
void OpticalAngles::InitializeMeasurement()
{
   PhysicalMeasurement::InitializeMeasurement();

   // Set the coordinate systems for the uplink and downlink events
   lightPath.AddCoordinateSystem(j2k);   // Base CS for the event

   // Because of the participant ordering, F1 is the fixed CS
   Integer index = lightPath.GetParticipantIndex(participants[0]);
   lightPath.AddCoordinateSystem(Fo, index);   // Participant 1 CS for the event
   index = lightPath.GetParticipantIndex(participants[1]);
   lightPath.AddCoordinateSystem(F2, index);   // Participant 2 CS for the event
}
