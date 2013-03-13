//$Id: DSNTwoWayRange.cpp 67 2010-03-05 21:56:16Z djconway@NDC $
//------------------------------------------------------------------------------
//                         DSNTwoWayRange
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2010/03/08
//
/**
 * The DSN 2-way range core measurement model.
 */
//------------------------------------------------------------------------------


#include "DSNTwoWayRange.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "Spacecraft.hpp"
#include "GroundstationInterface.hpp"
#include "Antenna.hpp"
#include "Transmitter.hpp"
#include "Receiver.hpp"
#include "Transponder.hpp"
#include "Troposphere.hpp"

#define DEBUG_RANGE_CALC_WITH_EVENTS
//#define VIEW_PARTICIPANT_STATES_WITH_EVENTS
//#define DEBUG_RANGE_CALC
//#define DEBUG_DERIVATIVES
//#define VIEW_PARTICIPANT_STATES


//------------------------------------------------------------------------------
// DSNTwoWayRange(const std::string nomme)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param nomme The name of the core measurement model
 */
//------------------------------------------------------------------------------
DSNTwoWayRange::DSNTwoWayRange(const std::string nomme) :
   TwoWayRange          ("DSNTwoWayRange", nomme)
{
   objectTypeNames.push_back("DSNTwoWayRange");

   // Prep value array in measurement
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "DSNTwoWayRange";
   currentMeasurement.type = Gmat::DSN_TWOWAYRANGE;

   // Default to no delays; these are obtained from the participant hardware
   transmitDelay = 0.0;    // DSN 2-way includes electronics delays
   targetDelay   = 0.0;    // Needed for light iteration, not used otherwise
   receiveDelay  = 0.0;    // DSN 2-way includes electronics delays

   covariance.SetDimension(1);
   covariance(0,0) = 1.0;

   // If we code in specific stations, we could do it like this:
//   freqMap["DSN43"] = 2090659968.0;
//   freqMap["DSN14"] = 2090659968.0;
}


//------------------------------------------------------------------------------
// ~DSNTwoWayRange()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DSNTwoWayRange::~DSNTwoWayRange()
{
}



//------------------------------------------------------------------------------
// DSNTwoWayRange(const DSNTwoWayRange& dsn)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param dsn The model that is getting copied
 */
//------------------------------------------------------------------------------
DSNTwoWayRange::DSNTwoWayRange(const DSNTwoWayRange& dsn) :
   TwoWayRange       (dsn),
   freqMap           (dsn.freqMap)
{
   currentMeasurement.value.push_back(0.0);
   currentMeasurement.typeName = "DSNTwoWayRange";
   currentMeasurement.type = Gmat::DSN_TWOWAYRANGE;
   currentMeasurement.uniqueID = dsn.currentMeasurement.uniqueID;
   currentMeasurement.participantIDs.push_back("NotSet");
   currentMeasurement.participantIDs.push_back("NotSet");

   covariance = dsn.covariance;
   freqMap    = dsn.freqMap;
}


//------------------------------------------------------------------------------
// DSNTwoWayRange& operator=(const DSNTwoWayRange& dsn)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param dsn The model that is getting copied
 *
 * @return This DSN 2-way range model, configured to match dsn.
 */
