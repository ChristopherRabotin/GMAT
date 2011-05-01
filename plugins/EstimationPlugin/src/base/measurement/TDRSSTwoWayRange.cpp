//$Id: TDRSSTwoWayRange.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         TDRSSTwoWayRange
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
// Created: 2010/05/10
//
/**
 * The TDRSS 2-way range core measurement model.
 */
//------------------------------------------------------------------------------


#include "TDRSSTwoWayRange.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"


//#define DEBUG_RANGE_CALC
//#define DEBUG_DERIVATIVES
//#define VIEW_PARTICIPANT_STATES
//#define CHECK_PARTICIPANT_LOCATIONS


//------------------------------------------------------------------------------
// TDRSSTwoWayRange(const std::string nomme) :
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param nomme The name of the core measurement model
 */
//------------------------------------------------------------------------------
TDRSSTwoWayRange::TDRSSTwoWayRange(const std::string nomme) :
   TwoWayRange          ("TDRSSTwoWayRange", nomme),
   tdrssUplinkDelay     (0.0),
   tdrssDownlinkDelay   (0.0),
   forwardlinkTime      (0.0),
   backlinkTime         (0.0),
   forwardlinkRange     (0.0),
   backlinkRange        (0.0)
{
   objectTypeNames.push_back("TDRSSTwoWayRange");

   // Prep value array in measurement
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "TDRSSTwoWayRange";
   currentMeasurement.type = Gmat::TDRSS_TWOWAYRANGE;

   forwardlinkLeg.SetName("TDRSSTwoWayRange_ForwardlinkLeg");
   backlinkLeg.SetName("TDRSSTwoWayRange_BacklinkLeg");

   covariance.SetDimension(1);
   covariance(0,0) = 1.0;

   eventCount = 4;
}


//------------------------------------------------------------------------------
// ~TDRSSTwoWayRange()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TDRSSTwoWayRange::~TDRSSTwoWayRange()
{
}



//------------------------------------------------------------------------------
// TDRSSTwoWayRange(const TDRSSTwoWayRange& tdrss)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param tdrss The model that is getting copied
 */
//------------------------------------------------------------------------------
TDRSSTwoWayRange::TDRSSTwoWayRange(const TDRSSTwoWayRange& tdrss) :
   TwoWayRange          (tdrss),
   tdrssUplinkDelay     (tdrss.tdrssUplinkDelay),
   tdrssDownlinkDelay   (tdrss.tdrssDownlinkDelay),
   forwardlinkTime      (tdrss.forwardlinkTime),
   backlinkTime         (tdrss.backlinkTime),
   forwardlinkLeg       (tdrss.forwardlinkLeg),
   backlinkLeg          (tdrss.backlinkLeg),
   forwardlinkRange     (tdrss.forwardlinkRange),
   backlinkRange        (tdrss.backlinkRange)
{
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "TDRSSTwoWayRange";
   currentMeasurement.type = Gmat::TDRSS_TWOWAYRANGE;
   currentMeasurement.uniqueID = tdrss.currentMeasurement.uniqueID;
   currentMeasurement.participantIDs.push_back("NotSet");
   currentMeasurement.participantIDs.push_back("NotSet");

   covariance = tdrss.covariance;
}


//------------------------------------------------------------------------------
// TDRSSTwoWayRange& operator=(const TDRSSTwoWayRange& tdrss)
//------------------------------------------------------------------------------
/**
 * Assignmant operator
 *
 * @param tdrss The model that is getting copied
 *
 * @return This TDRSS 2-way range model, configured to match tdrss.
 */
