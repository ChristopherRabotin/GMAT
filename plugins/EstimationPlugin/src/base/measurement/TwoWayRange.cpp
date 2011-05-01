//$Id: TwoWayRange.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         TwoWayRange
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
// Created: 2009/12/16
//
/**
 * The real world 2-way range base class
 */
//------------------------------------------------------------------------------


#include "TwoWayRange.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_HARDWARE_DELAYS


//------------------------------------------------------------------------------
// TwoWayRange(const std::string &type, const std::string &nomme)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The specific type of 2-way range being constructed
 * @param nomme Name of the 2-way range object
 */
//------------------------------------------------------------------------------
TwoWayRange::TwoWayRange(const std::string &type, const std::string &nomme) :
   PhysicalMeasurement        (type, nomme),
   tR                         (GmatTimeConstants::MJD_OF_J2000),
   tT						         (GmatTimeConstants::MJD_OF_J2000),
   tV						         (GmatTimeConstants::MJD_OF_J2000),
   transmitDelay			      (0.0),
   targetDelay                (0.0),
   receiveDelay               (0.0),
   uplinkTime                 (0.0),
   downlinkTime               (0.0),
   uplinkRange                (0.0),
   downlinkRange              (0.0)
{
   objectTypeNames.push_back("TwoWayRange");

   uplinkLeg.SetName("TwoWayRange_UplinkLeg");
   downlinkLeg.SetName("TwoWayRange_DownlinkLeg");

   eventCount = 2;
}


//------------------------------------------------------------------------------
// ~TwoWayRange()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TwoWayRange::~TwoWayRange()
{
}


//------------------------------------------------------------------------------
// TwoWayRange(const TwoWayRange& twr)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param twr The range object that is being copied to the new instance
 */
//------------------------------------------------------------------------------
TwoWayRange::TwoWayRange(const TwoWayRange& twr) :
   PhysicalMeasurement        (twr),
   tR                         (twr.tR),
   tT						         (twr.tT),
   tV						         (twr.tV),
   transmitDelay			      (twr.transmitDelay),
   targetDelay                (twr.targetDelay),
   receiveDelay               (twr.receiveDelay),
   uplinkTime                 (twr.uplinkTime),
   downlinkTime               (twr.downlinkTime),
   uplinkLeg                  (twr.uplinkLeg),
   downlinkLeg                (twr.downlinkLeg),
   uplinkRange                (twr.uplinkRange),
   downlinkRange              (twr.downlinkRange)
{
}


//------------------------------------------------------------------------------
// TwoWayRange& operator=(const TwoWayRange& twr)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param twr The object supplying the 2-way range configuration
 *
 * @return This instance, set to match twr
 */
