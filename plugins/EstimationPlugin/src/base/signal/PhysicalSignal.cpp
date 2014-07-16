//$Id$
//------------------------------------------------------------------------------
//                           PhysicalSignal
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
// Created: Jan 9, 2014
/**
 * Class used to model signals between two participants
 */
//------------------------------------------------------------------------------

#include "PhysicalSignal.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include "PropSetup.hpp"
#include "GroundstationInterface.hpp"

#include <sstream>                  // For stringstream


//#define DEBUG_EXECUTION
//#define DEBUG_LIGHTTIME
//#define SHOW_DATA
//#define DEBUG_DERIVATIVES
//#define DEBUG_TIMING


//------------------------------------------------------------------------------
// PhysicalSignal(const std::string &typeStr, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param typeStr The object type
 * @param name Name of the new object
 */
//------------------------------------------------------------------------------
PhysicalSignal::PhysicalSignal(const std::string &typeStr,
      const std::string &name) :
   SignalBase                 (typeStr, name),
   physicalSignalInitialized  (false)
{
}


//------------------------------------------------------------------------------
// ~PhysicalSignal()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PhysicalSignal::~PhysicalSignal()
{
}


//------------------------------------------------------------------------------
// PhysicalSignal(const PhysicalSignal& gs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ps Physical signal copied to make a new one
 */
//------------------------------------------------------------------------------
PhysicalSignal::PhysicalSignal(const PhysicalSignal& ps) :
   SignalBase                 (ps),
   physicalSignalInitialized  (false)
{
}


//------------------------------------------------------------------------------
// PhysicalSignal& operator=(const PhysicalSignal& ps)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ps PhysicalSignal that provides parameters for this one
 *
 * @return This signal set to match gs
 */
