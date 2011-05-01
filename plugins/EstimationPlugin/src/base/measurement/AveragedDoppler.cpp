//$Id: AveragedDoppler.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         AveragedDoppler
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS
// Task 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 27, 2010
//
/**
 * Base class for Doppler measurement models
 */
//------------------------------------------------------------------------------


#include "AveragedDoppler.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "Transponder.hpp"          // For turnaround ratio

//#define DEBUG_HARDWARE_DELAYS


//------------------------------------------------------------------------------
// Static members
//------------------------------------------------------------------------------
const std::string
AveragedDoppler::PARAMETER_TEXT[AveragedDopplerParamCount -
                                PhysicalMeasurementParamCount] =
{
      "AveragingInterval",
};

const Gmat::ParameterType
AveragedDoppler::PARAMETER_TYPE[AveragedDopplerParamCount -
                                PhysicalMeasurementParamCount] =
{
      Gmat::REAL_TYPE,
};


//------------------------------------------------------------------------------
// Public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// AveragedDoppler(const std::string &type, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The subtype of the Doppler measurement
 * @param withName The new object's name
 */
//------------------------------------------------------------------------------
AveragedDoppler::AveragedDoppler(const std::string &type,
      const std::string &withName) :
   PhysicalMeasurement        (type, withName),
   tm                         (GmatTimeConstants::MJD_OF_J2000),
   interval                   (1.0),  // 1 sec default interval
   turnaround                 (1.1)
{
   objectTypeNames.push_back("AveragedDoppler");
   parameterCount = AveragedDopplerParamCount;

   uplinkLegS.SetName("TwoWayDoppler_StartUplinkLeg");
   uplinkLegE.SetName("TwoWayDoppler_EndUplinkLeg");
   downlinkLegS.SetName("TwoWayDoppler_StartDownlinkLeg");
   downlinkLegE.SetName("TwoWayDoppler_EndDownlinkLeg");

   eventCount = 4;

   t3E[0] = -0.5;
   t3E[1] =  0.5;

   // Set default times and offsets to 0
   t1delay[0] = t1delay[1] = 0.0;
   t2delay[0] = t2delay[1] = 0.0;
   t3delay[0] = t3delay[1] = 0.0;
   t1T[0] = t1T[1] = 0.0;
   t2T[0] = t2T[1] = 0.0;
   t3R[0] = t3R[1] = 0.0;
}


//------------------------------------------------------------------------------
// ~AveragedDoppler()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
AveragedDoppler::~AveragedDoppler()
{
}


//------------------------------------------------------------------------------
// AveragedDoppler(const AveragedDoppler & ad) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ad The object that is being copied
 */
//------------------------------------------------------------------------------
AveragedDoppler::AveragedDoppler(const AveragedDoppler & ad) :
   PhysicalMeasurement        (ad),
   tm                         (ad.tm),
   interval                   (ad.interval),
   turnaround                 (ad.turnaround),
   uplinkLegS                 (ad.uplinkLegS),
   downlinkLegS               (ad.downlinkLegS),
   uplinkLegE                 (ad.uplinkLegE),
   downlinkLegE               (ad.downlinkLegE)
{
   t3E[0] = ad.t3E[0];
   t3E[1] = ad.t3E[1];

   t1delay[0] = ad.t1delay[0];
   t1delay[1] = ad.t1delay[1];
   t2delay[0] = ad.t2delay[0];
   t2delay[1] = ad.t2delay[1];
   t3delay[0] = ad.t3delay[0];
   t3delay[1] = ad.t3delay[1];

   t1T[0] = ad.t1T[0];
   t1T[1] = ad.t1T[1];
   t2T[0] = ad.t2T[0];
   t2T[1] = ad.t2T[1];
   t3R[0] = ad.t3R[0];
   t3R[1] = ad.t3R[1];
}


//------------------------------------------------------------------------------
// AveragedDoppler& operator=(const AveragedDoppler& ad)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ad The object that is being copied to this one
 *
 * @return This Doppler measurement, configured to match ad
 */
