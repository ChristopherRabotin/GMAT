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
 * Container for signal data
 */
//------------------------------------------------------------------------------

#ifndef SignalData_hpp
#define SignalData_hpp

#include "estimation_defs.hpp"
#include "gmatdefs.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"
#include "Rmatrix66.hpp"
#include "Rvector6.hpp"
#include "GmatTime.hpp"

// Forward references
class SpacePoint;
class PropSetup;

#define USE_PRECISION_TIME

/**
 * The SignalData class is a structure for communicating signal data information
 */
class ESTIMATION_API SignalData
{
public:
   SignalData();
   virtual ~SignalData();
   SignalData(const SignalData& sd);
   SignalData& operator=(const SignalData& sd);
   void CleanUp();

   // The data structures
   /// Name of the starting participant
   std::string transmitParticipant;
   /// Name of the end point participant
   std::string receiveParticipant;
   /// The starting participant
   SpacePoint *tNode;
   /// Flag indicating if the transmitter moved during light time iteration
   bool tMovable;
   /// The end point participant
   SpacePoint *rNode;
   /// Flag indicating if the transmitter moved during light time iteration
   bool rMovable;
   /// The propagator used for the transmitter, if used
   PropSetup *tPropagator;
   /// The propagator used for the receiver, if used
   PropSetup *rPropagator;
   /// Flag indicating if one of the participants is a ground station
   bool stationParticipant;

#ifdef USE_PRECISION_TIME
   /// Transmitter epoch
   GmatTime tPrecTime;                                       // made changes by TUAN NGUYEN
   GmatTime rPrecTime;                                       // made changes by TUAN NGUYEN
#else
   /// Transmitter epoch
   GmatEpoch tTime;                                // tTime will be removed and replaced by tPrecTime
   /// Receiver epoch
   GmatEpoch rTime;                                // rTime will be removed and replaced by rPrecTime
#endif

   /// MJ2000Eq location of the transmit node
   Rvector3 tLoc;
   /// SSBMJ2000 state of the transmit node's origin                  // made changes by TUAN NGUYEN
   Rvector6 tOStateSSB;                                               // made changes by TUAN NGUYEN
   /// Location of the transmit node in its coordinate system
   Rvector3 tLocTcs;
   /// MJ2000Eq location of the receive node
   Rvector3 rLoc;
   /// SSBMJ2000 state of the receive node's origin                  // made changes by TUAN NGUYEN
   Rvector6 rOStateSSB;                                              // made changes by TUAN NGUYEN
   /// Location of the receive node in its coordinate system
   Rvector3 rLocRcs;
   /// MJ2000Eq transmitter velocity
   Rvector3 tVel;
   /// Transmitter velocity in its coordinate system
   Rvector3 tVelTcs;
   /// MJ2000Eq receiver velocity
   Rvector3 rVel;
   /// Receiver velocity in its coordinate system
   Rvector3 rVelRcs;
   /// Displacement of origins from the transmit node at time tTime and receive node at time rTime
   Rvector3 j2kOriginSep;
   /// Relative velocity of the origin of the receive node at time rTime w.r.t. the origin of the transmit node at time tTime
   Rvector3 j2kOriginVel;
   /// The SSBMj2000 equatorial range vector from transmit at time tTime to receive node at time rTime
   Rvector3 rangeVecInertial;
   /// Relative velocity of the receive node at time rTime w.r.t. the transmit node at time tTime
   Rvector3 rangeRateVecInertial;                                   // made changes by TUAN NGUYEN
   /// The range vector from transmit to receive node in obs coordinates
   Rvector3 rangeVecObs;
   /// The range rate vector from transmit to receive node in obs coordinates
   Rvector3 rangeRateVecObs;

   /// feasibility
   bool        feasibility;            // Flag indicate signal is feasible          // made changes by TUAN NGUYEN
   std::string feasibilityReason;      // reason why signal is unfeasible           // made changes by TUAN NGUYEN
   Real        feasibilityValue;       // Contain elevation angle                   // made changes by TUAN NGUYEN

   // State Transition Matrices for derivatives of light time measurements
   /// The STM of transmit participant at transmit time t1
   Rmatrix66 tSTM;
   /// The STM of receive participant at receive time t2
   Rmatrix66 rSTM;

   /// The STM of transmit participant at measurement time tm                                                          // made changes by TUAN NGUYEN
   ///(note that : measurement time tm is different from transmit time t1 and receive time t2 due to hardware delay)   // made changes by TUAN NGUYEN
   Rmatrix66 tSTMtm;                                                                                                   // made changes by TUAN NGUYEN
   /// The STM of receive participant at  measurement time tm                                                          // made changes by TUAN NGUYEN
   ///(note that : measurement time tm is different from transmit time t1 and receive time t2 due to hardware delay)   // made changes by TUAN NGUYEN
   Rmatrix66 rSTMtm;                                                                                                   // made changes by TUAN NGUYEN

   /// Rotation matrix from J2K to transmitter coordinate system
   Rmatrix33 tJ2kRotation;
   /// Rotation matrix from J2K to receiver coordinate system
   Rmatrix33 rJ2kRotation;

   /// Correction identifiers
   StringArray correctionIDs;
   /// Correction data
   std::vector<Real> corrections;
   /// Flags for the corrections to use
   std::vector<bool> useCorrection;
   /// Flag for light time solution
   bool solveLightTime;

   /// Hardware delay associated with transmit participant           // made changes by TUAN NGUYEN
   Real tDelay;                                                      // made changes by TUAN NGUYEN
   /// Hardware delay associated with receive participant            // made changes by TUAN NGUYEN
   Real rDelay;                                                      // made changes by TUAN NGUYEN


   /// Linked list so separate signal paths are clear
   SignalData *next;
};

#endif /* SignalData_hpp */
