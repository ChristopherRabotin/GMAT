//$Id: USNTwoWayRange.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         USNTwoWayRange
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
// Created: 2009/12/18
//
/**
 * The USN 2-way range core measurement model.
 */
//------------------------------------------------------------------------------


#include "USNTwoWayRange.hpp"
#include "gmatdefs.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "Spacecraft.hpp"
#include "GroundstationInterface.hpp"
#include "Antenna.hpp"
#include "Transmitter.hpp"
#include "Receiver.hpp"
#include "Transponder.hpp"
#include "TimeSystemConverter.hpp"

#include "SpacePoint.hpp"

//#define DEBUG_RANGE_CALC
//#define VIEW_PARTICIPANT_STATES
//#define CHECK_PARTICIPANT_LOCATIONS
//#define DEBUG_DERIVATIVES
//#define PRELIMINARY_DERIVATIVE_CHECK    // Calc derivative while simulating


//------------------------------------------------------------------------------
// USNTwoWayRange(const std::string nomme)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param nomme The name of the core measurement model
 */
//------------------------------------------------------------------------------
USNTwoWayRange::USNTwoWayRange(const std::string nomme) :
   TwoWayRange          ("USNTwoWayRange", nomme),
   targetRangeRate      (0.0),
   uplinkRangeRate      (0.0),
   downlinkRangeRate    (0.0)
{
   objectTypeNames.push_back("USNTwoWayRange");

   // Prep value array in measurement
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "USNTwoWayRange";
   currentMeasurement.type = Gmat::USN_TWOWAYRANGE;
   currentMeasurement.eventCount = 2;

   covariance.SetDimension(1);
   covariance(0,0) = 1.0;
}


//------------------------------------------------------------------------------
// ~USNTwoWayRange()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
USNTwoWayRange::~USNTwoWayRange()
{
}



//------------------------------------------------------------------------------
// USNTwoWayRange(const USNTwoWayRange& usn)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param usn The model that is getting copied
 */
//------------------------------------------------------------------------------
USNTwoWayRange::USNTwoWayRange(const USNTwoWayRange& usn) :
   TwoWayRange       (usn),
   targetRangeRate   (usn.targetRangeRate),
   uplinkRangeRate   (usn.uplinkRangeRate),
   downlinkRangeRate (usn.downlinkRangeRate)
{
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "USNTwoWayRange";
   currentMeasurement.type = Gmat::USN_TWOWAYRANGE;
   currentMeasurement.eventCount = 2;
   currentMeasurement.uniqueID = usn.currentMeasurement.uniqueID;
   currentMeasurement.participantIDs.push_back("NotSet");
   currentMeasurement.participantIDs.push_back("NotSet");

   covariance = usn.covariance;
}


//------------------------------------------------------------------------------
// USNTwoWayRange& operator=(const USNTwoWayRange& usn)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param usn The model that is getting copied
 *
 * @return This USN 2-way range model, configured to match usn.
 */
