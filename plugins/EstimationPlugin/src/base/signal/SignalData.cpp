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
   tJ2kRotation         (true),                                         // Note that: Rmatrix33 B(true) is a contsructor of Rmatrix33. It defines matrix B to be a 3x3 identity matrix
   rJ2kRotation         (true),                                         // Note that: Rmatrix33 B(true) is a contsructor of Rmatrix33. It defines matrix B to be a 3x3 identity matrix
   tDelay               (0.0),
   rDelay               (0.0),
   solveLightTime       (true),
   feasibility          (true),
   feasibilityReason    ("N"),
   feasibilityValue     (90.0),
   arriveFreq           (-1.0),
   transmitFreq         (0.0),
   receiveFreq          (0.0),
   next                 (NULL)
{
   tSTM.ChangeSize(6,6,true);
   rSTM.ChangeSize(6,6,true);
   tSTMtm.ChangeSize(6,6,true);
   rSTMtm.ChangeSize(6,6,true);
   for(UnsignedInt i = 0; i < 6; ++i)
   {
      tSTM(i,i) = 1.0;
      rSTM(i,i) = 1.0;
      tSTMtm(i,i) = 1.0;
      rSTMtm(i,i) = 1.0;
   }
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
   arriveFreq           (sd.arriveFreq),
   transmitFreq         (sd.transmitFreq),
   receiveFreq          (sd.receiveFreq),
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

      if ((sd.tSTM.GetNumRows() != tSTM.GetNumRows())||(sd.tSTM.GetNumColumns() != tSTM.GetNumColumns()))
         tSTM.ChangeSize(sd.tSTM.GetNumRows(), sd.tSTM.GetNumColumns(), true);
      tSTM                 = sd.tSTM;

      if ((sd.rSTM.GetNumRows() != rSTM.GetNumRows())||(sd.rSTM.GetNumColumns() != rSTM.GetNumColumns()))
         rSTM.ChangeSize(sd.rSTM.GetNumRows(), sd.rSTM.GetNumColumns(), true);
      rSTM                 = sd.rSTM;

      if ((sd.tSTMtm.GetNumRows() != tSTMtm.GetNumRows())||(sd.tSTMtm.GetNumColumns() != tSTMtm.GetNumColumns()))
         tSTMtm.ChangeSize(sd.tSTMtm.GetNumRows(), sd.tSTMtm.GetNumColumns(), true);
      tSTMtm               = sd.tSTMtm;

      if ((sd.rSTMtm.GetNumRows() != rSTMtm.GetNumRows())||(sd.rSTMtm.GetNumColumns() != rSTMtm.GetNumColumns()))
         rSTMtm.ChangeSize(sd.rSTMtm.GetNumRows(), sd.rSTMtm.GetNumColumns(), true);
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
      arriveFreq           = sd.arriveFreq;
      transmitFreq         = sd.transmitFreq;
      receiveFreq          = sd.receiveFreq;
      next                 = NULL;
   }

   return *this;
}
