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
#include "Barycenter.hpp"
#include "Spacecraft.hpp"
#include "Transmitter.hpp"
#include "Transponder.hpp"
#include "Receiver.hpp"

#include <sstream>                  // For stringstream


//#define DEBUG_EXECUTION
//#define DEBUG_LIGHTTIME
//#define SHOW_DATA
//#define DEBUG_FEASIBILITY
//#define DEBUG_DERIVATIVES
//#define DEBUG_TIMING
//#define DEBUG_CONSTRUCTION
//#define DEBUG_MEASUREMENT_CORRECTION
//#define DEBUG_IONOSPHERE_MEDIA_CORRECTION
//#define DEBUG_TROPOSPHERE_MEDIA_CORRECTION
//#define DEBUG_RELATIVITY_CORRECTION
//#define DEBUG_RANGE_CALCULATION

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
   physicalSignalInitialized  (false),
   troposphere                (NULL),
#ifdef IONOSPHERE    // Required until the f2c issues for Mac and Linux have been resolved
   ionosphere                 (NULL),
#endif
   useRelativity              (false),
   useETTAI                   (false)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PhysicalSignal:: default construction\n");
#endif
   rampTable = NULL;
   beginIndex = 0;
   endIndex = 0;
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
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PhysicalSignal:: destruction\n");
#endif

   if (troposphere != NULL)
      delete troposphere;

#ifdef IONOSPHERE    // Required until the f2c issues for Mac and Linux have been resolved
   if (ionosphere != NULL)
      delete troposphere;
#endif
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
   physicalSignalInitialized  (false),
   useRelativity              (ps.useRelativity),
   relCorrection              (ps.relCorrection),
   useETTAI                   (ps.useETTAI),
#ifdef IONOSPHERE
   troposphere                (NULL),
   ionosphere                 (NULL)
#else
   troposphere                (NULL)
#endif
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PhysicalSignal:: copy construction\n");
#endif