//------------------------------------------------------------------------------
PhysicalSignal& PhysicalSignal::operator=(const PhysicalSignal& ps)
{
   if (this != &ps)
   {
      GmatBase::operator=(ps);
      physicalSignalInitialized = false;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* PhysicalSignal::Clone() const
//------------------------------------------------------------------------------
/**
 * Cloning method used to make a replica of a PhysicalSignal.
 *
 * @return A new PhysicalSignal configured like this one.
 */
//------------------------------------------------------------------------------
GmatBase* PhysicalSignal::Clone() const
{
   return new PhysicalSignal(*this);
}


//------------------------------------------------------------------------------
// void InitializeSignal()
//------------------------------------------------------------------------------
/**
 * Validates that everything needed is in place for the signal processing
 *
 * This method checks that all of the reference objects and object clones are
 * in place, and that they are initialized and ready to do the work required for
 * the signal computations.
 */
//------------------------------------------------------------------------------
void PhysicalSignal::InitializeSignal(bool chainForwards)
{
   if (!physicalSignalInitialized)
   {
      SignalBase::InitializeSignal(chainForwards);
      physicalSignalInitialized = true;
   }
}


//------------------------------------------------------------------------------
// bool ModelSignal(const GmatEpoch atEpoch, bool epochAtReceive)
//------------------------------------------------------------------------------
/**
 * Models the signal
 *
 * The call here requires that the raw data in the measurement data is set
 * correctly when the call is made.  Specifically, the call into this method
 * Requires the following setup prior to the call (a t prefix refers to the
 * signal transmitting participant; r to the signal receiving participant):
 *
 *    theData.tTime: For Spacecraft, set to the epoch matching the state known
 *                   in the associated propagator.  For groundstations, set to
 *                   the epoch of the receiver.  Station to station signals are
 *                   not supported (unless this piece is changed)
 *
 *    theData.rTime: For Spacecraft, set to the epoch matching the state known
 *                   in the associated propagator.  For groundstations, set to
 *                   the epoch of the receiver.  Station to station signals are
 *                   not supported (unless this piece is changed)
 *
 * The data needed in theData.tLoc, theData.tVel, theData.rLoc, and theData.rVel
 * are populated during execution of this method.
 *
 * @param atEpoch The base epoch of the signal.  This is the epoch at the node
 *                selected by epochAtReceive.
 * @param epochAtReceive true if the receive node is fixed in time, false if the
 *                       transmit node is fixed
 *
 * @return true if the signal was modeled, false if not
 */
//------------------------------------------------------------------------------
bool PhysicalSignal::ModelSignal(const GmatEpoch atEpoch, bool epochAtReceive)
{
   bool retval = false;
   satEpoch = atEpoch;

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("ModelSignal(%.12lf, %s) called\n", atEpoch,
            epochAtReceive ? "with fixed Receiver" : "with fixed Transmitter");

      MessageInterface::ShowMessage("Modeling %s -> %s\n",
            theData.transmitParticipant.c_str(),
            theData.receiveParticipant.c_str());

      MessageInterface::ShowMessage("tTime = %.12lf, rTime = %.12lf satEpoch = "
            "%.12lf\n", theData.tTime, theData.rTime, satEpoch);
   #endif

   if (!isInitialized)
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("   Calling signal initialization\n");
      #endif
      InitializeSignal(!epochAtReceive);
   }

   if (isInitialized)
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("   Signal initialized; Computing data\n");
      #endif

      // First make sure we start at the desired epoch
      MoveToEpoch(satEpoch, epochAtReceive, true);
      CalculateRangeVectorInertial();

      if (includeLightTime)
      {
         GenerateLightTimeData(satEpoch, epochAtReceive);
      }
      else
      {
         // Build the other data vectors
         CalculateRangeVectorObs();
         CalculateRangeRateVectorObs();
      }

      // Perform feasibility check
      if (theData.stationParticipant)
      {
         const Real* elData;
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         {
            Rvector6 state_sez(theData.rangeVecObs,
                  theData.rangeRateVecObs);
            elData = ((GroundstationInterface*)(theData.tNode))->
                  IsValidElevationAngle(state_sez);
         }

         if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         {
            Rvector6 state_sez(-theData.rangeVecObs,
                  -theData.rangeRateVecObs);
            elData = ((GroundstationInterface*)(theData.rNode))->
                  IsValidElevationAngle(state_sez);
         }

         signalIsFeasible = (elData[2] >= 1);

         #ifdef DEBUG_FEASIBILITY
            MessageInterface::ShowMessage("Obs vector = [%.3lf %.3lf %.3lf] "
                  "so %s\n", theData.rangeVecObs(0), theData.rangeVecObs(1),
                  theData.rangeVecObs(2), (signalIsFeasible ? "feasible" :
                  "infeasible"));
         #endif
      }
      else
      ///@todo: Put in test for obstructing bodies; for now, always feasible
      {
         signalIsFeasible = true;
      }

      // Report raw data
      if (navLog)
      {
         std::stringstream data;
         data.precision(16);

         if (logLevel <= 1)
         {
            Real range = theData.rangeVecInertial.GetMagnitude();
            if (range >= 0.0)
               data << "   " << GetPathDescription(false)
                    << " Range at A.1 epoch " << satEpoch
                    << " = " << range << "\n";
            else
               data << "   Range not valid\n";
         }

         if (logLevel == 0)
         {
            data.str() = "";
            data << "      Range vector:         "
                 << (theData.rangeVecInertial.ToString())
                 << "      Range vector Obs:     "
                 << (theData.rangeVecObs.ToString())
                 << "      RangeRate vector Obs: "
                 << (theData.rangeRateVecObs.ToString())
                 << "\n      Transmitter location: "
                 << (theData.tLoc.ToString())
                 << "      Receiver location:    "
                 << (theData.rLoc.ToString());
         }
         navLog->WriteData(data.str());
      }

      // if epoctAtReceive was true, transmitter moved and we need its epoch,
      // if false, we need the receiver epoch
      GmatEpoch nextEpoch = (epochAtReceive ? theData.tTime : theData.rTime);

      // This transmitter is the receiver for the next node
      bool nextFixed = (epochAtReceive ? true : false);

      bool nodePassed = true;

      if (epochAtReceive)
      {
         if (previous)
         {
            previous->SetSignalData(theData);

            /// @todo: If there is a transponder delay, apply it here, moving
            /// nextEpoch back by the delay time
            nodePassed = previous->ModelSignal(nextEpoch, nextFixed);
         }
      }
      else
      {
         if (next)
         {
            next->SetSignalData(theData);

            /// @todo: If there is a transponder delay, apply it here, moving
            /// nextEpoch ahead by the delay time
            nodePassed = next->ModelSignal(nextEpoch, nextFixed);
         }
      }

      retval = nodePassed;
   }

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("ModelSignal() call complete\n");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// const std::vector<RealArray>& ModelSignalDerivative(GmatBase* obj,
//       Integer forId)
//------------------------------------------------------------------------------
/**
 * Generates the derivative data for the signal path
 *
 * This method builds the derivative data for the current signal path by adding
 * together the data signal by signal in the theDataDerivatives data vector.
 *
 * @param obj The object supplying the "with respect to" parameter
 * @param id The ID for the "with respect to" parameter
 *
 * @return The vector od derivative data for this signal path
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& PhysicalSignal::ModelSignalDerivative(
      GmatBase* obj, Integer forId)
{
   theDataDerivatives.clear();

   if (logLevel < 2)
   {
      std::stringstream msg;
      msg << "Derivative computations performed for the "
            << GetPathDescription(false) << " Signal" << "\n";
      navLog->WriteData(msg.str());
   }

   Integer size = obj->GetEstimationParameterSize(forId);
   if (next)
   {
      if (logLevel < 2)
         navLog->WriteData("   Accessing a 'next' node\n");
      // Collect the data from the "next" node
      theDataDerivatives = next->ModelSignalDerivative(obj, forId);
      if (logLevel < 2)
         navLog->WriteData("   Access complete\n");
   }
   else
   {
      // Set up the vector for the data
      if (size <= 0)
         throw MeasurementException("The derivative parameter on derivative "
               "object " + obj->GetName() + "is not recognized");

      RealArray oneRow;
      oneRow.assign(size, 0.0);
      theDataDerivatives.push_back(oneRow);
   }

   // Check to see if obj is a participant
   GmatBase *objPtr = NULL;
   if (theData.tNode == obj)
      objPtr = theData.tNode;
   if (theData.rNode == obj)
      objPtr = theData.rNode;

   Integer parameterID = -1;
   if (objPtr != NULL)
   {
      if (forId > 250)
         parameterID = GetParmIdFromEstID(forId, obj);
      else
         parameterID = forId;

      if (objPtr->GetParameterText(parameterID) == "Position")
      {
         Rvector3 result;
         GetRangeDerivative(objPtr, true, false, result);

         for (UnsignedInt jj = 0; jj < 3; ++jj)
            theDataDerivatives[0][jj] = result[jj];
      }
      else if (objPtr->GetParameterText(parameterID) == "Velocity")
      {
         Rvector3 result;
         GetRangeDerivative(objPtr, false, true, result);

         for (UnsignedInt jj = 0; jj < 3; ++jj)
            theDataDerivatives[0][jj] = result[jj];
      }
      else if (objPtr->GetParameterText(parameterID) == "CartesianX")
      {
         Rvector6 result;
         GetRangeDerivative(objPtr, true, true, result);

         for (UnsignedInt jj = 0; jj < 6; ++jj)
            theDataDerivatives[0][jj] = result[jj];
      }
      else if (objPtr->GetParameterText(parameterID) == "Bias")
      {
         for (Integer i = 0; i < size; ++i)
            theDataDerivatives[0][i] += 1.0;
      }
      else
      {
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. something "
                     "independent, so zero\n");
         #endif
         for (UnsignedInt i = 0; i < 3; ++i)
            theDataDerivatives[0][i] += 0.0;
      }
   }

   if ((parameterID >= 0) && (logLevel < 2))
   {
      if (logLevel == 0)
      {
         std::stringstream msg;
         msg.precision(15);
         msg << "   Derivative is w.r.t " << obj->GetName()
                << "." << obj->GetParameterText(parameterID) << "\n";
         for (UnsignedInt i = 0; i < theDataDerivatives.size(); ++i)
         {
            msg << "      " << i << ":  [";
            for (UnsignedInt j = 0; j < theDataDerivatives[i].size(); ++j)
            {
               if (j > 0)
                  msg << ", ";
               msg << theDataDerivatives[i][j];
            }
            msg << "]\n";
         }

         navLog->WriteData(msg.str());
      }
   }

   return theDataDerivatives;
}


//------------------------------------------------------------------------------
// bool GenerateLightTimeData(const GmatEpoch atEpoch,const bool epochAtReceive)
//------------------------------------------------------------------------------
/**
 * Iterates propagation to generate a light time solution
 *
 * @param atEpoch Epoch for the fixed point state
 * @param epochAtReceive Flag indicating that the receiver is held fixed
 *
 * @return true is light time data was generated, false if there was a failure
 *         that did not throw
 */
//------------------------------------------------------------------------------
bool PhysicalSignal::GenerateLightTimeData(const GmatEpoch atEpoch,
      const bool epochAtReceive)
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Called GenerateLightTimeData(%.12lf, "
            "%s)\n", atEpoch, (epochAtReceive ? "Receiver fixed" :
            "Transmitter fixed"));
   #endif

   bool retval = false;

   if (includeLightTime)
   {
      // First make sure we start at the desired epoch
      MoveToEpoch(atEpoch, epochAtReceive, true);

      // Then compute the initial data
      Rvector3 displacement = theData.rLoc - theData.tLoc;
      Real deltaR = displacement.GetMagnitude();
      Real deltaT = (epochAtReceive ? -1.0 : 1.0) * deltaR /
            (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / 1000.0);

      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   DeltaT for light travel over "
               "distance %.3lf km = %le\n", deltaR, deltaT);
      #endif

      // Here we go; iterating for a light time solution
      Integer loopCount = 0;

      // Epoch difference, in seconds
      Real deltaE = (theData.rTime - theData.tTime) * GmatTimeConstants::SECS_PER_DAY;

      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("      Starting: dEpoch = %.12le, dR = "
               "%.3lf, dT = %.12le\n", deltaE, deltaR, deltaT);
         MessageInterface::ShowMessage("Initial x Positions: %s  %.3lf -->  "
               "%s  %.3lf\n", theData.tNode->GetName().c_str(), theData.tLoc(0),
               theData.rNode->GetName().c_str(), theData.rLoc(0));
      #endif

      // Loop to half microsecond precision or 10 times, whichever comes first
      while ((GmatMathUtil::Abs(deltaE - deltaT) > 5e-7) && (loopCount < 10))
      {
         #ifdef DEBUG_LIGHTTIME
            MessageInterface::ShowMessage("      Loop iteration %d\n",
                  loopCount);
         #endif
         MoveToEpoch(atEpoch + deltaT / GmatTimeConstants::SECS_PER_DAY,
               !epochAtReceive, false);
         deltaE = (epochAtReceive ? -1.0 : 1.0) *
               (theData.rTime - theData.tTime) * GmatTimeConstants::SECS_PER_DAY;
         displacement = theData.rLoc - theData.tLoc;

         #ifdef DEBUG_LIGHTTIME
            MessageInterface::ShowMessage("x Positions: %s  %.3lf -->  %s  "
                  "%.3lf\n", theData.tNode->GetName().c_str(), theData.tLoc(0),
                  theData.rNode->GetName().c_str(), theData.rLoc(0));
         #endif

         deltaR = displacement.GetMagnitude();
         deltaT = (epochAtReceive ? -1.0 : 1.0) * deltaR /
                     (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / 1000.0);
         #ifdef DEBUG_LIGHTTIME
            MessageInterface::ShowMessage("      ===> dEpoch = %.12le, dR = "
                  "%.3lf, dT = %.12le, trigger = %le\n", deltaE, deltaR, deltaT,
                  deltaE-deltaT);
         #endif
         ++loopCount;
      }
   }

   // Temporary check on data flow
   // Build the other data vectors
   CalculateRangeVectorObs();
   CalculateRangeRateVectorObs();

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Leaving GenerateLightTimeData\n");
   #endif


   return retval;
}