//------------------------------------------------------------------------------
AveragedDoppler& AveragedDoppler::operator=(const AveragedDoppler& ad)
{
   if (this != &ad)
   {
      PhysicalMeasurement::operator=(ad);

      t3E[0] = ad.t3E[0];
      t3E[1] = ad.t3E[1];

      t1delay[0] = ad.t1delay[0];
      t1delay[1] = ad.t1delay[1];
      t2delay[0] = ad.t2delay[0];
      t2delay[1] = ad.t2delay[1];
      t3delay[0] = ad.t3delay[0];
      t3delay[1] = ad.t3delay[1];

      t1T[0] = ad.t1T[0];
      t1T[1] = ad.t1T[1];
      t2T[0] = ad.t2T[0];
      t2T[1] = ad.t2T[1];
      t3R[0] = ad.t3R[0];
      t3R[1] = ad.t3R[1];

      turnaround = ad.turnaround;

      uplinkLegS   = ad.uplinkLegS;
      downlinkLegS = ad.downlinkLegS;
      uplinkLegE   = ad.uplinkLegE;
      downlinkLegE = ad.downlinkLegE;
   }

   return *this;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the script string for class parameters
 *
 * @param id The ID of the parameter
 *
 * @return The script string
 */
//------------------------------------------------------------------------------
std::string AveragedDoppler::GetParameterText(const Integer id) const
{
   if (id >= PhysicalMeasurementParamCount && id < AveragedDopplerParamCount)
      return PARAMETER_TEXT[id - PhysicalMeasurementParamCount];
   return PhysicalMeasurement::GetParameterText(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units associated with a parameter
 *
 * @param id The parameter's ID
 *
 * @return The parameter's units
 */
//------------------------------------------------------------------------------
std::string AveragedDoppler::GetParameterUnit(const Integer id) const
{
   if (id == AveragingInterval)
      return "sec";
   return PhysicalMeasurement::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID for a parameter
 *
 * @param str The script string associated with the parameter
 *
 * @return The parameter ID
 */
//------------------------------------------------------------------------------
Integer AveragedDoppler::GetParameterID(const std::string &str) const
{
   for (Integer i = PhysicalMeasurementParamCount;
         i < AveragedDopplerParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - PhysicalMeasurementParamCount])
         return i;
   }

   return PhysicalMeasurement::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter type for a parameter
 *
 * @param id The parameter's ID
 *
 * @return The parameter type
 */
//------------------------------------------------------------------------------
Gmat::ParameterType AveragedDoppler::GetParameterType(const Integer id) const
{
   if (id >= PhysicalMeasurementParamCount && id < AveragedDopplerParamCount)
      return PARAMETER_TYPE[id - PhysicalMeasurementParamCount];

   return PhysicalMeasurement::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing the type of a parameter
 *
 * @param id The parameter's ID
 *
 * @return A string describing the type of the parameter
 */
//------------------------------------------------------------------------------
std::string AveragedDoppler::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a real parameter
 *
 * @param id The ID of the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::GetRealParameter(const Integer id) const
{
   if (id == AveragingInterval)
      return interval;

   return PhysicalMeasurement::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a parameter
 *
 * @param id The ID of the parameter
 * @param value The new parameter value
 *
 * @return The value of the parameter upon exit
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::SetRealParameter(const Integer id, const Real value)
{
   if (id == AveragingInterval)
   {
      if (value > 0.0)
         interval = value;
      return interval;
   }

   return PhysicalMeasurement::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a real parameter from an array of reals
 *
 * @param id The parameter ID
 * @param index The index into the array for the desired value
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::GetRealParameter(const Integer id,
                                      const Integer index) const
{
   return PhysicalMeasurement::GetRealParameter(id, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * Retrieves a real parameter from a matrix of reals
 *
 * @param id The parameter ID
 * @param row The row index into the matrix for the desired value
 * @param col The column index into the matrix for the desired value
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::GetRealParameter(const Integer id, const Integer row,
                                      const Integer col) const
{
   return PhysicalMeasurement::GetRealParameter(id, row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a real parameter in an array of reals
 *
 * @param id The parameter ID
 * @param value The new parameter value
 * @param index The index into the array for the desired value
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::SetRealParameter(const Integer id,
                                      const Real value,
                                      const Integer index)
{
   return PhysicalMeasurement::SetRealParameter(id, value, index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value, const Integer row,
//       const Integer col)
//------------------------------------------------------------------------------
/**
 * Sets a real parameter in a matrix of reals
 *
 * @param id The parameter ID
 * @param value The new parameter value
 * @param row The row index into the matrix for the desired value
 * @param col The column index into the matrix for the desired value
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::SetRealParameter(const Integer id, const Real value,
                                      const Integer row, const Integer col)
{
   return PhysicalMeasurement::SetRealParameter(id, value, row, col);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a real parameter
 *
 * @param label The scripted label for the parameter
 *
 * @return The parameter value
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Sets the value of a parameter
 *
 * @param label The scripted label for the parameter
 * @param value The new parameter value
 *
 * @return The value of the parameter upon exit
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::SetRealParameter(const std::string &label,
                                      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a real parameter from an array of reals
 *
 * @param label The scripted label for the parameter
 * @param index The index into the array for the desired value
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::GetRealParameter(const std::string &label,
                                      const Integer index) const
{
   return GetRealParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value,
//       const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a real parameter in an array of reals
 *
 * @param label The scripted label for the parameter
 * @param value The new parameter value
 * @param index The index into the array for the desired value
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::SetRealParameter(const std::string &label,
                                      const Real value,
                                      const Integer index)
{
   return SetRealParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label, const Integer row,
//       const Integer col) const
//------------------------------------------------------------------------------
/**
 * Retrieves a real parameter from a matrix of reals
 *
 * @param label The scripted label for the parameter
 * @param row The row index into the matrix for the desired value
 * @param col The column index into the matrix for the desired value
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::GetRealParameter(const std::string &label,
                                      const Integer row,
                                      const Integer col) const
{
   return GetRealParameter(GetParameterID(label), row, col);
}


//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value,
//       const Integer row, const Integer col)
//------------------------------------------------------------------------------
/**
 * Sets a real parameter in a matrix of reals
 *
 * @param label The scripted label for the parameter
 * @param value The new parameter value
 * @param row The row index into the matrix for the desired value
 * @param col The column index into the matrix for the desired value
 *
 * @return The value
 */
//------------------------------------------------------------------------------
Real AveragedDoppler::SetRealParameter(const std::string &label,
                                      const Real value, const Integer row,
                                      const Integer col)
{
   return SetRealParameter(GetParameterID(label), value, row, col);
}


//------------------------------------------------------------------------------
// Event* GetEvent(UnsignedInt whichOne)
//------------------------------------------------------------------------------
/**
 * Retrieves an event for processing
 *
 * @param whichOne The index of the Event
 *
 * @return The event
 */
//------------------------------------------------------------------------------
Event* AveragedDoppler::GetEvent(UnsignedInt whichOne)
{
   Event *theEvent = NULL;

   switch (whichOne)
   {
      case 0:
         theEvent = &downlinkLegS;
         break;

      case 1:
         theEvent = &uplinkLegS;
         break;

      case 2:
         theEvent = &downlinkLegE;
         break;

      case 3:
         theEvent = &uplinkLegE;
         break;
   }

   return theEvent;
}


//------------------------------------------------------------------------------
// bool SetEventData(Event *locatedEvent)
//------------------------------------------------------------------------------
/**
 * Passes data from a solved Event to other events that need the data
 *
 * @param locatedEvent The Event that has been solved
 *
 * @return true if the Event was successfully processed, false if not.
 */
//------------------------------------------------------------------------------
bool AveragedDoppler::SetEventData(Event *locatedEvent)
{
   bool retval = false;

   if (locatedEvent == &downlinkLegS)
   {
      #ifdef DEBUG_EVENT
         MessageInterface::ShowMessage("Averaged Doppler is setting fixed "
               "state time for the start uplink leg\n");
      #endif
      // Set the fixed state time for the uplink leg
      Real start = t3R[0] + downlinkLegS.GetVarTimestep() - t2delay[0];
      uplinkLegS.SetFixedTimestep(start);

      // declare success!
      retval = true;
   }

   if (locatedEvent == &downlinkLegE)
   {
      #ifdef DEBUG_EVENT
         MessageInterface::ShowMessage("Averaged Doppler is setting fixed "
               "state time for the end uplink leg\n");
      #endif
      // Set the fixed state time for the uplink leg
      Real start = t3R[1] + downlinkLegE.GetVarTimestep() - t2delay[1];
      uplinkLegE.SetFixedTimestep(start);

      // declare success!
      retval = true;
   }

   if ((locatedEvent == &uplinkLegS) || (locatedEvent == &uplinkLegE))
   {
      // declare success!
      retval = true;
   }

   return retval;
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
bool AveragedDoppler::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string & name, const Integer index)
{
   return PhysicalMeasurement::SetRefObject(obj, type, name, index);
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
bool AveragedDoppler::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string & name)
{
   bool retval = false;

   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      // It's a participant, so add it to the events
      uplinkLegS.AddParticipant((SpacePoint *)obj);
      downlinkLegS.AddParticipant((SpacePoint *)obj);
      uplinkLegE.AddParticipant((SpacePoint *)obj);
      downlinkLegE.AddParticipant((SpacePoint *)obj);

      // Current code has spacecraft groundstation as its model
      // todo: Generalize to support two spacecraft interconnect
      if (obj->IsOfType(Gmat::SPACEOBJECT))
      {
         uplinkLegS.FixState(obj, false);
         uplinkLegE.FixState(obj, false);
      }
      else
      {
         downlinkLegS.FixState(obj, false);
         downlinkLegE.FixState(obj, false);
      }

      retval = true;
   }

   bool baseRet = PhysicalMeasurement::SetRefObject(obj, type, name);
   retval = (retval == true ? true : baseRet);

   return retval;
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
bool AveragedDoppler::Initialize()
{
   bool retval = false;

   if (PhysicalMeasurement::Initialize())
   {
      if (participants.size() < 2)
         MessageInterface::ShowMessage("Averaged Doppler calculations require "
               "2 participants; cannot initialize\n");
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
            MessageInterface::ShowMessage("Participant mismatch in averaged "
                  "Doppler measurement: Current code requires one Spacecraft "
                  "and one other SpacePoint participant; cannot initialize\n");
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// void InitializeMeasurement()
//------------------------------------------------------------------------------
/**
 * Initializes the measurement for use in estimation or simulation
 */
//------------------------------------------------------------------------------
void AveragedDoppler::InitializeMeasurement()
{
   PhysicalMeasurement::InitializeMeasurement();

   // Load the current delay data from the hardware
   SetHardwareDelays(false);

   // Set the coordinate systems for the uplink and downlink events
   uplinkLegS.AddCoordinateSystem(j2k);   // Base CS for the event
   downlinkLegS.AddCoordinateSystem(j2k); // Base CS for the event
   uplinkLegE.AddCoordinateSystem(j2k);   // Base CS for the event
   downlinkLegE.AddCoordinateSystem(j2k); // Base CS for the event

   // Because of the participant ordering, F1 is the fixed CS
   Integer index = uplinkLegS.GetParticipantIndex(participants[0]);
   uplinkLegS.AddCoordinateSystem(F1, index);  // Participant 1 CS for the event
   index = downlinkLegS.GetParticipantIndex(participants[0]);
   downlinkLegS.AddCoordinateSystem(F1, index);// Participant 2 CS for the event
   index = uplinkLegE.GetParticipantIndex(participants[0]);
   uplinkLegE.AddCoordinateSystem(F1, index);  // Participant 1 CS for the event
   index = downlinkLegE.GetParticipantIndex(participants[0]);
   downlinkLegE.AddCoordinateSystem(F1, index);// Participant 2 CS for the event

   index = uplinkLegS.GetParticipantIndex(participants[1]);
   uplinkLegS.AddCoordinateSystem(F2, index);  // Participant 1 CS for the event
   index = downlinkLegS.GetParticipantIndex(participants[1]);
   downlinkLegS.AddCoordinateSystem(F2, index);// Participant 2 CS for the event
   index = uplinkLegE.GetParticipantIndex(participants[1]);
   uplinkLegE.AddCoordinateSystem(F2, index);  // Participant 1 CS for the event
   index = downlinkLegE.GetParticipantIndex(participants[1]);
   downlinkLegE.AddCoordinateSystem(F2, index);// Participant 2 CS for the event

   SetupTimeIntervals();
}


//------------------------------------------------------------------------------
// void SetHardwareDelays(bool loadEvents)
//------------------------------------------------------------------------------
/**
 * Retrieves the delay data from the hardware associated with the measurement
 *
 * @param loadEvents true if the data is then passed to the associated event
 *                   objects, false if it is only used to set the internal
 *                   parameters
 */
//------------------------------------------------------------------------------
void AveragedDoppler::SetHardwareDelays(bool loadEvents)
{
   #ifdef DEBUG_HARDWARE_DELAYS
      MessageInterface::ShowMessage("AveragedDoppler time intervals before "
            "Hardware access:\n");
      MessageInterface::ShowMessage("   t1 start:      %.12lf\n",
            t1T[0] - t1delay[0]);
      MessageInterface::ShowMessage("   t1s delay:     %.12lf\n", t1delay[0]);
      MessageInterface::ShowMessage("   t1s transmit:  %.12lf\n", t1T[0]);
      MessageInterface::ShowMessage("   t2s delay:     %.12lf\n", t2delay[0]);
      MessageInterface::ShowMessage("   t2s transmit:  %.12lf\n", t2T[0]);
      MessageInterface::ShowMessage("   t3s receive:   %.12lf\n", t3R[0]);
      MessageInterface::ShowMessage("   t3s delay:     %.12lf\n", t3delay[0]);
      MessageInterface::ShowMessage("   t3s timetag:   %.12lf\n", t3E[0]);
      MessageInterface::ShowMessage("   ---------------------\n");
      MessageInterface::ShowMessage("   t1 end:        %.12lf\n",
            t1T[1] - t1delay[1]);
      MessageInterface::ShowMessage("   t1e delay:     %.12lf\n", t1delay[1]);
      MessageInterface::ShowMessage("   t1e transmit:  %.12lf\n", t1T[1]);
      MessageInterface::ShowMessage("   t2e delay:     %.12lf\n", t2delay[1]);
      MessageInterface::ShowMessage("   t2e transmit:  %.12lf\n", t2T[1]);
      MessageInterface::ShowMessage("   t3e receive:   %.12lf\n", t3R[1]);
      MessageInterface::ShowMessage("   t3e delay:     %.12lf\n", t3delay[1]);
      MessageInterface::ShowMessage("   t3e timetag:   %.12lf\n", t3E[1]);
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
         t1delay[0] = t1delay[1] =
               ((Sensor*)(participantHardware[0][i]))->GetDelay();
         // Set delay based on first transponder

         #ifdef DEBUG_HARDWARE_DELAYS
            MessageInterface::ShowMessage("   ***Getting frequency from %s"
                  "old value = %.3lf; ",
                  participantHardware[0][i]->GetName().c_str(), frequency);
         #endif
         // Frequency in Hz (parameter is in MHz)
         frequency =
               participantHardware[0][i]->GetRealParameter("Frequency") *
                     1.0e6;
         #ifdef DEBUG_HARDWARE_DELAYS
            MessageInterface::ShowMessage("   new value = %.3lf\n",
                  frequency);
         #endif

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

            MessageInterface::ShowMessage("      Getting ratio; "
                  "old value = %.12lf; ", turnaround);
         #endif

         t2delay[0] = t2delay[1] =
               ((Sensor*)(participantHardware[1][i]))->GetDelay();
         turnaround = ((Transponder*)(participantHardware[1][i]))->
               GetTurnAroundRatio();

         #ifdef DEBUG_HARDWARE_DELAYS
            MessageInterface::ShowMessage("   new value = %.12lf\n",
                  turnaround);
         #endif

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
         t3delay[0] = t3delay[1] =
               ((Sensor*)(participantHardware[0][i]))->GetDelay();
         break;
      }
   }

   if (loadEvents)
   {
      // Load the values into the light time corrections measurements as needed;
      // the only default event that we can load here is the downlink

      // Note that this default 2-way Doppler model assumes there is a hardware
      // delay at the end of each path!  Make sure to turn it off or zero it if
      // it's not part of the spec for the derived class.
      downlinkLegS.SetFixedTimestep(t3R[0]);
      downlinkLegE.SetFixedTimestep(t3R[1]);
   }

   SetupTimeIntervals();

   #ifdef DEBUG_HARDWARE_DELAYS
      MessageInterface::ShowMessage("AveragedDoppler time intervals after "
            "Hardware access:\n");

      MessageInterface::ShowMessage("   t1 start:      %.12lf\n",
            t1T[0] - t1delay[0]);
      MessageInterface::ShowMessage("   t1s delay:     %.12lf\n", t1delay[0]);
      MessageInterface::ShowMessage("   t1s transmit:  %.12lf\n", t1T[0]);
      MessageInterface::ShowMessage("   t2s delay:     %.12lf\n", t2delay[0]);
      MessageInterface::ShowMessage("   t2s transmit:  %.12lf\n", t2T[0]);
      MessageInterface::ShowMessage("   t3s receive:   %.12lf\n", t3R[0]);
      MessageInterface::ShowMessage("   t3s delay:     %.12lf\n", t3delay[0]);
      MessageInterface::ShowMessage("   t3s timetag:   %.12lf\n", t3E[0]);
      MessageInterface::ShowMessage("   ---------------------\n");
      MessageInterface::ShowMessage("   t1 end:        %.12lf\n",
            t1T[1] - t1delay[1]);
      MessageInterface::ShowMessage("   t1e delay:     %.12lf\n", t1delay[1]);
      MessageInterface::ShowMessage("   t1e transmit:  %.12lf\n", t1T[1]);
      MessageInterface::ShowMessage("   t2e delay:     %.12lf\n", t2delay[1]);
      MessageInterface::ShowMessage("   t2e transmit:  %.12lf\n", t2T[1]);
      MessageInterface::ShowMessage("   t3e receive:   %.12lf\n", t3R[1]);
      MessageInterface::ShowMessage("   t3e delay:     %.12lf\n", t3delay[1]);
      MessageInterface::ShowMessage("   t3e timetag:   %.12lf\n", t3E[1]);
   #endif
}


//------------------------------------------------------------------------------
// void AveragedDoppler::SetupTimeIntervals()
//------------------------------------------------------------------------------
/**
 * Initializes the time intervals for the light time correction iterations
 */
//------------------------------------------------------------------------------
void AveragedDoppler::SetupTimeIntervals()
{
   // Set 2 return epochs used as starting points in lighttime calcs
   t3E[0] = -interval / 2.0;
   t3E[1] =  interval / 2.0;

   // Set the receive offsets for the downlink signals
   t3R[0] = t3E[0] - t3delay[0];
   t3R[1] = t3E[1] - t3delay[1];

   // Load the downlink offsets
   downlinkLegS.SetFixedTimestep(t3R[0]);
   downlinkLegE.SetFixedTimestep(t3R[1]);
}
