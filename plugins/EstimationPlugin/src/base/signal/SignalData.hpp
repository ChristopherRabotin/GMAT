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

// Forward references
class SpacePoint;


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

   // The data structures
   /// Name of the starting participant
   std::string transmitParticipant;
   /// Name of the end point participant
   std::string receiveParticipant;
   /// The starting participant
   SpacePoint *tNode;
   /// The end point participant
   SpacePoint *rNode;
   /// Flag indicating if one of the participants is a ground station
   bool stationParticipant;

   /// Transmitter epoch
   GmatEpoch tTime;
   /// Receiver epoch
   GmatEpoch rTime;
   /// MJ2000Eq location of the transmit node
   Rvector3 tLoc;
   /// Location of the transmit node in its coordinate system
   Rvector3 tLocTcs;
   /// MJ2000Eq location of the receive node
   Rvector3 rLoc;
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
   /// Displacement of origins for the transmit and receive nodes
   Rvector3 j2kOriginSep;
   /// Relative velocity between the origins of the participants
   Rvector3 j2kOriginVel;
   /// The Mj2000 equatorial range vector from transmit to receive node
   Rvector3 rangeVecInertial;
   /// The range vector from transmit to receive node in obs coordinates
   Rvector3 rangeVecObs;
   /// The range rate vector from transmit to receive node in obs coordinates
   Rvector3 rangeRateVecObs;

   /// Correction identifiers
   StringArray correctionIDs;
   /// Correction data
   std::vector<Real> corrections;
   /// Flags for the corrections to use
   std::vector<bool> useCorrection;
   /// Flag for light time solution
   bool solveLightTime;

   /// Linked list so separate signal paths are clear
   SignalData *next;
};

#endif /* SignalData_hpp */
