//$Id: DSNTwoWayDoppler.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         DSNTwoWayDoppler
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


// this needs to be at the top for Ionosphere to work on Mac!
#include "RandomNumber.hpp"

#include "DSNTwoWayDoppler.hpp"
#include "MeasurementException.hpp"
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "GroundstationInterface.hpp"
#include "Transmitter.hpp"
#include "Receiver.hpp"
#include "Transponder.hpp"
#include <sstream>

//#define DEBUG_DOPPLER_CALC_WITH_EVENTS
//#define VIEW_PARTICIPANT_STATES_WITH_EVENTS
//#define DEBUG_DERIVATIVES
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

   if (obj == NULL)
      throw MeasurementException("Error: a NULL object inputs to DSNTwoWayDoppler::CalculateMeasurementDerivatives() function\n");
   

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


   RealArray oneRow;
   oneRow.assign(size, 0.0);
   currentDerivatives.clear();
   currentDerivatives.push_back(oneRow);

   if (objNumber == -1)
      return currentDerivatives;         // return zero vector when variable is independent of DSNTwoWay range

   Integer parameterID = GetParmIdFromEstID(id, obj);

   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("   Looking up id %d\n", parameterID);
   #endif

   // At this point, objPtr points to obj. Therefore, its value is not NULL
   if (objPtr != NULL)
   {
      Real preFactorS = turnaround * frequency / (interval * GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M);
      Real preFactorE = turnaround * frequencyE / (interval * GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M);

      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("preFactorE = %.12lf     prefactorS = %.12lf\n", preFactorE, preFactorS);
      #endif
      
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

            // Uplink leg
            Rvector3 uplinkDerivS, uplinkDerivE;
            GetRangeDerivative(uplinkLegS, stmInv, uplinkDerivS, false, 0, 1, true, false);            // for uplink leg, transmiter is station (index = 0), receiver is spacecraft (index = 1), derivative w.r.t transmiter object is false (w.r.t spacecraft)
            GetRangeDerivative(uplinkLegE, stmInv, uplinkDerivE, false, 0, 1, true, false);            // partial derivative w.r.t R but V

            // Downlink leg
            Rvector3 downlinkDerivS, downlinkDerivE;
            GetRangeDerivative(downlinkLegS, stmInv, downlinkDerivS, true, 1, 0, true, false);         // for downlink leg, transmiter is spacecarft (index = 1), receiver is station (index = 0), derivative w.r.t transmiter object is true (w.r.t spacecraft)
            GetRangeDerivative(downlinkLegE, stmInv, downlinkDerivE, true, 1, 0, true, false);         // partial derivative w.r.t R but V

            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("uplinkLegS derivative   : %s\n",uplinkDerivS.ToString().c_str());
               MessageInterface::ShowMessage("downlinkLegS derivative : %s\n\n",downlinkDerivS.ToString().c_str());
               MessageInterface::ShowMessage("uplinkLegE derivative   : %s\n",uplinkDerivE.ToString().c_str());
               MessageInterface::ShowMessage("downlinkLegE derivative : %s\n\n",downlinkDerivE.ToString().c_str());
            #endif

            for (Integer i = 0; i < size; ++i)
            {
               currentDerivatives[0][i] = preFactorE * (uplinkDerivE[i] + downlinkDerivE[i]) -
                                          preFactorS * (uplinkDerivS[i] + downlinkDerivS[i]);
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
            GetRangeDerivative(uplinkLegS, stmInv, uplinkDerivS, false, 0, 1, false, true);            // for uplink leg, transmiter is station (index = 0), receiver is spacecraft (index = 1), derivative w.r.t transmiter object is false (w.r.t spacecraft)
            GetRangeDerivative(uplinkLegE, stmInv, uplinkDerivE, false, 0, 1, false, true);            // partial derivative w.r.t V but R

            // Downlink leg
            Rvector3 downlinkDerivS, downlinkDerivE;
            GetRangeDerivative(downlinkLegS, stmInv, downlinkDerivS, true, 1, 0, false, true);         // for downlink leg, transmiter is spacecarft (index = 1), receiver is station (index = 0), derivative w.r.t transmiter object is true (w.r.t spacecraft)
            GetRangeDerivative(downlinkLegE, stmInv, downlinkDerivE, true, 1, 0, false, true);         // partial derivative w.r.t V but R

            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("uplinkLegS derivative   : %s\n",uplinkDerivS.ToString().c_str());
               MessageInterface::ShowMessage("downlinkLegS derivative : %s\n\n",downlinkDerivS.ToString().c_str());
               MessageInterface::ShowMessage("uplinkLegE derivative   : %s\n",uplinkDerivE.ToString().c_str());
               MessageInterface::ShowMessage("downlinkLegE derivative : %s\n\n",downlinkDerivE.ToString().c_str());
            #endif

            for (Integer i = 0; i < size; ++i)
            {
               currentDerivatives[0][i] = preFactorE * (uplinkDerivE[i] + downlinkDerivE[i]) -
                                          preFactorS * (uplinkDerivS[i] + downlinkDerivS[i]);
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
//          GetRangeDerivative(uplinkLegS, stmInv, uplinkDerivS, false, 0, 1, true, true);            // for uplink leg, transmiter is station (index = 0), receiver is spacecraft (index = 1), derivative w.r.t transmiter object is false (w.r.t spacecraft)
//          GetRangeDerivative(uplinkLegE, stmInv, uplinkDerivE, false, 0, 1, true, true);            // partial derivative w.r.t R and V
            GetRangeDerivative(uplinkLegS, stmInv, uplinkDerivS, false);            // for uplink leg, transmiter is station (index = 0), receiver is spacecraft (index = 1), derivative w.r.t transmiter object is false (w.r.t spacecraft)
            GetRangeDerivative(uplinkLegE, stmInv, uplinkDerivE, false);            // partial derivative w.r.t R and V

            // Downlink leg
            Rvector6 downlinkDerivS, downlinkDerivE;
//          GetRangeDerivative(downlinkLegS, stmInv, downlinkDerivS, true, 1, 0, true, true);         // for downlink leg, transmiter is spacecarft (index = 1), receiver is station (index = 0), derivative w.r.t transmiter object is true (w.r.t spacecraft)
//          GetRangeDerivative(downlinkLegE, stmInv, downlinkDerivE, true, 1, 0, true, true);         // partial derivative w.r.t R and V
            GetRangeDerivative(downlinkLegS, stmInv, downlinkDerivS, false);         // for downlink leg, transmiter is spacecarft (index = 1), receiver is station (index = 0), derivative w.r.t transmiter object is true (w.r.t spacecraft)
            GetRangeDerivative(downlinkLegE, stmInv, downlinkDerivE, false);         // partial derivative w.r.t R and V

            #ifdef DEBUG_DERIVATIVES
               MessageInterface::ShowMessage("uplinkLegS derivative   : %s\n",uplinkDerivS.ToString().c_str());
               MessageInterface::ShowMessage("downlinkLegS derivative : %s\n\n",downlinkDerivS.ToString().c_str());
               MessageInterface::ShowMessage("uplinkLegE derivative   : %s\n",uplinkDerivE.ToString().c_str());
               MessageInterface::ShowMessage("downlinkLegE derivative : %s\n\n",downlinkDerivE.ToString().c_str());
            #endif

            for (Integer i = 0; i < size; ++i)
            {
               currentDerivatives[0][i] = preFactorE * (uplinkDerivE[i] + downlinkDerivE[i]) -
                                          preFactorS * (uplinkDerivS[i] + downlinkDerivS[i]);
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
//#define USE_EARTHMJ2000EQ_CS
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

   // Get minimum elevation angle from ground station
   Real minAngle;
   if (participants[0]->IsOfType(Gmat::SPACECRAFT) == false)
      minAngle = ((GroundstationInterface*)participants[0])->GetRealParameter("MinimumElevationAngle");
   else if (participants[1]->IsOfType(Gmat::SPACECRAFT) == false)
      minAngle = ((GroundstationInterface*)participants[1])->GetRealParameter("MinimumElevationAngle");


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
//    outState = R_o_j2k * rangeVecInertial;
//    currentMeasurement.feasibilityValue = outState[2];
      outState = (R_o_j2k * rangeVecInertial).GetUnitVector();
      currentMeasurement.feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;      // elevation angle in degree

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

//      if (currentMeasurement.feasibilityValue > minAngle)
      {
         currentMeasurement.isFeasible = true;
         currentMeasurement.value[0] = 2*rangeVecInertial.GetMagnitude();     // It is set to range value
         currentMeasurement.eventCount = 4;

         retval = true;
      }
//      else
//      {
//         currentMeasurement.isFeasible = false;
//         currentMeasurement.value[0] = 0.0;
//         currentMeasurement.eventCount = 0;
//      }

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
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("DSN 2-Way Doppler Calculation:\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES_WITH_EVENTS
         DumpParticipantStates("********************************************\n"
               "Evaluating DSN 2-Way Doppler with located events");
      #endif

      SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
      std::string cbName1 = ((SpacePoint*)participants[0])->GetJ2000BodyName();
      CelestialBody* cb1 = solarSystem->GetBody(cbName1);
      std::string cbName2 = ((SpacePoint*)participants[1])->GetJ2000BodyName();
      CelestialBody* cb2 = solarSystem->GetBody(cbName2);


      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         //MessageInterface::ShowMessage("For Measurement S:\n");
         MessageInterface::ShowMessage("1. Range and range rate for measurement S:\n");
      #endif
      // 1. Get the range from the down link leg for measurement S:
      Rvector3 r1S, r2S;                    // position of downlink leg's participants in central body MJ2000Eq coordinate system 
      Rvector3 r1SB, r2SB;                  // position of downlink leg's participants in solar system bary center MJ2000Eq coordinate system

      Real t3RS, t2TS;                  
      r1S = downlinkLegS.GetPosition(participants[0]);                     // position of station at reception time t3R in central body MJ2000Eq coordinate system
      r2S = downlinkLegS.GetPosition(participants[1]);                     // position of spacecraft at transmit time t2T in central body MJ2000Eq coordinate system
      t3RS = downlinkLegS.GetEventData((GmatBase*) participants[0]).epoch;      // reception time t3R at station for downlink leg
      t2TS = downlinkLegS.GetEventData((GmatBase*) participants[1]).epoch;      // transmit time t2T at spacecraft for downlink leg

      Rvector3 ssb2cb_t3RS = cb1->GetMJ2000Position(t3RS) - ssb->GetMJ2000Position(t3RS);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t3R
      Rvector3 ssb2cb_t2TS = cb2->GetMJ2000Position(t2TS) - ssb->GetMJ2000Position(t2TS);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t2T
      
      r1SB = ssb2cb_t3RS + r1S;                                           // position of station at reception time t3R in SSB coordinate system
      r2SB = ssb2cb_t2TS + r2S;                                           // position of spacecraft at transmit time t2T in SSB coordinate system

# ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 downlinkVectorS = r2S - r1S;                               // down link vector in EarthMJ2000Eq coordinate system
#else
      Rvector3 downlinkVectorS = r2SB - r1SB;                             // down link vector in SSB coordinate system
#endif

      Real downlinkRangeS = downlinkVectorS.GetMagnitude();               // down link range (unit: km)

      // Calculate ET-TAI at t3RS:
      Real ettaiT3S = downlinkLegS.ETminusTAI(t3RS, (GmatBase*)participants[0]);

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         Rmatrix33 mtS = downlinkLegS.GetEventData((GmatBase*) participants[0]).rInertial2obj.Transpose();

         MessageInterface::ShowMessage("1.1. Downlink leg range for measurement S:\n");
         MessageInterface::ShowMessage("     Station %s position in %sMJ2000 coordinate system       :   r1S  = (%.12lf,  %.12lf,  %.12lf) km   at epoch t3R = %.12lf\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), r1S(0), r1S(1), r1S(2), t3RS);  
         MessageInterface::ShowMessage("     Spacecraft %s position in %sMJ2000 coordinate system    :   r2S  = (%.12lf,  %.12lf,  %.12lf) km   at epoch t2T = %.12lf\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), r2S(0), r2S(1), r2S(2), t2TS);
         MessageInterface::ShowMessage("     Station %s position in SSB inertial coordinate system   :   r1SB = (%.12lf,  %.12lf,  %.12lf) km   at epoch t3R = %.12lf\n", participants[0]->GetName().c_str(), r1SB(0), r1SB(1), r1SB(2), t3RS);  
         MessageInterface::ShowMessage("     Spacecraft %s position in SSB inertial coordinate system:   r2SB = (%.12lf,  %.12lf,  %.12lf) km   at epoch t2T = %.12lf\n", participants[1]->GetName().c_str(), r2SB(0), r2SB(1), r2SB(2), t2TS);
         MessageInterface::ShowMessage("     Transformation matrix from Earth fixed coordinate system to FK5 coordinate system at epoch t3R = %.12lf:\n", t3RS);
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtS(0,0), mtS(0,1), mtS(0,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtS(1,0), mtS(1,1), mtS(1,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtS(2,0), mtS(2,1), mtS(2,2));
#ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("     Downlink vector in EarthMJ2000Eq coordinate system:   downlinkVectorS = r2S - r1S = (%.12lf,  %.12lf,  %.12lf) km\n", downlinkVectorS(0), downlinkVectorS(1), downlinkVectorS(2));
#else
         MessageInterface::ShowMessage("     Downlink vector in SSB inertial coordinate system:   downlinkVectorS = r2SB - r1SB = (%.12lf,  %.12lf,  %.12lf) km\n", downlinkVectorS(0), downlinkVectorS(1), downlinkVectorS(2));
#endif
         MessageInterface::ShowMessage("     Downlink range without relativity correction: %.12lf km\n",downlinkRangeS);
         MessageInterface::ShowMessage("     Relativity correction for downlink leg      : %.12lf km\n", downlinkLegS.GetRelativityCorrection());
         MessageInterface::ShowMessage("     Downlink range with relativity correction   : %.12lf km\n", downlinkLegS.GetRelativityCorrection() + downlinkRangeS);
         MessageInterface::ShowMessage("     (ET-TAI) at t3RS = %.12le s\n", ettaiT3S);
      #endif

      // 2. Calculate down link range rate for measurement S:
      Rvector3 p1VS = downlinkLegS.GetVelocity(participants[0]);               // velocity of station at reception time t3R in central body MJ2000 coordinate system
      Rvector3 p2VS = downlinkLegS.GetVelocity(participants[1]);               // velocity of spacecraft at transmit time t2T in central body MJ2000 coordinate system

      Rvector3 ssb2cbV_t3RS = cb1->GetMJ2000Velocity(t3RS) - ssb->GetMJ2000Velocity(t3RS);      // velocity of central body at time t3R w.r.t SSB MJ2000Eq coordinate system
      Rvector3 ssb2cbV_t2TS = cb2->GetMJ2000Velocity(t2TS) - ssb->GetMJ2000Velocity(t2TS);      // velocity of central body at time t2T w.r.t SSB MJ2000Eq coordinate system
      
      Rvector3 p1VSB = ssb2cbV_t3RS + p1VS;                                   // velocity of station at reception time t3R in SSB coordinate system
      Rvector3 p2VSB = ssb2cbV_t2TS + p2VS;                                   // velocity of spacecraft at transmit time t2T in SSB coordinate system

      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 downRRateVecS = p2VS - p1VS;
#else
      Rvector3 downRRateVecS = p2VSB - p1VSB /* - r12_j2k_vel*/;
#endif
      Rvector3 rangeUnit = downlinkVectorS.GetUnitVector();
      downlinkRangeRate[0] = downRRateVecS * rangeUnit;

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("1.2. Downlink leg range rate for measurement S:\n");
         MessageInterface::ShowMessage("     Station %s velocity in %sMJ2000 coordinate system        :  p1VS  = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), p1VS(0), p1VS(1), p1VS(2));
         MessageInterface::ShowMessage("     Spacecraft %s velocity in %sMJ2000 coordinate system     :  p2VS  = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), p2VS(0), p2VS(1), p2VS(2));
         MessageInterface::ShowMessage("     Station %s velocity in SSB inertial coordinate system    :  p1VSB = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[0]->GetName().c_str(), p1VSB(0), p1VSB(1), p1VSB(2));
         MessageInterface::ShowMessage("     Spacecraft %s velocity in SSB inertial coordinate system :  p2VSB = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[1]->GetName().c_str(), p2VSB(0), p2VSB(1), p2VSB(2));
#ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("     Spacecraft velocity w/r/t Station in EarthMJ2000Eq coordinate system:  downRRateVecS = (%.12lf,  %.12lf,  %.12lf)km/s\n", downRRateVecS(0), downRRateVecS(1), downRRateVecS(2));
#else
         MessageInterface::ShowMessage("     Spacecraft velocity w/r/t Station in SSB inertial coordinate system:  downRRateVecS = (%.12lf,  %.12lf,  %.12lf)km/s\n", downRRateVecS(0), downRRateVecS(1), downRRateVecS(2));
#endif
         MessageInterface::ShowMessage("     Downlink range Rate:  downlinkRangeRateS = %.12lf km/s\n", downlinkRangeRate[0]);

         //MessageInterface::ShowMessage("  .Downlink Range Rate:  %.12lf km/s\n", downlinkRangeRate[0]);
      #endif

      // 3. Get the range from the up link leg for measurement S:
      Rvector3 r3S, r4S;                       // position of uplink leg's participants in central body MJ2000Eq coordinate system 
      Rvector3 r3SB, r4SB;                     // position of uplink leg's participants in Solar system bary center inertial coordinate system 

      Real t1TS, t2RS;
      r3S = uplinkLegS.GetPosition(participants[0]);                  // position of station at transmit time t1T in central body MJ2000Eq coordinate system
      r4S = uplinkLegS.GetPosition(participants[1]);                  // position of spacecraft at reception time t2R in central body MJ2000Eq coordinate system
      t1TS = uplinkLegS.GetEventData((GmatBase*) participants[0]).epoch;   // transmit time t1T at station for uplink leg
      t2RS = uplinkLegS.GetEventData((GmatBase*) participants[1]).epoch;   // reception time t2R at spacecraft for uplink leg

      Rvector3 ssb2cb_t1TS = cb1->GetMJ2000Position(t1TS) - ssb->GetMJ2000Position(t1TS);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t1T
      Rvector3 ssb2cb_t2RS = cb2->GetMJ2000Position(t2RS) - ssb->GetMJ2000Position(t2RS);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t2R
      
      r3SB = ssb2cb_t1TS + r3S;                                       // position of station at transmit time t1T in SSB coordinate system
      r4SB = ssb2cb_t2TS + r4S;                                       // position of spacecraft at reception time t2R in SSB coordinate system

#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 uplinkVectorS = r4S - r3S;
#else
      Rvector3 uplinkVectorS = r4SB - r3SB;
#endif
      Real uplinkRangeS = uplinkVectorS.GetMagnitude();
     
      // Calculate ET-TAI at t1TS:
      Real ettaiT1S = uplinkLegS.ETminusTAI(t1TS, (GmatBase*)participants[0]);

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         Rmatrix33 mtS1 = uplinkLegS.GetEventData((GmatBase*) participants[0]).rInertial2obj.Transpose();

         MessageInterface::ShowMessage("1.3. Uplink leg range for measurement S:\n");
         MessageInterface::ShowMessage("     Spacecraft %s position in %sMJ2000 coordinate system    :   r4S  = (%.12lf,  %.12lf,  %.12lf) km   at epoch t2RS = %.12lf\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), r4S(0), r4S(1), r4S(2), t2RS);
         MessageInterface::ShowMessage("     Station %s position in %sMJ2000 coordinate system       :   r3S  = (%.12lf,  %.12lf,  %.12lf) km   at epoch t1TS = %.12lf\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), r3S(0), r3S(1), r3S(2), t1TS);  
         MessageInterface::ShowMessage("     Spacecraft %s position in SSB inertial coordinate system:   r4SB = (%.12lf,  %.12lf,  %.12lf) km   at epoch t2RS = %.12lf\n", participants[1]->GetName().c_str(), r4SB(0), r4SB(1), r4SB(2), t2RS);
         MessageInterface::ShowMessage("     Station %s position in SSB inertial coordinate system   :   r3SB = (%.12lf,  %.12lf,  %.12lf) km   at epoch t1TS = %.12lf\n", participants[0]->GetName().c_str(), r3SB(0), r3SB(1), r3SB(2), t1TS);  
         MessageInterface::ShowMessage("     Transformation matrix from Earth fixed coordinate system to FK5 coordinate system at epoch tS3 = %.12lf:\n", t1TS);
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtS1(0,0), mtS1(0,1), mtS1(0,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtS1(1,0), mtS1(1,1), mtS1(1,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtS1(2,0), mtS1(2,1), mtS1(2,2));
#ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("     Uplink vector in EarthMJ2000Eq coordinate system:   uplinkVectorS = r4S - r3S = (%.12lf,  %.12lf,  %.12lf) km\n", uplinkVectorS(0), uplinkVectorS(1), uplinkVectorS(2));
#else
         MessageInterface::ShowMessage("     Uplink vector in SSB inertial coordinate system:   uplinkVectorS = r4SB - r3SB = (%.12lf,  %.12lf,  %.12lf) km\n", uplinkVectorS(0), uplinkVectorS(1), uplinkVectorS(2));
#endif
         MessageInterface::ShowMessage("     Uplink range without ralativity correction: %.12lf km\n", uplinkRangeS);
         MessageInterface::ShowMessage("     Relativity correction for uplink leg      : %.12lf km\n", uplinkLegS.GetRelativityCorrection());
         MessageInterface::ShowMessage("     Uplink range with relativity correction   : %.12lf km\n", uplinkLegS.GetRelativityCorrection() + uplinkRangeS);
         MessageInterface::ShowMessage("     (ET-TAI) at t1TS = %.12le s\n", ettaiT1S);
      #endif

      // 4. Calculate up link range rate for measurement S:
      Rvector3 p3VS = uplinkLegS.GetVelocity(participants[0]);               // velocity of station at transmit time t1T in central body MJ2000 coordinate system
      Rvector3 p4VS = uplinkLegS.GetVelocity(participants[1]);               // velocity of specraft at reception time t2R in central body MJ2000 coordinate system

      Rvector3 ssb2cbV_t1TS = cb1->GetMJ2000Velocity(t1TS) - ssb->GetMJ2000Velocity(t1TS);      // velocity of central body at time t1T w.r.t SSB inertial coordinate system
      Rvector3 ssb2cbV_t2RS = cb2->GetMJ2000Velocity(t2RS) - ssb->GetMJ2000Velocity(t2RS);      // velocity of central body at time t2R w.r.t SSB inertial coordinate system
      
      Rvector3 p3VSB = ssb2cbV_t1TS + p3VS;                                  // velocity of station at transmit time t1T in SSB inertial coordinate system
      Rvector3 p4VSB = ssb2cbV_t2RS + p4VS;                                  // velocity of spacecraft at reception time t2R in SSB inertial coordinate system

      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 upRRateVecS = p4VS - p3VS;
#else
      Rvector3 upRRateVecS = p4VSB - p3VSB /* - r12_j2k_vel*/ ;
#endif
      rangeUnit = uplinkVectorS.GetUnitVector();
      uplinkRangeRate[0] = upRRateVecS * rangeUnit;

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("1.4. Uplink leg range rate for measurement S:\n");
         MessageInterface::ShowMessage("     Station %s velocity in %sMJ2000 coordinate system       :  p3VS  = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), p3VS(0), p3VS(1), p3VS(2));
         MessageInterface::ShowMessage("     Spacecraft %s velocity in %sMJ2000 coordinate system    :  p4VS  = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), p4VS(0), p4VS(1), p4VS(2));
         MessageInterface::ShowMessage("     Station %s velocity in SSB inertial coordinate system   :  p3VSB = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[0]->GetName().c_str(), p3VSB(0), p3VSB(1), p3VSB(2));
         MessageInterface::ShowMessage("     Spacecraft %s velocity in SSB inertial coordinate system:  p4VSB = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[1]->GetName().c_str(), p4VSB(0), p4VSB(1), p4VSB(2));
#ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("     Spacecraft velocity w/r/t Station in EarthMJ2000Eq coordinate system:  upRRateVecS = (%.12lf,  %.12lf,  %.12lf)km/s\n", upRRateVecS(0), upRRateVecS(1), upRRateVecS(2));
#else
         MessageInterface::ShowMessage("     Spacecraft velocity w/r/t Station in SSB inertial coordinate system:  upRRateVecS = (%.12lf,  %.12lf,  %.12lf)km/s\n", upRRateVecS(0), upRRateVecS(1), upRRateVecS(2));
#endif
         MessageInterface::ShowMessage("     Uplink range rate:  uplinkRangeRateS = %.12lf km/s\n", uplinkRangeRate[0]);
      #endif


      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("1.5. Travel time and delays for measurement S:\n");
         MessageInterface::ShowMessage("     Uplink travel time  w/o relativity, media corrections : %.12lf s\n", uplinkRangeS*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);
         MessageInterface::ShowMessage("     Downlink travel time w/o relativity, media corrections: %.12lf s\n", downlinkRangeS*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);
         MessageInterface::ShowMessage("     Delay between receiving and transmitting signal on participant 2: %.12lf s\n", (t2TS - t2RS)*86400);
      #endif


      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("2. Range and range rate for measurement E:\n");
         //MessageInterface::ShowMessage("For Measurement E:\n");
      #endif
      // 5. Get the range from the down link leg for measurement E:
      Rvector3 r1E, r2E;                             // position of downlink leg's participants in central body MJ2000Eq coordinate system 
      Rvector3 r1EB, r2EB;                           // position of downlink leg's participants in Solar system bary center inertial coordinate system 

      Real t3RE, t2TE;
      r1E = downlinkLegE.GetPosition(participants[0]);                     // position of station at reception time t3R in central body MJ2000Eq coordinate system
      r2E = downlinkLegE.GetPosition(participants[1]);                     // position of spacecraft at reception time t2T in central body MJ2000Eq coordinate system
      t3RE = downlinkLegE.GetEventData((GmatBase*) participants[0]).epoch;      // reception time t3R at station for downlink leg
      t2TE = downlinkLegE.GetEventData((GmatBase*) participants[1]).epoch;      // transmit time t2T at spacecraft for downlink leg

      Rvector3 ssb2cb_t3RE = cb1->GetMJ2000Position(t3RE) - ssb->GetMJ2000Position(t3RE);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t3R
      Rvector3 ssb2cb_t2TE = cb2->GetMJ2000Position(t2TE) - ssb->GetMJ2000Position(t2TE);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t2T
      
      r1EB = ssb2cb_t3RE + r1E;                                           // position of station at reception time t3R in SSB coordinate system
      r2EB = ssb2cb_t2TE + r2E;                                           // position of spacecraft at transmit time t2T in SSB coordinate system

#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 downlinkVectorE = r2E - r1E;
#else
      Rvector3 downlinkVectorE = r2EB - r1EB;                             // down link vector in SSB coordinate system
#endif
      Real downlinkRangeE = downlinkVectorE.GetMagnitude();               // down link range (unit: km)

      // Calculate ET-TAI at t3RE:
      Real ettaiT3E = downlinkLegE.ETminusTAI(t3RE, (GmatBase*)participants[0]);

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         Rmatrix33 mtE = downlinkLegE.GetEventData((GmatBase*) participants[0]).rInertial2obj.Transpose();

         MessageInterface::ShowMessage("2.1. Downlink leg range for measurement E:\n");
         MessageInterface::ShowMessage("     Station %s position in %sMJ2000 coordinate system       :   r1E  = (%.12lf,  %.12lf,  %.12lf) km   at epoch t3RE= %.12lf\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), r1E(0), r1E(1), r1E(2), t3RE);  
         MessageInterface::ShowMessage("     Spacecraft %s position in %sMJ2000 coordinate system    :   r2E  = (%.12lf,  %.12lf,  %.12lf) km   at epoch t2TE= %.12lf\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), r2E(0), r2E(1), r2E(2), t2TE);
         MessageInterface::ShowMessage("     Station %s position in SSB inertial coordinate system   :   r1EB = (%.12lf,  %.12lf,  %.12lf) km   at epoch t3RE= %.12lf\n", participants[0]->GetName().c_str(), r1EB(0), r1EB(1), r1EB(2), t3RE);  
         MessageInterface::ShowMessage("     Spacecraft %s position in SSB inertial coordinate system:   r2EB = (%.12lf,  %.12lf,  %.12lf) km   at epoch t2TE= %.12lf\n", participants[1]->GetName().c_str(), r2EB(0), r2EB(1), r2EB(2), t2TE);
         MessageInterface::ShowMessage("     Transformation matrix from Earth fixed coordinate system to FK5 coordinate system at epoch t3RE = %.12lf:\n", t3RE);
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtE(0,0), mtE(0,1), mtE(0,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtE(1,0), mtE(1,1), mtE(1,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtE(2,0), mtE(2,1), mtE(2,2));
#ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("     Downlink vector in EarthMJ2000Eq coordinate system:   downlinkVectorE = r2E - r1E = (%.12lf,  %.12lf,  %.12lf) km\n", downlinkVectorE(0), downlinkVectorE(1), downlinkVectorE(2));
#else
         MessageInterface::ShowMessage("     Downlink vector in SSB inertial coordinate system:   downlinkVectorE = r2EB - r1EB = (%.12lf,  %.12lf,  %.12lf) km\n", downlinkVectorE(0), downlinkVectorE(1), downlinkVectorE(2));
#endif
         MessageInterface::ShowMessage("     Downlink range without relativity correction: %.12lf km\n",downlinkRangeE);
         MessageInterface::ShowMessage("     Relativity correction for downlink leg      : %.12lf km\n", downlinkLegE.GetRelativityCorrection());
         MessageInterface::ShowMessage("     Downlink range with relativity correction   : %.12lf km\n", downlinkLegE.GetRelativityCorrection() + downlinkRangeE);
         MessageInterface::ShowMessage("     (ET-TAI) at t3RE = %.12le s\n", ettaiT3E);
      #endif

      // 6. Calculate down link range rate for measurement E:
      Rvector3 p1VE = downlinkLegE.GetVelocity(participants[0]);               // velocity of station at reception time t3R in central body MJ2000 coordinate system
      Rvector3 p2VE = downlinkLegE.GetVelocity(participants[1]);               // velocity of spacecraft at transmit time t2T in central body MJ2000 coordinate system

      Rvector3 ssb2cbV_t3RE = cb1->GetMJ2000Velocity(t3RE) - ssb->GetMJ2000Velocity(t3RE);      // velocity of central body at time t3R w.r.t SSB MJ2000Eq coordinate system
      Rvector3 ssb2cbV_t2TE = cb2->GetMJ2000Velocity(t2TE) - ssb->GetMJ2000Velocity(t2TE);      // velocity of central body at time t2T w.r.t SSB MJ2000Eq coordinate system
      
      Rvector3 p1VEB = ssb2cbV_t3RE + p1VE;                                    // velocity of station at reception time t3R in SSB coordinate system
      Rvector3 p2VEB = ssb2cbV_t2TE + p2VE;                                    // velocity of spacecraft at transmit time t2T in SSB coordinate system


      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 downRRateVecE = p2VE - p1VE;
#else
      Rvector3 downRRateVecE = p2VEB - p1VEB /* - r12_j2k_vel*/;
#endif
      rangeUnit = downlinkVectorE.GetUnitVector();
      downlinkRangeRate[1] = downRRateVecE * rangeUnit;

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("2.2. Downlink leg range rate for measurement E:\n");
         MessageInterface::ShowMessage("     Station %s velocity in %sMJ2000 coordiante system       :  p1VE  = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), p1VE(0), p1VE(1), p1VE(2));
         MessageInterface::ShowMessage("     Spacecraft %s velocity in %sMJ2000 coordiante system    :  p2VE  = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), p2VE(0), p2VE(1), p2VE(2));
         MessageInterface::ShowMessage("     Station %s velocity in SSB inertial coordiante system   :  p1VEB = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[0]->GetName().c_str(), p1VEB(0), p1VEB(1), p1VEB(2));
         MessageInterface::ShowMessage("     Spacecraft %s velocity in SSB inertial coordiante system:  p2VEB = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[1]->GetName().c_str(), p2VEB(0), p2VEB(1), p2VEB(2));
#ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("     Spacecraft velocity w/r/t Station in EarthMJ2000Eq coordinate system:  downRRateVecE = (%.12lf,  %.12lf,  %.12lf)km/s\n", downRRateVecE(0), downRRateVecE(1), downRRateVecE(2));
#else
         MessageInterface::ShowMessage("     Spacecraft velocity w/r/t Station in SSB inertial coordinate system:  downRRateVecE = (%.12lf,  %.12lf,  %.12lf)km/s\n", downRRateVecE(0), downRRateVecE(1), downRRateVecE(2));
#endif
         MessageInterface::ShowMessage("     Downlink range Rate:  downlinkRangeRateE = %.12lf km/s\n", downlinkRangeRate[1]);
      #endif

      // 7. Get the range from the up link leg for measurement E:
      Rvector3 r3E, r4E;                             // position of uplink leg's participants in central body MJ2000Eq coordinate system 
      Rvector3 r3EB, r4EB;                           // position of uplink leg's participants in Solar system bary center inertial coordinate system

      Real t1TE, t2RE;
      r3E = uplinkLegE.GetPosition(participants[0]);                  // position of station at transmit time t1T in central body MJ2000Eq coordinate system
      r4E = uplinkLegE.GetPosition(participants[1]);                  // position of spacecraft at reception time t2R in central body MJ2000Eq coordinate system
      t1TE = uplinkLegE.GetEventData((GmatBase*) participants[0]).epoch;   // transmit time t1T at station for uplink leg
      t2RE = uplinkLegE.GetEventData((GmatBase*) participants[1]).epoch;   // reception time t2R at spacecraft for uplink leg

      Rvector3 ssb2cb_t1TE = cb1->GetMJ2000Position(t1TE) - ssb->GetMJ2000Position(t1TE);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t1T
      Rvector3 ssb2cb_t2RE = cb2->GetMJ2000Position(t2RE) - ssb->GetMJ2000Position(t2RE);      // vector from solar system bary center to central body in SSB MJ2000Eq coordinate system at time t2R
      
      r3EB = ssb2cb_t1TE + r3E;                                       // position of station at transmit time t1T in SSB inertial coordinate system
      r4EB = ssb2cb_t2TE + r4E;                                       // position of spacecraft at reception time t2R in SSB inertial coordinate system

#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 uplinkVectorE = r4E - r3E;
#else
      Rvector3 uplinkVectorE = r4EB - r3EB;
#endif
      Real uplinkRangeE = uplinkVectorE.GetMagnitude();
     
      // Calculate ET-TAI at t1TE:
      Real ettaiT1E = uplinkLegE.ETminusTAI(t1TE, (GmatBase*)participants[0]);

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         Rmatrix33 mtE1 = uplinkLegE.GetEventData((GmatBase*) participants[0]).rInertial2obj.Transpose();

         MessageInterface::ShowMessage("2.3. Uplink leg range for measurement E:\n");
         MessageInterface::ShowMessage("     Spacecraft %s position in %sMJ2000 coordinate system    :   r4E  = (%.12lf,  %.12lf,  %.12lf) km   at epoch t2RE = %.12lf\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), r4E(0), r4E(1), r4E(2), t2RE);
         MessageInterface::ShowMessage("     Station %s position in %sMJ2000 coordinate system       :   r3E  = (%.12lf,  %.12lf,  %.12lf) km   at epoch t1TE = %.12lf\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), r3E(0), r3E(1), r3E(2), t1TE);
         MessageInterface::ShowMessage("     Spacecraft %s position in SSB inertial coordinate system:   r4EB = (%.12lf,  %.12lf,  %.12lf) km   at epoch t2RE = %.12lf\n", participants[1]->GetName().c_str(), r4EB(0), r4EB(1), r4EB(2), t2RE);
         MessageInterface::ShowMessage("     Station %s position in SSB inertial coordinate system   :   r3EB = (%.12lf,  %.12lf,  %.12lf) km   at epoch t1TE = %.12lf\n", participants[0]->GetName().c_str(), r3EB(0), r3EB(1), r3EB(2), t1TE);
         MessageInterface::ShowMessage("     Transformation matrix from Earth fixed coordinate system to FK5 coordinate system at epoch t1TE = %.12lf:\n", t1TE);
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtE1(0,0), mtE1(0,1), mtE1(0,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtE1(1,0), mtE1(1,1), mtE1(1,2));
         MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mtE1(2,0), mtE1(2,1), mtE1(2,2));
#ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("     Uplink vector in EarthMJ2000Eq coordinate system:   uplinkVectorE = r4E - r3E = (%.12lf,  %.12lf,  %.12lf) km\n", uplinkVectorE(0), uplinkVectorE(1), uplinkVectorE(2));
#else
         MessageInterface::ShowMessage("     Uplink vector in SSB inertial coordinate system:   uplinkVectorE = r4EB - r3EB = (%.12lf,  %.12lf,  %.12lf) km\n", uplinkVectorE(0), uplinkVectorE(1), uplinkVectorE(2));
#endif
         MessageInterface::ShowMessage("     Uplink range without ralativity correction: %.12lf km\n", uplinkRangeE);
         MessageInterface::ShowMessage("     Relativity correction for uplink leg      : %.12lf km\n", uplinkLegE.GetRelativityCorrection());
         MessageInterface::ShowMessage("     Uplink range with relativity correction   : %.12lf km\n", uplinkLegE.GetRelativityCorrection() + uplinkRangeE);
         MessageInterface::ShowMessage("     (ET-TAI) at t1TE = %.12le s\n", ettaiT1E);
      #endif

      // 8. Calculate up link range rate for measurement E:
      Rvector3 p3VE = uplinkLegE.GetVelocity(participants[0]);                  // velocity of station at transmit time t1T in central body MJ2000 coordinate system
      Rvector3 p4VE = uplinkLegE.GetVelocity(participants[1]);                  // velocity of spacecraft at reception time t2R in central body MJ2000 coordinate system

      Rvector3 ssb2cbV_t1TE = cb1->GetMJ2000Velocity(t1TE) - ssb->GetMJ2000Velocity(t1TE);      // velocity of central body at time t1T w.r.t SSB MJ2000Eq coordinate system
      Rvector3 ssb2cbV_t2RE = cb2->GetMJ2000Velocity(t2RE) - ssb->GetMJ2000Velocity(t2RE);      // velocity of central body at time t2R w.r.t SSB MJ2000Eq coordinate system
      
      Rvector3 p3VEB = ssb2cbV_t1TE + p3VE;                                     // velocity of station at transmit time t1T in SSB coordinate system
      Rvector3 p4VEB = ssb2cbV_t2RE + p4VE;                                     // velocity of spacecraft at reception time t2R in SSB coordinate system

      // @todo Relative origin velocities need to be subtracted when the origins
      // differ; check and fix that part using r12_j2k_vel here.  It's not yet
      // incorporated because we need to handle the different epochs for the
      // bodies, and we ought to do this part in barycentric coordinates
#ifdef USE_EARTHMJ2000EQ_CS
      Rvector3 upRRateVecE = p4VE - p3VE;
#else
      Rvector3 upRRateVecE = p4VEB - p3VEB /* - r12_j2k_vel*/ ;
#endif
      rangeUnit = uplinkVectorE.GetUnitVector();
      uplinkRangeRate[1] = upRRateVecE * rangeUnit;

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("2.4. Uplink leg range rate for measurement E:\n");
         MessageInterface::ShowMessage("     Station %s velocity in %sMJ2000 coordinate system       :  p3VE  = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[0]->GetName().c_str(), participants[0]->GetJ2000BodyName().c_str(), p3VE(0), p3VE(1), p3VE(2));
         MessageInterface::ShowMessage("     Spacecraft %s velocity in %sMJ2000 coordinate system    :  p4VE  = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[1]->GetName().c_str(), participants[1]->GetJ2000BodyName().c_str(), p4VE(0), p4VE(1), p4VE(2));
         MessageInterface::ShowMessage("     Station %s velocity in SSB inertial coordinate system   :  p3VEB = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[0]->GetName().c_str(), p3VEB(0), p3VEB(1), p3VEB(2));
         MessageInterface::ShowMessage("     Spacecraft %s velocity in SSB inertial coordinate system:  p4VEB = (%.12lf,  %.12lf,  %.12lf)km/s\n", participants[1]->GetName().c_str(), p4VEB(0), p4VEB(1), p4VEB(2));
#ifdef USE_EARTHMJ2000EQ_CS
         MessageInterface::ShowMessage("     Spacecraft velocity w/r/t Station in EarthMJ2000Eq coordinate system:  upRRateVecE = (%.12lf,  %.12lf,  %.12lf)km/s\n", upRRateVecE(0), upRRateVecE(1), upRRateVecE(2));
#else
         MessageInterface::ShowMessage("     Spacecraft velocity w/r/t Station in SSB inertial coordinate system:  upRRateVecE = (%.12lf,  %.12lf,  %.12lf)km/s\n", upRRateVecE(0), upRRateVecE(1), upRRateVecE(2));
#endif
         MessageInterface::ShowMessage("     Uplink range rate:  uplinkRangeRateE = %.12lf km/s\n", uplinkRangeRate[1]);
      #endif


      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("2.5. Travel time and delays for measurement E:\n");
         MessageInterface::ShowMessage("     Uplink travel time  : %.12lf s\n", uplinkRangeE*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);
         MessageInterface::ShowMessage("     Downlink travel time: %.12lf s\n", downlinkRangeE*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);
         MessageInterface::ShowMessage("     Delay between receiving and transmitting on participant 2: %.12lf s\n", (t2TE - t2RE)*86400);
      #endif


      Real dtS, dtE, dtdt, preFactor;

      // 9. Get sensors used in DSN 2-ways Doppler
      UpdateHardware();

      if (participantHardware.empty()||
            ((!participantHardware.empty())&&
              participantHardware[0].empty()&&
              participantHardware[1].empty()
            )
         )
      {
         // Throw an exception when no hardware is defined due to signal frequency is specified base on hardware
         throw MeasurementException("No transmmitter, transponder, and receiver is defined in measurement participants.\n");
         return false;
      }

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("Start real measurement (with hardware delay and media correction):\n");
      #endif
      ObjectArray objList1;
      ObjectArray objList2;
      ObjectArray objList3;
      //         objList1 := all transmitters in participantHardware list
      //         objList2 := all receivers in participantHardware list
      //         objList3 := all transponders in participantHardware list
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

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   List of sensors: %s, %s, %s\n",
            gsTransmitter->GetName().c_str(), gsReceiver->GetName().c_str(),
            scTransponder->GetName().c_str());
      #endif


      // 10. Get transmitter, receiver, and transponder delays:
      Real transmitDelay = gsTransmitter->GetDelay();
      Real receiveDelay = gsReceiver->GetDelay();
      Real targetDelay = scTransponder->GetDelay();


      // 11. Get frequency, frequency band, turn around ratio, and time interval:
      Real uplinkFreqS, uplinkFreqE;
      Real uplinkFreqAtRecei;             // uplink frequency at receive epoch   // unit: MHz
      if (rampTB == NULL)
      {
         // Get uplink frequency from transmitter of ground station (participants[0])
         Signal* uplinkSignal = gsTransmitter->GetSignal();
         uplinkFreqS = uplinkFreqE = uplinkSignal->GetValue();      // unit: MHz
         uplinkFreqAtRecei = uplinkFreqS;                           // unit: MHz       // for constant frequency
         frequency  = frequencyE = uplinkFreqS*1.0e6;               // unit: Hz

         // Get frequency band:
         if (obsData != NULL)
            freqBand = freqBandE = obsData->uplinkBand;            // frequency band for this case is specified by observation data
         else
            freqBand = freqBandE = FrequencyBand(frequency);       // frequency band for this case is specified as shown in WikiPedia

         // Get turn around ratio:
         turnaround = scTransponder->GetTurnAroundRatio();         // the value of turn around ratio is get from GMAT script
         if (turnaround == 1.0)                                    // if the value of turn around ratio is not set by GMAT script, GMAT sets the value associated to frequency band   
            turnaround = GetTurnAroundRatio(freqBand);

         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
           MessageInterface::ShowMessage("   Uplink frequency is gotten from hardware...\n"); 
         #endif
      }
      else
      {
         // Get uplink frequency from ramped frequency table
         frequency = GetFrequencyFromRampTable(t1TS);                          // unit: Hz
         frequencyE = GetFrequencyFromRampTable(t1TE);                         // unit: Hz
         uplinkFreqS = frequency/1.0e6;                                        // unit: MHz
         uplinkFreqE = frequencyE/1.0e6;                                       // unit: MHz
         uplinkFreqAtRecei = GetFrequencyFromRampTable(t3RE) / 1.0e6;          // unit: MHz      // for ramped frequency

         // Get frequency band from ramp table:
         freqBand = GetUplinkBandFromRampTable(t1TS);
         freqBandE = GetUplinkBandFromRampTable(t1TE);
         if (freqBand != freqBandE)
            throw MeasurementException("Error: Frequency bands for S path and E path are not the same. In DSNTwoWayDoppler calculation, it assumes that frequency band for S path and E path signals have to be the same !!!\n");  

         turnaround = GetTurnAroundRatio(freqBand);            // Get value of turn around ratio associated with frequency band

         #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("   Uplink frequency is gotten from ramp table...: frequency = %.12le\n", frequency); 
         #endif
      }

      // Get time interval
      // Note that: when no observation data is used, interval value obtained from GMAT script was used
      if (obsData != NULL)
         interval = obsData->dopplerCountInterval;


      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("3. Sensors' infor:\n");
         MessageInterface::ShowMessage("    List of sensors: %s, %s, %s\n",
            gsTransmitter->GetName().c_str(), gsReceiver->GetName().c_str(),
            scTransponder->GetName().c_str());
         MessageInterface::ShowMessage("    Transmitter delay = %.12le s\n", transmitDelay);
         MessageInterface::ShowMessage("    Receiver delay    = %.12le s\n", receiveDelay);
         MessageInterface::ShowMessage("    Transponder delay = %.12le s\n", targetDelay);
         MessageInterface::ShowMessage("    UpLink signal frequency for S path = %.12le MHz\n", uplinkFreqS);
         MessageInterface::ShowMessage("    UpLink signal frequency for E path = %.12le MHz\n", uplinkFreqE);

         MessageInterface::ShowMessage("    frequency band for S path = %d\n", freqBand);
         MessageInterface::ShowMessage("    frequency band for E path = %d\n", freqBandE);
         MessageInterface::ShowMessage("    turn around ratio = %lf\n", turnaround);
         MessageInterface::ShowMessage("    time interval = %lf s\n", interval);
      #endif


      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("4. Media Correction Calculation:\n");
      #endif

      // 12. Calculate for measurement S:
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("4.1. Media correction calculation for measurement S:\n");
         MessageInterface::ShowMessage("4.1.1. Media correction for uplink leg:\n");
      #endif

      // 12.1. Calculate media correction for uplink leg:
      // r3SB and r4SB are location of station and spacecraft in SSB inertial coordinate system for uplink leg   for S path
      RealArray uplinkCorrectionS = CalculateMediaCorrection(uplinkFreqS, r3SB, r4SB, t1TS, t2RS, minAngle);
     
      Real uplinkRangeCorrectionS = uplinkCorrectionS[0]*GmatMathConstants::M_TO_KM + uplinkLegS.GetRelativityCorrection();
      Real uplinkRealRangeS = uplinkRangeS + uplinkRangeCorrectionS;
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("        Uplink media correction           = %.12lf m\n",uplinkCorrectionS[0]);
         MessageInterface::ShowMessage("        Uplink relativity correction      = %.12lf km\n",uplinkLegS.GetRelativityCorrection());
         MessageInterface::ShowMessage("        Uplink range correction           = %.12lf km\n",uplinkRangeCorrectionS);
         MessageInterface::ShowMessage("        Uplink precision light time range = %.12lf km\n",uplinkRangeS);
         MessageInterface::ShowMessage("        Uplink real range                 = %.12lf km\n",uplinkRealRangeS);
      #endif

      // 12.2. Doppler shift the frequency from the transmitter using uplinkRangeRate:
      Real uplinkDSFreqS = (1 - uplinkRangeRate[0]*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*uplinkFreqS;

      // 12.3. Set frequency for the input signal of transponder
      Signal* inputSignal = scTransponder->GetSignal(0);
      inputSignal->SetValue(uplinkDSFreqS);
      scTransponder->SetSignal(inputSignal, 0);

      // 12.4. Check the transponder feasibility to receive the input signal:
      if (scTransponder->IsFeasible(0) == false)
      {
          currentMeasurement.isFeasible = false;
          currentMeasurement.value[0] = 0;
          throw MeasurementException("The transponder is unfeasible to receive uplink signal for S path.\n");
      }

      // 12.5. Get frequency of transponder output signal
      Signal* outputSignal = scTransponder->GetSignal(1);
      Real downlinkFreqS = outputSignal->GetValue();

      // 12.6. Doppler shift the transponder output frequency by the downlinkRangeRate:
      Real downlinkDSFreqS = (1 - downlinkRangeRate[0]*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*downlinkFreqS;

      // 12.7. Set frequency on receiver
      Signal* downlinkSignal = gsReceiver->GetSignal();
      downlinkSignal->SetValue(downlinkDSFreqS);

      // 12.8. Check the receiver feasibility to receive the downlink signal
      if (gsReceiver->IsFeasible() == false)
      {
          currentMeasurement.isFeasible = false;
          currentMeasurement.value[0] = 0;
          throw MeasurementException("The receiver is unfeasible to receive downlink signal for S path.\n");
      }

      // 12.9. Calculate media correction for downlink leg:
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("4.1.2. Media correction for downlink leg:\n");
         MessageInterface::ShowMessage("       Uplink transmit frequency                                                            = %.12le MHz\n", uplinkFreqS);
         MessageInterface::ShowMessage("       Uplink Doppler shift frequency                                                       = %.12le MHz\n", uplinkDSFreqS);
         MessageInterface::ShowMessage("       Downlink frequency = Turn around ratio x Uplink Doppler shift frequency              = %.12le Mhz\n", downlinkFreqS);
         MessageInterface::ShowMessage("       Downlink Doppler shift frequency = (1 - downlink range rate / c)x Downlink frequency = %.12le MHz\n", downlinkDSFreqS);
      #endif
      // r1SB and r2SB are location of station and spacecraft in central body inertial coordinate system for downlink leg   for S path
      RealArray downlinkCorrectionS = CalculateMediaCorrection(downlinkDSFreqS, r1SB, r2SB, t3RS, t2TS, minAngle);

      Real downlinkRangeCorrectionS = downlinkCorrectionS[0]*GmatMathConstants::M_TO_KM + downlinkLegS.GetRelativityCorrection();
      Real downlinkRealRangeS = downlinkRangeS + downlinkRangeCorrectionS;
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("       Downlink media correction           = %.12lf m\n",downlinkCorrectionS[0]);
         MessageInterface::ShowMessage("       Downlink relativity correction      = %.12lf km\n",downlinkLegS.GetRelativityCorrection());
         MessageInterface::ShowMessage("       Downlink range correction           = %.12lf km\n",downlinkRangeCorrectionS);
         MessageInterface::ShowMessage("       Downlink precision light time range = %.12lf km\n",downlinkRangeS);
         MessageInterface::ShowMessage("       Downlink real range                 = %.12lf km\n",downlinkRealRangeS);
      #endif

      // 13. Calculate for measurement E:
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("4.2. Media correction calculation for measurement E:\n");
         MessageInterface::ShowMessage("4.2.1. Media correction for uplink leg:\n");
      #endif
      // 13.1. Calculate media correction for uplink leg:
      // r3EB and r4EB are location of station and spacecraft in SSB inertial coordinate system for uplink leg   for E path
      RealArray uplinkCorrectionE = CalculateMediaCorrection(uplinkFreqE, r3EB, r4EB, t1TE, t2RE, minAngle);

      Real uplinkRangeCorrectionE = uplinkCorrectionE[0]*GmatMathConstants::M_TO_KM + uplinkLegE.GetRelativityCorrection();
      Real uplinkRealRangeE = uplinkRangeE + uplinkRangeCorrectionE;
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("        Uplink media correction           = %.12lf m\n",uplinkCorrectionE[0]);
         MessageInterface::ShowMessage("        Uplink relativity correction      = %.12lf km\n",uplinkLegE.GetRelativityCorrection());
         MessageInterface::ShowMessage("        Uplink range correction           = %.12lf km\n",uplinkRangeCorrectionE);
         MessageInterface::ShowMessage("        Uplink precision light time range = %.12lf km\n",uplinkRangeE);
         MessageInterface::ShowMessage("        Uplink real range                 = %.12lf km\n",uplinkRealRangeE);
      #endif

      // 13.2. Doppler shift the frequency from the transmitter using uplinkRangeRate:
      Real uplinkDSFreqE = (1 - uplinkRangeRate[1]*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*uplinkFreqE;

      // 13.3. Set frequency for the input signal of transponder
      inputSignal = scTransponder->GetSignal(0);
      inputSignal->SetValue(uplinkDSFreqE);
      scTransponder->SetSignal(inputSignal, 0);

      // 13.4. Check the transponder feasibility to receive the input signal:
      if (scTransponder->IsFeasible(0) == false)
      {
         currentMeasurement.isFeasible = false;
         currentMeasurement.value[0] = 0;
         throw MeasurementException("The transponder is unfeasible to receive uplink signal for E path.\n");
      }

      // 13.5. Get frequency of transponder output signal
      outputSignal = scTransponder->GetSignal(1);
      Real downlinkFreqE = outputSignal->GetValue();

      // 13.6. Doppler shift the transponder output frequency by the downlinkRangeRate:
      Real downlinkDSFreqE = (1 - downlinkRangeRate[1]*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*downlinkFreqE;

      // 13.7. Set frequency on receiver
      downlinkSignal = gsReceiver->GetSignal();
      downlinkSignal->SetValue(downlinkDSFreqE);

      // 13.8. Check the receiver feasibility to receive the downlink signal
      if (gsReceiver->IsFeasible() == false)
      {
         currentMeasurement.isFeasible = false;
         currentMeasurement.value[0] = 0;
         throw MeasurementException("The receiver is unfeasible to receive downlink signal for E path.\n");
      }

      // 13.9. Calculate media correction for downlink leg:
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("4.2.2. Media correction for downlink leg:\n");
         MessageInterface::ShowMessage("       Uplink transmit frequency                                                            = %.12le MHz\n", uplinkFreqE);
         MessageInterface::ShowMessage("       Uplink Doppler shift frequency                                                       = %.12le MHz\n", uplinkDSFreqE);
         MessageInterface::ShowMessage("       Downlink frequency = Turn around ratio x Uplink Doppler shift frequency              = %.12le Mhz\n", downlinkFreqE);
         MessageInterface::ShowMessage("       Downlink Doppler shift frequency = (1 - downlink range rate / c)x Downlink frequency = %.12le MHz\n", downlinkDSFreqE);
      #endif

      // r1EB and r2EB are location of station and spacecraft in SSB inertial coordinate system for downlink leg   for E path
         RealArray downlinkCorrectionE = CalculateMediaCorrection(downlinkDSFreqE, r1EB, r2EB, t3RE, t2TE, minAngle);

      Real downlinkRangeCorrectionE = downlinkCorrectionE[0]*GmatMathConstants::M_TO_KM + downlinkLegE.GetRelativityCorrection();
      Real downlinkRealRangeE = downlinkRangeE + downlinkRangeCorrectionE;
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("       Downlink media correction           = %.12lf m\n",downlinkCorrectionE[0]);
         MessageInterface::ShowMessage("       Downlink relativity correction      = %.12lf km\n",downlinkLegE.GetRelativityCorrection());
         MessageInterface::ShowMessage("       Downlink range correction           = %.12lf km\n",downlinkRangeCorrectionE);
         MessageInterface::ShowMessage("       Downlink precision light time range = %.12lf km\n",downlinkRangeE);
         MessageInterface::ShowMessage("       Downlink real range                 = %.12lf km\n",downlinkRealRangeE);
      #endif


      // 14. Time travel for start path and the end path:
      // 14.1. Calculate ET-TAI correction
      Real ettaiCorrectionS = (useETminusTAICorrection?(ettaiT1S - ettaiT3S):0.0);
      Real ettaiCorrectionE = (useETminusTAICorrection?(ettaiT1E - ettaiT3E):0.0);
      // 14.2 Total times for the start path and the end path:
      dtS = (uplinkRealRangeS + downlinkRealRangeS)*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM + ettaiCorrectionS + 
            transmitDelay + receiveDelay + targetDelay;
      dtE = (uplinkRealRangeE + downlinkRealRangeE)*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM + ettaiCorrectionE + 
            transmitDelay + receiveDelay + targetDelay;
      
      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         Real uplinkTimeS   = uplinkRealRangeS*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;
         Real downlinkTimeS = downlinkRealRangeS*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;
         Real uplinkTimeE   = uplinkRealRangeE*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;
         Real downlinkTimeE = downlinkRealRangeE*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;

         MessageInterface::ShowMessage("5. Travel time:\n");
         MessageInterface::ShowMessage("5.1. Travel time for S-path:\n");
         MessageInterface::ShowMessage("     Uplink time         = %.12lf s\n", uplinkTimeS);
         MessageInterface::ShowMessage("     Downlink time       = %.12lf s\n", downlinkTimeS);
         MessageInterface::ShowMessage("     (ET-TAI) correction = %.12le s\n", ettaiCorrectionS);
         MessageInterface::ShowMessage("     Transmit delay      = %.12le s\n", transmitDelay);
         MessageInterface::ShowMessage("     Transpond delay     = %.12le s\n", targetDelay);
         MessageInterface::ShowMessage("     Receive delay       = %.12le s\n", receiveDelay);
         MessageInterface::ShowMessage("     Real travel time    = %.15lf s\n", dtS);

         MessageInterface::ShowMessage("5.2. Travel time for E-path:\n");
         MessageInterface::ShowMessage("     Uplink time         = %.12lf s\n", uplinkTimeE);
         MessageInterface::ShowMessage("     Downlink time       = %.12lf s\n", downlinkTimeE);
         MessageInterface::ShowMessage("     (ET-TAI) correction = %.12le s\n", ettaiCorrectionE);
         MessageInterface::ShowMessage("     Transmit delay      = %.12le s\n", transmitDelay);
         MessageInterface::ShowMessage("     Transpond delay     = %.12le s\n", targetDelay);
         MessageInterface::ShowMessage("     Receive delay       = %.12le s\n", receiveDelay);
         MessageInterface::ShowMessage("     Real travel time    = %.15lf s\n", dtE);
      #endif


      // 15. Check feasibility of signals for Start path and End path:
      currentMeasurement.isFeasible = true;
      // 15.1. Check for Start part
      // UpdateRotationMatrix(t1TS, "R_o_j2k");
      UpdateRotationMatrix(t1TS, "o_j2k");
      Rvector3 outState = (R_o_j2k * (r4S - r3S)).GetUnitVector();
      currentMeasurement.feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;      // elevation angle in degree
      if (currentMeasurement.feasibilityValue > minAngle)
      {
         // UpdateRotationMatrix(t3RS, "R_o_j2k");
         UpdateRotationMatrix(t3RS, "o_j2k");
         outState = (R_o_j2k * (r2S - r1S)).GetUnitVector();
         Real feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;                 // elevation angle in degree

         if (feasibilityValue > minAngle)
         {
            currentMeasurement.unfeasibleReason = "N";
            currentMeasurement.isFeasible = true;
         }
         else
         {
            currentMeasurement.feasibilityValue = feasibilityValue;
            currentMeasurement.unfeasibleReason = "B2S";      // S-path's downlink leg signal was blocked
            currentMeasurement.isFeasible = false;
         }
      }
      else
      {
         currentMeasurement.unfeasibleReason = "B1S";         // S-path's uplink leg signal was blocked
         currentMeasurement.isFeasible = false;
      }

      // 15.2. When Start path is feasible, we check for End part
      if (currentMeasurement.isFeasible)
      {
         // UpdateRotationMatrix(t1TE, "R_o_j2k");
         UpdateRotationMatrix(t1TE, "o_j2k");
         outState = (R_o_j2k * (r4E - r3E)).GetUnitVector();
         currentMeasurement.feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;      // elevation angle in degree
         if (currentMeasurement.feasibilityValue > minAngle)
         {
            // UpdateRotationMatrix(t3RE, "R_o_j2k");
            UpdateRotationMatrix(t3RE, "o_j2k");
            outState = (R_o_j2k * (r2E - r1E)).GetUnitVector();
            Real feasibilityValue = asin(outState[2])*GmatMathConstants::DEG_PER_RAD;                 // elevation angle in degree

            if (feasibilityValue > minAngle)
            {
               currentMeasurement.unfeasibleReason = "N";
               currentMeasurement.isFeasible = true;
            }
            else
            {
               currentMeasurement.feasibilityValue = feasibilityValue;
               currentMeasurement.unfeasibleReason = "B2E";         // E-path's downlink leg signal was blocked
               currentMeasurement.isFeasible = false;
            }
         }
         else
         {
            currentMeasurement.unfeasibleReason = "B1E";            // E-path's uplink leg signal was blocked
            currentMeasurement.isFeasible = false;
         }
      }
     

      // 16. Calculate Frequency Doppler Shift:
      dtdt = dtE - dtS;

      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("6. Calculate Frequency Doppler Shift:\n");
         MessageInterface::ShowMessage("    t1S                   : %.12lf s\n"
                                    "    t1E                   : %.12lf s\n"
                              "    t3S                   : %.12lf s\n"
                              "    t3E                   : %.12lf s\n"
                                       "    Turnaround ratio      : %.15lf\n"
                              "    M2R                   : %.15lf\n"
                                     "    Time interval         : %.12lf s\n", t1TS, t1TE, t3RS, t3RE, turnaround, M2R, interval);
         MessageInterface::ShowMessage("    Travel time for S path: %.15lf s\n", dtS);
         MessageInterface::ShowMessage("    Travel time for E path: %.15lf s\n", dtE);
         MessageInterface::ShowMessage("         dtdt = dtE - dtS : %.15lf s\n", dtdt);
      #endif

      currentMeasurement.uplinkFreq = frequencyE;                          // unit: Hz        // uplink frequency for E path
      currentMeasurement.uplinkFreqAtRecei = uplinkFreqAtRecei * 1.0e6;    // unit: Hz
      currentMeasurement.uplinkBand = freqBand;
      currentMeasurement.dopplerCountInterval = interval;

      if (rampTB != NULL)
      {
         Integer errnum;
         try
         {
//           currentMeasurement.value[0] = (M2R*IntegralRampedFrequency(t3RE, interval) - turnaround*IntegralRampedFrequency(t1TE, interval + dtS-dtE))/ interval;
             currentMeasurement.value[0] = - turnaround*IntegralRampedFrequency(t1TE, interval + dtS-dtE, errnum)/ interval;
         } catch (MeasurementException exp)
         {
            currentMeasurement.value[0] = 0.0;                  // It has no C-value due to the failure of calculation of IntegralRampedFrequency()
//            currentMeasurement.uplinkFreq        = frequencyE;                   // unit: Hz
//            currentMeasurement.uplinkFreqAtRecei = uplinkFreqAtRecei * 1.0e6;    // unit: Hz
//            currentMeasurement.uplinkBand = freqBand;
//            currentMeasurement.dopplerCountInterval = interval;
            currentMeasurement.isFeasible = false;
            currentMeasurement.unfeasibleReason = "R";
//          currentMeasurement.feasibilityValue is set to elevation angle as shown in section 15
         
            if ((errnum == 2)||(errnum == 3))
               throw exp;
            else
               return false;
         }

         #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
            Real rampedIntegral = IntegralRampedFrequency(t1TE, interval + dtS-dtE, errnum);
            MessageInterface::ShowMessage("    Use ramped DSNTwoWayDoppler calculation:\n");
            MessageInterface::ShowMessage("    ramped integral = %.12lf\n", rampedIntegral);
         #endif
      }
      else
      {
         #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("    Use unramped DSNTwoWayDoppler calculation:\n");
            MessageInterface::ShowMessage("       frequency        : %.12lf Hz\n", frequency);
         #endif
       
         currentMeasurement.value[0] = -turnaround*frequency*(interval - dtdt)/interval;
      }


      #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("    Calculated Doppler C = %.12le Hz\n", currentMeasurement.value[0]);
         if (obsData != NULL)
         {
            MessageInterface::ShowMessage("    Observated Doppler O = %.12le Hz\n", obsData->value[0]);
            MessageInterface::ShowMessage("    O-C = %.12lf Hz\n", obsData->value[0] - currentMeasurement.value[0]);
         }
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

      // Add noise to calculated measurement
      if (noiseSigma != NULL)
      {
         #ifdef DEBUG_DOPPLER_CALC_WITH_EVENTS
            MessageInterface::ShowMessage("Generate noise and it to measurement\n"); 
         #endif
         Real v_sign = ((currentMeasurement.value[0] < 0.0)?-1.0:1.0);
         RandomNumber* rn = RandomNumber::Instance();
         Real val = rn->Gaussian(currentMeasurement.value[0], noiseSigma->GetElement(0));
         while (val*v_sign <= 0.0)
            val = rn->Gaussian(currentMeasurement.value[0], noiseSigma->GetElement(0));
         currentMeasurement.value[0] = val;
      }

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


//------------------------------------------------------------------------------
// Real DSNTwoWayDoppler::GetTurnAroundRatio(Integer freqBand)
//------------------------------------------------------------------------------
/**
 * Retrieves trun around ratio
 *
 * @param freqBand   frequency band
 *
 * return   the value of trun around ratio associated with frequency band 
 */
//------------------------------------------------------------------------------
Real DSNTwoWayDoppler::GetTurnAroundRatio(Integer freqBand)
{
   switch (freqBand)
   {
      case 1:            // for S-band, turn around ratio is 240/221
         return 240.0/221.0;
      case 2:            // for X-band, turn around ratio is 880/749
         return 880.0/749.0;
   }

   // Display an error message when frequency band is not specified 
   std::stringstream ss;
   ss << "Error: frequency band " << freqBand << " is not specified.\n";
   throw MeasurementException(ss.str());
}