//   if (troposphere !=NULL)
//      delete troposphere;
//   troposphere = (Troposphere*)(ps.troposphere->Clone());
//
//#ifdef IONOSPHERE    // Required until the f2c issues for Mac and Linux have been resolved
//   if (ionosphere !=NULL)
//      delete ionosphere;
//   ionosphere = (Ionosphere*)(ps.ionosphere->Clone());
//#endif
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
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PhysicalSignal::operator =\n");
#endif

   if (this != &ps)
   {
      GmatBase::operator=(ps);
      physicalSignalInitialized = false;
      useRelativity             = ps.useRelativity;

      relCorrection             = ps.relCorrection;
      useETTAI                  = ps.useETTAI;

//      if (troposphere !=NULL)
//         delete troposphere;
//      troposphere = (Troposphere*)(ps.troposphere->Clone());
//
//#ifdef IONOSPHERE    // Required until the f2c issues for Mac and Linux have been resolved
//      if (ionosphere !=NULL)
//         delete ionosphere;
//      ionosphere = (Ionosphere*)(ps.ionosphere->Clone());
//#endif
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
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("PhysicalSignal::Clone()\n");
#endif

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
#ifndef USE_PRECISION_TIME
// This function will be removed and replaced by bool PhysicalSignal::ModelSignal(const GmatTime atEpoch, bool epochAtReceive)
bool PhysicalSignal::ModelSignal(const GmatEpoch atEpoch, bool epochAtReceive)
{
   bool retval = false;
   satEpoch = atEpoch;
   relCorrection = 0.0;
   ettaiCorrection = 0.0;                                                                     // unit: km
   
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("ModelSignal(%.12lf, %s) called\n", atEpoch,
            epochAtReceive ? "with fixed Receiver" : "with fixed Transmitter");

      MessageInterface::ShowMessage("Modeling %s -> %s\n",
            theData.transmitParticipant.c_str(),
            theData.receiveParticipant.c_str());

      MessageInterface::ShowMessage("tTime = %.12lf, rTime = %.12lf satEpoch = "
            "%.12lf\n", theData.tTime, theData.rTime, satEpoch);
   #endif

   #ifdef DEBUG_RANGE_CALCULATION 
      MessageInterface::ShowMessage("   +++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
      MessageInterface::ShowMessage("   ++++    For leg from %s to %s :\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
      MessageInterface::ShowMessage("   +++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
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
      
      // 1. First make sure we start at the desired epoch
      MoveToEpoch(satEpoch, epochAtReceive, true);
      CalculateRangeVectorInertial();
      Real geoRange = theData.rangeVecInertial.GetMagnitude();
     
      #ifdef DEBUG_RANGE_CALCULATION 
         MessageInterface::ShowMessage("   1. Compute Range Vector before light time correction for the Leg from <TNode = %s> to <RNode = %s>:\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
         MessageInterface::ShowMessage("      . %s position in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km at time tT= %.12lf\n", theData.tNode->GetName().c_str(), tcs->GetOriginName().c_str(), theData.tLoc[0], theData.tLoc[1], theData.tLoc[2], theData.tTime);
         MessageInterface::ShowMessage("      . %s position in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km at time tR = %.12lf\n", theData.rNode->GetName().c_str(), rcs->GetOriginName().c_str(), theData.rLoc[0], theData.rLoc[1], theData.rLoc[2], theData.rTime);
         Rvector3 tCSorigin =  theData.tOStateSSB.GetR();
         Rvector3 rCSorigin =  theData.rOStateSSB.GetR();
         Rvector3 tLocSSB = theData.tLoc + theData.tOStateSSB.GetR();
         Rvector3 rLocSSB = theData.rLoc + theData.rOStateSSB.GetR();
         MessageInterface::ShowMessage("      . %s in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tT = %.12lf\n", theData.tNode->GetName().c_str(), tLocSSB[0], tLocSSB[1], tLocSSB[2], theData.tTime);
         MessageInterface::ShowMessage("      . %s in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tR = %.12lf\n", theData.rNode->GetName().c_str(), rLocSSB[0], rLocSSB[1], rLocSSB[2], theData.rTime);
         MessageInterface::ShowMessage("      . Range vector in SSBMJ2000: (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecInertial[0], theData.rangeVecInertial[1], theData.rangeVecInertial[2]);
         MessageInterface::ShowMessage("      . Range vector in obs CS   : (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecObs[0], theData.rangeVecObs[1], theData.rangeVecObs[2]);
         MessageInterface::ShowMessage("      . R_Obs_j2k matrix  : (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(0,0), R_Obs_j2k(0,1), R_Obs_j2k(0,2));
         MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(1,0), R_Obs_j2k(1,1), R_Obs_j2k(1,2));
         MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(2,0), R_Obs_j2k(2,1), R_Obs_j2k(2,2));
      #endif


      // 2. Compute light time solution if it is needed and solve for range vector
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
     
      #ifdef DEBUG_RANGE_CALCULATION 
         MessageInterface::ShowMessage("   2. Compute Range Vector after light time correction for the Leg from <TNode = %s> to <RNode = %s>:\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
         MessageInterface::ShowMessage("      . %s position in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km at time tT= %.12lf\n", theData.tNode->GetName().c_str(), tcs->GetOriginName().c_str(), theData.tLoc[0], theData.tLoc[1], theData.tLoc[2], theData.tTime);
         MessageInterface::ShowMessage("      . %s position in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km at time tR = %.12lf\n", theData.rNode->GetName().c_str(), rcs->GetOriginName().c_str(), theData.rLoc[0], theData.rLoc[1], theData.rLoc[2], theData.rTime);
         tCSorigin =  theData.tOStateSSB.GetR();
         rCSorigin =  theData.rOStateSSB.GetR();
         tLocSSB = theData.tLoc + theData.tOStateSSB.GetR();
         rLocSSB = theData.rLoc + theData.rOStateSSB.GetR();
         MessageInterface::ShowMessage("      . %s in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tT = %.12lf\n", theData.tNode->GetName().c_str(), tLocSSB[0], tLocSSB[1], tLocSSB[2], theData.tTime);
         MessageInterface::ShowMessage("      . %s in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tR = %.12lf\n", theData.rNode->GetName().c_str(), rLocSSB[0], rLocSSB[1], rLocSSB[2], theData.rTime);
         MessageInterface::ShowMessage("      . Range vector in SSBMJ2000: (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecInertial[0], theData.rangeVecInertial[1], theData.rangeVecInertial[2]);
         MessageInterface::ShowMessage("      . Range vector in obs CS   : (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecObs[0], theData.rangeVecObs[1], theData.rangeVecObs[2]);
         MessageInterface::ShowMessage("      . R_Obs_j2k matrix  : (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(0,0), R_Obs_j2k(0,1), R_Obs_j2k(0,2));
         MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(1,0), R_Obs_j2k(1,1), R_Obs_j2k(1,2));
         MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(2,0), R_Obs_j2k(2,1), R_Obs_j2k(2,2));
      #endif

      // 3. Calculate ET-TAI correction for this signal leg
      if (useETTAI)
      {
         // Compute ET-TAI at trasnmite node
         Real tETTAI = ETminusTAI(theData.tTime, theData.tNode);
         // Compute ET-TAI at receive node
         Real rETTAI = ETminusTAI(theData.rTime, theData.rNode);
         // Compute ET-TAI correction for this signal leg
         ettaiCorrection = (tETTAI - rETTAI)*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM;      // unit: km
         UnsignedInt i = 0;
         for (; i < theData.correctionIDs.size(); ++i)
         {
            if (theData.correctionIDs[i] == "ET-TAI")
               break;
         }
         theData.corrections[i] = ettaiCorrection;                                                         // unit: km
      }
     
      // 4. Perform feasibility check
      if (theData.stationParticipant)
      {
         const Real* elData;
         bool signalIsFeasibleT, signalIsFeasibleR;
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         {
            Rvector6 state_sez(theData.rangeVecObs,
                  theData.rangeRateVecObs);
            elData = ((GroundstationInterface*)(theData.tNode))->
                  IsValidElevationAngle(state_sez);
            signalIsFeasibleT = (elData[2] > 0.0);
            theData.feasibility = signalIsFeasibleT;
            if (!theData.feasibility)
               theData.feasibilityReason = "B";
            theData.feasibilityValue = elData[0];

            #ifdef DEBUG_FEASIBILITY
            MessageInterface::ShowMessage("At transmit node: Obs vector = [%.3lf %.3lf %.3lf] "
                  "so %s\n", theData.rangeVecObs(0), theData.rangeVecObs(1),
                  theData.rangeVecObs(2), (signalIsFeasibleT ? "feasible" :
                  "infeasible"));
            #endif

         }

         if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         {
            Rvector6 state_sez(-theData.rangeVecObs,
                  -theData.rangeRateVecObs);
            elData = ((GroundstationInterface*)(theData.rNode))->
                  IsValidElevationAngle(state_sez);
            signalIsFeasibleR = (elData[2] > 0.0);
            theData.feasibility = signalIsFeasibleR;
            if (!theData.feasibility)
               theData.feasibilityReason = "B";
            theData.feasibilityValue = elData[0];

            #ifdef DEBUG_FEASIBILITY
            MessageInterface::ShowMessage("At receive node: Obs vector = [%.3lf %.3lf %.3lf] "
                  "so %s\n", theData.rangeVecObs(0), theData.rangeVecObs(1),
                  theData.rangeVecObs(2), (signalIsFeasibleR ? "feasible" :
                  "infeasible"));
            #endif
         }

         signalIsFeasible = signalIsFeasibleT && signalIsFeasibleR;

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

      #ifdef DEBUG_RANGE_CALCULATION 
         MessageInterface::ShowMessage("   3. Summary of signal leg from %s to %s:\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());

         MessageInterface::ShowMessage("      . Geometric range       = %.12lf km\n", geoRange);
         if (includeLightTime)
         {
            Real lightTimeRange = theData.rangeVecInertial.GetMagnitude();
            MessageInterface::ShowMessage("      . Light time solution range = %.12lf km\n", lightTimeRange);
         }
         MessageInterface::ShowMessage("      . Relativity correction = %.12lf km\n", relCorrection);
         MessageInterface::ShowMessage("      . ET-TAI correction     = %.12lf km\n", ettaiCorrection);
         MessageInterface::ShowMessage("      . Feasibility           = %s\n", (signalIsFeasible?"true":"false"));
      #endif
      
      // 5. Report raw data
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
      
      // 6. Run ModelSignal for the next leg in signal path:
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
#else

bool PhysicalSignal::ModelSignal(const GmatTime atEpoch, bool epochAtReceive)
{
   bool retval = false;
   
   //satPrecEpoch = atEpoch;
   if (epochAtReceive)
   {
      // time tag is at the end of signal path
      if (next == NULL)
      {
         // for the last signal leg, rDelay = hardware delay
         satPrecEpoch = atEpoch - theData.rDelay/GmatTimeConstants::SECS_PER_DAY;
      }
      else
      {
         // for other leg, rDelay = 1/2 hardware delay
         satPrecEpoch = atEpoch - 2*theData.rDelay/GmatTimeConstants::SECS_PER_DAY;
      }
   }
   else
   {
      // time tag is at the beginning of signal path
      if (previous == NULL)
      {
         // for the first signal leg, tDelay = hardware delay
         satPrecEpoch = atEpoch + theData.tDelay/GmatTimeConstants::SECS_PER_DAY;
      }
      else
      {
         // for other leg, tDelay = 1/2 hardware delay
         satPrecEpoch = atEpoch + 2*theData.tDelay/GmatTimeConstants::SECS_PER_DAY;
      }
   }


   relCorrection = 0.0;
   ettaiCorrection = 0.0;                                                                       // unit: km

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("ModelSignal(%.12lf, %s) called\n", satPrecEpoch.GetMjd(),
            epochAtReceive ? "with fixed Receiver" : "with fixed Transmitter");

      MessageInterface::ShowMessage("Modeling %s -> %s\n",
            theData.transmitParticipant.c_str(),
            theData.receiveParticipant.c_str());

      MessageInterface::ShowMessage("tPrecTime = %.12lf, rPrecTime = %.12lf satPrecEpoch = "
            "%.12lf\n", theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd(), satPrecEpoch.GetMjd());
   #endif

   #ifdef DEBUG_RANGE_CALCULATION 
      GmatTime tm = atEpoch;
      MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
      MessageInterface::ShowMessage("   ++++   Range, relativity correction, and ET-TAI correction calculation for leg from %s to %s at fixed time %.12lf A1Mjd:\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str(), tm.GetMjd());
      MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
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
      
      // 1. First make sure we start at the desired epoch
      MoveToEpoch(satPrecEpoch, epochAtReceive, true);
      CalculateRangeVectorInertial();
      Real geoRange = theData.rangeVecInertial.GetMagnitude();

      // Build the other data vectors
      CalculateRangeVectorObs();
      CalculateRangeRateVectorObs();

      #ifdef DEBUG_RANGE_CALCULATION 
         MessageInterface::ShowMessage("   1. Compute Range Vector before light time correction for the Leg from <TNode = %s> to <RNode = %s>:\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
         MessageInterface::ShowMessage("      . %s position in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km at time tT= %.12lf\n", theData.tNode->GetName().c_str(), tcs->GetOriginName().c_str(), theData.tLoc[0], theData.tLoc[1], theData.tLoc[2], theData.tPrecTime.GetMjd());
         MessageInterface::ShowMessage("      . %s position in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km at time tR = %.12lf\n", theData.rNode->GetName().c_str(), rcs->GetOriginName().c_str(), theData.rLoc[0], theData.rLoc[1], theData.rLoc[2], theData.rPrecTime.GetMjd());
         Rvector3 tCSorigin =  theData.tOStateSSB.GetR();
         Rvector3 rCSorigin =  theData.rOStateSSB.GetR();
         Rvector3 tLocSSB = theData.tLoc + theData.tOStateSSB.GetR();
         Rvector3 rLocSSB = theData.rLoc + theData.rOStateSSB.GetR();
         MessageInterface::ShowMessage("      . %s in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tT = %.12lf\n", theData.tNode->GetName().c_str(), tLocSSB[0], tLocSSB[1], tLocSSB[2], theData.tPrecTime.GetMjd());
         MessageInterface::ShowMessage("      . %s in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tR = %.12lf\n", theData.rNode->GetName().c_str(), rLocSSB[0], rLocSSB[1], rLocSSB[2], theData.rPrecTime.GetMjd());
         MessageInterface::ShowMessage("      . Range vector in SSBMJ2000: (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecInertial[0], theData.rangeVecInertial[1], theData.rangeVecInertial[2]);
         MessageInterface::ShowMessage("      . Range vector in obs CS   : (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecObs[0], theData.rangeVecObs[1], theData.rangeVecObs[2]);
         MessageInterface::ShowMessage("      . R_Obs_j2k matrix  : (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(0,0), R_Obs_j2k(0,1), R_Obs_j2k(0,2));
         MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(1,0), R_Obs_j2k(1,1), R_Obs_j2k(1,2));
         MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(2,0), R_Obs_j2k(2,1), R_Obs_j2k(2,2));
      #endif
      
      // 2. Compute light time solution if it is needed and solve for range vector
      if (includeLightTime)
      {
         GenerateLightTimeData(satPrecEpoch, epochAtReceive);
      }


      #ifdef DEBUG_RANGE_CALCULATION 
         MessageInterface::ShowMessage("   2. Compute Range Vector after light time correction for the Leg from <TNode = %s> to <RNode = %s>:\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
         MessageInterface::ShowMessage("      . %s position in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km at time tT= %.12lf\n", theData.tNode->GetName().c_str(), tcs->GetOriginName().c_str(), theData.tLoc[0], theData.tLoc[1], theData.tLoc[2], theData.tPrecTime.GetMjd());
         MessageInterface::ShowMessage("      . %s position in %sMJ2000 cs : (%.12lf,   %.12lf,   %.12lf)km at time tR = %.12lf\n", theData.rNode->GetName().c_str(), rcs->GetOriginName().c_str(), theData.rLoc[0], theData.rLoc[1], theData.rLoc[2], theData.rPrecTime.GetMjd());
         tCSorigin =  theData.tOStateSSB.GetR();
         rCSorigin =  theData.rOStateSSB.GetR();
         tLocSSB = theData.tLoc + theData.tOStateSSB.GetR();
         rLocSSB = theData.rLoc + theData.rOStateSSB.GetR();
         MessageInterface::ShowMessage("      . %s in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tT = %.12lf\n", theData.tNode->GetName().c_str(), tLocSSB[0], tLocSSB[1], tLocSSB[2], theData.tPrecTime.GetMjd());
         MessageInterface::ShowMessage("      . %s in SSBMJ2000 : (%.12lf,   %.12lf,   %.12lf)km at time tR = %.12lf\n", theData.rNode->GetName().c_str(), rLocSSB[0], rLocSSB[1], rLocSSB[2], theData.rPrecTime.GetMjd());
         MessageInterface::ShowMessage("      . Range vector in SSBMJ2000: (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecInertial[0], theData.rangeVecInertial[1], theData.rangeVecInertial[2]);
         MessageInterface::ShowMessage("      . Range vector in obs CS   : (%.12lf,   %.12lf,   %.12lf)km\n", theData.rangeVecObs[0], theData.rangeVecObs[1], theData.rangeVecObs[2]);
         MessageInterface::ShowMessage("      . R_Obs_j2k matrix  : (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(0,0), R_Obs_j2k(0,1), R_Obs_j2k(0,2));
         MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(1,0), R_Obs_j2k(1,1), R_Obs_j2k(1,2));
         MessageInterface::ShowMessage("                            (%.12lf   %.12lf   %.12lf)\n", R_Obs_j2k(2,0), R_Obs_j2k(2,1), R_Obs_j2k(2,2));
      #endif
      
      // 3. Calculate ET-TAI correction for this signal leg
      if (useETTAI)
      {
         // Compute ET-TAI at trasnmite node
         Real tETTAI = ETminusTAI(theData.tPrecTime.GetMjd(), theData.tNode);
         // Compute ET-TAI at receive node
         Real rETTAI = ETminusTAI(theData.rPrecTime.GetMjd(), theData.rNode);
         // Compute ET-TAI correction for this signal leg
         ettaiCorrection = (tETTAI - rETTAI)*GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM;      // unit: km
         UnsignedInt i = 0;
         for (; i < theData.correctionIDs.size(); ++i)
         {
            if (theData.correctionIDs[i] == "ET-TAI")
               break;
         }
         theData.corrections[i] = ettaiCorrection;                                                         // unit: km
      }
          
      // 4. Perform feasibility check
      if (theData.stationParticipant)
      {
         const Real* elData;
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         {
            Rvector6 state_sez(theData.rangeVecObs,
                  theData.rangeRateVecObs);
            elData = ((GroundstationInterface*)(theData.tNode))->
                  IsValidElevationAngle(state_sez);
            signalIsFeasible = (elData[2] > 0.0);
            theData.feasibility = signalIsFeasible;
            theData.feasibilityReason = (theData.feasibility?"N":"B");   // "B": signal is blocked; "N": normal state
            theData.feasibilityValue = elData[0];

            #ifdef DEBUG_FEASIBILITY
            MessageInterface::ShowMessage("At transmit node: Obs vector = [%.12lf,  %.12lf,  %.12lf]km "
                  "so %s\n", theData.rangeVecObs(0), theData.rangeVecObs(1),
                  theData.rangeVecObs(2), (signalIsFeasible ? "feasible" :
                  "infeasible"));
            MessageInterface::ShowMessage(" elData = (%.8lf  %.8lf   %.8lf) degree\n", elData[0], elData[1], elData[2]);
            #endif
         }
         else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         {
            Rvector6 state_sez(-theData.rangeVecObs,
                  -theData.rangeRateVecObs);
            elData = ((GroundstationInterface*)(theData.rNode))->
                  IsValidElevationAngle(state_sez);
            signalIsFeasible = (elData[2] > 0.0);
            theData.feasibility = signalIsFeasible;
            theData.feasibilityReason = (theData.feasibility?"N":"B");   // "B": signal is blocked; "N": normal state
            theData.feasibilityValue = elData[0];

            #ifdef DEBUG_FEASIBILITY
            MessageInterface::ShowMessage("At receive node: Obs vector = [%.12lf,  %.12lf,  %.12lf]km "
                  "so %s\n", theData.rangeVecObs(0), theData.rangeVecObs(1),
                  theData.rangeVecObs(2), (signalIsFeasible ? "feasible" :
                  "infeasible"));
            MessageInterface::ShowMessage(" elData = (%.8lf  %.8lf   %.8lf) degree\n", elData[0], elData[1], elData[2]);
            #endif
         }
      }
      else
      ///@todo: Put in test for obstructing bodies; for now, always feasible
      {
         signalIsFeasible = true;
      }

      #ifdef DEBUG_RANGE_CALCULATION 
         MessageInterface::ShowMessage("   3. Summary for signal leg from %s to %s:\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());

         MessageInterface::ShowMessage("      . Geometric range       = %.12lf km\n", geoRange);
         if (includeLightTime)
         {
            Real lightTimeRange = theData.rangeVecInertial.GetMagnitude();
            MessageInterface::ShowMessage("      . Light time solution range = %.12lf km\n", lightTimeRange);
         }
         MessageInterface::ShowMessage("      . Relativity correction = %.12lf km\n", relCorrection);
         MessageInterface::ShowMessage("      . ET-TAI correction     = %.12lf km\n", ettaiCorrection);
         MessageInterface::ShowMessage("      . Feasibility           = %s\n\n", (signalIsFeasible?"true":"false"));
      #endif

      
      // 5. Report raw data
      if (navLog)
      {
         std::stringstream data;
         data.precision(16);

         if (logLevel <= 1)
         {
            Real range = theData.rangeVecInertial.GetMagnitude();
            if (range >= 0.0)
               data << "   " << GetPathDescription(false)
                    << " Range at A.1 epoch " << satPrecEpoch.GetMjd()
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
     
      // 6. Run ModelSignal for the next leg in signal path:
      // if epoctAtReceive was true, transmitter moved and we need its epoch,
      // if false, we need the receiver epoch
      GmatTime nextPrecEpoch = (epochAtReceive ? theData.tPrecTime : theData.rPrecTime);
      //// Assumption: theData.tDelay and theData.rDelay have to be specified before running ModelSignal() function
      //GmatTime nextPrecEpoch;
      //if (epochAtReceive)
      //   nextPrecEpoch = theData.tPrecTime - theData.tDelay*2/GmatTimeConstants::SECS_PER_DAY;          // go backward
      //else
      //   nextPrecEpoch = theData.rPrecTime + theData.rDelay*2/GmatTimeConstants::SECS_PER_DAY;          // go foreward

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
            nodePassed = previous->ModelSignal(nextPrecEpoch, nextFixed);
         }
      }
      else
      {
         if (next)
         {
            next->SetSignalData(theData);

            /// @todo: If there is a transponder delay, apply it here, moving
            /// nextEpoch ahead by the delay time
            nodePassed = next->ModelSignal(nextPrecEpoch, nextFixed);
         }
      }
     
      retval = nodePassed;
   }
   
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("ModelSignal(%.12lf, %s) exit\n", satPrecEpoch.GetMjd(),
            epochAtReceive ? "with fixed Receiver" : "with fixed Transmitter");
   #endif

   return retval;
}
#endif

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
   #ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("PhysicalSignal::ModelSignalDerivative(%s, %d) called for signal leg <%s %s>\n", obj->GetName().c_str(), forId, theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   #endif

   // Verify valid input
   if (obj == NULL)
      throw MeasurementException("Error: a NULL object inputs to PhysicalSignal::ModelSignalDerivative() function\n");

   // Get parameter ID
   Integer parameterID = -1;
   if (forId > 250)
      parameterID = GetParmIdFromEstID(forId, obj);
   else
      parameterID = forId;
   std::string paramName = obj->GetParameterText(parameterID);
   #ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("Solver-for parameter: %s\n", obj->GetParameterText(parameterID).c_str());
   #endif

   // Verify initialization
   if (!isInitialized)
   {
      #ifdef DEBUG_EXECUTION
         MessageInterface::ShowMessage("   Calling signal initialization\n");
      #endif
      InitializeSignal();
   }

   // Clear derivative data
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

   //Integer parameterID = -1;
   if (objPtr != NULL)
   {
      //if (forId > 250)
      //   parameterID = GetParmIdFromEstID(forId, obj);
      //else
      //   parameterID = forId;

      //#ifdef DEBUG_DERIVATIVES
      //MessageInterface::ShowMessage("Solver-for parameter: %s\n", objPtr->GetParameterText(parameterID).c_str());
      //#endif

      //std::string paramName = objPtr->GetParameterText(parameterID);
      if (paramName == "Position")
      {
         Rvector3 result;
         GetRangeDerivative(objPtr, true, false, result);

         for (UnsignedInt jj = 0; jj < 3; ++jj)
         {
            // theDataDerivatives[0][jj] = result[jj];
            // It accumulates derivatives of all signal legs in its path
            if (next)
               theDataDerivatives[0][jj] += result[jj];         
            else
               theDataDerivatives[0][jj] = result[jj];
         }
      }
      else if (paramName == "Velocity")
      {
         Rvector3 result;
         GetRangeDerivative(objPtr, false, true, result);

         for (UnsignedInt jj = 0; jj < 3; ++jj)
         {
            // theDataDerivatives[0][jj] = result[jj];
            // It accumulates derivatives of all signal legs in its path
            if (next)
               theDataDerivatives[0][jj] += result[jj];         
            else
               theDataDerivatives[0][jj] = result[jj];
         }
      }
      else if (paramName == "CartesianX")
      {
         Rvector6 result;
         GetRangeDerivative(objPtr, true, true, result);

         for (UnsignedInt jj = 0; jj < 6; ++jj)
         {
            // theDataDerivatives[0][jj] = result[jj];
            // It accumulates derivatives of all signal legs in its path
            if (next)
               theDataDerivatives[0][jj] += result[jj];         
            else
               theDataDerivatives[0][jj] = result[jj];
         }
         #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("Derivatives for signal path <%s %s>: [", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
            for (UnsignedInt jj = 0; jj < 6; ++jj)
               MessageInterface::ShowMessage("   %.12lf", result[jj]);
            MessageInterface::ShowMessage("]\n");
            MessageInterface::ShowMessage("Accumulate derivatives: [");
            for (UnsignedInt jj = 0; jj < 6; ++jj)
               MessageInterface::ShowMessage("   %.12lf", theDataDerivatives[0][jj]);
            MessageInterface::ShowMessage("]\n");

         #endif
      }
      else
      {
         UnsignedInt startIndex = paramName.size()-4;
         if (paramName.substr(startIndex) == "Bias")
         {
            if (previous == NULL)
            {  // This signal leg  object is the first one in signal path  
               SignalBase* firstleg = this;
               // Get last signal leg
               SignalBase* lastleg = this;
               while (lastleg->GetNext() !=NULL)
                  lastleg = lastleg->GetNext();
               
               if ((firstleg->GetSignalData().tNode->IsOfType(Gmat::GROUND_STATION)) && (lastleg->GetSignalData().rNode->IsOfType(Gmat::GROUND_STATION) == false))
               {
                  for (Integer i = 0; i < size; ++i)
                     theDataDerivatives[0][i] += 1.0;
               }
            }
            else if (next == NULL)
            {
               // This signal leg is the last one in signal path
               if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
               {
                  for (Integer i = 0; i < size; ++i)
                     theDataDerivatives[0][i] += 1.0;
               }
            }

            #ifdef DEBUG_DERIVATIVES
            MessageInterface::ShowMessage("   Deriv is w.r.t. %s  it value %lf\n", paramName.c_str(), theDataDerivatives[0][0]);
            #endif

            //for (Integer i = 0; i < size; ++i)
            //   theDataDerivatives[0][i] += 1.0;
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

   #ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("Exit PhysicalSignal::ModelSignalDerivative(%s, %d) for signal leg <%s %s>\n", obj->GetName().c_str(), forId, theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   #endif


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
#ifndef USE_PRECISION_TIME
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
      Rvector3 rangeGeoInertial = theData.rLoc - theData.tLoc;                                              // Range vector as seen from geocentric inertial obeserver (GMAT MathSpec Eq. 6.10)
      Rvector3 displacement = rangeGeoInertial + (theData.rOStateSSB.GetR() - theData.tOStateSSB.GetR());   // Range vector as seen from Barycentric inertial observer (GMAT MathSpec Eq. 6.12)

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
         Rvector3 rLocSSB = theData.rLoc + theData.rOStateSSB.GetR();
         Rvector3 tLocSSB = theData.tLoc + theData.tOStateSSB.GetR();
         // Range vector as seen from Barycentric inertial observer (GMAT MathSpec Eq. 6.12)
         displacement = rLocSSB - tLocSSB;
       
         #ifdef DEBUG_LIGHTTIME
            MessageInterface::ShowMessage("Positions in SSBMJ2000 cs: %s  (%.12lf   %.12lf   %.12lf)km -->  %s  "
                  "(%.12lf   %.12lf   %.12lf)km\n", theData.tNode->GetName().c_str(), tLocSSB(0), tLocSSB(1), tLocSSB(2),
                  theData.rNode->GetName().c_str(), rLocSSB(0), rLocSSB(1), rLocSSB(2));
         #endif

         relCorrection = 0.0;
         if (useRelativity)
         {
            relCorrection = RelativityCorrection(theData.tLoc+theData.tOStateSSB.GetR(), theData.rLoc+theData.rOStateSSB.GetR(),theData.tTime, theData.rTime);
         }

         Real lightTimeRange = displacement.GetMagnitude();
         deltaR = lightTimeRange + relCorrection;
         deltaT = (epochAtReceive ? -1.0 : 1.0) * deltaR /
                     (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / 1000.0);

         #ifdef DEBUG_LIGHTTIME
            MessageInterface::ShowMessage("Ligh Time range       = %.12lf km\n", lightTimeRange);  
            MessageInterface::ShowMessage("Relativity correction = %.12lf km\n", relCorrection);
            MessageInterface::ShowMessage("      ===> dEpoch = %.12le, dR = "
                  "%.3lf, dT = %.12le, trigger = %le\n", deltaE, deltaR, deltaT,
                  deltaE-deltaT);
         #endif
         ++loopCount;
      }
   }

   // Temporary check on data flow
   // Build the other data vectors
   CalculateRangeVectorInertial();
   CalculateRangeVectorObs();
   CalculateRangeRateVectorObs();

   return retval;
}
#else


bool PhysicalSignal::GenerateLightTimeData(const GmatTime atEpoch,
      const bool epochAtReceive)
{
   #ifdef DEBUG_EXECUTION
      GmatTime t = atEpoch;
      MessageInterface::ShowMessage("Called GenerateLightTimeData(%.12lf, "
            "%s)\n", t.GetMjd(), (epochAtReceive ? "Receiver fixed" :
            "Transmitter fixed"));
   #endif

   // It is equivalant to range tolerance = time tolerance * speed of light = (1.0e-12 s)x(299792458.0 m/s) = 0.0002998 m = 0.3 mm 
   Real timeTolerance = 1.0e-12;

   bool retval = false;

   if (includeLightTime)
   {
      // First make sure we start at the desired epoch
      MoveToEpoch(atEpoch, epochAtReceive, true);

      // Then compute the initial data
      Rvector3 rangeGeoInertial = theData.rLoc - theData.tLoc;                                              // Range vector as seen from geocentric inertial obeserver (GMAT MathSpec Eq. 6.10)
      Rvector3 displacement = rangeGeoInertial + (theData.rOStateSSB.GetR() - theData.tOStateSSB.GetR());   // Range vector as seen from Barycentric inertial observer (GMAT MathSpec Eq. 6.12)

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
      Real deltaE = (theData.rPrecTime - theData.tPrecTime).GetTimeInSec();

      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("      Starting: dEpoch = %.12le, dR = "
               "%.3lf, dT = %.12le\n", deltaE, deltaR, deltaT);
         MessageInterface::ShowMessage("Initial x Positions: %s  %.3lf -->  "
               "%s  %.3lf\n", theData.tNode->GetName().c_str(), theData.tLoc(0),
               theData.rNode->GetName().c_str(), theData.rLoc(0));
      #endif

      // Loop to half microsecond precision or 10 times, whichever comes first
      while ((GmatMathUtil::Abs(deltaE - deltaT) > timeTolerance) && (loopCount < 10))
      {
         #ifdef DEBUG_LIGHTTIME
            MessageInterface::ShowMessage("      Loop iteration %d\n",
                  loopCount);
         #endif
         MoveToEpoch(atEpoch + deltaT / GmatTimeConstants::SECS_PER_DAY,
               !epochAtReceive, false);
         deltaE = (epochAtReceive ? -1.0 : 1.0) *
          (theData.rPrecTime - theData.tPrecTime).GetTimeInSec();
         Rvector3 rLocSSB = theData.rLoc + theData.rOStateSSB.GetR();
         Rvector3 tLocSSB = theData.tLoc + theData.tOStateSSB.GetR();
         // Range vector as seen from Barycentric inertial observer (GMAT MathSpec Eq. 6.12)
         displacement = rLocSSB - tLocSSB;
       
         #ifdef DEBUG_LIGHTTIME
            MessageInterface::ShowMessage("Positions in SSBMJ2000 cs: %s  (%.12lf   %.12lf   %.12lf)km -->  %s  "
                  "(%.12lf   %.12lf   %.12lf)km\n", theData.tNode->GetName().c_str(), tLocSSB(0), tLocSSB(1), tLocSSB(2),
                  theData.rNode->GetName().c_str(), rLocSSB(0), rLocSSB(1), rLocSSB(2));
         #endif

         relCorrection = 0.0;
         if (useRelativity)
         {
            relCorrection = RelativityCorrection(theData.tLoc+theData.tOStateSSB.GetR(), theData.rLoc+theData.rOStateSSB.GetR(),theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd());
         }

         Real lightTimeRange = displacement.GetMagnitude();
         deltaR = lightTimeRange + relCorrection;
         deltaT = (epochAtReceive ? -1.0 : 1.0) * deltaR /
                     (GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / 1000.0);
         #ifdef DEBUG_LIGHTTIME
            MessageInterface::ShowMessage("Ligh Time range       = %.12lf\n", lightTimeRange);  
            MessageInterface::ShowMessage("Relativity correction = %.12lf\n", relCorrection);
            MessageInterface::ShowMessage("      ===> dEpoch = %.12le, dR = "
                  "%.8lf, dT = %.12le, trigger = %le\n", deltaE, deltaR, deltaT,
                  deltaE-deltaT);
         #endif
         ++loopCount;
      }
   }

   // Temporary check on data flow
   // Build the other data vectors
   CalculateRangeVectorInertial();
   CalculateRangeVectorObs();
   CalculateRangeRateVectorObs();

   return retval;
}
#endif


bool PhysicalSignal::HardwareDelayCalculation()
{
   bool retval = false;
   Real timeDelay;

   // 1. Get hardware delay from theData.tNode
   if (theData.tNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Transmit participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }

   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Get time delay from ground station' transmitter
      ObjectArray hardwareList = ((GroundstationInterface*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         if (hardwareList[i]->IsOfType("Transmitter"))
         {
            timeDelay = ((Transmitter*)hardwareList[i])->GetDelay();           // unit: second 
            break;
         }
      }
         
      if (i == hardwareList.size())
         timeDelay = 0.0;                     // no delay if no hardware is used
   }
   else
   {
      // Get time delay from spacecraft's transmitter or transponder
      ObjectArray hardwareList = ((Spacecraft*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         if (hardwareList[i]->IsOfType("Transmitter"))
         {
            timeDelay = ((Transmitter*)hardwareList[i])->GetDelay();    // unit: second 
            break;
         }
         if (hardwareList[i]->IsOfType("Transponder"))
         {
            timeDelay = ((Transponder*)hardwareList[i])->GetDelay();    // unit: second 
            break;
         }
      }

      if (i == hardwareList.size())
         timeDelay = 0.0;                   // no delay if no hardware is used
   }

   if (previous == NULL)
      theData.tDelay = timeDelay;          // for the first signal leg, it needs to add all delay time of ground station's transmitter 
   else
      theData.tDelay = timeDelay/2;        // otherwise, it needs to add only half delay time of spacecraft's transponder


   // 2. Get hardware delay from theData.rNode
   if (theData.rNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Receive participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }

   if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Get time delay from ground station' receiver
      ObjectArray hardwareList = ((GroundstationInterface*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         if (hardwareList[i]->IsOfType("Receiver"))
         {
            timeDelay = ((Receiver*)hardwareList[i])->GetDelay();           // unit: second 
            break;
         }
      }
         
      if (i == hardwareList.size())
         timeDelay = 0.0;                      // no delay if no hardware is used
   }
   else
   {
      // Get time delay from spacecraft's receiver or transponder
      ObjectArray hardwareList = ((Spacecraft*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         if (hardwareList[i]->IsOfType("Receiver"))
         {
            timeDelay = ((Receiver*)hardwareList[i])->GetDelay();    // unit: second 
            break;
         }
         if (hardwareList[i]->IsOfType("Transponder"))
         {
            timeDelay = ((Transponder*)hardwareList[i])->GetDelay();    // unit: second 
            break;
         }
      }

      if (i == hardwareList.size())
         timeDelay = 0.0;                      // no delay if no hardware is used
   }

   if (next == NULL)
      theData.rDelay = timeDelay;             // for the last signal leg, it needs to add all delay time of ground station's receiver 
   else
      theData.rDelay = timeDelay/2;           // otherwise, it needs to add only half delay time of spacecraft's transponder

#ifdef DEBUG_RANGE_CALCULATION 
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("   ++++    Hardware delay calculation for leg from %s to %s :\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("     . %s's %s hardware delay   : %.12le sec\n", theData.tNode->GetName().c_str(), ((previous == NULL)?"":"half of"), theData.tDelay);
   MessageInterface::ShowMessage("     . %s's %s hardware delay   : %.12le sec\n", theData.rNode->GetName().c_str(), ((next == NULL)?"":"half of"), theData.rDelay);
   MessageInterface::ShowMessage("     . Total hardware delay for this signal leg: %.12le sec\n\n", theData.tDelay + theData.rDelay);
#endif
   retval = true;
   return retval;
}



//---------------------------------------------------------------------------------------------
// bool MediaCorrectionCalculation(std::vector<RampTableData>* rampTB)
//---------------------------------------------------------------------------------------------
/**
* This function is used to calculate media (troposphere and ionosphere) correction for a given 
* signal leg
* 
* @param rampTB   Table containing the information about ramped frequency
*
* return true if calculation is completed
*/
//---------------------------------------------------------------------------------------------
bool PhysicalSignal::MediaCorrectionCalculation1(std::vector<RampTableData>* rampTB)
{
#ifdef IONOSPHERE    // Required until the f2c issues for Mac and Linux have been resolved
   if ((troposphere == NULL)&&(ionosphere == NULL))
      return true;
#else
   if (troposphere == NULL)
      return true;
#endif

   // 0. Verify the exsisting of transmit participant and receive participant 
   if (theData.tNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Transmit participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }
   if (theData.rNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Receive participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }


   bool retval = false;
   mediaCorrection = 0.0;                                                               // unit: km
   Real frequency = 0.0;                                                                // unit: MHz
   Real dsFrequency = 0.0;                                                              // unit: MHz

   // 1. Get frequency from theData.tNode
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Get frequency from ground station's transmiter or from ramped frequency table
      if (rampTB)
      {
         // Get frequency from ramped table if it is used
         GmatTime t1 = theData.tPrecTime - theData.tDelay/GmatTimeConstants::SECS_PER_DAY;
         frequency = GetFrequencyFromRampTable(t1.GetMjd(), rampTB)/1.0e6;                     // unit: Mhz
      }
      else
      {
         // Get frequency from ground station' transmitter
         ObjectArray hardwareList = ((GroundstationInterface*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();           // unit: MHz 
               break;
            }
         }
         
         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << theData.tNode->GetName() << " does not have a transmitter to transmit signal\n";
            throw MeasurementException(ss.str());
         }
      }
   }
   else
   {
      // Get frequency from spacecraft's transmitter or transponder
      ObjectArray hardwareList = ((Spacecraft*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         if (hardwareList[i]->IsOfType("Transmitter"))
         {
            frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();    // unit: MHz 
            break;
         }
         if (hardwareList[i]->IsOfType("Transponder"))
         {
            frequency = ((Transponder*)hardwareList[i])->GetSignal(1)->GetValue();    // unit: MHz 
            break;
         }
      }

      if (i == hardwareList.size())
      {
         std::stringstream ss;
         ss << "Error: Spacecraft " << theData.tNode->GetName() << " does not have a transmitter or transponder to transmit signal\n";
         throw MeasurementException(ss.str());
      }
   }

   // 2. Compute range rate from transmit participant to receive participant 
   // (range rate equals the projection of range rate vector to range vector)
   Real rangeRate = theData.rangeVecInertial.GetUnitVector() * theData.rangeRateVecInertial;                            // unit: km/s

   // 3. Compute doppler shift frequency at receiver
   dsFrequency = frequency*(1 - rangeRate/(GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM));   // unt: Mhz

   // 4. Set receive frequency to receiver
   if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Set doppler shift frequency to ground station' receiver
      ObjectArray hardwareList = ((GroundstationInterface*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         if (hardwareList[i]->IsOfType("Receiver"))
         {
            Signal* inputSignal = ((Receiver*)hardwareList[i])->GetSignal();
            inputSignal->SetValue(dsFrequency);                               // unit: MHz 
            ((Receiver*)hardwareList[i])->SetSignal(inputSignal);
            break;
         }
      }
         
      if (i == hardwareList.size())
      {
            std::stringstream ss;
            ss << "Error: Ground station " << theData.rNode->GetName() << " does not have a receiver to receive signal\n";
            throw MeasurementException(ss.str());
      }

      // Note that: there is no next leg when the receive participant is a ground station 
      if (next)
         throw MeasurementException("Error: ground station is in middle of signal path\n");
   }
   else
   {
      // Set doppler shift frequency to spacecraft's transponder
      ObjectArray hardwareList = ((Spacecraft*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
      //StringArray sr = ((Spacecraft*)theData.rNode)->GetStringArrayParameter("AddHardware");
      //for(int k=0; k < sr.size(); ++k)
      //   MessageInterface::ShowMessage("&&& %s\n",sr[k].c_str());

      
      //MessageInterface::ShowMessage("Hardware used in <%s,%p>: %s\n", theData.rNode->GetName().c_str(), theData.rNode, (hardwareList.size() == 0?"No hardware used":""));
      //if (hardwareList.size() == 0)
      //   MessageInterface::ShowMessage("Object data:\n%s", ((Spacecraft*)theData.rNode)->GetGeneratingString(Gmat::SHOW_SCRIPT, "   ").c_str());
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         //MessageInterface::ShowMessage("  %d.%s\n", i, hardwareList[i]->GetName().c_str());
         if (hardwareList[i]->IsOfType("Transponder"))
         {
            Signal* inputSignal = ((Transponder*)hardwareList[i])->GetSignal(0);
            inputSignal->SetValue(dsFrequency);                               // unit: Mhz
            ((Transponder*)hardwareList[i])->SetSignal(inputSignal, 0);       // This function force to calculate transmit frequency of the transponder
            break;
         }
      }

      if (i == hardwareList.size())
      {
         std::stringstream ss;
         ss << "Error: Spacecraft " << theData.rNode->GetName() << " does not have a transponder to pass signal\n";
         throw MeasurementException(ss.str());
      }
   }


   // 5. Computer media correction
   Rvector3 r1B = theData.tLoc + theData.tOStateSSB.GetR();
   Rvector3 r2B = theData.rLoc + theData.rOStateSSB.GetR();
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
      MediaCorrection(frequency, r1B, r2B, theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd());
   else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
      MediaCorrection(dsFrequency, r2B, r1B, theData.rPrecTime.GetMjd(), theData.tPrecTime.GetMjd());
   else
      MediaCorrection(frequency, r1B, r2B, theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd());

#ifdef DEBUG_RANGE_CALCULATION
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("   ++++    Media corrections calculation for leg from %s to %s :\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("     .Frequency       : %.12le Mhz\n", frequency);
   UnsignedInt i;
   for(i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Troposphere")
         break;
   }
   if (theData.useCorrection[i])
      MessageInterface::ShowMessage("     .Troposphere correction : %.12lf\n", theData.corrections[i]);
   for(i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Ionosphere")
         break;
   }
   if (theData.useCorrection[i])
      MessageInterface::ShowMessage("     .Ionosphere correction : %.12lf\n", theData.corrections[i]);
   
   MessageInterface::ShowMessage("     . Doppler shift frequency: %.12le Mhz\n\n", dsFrequency);

#endif
   retval = true;

   //// 6. Repeat for next physical signal object
   //if (next)
   //   retval = (retval && next->MediaCorrectionCalculation(rampTB));

   return retval;
}

      
bool PhysicalSignal::MediaCorrectionCalculation(std::vector<RampTableData>* rampTB)
{
   // 1. Verify the exsisting of transmit participant and receive participant 
   if (theData.tNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Transmit participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }
   if (theData.rNode == NULL)
   {
      std::stringstream ss;
      ss << "Error: Receive participant of leg " << GetName() << " is NULL";
      throw MeasurementException(ss.str());
   }

   // 2. Get media correction options from ground station
   GroundstationInterface* gs = NULL;
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
      gs = (GroundstationInterface*)theData.tNode;
   else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
      gs = (GroundstationInterface*)theData.rNode;

   // 3. Set tropspohere and ionosphere
   std::string troposphereModel = "None";
   std::string ionosphereModel = "None";
   if (gs)
   {
      troposphereModel = gs->GetStringParameter("TroposphereModel");
      ionosphereModel = gs->GetStringParameter("IonosphereModel");
   }
   
   theData.correctionIDs.push_back("Troposphere");
   theData.corrections.push_back(0.0);
   if (troposphereModel == "HopfieldSaastamoinen")
   {
      if (troposphere == NULL)
         troposphere = new Troposphere(gs->GetName()+"_Troposphere");
      theData.useCorrection.push_back(true);
   }
   else
      theData.useCorrection.push_back(false);

#ifdef IONOSPHERE
   theData.correctionIDs.push_back("Ionosphere");
   theData.corrections.push_back(0.0);
   if (ionosphereModel == "IRI2007")
   {
      if (ionosphere == NULL)
         ionosphere = new Ionosphere(gs->GetName()+"_Ionosphere");
      theData.useCorrection.push_back(true);
   }
   else
      theData.useCorrection.push_back(false);
#endif

#ifdef IONOSPHERE
   if ((troposphere == NULL)&&(ionosphere == NULL))
      return true;
#else   if ((troposphere == NULL)&&(ionosphere == NULL))
      return true;
#endif

   bool retval = false;
   mediaCorrection = 0.0;                                                               // unit: km
   Real frequency = 0.0;                                                                // unit: MHz
   Real dsFrequency = 0.0;                                                              // unit: MHz
   
   // 4. Get frequency from theData.tNode
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Get frequency from ground station's transmiter or from ramped frequency table
      if (rampTB)
      {
         // Get frequency from ramped table if it is used
         GmatTime t1 = theData.tPrecTime - theData.tDelay/GmatTimeConstants::SECS_PER_DAY;
         frequency = GetFrequencyFromRampTable(t1.GetMjd(), rampTB)/1.0e6;                     // unit: Mhz
      }
      else
      {
         // Get frequency from ground station' transmitter
         ObjectArray hardwareList = ((GroundstationInterface*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
         UnsignedInt i;
         for (i = 0; i < hardwareList.size(); ++i)
         {
            if (hardwareList[i]->IsOfType("Transmitter"))
            {
               frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();           // unit: MHz 
               break;
            }
         }
         
         if (i == hardwareList.size())
         {
            std::stringstream ss;
            ss << "Error: Ground station " << theData.tNode->GetName() << " does not have a transmitter to transmit signal\n";
            throw MeasurementException(ss.str());
         }
      }
   }
   else
   {
      // Get frequency from spacecraft's transmitter or transponder
      ObjectArray hardwareList = ((Spacecraft*)theData.tNode)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         if (hardwareList[i]->IsOfType("Transmitter"))
         {
            frequency = ((Transmitter*)hardwareList[i])->GetSignal()->GetValue();    // unit: MHz 
            break;
         }
         if (hardwareList[i]->IsOfType("Transponder"))
         {
            frequency = ((Transponder*)hardwareList[i])->GetSignal(1)->GetValue();    // unit: MHz 
            break;
         }
      }

      if (i == hardwareList.size())
      {
         std::stringstream ss;
         ss << "Error: Spacecraft " << theData.tNode->GetName() << " does not have a transmitter or transponder to transmit signal\n";
         throw MeasurementException(ss.str());
      }
   }

   // 5. Compute range rate from transmit participant to receive participant 
   // (range rate equals the projection of range rate vector to range vector)
   Real rangeRate = theData.rangeVecInertial.GetUnitVector() * theData.rangeRateVecInertial;                            // unit: km/s

   // 6. Compute doppler shift frequency at receiver
   dsFrequency = frequency*(1 - rangeRate/(GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM*GmatMathConstants::M_TO_KM));   // unt: Mhz

   // 7. Set receive frequency to receiver
   if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
   {
      // Set doppler shift frequency to ground station' receiver
      ObjectArray hardwareList = ((GroundstationInterface*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         if (hardwareList[i]->IsOfType("Receiver"))
         {
            Signal* inputSignal = ((Receiver*)hardwareList[i])->GetSignal();
            inputSignal->SetValue(dsFrequency);                               // unit: MHz 
            ((Receiver*)hardwareList[i])->SetSignal(inputSignal);
            break;
         }
      }
         
      if (i == hardwareList.size())
      {
            std::stringstream ss;
            ss << "Error: Ground station " << theData.rNode->GetName() << " does not have a receiver to receive signal\n";
            throw MeasurementException(ss.str());
      }

      // Note that: there is no next leg when the receive participant is a ground station 
      if (next)
         throw MeasurementException("Error: ground station is in middle of signal path\n");
   }
   else
   {
      // Set doppler shift frequency to spacecraft's transponder
      ObjectArray hardwareList = ((Spacecraft*)theData.rNode)->GetRefObjectArray(Gmat::HARDWARE);
      //StringArray sr = ((Spacecraft*)theData.rNode)->GetStringArrayParameter("AddHardware");
      //for(int k=0; k < sr.size(); ++k)
      //   MessageInterface::ShowMessage("&&& %s\n",sr[k].c_str());

      
      //MessageInterface::ShowMessage("Hardware used in <%s,%p>: %s\n", theData.rNode->GetName().c_str(), theData.rNode, (hardwareList.size() == 0?"No hardware used":""));
      //if (hardwareList.size() == 0)
      //   MessageInterface::ShowMessage("Object data:\n%s", ((Spacecraft*)theData.rNode)->GetGeneratingString(Gmat::SHOW_SCRIPT, "   ").c_str());
      UnsignedInt i;
      for (i = 0; i < hardwareList.size(); ++i)
      {
         //MessageInterface::ShowMessage("  %d.%s\n", i, hardwareList[i]->GetName().c_str());
         if (hardwareList[i]->IsOfType("Transponder"))
         {
            Signal* inputSignal = ((Transponder*)hardwareList[i])->GetSignal(0);
            inputSignal->SetValue(dsFrequency);                               // unit: Mhz
            ((Transponder*)hardwareList[i])->SetSignal(inputSignal, 0);       // This function force to calculate transmit frequency of the transponder
            break;
         }
      }

      if (i == hardwareList.size())
      {
         std::stringstream ss;
         ss << "Error: Spacecraft " << theData.rNode->GetName() << " does not have a transponder to pass signal\n";
         throw MeasurementException(ss.str());
      }
   }

   // 8. Computer media correction
   Rvector3 r1B = theData.tLoc + theData.tOStateSSB.GetR();
   Rvector3 r2B = theData.rLoc + theData.rOStateSSB.GetR();
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      MediaCorrection(frequency, r1B, r2B, theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd());
   }
   else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
   {
      MediaCorrection(dsFrequency, r2B, r1B, theData.rPrecTime.GetMjd(), theData.tPrecTime.GetMjd());
   }
   else
   {
      MediaCorrection(frequency, r1B, r2B, theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd());
   }

#ifdef DEBUG_RANGE_CALCULATION
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("   ++++    Media corrections calculation for leg from %s to %s :\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   MessageInterface::ShowMessage("   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
   MessageInterface::ShowMessage("     .Frequency       : %.12le Mhz\n", frequency);
   UnsignedInt i;
   for(i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Troposphere")
         break;
   }
   if (theData.useCorrection[i])
      MessageInterface::ShowMessage("     .Troposphere correction : %.12lf\n", theData.corrections[i]);
   for(i = 0; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Ionosphere")
         break;
   }
   if (theData.useCorrection[i])
      MessageInterface::ShowMessage("     .Ionosphere correction : %.12lf\n", theData.corrections[i]);
   
   MessageInterface::ShowMessage("     . Doppler shift frequency: %.12le Mhz\n\n", dsFrequency);

#endif
   retval = true;

   //// 6. Repeat for next physical signal object
   //if (next)
   //   retval = (retval && next->MediaCorrectionCalculation(rampTB));

   return retval;
}

      
//----------------------------------------------------------------------
// void AddCorrection(const std::string& modelName, 
//                    const std::string& correctionType)
//----------------------------------------------------------------------
/**
 * This function is used to add correction model to the measurement
 *
 * @param nodelName         The name of correction model involving in the
 *                          measurement
 * @param correctionType    The name of correction type such as: 
 *                          //TroposphereModel, IonosphereModel, 
 *                          ET-TAI, or Relativity
 */
//----------------------------------------------------------------------
void PhysicalSignal::AddCorrection(const std::string& modelName, 
   const std::string& correctionType)
{
   #ifdef DEBUG_MEASUREMENT_CORRECTION
      MessageInterface::ShowMessage("Adding correction type <%s> with named <%s>\n",
        correctionType.c_str(), modelName.c_str());
   #endif
   
//   if (correctionType == "TroposphereModel")
//   {
//      if (modelName == "HopfieldSaastamoinen")
//      {
//         // Create troposphere model
//         if (troposphere != NULL)
//            delete troposphere;
//         troposphere = new Troposphere(modelName);
//         if (troposphere == NULL)
//            throw MeasurementException("Error: Fail to create Troposphere model\n");
//
//         #ifdef DEBUG_MEASUREMENT_CORRECTION
//            MessageInterface::ShowMessage("   Set as troposphere model:   troposphere(%p)\n", troposphere);
//         #endif
//       
//         // Set correction to theData
//         UnsignedInt i = 0;
//         for (; i < theData.correctionIDs.size(); ++i)
//            if (theData.correctionIDs[i] == "Troposphere")
//               break;
//         if (i == theData.correctionIDs.size())
//         {
//            theData.correctionIDs.push_back("Troposphere");
//            theData.useCorrection.push_back(true);
//            theData.corrections.push_back(0.0);
//         }
//         else
//         {
//            theData.useCorrection[i] = true;
//            theData.corrections[i] = 0.0;
//         }
//      }
//      else if (modelName == "None")
//      {
//         // Set correction to theData
//         UnsignedInt i = 0;
//         for (; i < theData.correctionIDs.size(); ++i)
//         if (theData.correctionIDs[i] == "Troposphere")
//            break;
//         if (i < theData.correctionIDs.size())
//         {
//            theData.useCorrection[i] = false;
//            theData.corrections[i] = 0.0;
//         }
//      }
//      else
//      {
//         throw MeasurementException("Error: '" + modelName +"' is not a valid name for Troposphere correction.\n"
//         +"Currently only 'HopfieldSaastamoinen' is allowed for Troposphere.\n");
//      }
//   }
//   else if (correctionType == "IonosphereModel")
//   {
//      if (modelName == "IRI2007")
//      {
/////// TBD: Determine if there is a more generic way to add these
//         // Create IRI2007 ionosphere model
//         #ifdef IONOSPHERE
//            // Create ionosphere model
//            if (ionosphere != NULL)
//               delete ionosphere;
//            ionosphere = new Ionosphere(modelName);
//            if (ionosphere == NULL)
//               throw MeasurementException("Error: Fail to create Ionosphere model\n");
//
//            #ifdef DEBUG_MEASUREMENT_CORRECTION
//               MessageInterface::ShowMessage("   Set as ionosphere model:    ionosphere(%p)\n", ionosphere);
//            #endif
//
//            // Set correction
//            UnsignedInt i = 0;
//            for (; i < theData.correctionIDs.size(); ++i)
//               if (theData.correctionIDs[i] == "Ionosphere")
//                  break;
//            if (i == theData.correctionIDs.size())
//            {
//               theData.correctionIDs.push_back("Ionosphere");
//               theData.useCorrection.push_back(true);
//               theData.corrections.push_back(0.0);
//            }
//            else
//            {
//               theData.useCorrection[i] = true;
//               theData.corrections[i] = 0.0;
//            }
//         #else
//            MessageInterface::ShowMessage("Ionosphere IRI2007 model currently is not "
//                     "available.\nIt will be be added to GMAT in a future release.\n");
//
//            throw MeasurementException("Ionosphere IRI2007 model currently is not "
//               "available.\nIt will be be added to GMAT in a future release.\n");
//         #endif
//      }
//      else if (modelName == "None")
//      {
//         // Set correction to theData
//         UnsignedInt i = 0;
//         for (; i < theData.correctionIDs.size(); ++i)
//            if (theData.correctionIDs[i] == "Ionosphere")
//               break;
//         if (i < theData.correctionIDs.size())
//         {
//            theData.useCorrection[i] = false;
//            theData.corrections[i] = 0.0;
//         }
//      }
//      else
//      {
//         throw MeasurementException("Error: '" + modelName + "' is not a valid name for Ionosphere correction.\n"
//         +"Currently only 'IRI2007' is allowed for Ionosphere.\n");
//      }
//   }
//   else 
   if (correctionType == "Relativity")
   {
      // Set relativity correction
      UnsignedInt i = 0;
      for (; i < theData.correctionIDs.size(); ++i)
         if (theData.correctionIDs[i] == "Relativity")
            break;
      if (i == theData.correctionIDs.size())
      {
          theData.correctionIDs.push_back("Relativity");
          theData.useCorrection.push_back(true);
          theData.corrections.push_back(0.0);
      }
      else
      {
         theData.useCorrection[i] = true;
         theData.corrections[i] = 0.0;
      }
      useRelativity = true;
   }
   else if (correctionType == "ET-TAI")
   {
      // Set ET-TAI correction
      UnsignedInt i = 0;
      for (; i < theData.correctionIDs.size(); ++i)
         if (theData.correctionIDs[i] == "ET-TAI")
            break;
      if (i == theData.correctionIDs.size())
      {
          theData.correctionIDs.push_back("ET-TAI");
          theData.useCorrection.push_back(true);
          theData.corrections.push_back(0.0);
      }
      else
      {
         theData.useCorrection[i] = true;
         theData.corrections[i] = 0.0;
      }
      useETTAI = true;
   }

   // Add correction model to the next SignalBase in this signal path
   if (next)
      next->AddCorrection(modelName, correctionType);
}



//--------------------------------------------------------------------------------------------
// Real LightTimeCorrection::RelativityCorrection(Rvector3 r1B, Rvector3 r2B, Real t1, Real t2)
//--------------------------------------------------------------------------------------------
/**
 * Calculate relativity correction
 *
 * @param t1     Epoch used by the first participant (unit: A1Mid)
 * @param t2     Epoch used by the second participant (unit: A1Mid)
 * @param r1B    Position of the first participant in SSB coordinate system at epoch t1 (unit:km)
 * @param r2B    Position of the second participant in SSB coordinate system at epoch t2 (unit:km)
 *
 * return range correction (unit: km)
 */
//--------------------------------------------------------------------------------------------
Real PhysicalSignal::RelativityCorrection(Rvector3 r1B, Rvector3 r2B, Real t1, Real t2)
{
#ifdef DEBUG_RELATIVITY_CORRECTION
   MessageInterface::ShowMessage("PhysicalSignal::RelativityCorrection(): Input parameters\n");
   MessageInterface::ShowMessage(" r1B = (%.6lf   %.6lf   %.6lf)km at time t1 = %.12lf\n", r1B[0], r1B[1], r1B[2], t1);
   MessageInterface::ShowMessage(" r2B = (%.6lf   %.6lf   %.6lf)km at time t2 = %.12lf\n", r2B[0], r2B[1], r2B[2], t2);
#endif
   Rvector3 P1, P2, BP1, BP2, r1, r2, r12;
   Real r1Mag, r2Mag, r12Mag;

   SpecialCelestialPoint* barycenter = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
   Rvector3 B1 = barycenter->GetMJ2000Position(t1);
   Rvector3 B2 = barycenter->GetMJ2000Position(t2);

   StringArray planetList = solarSystem->GetBodiesInUse();
   CelestialBody* sun = solarSystem->GetBody("Sun");
   Real gammar = 1.0;

   Real relCorr = 0.0;
   for (UnsignedInt i = 0; i < planetList.size(); ++i)
   {
      CelestialBody* planet = solarSystem->GetBody(planetList[i]);
      Real planetMu = planet->GetRealParameter(planet->GetParameterID("Mu"));

      P1 = planet->GetMJ2000Position(t1);            // Planet's position in SSB coordinate system at time t1
      P2 = planet->GetMJ2000Position(t2);            // Planet's position in SSB coordinate system at time t2
      BP1 = P1 - B1;                           // vector from SSB to planet in  SSB coordinate system at time t1
      BP2 = P2 - B2;                           // vector from SSB to planet in  SSB coordinate system at time t2

      r1 = r1B - BP1;                          // position of the first participant in the local inertial coordinate system of the planet at time t1
      r2 = r2B - BP2;                          // position of the first participant in the local inertial coordinate system of the planet at time t2
      r12 = r2 - r1;                           // range vector in the local inertial coordinate system of the planet 
      r1Mag = r1.Norm();                       // unit: km
      r2Mag = r2.Norm();                       // unit: km
      r12Mag = r12.Norm();                     // unit: km

      Real c = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / GmatMathConstants::KM_TO_M;      // unit: km/s
      Real term1 = (1+gammar)*(planetMu/c)/c;         // unit: km
      Real correction;
      if (planet == sun)
      {
         correction = term1*GmatMathUtil::Ln((r1Mag + r2Mag + r12Mag + term1)/(r1Mag + r2Mag - r12Mag + term1));
      }
      else
      {
         correction = term1*GmatMathUtil::Ln((r1Mag + r2Mag + r12Mag)/(r1Mag + r2Mag - r12Mag));
      }
      relCorr += correction;
   }

   UnsignedInt i = 0;
   for(; i < theData.correctionIDs.size(); ++i)
     if (theData.correctionIDs[i] == "Relativity")
         break;

   if (i < theData.correctionIDs.size())
   {
      if (theData.useCorrection[i])
         theData.corrections[i] = relCorr;                  // unit: Km
      else
         theData.corrections[i] = 0.0;
   }

#ifdef DEBUG_RELATIVITY_CORRECTION
   MessageInterface::ShowMessage("Relativity correction: %le\n", relCorr);
#endif
   return relCorr;
}


/// Calculate ET - TAI at a ground station on Earth or a spacecraft
Real PhysicalSignal::ETminusTAI(Real tA1MJD, SpacePoint* participant)
{
   // Step 2:
   // Specify celestial bodies and special celestial points:
   CelestialBody* sun = solarSystem->GetBody("Sun");
   CelestialBody* earth = solarSystem->GetBody("Earth");
   CelestialBody* luna  = solarSystem->GetBody("Luna");
   CelestialBody* mars = solarSystem->GetBody("Mars");
   CelestialBody* jupiter= solarSystem->GetBody("Jupiter");
   CelestialBody* saturn = solarSystem->GetBody("Saturn");
   SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
   // Create Eath-Moon Barycenter
   Barycenter* emb = new Barycenter("EarthMoonBarycenter");
   emb->SetRefObject(earth, Gmat::SPACE_POINT, "Earth");
   emb->SetRefObject(luna, Gmat::SPACE_POINT, "Luna");
   emb->SetStringParameter("BodyNames", "Earth", 0);
   emb->SetStringParameter("BodyNames", "Luna", 1);
   emb->Initialize();

   // Specify position and velocity of celestial bodies and special celestial points
   Rvector3 ssbPos = ssb->GetMJ2000Position(tA1MJD);
   Rvector3 ssbVel = ssb->GetMJ2000Velocity(tA1MJD);
   Rvector3 sunPos = sun->GetMJ2000Position(tA1MJD);
   Rvector3 sunVel = sun->GetMJ2000Velocity(tA1MJD);
   Rvector3 earthPos = earth->GetMJ2000Position(tA1MJD);
   Rvector3 earthVel = earth->GetMJ2000Velocity(tA1MJD);
   Rvector3 marsPos = mars->GetMJ2000Position(tA1MJD);
   Rvector3 marsVel = mars->GetMJ2000Velocity(tA1MJD);
   Rvector3 jupiterPos = jupiter->GetMJ2000Position(tA1MJD);
   Rvector3 jupiterVel = jupiter->GetMJ2000Velocity(tA1MJD);
   Rvector3 saturnPos = saturn->GetMJ2000Position(tA1MJD);
   Rvector3 saturnVel = saturn->GetMJ2000Velocity(tA1MJD);

   Rvector3 emPos = emb->GetMJ2000Position(tA1MJD);
   Rvector3 emVel = emb->GetMJ2000Velocity(tA1MJD);

   Rvector3 lunaPos = luna->GetMJ2000Position(tA1MJD);
   Rvector3 lunaVel = luna->GetMJ2000Velocity(tA1MJD);

   // Step 3:
   // Note that: position vector participant->GetMJ2000Position(tA1MJD) is pointing from j2kBody to participant (not from SSB nor Earth)
   SpacePoint* j2kBody = participant->GetJ2000Body();
   Rvector3 Earth2GS = participant->GetMJ2000Position(tA1MJD) + j2kBody->GetMJ2000Position(tA1MJD) - earth->GetMJ2000Position(tA1MJD);
   Rvector3 Earth2GS_Vel = participant->GetMJ2000Velocity(tA1MJD) + j2kBody->GetMJ2000Velocity(tA1MJD) - earth->GetMJ2000Velocity(tA1MJD);
   
   // Step 4:
   //Define constants
   Real c = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM * GmatMathConstants::M_TO_KM;         // light speed (unit: km/s)
   Real muEarth   = earth->GetRealParameter(earth->GetParameterID("Mu"));                  // mu = 398600.4415 for Earth
   Real muSun     = sun->GetRealParameter(earth->GetParameterID("Mu"));                    // mu = 132712440017.99 for Sun
   Real muJupiter = jupiter->GetRealParameter(earth->GetParameterID("Mu"));                // mu = 126712767.8578 for Jupiter
   Real muSaturn  = saturn->GetRealParameter(earth->GetParameterID("Mu"));                 // mu = 37940626.061137 for Saturn
   Real muMars    = mars->GetRealParameter(earth->GetParameterID("Mu"));                   // mu = 42828.314258067 for Mars
   Rvector3 Sun_wrt_SSB_Vel = sunVel - ssbVel;
   Rvector3 EM_wrt_Sun_Pos = emPos - sunPos;
   Rvector3 EM_wrt_Sun_Vel = emVel - sunVel;
   Rvector3 EM_wrt_SSB_Vel = emVel - ssbVel;
   Rvector3 E_wrt_SSB_Vel = earthVel - ssbVel;
   Rvector3 E_wrt_EM_Pos = earthPos - emPos;
   Rvector3 Jup_wrt_Sun_Pos = jupiterPos - sunPos;
   Rvector3 Jup_wrt_Sun_Vel = jupiterVel - sunVel;
   Rvector3 Sat_wrt_Sun_Pos = saturnPos - sunPos;
   Rvector3 Sat_wrt_Sun_Vel = saturnVel - sunVel;
   
   // ET minus TAI calculation.  Eq. 2-23 on p. 2-14 of Moyer
   Real ET_TAI = 32.184 + 2*(EM_wrt_Sun_Vel/c)*(EM_wrt_Sun_Pos/c) + 
                (EM_wrt_SSB_Vel/c)*(E_wrt_EM_Pos/c) + 
                (E_wrt_SSB_Vel/c)*(Earth2GS/c) + 
                (muJupiter  / (muSun + muJupiter)) * (Jup_wrt_Sun_Vel/c)*(Jup_wrt_Sun_Pos/c) + 
                (muSaturn  / (muSun + muSaturn)) * (Sat_wrt_Sun_Vel/c)*(Sat_wrt_Sun_Pos/c) + 
                (Sun_wrt_SSB_Vel/c)*(EM_wrt_Sun_Pos/c);
   
   //(muMars  / (c^2 * (muSun + muMars) )) * (Mars_wrt_Sun_Vel'*Mars_wrt_Sun_Pos);  % is this Mars term correct?
   
   if (participant->IsOfType(Gmat::SPACECRAFT))
   {
      // Compute PSat in Eq 2-24 Moyer:
      Real Psat = 2*(Earth2GS_Vel/c)*(Earth2GS/c);
      ET_TAI = ET_TAI + Psat;
   }

   if (emb)
      delete emb;

   return ET_TAI;               // unit: second
}


//------------------------------------------------------------------------
// RealArray MediaCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch1, Real epoch2)
//------------------------------------------------------------------------
/**
 * This function is used to calculate media corrections.
 *
 * @param freq    The frequency of signal   (unit: MHz)
 * @param r1B     Position of ground station in SSB FK5 coordinate system 
 * @param r2B     Position of spacecraft in SSB FK5 coordinate system 
 * @param epoch1  The time at which signal is transmitted from or received at ground station
 * @param epoch2  The time at which signal is received from or transmitted at spacecraft
 * return         An array containing results fo media correction
 */
//------------------------------------------------------------------------
RealArray PhysicalSignal::MediaCorrection(Real freq, Rvector3 r1B, Rvector3 r2B, Real epoch1, Real epoch2)
{
   #ifdef DEBUG_MEASUREMENT_CORRECTION
      MessageInterface::ShowMessage("start PhysicalMeasurement::MediaCorrection()\n");
   #endif
   Real epsilon = 1.0e-8;

   RealArray tropoCorrection;
   tropoCorrection.push_back(0.0);
   tropoCorrection.push_back(0.0);
   tropoCorrection.push_back(0.0);
   
   RealArray ionoCorrection;
   ionoCorrection.push_back(0.0);
   ionoCorrection.push_back(0.0);
   ionoCorrection.push_back(0.0);

   RealArray mediaCorrection;
   mediaCorrection.push_back(0.0);
   mediaCorrection.push_back(0.0);
   mediaCorrection.push_back(0.0);

   // 1. Run Troposphere correction:
   UpdateRotationMatrix(epoch1, "o_j2k");
   Rvector3 rangeVector = r2B - r1B;                                         // vector pointing from ground station to spacecraft in FK5 coordinate system
   Real elevationAngle = asin((R_Obs_j2k*(rangeVector.GetUnitVector())).GetElement(2));   // unit: radian
   
   if (elevationAngle > epsilon)
   {
      tropoCorrection = TroposphereCorrection(freq, rangeVector.GetMagnitude(), elevationAngle);
      #ifdef DEBUG_MEASUREMENT_CORRECTION
         MessageInterface::ShowMessage(" frequency = %le MHz, epoch = %lf,     r2B-r1B = ('%.8lf   %.8lf   %.8lf')km\n", freq, epoch, rangeVector[0], rangeVector[1], rangeVector[2]);
         MessageInterface::ShowMessage(" TroposhereCorrection = (%lf m,  %lf arcsec,   %le s)\n", tropoCorrection[0], tropoCorrection[1], tropoCorrection[2]);
      #endif
      
      mediaCorrection[0] = tropoCorrection[0];
      mediaCorrection[1] = tropoCorrection[1];
      mediaCorrection[2] = tropoCorrection[2];
   }
   // Update value of theData.corrections and theData.useCorrection
   UnsignedInt i = 0;
   for (; i < theData.correctionIDs.size(); ++i)
   {
      if (theData.correctionIDs[i] == "Troposphere")
         break;
   }
   theData.corrections[i] = mediaCorrection[0]*GmatMathConstants::M_TO_KM;
   theData.useCorrection[i] = (troposphere != NULL);

   #ifdef IONOSPHERE
      // 2. Run Ionosphere correction:
      if (elevationAngle > epsilon)
      {
         ionoCorrection = IonosphereCorrection(freq, r1B, r2B, epoch1, epoch2);
         #ifdef DEBUG_MEASUREMENT_CORRECTION
            MessageInterface::ShowMessage(" frequency = %le MHz, epoch1 = %lf,  r1B = ('%.8lf   %.8lf   %.8lf')km\n", freq, epoch1, r1B[0], r1B[1], r1B[2]);
            MessageInterface::ShowMessage("                      epoch2 = %lf,  r2B = ('%.8lf   %.8lf   %.8lf')km\n", epoch2, r2B[0], r2B[1], r2B[2]);
            MessageInterface::ShowMessage(" IonoshereCorrection = (%lf m,  %lf arcsec,   %le s)\n", ionoCorrection[0], ionoCorrection[1], ionoCorrection[2]);
         #endif
      
         // 3. Combine effects:
         mediaCorrection[0] += ionoCorrection[0];
         mediaCorrection[1] += ionoCorrection[1];
         mediaCorrection[2] += ionoCorrection[2];
      }

      for (i = 0; i < theData.correctionIDs.size(); ++i)
      {
         if (theData.correctionIDs[i] == "Ionosphere")
            break;
      }
      theData.corrections[i] = ionoCorrection[0]*GmatMathConstants::M_TO_KM;
      theData.useCorrection[i] = (ionosphere != NULL);
      
   #endif
   
   
   #ifdef DEBUG_MEASUREMENT_CORRECTION
      MessageInterface::ShowMessage("exit PhysicalMeasurement::MediaCorrection()\n");
   #endif

   return mediaCorrection;
}


//------------------------------------------------------------------------
//RealArray TroposphereCorrection(Real freq, Real distance, Real elevationAngle)
//------------------------------------------------------------------------
/**
 * This function is used to calculate Troposphere correction.
 *
 * @param freq            The frequency of signal                               (unit: MHz)
 * @param distance        Distance from ground station to spacecraft            (unit: km)
 * @param elevationAngle  The elevation angle from ground station to spacecraft (unit: radian)
 *                 
 * return Troposphere correction vector
 */
//------------------------------------------------------------------------
RealArray PhysicalSignal::TroposphereCorrection(Real freq, Real distance, Real elevationAngle)
{
   RealArray tropoCorrection;
 
   if (troposphere != NULL)
   {
      // Set troposphere's ref objects
      troposphere->SetSolarSystem(solarSystem);

      // Set temperature, pressure, humidity to Troposphere object
      Real wavelength = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / (freq*1.0e6);

      GroundstationInterface* gs = NULL; 
      if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         gs = (GroundstationInterface*)theData.tNode;
      if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         gs = (GroundstationInterface*)theData.rNode;
      if (gs != NULL)
      {
         // spacecraft to ground station troposphere correction
         troposphere->SetTemperature(gs->GetRealParameter("Temperature"));
         troposphere->SetPressure(gs->GetRealParameter("Pressure"));
         troposphere->SetHumidityFraction(gs->GetRealParameter("Humidity")/100);
         troposphere->SetWaveLength(wavelength);
         troposphere->SetElevationAngle(elevationAngle);
         troposphere->SetRange(distance*GmatMathConstants::KM_TO_M);
         tropoCorrection = troposphere->Correction();
         //Real rangeCorrection = tropoCorrection[0]/GmatMathConstants::KM_TO_M;
     }
     else
     {
        // @todo: spacecraft to spacraft troposphere correction is not define in MathSpec
        // Now it is set to 0
        tropoCorrection.push_back(0.0);
        tropoCorrection.push_back(0.0);
        tropoCorrection.push_back(0.0);
     }
     
      #ifdef DEBUG_TROPOSPHERE_MEDIA_CORRECTION
         MessageInterface::ShowMessage("       *Run Troposphere media correction:\n");
         MessageInterface::ShowMessage("         .Wave length = %.12lf m\n", wavelength);
         MessageInterface::ShowMessage("         .Elevation angle = %.12lf degree\n", elevationAngle*GmatMathConstants::DEG_PER_RAD);
         MessageInterface::ShowMessage("         .Range correction = %.12lf m\n", tropoCorrection[0]);
      #endif
   }
   else
   {
      tropoCorrection.push_back(0.0);
      tropoCorrection.push_back(0.0);
      tropoCorrection.push_back(0.0);
   }

   return tropoCorrection;
}


//------------------------------------------------------------------------
//RealArray PhysicalMeasurement::IonosphereCorrection(Real freq, Rvector3 r1, Rvector3 r2, Real epoch1, Real epoch2)
//------------------------------------------------------------------------
/**
 * This function is used to calculate Ionosphere correction.
 *
 * @param freq    The frequency of signal                     (unit: MHz)
 * @param r1B     Position of ground station in SSBMJ2000Eq   (unit: km)
 * @param r2B     Position of spacecraft in SSBMJMJ2000Eq     (unit: km)
 * @param epoch1  Time at which the signal is transmitted or received at ground station      (unit: Julian day)
 * @param epoch   Time at which the signal is transmitted or received at spacecraft          (unit: Julian day)
 * return         An array containing results of Ionosphere correction
 */
//------------------------------------------------------------------------
#ifdef IONOSPHERE
RealArray PhysicalSignal::IonosphereCorrection(Real freq, Rvector3 r1B, Rvector3 r2B, Real epoch1, Real epoch2)
{
   RealArray ionoCorrection;
   if (ionosphere != NULL)
   {
      GroundstationInterface* gs = NULL;
      if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         gs = (GroundstationInterface*)theData.tNode;
      if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         gs = (GroundstationInterface*)theData.rNode;
      
      if (gs == NULL)
      {
         ionoCorrection.push_back(0.0);
         ionoCorrection.push_back(0.0);
         ionoCorrection.push_back(0.0);
      }
      else
      {
         // 0. Set ionosphere's ref objects
         ionosphere->SetSolarSystem(solarSystem);

         // 1. Set wave length:
         Real wavelength = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / (freq*1.0e6);      // unit: meter
         ionosphere->SetWaveLength(wavelength);

         // 2. Set time:
         ionosphere->SetTime(epoch1);                                                        // unit: Julian day

         // 3. Set station and spacecraft positions:
         // Create EarthMJ2000Eq coordinate system
         CelestialBody *earthBody = solarSystem->GetBody("Earth");
         CoordinateSystem* fk5cs = CoordinateSystem::CreateLocalCoordinateSystem("Earthfk5", "MJ2000Eq",
               earthBody, NULL, NULL, earthBody, solarSystem);
         // Get Earthfixed coordinate system
         CoordinateSystem* cs = gs->GetBodyFixedCoordinateSystem();
         // Get rotation matrix from EarthMJ2000 cs to Earthfixed cs
         Rvector inState(6, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
         Rvector outState(6, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
         CoordinateConverter* cv = new CoordinateConverter();
         A1Mjd time1(epoch1);                                        // time when signal is transmitted or received at ground station
         A1Mjd time2(epoch2);                                        // time when signal is transmitted or received at spacecraft
         //cv->Convert(time, inState, cs, outState, fk5cs);
         // Rmatrix33 R_g_j2k    = cv->GetLastRotationMatrix().Transpose();
         cv->Convert(time1, inState, fk5cs, outState, cs);
         Rmatrix33 R_g_j2k    = cv->GetLastRotationMatrix();        // rotation matrix converting from fK5 to Earth Bodyfixed at time when ground station transmits or receive signal

         // Specify location of ground station and spacecraft in EarthFixed coordinate system
         SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
         Rvector3 ssb2Earth1 = earthBody->GetMJ2000Position(time1) - ssb->GetMJ2000Position(time1);
         Rvector3 ssb2Earth2 = earthBody->GetMJ2000Position(time1) - ssb->GetMJ2000Position(time2);
         Rvector3 r1 = r1B - ssb2Earth1;                            // ground station position at epoch1 in EarthMJ2000 at epoch1
         Rvector3 r2 = r2B - ssb2Earth2;                            // spacecraft position at epoch2 in EarthMJ2000 at epoch1
         Rvector3 r1_ebf = R_g_j2k*r1;                              // ground station position at epoch1 in EarthBodyFixed at epoch1
         Rvector3 r2_ebf = R_g_j2k*r2;                              // spacecraft position at epoch2 in EarthBodyFixed at epoch1

         ionosphere->SetStationPosition(r1_ebf);                                  // unit: km
         ionosphere->SetSpacecraftPosition(r2_ebf);                               // unit: km

         // 4. Set earth radius:
         SpacePoint* earth    = (SpacePoint*)gs->GetRefObject(Gmat::SPACE_POINT, "Earth");
         Real earthRadius    = earth->GetRealParameter("EquatorialRadius");
         ionosphere->SetEarthRadius(earthRadius);                                 // unit: km

         #ifdef DEBUG_IONOSPHERE_MEDIA_CORRECTION
           MessageInterface::ShowMessage("      *Run Ionosphere media correction for:\n");
           MessageInterface::ShowMessage("         +Earth radius = %lf km\n", earthRadius);
           MessageInterface::ShowMessage("         +Wave length = %.12lf m\n", wavelength);
           MessageInterface::ShowMessage("         +Time = %.12lf\n", epoch1);
           MessageInterface::ShowMessage("         +Station location in Earth body fixed coordinate system (km)   : (%.12lf,  %.12lf,   %.12lf)\n", r1_ebf[0], r1_ebf[1], r1_ebf[2]); 
           MessageInterface::ShowMessage("         +Spacecraft location in Earth body fixed coordinate system (km): (%.12lf,  %.12lf,   %.12lf)\n", r2_ebf[0], r2_ebf[1], r2_ebf[2]);
         #endif

         // 5. Run ionosphere correction:
         ionoCorrection = ionosphere->Correction();
         Real rangeCorrection = ionoCorrection[0]*GmatMathConstants::M_TO_KM;            // unit: meter

         // 6. Clean up
         if (cv)
            delete cv;
         if (fk5cs)
            delete fk5cs;

         #ifdef DEBUG_IONOSPHERE_MEDIA_CORRECTION
           //   MessageInterface::ShowMessage("      *Ionosphere media correction result:\n");
           MessageInterface::ShowMessage("         +Range correction = %.12lf m\n", rangeCorrection*GmatMathConstants::KM_TO_M);
         #endif
      }
   }
   else
   {
      ionoCorrection.push_back(0.0);
      ionoCorrection.push_back(0.0);
      ionoCorrection.push_back(0.0);
   }

   return ionoCorrection;
}
#endif



void PhysicalSignal::SpecifyBeginEndIndexesOfRampTable()
{
   // 1. Get search key
   std::string gsName, scName, gsID, scID, searchkey;
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      gsName = theData.tNode->GetName();
      gsID   = theData.tNode->GetStringParameter("Id");
      scName = theData.rNode->GetName();
      scID   = theData.rNode->GetStringParameter("Id");
   }
   else
   {
      gsName = theData.rNode->GetName();
      gsID   = theData.rNode->GetStringParameter("Id");
      scName = theData.tNode->GetName();
      scID   = theData.tNode->GetStringParameter("Id");
   }
   searchkey = gsID + " " + scID + " ";
   
   // 2. Search for the beginning index
   beginIndex = 0;
   for(; beginIndex < (*rampTable).size(); ++beginIndex)
   {
      if (searchkey == (*rampTable)[beginIndex].indexkey.substr(0,searchkey.size()))
         break;
   }
   
   // 3. Search for the ending index
   endIndex = beginIndex; 
   for(; endIndex < (*rampTable).size(); ++endIndex)
   {
      if (searchkey != (*rampTable)[endIndex].indexkey.substr(0,searchkey.size()))
         break;
   }
   
   // 4. Verify number of data records
   if ((endIndex - beginIndex) < 2)
   {
      std::stringstream ss;
      ss << "Error: Ramp table has " << (endIndex - beginIndex) << " frequency data records for uplink signal from "<< gsName << " to " << scName << ". It needs at least 2 records\n";
      throw MeasurementException(ss.str());
   }
}




//------------------------------------------------------------------------------------------------
// Real PhysicalSignal::GetFrequencyFromRampTable(Real t, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------------------------
/**
* This function is used to get frequency at a given epoch from ramped frequency table
*
* @param t        Epoch (in A1Mjd) at which frequency needed to spicify
* @param rampTB   Table containing information about ramped frequency
*
* return          frequency (unit: Hz) 
* Assumptions: ramp table had been sorted by epoch
*/
//------------------------------------------------------------------------------------------------
Real PhysicalSignal::GetFrequencyFromRampTable(Real t, std::vector<RampTableData>* rampTB)
{
   if (rampTB == NULL)
	   throw MeasurementException("Error: No ramp table available for measurement calculation\n");
   if ((*rampTB).size() == 0)
	   throw MeasurementException("Error: No data is in Ramp table\n");
   
   if (rampTable == NULL)
   {
      rampTable = rampTB;
      SpecifyBeginEndIndexesOfRampTable();
   }

   //if (t <= (*rampTB)[0].epoch)
	  // return (*rampTB)[0].rampFrequency;
   //else if (t >= (*rampTB)[(*rampTB).size()-1].epoch)
	  // return (*rampTB)[(*rampTB).size()-1].rampFrequency;
   if (t <= (*rampTB)[beginIndex].epoch)
	   return (*rampTB)[beginIndex].rampFrequency;
   else if (t >= (*rampTB)[endIndex-1].epoch)
	   return (*rampTB)[endIndex-1].rampFrequency;

   // search for interval which contains time t:
   //UnsignedInt interval_index = 0;
   //for (UnsignedInt i = 1; i < (*rampTB).size(); ++i)
   //{
   //   if (t < (*rampTB)[i].epoch)
	  // {
   //      interval_index = i-1;      
		 //  break;
	  // }
   //}
   UnsignedInt interval_index = beginIndex;
   for (UnsignedInt i = beginIndex+1; i < endIndex; ++i)
   {
      if (t < (*rampTB)[i].epoch)
	   {
         interval_index = i-1;      
		   break;
	   }
   }
   
   // specify frequency at time t:
   Real t_start = (*rampTB)[interval_index].epoch;
   Real f0 = (*rampTB)[interval_index].rampFrequency;
   Real f_dot = (*rampTB)[interval_index].rampRate;
	   
   Real f = f0 + f_dot*(t - t_start);

   return f;
}


//------------------------------------------------------------------------------------------------
// Real PhysicalSignal::GetFrequencyBandFromRampTable(Real t, std::vector<RampTableData>* rampTB)
//------------------------------------------------------------------------------------------------
/**
* This function is used to get frequency band at a given epoch from ramped frequency table
*
* @param t        Epoch (in A1Mjd) at which frequency needed to spicify
* @param rampTB   Table containing information about ramped frequency
*
* return          band index of frequency
* Assumptions: ramp table had been sorted by epoch
*/
//------------------------------------------------------------------------------------------------
Integer PhysicalSignal::GetFrequencyBandFromRampTable(Real t, std::vector<RampTableData>* rampTB)
{
   if (rampTB == NULL)
      throw MeasurementException("Error: No ramp table available for measurement calculation\n");
   if ((*rampTB).size() == 0)
      throw MeasurementException("Error: No data is in ramp table\n");

   if (rampTable == NULL)
   {
      rampTable = rampTB;
      SpecifyBeginEndIndexesOfRampTable();
   }

   //if (t <= (*rampTB)[0].epoch)
   //   return (*rampTB)[0].uplinkBand;
   //else if (t >= (*rampTB)[(*rampTB).size()-1].epoch)
   //   return (*rampTB)[(*rampTB).size()-1].uplinkBand;
   if (t <= (*rampTB)[beginIndex].epoch)
      return (*rampTB)[beginIndex].uplinkBand;
   else if (t >= (*rampTB)[endIndex-1].epoch)
      return (*rampTB)[endIndex-1].uplinkBand;

   // search for interval which contains time t:
   //Real upBand = 0;
   //for (UnsignedInt i = 1; i < (*rampTB).size(); ++i)
   //{
   //   if (t < (*rampTB)[i].epoch)
   //   {
   //      upBand = (*rampTB)[i-1].uplinkBand;
   //      break;
   //   }
   //}
   Real upBand = 0;
   for (UnsignedInt i = beginIndex+1; i < endIndex; ++i)
   {
      if (t < (*rampTB)[i].epoch)
      {
         upBand = (*rampTB)[i-1].uplinkBand;
         break;
      }
   }

   return upBand;
}


//------------------------------------------------------------------------------
// Integer FrequencyBand(Real frequency)
//------------------------------------------------------------------------------
/**
 * Get frequency band corresponding to a given frequency
 *
 * @param frequency   frequency in Hz
 *
 * @return an integer corresponding to frequency band
 */
//------------------------------------------------------------------------------
Integer PhysicalSignal::FrequencyBand(Real frequency)
{
   Integer freqBand = 0;

   // S-band
   if ((frequency >= 2000000000.0) && (frequency <= 4000000000.0))
      freqBand = 1;               // 1 for S-band

   // X-band (Band values from Wikipedia; check them!
   if ((frequency >= 7000000000.0) && (frequency <= 8400000000.0))
      freqBand = 2;               // 2 for X-band

   if (freqBand == 0)
   {
      std::stringstream strs;
      strs << "Error in PhysicalMeasurement::FrequencyBand():  GMAT cannot specify frequency band for frequency = " << frequency <<" Hz\n";
      throw MeasurementException(strs.str());
   }

   return freqBand;
}



