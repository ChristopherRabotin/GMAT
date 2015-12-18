//$Id$
//------------------------------------------------------------------------------
//                           SignalData
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
// Created: Feb 27, 2014
/**
 * Data used in the measurement modeling subsystem
 */
//------------------------------------------------------------------------------

#include "SignalData.hpp"
#include "PropSetup.hpp"


//------------------------------------------------------------------------------
// SignalData():
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
SignalData::SignalData() :
   transmitParticipant  (""),
   receiveParticipant   (""),
   tNode                (NULL),
   tMovable             (false),
   rNode                (NULL),
   rMovable             (false),
   tPropagator          (NULL),
   rPropagator          (NULL),
   stationParticipant   (false),
   tPrecTime            (21545.0),
   rPrecTime            (21545.0),

#ifndef NEW_TYPE_OF_STATE_TRANSITION_MATRIX                                                                                                                                              // made changes by TUAN NGUYEN
   tSTM                 (true),         //?? tSTM is a Rmatrix66        // Note that: Rmatrix66 A(true) is a contsructor of Rmatrix66. It defines matrix A to be a 6x6 identity matrix   // made changes by TUAN NGUYEN
   rSTM                 (true),         //?? rSTM is a Rmatrix66        // Note that: Rmatrix66 A(true) is a contsructor of Rmatrix66. It defines matrix A to be a 6x6 identity matrix   // made changes by TUAN NGUYEN
#endif                                                                                                                                                                                   // made changes by TUAN NGUYEN

   tJ2kRotation         (true),                                         // Note that: Rmatrix33 B(true) is a contsructor of Rmatrix33. It defines matrix B to be a 3x3 identity matrix
   rJ2kRotation         (true),                                         // Note that: Rmatrix33 B(true) is a contsructor of Rmatrix33. It defines matrix B to be a 3x3 identity matrix
   tDelay               (0.0),
   rDelay               (0.0),
   solveLightTime       (true),
   feasibility          (true),
   feasibilityReason    ("N"),
   feasibilityValue     (90.0),
   arriveFreq           (-1.0),                  // made changes by TUAN NGUYEN
   transmitFreq         (0.0),                   // made changes by TUAN NGUYEN
   receiveFreq          (0.0),                   // made changes by TUAN NGUYEN
   next                 (NULL)
{
#ifdef NEW_TYPE_OF_STATE_TRANSITION_MATRIX      // made changes by TUAN NGUYEN
   tSTM.ChangeSize(6,6,true);                   // made changes by TUAN NGUYEN
   rSTM.ChangeSize(6,6,true);                   // made changes by TUAN NGUYEN
   tSTMtm.ChangeSize(6,6,true);                 // made changes by TUAN NGUYEN
   rSTMtm.ChangeSize(6,6,true);                 // made changes by TUAN NGUYEN
   for(UnsignedInt i = 0; i < 6; ++i)           // made changes by TUAN NGUYEN
   {                                            // made changes by TUAN NGUYEN
      tSTM(i,i) = 1.0;                          // made changes by TUAN NGUYEN
      rSTM(i,i) = 1.0;                          // made changes by TUAN NGUYEN
      tSTMtm(i,i) = 1.0;                        // made changes by TUAN NGUYEN
      rSTMtm(i,i) = 1.0;                        // made changes by TUAN NGUYEN
   }                                            // made changes by TUAN NGUYEN
#endif
}


//------------------------------------------------------------------------------
// ~SignalData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SignalData::~SignalData()
{
   CleanUp();
}


void SignalData::CleanUp()
{
   correctionIDs.clear();
   corrections.clear();
   useCorrection.clear();
}


//------------------------------------------------------------------------------
// SignalData(const SignalData& sd)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param sd The data copied to the new one
 */
//------------------------------------------------------------------------------
SignalData::SignalData(const SignalData& sd) :
   transmitParticipant  (sd.transmitParticipant),
   receiveParticipant   (sd.receiveParticipant),
   tNode                (sd.tNode),
   tMovable             (sd.tMovable),
   rNode                (sd.rNode),
   rMovable             (sd.rMovable),
   tPropagator          (sd.tPropagator),
   rPropagator          (sd.rPropagator),
   stationParticipant   (sd.stationParticipant),
   tPrecTime            (sd.tPrecTime),
   rPrecTime            (sd.rPrecTime),
   tLoc                 (sd.tLoc),
   tOStateSSB           (sd.tOStateSSB),
   tLocTcs              (sd.tLocTcs),
   rLoc                 (sd.rLoc),
   rOStateSSB           (sd.rOStateSSB),
   rLocRcs              (sd.rLocRcs),
   tVel                 (sd.tVel),
   tVelTcs              (sd.tVelTcs),
   rVel                 (sd.rVel),
   rVelRcs              (sd.rVelRcs),
   j2kOriginSep         (sd.j2kOriginSep),
   j2kOriginVel         (sd.j2kOriginVel),
   rangeVecInertial     (sd.rangeVecInertial),
   rangeRateVecInertial (sd.rangeRateVecInertial),
   rangeVecObs          (sd.rangeVecObs),
   rangeRateVecObs      (sd.rangeRateVecObs),
   tSTM                 (sd.tSTM),
   rSTM                 (sd.rSTM),
   tSTMtm               (sd.tSTMtm),
   rSTMtm               (sd.rSTMtm),
   tJ2kRotation         (sd.tJ2kRotation),
   rJ2kRotation         (sd.rJ2kRotation),
   correctionIDs        (sd.correctionIDs),
   corrections          (sd.corrections),
   useCorrection        (sd.useCorrection),
   tDelay               (sd.tDelay),
   rDelay               (sd.rDelay),
   solveLightTime       (sd.solveLightTime),
   feasibility          (sd.feasibility),
   feasibilityReason    (sd.feasibilityReason),
   feasibilityValue     (sd.feasibilityValue),
   arriveFreq           (sd.arriveFreq),                      // made changes by TUAN NGUYEN
   transmitFreq         (sd.transmitFreq),                    // made changes by TUAN NGUYEN
   receiveFreq          (sd.receiveFreq),                     // made changes by TUAN NGUYEN
   next                 (NULL)
{
}