//------------------------------------------------------------------------------
USNTwoWayRange& USNTwoWayRange::operator=(const USNTwoWayRange& usn)
{
   if (this != &usn)
   {
      TwoWayRange::operator=(usn);

      // Allocate exactly one value in current measurement for range
      currentMeasurement.value.clear();
      currentMeasurement.value.push_back(0.0);
      currentMeasurement.typeName = "USNTwoWayRange";
      currentMeasurement.type = Gmat::USN_TWOWAYRANGE;
      currentMeasurement.uniqueID = usn.currentMeasurement.uniqueID;

      targetRangeRate = usn.targetRangeRate;
      uplinkRangeRate = usn.uplinkRangeRate;
      downlinkRangeRate = usn.downlinkRangeRate;
      covariance = usn.covariance;
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
 * @return A new USN 2-way range model configured to match this one
 */
//------------------------------------------------------------------------------
GmatBase* USNTwoWayRange::Clone() const
{
   return new USNTwoWayRange(*this);
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
bool USNTwoWayRange::Initialize()
{
   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Entered RangeMeasurement::Initialize(); "
            "this = %p\n", this);
   #endif

   bool retval = false;

   if (TwoWayRange::Initialize())
      retval = true;

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
const std::vector<RealArray>& USNTwoWayRange::CalculateMeasurementDerivatives(
      GmatBase *obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("USNTwoWayRange::CalculateMeasurement"
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

//            CalculateRangeVectorInertial();
//            Rvector3 tmp, result;
//            Rvector3 rangeUnit = rangeVecInertial.GetUnitVector();
//            #ifdef DEBUG_DERIVATIVES
//               MessageInterface::ShowMessage("   RVInertial      = %.12lf %.12lf %.12lf\n",
//                        rangeVecInertial[0], rangeVecInertial[1], rangeVecInertial[2]);
//               MessageInterface::ShowMessage("   Unit RVInertial = %.12lf %.12lf %.12lf ",
//                        rangeUnit[0], rangeUnit[1], rangeUnit[2]);
//            #endif
//            if (stationParticipant)
//            {
//               for (UnsignedInt i = 0; i < 3; ++i)
//                  tmp[i] = - rangeUnit[i];
//
//               // for a Ground Station, need to rotate to the F1 frame
//               result = tmp * R_j2k_1;
//               for (UnsignedInt jj = 0; jj < 3; jj++)
//                  currentDerivatives[0][jj] = result[jj];
//            }
//            else
//            {
//               // for a spacecraft participant 1, we don't need the rotation matrices (I33)
//               for (UnsignedInt i = 0; i < 3; ++i)
//                  currentDerivatives[0][i] = - rangeUnit[i];
//            }
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() +" velocity is not yet implemented");

//            for (UnsignedInt i = 0; i < 3; ++i)
//               currentDerivatives[0][i] = 0.0;
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() + " CartesianState is not yet implemented");
//
//            CalculateRangeVectorInertial();
//            Rvector3 tmp, result;
//            Rvector3 rangeUnit = rangeVecInertial.GetUnitVector();
//            #ifdef DEBUG_DERIVATIVES
//               MessageInterface::ShowMessage("   RVInertial      = %.12lf %.12lf %.12lf\n",
//                        rangeVecInertial[0], rangeVecInertial[1], rangeVecInertial[2]);
//               MessageInterface::ShowMessage("   Unit RVInertial = %.12lf %.12lf %.12lf ",
//                        rangeUnit[0], rangeUnit[1], rangeUnit[2]);
//            #endif
//            if (stationParticipant)
//            {
//               for (UnsignedInt i = 0; i < size; ++i)
//                  tmp[i] = - rangeUnit[i];
//
//               // for a Ground Station, need to rotate to the F1 frame
//               result = tmp * R_j2k_1;
//               for (UnsignedInt jj = 0; jj < size; jj++)
//                  currentDerivatives[0][jj] = result[jj];
//            }
//            else
//            {
//               // for a spacecraft participant 1, we don't need the rotation matrices (I33)
//               for (UnsignedInt i = 0; i < size; ++i)
//                  currentDerivatives[0][i] = - rangeUnit[i];
//            }
//            // velocity all zeroes
//            for (UnsignedInt ii = 3; ii < size; ii++)
//               currentDerivatives[0][ii] = 0.0;
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

            Rvector3 uplinkRderiv;
            GetRangeDerivative(uplinkLeg, stmInv, uplinkRderiv, false, 0, 1,
                  true, false);

            // Downlink leg
            Rvector3 downlinkRderiv;
            GetRangeDerivative(downlinkLeg, stmInv, downlinkRderiv, false, 0, 1,
                  true, false);

            // Add 'em up per eq 7.52 and 7.53
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] =
                     0.5 * (uplinkRderiv[i] + downlinkRderiv[i]);
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

            Rvector3 uplinkVderiv;
            GetRangeDerivative(uplinkLeg, stmInv, uplinkVderiv, false, 0, 1,
                  false);

            // Downlink leg
            Rvector3 downlinkVderiv;
            GetRangeDerivative(downlinkLeg, stmInv, downlinkVderiv, false, 0, 1,
                  false);

            // Add 'em up per eq 7.52 and 7.53
            for (Integer i = 0; i < size; ++i)
               currentDerivatives[0][i] =
                     0.5 * (uplinkVderiv[i] + downlinkVderiv[i]);
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

            Rvector6 uplinkDeriv;
            GetRangeDerivative(uplinkLeg, stmInv, uplinkDeriv, false);

            // Downlink leg
            Rvector6 downlinkDeriv;
            GetRangeDerivative(downlinkLeg, stmInv, downlinkDeriv, false);


            // Add 'em up per eq 7.52 and 7.53
            for (Integer i = 0; i < 6; ++i)
               currentDerivatives[0][i] =
                     0.5 * (uplinkDeriv[i] + downlinkDeriv[i]);
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
bool USNTwoWayRange::Evaluate(bool withEvents)
{
   bool retval = false;

   if (!initialized)
      InitializeMeasurement();

   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Entered USNTwoWayRange::Evaluate()\n");
      MessageInterface::ShowMessage("  ParticipantCount: %d\n",
            participants.size());
   #endif

   if (withEvents == false)
   {
      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("USN 2-Way Range Calculation without "
               "events\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES
         DumpParticipantStates("++++++++++++++++++++++++++++++++++++++++++++\n"
               "Evaluating USN 2-Way Range without events");
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
         MessageInterface::ShowMessage("Calculating USN 2-Way Range at epoch "
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
         currentMeasurement.eventCount = 2;

         retval = true;
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
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("USN 2-Way Range Calculation:\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES_WITH_EVENTS
         DumpParticipantStates("********************************************\n"
               "Evaluating USN 2-Way Range with located events");
      #endif

      // 1. Get the range from the down link
      Rvector3 r1, r2;
      r1 = downlinkLeg.GetPosition(participants[0]);
      r2 = downlinkLeg.GetPosition(participants[1]);
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("r1 = (%f,  %f,  %f)\n", r1.Get(0), r1.Get(1), r1.Get(2));
         MessageInterface::ShowMessage("r2 = (%f,  %f,  %f)\n", r2.Get(0), r2.Get(1), r2.Get(2));
      #endif
      Rvector3 downlinkVector = r2 - r1;		// rVector = r2 - r1;
      downlinkRange = downlinkVector.GetMagnitude();
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Downlink Range = r2-r1:  %.12lf km\n",
               downlinkRange);
      #endif

	   // 2. Calculate down link range rate:
      Rvector3 p1V = downlinkLeg.GetVelocity(participants[0]);
      Rvector3 p2V = downlinkLeg.GetVelocity(participants[1]);
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("p1V = (%f,  %f,  %f)\n", p1V.Get(0), p1V.Get(1), p1V.Get(2));
         MessageInterface::ShowMessage("p2V = (%f,  %f,  %f)\n", p2V.Get(0), p2V.Get(1), p2V.Get(2));
      #endif
      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
      Rvector downRRateVec = p2V - p1V /* - r12_j2k_vel*/;
      Rvector3 rangeUnit = downlinkVector.GetUnitVector();
      downlinkRangeRate = downRRateVec * rangeUnit;
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Downlink Range Rate:  %.12lf km/s\n",
               downlinkRangeRate);
      #endif

      // 3. Get the transponder delay
      targetDelay = GetDelay(1,0);
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage(
               "    USN Transponder delay for %s = %.12lf s\n",
               participants[1]->GetName().c_str(), targetDelay);
      #endif

      // 4. Get the range from the uplink
      Rvector3 r3, r4;
      r3 = uplinkLeg.GetPosition(participants[0]);
      r4 = uplinkLeg.GetPosition(participants[1]);
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("r3 = (%f,  %f,  %f)\n", r3.Get(0), r3.Get(1), r3.Get(2));
         MessageInterface::ShowMessage("r4 = (%f,  %f,  %f)\n", r4.Get(0), r4.Get(1), r4.Get(2));
      #endif
      Rvector3 uplinkVector = r4 - r3;
      uplinkRange = uplinkVector.GetMagnitude();
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Uplink Range = r4-r3:  %.12lf km\n",
               uplinkRange);
      #endif

	   // 5. Calculate up link range rate
      Rvector3 p3V = uplinkLeg.GetVelocity(participants[0]);
      Rvector3 p4V = uplinkLeg.GetVelocity(participants[1]);
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("p3V = (%f,  %f,  %f)\n", p3V.Get(0), p3V.Get(1), p3V.Get(2));
         MessageInterface::ShowMessage("p4V = (%f,  %f,  %f)\n", p4V.Get(0), p4V.Get(1), p4V.Get(2));
      #endif
      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
      Rvector upRRateVec = p4V - p3V /* - r12_j2k_vel*/ ;
      rangeUnit = uplinkVector.GetUnitVector();
      uplinkRangeRate = upRRateVec * rangeUnit;
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Uplink Range Rate:  %.12lf km/s\n",
               uplinkRangeRate);
      #endif

      // 5.1. Target range rate: Do we need this as well?
      targetRangeRate = (downlinkRangeRate + uplinkRangeRate) / 2.0;
	   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Target Range Rate:  %.12lf km/s\n",
                targetRangeRate);
		#endif
      

	   // 6. Get sensors used in USN 2-ways range
	   ObjectArray objList1;
	   ObjectArray objList2;
	   ObjectArray objList3;
	   //			objList1 := all transmitters in participantHardware list
	   //			objList2 := all receivers in participantHardware list
	   //			objList3 := all transponders in participantHardware list
	   if (participantHardware.empty()||
	   		((!participantHardware.empty())&&
	   		  participantHardware[0].empty()&&
	   		  participantHardware[1].empty()
	   		)
	   	)
	   {
	   	// DO NOT LEAVE THIS TYPE OF MESSAGE IN THE CODE WITHOUT #ifdef WRAPPERS!!!
         //MessageInterface::ShowMessage("    Ideal measurement (no hardware delay and no media correction involve):\n");
		   Real realRange = (uplinkRange + downlinkRange)/2;
		   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
				MessageInterface::ShowMessage("   Range = %.12lf km\n", realRange);
			#endif

	   	// Set value for currentMeasurement
		   currentMeasurement.value[0] = realRange;
	      currentMeasurement.isFeasible = true;

	      return true;
	   }

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

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("   List of sensors: %s, %s, %s\n",
					gsTransmitter->GetName().c_str(), gsReceiver->GetName().c_str(),
					scTransponder->GetName().c_str());
		#endif


	   // 7. Get frequency from transmitter of ground station (participants[0])
	   Signal* uplinkSignal = gsTransmitter->GetSignal();
	   Real uplinkFreq = uplinkSignal->GetValue();

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("   UpLink signal frequency = %.12lf MHz\n", uplinkFreq);
		#endif

	   // 8. Calculate media correction for uplink leg:
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("      Media correction for uplink leg\n");
      #endif
      Real roundTripTime = ((uplinkRange + downlinkRange)*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)/GmatTimeConstants::SECS_PER_DAY;
      // DO NOT LEAVE THIS TYPE OF MESSAGE IN THE CODE WITHOUT #ifdef WRAPPERS!!!
      //MessageInterface::ShowMessage("Round trip time = %.12lf\n", roundTripTime);
      RealArray uplinkCorrection = CalculateMediaCorrection(uplinkFreq, r1, r2, currentMeasurement.epoch - roundTripTime);
      Real uplinkRangeCorrection = uplinkCorrection[0]/GmatMathConstants::KM_TO_M;
      Real uplinkRealRange = uplinkRange + uplinkRangeCorrection;
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("      Uplink range correction = %.12lf km\n",uplinkRangeCorrection);
			MessageInterface::ShowMessage("      Uplink real range = %.12lf km\n",uplinkRealRange);
		#endif

		// 9. Doppler shift the frequency from the transmitter using uplinkRangeRate:
	   Real uplinkDSFreq = (1 - uplinkRangeRate*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*uplinkFreq;

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("    Uplink Doppler shift frequency = %.12lf MHz\n", uplinkDSFreq);
		#endif

	   // 10.Set frequency for the input signal of transponder
	   Signal* inputSignal = scTransponder->GetSignal(0);
	   inputSignal->SetValue(uplinkDSFreq);
	   scTransponder->SetSignal(inputSignal, 0);

	   // 11. Check the transponder feasibility to receive the input signal:
	   if (scTransponder->IsFeasible(0) == false)
	   {
	   	 currentMeasurement.isFeasible = false;
	   	 currentMeasurement.value[0] = 0;
	   	 MessageInterface::ShowMessage("The transponder is unfeasible to receive uplink signal.\n");
	   	 throw new GmatBaseException("The transponder is unfeasible to receive uplink signal.\n");
	   }

	   // 12. Get frequency of transponder output signal
	   Signal* outputSignal = scTransponder->GetSignal(1);
	   Real downlinkFreq = outputSignal->GetValue();

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("    Downlink frequency = %.12lf Mhz\n", downlinkFreq);
		#endif

	   // 13. Doppler shift the transponder output frequency by the downlinkRangeRate:
	   Real downlinkDSFreq = (1 - downlinkRangeRate*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*downlinkFreq;

	   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("    Downlink Doppler shift frequency = %.12lf MHz\n", downlinkDSFreq);
		#endif

	   // 14. Set frequency on receiver
	   Signal* downlinkSignal = gsReceiver->GetSignal();
	   downlinkSignal->SetValue(downlinkDSFreq);

	   // 15. Check the receiver feasibility to receive the downlink signal
	   if (gsReceiver->IsFeasible() == false)
	   {
	   	 currentMeasurement.isFeasible = false;
	   	 currentMeasurement.value[0] = 0;
	   	 throw new MeasurementException("The receiver is unfeasible to receive downlink signal.\n");
	   }

	   // 16. Calculate media correction for downlink leg:
	   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
	      MessageInterface::ShowMessage("      Media correction for downlink leg\n");
	   #endif
	   RealArray downlinkCorrection = CalculateMediaCorrection(downlinkDSFreq, r3, r4, currentMeasurement.epoch);
	   Real downlinkRangeCorrection = downlinkCorrection[0]/GmatMathConstants::KM_TO_M;
	   Real downlinkRealRange = downlinkRange + downlinkRangeCorrection;
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("      Downlink range correction = %.12lf km\n",downlinkRangeCorrection);
			MessageInterface::ShowMessage("      Downlink real range = %.12lf km\n",downlinkRealRange);
		#endif


	   // 17. Calculate uplink time and down link time: (Is it needed???)
	   uplinkTime   = uplinkRealRange*GmatMathConstants::KM_TO_M / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;
	   downlinkTime = downlinkRealRange*GmatMathConstants::KM_TO_M / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("    Uplink time = %.12lf s\n",uplinkTime);
			MessageInterface::ShowMessage("    Downlink time = %.12lf s\n",downlinkTime);
		#endif

	   // 18. Calculate real range
	   Real realRange = uplinkRealRange + downlinkRealRange +
	   		targetDelay*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M;
	   
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Calculated real range = %.12lf km\n", realRange/2);
      #endif

	   // 19. Set value for currentMeasurement
	   currentMeasurement.value[0] = realRange / 2.0;
      currentMeasurement.isFeasible = true;


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
