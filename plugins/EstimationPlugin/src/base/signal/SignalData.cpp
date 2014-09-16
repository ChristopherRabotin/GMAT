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
#ifdef USE_PRECISION_TIME
   tPrecTime            (21545.0),                  // made changes by TUAN NGUYEN
   rPrecTime            (21545.0),                  // made changes by TUAN NGUYEN
#else
   tTime                (21545.0),
   rTime                (21545.0),
#endif
   tSTM                 (true),
   rSTM                 (true),
   tJ2kRotation         (true),
   rJ2kRotation         (true),
   tDelay               (0.0),                      // made changes by TUAN NGUYEN
   rDelay               (0.0),                      // made changes by TUAN NGUYEN
   solveLightTime       (true),
   feasibility          (true),                     // made changes by TUAN NGUYEN
   feasibilityReason    ("N"),                      // made changes by TUAN NGUYEN
   feasibilityValue     (90.0),                     // made changes by TUAN NGUYEN
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
#ifdef USE_PRECISION_TIME
   tPrecTime            (sd.tPrecTime),                  // made changes by TUAN NGUYEN
   rPrecTime            (sd.rPrecTime),                  // made changes by TUAN NGUYEN
#else
   tTime                (sd.tTime),
   rTime                (sd.rTime),
#endif
   tLoc                 (sd.tLoc),
   tOStateSSB           (sd.tOStateSSB),                // made changes by TUAN NGUYEN
   tLocTcs              (sd.tLocTcs),
   rLoc                 (sd.rLoc),
   rOStateSSB           (sd.rOStateSSB),                // made changes by TUAN NGUYEN
   rLocRcs              (sd.rLocRcs),
   tVel                 (sd.tVel),
   tVelTcs              (sd.tVelTcs),
   rVel                 (sd.rVel),
   rVelRcs              (sd.rVelRcs),
   j2kOriginSep         (sd.j2kOriginSep),
   j2kOriginVel         (sd.j2kOriginVel),
   rangeVecInertial     (sd.rangeVecInertial),
   rangeRateVecInertial (sd.rangeRateVecInertial),      // made changes by TUAN NGUYEN
   rangeVecObs          (sd.rangeVecObs),
   rangeRateVecObs      (sd.rangeRateVecObs),
   tSTM                 (sd.tSTM),
   rSTM                 (sd.rSTM),
   tJ2kRotation         (sd.tJ2kRotation),
   rJ2kRotation         (sd.rJ2kRotation),
   correctionIDs        (sd.correctionIDs),
   corrections          (sd.corrections),
   useCorrection        (sd.useCorrection),
   tDelay               (sd.tDelay),                    // made changes by TUAN NGUYEN
   rDelay               (sd.rDelay),                    // made changes by TUAN NGUYEN
   solveLightTime       (sd.solveLightTime),
   feasibility          (sd.feasibility),               // made changes by TUAN NGUYEN
   feasibilityReason    (sd.feasibilityReason),         // made changes by TUAN NGUYEN
   feasibilityValue     (sd.feasibilityValue),          // made changes by TUAN NGUYEN
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
#ifdef USE_PRECISION_TIME
      tPrecTime            = sd.tPrecTime;               // made changes by TUAN NGUYEN
      rPrecTime            = sd.rPrecTime;               // made changes by TUAN NGUYEN
#else
      tTime                = sd.tTime;
      rTime                = sd.rTime;
#endif
      stationParticipant   = sd.stationParticipant;
      tLoc                 = sd.tLoc;
      tOStateSSB           = sd.tOStateSSB;              // made changes by TUAN NGUYEN
      tLocTcs              = sd.tLocTcs;
      rLoc                 = sd.rLoc;
      rOStateSSB           = sd.rOStateSSB;              // made changes by TUAN NGUYEN
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
      tSTM                 = sd.tSTM;
      rSTM                 = sd.rSTM;
      tJ2kRotation         = sd.tJ2kRotation;
      rJ2kRotation         = sd.rJ2kRotation;
      correctionIDs        = sd.correctionIDs;
      corrections          = sd.corrections;
      useCorrection        = sd.useCorrection;
      tDelay               = sd.tDelay;                  // made changes by TUAN NGUYEN
      rDelay               = sd.rDelay;                  // made changes by TUAN NGUYEN
      solveLightTime       = sd.solveLightTime;
      feasibility          = sd.feasibility;             // made changes by TUAN NGUYEN
      feasibilityReason    = sd.feasibilityReason;       // made changes by TUAN NGUYEN
      feasibilityValue     = sd.feasibilityValue;        // made changes by TUAN NGUYEN
      next                 = NULL;
   }

   return *this;
}
