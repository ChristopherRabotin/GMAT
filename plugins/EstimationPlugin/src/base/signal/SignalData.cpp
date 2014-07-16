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
   tTime                (21545.0),
   rTime                (21545.0),
   tSTM                 (true),
   rSTM                 (true),
   tJ2kRotation         (true),
   rJ2kRotation         (true),
   solveLightTime       (true),
   next                 (NULL)
{
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
   tTime                (sd.tTime),
   rTime                (sd.rTime),
   tLoc                 (sd.tLoc),
   tLocTcs              (sd.tLocTcs),
   rLoc                 (sd.rLoc),
   rLocRcs              (sd.rLocRcs),
   tVel                 (sd.tVel),
   tVelTcs              (sd.tVelTcs),
   rVel                 (sd.rVel),
   rVelRcs              (sd.rVelRcs),
   j2kOriginSep         (sd.j2kOriginSep),
   j2kOriginVel         (sd.j2kOriginVel),
   rangeVecInertial     (sd.rangeVecInertial),
   rangeVecObs          (sd.rangeVecObs),
   rangeRateVecObs      (sd.rangeRateVecObs),
   tSTM                 (sd.tSTM),
   rSTM                 (sd.rSTM),
   tJ2kRotation         (sd.tJ2kRotation),
   rJ2kRotation         (sd.rJ2kRotation),
   correctionIDs        (sd.correctionIDs),
   corrections          (sd.corrections),
   useCorrection        (sd.useCorrection),
   solveLightTime       (sd.solveLightTime),
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
      tTime                = sd.tTime;
      rTime                = sd.rTime;
      stationParticipant   = sd.stationParticipant;
      tLoc                 = sd.tLoc;
      tLocTcs              = sd.tLocTcs;
      rLoc                 = sd.rLoc;
      rLocRcs              = sd.rLocRcs;
      tVel                 = sd.tVel;
      tVelTcs              = sd.tVelTcs;
      rVel                 = sd.rVel;
      rVelRcs              = sd.rVelRcs;
      j2kOriginSep         = sd.j2kOriginSep;
      j2kOriginVel         = sd.j2kOriginVel;
      rangeVecInertial     = sd.rangeVecInertial;
      rangeVecObs          = sd.rangeVecObs;
      rangeRateVecObs      = sd.rangeRateVecObs;
      tSTM                 = sd.tSTM;
      rSTM                 = sd.rSTM;
      tJ2kRotation         = sd.tJ2kRotation;
      rJ2kRotation         = sd.rJ2kRotation;
      correctionIDs        = sd.correctionIDs;
      corrections          = sd.corrections;
      useCorrection        = sd.useCorrection;
      solveLightTime       = sd.solveLightTime;
   }

   return *this;
}
