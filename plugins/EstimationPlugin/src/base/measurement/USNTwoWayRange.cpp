//$Id: USNTwoWayRange.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         USNTwoWayRange
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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

// this needs to be at the top for Ionosphere to work on Mac!
#include "RandomNumber.hpp"

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

//#define DEBUG_RANGE_CALC_WITH_EVENTS
//#define VIEW_PARTICIPANT_STATES_WITH_EVENTS
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
//          GetRangeDerivative(uplinkLeg, stmInv, uplinkDeriv, false, 0, 1, true, true);

            // Downlink leg
            Rvector6 downlinkDeriv;
            GetRangeDerivative(downlinkLeg, stmInv, downlinkDeriv, false);
//          GetRangeDerivative(downlinkLeg, stmInv, downlinkDeriv, true, 1, 0, true, true);


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

   // Get minimum elevation angle for ground station
   Real minAngle;
   if (participants[0]->IsOfType(Gmat::GROUND_STATION))
      minAngle = ((GroundstationInterface*)participants[0])->GetRealParameter("MinimumElevationAngle");
   else if (participants[1]->IsOfType(Gmat::GROUND_STATION))
      minAngle = ((GroundstationInterface*)participants[1])->GetRealParameter("MinimumElevationAngle");

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
//    outState = R_o_j2k * rangeVecInertial;
//    currentMeasurement.feasibilityValue = outState[2];
      outState = (R_o_j2k * rangeVecInertial).GetUnitVector();
      currentMeasurement.feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;      // elevation angle in degree

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

      //if (currentMeasurement.feasibilityValue > minAngle)
      {
         currentMeasurement.isFeasible = true;
         currentMeasurement.value[0] = rangeVecInertial.GetMagnitude();
         currentMeasurement.eventCount = 2;

         SetHardwareDelays(false);

         retval = true;
      }
      //else
      //{
      //   currentMeasurement.isFeasible = false;
      //   currentMeasurement.value[0] = 0.0;
      //   currentMeasurement.eventCount = 0;
      //}

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
         MessageInterface::ShowMessage("   Elevation angle =  %lf degree\n",
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

      SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
      std::string cbName1 = ((SpacePoint*)participants[0])->GetJ2000BodyName();
      CelestialBody* cb1 = solarSystem->GetBody(cbName1);
      std::string cbName2 = ((SpacePoint*)participants[1])->GetJ2000BodyName();
      CelestialBody* cb2 = solarSystem->GetBody(cbName2);


      // 1. Get the range from the down link
      Rvector3 r1, r2;                  // position of downlink leg's participants in central body MJ2000Eq coordinate system 
      Rvector3 r1B, r2B;                // position of downlink leg's participants in solar system bary center MJ2000Eq coordinate system

      r1 = downlinkLeg.GetPosition(participants[0]);                              // position of station at reception time t3R in central body MJ2000Eq coordinate system
      r2 = downlinkLeg.GetPosition(participants[1]);                              // position of spacecraft at transmit time t2T in central body MJ2000Eq coordinate system
      t3R = downlinkLeg.GetEventData((GmatBase*) participants[0]).epoch;          // reception time at station for downlink leg
      t2T = downlinkLeg.GetEventData((GmatBase*) participants[1]).epoch;          // transmit time at spacecraft for downlink leg

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("Debug downlinkLeg <'%s',%p>: r1 = (%lf  %lf  %lf)\n", downlinkLeg.GetName().c_str(), &downlinkLeg, r1[0], r1[1], r1[2]);
         MessageInterface::ShowMessage("                             r2 = (%lf  %lf  %lf)\n", r2[0], r2[1], r2[2]);
      #endif

      Rvector3 ssb2cb_t3R = cb1->GetMJ2000Position(t3R) - ssb->GetMJ2000Position(t3R);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t3R
      Rvector3 ssb2cb_t2T = cb2->GetMJ2000Position(t2T) - ssb->GetMJ2000Position(t2T);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t2T
      
      r1B = ssb2cb_t3R + r1;                                                                // position of station at reception time t3R in SSB coordinate system
      r2B = ssb2cb_t2T + r2;                                                                // position of spacecraft at transmit time t2T in SSB coordinate system

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         Rmatrix33 mt = downlinkLeg.GetEventData((GmatBase*) participants[0]).rInertial2obj.Transpose();
         MessageInterface::ShowMessage("1. Get downlink leg range:\n");
         MessageInterface::ShowMessage("   Station %s position in %sMJ2000 coordinate system    : r1 = (%.12lf, %.12lf, %.12lf)km  at epoch t3R = %.12lf\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), r1.Get(0), r1.Get(1), r1.Get(2), t3R);
         MessageInterface::ShowMessage("   Spacecraft %s position in %sMJ2000 coordinate system : r2 = (%.12lf, %.12lf, %.12lf)km  at epoch t2T = %.12lf\n", participants[1]->GetName().c_str(),  participants[1]->GetJ2000BodyName().c_str(), r2.Get(0), r2.Get(1), r2.Get(2), t2T);
         MessageInterface::ShowMessage("   Station %s position in SSB coordinate system         : r1B = (%.12lf, %.12lf, %.12lf)km  at epoch t3R = %.12lf\n", participants[0]->GetName().c_str(), r1B.Get(0), r1B.Get(1), r1B.Get(2), t3R);
         MessageInterface::ShowMessage("   Spacecraft %s position in  SSB coordinate system     : r2B = (%.12lf, %.12lf, %.12lf)km  at epoch t2T = %.12lf\n", participants[1]->GetName().c_str(), r2B.Get(0), r2B.Get(1), r2B.Get(2), t2T);

         MessageInterface::ShowMessage("   Transformation matrix from Earth fixed coordinate system to EarthFK5 coordinate system at epoch t3R = %.12lf:\n", t3R);
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt(0,0), mt(0,1), mt(0,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt(1,0), mt(1,1), mt(1,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt(2,0), mt(2,1), mt(2,2));
      #endif
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 downlinkVector = r2 - r1;
#else
      Rvector3 downlinkVector = r2B - r1B;      // rVector = r2 - r1;
#endif
      downlinkRange = downlinkVector.GetMagnitude();

      // Calculate ET-TAI at t3R:
      Real ettaiT3 = downlinkLeg.ETminusTAI(t3R, (GmatBase*)participants[0]);

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         #ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("   Downlink range without relativity correction = r2-r1:  %.12lf km\n", downlinkRange);
         #else
         MessageInterface::ShowMessage("   Downlink range without relativity correction = r2B-r1B:  %.12lf km\n", downlinkRange);
         #endif
         MessageInterface::ShowMessage("   Relativity correction for downlink leg    = %.12lf km\n", downlinkLeg.GetRelativityCorrection());
         MessageInterface::ShowMessage("   Downlink range with relativity correction = %.12lf km\n", downlinkRange + downlinkLeg.GetRelativityCorrection());
         MessageInterface::ShowMessage("   (ET-TAI) at t3R = %.12le s\n", ettaiT3);
      #endif


      // 2. Calculate down link range rate:
      Rvector3 p1V = downlinkLeg.GetVelocity(participants[0]);                        // velocity of station at reception time t3R in central body MJ2000 coordinate system
      Rvector3 p2V = downlinkLeg.GetVelocity(participants[1]);                        // velocity of specraft at transmit time t2T in central body MJ2000 coordinate system

      Rvector3 ssb2cbV_t3R = cb1->GetMJ2000Velocity(t3R) - ssb->GetMJ2000Velocity(t3R);      // velocity of central body at time t3R w.r.t SSB MJ2000Eq coordinate system
      Rvector3 ssb2cbV_t2T = cb2->GetMJ2000Velocity(t2T) - ssb->GetMJ2000Velocity(t2T);      // velocity of central body at time t2T w.r.t SSB MJ2000Eq coordinate system
      
      Rvector3 p1VB = ssb2cbV_t3R + p1V;                                             // velocity of station at reception time t3R in SSB coordinate system
      Rvector3 p2VB = ssb2cbV_t2T + p2V;                                             // velocity of spacecraft at transmit time t2T in SSB coordinate system

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("2. Get downlink leg range rate:\n");
         MessageInterface::ShowMessage("   Station %s velocity in %sMJ2000 coordinate system    : p1V = (%.12lf, %.12lf, %.12lf)km/s\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), p1V.Get(0), p1V.Get(1), p1V.Get(2));
         MessageInterface::ShowMessage("   Spacecraft %s velocity in %sMJ2000 coordinate system : p2V = (%.12lf, %.12lf, %.12lf)km/s\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), p2V.Get(0), p2V.Get(1), p2V.Get(2));
         MessageInterface::ShowMessage("   Station %s velocity in SSB coordinate system         : p1VB = (%.12lf, %.12lf, %.12lf)km/s\n", participants[0]->GetName().c_str(), p1VB.Get(0), p1VB.Get(1), p1VB.Get(2));
         MessageInterface::ShowMessage("   Spacecraft %s velocity in SSB coordinate system      : p2VB = (%.12lf, %.12lf, %.12lf)km/s\n", participants[1]->GetName().c_str(), p2VB.Get(0), p2VB.Get(1), p2VB.Get(2));
      #endif

      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector downRRateVec = p2V - p1V /* - r12_j2k_vel*/;
#else
      Rvector downRRateVec = p2VB - p1VB /* - r12_j2k_vel*/;
#endif
      Rvector3 rangeUnit = downlinkVector.GetUnitVector();
      downlinkRangeRate = downRRateVec * rangeUnit;
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Downlink Range Rate:  %.12lf km/s\n",
               downlinkRangeRate);
      #endif

      
      //// 3. Get the transponder delay
      //targetDelay = GetDelay(1,0);
      //#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
      //   MessageInterface::ShowMessage(
      //         "    USN Transponder delay for %s = %.12lf s\n",
      //         participants[1]->GetName().c_str(), targetDelay);
      //#endif

      // 3. Get the range from the uplink
      Rvector3 r3, r4;                  // position of uplink leg's participants in central body MJ2000Eq coordinate system
      Rvector3 r3B, r4B;                // position of uplink leg's participants in solar system bary center MJ2000Eq coordinate system

      r3 = uplinkLeg.GetPosition(participants[0]);                                       // position of station at transmit time t1T in central body MJ2000Eq coordinate system
      r4 = uplinkLeg.GetPosition(participants[1]);                                       // position of spacecraft at reception time t2R in central body MJ2000Eq coordinate system
      t1T = uplinkLeg.GetEventData((GmatBase*) participants[0]).epoch;                   // transmit time at station for uplink leg
      t2R = uplinkLeg.GetEventData((GmatBase*) participants[1]).epoch;                   // reception time at spacecraft for uplink leg
     
      Rvector3 ssb2cb_t2R = cb2->GetMJ2000Position(t2R) - ssb->GetMJ2000Position(t2R);   // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t2R
      Rvector3 ssb2cb_t1T = cb1->GetMJ2000Position(t1T) - ssb->GetMJ2000Position(t1T);   // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t1T
      
      r3B = ssb2cb_t1T + r3;                                             // position of station at transmit time t1T in SSB coordinate system
      r4B = ssb2cb_t2R + r4;                                             // position of spacecraft at reception time t2R in SSB coordinate system


      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         Rmatrix33 mt1 = uplinkLeg.GetEventData((GmatBase*) participants[0]).rInertial2obj.Transpose();

         MessageInterface::ShowMessage("3. Get uplink leg range:\n");
         MessageInterface::ShowMessage("   Spacecraft %s position in %sMJ2000 coordinate system : r4 = (%.12lf, %.12lf, %.12lf)km   at epoch t2R = %.12lf\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), r4.Get(0), r4.Get(1), r4.Get(2), t2R);
         MessageInterface::ShowMessage("   Station %s position in %sMJ2000 coordinate system    : r3 = (%.12lf, %.12lf, %.12lf)km   at epoch t1T = %.12lf\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), r3.Get(0), r3.Get(1), r3.Get(2), t1T);
         MessageInterface::ShowMessage("   Spacecraft %s position in SSB coordinate system      : r4B = (%.12lf, %.12lf, %.12lf)km   at epoch t2R = %.12lf\n", participants[1]->GetName().c_str(), r4B.Get(0), r4B.Get(1), r4B.Get(2), t2R);
         MessageInterface::ShowMessage("   Station %s position in SSB coordinate system         : r3B = (%.12lf, %.12lf, %.12lf)km   at epoch t1T = %.12lf\n", participants[0]->GetName().c_str(), r3B.Get(0), r3B.Get(1), r3B.Get(2), t1T);

         MessageInterface::ShowMessage("   Transformation matrix from Earth fixed coordinate system to EarthFK5 coordinate system at epoch t1T = %.12lf:\n", t1T);
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt1(0,0), mt1(0,1), mt1(0,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt1(1,0), mt1(1,1), mt1(1,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt1(2,0), mt1(2,1), mt1(2,2));
      #endif


#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 uplinkVector = r4 - r3;
#else
      Rvector3 uplinkVector = r4B - r3B;
#endif
      uplinkRange = uplinkVector.GetMagnitude();

      // Calculate ET-TAI at t1T:
      Real ettaiT1 = downlinkLeg.ETminusTAI(t1T, (GmatBase*)participants[0]);

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         #ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("   Uplink range without relativity correction = r4-r3:  %.12lf km\n", uplinkRange);
         #else
         MessageInterface::ShowMessage("   Uplink range without relativity correction = r4B-r3B:  %.12lf km\n", uplinkRange);
         #endif
         MessageInterface::ShowMessage("   Relativity correction for uplink leg    = %.12lf km\n", uplinkLeg.GetRelativityCorrection());
         MessageInterface::ShowMessage("   Uplink range without relativity correction = %.12lf km\n", uplinkRange + uplinkLeg.GetRelativityCorrection());
         MessageInterface::ShowMessage("   (ET-TAI) at t1T = %.12le s\n", ettaiT1);
      #endif


      // 4. Calculate uplink range rate
      Rvector3 p3V = uplinkLeg.GetVelocity(participants[0]);
      Rvector3 p4V = uplinkLeg.GetVelocity(participants[1]);

      Rvector3 ssb2cbV_t2R = cb2->GetMJ2000Velocity(t2R) - ssb->GetMJ2000Velocity(t2R);      // velocity of central body at time t2R w.r.t SSB MJ2000Eq coordinate system
      Rvector3 ssb2cbV_t1T = cb1->GetMJ2000Velocity(t1T) - ssb->GetMJ2000Velocity(t1T);      // velocity of central body at time t1T w.r.t SSB MJ2000Eq coordinate system
      
      Rvector3 p3VB = ssb2cbV_t1T + p3V;                                       // velocity of station at reception time t1T in SSB coordinate system
      Rvector3 p4VB = ssb2cbV_t2R + p4V;                                       // velocity of spacecraft at transmit time t2R in SSB coordinate system

      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector upRRateVec = p4V - p3V /* - r12_j2k_vel*/ ;
#else
      Rvector upRRateVec = p4VB - p3VB /* - r12_j2k_vel*/ ;
#endif
      rangeUnit = uplinkVector.GetUnitVector();
      uplinkRangeRate = upRRateVec * rangeUnit;

      // 4.1. Target range rate: Do we need this as well?
      targetRangeRate = (downlinkRangeRate + uplinkRangeRate) / 2.0;

      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("4. Get uplink leg range rate:\n");
         MessageInterface::ShowMessage("   Station %s velocity in %sMJ2000 coordinate system    : p3V = (%.12lf, %.12lf, %.12lf)km/s\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), p3V.Get(0), p3V.Get(1), p3V.Get(2));
         MessageInterface::ShowMessage("   Spacecraft %s velocity in %sMJ2000 coordinate system : p4V = (%.12lf, %.12lf, %.12lf)km/s\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), p4V.Get(0), p4V.Get(1), p4V.Get(2));
         MessageInterface::ShowMessage("   Station %s velocity in SSB coordinate system         : p3VB = (%.12lf, %.12lf, %.12lf)km/s\n", participants[0]->GetName().c_str(), p3VB.Get(0), p3VB.Get(1), p3VB.Get(2));
         MessageInterface::ShowMessage("   Spacecraft %s velocity in SSB coordinate system      : p4VB = (%.12lf, %.12lf, %.12lf)km/s\n", participants[1]->GetName().c_str(), p4VB.Get(0), p4VB.Get(1), p4VB.Get(2));
         MessageInterface::ShowMessage("   Uplink Range Rate:  %.12lf km/s\n", uplinkRangeRate);
         MessageInterface::ShowMessage("   Target Range Rate:  %.12lf km/s\n", targetRangeRate);
         MessageInterface::ShowMessage("   Delay between transmiting signal and receiving signal at transponder:  t2T - t2R = %.12le s\n", (t2T-t2R)*86400);
      #endif
      
      // 5. Calculate ET-TAI correction
      Real ettaiCorrection = (useETminusTAICorrection?(ettaiT1 - ettaiT3):0.0);


      RealArray uplinkCorrection, downlinkCorrection;
      Real uplinkRangeCorrection, downlinkRangeCorrection, realRange;
      uplinkRangeCorrection = downlinkRangeCorrection = 0.0;
      for (int i= 0; i < 3; ++i)
      {
         uplinkCorrection.push_back(0.0);
         downlinkCorrection.push_back(0.0);
      }
      
      // 6. Get sensors used in USN 2-ways range
      ObjectArray objList1;
      ObjectArray objList2;
      ObjectArray objList3;
      //         objList1 := all transmitters in participantHardware list
      //         objList2 := all receivers in participantHardware list
      //         objList3 := all transponders in participantHardware list

      UpdateHardware();
      
      if (!(participantHardware.empty()||
             ((!participantHardware.empty())&&
              participantHardware[0].empty()&&
              participantHardware[1].empty()
             )
           )
         )
      {

         for(std::vector<Hardware*>::iterator hw = participantHardware[0].begin();
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

         for(std::vector<Hardware*>::iterator hw = participantHardware[1].begin();
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
            throw MeasurementException(((objList1.size() == 0)?"Error: The first participant does not have a transmitter to send signal.\n":"Error: The first participant has more than one transmitter.\n"));
         if (objList2.size() != 1)
            throw MeasurementException(((objList2.size() == 0)?"Error: The first participant does not have a receiver to receive signal.\n":"Error: The first participant has more than one receiver.\n"));
         if (objList3.size() != 1)
            throw MeasurementException((objList3.size() == 0)?"Error: The second participant does not have a transponder to transpond signal.\n":"Error: The second participant has more than one transponder.\n");

         Transmitter*    gsTransmitter    = (Transmitter*)objList1[0];
         Receiver*       gsReceiver       = (Receiver*)objList2[0];
         Transponder*    scTransponder    = (Transponder*)objList3[0];
         if (gsTransmitter == NULL)
            throw MeasurementException("Transmitter is NULL object.\n");
         if (gsReceiver == NULL)
            throw MeasurementException("Receiver is NULL object.\n");
         if (scTransponder == NULL)
            throw MeasurementException("Transponder is NULL object.\n");

         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("5. Sensors, delays, and signals:\n");
            MessageInterface::ShowMessage("   List of sensors: %s, %s, %s\n",
               gsTransmitter->GetName().c_str(), gsReceiver->GetName().c_str(),
               scTransponder->GetName().c_str());
         #endif


         // 7. Get transponder delays: (Note that: USN 2-way range only needs transponder delay for calculation)
         targetDelay = scTransponder->GetDelay();

         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("   Transponder delay = %le s\n", scTransponder->GetDelay());
         #endif
     

         // 8. Get frequency from transmitter of ground station (participants[0])
         Real uplinkFreq;                      // unit: MHz
         Real uplinkFreqAtRecei;               // unit: MHz         // uplink frequency at receive epoch
         if (obsData == NULL)
         {
            if (rampTB == NULL)
            {
               // Get uplink frequency from GMAT script when ramp table is not used
               Signal* uplinkSignal = gsTransmitter->GetSignal();
               uplinkFreq        = uplinkSignal->GetValue();                  // unit: MHz
               uplinkFreqAtRecei = uplinkFreq;                                // unit: MHz       // for constant frequency
               frequency         = uplinkFreq*1.0e6;                          // unit: Hz

               // Get uplink band based on definition of frequency range
               freqBand = FrequencyBand(frequency);

               // Range modulo constant is specified by GMAT script
               #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
                  MessageInterface::ShowMessage("   Uplink frequency is gotten from GMAT script...\n"); 
               #endif
            }
            else
            {
               // Get uplink frequency at a given time from ramped frequency table
               frequency         = GetFrequencyFromRampTable(t1T);            // unit: Hz      // Get frequency at transmit time t1T
               uplinkFreq        = frequency/1.0e6;                           // unit MHz
               uplinkFreqAtRecei = GetFrequencyFromRampTable(t3R) / 1.0e6;    // unit MHz      // for ramped frequency

               // Get frequency band from ramp table at given time
               freqBand = GetUplinkBandFromRampTable(t1T);

               // Range modulo constant is specified by GMAT script
               #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
                  MessageInterface::ShowMessage("   Uplink frequency is gotten from ramp table...: frequency = %.12le\n", frequency); 
               #endif
            }
         }
         else
         {
            if (rampTB == NULL)
            {
               //// Get uplink frequency at a given time from observation data
               //frequency = obsData->uplinkFreq;                       // unit: Hz   

               // Get uplink frequency from GMAT script when ramp table is not used
               Signal* uplinkSignal = gsTransmitter->GetSignal();
               uplinkFreq = uplinkSignal->GetValue();                  // unit: MHz
               uplinkFreqAtRecei = uplinkFreq;                                // unit: MHz       // for constant frequency
               frequency = uplinkFreq*1.0e6;                          // unit: Hz

               // Get uplink band based on definition of frequency range
               freqBand = FrequencyBand(frequency);

               // Range modulo constant is specified by GMAT script
               #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
                  MessageInterface::ShowMessage("   Uplink frequency is gotten from observation data...: frequency = %.12le\n", frequency); 
               #endif
            }
            else
            {
               //// Get uplink frequency at a given time from ramped frequency table
               //frequency = GetFrequencyFromRampTable(t1T);            // unit: Hz      // Get frequency at transmit time t1T

               // Get uplink frequency at a given time from ramped frequency table
               frequency         = GetFrequencyFromRampTable(t1T);            // unit: Hz      // Get frequency at transmit time t1T
               uplinkFreq = frequency / 1.0e6;                           // unit MHz
               uplinkFreqAtRecei = GetFrequencyFromRampTable(t3R) / 1.0e6;    // unit MHz      // for ramped frequency

               // Get frequency band from ramp table at given time
               freqBand = GetUplinkBandFromRampTable(t1T);

               // Range modulo constant is specified by GMAT script
               #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
                  MessageInterface::ShowMessage("   Uplink frequency is gotten from ramp table...: frequency = %.12le\n", frequency); 
               #endif
            }

            //uplinkFreq = frequency/1.0e6;                            // unit: MHz
            //freqBand = obsData->uplinkBand;
            obsValue = obsData->value;                               // unit: range unit

            #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
               MessageInterface::ShowMessage("   Uplink band, range modulo constant, and observation value are gotten from observation data...\n"); 
            #endif
         }

         // 9. Calculate media correction for uplink leg:
         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS   
            MessageInterface::ShowMessage("6. Media correction for uplink leg\n");
            MessageInterface::ShowMessage("   UpLink signal frequency = %.12lf MHz\n", uplinkFreq);
         #endif

         // In uplink leg, r3B and r4B are location of station and spacecraft in SSBMJ2000Eq coordinate system
         uplinkCorrection = CalculateMediaCorrection(uplinkFreq, r3B, r4B, t1T, t2R, minAngle);

         uplinkRangeCorrection = uplinkCorrection[0]*GmatMathConstants::M_TO_KM + uplinkLeg.GetRelativityCorrection();
         Real uplinkRealRange = uplinkRange + uplinkRangeCorrection;
         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("   Uplink media correction           = %.12lf m\n",uplinkCorrection[0]);
            MessageInterface::ShowMessage("   Uplink relativity correction      = %.12lf km\n",uplinkLeg.GetRelativityCorrection());
            MessageInterface::ShowMessage("   Uplink total range correction     = %.12lf km\n",uplinkRangeCorrection);
            MessageInterface::ShowMessage("   Uplink precision light time range = %.12lf km\n",uplinkRange);
            MessageInterface::ShowMessage("   Uplink real range                 = %.12lf km\n",uplinkRealRange);
         #endif


         // 10. Doppler shift the frequency from the transmitter using uplinkRangeRate:
         Real uplinkDSFreq = (1 - uplinkRangeRate*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*uplinkFreq;

         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("7. Transponder input and output frequencies\n");
            MessageInterface::ShowMessage("   Uplink Doppler shift frequency = %.12lf MHz\n", uplinkDSFreq);
         #endif

         // 11.Set frequency for the input signal of transponder
         Signal* inputSignal = scTransponder->GetSignal(0);
         inputSignal->SetValue(uplinkDSFreq);
         scTransponder->SetSignal(inputSignal, 0);

         // 12. Check the transponder feasibility to receive the input signal:
         if (scTransponder->IsFeasible(0) == false)
         {
            currentMeasurement.isFeasible = false;
            currentMeasurement.value[0] = 0;
            throw MeasurementException("The transponder is unfeasible to receive uplink signal.\n");
         }

         // 13. Get frequency of transponder output signal
         Signal* outputSignal = scTransponder->GetSignal(1);
         Real downlinkFreq = outputSignal->GetValue();

         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("    Downlink frequency = %.12lf Mhz\n", downlinkFreq);
         #endif

         // 14. Doppler shift the transponder output frequency:
         Real downlinkDSFreq = (1 - downlinkRangeRate*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*downlinkFreq;

         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("    Downlink Doppler shift frequency = %.12lf MHz\n", downlinkDSFreq);
         #endif

         // 15. Set frequency on receiver
         Signal* downlinkSignal = gsReceiver->GetSignal();
         downlinkSignal->SetValue(downlinkDSFreq);

         // 16. Check the receiver feasibility to receive the downlink signal
         if (gsReceiver->IsFeasible() == false)
         {
            currentMeasurement.isFeasible = false;
            currentMeasurement.value[0] = 0;
            throw MeasurementException("The receiver is unfeasible to receive downlink signal.\n");
         }

         // 17. Calculate media correction for downlink leg:
         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("8. Media correction for downlink leg\n");
         #endif
         // In down link leg, r1B and r2B are location of station and spacecraft in SSBMJ2000Eq coordinate system
         downlinkCorrection = CalculateMediaCorrection(downlinkDSFreq, r1B, r2B, t3R, t2T, minAngle);

         downlinkRangeCorrection = downlinkCorrection[0]*GmatMathConstants::M_TO_KM + downlinkLeg.GetRelativityCorrection();
         Real downlinkRealRange = downlinkRange + downlinkRangeCorrection;
         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("   Downlink media correction           = %.12lf m\n",downlinkCorrection[0]);
            MessageInterface::ShowMessage("   Downlink relativity correction      = %.12lf km\n",downlinkLeg.GetRelativityCorrection());
            MessageInterface::ShowMessage("   Downlink total range correction     = %.12lf km\n",downlinkRangeCorrection);
            MessageInterface::ShowMessage("   Downlink precision light time range = %.12lf km\n",downlinkRange);
            MessageInterface::ShowMessage("   Downlink real range                 = %.12lf km\n",downlinkRealRange);
         #endif

         // 18. Calculate real range
         realRange = uplinkRealRange + downlinkRealRange +
            (targetDelay + ettaiCorrection)*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M;


         currentMeasurement.uplinkFreq = uplinkFreq * 1.0e6;
         currentMeasurement.uplinkFreq = uplinkFreqAtRecei * 1.0e6;

      }
      else
      {
         #ifdef IONOSPHERE
            if ((troposphere != NULL)||(ionosphere != NULL))
         #else
            if (troposphere != NULL)
         #endif
               throw MeasurementException("Error: missing transmiter, transponder, or receiver in order to compute media correction\n");

         // No hardware useage in this case. It will have no media correction and hardware delay
         // light time correction, relativity correction, and ET-TAI correction is still added
         realRange = uplinkRange + downlinkRange + ettaiCorrection*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M;
      }


      //19. Verify uplink leg light path not to be blocked by station's central body
      UpdateRotationMatrix(t1T, "o_j2k");
      Rvector3 outState = (R_o_j2k * (r4B - r3B)).GetUnitVector();
      currentMeasurement.feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;      // elevation angle in degree
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("Uplink elevation angle = %.8lf     minAngle = %.8lf\n", currentMeasurement.feasibilityValue, minAngle);
      #endif

      if (currentMeasurement.feasibilityValue > minAngle)
      {
         UpdateRotationMatrix(t3R, "o_j2k");
         outState = (R_o_j2k * (r2B - r1B)).GetUnitVector();
         Real feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;                 // elevation angle in degree
         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("Downlink elevation angle = %.8lf\n", currentMeasurement.feasibilityValue);
         #endif

         if (feasibilityValue > minAngle)
         {
            currentMeasurement.unfeasibleReason = "N";
            currentMeasurement.isFeasible = true;
         }
         else
         {
            currentMeasurement.feasibilityValue = feasibilityValue;
            currentMeasurement.unfeasibleReason = "B2";
            currentMeasurement.isFeasible = false;
         }
      }
      else
      {
         currentMeasurement.unfeasibleReason = "B1";
         currentMeasurement.isFeasible = false;
      }


      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("5. Calculated half range:\n");
         MessageInterface::ShowMessage("   Media correction                    = %.12lf km\n", (uplinkCorrection[0] + downlinkCorrection[0])/2);
         MessageInterface::ShowMessage("   Relativity correction               = %.12lf km\n", (uplinkLeg.GetRelativityCorrection() + downlinkLeg.GetRelativityCorrection())/2);
         MessageInterface::ShowMessage("   Total range correction              = %.12lf km\n", (uplinkRangeCorrection + downlinkRangeCorrection)/2);
         MessageInterface::ShowMessage("   Downlink precision light time range = %.12lf km\n", (uplinkRange + downlinkRange)/2);
         MessageInterface::ShowMessage("   ET-TAI correction                   = %.12le km\n", (ettaiCorrection/2)*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M);
         MessageInterface::ShowMessage("   transponder delay correction        = %.12le km\n", (targetDelay/2)*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M);
         MessageInterface::ShowMessage("   Calculated real range               = %.12lf km\n", realRange/2);
      #endif

      // 20. Set value for currentMeasurement
      currentMeasurement.value[0] = realRange / 2.0;

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

      // 21. Add noise to current measurement if it is needed
      if (noiseSigma != NULL)
      {
         RandomNumber* rn = RandomNumber::Instance();
         Real val = rn->Gaussian(currentMeasurement.value[0], noiseSigma->GetElement(0));
         while (val <= 0.0)
            val = rn->Gaussian(currentMeasurement.value[0], noiseSigma->GetElement(0));
         currentMeasurement.value[0] = val;
      }

      retval = true;
   }

   return retval;
}