//------------------------------------------------------------------------------
// SignalData& SignalData::operator =(const SignalData& sd)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param sd The data copied to this one
 *
 * @return This onject, set to match sd
 */
//------------------------------------------------------------------------------
SignalData& SignalData::operator=(const SignalData& sd)
{
   if (this != &sd)
   {
      transmitParticipant  = sd.transmitParticipant;
      receiveParticipant   = sd.receiveParticipant;
      tNode                = sd.tNode;
      tMovable             = sd.tMovable;
      rNode                = sd.rNode;
      rMovable             = sd.rMovable;
      tPropagator          = sd.tPropagator;
      rPropagator          = sd.rPropagator;
      tPrecTime            = sd.tPrecTime;
      rPrecTime            = sd.rPrecTime;
      stationParticipant   = sd.stationParticipant;
      tLoc                 = sd.tLoc;
      tOStateSSB           = sd.tOStateSSB;
      tLocTcs              = sd.tLocTcs;
      rLoc                 = sd.rLoc;
      rOStateSSB           = sd.rOStateSSB;
      rLocRcs              = sd.rLocRcs;
      tVel                 = sd.tVel;
      tVelTcs              = sd.tVelTcs;
      rVel                 = sd.rVel;
      rVelRcs              = sd.rVelRcs;
      j2kOriginSep         = sd.j2kOriginSep;
      j2kOriginVel         = sd.j2kOriginVel;
      rangeVecInertial     = sd.rangeVecInertial;
      rangeRateVecInertial = sd.rangeRateVecInertial;
      rangeVecObs          = sd.rangeVecObs;
      rangeRateVecObs      = sd.rangeRateVecObs;
#ifdef NEW_TYPE_OF_STATE_TRANSITION_MATRIX      // made changes by TUAN NGUYEN      
      if ((sd.tSTM.GetNumRows() != tSTM.GetNumRows())||(sd.tSTM.GetNumColumns() != tSTM.GetNumColumns()))            // made changes by TUAN NGUYEN
         tSTM.ChangeSize(sd.tSTM.GetNumRows(), sd.tSTM.GetNumColumns(), true);                                       // made changes by TUAN NGUYEN
#endif
      tSTM                 = sd.tSTM;
#ifdef NEW_TYPE_OF_STATE_TRANSITION_MATRIX      // made changes by TUAN NGUYEN      
      if ((sd.rSTM.GetNumRows() != rSTM.GetNumRows())||(sd.rSTM.GetNumColumns() != rSTM.GetNumColumns()))            // made changes by TUAN NGUYEN
         rSTM.ChangeSize(sd.rSTM.GetNumRows(), sd.rSTM.GetNumColumns(), true);                                       // made changes by TUAN NGUYEN
#endif
      rSTM                 = sd.rSTM;
#ifdef NEW_TYPE_OF_STATE_TRANSITION_MATRIX      // made changes by TUAN NGUYEN      
      if ((sd.tSTMtm.GetNumRows() != tSTMtm.GetNumRows())||(sd.tSTMtm.GetNumColumns() != tSTMtm.GetNumColumns()))    // made changes by TUAN NGUYEN
         tSTMtm.ChangeSize(sd.tSTMtm.GetNumRows(), sd.tSTMtm.GetNumColumns(), true);                                 // made changes by TUAN NGUYEN
#endif
      tSTMtm               = sd.tSTMtm;
#ifdef NEW_TYPE_OF_STATE_TRANSITION_MATRIX      // made changes by TUAN NGUYEN      
      if ((sd.rSTMtm.GetNumRows() != rSTMtm.GetNumRows())||(sd.rSTMtm.GetNumColumns() != rSTMtm.GetNumColumns()))    // made changes by TUAN NGUYEN
         rSTMtm.ChangeSize(sd.rSTMtm.GetNumRows(), sd.rSTMtm.GetNumColumns(), true);                                 // made changes by TUAN NGUYEN
#endif
      rSTMtm               = sd.rSTMtm;
      
      tJ2kRotation         = sd.tJ2kRotation;
      rJ2kRotation         = sd.rJ2kRotation;
      correctionIDs        = sd.correctionIDs;
      corrections          = sd.corrections;
      useCorrection        = sd.useCorrection;
      tDelay               = sd.tDelay;
      rDelay               = sd.rDelay;
      solveLightTime       = sd.solveLightTime;
      feasibility          = sd.feasibility;
      feasibilityReason    = sd.feasibilityReason;
      feasibilityValue     = sd.feasibilityValue;
      arriveFreq           = sd.arriveFreq;                  // made changes by TUAN NGUYEN
      transmitFreq         = sd.transmitFreq;                // made changes by TUAN NGUYEN
      receiveFreq          = sd.receiveFreq;                 // made changes by TUAN NGUYEN 
      next                 = NULL;
   }

   return *this;
}
