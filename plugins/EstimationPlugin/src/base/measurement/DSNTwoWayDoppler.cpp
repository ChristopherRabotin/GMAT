//$Id: DSNTwoWayDoppler.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         DSNTwoWayDoppler
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
 * Measurement model class for Doppler measurements made by the deep space
 * network
 */
//------------------------------------------------------------------------------


#include "DSNTwoWayDoppler.hpp"
#include "MeasurementException.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "Transmitter.hpp"
#include "Receiver.hpp"
#include "Transponder.hpp"


//#define DEBUG_DOPPLER_CALC_WITH_EVENTS
//#define VIEW_PARTICIPANT_STATES_WITH_EVENTS
//#define DEBUG_DOPPLER_CALC
//#define VIEW_PARTICIPANT_STATES


//------------------------------------------------------------------------------
// DSNTwoWayDoppler(const std::string& withName)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param withName Name of the new object
 */
//------------------------------------------------------------------------------
DSNTwoWayDoppler::DSNTwoWayDoppler(const std::string& withName) :
   AveragedDoppler      ("DSNTwoWayDoppler", withName)
{
   objectTypeNames.push_back("DSNTwoWayDoppler");

   // Prep value array in measurement
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "DSNTwoWayDoppler";
   currentMeasurement.type = Gmat::DSN_TWOWAYDOPPLER;
   currentMeasurement.eventCount = 4;

   currentMeasurement.participantIDs.push_back("NotSet");
   currentMeasurement.participantIDs.push_back("NotSet");

   covariance.SetDimension(1);
   covariance(0,0) = 1.0;
}


//------------------------------------------------------------------------------
// ~DSNTwoWayDoppler()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DSNTwoWayDoppler::~DSNTwoWayDoppler()
{
}


//------------------------------------------------------------------------------
// DSNTwoWayDoppler(const DSNTwoWayDoppler & dd)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param dd The object that gets copied to make this new one
 */
//------------------------------------------------------------------------------
DSNTwoWayDoppler::DSNTwoWayDoppler(const DSNTwoWayDoppler & dd) :
   AveragedDoppler      (dd)
{
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "DSNTwoWayDoppler";
   currentMeasurement.type = Gmat::DSN_TWOWAYDOPPLER;
   currentMeasurement.eventCount = 4;
   currentMeasurement.uniqueID = dd.currentMeasurement.uniqueID;
   currentMeasurement.participantIDs.push_back("NotSet");
   currentMeasurement.participantIDs.push_back("NotSet");

   covariance = dd.covariance;
}


//------------------------------------------------------------------------------
// DSNTwoWayDoppler& operator=(const DSNTwoWayDoppler& dd)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param dd The object that gets copied to make this new one
 *
 * @return this object configured to match dd
 */