//------------------------------------------------------------------------------
TDRSSTwoWayRange& TDRSSTwoWayRange::operator=(const TDRSSTwoWayRange& tdrss)
{
   if (this != &tdrss)
   {
      TwoWayRange::operator=(tdrss);

      // Allocate exactly one value in current measurement for range
      currentMeasurement.value.clear();
      currentMeasurement.value.push_back(0.0);
      currentMeasurement.typeName = "TDRSSTwoWayRange";
      currentMeasurement.type = Gmat::TDRSS_TWOWAYRANGE;
      currentMeasurement.uniqueID = tdrss.currentMeasurement.uniqueID;

      tdrssUplinkDelay     = tdrss.tdrssUplinkDelay;
      tdrssDownlinkDelay   = tdrss.tdrssDownlinkDelay;
      forwardlinkTime      = tdrss.forwardlinkTime;
      backlinkTime         = tdrss.backlinkTime;
      forwardlinkLeg       = tdrss.forwardlinkLeg;
      backlinkLeg          = tdrss.backlinkLeg;
      forwardlinkRange     = tdrss.forwardlinkRange;
      backlinkRange        = tdrss.backlinkRange;

      covariance = tdrss.covariance;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new model that matches this one, and returns it as a GmatBase
 * pointer
 *
 * @return A new TDRSS 2-way range model configured to match this one
 */
//------------------------------------------------------------------------------
GmatBase* TDRSSTwoWayRange::Clone() const
{
   return new TDRSSTwoWayRange(*this);
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
bool TDRSSTwoWayRange::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string &name)
{
   if (obj->IsOfType(Gmat::SPACE_POINT))
   {
      if (find(participants.begin(), participants.end(), obj) == participants.end())
      {
         std::vector<Hardware*> hv;
         // Cheating here for the moment to be sure GroundStation is 1st object
         if (obj->IsOfType(Gmat::GROUND_STATION))
         {
            participants.insert(participants.begin(), (SpacePoint*)obj);
            participantHardware.insert(participantHardware.begin(), hv);
            stationParticipant = true;
         }
         else
         {
            participants.push_back((SpacePoint*)obj);
            participantHardware.push_back(hv);
         }

         // Set IDs
         currentMeasurement.participantIDs.clear();
         for (std::vector<SpacePoint*>::iterator i = participants.begin();
               i != participants.end(); ++i)
         {
            currentMeasurement.participantIDs.push_back((*i)->
                  GetStringParameter("Id"));
         }

         #ifdef DEBUG_MEASUREMENT_INITIALIZATION
            MessageInterface::ShowMessage(
                  "Added %s named %s to a %s TDRSS 2-Way Range Measurement\n",
                  obj->GetTypeName().c_str(), obj->GetName().c_str(),
                  typeName.c_str());

            if (participants.size() == 2)
               CalculateMeasurement(false);
         #endif
      }

      #ifdef DEBUG_MEASUREMENT_INITIALIZATION
         MessageInterface::ShowMessage("TDRSSTwoWayRange::SetRefObject() "
               "Participant List:\n");
         for (UnsignedInt i = 0; i < participants.size(); ++i)
            MessageInterface::ShowMessage("   %s\n",
                  participants[i]->GetName().c_str());
      #endif

      return true;
   }

   return TwoWayRange::SetRefObject(obj, type, name);
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
bool TDRSSTwoWayRange::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
      const std::string &name, const Integer index)
{
   return TwoWayRange::SetRefObject(obj, type, name, index);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the model prior to performing measurement computations
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool TDRSSTwoWayRange::Initialize()
{
   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Entered RangeMeasurement::Initialize(); "
            "this = %p\n", this);
   #endif

   bool retval = false;

   if (TwoWayRange::Initialize())
   {
      if (participants.size() < 3)
         MessageInterface::ShowMessage("TDRSS Range vector calculations "
               "require 3 participants; cannot initialize\n");
      else
      {
         // For now, require specific order for the participants
         // todo: Allow arbitrary participant ordering
         if ((participants[0]->IsOfType(Gmat::SPACE_POINT)) &&
             (participants[1]->IsOfType(Gmat::SPACECRAFT)) &&
             (participants[2]->IsOfType(Gmat::SPACECRAFT)))
         {
            satEpochID = participants[1]->GetParameterID("A1Epoch");

            for (UnsignedInt i = 0; i < participants.size(); ++i)
               currentMeasurement.participantIDs[i] =
                     participants[i]->GetStringParameter("Id");

            // Update the delay parameters
            SetHardwareDelays(false);

            // Groundstation -> TDRS
            uplinkLeg.AddParticipant(participants[0]);
            uplinkLeg.AddParticipant(participants[1]);
            uplinkLeg.FixState(participants[1], false);

            // TDRS -> target
            forwardlinkLeg.AddParticipant(participants[1]);
            forwardlinkLeg.AddParticipant(participants[2]);
            forwardlinkLeg.FixState(participants[2], false);
            forwardlinkLeg.SetFixedTimestep(-tdrssUplinkDelay);

            // target -> TDRS
            backlinkLeg.AddParticipant(participants[2]);
            backlinkLeg.AddParticipant(participants[1]);
            backlinkLeg.FixState(participants[1], false);
            backlinkLeg.SetFixedTimestep(-targetDelay);

            // TDRS -> Groundstation
            downlinkLeg.AddParticipant(participants[1]);
            downlinkLeg.AddParticipant(participants[0]);
            downlinkLeg.FixState(participants[0], false);
            downlinkLeg.SetFixedTimestep(-tdrssDownlinkDelay);

            retval = true;
         }
         else
         {
            MessageInterface::ShowMessage("Participant mismatch in TDRSS Range "
                  "measurement: Current code requires two Spacecraft and one "
                  "other SpacePoint participant; cannot initialize\n");
         }
      }
   }

   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("   Initialization %s with %d "
            "participants\n", (retval ? "succeeded" : "failed"),
            participants.size());
   #endif

   return retval;
}

//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(
//       GmatBase *obj, Integer id)
//------------------------------------------------------------------------------
/**
 * Calculates the measurement derivatives for the model
 *
 * @param obj The object supplying the "with respect to" parameter
 * @param id The ID of the parameter
 *
 * @return A matrix of the derivative data, contained in a vector of Real
 *         vectors
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& TDRSSTwoWayRange::CalculateMeasurementDerivatives(
      GmatBase *obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("TDRSSTwoWayRange::CalculateMeasurement"
            "Derivatives(%s, %d) called\n", obj->GetName().c_str(), id);
   #endif

   if (!initialized)
      InitializeMeasurement();

   GmatBase *objPtr = NULL;

   Integer size = obj->GetEstimationParameterSize(id);
   Integer objNumber = -1;

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   ParameterSize = %d\n", size);
   #endif

   if (size <= 0)
      throw MeasurementException("The derivative parameter on derivative "
            "object " + obj->GetName() + "is not recognized");

   // Check to see if obj is a participant
   for (UnsignedInt i = 0; i < this->participants.size(); ++i)
   {
      if (participants[i] == obj)
      {
         objPtr = participants[i];
         objNumber = i + 1;
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Participant %s found\n",
                  objPtr->GetName().c_str());
         #endif
         break;
      }
   }

   // Or if it is the measurement model for this object
   if (obj->IsOfType(Gmat::MEASUREMENT_MODEL))
   if (obj->GetRefObject(Gmat::CORE_MEASUREMENT, "") == this)
   {
      objPtr = obj;
      objNumber = 0;
      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   The measurement is the object\n",
               objPtr->GetName().c_str());
      #endif
   }

   if (objNumber == -1)
      throw MeasurementException(
            "TDRSSTwoWayRange error - object is neither participant nor "
            "measurement model.");

   RealArray oneRow;
   oneRow.assign(size, 0.0);
   currentDerivatives.clear();
   currentDerivatives.push_back(oneRow);

   Integer parameterID = GetParmIdFromEstID(id, obj);

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   Looking up id %d\n", parameterID);
   #endif

   if (objPtr != NULL)
   {
      if (objNumber == 1) // participant number 1, either a GroundStation or a Spacecraft
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of Participant"
                     " 1\n", objPtr->GetParameterText(parameterID).c_str());
         #endif
         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " Position is not yet implemented");
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " Velocity is not yet implemented");
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " CartesianState is not yet implemented");
         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
         else
         {
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("   Deriv is w.r.t. something "
                        "independent, so zero\n");
            #endif
            for (UnsignedInt i = 0; i < 3; ++i)
               currentDerivatives[0][i] = 0.0;
         }
      }
      else if (objNumber == 2) // participant 2, should be a TDRSS Spacecraft
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of Participant"
                     " 1\n", objPtr->GetParameterText(parameterID).c_str());
         #endif
         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " Position is not yet implemented");
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " Velocity is not yet implemented");
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " CartesianState is not yet implemented");
         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
         else
         {
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("   Deriv is w.r.t. something "
                        "independent, so zero\n");
            #endif
            for (UnsignedInt i = 0; i < 3; ++i)
               currentDerivatives[0][i] = 0.0;
         }
      }
      else if (objNumber == 3) // participant 3, always a Spacecraft
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of Participant"
                     " 3\n", objPtr->GetParameterText(parameterID).c_str());
         #endif

         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            // Get the inverse of the orbit STM at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            Rvector3 forwardlinkRderiv;
            GetRangeDerivative(forwardlinkLeg, stmInv, forwardlinkRderiv, false,
                  1, 2, true, false);

            // Downlink leg
            Rvector3 backlinkRderiv;
            GetRangeDerivative(backlinkLeg, stmInv, backlinkRderiv, false, 1, 2,
                  true, false);

            // Add 'em up per eq tbd
            for (Integer i = 0; i < 3; ++i)
               currentDerivatives[0][i] =
                     0.5 * (forwardlinkRderiv[i] + backlinkRderiv[i]);
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("Position Derivative: [%.12lf "
                     "%.12lf %.12lf]\n", currentDerivatives[0][0],
                     currentDerivatives[0][1], currentDerivatives[0][2]);
            #endif
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            // Get the inverse of the orbit STM at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            Rvector3 forwardlinkVderiv;
            GetRangeDerivative(forwardlinkLeg, stmInv, forwardlinkVderiv, false,
                  1, 2, false);

            // Downlink leg
            Rvector3 backlinkVderiv;
            GetRangeDerivative(backlinkLeg, stmInv, backlinkVderiv, false, 1, 2,
                  false);

            // Add 'em up per eq tbd
            for (Integer i = 0; i < 3; ++i)
               currentDerivatives[0][i] =
                     0.5 * (forwardlinkVderiv[i] + backlinkVderiv[i]);
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("Velocity Derivative: [%.12lf "
                     "%.12lf %.12lf]\n", currentDerivatives[0][0],
                     currentDerivatives[0][1], currentDerivatives[0][2]);
            #endif
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            // Get the inverse of the orbit STM at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            Rvector6 forwardlinkDeriv;
            GetRangeDerivative(forwardlinkLeg, stmInv, forwardlinkDeriv, false,
                  1, 2);

            // Downlink leg
            Rvector6 backlinkDeriv;
            GetRangeDerivative(backlinkLeg, stmInv, backlinkDeriv, false, 1, 2);


            // Add 'em up per eq tbd
            for (Integer i = 0; i < 6; ++i)
               currentDerivatives[0][i] =
                     0.5 * (forwardlinkDeriv[i] + backlinkDeriv[i]);
            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("CartesianState Derivative: "
                     "[%.12lf %.12lf %.12lf %.12lf %.12lf %.12lf]\n",
                     currentDerivatives[0][0], currentDerivatives[0][1],
                     currentDerivatives[0][2], currentDerivatives[0][3],
                     currentDerivatives[0][4], currentDerivatives[0][5]);
            #endif
         }
         else if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
         else
         {
            for (UnsignedInt i = 0; i < 3; ++i)
               currentDerivatives[0][i] = 0.0;
         }

      }
      else if (objNumber == 0) // measurement model
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of the "
                     "measurement model\n",
                     objPtr->GetParameterText(parameterID).c_str());
         #endif
         if (objPtr->GetParameterText(parameterID) == "Bias")
         {
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 1.0;
         }
      }
      else
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of a non-"
                     "Participant\n",
                     objPtr->GetParameterText(parameterID).c_str());
         #endif
         for (UnsignedInt i = 0; i < 3; ++i)
            currentDerivatives[0][i] = 0.0;
      }

      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   Deriv =\n   ");

         for (Integer i = 0; i < size; ++i)
            MessageInterface::ShowMessage("   %.12le",currentDerivatives[0][i]);
         MessageInterface::ShowMessage("\n");
      #endif
   }
   return currentDerivatives;
}


//------------------------------------------------------------------------------
// bool Evaluate(bool withEvents)
//------------------------------------------------------------------------------
/**
 * Calculates measurement values based on the current state of the participants.
 *
 * This method can perform the calculations either with or without event
 * corrections.  When calculating without events, the purpose of the calculation
 * is to determine feasibility of the measurement.
 *
 * @param withEvents Flag used to toggle event inclusion
 *
 * @return true if the measurement was calculated, false if not
 */
//------------------------------------------------------------------------------
bool TDRSSTwoWayRange::Evaluate(bool withEvents)
{
   bool retval = false;

   if (!initialized)
      InitializeMeasurement();

   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Entered TDRSSTwoWayRange::Evaluate()\n");
      MessageInterface::ShowMessage("  ParticipantCount: %d\n",
            participants.size());
   #endif

   if (withEvents == false)
   {
      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("TDRSS 2-Way Range Calculation without "
               "events\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES
         DumpParticipantStates("++++++++++++++++++++++++++++++++++++++++++++\n"
               "Evaluating TDRSS 2-Way Range without events");
      #endif

      if (CheckLOS(0, 1, NULL) && CheckLOS(1, 2, NULL))
      {
         // Calculate the range vector between the groundstation and the TDRS
         CalculateRangeVectorInertial(0, 1);
         Rvector3 outState;

         // Set feasibility off of topospheric horizon, set by the Z value in topo
         // coords
         std::string updateAll = "All";
         UpdateRotationMatrix(currentMeasurement.epoch, updateAll);
         outState = R_o_j2k * rangeVecInertial;
         currentMeasurement.feasibilityValue = outState[2];

         #ifdef CHECK_PARTICIPANT_LOCATIONS
            MessageInterface::ShowMessage("Evaluating without events\n");
            MessageInterface::ShowMessage("Calculating TDRSS 2-Way Range at epoch "
                  "%.12lf\n", currentMeasurement.epoch);
            MessageInterface::ShowMessage("   J2K Location of %s, id = '%s':  %s",
                  participants[0]->GetName().c_str(),
                  currentMeasurement.participantIDs[0].c_str(),
                  p1Loc.ToString().c_str());
            MessageInterface::ShowMessage("   J2K Location of %s, id = '%s':  %s",
                  participants[1]->GetName().c_str(),
                  currentMeasurement.participantIDs[1].c_str(),
                  p2Loc.ToString().c_str());
            Rvector3 bfLoc = R_o_j2k * p1Loc;
            MessageInterface::ShowMessage("   BodyFixed Location of %s:  %s",
                  participants[0]->GetName().c_str(),
                  bfLoc.ToString().c_str());
            bfLoc = R_o_j2k * p2Loc;
            MessageInterface::ShowMessage("   BodyFixed Location of %s:  %s\n",
                  participants[1]->GetName().c_str(),
                  bfLoc.ToString().c_str());
         #endif

         if (currentMeasurement.feasibilityValue > 0.0)
         {
            currentMeasurement.isFeasible = true;
            currentMeasurement.value[0] = rangeVecInertial.GetMagnitude();
            currentMeasurement.eventCount = 4;

            retval = true;
         }
      }
      else
      {
         currentMeasurement.isFeasible = false;
         currentMeasurement.value[0] = 0.0;
         currentMeasurement.eventCount = 0;
      }

      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("Calculating Range at epoch %.12lf\n",
               currentMeasurement.epoch);
         MessageInterface::ShowMessage("   Location of %s, id = '%s':  %s",
               participants[0]->GetName().c_str(),
               currentMeasurement.participantIDs[0].c_str(),
               p1Loc.ToString().c_str());
         MessageInterface::ShowMessage("   Location of %s, id = '%s':  %s",
               participants[1]->GetName().c_str(),
               currentMeasurement.participantIDs[1].c_str(),
               p2Loc.ToString().c_str());
         MessageInterface::ShowMessage("   Range Vector (inertial):  %s\n",
               rangeVecInertial.ToString().c_str());
         MessageInterface::ShowMessage("   R(Groundstation) dot RangeVec =  %lf\n",
               currentMeasurement.feasibilityValue);
         MessageInterface::ShowMessage("   Feasibility:  %s\n",
               (currentMeasurement.isFeasible ? "true" : "false"));
         MessageInterface::ShowMessage("   Range is %.12lf\n",
               currentMeasurement.value[0]);
         MessageInterface::ShowMessage("   EventCount is %d\n",
               currentMeasurement.eventCount);
      #endif

      #ifdef SHOW_RANGE_CALC
         MessageInterface::ShowMessage("Range at epoch %.12lf is ",
               currentMeasurement.epoch);
         if (currentMeasurement.isFeasible)
            MessageInterface::ShowMessage("feasible, value = %.12lf\n",
               currentMeasurement.value[0]);
         else
            MessageInterface::ShowMessage("not feasible\n");
      #endif
   }
   else
   {
      // Calculate the corrected range measurement
      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("TDRSS 2-Way Range Calculation:\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES
         DumpParticipantStates("********************************************\n"
               "Evaluating TDRSS 2-Way Range with located events");
         MessageInterface::ShowMessage("Calculating TDRSS 2-Way Range at epoch "
               "%.12lf\n", currentMeasurement.epoch);
      #endif

      // Get the range from the downlink
      Rvector3 r1, r2;
      r1 = downlinkLeg.GetPosition(participants[0]);
      r2 = downlinkLeg.GetPosition(participants[1]);
      Rvector3 rVector = r2 - r1;
      Real realRange = rVector.GetMagnitude();

      #ifdef VIEW_PARTICIPANT_STATES
         MessageInterface::ShowMessage("   %s at downlink: %.12lf %.12lf %.12lf\n",
               participants[0]->GetName().c_str(), r1[0], r1[1], r1[2]);
         MessageInterface::ShowMessage("   %s at downlink: %.12lf %.12lf %.12lf\n",
               participants[1]->GetName().c_str(), r2[0], r2[1], r2[2]);
      #endif
      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("   Downlink Range:  %.12lf\n",
               rVector.GetMagnitude());
      #endif

      r1 = backlinkLeg.GetPosition(participants[1]);
      r2 = backlinkLeg.GetPosition(participants[2]);
      if (CheckSat2SatLOS(r1, r2, NULL) == false)
         return false;
      rVector = r2 - r1;
      realRange += rVector.GetMagnitude();
      #ifdef VIEW_PARTICIPANT_STATES
         MessageInterface::ShowMessage("   %s at backlink: %.12lf %.12lf %.12lf\n",
               participants[1]->GetName().c_str(), r1[0], r1[1], r1[2]);
         MessageInterface::ShowMessage("   %s at backlink: %.12lf %.12lf %.12lf\n",
               participants[2]->GetName().c_str(), r2[0], r2[1], r2[2]);
      #endif

      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("   Backlink Range:  %.12lf\n",
                  rVector.GetMagnitude());
      #endif

      // Add the pseudorange from the transponder delay
      targetDelay = GetDelay(2,0);
      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("   TDRSS target delay is %.12lf\n",
               targetDelay);
      #endif
      realRange += GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M *
            targetDelay;

      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("   Delay Range (%le sec delay):  "
               "%.12lf\n", targetDelay,
               (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM/GmatMathConstants::KM_TO_M * targetDelay));
      #endif

      r1 = forwardlinkLeg.GetPosition(participants[2]);
      r2 = forwardlinkLeg.GetPosition(participants[1]);
      if (CheckSat2SatLOS(r1, r2, NULL) == false)
         return false;
      rVector = r2 - r1;
      realRange += rVector.GetMagnitude();

      #ifdef VIEW_PARTICIPANT_STATES
         MessageInterface::ShowMessage("   %s at frwdlink: %.12lf %.12lf %.12lf\n",
               participants[2]->GetName().c_str(), r1[0], r1[1], r1[2]);
         MessageInterface::ShowMessage("   %s at frwdlink: %.12lf %.12lf %.12lf\n",
               participants[1]->GetName().c_str(), r2[0], r2[1], r2[2]);
      #endif
      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("   forwardlink Range:  %.12lf\n",
               rVector.GetMagnitude());
      #endif

      r1 = uplinkLeg.GetPosition(participants[1]);
      r2 = uplinkLeg.GetPosition(participants[0]);
      rVector = r2 - r1;
      realRange += rVector.GetMagnitude();

      #ifdef VIEW_PARTICIPANT_STATES
         MessageInterface::ShowMessage("   %s at frwdlink: %.12lf %.12lf %.12lf\n",
               participants[1]->GetName().c_str(), r1[0], r1[1], r1[2]);
         MessageInterface::ShowMessage("   %s at frwdlink: %.12lf %.12lf %.12lf\n",
               participants[0]->GetName().c_str(), r2[0], r2[1], r2[2]);
      #endif
      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("   Uplink Range:  %.12lf\n",
               rVector.GetMagnitude());
      #endif

      currentMeasurement.value[0] = realRange / 2.0;

      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("   Calculated Range:  %.12lf\n",
               currentMeasurement.value[0]);
      #endif

      retval = true;
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
Event* TDRSSTwoWayRange::GetEvent(UnsignedInt whichOne)
{
   Event *retval = NULL;

   // Work backwards from the signal reception
   switch (whichOne)
   {
      case 0:
         retval = &downlinkLeg;
         break;

      case 1:
         retval = &backlinkLeg;
         break;

      case 2:
         retval = &forwardlinkLeg;
         break;

      case 3:
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
bool TDRSSTwoWayRange::SetEventData(Event *locatedEvent)
{
   bool retval = false;
   Real start;

   if (locatedEvent == &downlinkLeg)
   {
      #ifdef DEBUG_EVENT
         MessageInterface::ShowMessage("2-Way Range is setting fixed state "
               "time for the uplink leg\n");
      #endif

      // Set the fixed state time for the backlink leg
      start = -receiveDelay + downlinkLeg.GetVarTimestep() - tdrssDownlinkDelay;
      backlinkLeg.SetFixedTimestep(start);

      // declare success!
      retval = true;
   }

   if (locatedEvent == &backlinkLeg)
   {
      #ifdef DEBUG_EVENT
         MessageInterface::ShowMessage("TDRSS 2-Way Range is setting fixed "
               "state time for the backlink leg\n");
      #endif

      // Set the fixed state time for the backlink leg
      start = - receiveDelay + downlinkLeg.GetVarTimestep() - tdrssDownlinkDelay
            + backlinkLeg.GetVarTimestep() - targetDelay;
      forwardlinkLeg.SetFixedTimestep(start);

      // declare success!
      retval = true;
   }

   if (locatedEvent == &forwardlinkLeg)
   {
      #ifdef DEBUG_EVENT
         MessageInterface::ShowMessage("TDRSS 2-Way Range is setting fixed "
               "state time for the forwardlink leg\n");
      #endif

      // Set the fixed state time for the backlink leg
      start = - receiveDelay + downlinkLeg.GetVarTimestep() - tdrssDownlinkDelay
            + backlinkLeg.GetVarTimestep() - targetDelay
            + forwardlinkLeg.GetVarTimestep() - tdrssUplinkDelay;
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
void TDRSSTwoWayRange::SetHardwareDelays(bool loadEvents)
{
   #ifdef DEBUG_HARDWARE_DELAYS
      MessageInterface::ShowMessage("In <%p>, initial Hardware delays set to:\n"
            "   Transmitter delay:           %.12lf sec\n"
            "   Uplink transponder delay:    %.12lf sec\n"
            "   Target transponder delay:    %.12lf sec\n"
            "   Downlink transponder delay:  %.12lf sec\n"
            "   Receiver delay:              %.12lf sec\n", this,
            transmitDelay, tdrssUplinkDelay, targetDelay, tdrssDownlinkDelay,
            receiveDelay);
   #endif

   Real satDelay = targetDelay;
   TwoWayRange::SetHardwareDelays(false);
   targetDelay = satDelay;

   // Now get up/forward link delay
   for (UnsignedInt i = 0; i < participantHardware[1].size(); ++i)
   {
      if (participantHardware[1][i]->IsOfType("Transponder"))
      {
         tdrssUplinkDelay = ((Sensor*)(participantHardware[1][i]))->GetDelay();
         break;
      }
   }

   // Now get back/downward link delay
   for (UnsignedInt i = participantHardware[1].size(); i > 0; --i)
   {
      if (participantHardware[1][i-1]->IsOfType("Transponder"))
      {
         tdrssDownlinkDelay = ((Sensor*)(participantHardware[1][i-1]))->GetDelay();
         break;
      }
   }

   // Transponder on the 3rd participant was up/forward, so move it if found
   for (UnsignedInt i = 0; i < participantHardware[2].size(); ++i)
   {
      if (participantHardware[2][i]->IsOfType("Transponder"))
      {
         #ifdef DEBUG_HARDWARE_DELAYS
            MessageInterface::ShowMessage("   ***Getting delay from %s\n",
                  participantHardware[2][i]->GetName().c_str());
         #endif
         targetDelay = ((Sensor*)(participantHardware[2][i]))->GetDelay();
         break;
      }
   }

//   if (loadEvents)
//   {
//      // Load the values into the light time corrections measurements as needed;
//      // the only default event that we can load here is the downlink
//      downlinkLeg.SetFixedTimestep(-receiveDelay);
//   }

   #ifdef DEBUG_HARDWARE_DELAYS
      MessageInterface::ShowMessage("Hardware delays set to:\n"
            "   Transmitter delay:           %.12lf sec\n"
            "   Uplink transponder delay:    %.12lf sec\n"
            "   Target transponder delay:    %.12lf sec\n"
            "   Downlink transponder delay:  %.12lf sec\n"
            "   Receiver delay:              %.12lf sec\n",
            transmitDelay, tdrssUplinkDelay, targetDelay, tdrssDownlinkDelay,
            receiveDelay);
   #endif
}