//------------------------------------------------------------------------------
TwoWayRange& TwoWayRange::operator=(const TwoWayRange& twr)
{
   if (this != &twr)
   {
      PhysicalMeasurement::operator=(twr);

      tR            = twr.tR;
      tT			= twr.tT;
      tV			= twr.tV;
      transmitDelay = twr.transmitDelay;
      targetDelay   = twr.targetDelay;
      receiveDelay  = twr.receiveDelay;
      uplinkTime    = twr.uplinkTime;
      downlinkTime  = twr.downlinkTime;
      uplinkLeg     = twr.uplinkLeg;
      downlinkLeg   = twr.downlinkLeg;
      uplinkRange   = twr.uplinkRange;
      downlinkRange = twr.downlinkRange;
      satEpochID    = twr.satEpochID;
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
bool TwoWayRange::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string &name)
{
   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      // It's a participant, so add it to the events
      uplinkLeg.AddParticipant((SpacePoint *)obj);
      downlinkLeg.AddParticipant((SpacePoint *)obj);

      // Current code has spacecraft groundstation as its model
      // todo: Generalize to support two spacecraft interconnect
      if (obj->IsOfType(Gmat::SPACEOBJECT))
         uplinkLeg.FixState(obj, false);
      else
         downlinkLeg.FixState(obj, false);
   }

   return PhysicalMeasurement::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
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
bool TwoWayRange::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string &name, const Integer index)
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
bool TwoWayRange::Initialize()
{
   bool retval = false;

   if (PhysicalMeasurement::Initialize())
   {
      if (participants.size() < 2)
         MessageInterface::ShowMessage("Range vector calculations require 2 "
               "participants; cannot initialize\n");
      else
      {
         // For now, require specific order for the participants
         // todo: Allow arbitrary participant ordering
         if ((participants[0]->IsOfType(Gmat::SPACE_POINT)) &&
             (participants[1]->IsOfType(Gmat::SPACECRAFT)))
         {
            satEpochID = participants[1]->GetParameterID("A1Epoch");

            // Reserve space and set IDs for each participant
            currentMeasurement.participantIDs.assign(participants.size(), "");
            for (UnsignedInt i = 0; i < participants.size(); ++i)
            {
               currentMeasurement.participantIDs[i] =
                     participants[i]->GetStringParameter("Id");
            }

            SetHardwareDelays(false);
            retval = true;
         }
         else
         {
            MessageInterface::ShowMessage("Participant mismatch in Range "
                  "measurement: Current code requires one Spacecraft and one "
                  "other SpacePoint participant; cannot initialize\n");
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Event* GetEvent(UnsignedInt whichOne)
//------------------------------------------------------------------------------
/**
 * Retrieves the uplink and downlink events
 *
 * @param whichOne The number of the event that is being retrieved
 *
 * @return A pointer to the event
 */
//------------------------------------------------------------------------------
Event* TwoWayRange::GetEvent(UnsignedInt whichOne)
{
   Event *retval = NULL;

   switch (whichOne)
   {
      case 0:
         retval = &downlinkLeg;
         break;

      case 1:
         retval = &uplinkLeg;
         break;

      default:
         break;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetEventData(Event *locatedEvent)
//------------------------------------------------------------------------------
/**
 * Passes data used by an event into the event
 *
 * This method is used to pass data from one event to another for events that
 * depend on each other.  In this case, the fixed time for the uplink event
 * is a value solved for in the downlink event combined with the transponder
 * delay offset.
 *
 * @param locatedEvent The event that generated the data
 *
 * @return true if the data was processed or ignored, false if not
 */
//------------------------------------------------------------------------------
bool TwoWayRange::SetEventData(Event *locatedEvent)
{
   bool retval = false;

   if (locatedEvent == &downlinkLeg)
   {
      #ifdef DEBUG_EVENT
         MessageInterface::ShowMessage("2-Way Range is setting fixed state "
               "time for the uplink leg\n");
      #endif
      // Set the fixed state time for the uplink leg
      Real start = - receiveDelay + downlinkLeg.GetVarTimestep() - targetDelay;
      uplinkLeg.SetFixedTimestep(start);

      // declare success!
      retval = true;
   }

   if (locatedEvent == &uplinkLeg)
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
 * manages the coordinate systems used in the uplink and downlink light time
 * correction events.
 */
//------------------------------------------------------------------------------
void TwoWayRange::InitializeMeasurement()
{
   PhysicalMeasurement::InitializeMeasurement();

   // Set the coordinate systems for the uplink and downlink events
   uplinkLeg.AddCoordinateSystem(j2k);   // Base CS for the event
   downlinkLeg.AddCoordinateSystem(j2k); // Base CS for the event

   // Because of the participant ordering, F1 is the fixed CS
   Integer index = uplinkLeg.GetParticipantIndex(participants[0]);
   uplinkLeg.AddCoordinateSystem(F1, index);   // Participant 1 CS for the event
   index = downlinkLeg.GetParticipantIndex(participants[0]);
   downlinkLeg.AddCoordinateSystem(F1, index); // Participant 2 CS for the event

   index = uplinkLeg.GetParticipantIndex(participants[1]);
   uplinkLeg.AddCoordinateSystem(F2, index);   // Participant 1 CS for the event
   index = downlinkLeg.GetParticipantIndex(participants[1]);
   downlinkLeg.AddCoordinateSystem(F2, index); // Participant 2 CS for the event
}


//------------------------------------------------------------------------------
// void SetHardwareDelays(bool loadEvents)
//------------------------------------------------------------------------------
/**
 * Retrieves delay vales and passes them into the events that need them.
 *
 * The default implementation does nothing but define the interface used in the
 * derived classes.
 *
 * @param loadEvents Flag used to automatically load the events with the delays
 *                   (defaults to true)
 */
//------------------------------------------------------------------------------
void TwoWayRange::SetHardwareDelays(bool loadEvents)
{
   #ifdef DEBUG_HARDWARE_DELAYS
      MessageInterface::ShowMessage("TwoWayRange::SetHardwareDelays(%s)\n",
            (loadEvents ? "true" : "false"));
      MessageInterface::ShowMessage("Initial Hardware delays set to:\n"
            "   Transmitter delay:  %.12lf sec\n"
            "   Transponder delay:  %.12lf sec\n"
            "   Receiver delay:     %.12lf sec\n",
            transmitDelay, targetDelay, receiveDelay);
      MessageInterface::ShowMessage("Participant count:  %d\n",
            participants.size());
      MessageInterface::ShowMessage("Participant hardware size:  %d\n",
            participantHardware.size());
    #endif

   // Transmitter sits on the 1st participant
   #ifdef DEBUG_HARDWARE_DELAYS
      MessageInterface::ShowMessage("Setting Transmitter delay (size %d)\n",
            participantHardware[0].size());
   #endif
   for (UnsignedInt i = 0; i < participantHardware[0].size(); ++i)
   {
      if (participantHardware[0][i]->IsOfType("Transmitter"))
      {
         transmitDelay = ((Sensor*)(participantHardware[0][i]))->GetDelay();
         break;
      }
   }

   // Default Transponder sits on the 2nd participant
   #ifdef DEBUG_HARDWARE_DELAYS
      MessageInterface::ShowMessage("Setting Transponder delay (size %d)\n",
            participantHardware[1].size());
   #endif
   for (UnsignedInt i = 0; i < participantHardware[1].size(); ++i)
   {
      #ifdef DEBUG_HARDWARE_DELAYS
         MessageInterface::ShowMessage("   Transponder %d is %s\n", i,
               (participantHardware[1][i] == NULL ? "NULL" :
                participantHardware[1][i]->GetName().c_str()));
      #endif
      if (participantHardware[1][i]->IsOfType("Transponder"))
      {
         // Set delay based on first transponder
         #ifdef DEBUG_HARDWARE_DELAYS
            MessageInterface::ShowMessage("   ***Getting delay from %s\n",
                  participantHardware[1][i]->GetName().c_str());
         #endif
         targetDelay = ((Sensor*)(participantHardware[1][i]))->GetDelay();
         break;
      }
   }

   // Receiver sits on the 1st participant
   #ifdef DEBUG_HARDWARE_DELAYS
      MessageInterface::ShowMessage("Setting Receiver delay (size %d)\n",
            participantHardware[0].size());
   #endif
   for (UnsignedInt i = 0; i < participantHardware[0].size(); ++i)
   {
      if (participantHardware[0][i]->IsOfType("Receiver"))
      {
         receiveDelay = ((Sensor*)(participantHardware[0][i]))->GetDelay();
         break;
      }
   }

   if (loadEvents)
   {
      // Load the values into the light time corrections measurements as needed;
      // the only default event that we can load here is the downlink

      // Note that this default 2-way range model assumes there is a hardware
      // delay at the end!  Make sure to turn it off or zero it if it's not
      // part of the spec.
      downlinkLeg.SetFixedTimestep(-receiveDelay);
   }

   #ifdef DEBUG_HARDWARE_DELAYS
      MessageInterface::ShowMessage("Hardware delays set to:\n"
            "   Transmitter delay:  %.12lf sec\n"
            "   Transponder delay:  %.12lf sec\n"
            "   Receiver delay:     %.12lf sec\n",
            transmitDelay, targetDelay, receiveDelay);
   #endif
}
