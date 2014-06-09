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
 * Class used to model instantaneous signals
 */
//------------------------------------------------------------------------------

#include "PhysicalSignal.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include <sstream>                  // For stringstream


//#define DEBUG_EXECUTION
#define SHOW_DATA


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
   SignalBase           (typeStr, name)
{
}


//------------------------------------------------------------------------------
// PhysicalSignal::~PhysicalSignal()
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
 * @param gs Geometric signal copied to make a new one
 */
//------------------------------------------------------------------------------
PhysicalSignal::PhysicalSignal(const PhysicalSignal& gs) :
   SignalBase           (gs)
{
}


//------------------------------------------------------------------------------
// PhysicalSignal& operator=(const PhysicalSignal& gs)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param gs PhysicalSignal that provides parameters for this one
 *
 * @return This signal set to match gs
 */
//------------------------------------------------------------------------------
PhysicalSignal& PhysicalSignal::operator=(const PhysicalSignal& gs)
{
   if (this != &gs)
   {
      GmatBase::operator=(gs);
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
// bool ModelSignal(bool epochAtReceive)
//------------------------------------------------------------------------------
/**
 * Models the signal
 *
 * @param epochAtReceive true if the receive node is fixed in time, false if the
 *                       transmit node is fixed
 *
 * @return true if the signal was modeled, false if not
 */
//------------------------------------------------------------------------------
bool PhysicalSignal::ModelSignal(bool epochAtReceive)
{
   bool retval = false;

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("      ModelSignal() called, %s\n",
            (isInitialized ? "Already initialized" : "Initializing"));
   #endif

   if (!isInitialized)
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("   Calling signal initialization\n");
      #endif
      InitializeSignal();
   }

   if (isInitialized)
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("   Signal initialized; Computing data\n");
      #endif

      CalculateRangeVectorInertial();
      CalculateRangeVectorObs();
      CalculateRangeRateVectorObs();

      // Perform feasibility check
      if (theData.stationParticipant)
      {
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         {
            if (theData.rangeVecObs[2] > 0)
               signalIsFeasible = true;
            else
               signalIsFeasible = false;
         }
         if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         {
            if (-theData.rangeVecObs[2] > 0)
               signalIsFeasible = true;
            else
               signalIsFeasible = false;
         }
         #ifdef DEBUG_DEASIBILITY
            MessageInterface::ShowMessage("Obs vector = [%s] so %s\n",
                  theData.rangeVecObs.ToString().c_str(),
                  (signalIsFeasible ? "feasible" : "infeasible"));
         #endif
      }
      else
      ///@todo: Put in test for obscuring bodies; for now, always feasible
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

      if (next)
         next->ModelSignal();

      retval = true;
   }

   return retval;
}

//------------------------------------------------------------------------------
// const std::vector<RealArray>& PhysicalSignal::ModelSignalDerivative(
//       GmatBase* obj, Integer forId)
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
      parameterID = GetParmIdFromEstID(forId, obj);

      if (objPtr->GetParameterText(parameterID) == "Position")
      {
         CalculateRangeVectorInertial();
         Rvector3 tmp, result;
         Rvector3 rangeUnit = theData.rangeVecInertial.GetUnitVector();

         //if (theData.stationParticipant)
         // Not sure if this piece is the right way to do this
         if (objPtr->IsOfType(Gmat::GROUND_STATION))
         {
            for (UnsignedInt i = 0; i < 3; ++i)
               tmp[i] = - rangeUnit[i];

            // for a Ground Station, need to rotate to the F1 frame
            result = tmp * (theData.tNode->IsOfType(Gmat::GROUND_STATION) ?
                  R_j2k_Transmitter : R_j2k_Receiver);
            for (UnsignedInt jj = 0; jj < 3; jj++)
               theDataDerivatives[0][jj] += result[jj];
         }
         else
         {
            // for a spacecraft participant 1, we don't need the rotation matrices (I33)
            for (UnsignedInt i = 0; i < 3; ++i)
               theDataDerivatives[0][i] += -rangeUnit[i];
         }
      }
      else if (objPtr->GetParameterText(parameterID) == "Velocity")
      {
         for (UnsignedInt i = 0; i < 3; ++i)
            theDataDerivatives[0][i] += 0.0;
      }
      else if (objPtr->GetParameterText(parameterID) == "CartesianX")
      {
         CalculateRangeVectorInertial();
         Rvector3 tmp, result;
         Rvector3 rangeUnit = theData.rangeVecInertial.GetUnitVector();

         //if (theData.stationParticipant)
         // Not sure if this piece is the right way to do this
         if (objPtr->IsOfType(Gmat::GROUND_STATION))
         {
            for (UnsignedInt i = 0; i < 3; ++i)
               tmp[i] = - rangeUnit[i];

            // for a Ground Station, need to rotate to the F1 frame
            result = tmp * (theData.tNode->IsOfType(Gmat::GROUND_STATION) ?
                  R_j2k_Transmitter : R_j2k_Receiver);
            for (UnsignedInt jj = 0; jj < 3; jj++)
               theDataDerivatives[0][jj] += result[jj];
         }
         else
         {
            // for a spacecraft participant 1, we don't need the rotation matrices (I33)
            for (UnsignedInt i = 0; i < 3; ++i)
               theDataDerivatives[0][i] += - rangeUnit[i];
         }
         // velocity all zeroes
         for (UnsignedInt ii = 3; ii < 6; ii++)
            theDataDerivatives[0][ii] += 0.0;
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