//------------------------------------------------------------------------------
DSNTwoWayDoppler& DSNTwoWayDoppler::operator=(const DSNTwoWayDoppler& dd)
{
   if (this != &dd)
   {
      AveragedDoppler::operator=(dd);

      // Allocate exactly one value in current measurement for range
      currentMeasurement.value.clear();
      currentMeasurement.value.push_back(0.0);
      currentMeasurement.typeName = "DSNTwoWayDoppler";
      currentMeasurement.type = Gmat::DSN_TWOWAYDOPPLER;
      currentMeasurement.eventCount = 4;
      currentMeasurement.uniqueID = dd.currentMeasurement.uniqueID;
      currentMeasurement.participantIDs = dd.currentMeasurement.participantIDs;

      covariance = dd.covariance;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Method used to make a new instance from a GMatBase pointer
 *
 * @return A copy of this object
 */
//------------------------------------------------------------------------------
GmatBase* DSNTwoWayDoppler::Clone() const
{
   return new DSNTwoWayDoppler(*this);
}


//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(
//          GmatBase *obj, Integer id)
//------------------------------------------------------------------------------
/**
 * Derivative evaluation method used in estimation
 *
 * @param obj The object supplying the "with respect to" parameter
 * @param id The id of the with respect to parameter
 *
 * @return The array of derivative data
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& DSNTwoWayDoppler::CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("DSNTwoWayDoppler::CalculateMeasurement"
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
   for (UnsignedInt i = 0; i < participants.size(); ++i)
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
            "USNTwoWayRange error - object is neither participant nor "
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
                  participants[0]->GetName() +" position is not yet implemented");

         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() +" velocity is not yet implemented");
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " CartesianState is not yet "
                        "implemented");
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
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] = 0.0;
         }

      }
      else if (objNumber == 2) // participant 2, always a Spacecraft
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of Participant"
                     " 2\n", objPtr->GetParameterText(parameterID).c_str());
         #endif

         if (objPtr->GetParameterText(parameterID) == "Position")
         {
            // Get the inverse of the orbit STM at the measurement epoch
            // Will need adjustment if stm changes
            Rmatrix stmInv(6,6);
            GetInverseSTM(obj, stmInv);

            Rvector3 uplinkDerivS, uplinkDerivE;
            GetRangeDerivative(uplinkLegS, stmInv, uplinkDerivS, false, 0,
                  1, true, false);
            GetRangeDerivative(uplinkLegE, stmInv, uplinkDerivE, false, 0,
                  1, true, false);

            // Downlink leg
            Rvector3 downlinkDerivS, downlinkDerivE;
            GetRangeDerivative(downlinkLegS, stmInv, downlinkDerivS, false, 0,
                  1, true, false);
            GetRangeDerivative(downlinkLegE, stmInv, downlinkDerivE, false, 0,
                  1, true, false);

            Real preFactor = turnaround * frequency / (interval *
                  GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M);

            for (Integer i = 0; i < size; ++i)
            {
               currentDerivatives[0][i] = preFactor * (
                     uplinkDerivE[i] + downlinkDerivE[i] -
                     uplinkDerivS[i] - downlinkDerivS[i]);
            }

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

            Rvector3 uplinkDerivS, uplinkDerivE;
            GetRangeDerivative(uplinkLegS, stmInv, uplinkDerivS, false, 0, 1,
                  false, true);
            GetRangeDerivative(uplinkLegE, stmInv, uplinkDerivE, false, 0, 1,
                  false, true);

            // Downlink leg
            Rvector3 downlinkDerivS, downlinkDerivE;
            GetRangeDerivative(downlinkLegS, stmInv, downlinkDerivS, false, 0,
                  1, false, true);
            GetRangeDerivative(downlinkLegE, stmInv, downlinkDerivE, false, 0,
                  1, false, true);

            Real preFactor = turnaround * frequency / (interval *
                  GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M);

            for (Integer i = 0; i < size; ++i)
            {
               currentDerivatives[0][i] = preFactor * (
                     uplinkDerivE[i] + downlinkDerivE[i] -
                     uplinkDerivS[i] - downlinkDerivS[i]);
            }

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

            Rvector6 uplinkDerivS, uplinkDerivE;
            GetRangeDerivative(uplinkLegS, stmInv, uplinkDerivS, false);
            GetRangeDerivative(uplinkLegE, stmInv, uplinkDerivE, false);

            // Downlink leg
            Rvector6 downlinkDerivS, downlinkDerivE;
            GetRangeDerivative(downlinkLegS, stmInv, downlinkDerivS, false);
            GetRangeDerivative(downlinkLegE, stmInv, downlinkDerivE, false);

            Real preFactor = turnaround * frequency / (interval *
                  GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M);

            for (Integer i = 0; i < size; ++i)
            {
               currentDerivatives[0][i] = preFactor * (
                     uplinkDerivE[i] + downlinkDerivE[i] -
                     uplinkDerivS[i] - downlinkDerivS[i]);
            }

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
         for (Integer i = 0; i < size; ++i)
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
      for (Integer i = 0; i < size; ++i)
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
 * Evaluates the measurement
 *
 * When evaluated without events, the DSN 2-way Doppler measurement value is
 * always zero.  That evaluation is used to determine is there is a nominal
 * (geometric) line of sight between the participants at the measurement epoch.
 *
 * @param withEvents Flag used to trigger usage of data from event evaluation as
 *                   part of the measurement
 *
 * @return true if the event is feasible, false if it could not be calculated
 *         or is infeasible
 */
//------------------------------------------------------------------------------
bool DSNTwoWayDoppler::Evaluate(bool withEvents)
{
   bool retval = false;

   if (!initialized)
      InitializeMeasurement();

   #ifdef DEBUG_DOPPLER_CALC
      MessageInterface::ShowMessage("Entered DSNTwoWayDoppler::Evaluate()\n");
      MessageInterface::ShowMessage("  ParticipantCount: %d\n",
            participants.size());
   #endif

   if (withEvents == false)
   {
      #ifdef DEBUG_DOPPLER_CALC
         MessageInterface::ShowMessage("DSN 2-Way Doppler Calculation without "
               "events\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES
         DumpParticipantStates("++++++++++++++++++++++++++++++++++++++++++++\n"
               "Evaluating DSN 2-Way Doppler without events");
      #endif

      CalculateRangeVectorInertial();
      Rvector3 outState;

      // Set feasibility off of topocentric horizon, set by the Z value in topo
      // coords
      std::string updateAll = "All";
      UpdateRotationMatrix(currentMeasurement.epoch, updateAll);
      outState = R_o_j2k * rangeVecInertial;
      currentMeasurement.feasibilityValue = outState[2];

      #ifdef CHECK_PARTICIPANT_LOCATIONS
         MessageInterface::ShowMessage("Evaluating without events\n");
         MessageInterface::ShowMessage("Calculating DSN 2-Way Doppler at epoch "
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
         currentMeasurement.value[0] = 0.0;     // No Doppler shift w/o events
         currentMeasurement.eventCount = 4;

         retval = true;
      }
      else
      {
         currentMeasurement.isFeasible = false;
         currentMeasurement.value[0] = 0.0;
         currentMeasurement.eventCount = 0;
      }

      #ifdef DEBUG_DOPPLER_CALC
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
         MessageInterface::ShowMessage("   Range Vector:  %s\n",
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
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("DSN 2-Way Doppler Calculation:\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES_WITH_EVENTS
         DumpParticipantStates("********************************************\n"
               "Evaluating DSN 2-Way Doppler with located events");
      #endif


      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("For Measurement S:\n");
      #endif
      // 1. Get the range from the down link leg for measurement S:
      Rvector3 rS1, rS2;
      rS1 = downlinkLegS.GetPosition(participants[0]);
      rS2 = downlinkLegS.GetPosition(participants[1]);
      Rvector3 downlinkVectorS = rS2 - rS1;
      Real downlinkRangeS = downlinkVectorS.GetMagnitude();
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  .Downlink Range:  %.12lf km\n",
            downlinkRangeS);
      #endif

      // 2. Calculate down link range rate for measurement S:
      Rvector3 pS1V = downlinkLegS.GetVelocity(participants[0]);
      Rvector3 pS2V = downlinkLegS.GetVelocity(participants[1]);
      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
      Rvector3 downRRateVecS = pS2V - pS1V /* - r12_j2k_vel*/;
      Rvector3 rangeUnit = downlinkVectorS.GetUnitVector();
      downlinkRangeRate[0] = downRRateVecS * rangeUnit;
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  .Downlink Range Rate:  %.12lf km/s\n",
            downlinkRangeRate[0]);
      #endif

      // 3. Get the range from the up link leg for measurement S:
      Rvector3 rS3, rS4;
      rS3 = uplinkLegS.GetPosition(participants[0]);
      rS4 = uplinkLegS.GetPosition(participants[1]);
      Rvector3 uplinkVectorS = rS4 - rS3;
      Real uplinkRangeS = uplinkVectorS.GetMagnitude();
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  .Uplink Range:  %.12lf km\n",
            uplinkRangeS);
      #endif

      // 4. Calculate up link range rate for measurement S:
      Rvector3 pS3V = uplinkLegS.GetVelocity(participants[0]);
      Rvector3 pS4V = uplinkLegS.GetVelocity(participants[1]);
      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
      Rvector3 upRRateVecS = pS4V - pS3V /* - r12_j2k_vel*/ ;
      rangeUnit = uplinkVectorS.GetUnitVector();
      uplinkRangeRate[0] = upRRateVecS * rangeUnit;
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  .Uplink Range Rate:  %.12lf km/s\n",
            uplinkRangeRate[0]);
      #endif


      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("For Measurement E:\n");
      #endif
      // 5. Get the range from the down link leg for measurement E:
      Rvector3 rE1, rE2;
      rE1 = downlinkLegE.GetPosition(participants[0]);
      rE2 = downlinkLegE.GetPosition(participants[1]);
      Rvector3 downlinkVectorE = rE2 - rE1;
      Real downlinkRangeE = downlinkVectorE.GetMagnitude();
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  .Downlink Range:  %.12lf km\n",
            downlinkRangeE);
      #endif

      // 6. Calculate down link range rate for measurement E:
      Rvector3 pE1V = downlinkLegE.GetVelocity(participants[0]);
      Rvector3 pE2V = downlinkLegE.GetVelocity(participants[1]);
      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
      Rvector3 downRRateVecE = pE2V - pE1V /* - r12_j2k_vel*/;
      rangeUnit = downlinkVectorE.GetUnitVector();
      downlinkRangeRate[1] = downRRateVecE * rangeUnit;
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  .Downlink Range Rate:  %.12lf km/s\n",
            downlinkRangeRate[1]);
      #endif

      // 7. Get the range from the up link leg for measurement E:
      Rvector3 rE3, rE4;
      rE3 = uplinkLegE.GetPosition(participants[0]);
      rE4 = uplinkLegE.GetPosition(participants[1]);
      Rvector3 uplinkVectorE = rE4 - rE3;
      Real uplinkRangeE = uplinkVectorE.GetMagnitude();
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  .Uplink Range:  %.12lf km\n",
            uplinkRangeE);
      #endif

     // 8. Calculate up link range rate for measurement E:
      Rvector3 pE3V = uplinkLegE.GetVelocity(participants[0]);
      Rvector3 pE4V = uplinkLegE.GetVelocity(participants[1]);
      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
      Rvector3 upRRateVecE = pE4V - pE3V /* - r12_j2k_vel*/ ;
      rangeUnit = uplinkVectorE.GetUnitVector();
      uplinkRangeRate[1] = upRRateVecE * rangeUnit;
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  .Uplink Range Rate:  %.12lf km/s\n",
            uplinkRangeRate[1]);
      #endif


      Real dtS, dtE, dtdt, preFactor;

		// 9. Get sensors used in DSN 2-ways Doppler
	   if (participantHardware.empty()||
	   		((!participantHardware.empty())&&
	   		  participantHardware[0].empty()&&
	   		  participantHardware[1].empty()
	   		)
	   	)
	   {
	      // Total time for the start path
	      dtS = t1delay[0] - uplinkLegS.GetVarTimestep() + t2delay[0] -
	            downlinkLegS.GetVarTimestep() + t3delay[0];

	      // Total time for the end path
	      dtE = t1delay[1] - uplinkLegE.GetVarTimestep() + t2delay[1] -
	            downlinkLegE.GetVarTimestep() + t3delay[1];

         #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
	         MessageInterface::ShowMessage("Ideal measurement (no hardware delay and no media correction involve):\n");
            MessageInterface::ShowMessage("  Time for start path:  %.12lf s\n",
               dtS);
            MessageInterface::ShowMessage("  Time for end path:    %.12lf s\n",
               dtE);
         #endif
         // Difference between the two
         dtdt = dtE - dtS;

         preFactor = turnaround * frequency / interval;

         #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("  Frequency: %.12lf MHz\n"
                                          "  Turnaround Factor: %.12lf\n"
                                          "  Delta t: %.12lf s\n", frequency/1.0e6,
                                         turnaround, interval);
            MessageInterface::ShowMessage("  Time difference:      %.12lf s\n",
               dtdt);
            MessageInterface::ShowMessage("  Prefactor:            %.12lf\n",
               preFactor);
         #endif

         currentMeasurement.value[0] = preFactor * dtdt;
         currentMeasurement.isFeasible = true;

         #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("  Calculated Doppler:  %.12lf\n",
               currentMeasurement.value[0]);
         #endif

	      return true;
	   }

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("Real measurement (with hardware delay and media correction):\n");
      #endif
		ObjectArray objList1;
		ObjectArray objList2;
		ObjectArray objList3;
		//			objList1 := all transmitters in participantHardware list
		//			objList2 := all receivers in participantHardware list
		//			objList3 := all transponders in participantHardware list
		for(std::vector<Hardware*>::iterator hw = this->participantHardware[0].begin();
		  		hw != this->participantHardware[0].end(); ++hw)
		{
		 	if ((*hw) != NULL)
		  	{
		  		if ((*hw)->GetTypeName() == "Transmitter")
		  			objList1.push_back(*hw);
		  		if ((*hw)->GetTypeName() == "Receiver")
		  			objList2.push_back(*hw);
		  	}
		  	else
		  		MessageInterface::ShowMessage(" sensor = NULL\n");
		}

		for(std::vector<Hardware*>::iterator hw = this->participantHardware[1].begin();
		  		hw != this->participantHardware[1].end(); ++hw)
		{
		  	if ((*hw) != NULL)
		  	{
		  		if ((*hw)->GetTypeName() == "Transponder")
		  			objList3.push_back(*hw);
		  	}
		  	else
		  		MessageInterface::ShowMessage(" sensor = NULL\n");
		}

		if (objList1.size() != 1)
		{
		  	MessageInterface::ShowMessage("The first participant does not have only 1 transmitter to send signal.\n");
		   throw new MeasurementException("The first participant does not have only 1 transmitter to send signal.\n");
		}
		if (objList2.size() != 1)
		{
		 	MessageInterface::ShowMessage("The first participant does not have only 1 receiver to receive signal.\n");
		   throw new MeasurementException("The first participant does not have only 1 receiver to receive signal.\n");
		}
		if (objList3.size() != 1)
		{
		  	MessageInterface::ShowMessage("The second participant does not have only 1 transponder to transpond signal.\n");
		  	throw new MeasurementException("The second participant does not have only 1 transponder to transpond signal.\n");
		}

	   Transmitter* 	gsTransmitter 	= (Transmitter*)objList1[0];
	   Receiver* 		gsReceiver 		= (Receiver*)objList2[0];
	   Transponder* 	scTransponder 	= (Transponder*)objList3[0];
	   if (gsTransmitter == NULL)
	   {
	   	MessageInterface::ShowMessage("Transmitter is NULL object.\n");
	   	throw new GmatBaseException("Transmitter is NULL object.\n");
	   }
	   if (gsReceiver == NULL)
	   {
	   	MessageInterface::ShowMessage("Receiver is NULL object.\n");
	   	throw new GmatBaseException("Receiver is NULL object.\n");
	   }
	   if (scTransponder == NULL)
	   {
	   	MessageInterface::ShowMessage("Transponder is NULL object.\n");
	   	throw new GmatBaseException("Transponder is NULL object.\n");
	   }

		#ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("   List of sensors: %s, %s, %s\n",
				gsTransmitter->GetName().c_str(), gsReceiver->GetName().c_str(),
				scTransponder->GetName().c_str());
		#endif

      // 10. Get transmitter, receiver, and transponder delays:
		Real transmitDelay = gsTransmitter->GetDelay();
		Real receiveDelay = gsReceiver->GetDelay();
		Real targetDelay = scTransponder->GetDelay();

		#ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("  Transmitter delay = %.12lf s\n", transmitDelay);
			MessageInterface::ShowMessage("  Receiver delay = %.12lf s\n", receiveDelay);
			MessageInterface::ShowMessage("  Transponder delay = %.12lf s\n", targetDelay);
		#endif


		// 11. Get frequency from transmitter of ground station (participants[0])
		Signal* uplinkSignal = gsTransmitter->GetSignal();
		Real uplinkFreq = uplinkSignal->GetValue();
		#ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("  UpLink signal frequency = %.12lf MHz\n", uplinkFreq);
		#endif

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("  Media Correction Calculations:\n");
      #endif

		// 12. Calculate for measurement S:
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
	      MessageInterface::ShowMessage("  *For measurement S:\n");
      #endif
      // 12.1. Calculate media correction for uplink leg:
	   #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
	      MessageInterface::ShowMessage("     Media correction for uplink leg for measurement S:\n");
	   #endif
	   Real timeOffset = ((uplinkRangeS + downlinkRangeS)*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)/GmatTimeConstants::SECS_PER_DAY;
	   RealArray uplinkCorrectionS = CalculateMediaCorrection(uplinkFreq, rS1, rS2, currentMeasurement.epoch+t3E[0]/GmatTimeConstants::SECS_PER_DAY-timeOffset);
	   Real uplinkRangeCorrectionS = uplinkCorrectionS[0]/GmatMathConstants::KM_TO_M;
	   Real uplinkRealRangeS = uplinkRangeS + uplinkRangeCorrectionS;
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("     Uplink range correction = %.12lf km\n",uplinkRangeCorrectionS);
			MessageInterface::ShowMessage("     Uplink real range = %.12lf km\n",uplinkRealRangeS);
		#endif

		// 12.2. Doppler shift the frequency from the transmitter using uplinkRangeRate:
		Real uplinkDSFreqS = (1 - uplinkRangeRate[0]*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*uplinkFreq;
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("    Uplink Doppler shift frequency = %.12lf MHz\n", uplinkDSFreqS);
		#endif

	   // 12.3. Set frequency for the input signal of transponder
	   Signal* inputSignal = scTransponder->GetSignal(0);
	   inputSignal->SetValue(uplinkDSFreqS);
	   scTransponder->SetSignal(inputSignal, 0);

	   // 12.4. Check the transponder feasibility to receive the input signal:
	   if (scTransponder->IsFeasible(0) == false)
	   {
	   	currentMeasurement.isFeasible = false;
	   	currentMeasurement.value[0] = 0;
	   	MessageInterface::ShowMessage("The transponder is unfeasible to receive uplink signal.\n");
	   	throw new GmatBaseException("The transponder is unfeasible to receive uplink signal.\n");
	   }

	   // 12.5. Get frequency of transponder output signal
	   Signal* outputSignal = scTransponder->GetSignal(1);
	   Real downlinkFreqS = outputSignal->GetValue();
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("    Downlink frequency = %.12lf Mhz\n", downlinkFreqS);
		#endif

	   // 12.6. Doppler shift the transponder output frequency by the downlinkRangeRate:
	   Real downlinkDSFreqS = (1 - downlinkRangeRate[0]*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*downlinkFreqS;
	   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("    Downlink Doppler shift frequency = %.12lf MHz\n", downlinkDSFreqS);
		#endif

		// 12.7. Set frequency on receiver
		Signal* downlinkSignal = gsReceiver->GetSignal();
		downlinkSignal->SetValue(downlinkDSFreqS);

		// 12.8. Check the receiver feasibility to receive the downlink signal
		if (gsReceiver->IsFeasible() == false)
		{
		  	currentMeasurement.isFeasible = false;
		  	currentMeasurement.value[0] = 0;
		  	MessageInterface::ShowMessage("The receiver is unfeasible to receive downlink signal.\n");
		  	throw new MeasurementException("The receiver is unfeasible to receive downlink signal.\n");
		}

	   // 12.9. Calculate media correction for downlink leg:
	   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
	      MessageInterface::ShowMessage("      Media correction for downlink leg for measurement S:\n");
	   #endif
		RealArray downlinkCorrectionS = CalculateMediaCorrection(downlinkDSFreqS, rS3, rS4, currentMeasurement.epoch+t3E[0]/GmatTimeConstants::SECS_PER_DAY);
	   Real downlinkRangeCorrectionS = downlinkCorrectionS[0]/GmatMathConstants::KM_TO_M;
		Real downlinkRealRangeS = downlinkRangeS + downlinkRangeCorrectionS;
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("      Downlink range correction = %.12lf km\n",downlinkRangeCorrectionS);
			MessageInterface::ShowMessage("      Downlink real range = %.12lf km\n",downlinkRealRangeS);
		#endif

		// 13. Calculate for measurement E:
	   #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
		   MessageInterface::ShowMessage("  *For measurement E:\n");
	   #endif
	   // 13.1. Calculate media correction for uplink leg:
		#ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
		   MessageInterface::ShowMessage("     Media correction for uplink leg for measurement E\n");
		#endif
		timeOffset = ((uplinkRangeE + downlinkRangeE)*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)/GmatTimeConstants::SECS_PER_DAY;
	   RealArray uplinkCorrectionE = CalculateMediaCorrection(uplinkFreq, rE1, rE2, currentMeasurement.epoch+t3E[1]/GmatTimeConstants::SECS_PER_DAY-timeOffset);
	   Real uplinkRangeCorrectionE = uplinkCorrectionE[0]/GmatMathConstants::KM_TO_M;
		Real uplinkRealRangeE = uplinkRangeE + uplinkRangeCorrectionE;
	   #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("     Uplink range correction = %.12lf km\n",uplinkRangeCorrectionE);
			MessageInterface::ShowMessage("     Uplink real range = %.12lf km\n",uplinkRealRangeE);
      #endif

		// 13.2. Doppler shift the frequency from the transmitter using uplinkRangeRate:
		Real uplinkDSFreqE = (1 - uplinkRangeRate[1]*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*uplinkFreq;
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("    Uplink Doppler shift frequency = %.12lf MHz\n", uplinkDSFreqE);
		#endif

	   // 13.3. Set frequency for the input signal of transponder
		inputSignal = scTransponder->GetSignal(0);
		inputSignal->SetValue(uplinkDSFreqE);
		scTransponder->SetSignal(inputSignal, 0);

		// 13.4. Check the transponder feasibility to receive the input signal:
		if (scTransponder->IsFeasible(0) == false)
		{
		  	currentMeasurement.isFeasible = false;
		  	currentMeasurement.value[0] = 0;
		  	MessageInterface::ShowMessage("The transponder is unfeasible to receive uplink signal.\n");
	   	throw new GmatBaseException("The transponder is unfeasible to receive uplink signal.\n");
	   }

	   // 13.5. Get frequency of transponder output signal
		outputSignal = scTransponder->GetSignal(1);
		Real downlinkFreqE = outputSignal->GetValue();
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("    Downlink frequency = %.12lf Mhz\n", downlinkFreqE);
		#endif

	   // 13.6. Doppler shift the transponder output frequency by the downlinkRangeRate:
		Real downlinkDSFreqE = (1 - downlinkRangeRate[1]*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*downlinkFreqE;
	   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("    Downlink Doppler shift frequency = %.12lf MHz\n", downlinkDSFreqE);
		#endif

		// 13.7. Set frequency on receiver
		downlinkSignal = gsReceiver->GetSignal();
		downlinkSignal->SetValue(downlinkDSFreqE);

		// 13.8. Check the receiver feasibility to receive the downlink signal
		if (gsReceiver->IsFeasible() == false)
		{
		  	currentMeasurement.isFeasible = false;
		  	currentMeasurement.value[0] = 0;
		  	MessageInterface::ShowMessage("The receiver is unfeasible to receive downlink signal.\n");
		  	throw new MeasurementException("The receiver is unfeasible to receive downlink signal.\n");
		}

		// 13.9. Calculate media correction for downlink leg:
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
		   MessageInterface::ShowMessage("      Media correction for downlink leg for measurement E:\n");
		#endif
		RealArray downlinkCorrectionE = CalculateMediaCorrection(downlinkDSFreqE, rE3, rE4, currentMeasurement.epoch+t3E[1]/GmatTimeConstants::SECS_PER_DAY);
	   Real downlinkRangeCorrectionE = downlinkCorrectionE[0]/GmatMathConstants::KM_TO_M;
		Real downlinkRealRangeE = downlinkRangeE + downlinkRangeCorrectionE;
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("      Downlink range correction = %.12lf km\n",downlinkRangeCorrectionE);
			MessageInterface::ShowMessage("      Downlink real range = %.12lf km\n",downlinkRealRangeE);
		#endif


	   // 14. Total times for the start path and the end path:
	   dtS = (uplinkRealRangeS + downlinkRealRangeS)*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM +
	   		transmitDelay + receiveDelay + targetDelay;
	   dtE = (uplinkRealRangeE + downlinkRealRangeE)*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM +
	   		transmitDelay + receiveDelay + targetDelay;

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
	      MessageInterface::ShowMessage("Measurement with hardware delay and media correction:\n");
         MessageInterface::ShowMessage("  Time for start path:  %.12lf s\n", dtS);
         MessageInterface::ShowMessage("  Time for end path:    %.12lf s\n", dtE);
      #endif

      // 15. Calculate Frequency Doppler Shift:
      dtdt = dtE - dtS;
      turnaround = scTransponder->GetTurnAroundRatio();
      frequency = uplinkFreq*1.0e6;
      preFactor = turnaround * frequency / interval;

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  Frequency: %.12lf MHz\n"
                                       "  Turnaround Factor: %.12lf\n"
                                       "  Delta t: %.12lf s\n", frequency/1.0e6,
                                      turnaround, interval);
         MessageInterface::ShowMessage("  Time difference:      %.12lf s\n",
            dtdt);
         MessageInterface::ShowMessage("  Prefactor:            %.12lf\n",
            preFactor);
      #endif

      currentMeasurement.value[0] = preFactor * dtdt;
      currentMeasurement.isFeasible = true;
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("  Calculated Doppler:  %.12lf\n",
            currentMeasurement.value[0]);
      #endif



      #ifdef PRELIMINARY_DERIVATIVE_CHECK
         MessageInterface::ShowMessage("Participants:\n ");
         for (UnsignedInt i = 0; i < participants.size(); ++i)
            MessageInterface::ShowMessage("   %d: %s of type %s\n", i,
                  participants[i]->GetName().c_str(),
                  participants[i]->GetTypeName().c_str());

         Integer id = participants[1]->GetType() * 250 +
               participants[1]->GetParameterID("CartesianX");
         CalculateMeasurementDerivatives(participants[1], id);
      #endif

      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// void SetHardwareDelays(bool loadEvents)
//------------------------------------------------------------------------------
/**
 * Retrieves hardware delays is available
 *
 * @param loadEvents Flag used to indicate if events can be preloaded; if true,
 *                   those that can be loaded are passed the corresponding
 *                   delays.
 */
//------------------------------------------------------------------------------
void DSNTwoWayDoppler::SetHardwareDelays(bool loadEvents)
{
   AveragedDoppler::SetHardwareDelays(loadEvents);
}