//------------------------------------------------------------------------------
DSNTwoWayRange& DSNTwoWayRange::operator=(const DSNTwoWayRange& dsn)
{
   if (this != &dsn)
   {
      TwoWayRange::operator=(dsn);

      // Allocate exactly one value in current measurement for range
      currentMeasurement.value.clear();
      currentMeasurement.value.push_back(0.0);
      currentMeasurement.typeName = "DSNTwoWayRange";
      currentMeasurement.type = Gmat::DSN_TWOWAYRANGE;
      currentMeasurement.uniqueID = dsn.currentMeasurement.uniqueID;

      covariance = dsn.covariance;
      freqMap    = dsn.freqMap;
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
 * @return A new DSN 2-way range model configured to match this one
 */
//------------------------------------------------------------------------------
GmatBase* DSNTwoWayRange::Clone() const
{
   return new DSNTwoWayRange(*this);
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
bool DSNTwoWayRange::Initialize()
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
const std::vector<RealArray>& DSNTwoWayRange::CalculateMeasurementDerivatives(
      GmatBase *obj, Integer id)
{
   #ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("DSNTwoWayRange::CalculateMeasurement"
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
            "DSNTwoWayRange error - object is neither participant nor "
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
//         CalculateRangeVectorInertial();
//         Rvector3 tmp, result;
//         Rvector3 rangeUnit = rangeVecInertial.GetUnitVector();
//         #ifdef DEBUG_DERIVATIVES
//            MessageInterface::ShowMessage("   RVInertial      = %.12lf %.12lf %.12lf\n",
//                     rangeVecInertial[0], rangeVecInertial[1], rangeVecInertial[2]);
//            MessageInterface::ShowMessage("   Unit RVInertial = %.12lf %.12lf %.12lf ",
//                     rangeUnit[0], rangeUnit[1], rangeUnit[2]);
//         #endif
//         if (stationParticipant)
//         {
//            for (UnsignedInt i = 0; i < 3; ++i)
//               tmp[i] = - rangeUnit[i];
//
//            // for a Ground Station, need to rotate to the F1 frame
//            result = tmp * R_j2k_1;
//            for (UnsignedInt jj = 0; jj < 3; jj++)
//               currentDerivatives[0][jj] = result[jj];
//         }
//         else
//         {
//            // for a spacecraft participant 1, we don't need the rotation matrices (I33)
//            for (UnsignedInt i = 0; i < 3; ++i)
//               currentDerivatives[0][i] = - rangeUnit[i];
//         }
         }
         else if (objPtr->GetParameterText(parameterID) == "Velocity")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() +" velocity is not yet implemented");
//         for (UnsignedInt i = 0; i < 3; ++i)
//            currentDerivatives[0][i] = 0.0;
         }
         else if (objPtr->GetParameterText(parameterID) == "CartesianX")
         {
            throw MeasurementException("Derivative w.r.t. " +
                  participants[0]->GetName() +" Cartesian state is not yet "
                        "implemented");
//         CalculateRangeVectorInertial();
//         Rvector3 tmp, result;
//         Rvector3 rangeUnit = rangeVecInertial.GetUnitVector();
//         #ifdef DEBUG_DERIVATIVES
//            MessageInterface::ShowMessage("   RVInertial      = %.12lf %.12lf %.12lf\n",
//                     rangeVecInertial[0], rangeVecInertial[1], rangeVecInertial[2]);
//            MessageInterface::ShowMessage("   Unit RVInertial = %.12lf %.12lf %.12lf ",
//                     rangeUnit[0], rangeUnit[1], rangeUnit[2]);
//         #endif
//         if (stationParticipant)
//         {
//            for (UnsignedInt i = 0; i < 3; ++i)
//               tmp[i] = - rangeUnit[i];
//
//            // for a Ground Station, need to rotate to the F1 frame
//            result = tmp * R_j2k_1;
//            for (UnsignedInt jj = 0; jj < 3; jj++)
//               currentDerivatives[0][jj] = result[jj];
//         }
//         else
//         {
//            // for a spacecraft participant 1, we don't need the rotation matrices (I33)
//            for (UnsignedInt i = 0; i < 3; ++i)
//               currentDerivatives[0][i] = - rangeUnit[i];
//         }
//         // velocity all zeroes
//         for (UnsignedInt ii = 3; ii < 6; ii++)
//            currentDerivatives[0][ii] = 0.0;
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
      else if (objNumber == 2) // participant 2, always a Spacecraft
      {
         Real fFactor = GetFrequencyFactor(frequency) /
               (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M);

         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s of Participant"
                     " 2\n", objPtr->GetParameterText(parameterID).c_str());
            MessageInterface::ShowMessage("   freq = %15lf, F = %15lf, F/c = "
                  "%15lf\n", frequency, GetFrequencyFactor(frequency), fFactor);
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

            // Add 'em up per eq 7.80
            for (Integer i = 0; i < 3; ++i)
               currentDerivatives[0][i] =
                     fFactor * (uplinkRderiv[i] + downlinkRderiv[i]);
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

            // Add 'em up per eq 7.81
            for (Integer i = 0; i < 3; ++i)
               currentDerivatives[0][i] =
                     fFactor * (uplinkVderiv[i] + downlinkVderiv[i]);
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


            // Add 'em up per eq 7.80 and 7.81
            for (Integer i = 0; i < 6; ++i)
               currentDerivatives[0][i] =
                     fFactor * (uplinkDeriv[i] + downlinkDeriv[i]);
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
bool DSNTwoWayRange::Evaluate(bool withEvents)
{
   bool retval = false;

   if (!initialized)
      InitializeMeasurement();

   #ifdef DEBUG_RANGE_CALC
      MessageInterface::ShowMessage("Entered DSNTwoWayRange::Evaluate(%s)\n",
            (withEvents ? "true" : "false"));
      MessageInterface::ShowMessage("  ParticipantCount: %d\n",
            participants.size());
   #endif

   if (withEvents == false)
   {
      #ifdef DEBUG_RANGE_CALC
         MessageInterface::ShowMessage("DSN 2-Way Range Calculation without "
               "events\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES
         DumpParticipantStates("++++++++++++++++++++++++++++++++++++++++++++\n"
               "Evaluating DSN 2-Way Range without events");
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
         MessageInterface::ShowMessage("Calculating DSN 2-Way Range at epoch "
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

         SetHardwareDelays(false);

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
         MessageInterface::ShowMessage("\n\n DSN 2-Way Range Calculation:\n");
      #endif

      #ifdef VIEW_PARTICIPANT_STATES_WITH_EVENTS
         DumpParticipantStates("********************************************\n"
               "Evaluating DSN 2-Way Range with located events");
      #endif

      // 1. Get the range from the down link
      Rvector3 r1, r2;
	  Real t1, t2;
      r1 = downlinkLeg.GetPosition(participants[0]);
      r2 = downlinkLeg.GetPosition(participants[1]);
	  t1 = downlinkLeg.GetEventData((GmatBase*) participants[0]).epoch;
	  t2 = downlinkLeg.GetEventData((GmatBase*) participants[1]).epoch;
	  Rmatrix33 mt = downlinkLeg.GetEventData((GmatBase*) participants[0]).rInertial2obj.Transpose();
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
	     MessageInterface::ShowMessage("1. Get downlink leg range:\n");
         MessageInterface::ShowMessage("   Ground station position in FK5: r1 = (%f, %f, %f)km  at epoch = %18.12lf\n", r1.Get(0), r1.Get(1), r1.Get(2), t1);
         MessageInterface::ShowMessage("   Spacecraft position in FK5    : r2 = (%f, %f, %f)km  at epoch = %18.12lf\n", r2.Get(0), r2.Get(1), r2.Get(2), t2);
		 MessageInterface::ShowMessage("   Transformation matrix from Earth fixed coordinate system to FK5 coordinate system at epoch = %18.12lf:\n", t1);
		 MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt(0,0), mt(0,1), mt(0,2));
		 MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt(1,0), mt(1,1), mt(1,2));
		 MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt(2,0), mt(2,1), mt(2,2));
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
	     MessageInterface::ShowMessage("2. Get downlink leg range rate:\n");
         MessageInterface::ShowMessage("   Ground station velocity in FK5: p1V = (%f, %f, %f)km/s\n", p1V.Get(0), p1V.Get(1), p1V.Get(2));
         MessageInterface::ShowMessage("   Spacecraft velocity in FK5    : p2V = (%f, %f, %f)km/s\n", p2V.Get(0), p2V.Get(1), p2V.Get(2));
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


      // 3. Get the range from the uplink
      Rvector3 r3, r4;
	  Real t3, t4;
      r3 = uplinkLeg.GetPosition(participants[0]);
      r4 = uplinkLeg.GetPosition(participants[1]);
	  t3 = uplinkLeg.GetEventData((GmatBase*) participants[0]).epoch;
	  t4 = uplinkLeg.GetEventData((GmatBase*) participants[1]).epoch;
	  Rmatrix33 mt1 = uplinkLeg.GetEventData((GmatBase*) participants[0]).rInertial2obj.Transpose();
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
	     MessageInterface::ShowMessage("3. Get uplink leg range:\n");
         MessageInterface::ShowMessage("   Spacecraft position in FK5    : r4 = (%f, %f, %f)km   at epoch = %18.12lf\n", r4.Get(0), r4.Get(1), r4.Get(2), t4);
         MessageInterface::ShowMessage("   Ground station position in FK5: r3 = (%f, %f, %f)km   at epoch = %18.12lf\n", r3.Get(0), r3.Get(1), r3.Get(2), t3);
		 MessageInterface::ShowMessage("   Transformation matrix from Earth fixed coordinate system to FK5 coordinate system at epoch = %18.12lf:\n", t3);
		 MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt1(0,0), mt1(0,1), mt1(0,2));
		 MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt1(1,0), mt1(1,1), mt1(1,2));
		 MessageInterface::ShowMessage("                %18.12lf  %18.12lf  %18.12lf\n", mt1(2,0), mt1(2,1), mt1(2,2));
      #endif
      Rvector3 uplinkVector = r4 - r3;
      uplinkRange = uplinkVector.GetMagnitude();
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("   Uplink Range = r4-r3:  %.12lf km\n",
               uplinkRange);
      #endif


   	// 4. Calculate up link range rate
      Rvector3 p3V = uplinkLeg.GetVelocity(participants[0]);
      Rvector3 p4V = uplinkLeg.GetVelocity(participants[1]);
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
	     MessageInterface::ShowMessage("4. Get uplink leg range rate:\n");
         MessageInterface::ShowMessage("   Ground station velocity in FK5: p3V = (%f, %f, %f)km/s\n", p3V.Get(0), p3V.Get(1), p3V.Get(2));
         MessageInterface::ShowMessage("   Spacecraft velocity in FK5    : p4V = (%f, %f, %f)km/s\n", p4V.Get(0), p4V.Get(1), p4V.Get(2));
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

      // 4.1. Target range rate: Do we need this as well?
      targetRangeRate = (downlinkRangeRate + uplinkRangeRate) / 2.0;

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("   Target Range Rate:  %.12lf km/s\n",
						targetRangeRate);
		#endif


		// 5. Get sensors used in DSN 2-ways range
	   if (participantHardware.empty()||
	   		((!participantHardware.empty())&&
	   		  participantHardware[0].empty()&&
	   		  participantHardware[1].empty()
	   		)
	   	)
	   {
         // DO NOT LEAVE THIS RAW IN A SOURCE FILE!!!
	   	// MessageInterface::ShowMessage("    Ideal measurement (no hardware delay and no media correction involve):\n");

		   // Calculate uplink time and down link time: (Is it needed???)
		   uplinkTime   = uplinkRange*GmatMathConstants::KM_TO_M / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;
		   downlinkTime = downlinkRange*GmatMathConstants::KM_TO_M / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;
		   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
		      MessageInterface::ShowMessage("  Ideal measurement (no hardware delay and no media correction involve):\n");
			  MessageInterface::ShowMessage("    Uplink time = %.12lf s\n",uplinkTime);
			  MessageInterface::ShowMessage("    Downlink time = %.12lf s\n",downlinkTime);
		   #endif

		   // Calculate real range
		   Real freqFactor = GetFrequencyFactor(frequency);	// Notice that: unit of "frequency" varaibel is Hz (not MHz)
		   Real realTravelTime = uplinkTime + downlinkTime + receiveDelay + transmitDelay + targetDelay;	// unit: second
		   Real realRangeKm = 0.5 *realTravelTime * GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM/1000.0;    // unit: km
		   Real realRange = realTravelTime * freqFactor;													// unit: no unit

		   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
		      MessageInterface::ShowMessage("   Frequency = %.12lf MHz (This value is set to the default value in PhysicalMeasurement class due to no hardware used.)\n", frequency/1.0e6);
			  MessageInterface::ShowMessage("   Frequency factor = %.12lf MHz\n", freqFactor/1.0e6);
	          MessageInterface::ShowMessage("   Range in km = %.12lf km\n", realRangeKm);
			  MessageInterface::ShowMessage("   uplinkRange = %lfkm   downlinkRange = %lfkm\n", uplinkRange, downlinkRange);
			  MessageInterface::ShowMessage("   receiveDelay = %lfm   transmitDelay = %lfm   targetDelay = %lfm\n", receiveDelay*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM, transmitDelay*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM, targetDelay*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);
	          MessageInterface::ShowMessage("   Range = %.12lf (It has no unit)\n", realRange);
	       #endif

	      // Set value for currentMeasurement
		   currentMeasurement.value[0] = realRange;
	      currentMeasurement.isFeasible = true;

	      return true;
	   }

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

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
	        MessageInterface::ShowMessage("5. Sensors, delays, and signals:\n");
			MessageInterface::ShowMessage("   List of sensors: %s, %s, %s\n",
				gsTransmitter->GetName().c_str(), gsReceiver->GetName().c_str(),
				scTransponder->GetName().c_str());
		#endif


		// 6. Get transmitter, receiver, and transponder delays:
		transmitDelay = gsTransmitter->GetDelay();
		receiveDelay = gsReceiver->GetDelay();
		targetDelay = scTransponder->GetDelay();

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("   Transmitter delay = %le s\n", gsTransmitter->GetDelay());
			MessageInterface::ShowMessage("   Receiver delay = %le s\n", gsReceiver->GetDelay());
			MessageInterface::ShowMessage("   Transponder delay = %le s\n", scTransponder->GetDelay());
		#endif


		// 7. Get frequency from transmitter of ground station (participants[0])
		Signal* uplinkSignal = gsTransmitter->GetSignal();
		Real uplinkFreq = uplinkSignal->GetValue();


        // 8. Calculate media correction for uplink leg:
        #ifdef DEBUG_RANGE_CALC_WITH_EVENTS   
           MessageInterface::ShowMessage("6. Media correction for uplink leg\n");
		   MessageInterface::ShowMessage("   UpLink signal frequency = %.12lf MHz\n", uplinkFreq);
        #endif
        Real roundTripTime = ((uplinkRange + downlinkRange)*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)/GmatTimeConstants::SECS_PER_DAY;
        RealArray uplinkCorrection = CalculateMediaCorrection(uplinkFreq, r1, r2, currentMeasurement.epoch - roundTripTime);
        Real uplinkRangeCorrection = uplinkCorrection[0]/GmatMathConstants::KM_TO_M;
        Real uplinkRealRange = uplinkRange + uplinkRangeCorrection;
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("   Uplink range correction = %.12lf km\n",uplinkRangeCorrection);
			MessageInterface::ShowMessage("   Uplink real range = %.12lf km\n",uplinkRealRange);
		#endif


		// 9. Doppler shift the frequency from the transmitter using uplinkRangeRate:
	   Real uplinkDSFreq = (1 - uplinkRangeRate*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*uplinkFreq;

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
	        MessageInterface::ShowMessage("7. Transponder input and output frequencies\n");
			MessageInterface::ShowMessage("   Uplink Doppler shift frequency = %.12lf MHz\n", uplinkDSFreq);
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
			MessageInterface::ShowMessage("  Downlink frequency = %.12lf Mhz\n", downlinkFreq);
		#endif


	   // 13. Doppler shift the transponder output frequency by the downlinkRangeRate:
	   Real downlinkDSFreq = (1 - downlinkRangeRate*GmatMathConstants::KM_TO_M/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM)*downlinkFreq;

	   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("  Downlink Doppler shift frequency = %.12lf MHz\n", downlinkDSFreq);
		#endif


	   // 14. Set frequency on receiver
	   Signal* downlinkSignal = gsReceiver->GetSignal();
	   downlinkSignal->SetValue(downlinkDSFreq);


	   // 15. Check the receiver feasibility to receive the downlink signal
	   if (gsReceiver->IsFeasible() == false)
	   {
	   	 currentMeasurement.isFeasible = false;
	   	 currentMeasurement.value[0] = 0;
		 MessageInterface::ShowMessage("The receiver is unfeasible to receive downlink signal.\n");
	   	 throw new MeasurementException("The receiver is unfeasible to receive downlink signal.\n");
	   }


	   // 16. Calculate media correction for downlink leg:
      #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
         MessageInterface::ShowMessage("8. Media correction for downlink leg\n");
      #endif
	   RealArray downlinkCorrection = CalculateMediaCorrection(downlinkDSFreq, r3, r4, currentMeasurement.epoch);
	   Real downlinkRangeCorrection = downlinkCorrection[0]/GmatMathConstants::KM_TO_M;
	   Real downlinkRealRange = downlinkRange + downlinkRangeCorrection;
		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
			MessageInterface::ShowMessage("   Downlink range correction = %.12lf km\n",downlinkRangeCorrection);
			MessageInterface::ShowMessage("   Downlink real range = %.12lf km\n",downlinkRealRange);
		#endif

	   // 17. Calculate uplink time and down link time: (Is it needed???)
	   uplinkTime   = uplinkRealRange*GmatMathConstants::KM_TO_M / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;
	   downlinkTime = downlinkRealRange*GmatMathConstants::KM_TO_M / GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;

		#ifdef DEBUG_RANGE_CALC_WITH_EVENTS
	        MessageInterface::ShowMessage("9. Travel time:\n");
			MessageInterface::ShowMessage("   Uplink time = %.12lf s\n",uplinkTime);
			MessageInterface::ShowMessage("   Downlink time = %.12lf s\n",downlinkTime);
		#endif


	   // 18. Calculate real range
		//	   Real realRange = ((upRange + downRange) /
		//            (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M) +
		//            receiveDelay + transmitDelay + targetDelay) * freqFactor;

	   // Real freqFactor = GetFrequencyFactor(frequency);
	   Real freqFactor = GetFrequencyFactor(uplinkFreq*1.0e6);		// Notice that: unit of "uplinkFreq" is MHz (not Hz)
  	   Real realTravelTime = uplinkTime + downlinkTime + receiveDelay + transmitDelay + targetDelay;    // unit: second
	   Real realRangeKm = 0.5*realTravelTime * GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM/1000.0;		// unit: km
       Real realRange = realTravelTime * freqFactor;													// unit: no unit

	   #ifdef DEBUG_RANGE_CALC_WITH_EVENTS
		  MessageInterface::ShowMessage("   Frequency factor = %.12lf MHz\n", freqFactor/1.0e6);
		  MessageInterface::ShowMessage("   Calculated real range in km = %.12lf km\n", realRangeKm);
          MessageInterface::ShowMessage("   Calculated real range = %.12lf (It has no unit)\n", realRange);
       #endif



	   // 19. Set value for currentMeasurement
//	   currentMeasurement.value[0] = realRange;
      currentMeasurement.value[0] = realRangeKm;
      currentMeasurement.isFeasible = true;

      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// Real GetFrequencyFactor(Real frequency)
//------------------------------------------------------------------------------
/**
 * Constructs the multiplier used to convert into range units
 *
 * @param frequency F_T used in the computation.  The default (0.0) generates
 *                  the factor for DSN14.
 *
 * @return The factor used in the conversion.
 */
//------------------------------------------------------------------------------
Real DSNTwoWayRange::GetFrequencyFactor(Real frequency)
{
   // Default value: DSN14 factor with the S-band multiplier
   Real factor = 2090659968.0 / 2.0;

   // Map the frequency to the corresponding factor here
   // S-band
   if ((frequency >= 2000000000.0) && (frequency <= 4000000000.0))
      factor = frequency / 2.0;

   // X-band (Band values from Wikipedia; check them!
   if ((frequency >= 7900000000.0) && (frequency <= 8400000000.0))
      factor = frequency * 11.0 / 75.0;

   // Todo: Figure out how to detect HEV and BVE

   return factor;
}
