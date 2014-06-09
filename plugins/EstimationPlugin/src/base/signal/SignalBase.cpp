//$Id$
//------------------------------------------------------------------------------
//                           SignalBase
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
 * Base class for signals between two measurement participants
 */
//------------------------------------------------------------------------------

#include "SignalBase.hpp"
#include "SpaceObject.hpp"
#include "BodyFixedPoint.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"


//#define DEBUG_INITIALIZATION



//---------------------------------
// static data
//---------------------------------
const Rmatrix33  SignalBase::I33        = Rmatrix33(true);
const Rmatrix33  SignalBase::zero33     = Rmatrix33(false);

const Rvector3   SignalBase::zeroVector = Rvector3(0.0,0.0,0.0);
const Rvector3   SignalBase::xUnit      = Rvector3(1.0,0.0,0.0);
const Rvector3   SignalBase::yUnit      = Rvector3(0.0,1.0,0.0);
const Rvector3   SignalBase::zUnit      = Rvector3(0.0,0.0,1.0);



//------------------------------------------------------------------------------
// SignalBase(const std::string &typeStr, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param typeStr The object type
 * @param name Name of the new object
 */
//------------------------------------------------------------------------------
SignalBase::SignalBase(const std::string &typeStr,
      const std::string &name) :
   GmatBase          (Gmat::GENERIC_OBJECT, typeStr, name),
   next                 (NULL),
   tcs                  (NULL),
   rcs                  (NULL),
   ocs                  (NULL),
   j2k                  (NULL),
   satEpoch             (21545.0),
   signalIsFeasible     (false),       // Not feasible until calculated!
   solarSystem          (NULL),
   navLog               (NULL),
   logLevel             (1)
{
}


//------------------------------------------------------------------------------
// ~SignalBase()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
SignalBase::~SignalBase()
{
   // Clear the entire list
   if (next)
      delete next;
}


//------------------------------------------------------------------------------
// SignalBase(const SignalBase& sb) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param sb The signal base used to create this new one
 */
//------------------------------------------------------------------------------
SignalBase::SignalBase(const SignalBase& sb) :
   GmatBase             (sb),
   next                 (NULL),
   theData              (sb.theData),
   tcs                  (NULL),
   rcs                  (NULL),
   ocs                  (NULL),
   j2k                  (NULL),
   satEpoch             (21545.0),
   signalIsFeasible     (false),       // Never feasible until calculated!
   solarSystem          (sb.solarSystem),
   navLog               (sb.navLog),
   logLevel             (sb.logLevel)
{
   // Clone the list
   if (sb.next)
      Add((SignalBase*)(sb.next->Clone()));
}


//------------------------------------------------------------------------------
// SignalBase& operator=(const SignalBase& sb)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param sb The signal base used to configure this one
 *
 * @return This SignalBase, set to look like sb
 */
//------------------------------------------------------------------------------
SignalBase& SignalBase::operator=(const SignalBase& sb)
{
   if (this != &sb)
   {
      GmatBase::operator=(sb);

      theData             = sb.theData;
      tcs                 = NULL;
      rcs                 = NULL;
      ocs                 = NULL;
      j2k                 = NULL;
      satEpoch            = 21545.0;
      signalIsFeasible    = false;     // Never feasible until calculated!
      solarSystem         = sb.solarSystem;
      navLog              = sb.navLog;
      logLevel            = sb.logLevel;

      if (next)
      {
         delete next;
         next = NULL;
      }
      if (sb.next)
         Add((SignalBase*)(sb.next->Clone()));


      isInitialized = false;
   }

   return *this;
}


//------------------------------------------------------------------------------
// bool SetProgressReporter(ProgressReporter* reporter)
//------------------------------------------------------------------------------
/**
 * Sets the reporter that us used to collect run data
 *
 * @param reporter The progress reporter
 *
 * @return true is the reporter set, false if not
 */
//------------------------------------------------------------------------------
bool SignalBase::SetProgressReporter(ProgressReporter* reporter)
{
   bool retval = false;

   if (reporter)
   {
      navLog = reporter;
      retval = true;
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetTransmitParticipantName(std::string name)
//------------------------------------------------------------------------------
/**
 * Sets the transmit node name
 *
 * @param name The name of the node that transmits the signal
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SignalBase::SetTransmitParticipantName(std::string name)
{
   bool retval = false;
   if (name != "")
   {
      theData.transmitParticipant = name;
      retval = true;
   }
   return retval;
}


//------------------------------------------------------------------------------
// bool SetReceiveParticipantName(std::string name)
//------------------------------------------------------------------------------
/**
 * Sets the receive node name
 *
 * @param name The name of the node that receives the signal
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SignalBase::SetReceiveParticipantName(std::string name)
{
   bool retval = false;
   if (name != "")
   {
      theData.receiveParticipant = name;
      retval = true;
   }
   return retval;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns the list of signal nodes
 *
 * @param type The expected type for the return list (unused in this method)
 *
 * @return The list of nodes
 */
//------------------------------------------------------------------------------
const StringArray& SignalBase::GetRefObjectNameArray(
      const Gmat::ObjectType type)
{
   refObjectNames.clear();
   if (theData.transmitParticipant != "")
      refObjectNames.push_back(theData.transmitParticipant);
   if (theData.receiveParticipant != "")
      refObjectNames.push_back(theData.receiveParticipant);

   return refObjectNames;
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
//       const std::string& name)
//------------------------------------------------------------------------------
/**
 * Sets the reference object pointers for the signal nodes
 *
 * @param obj The reference object
 * @param type The type of the object (unused here)
 * @param name The object's name
 *
 * @return true if the object was set, false if not
 */
//------------------------------------------------------------------------------
bool SignalBase::SetRefObject(GmatBase* obj, const Gmat::ObjectType type,
      const std::string& name)
{
   bool retval = false;

   if (obj != NULL)
   {
      if (obj->IsOfType("SpacePoint"))
      {
         if (theData.transmitParticipant == name)
         {
            theData.tNode = (SpacePoint*)obj;
            retval = true;
         }
         if (theData.receiveParticipant == name)
         {
            theData.rNode = (SpacePoint*)obj;
            retval = true;
         }
      }
   }

   if (retval == false)
      retval = GmatBase::SetRefObject(obj, type, name);

   return retval;
}


//------------------------------------------------------------------------------
// bool RenameRefObject(const Gmat::ObjectType type, const std::string& oldName,
//       const std::string& newName)
//------------------------------------------------------------------------------
/**
 * Renames the nodes when the core object name is changed
 *
 * @param type The node type (unused here)
 * @param oldName The name the object had before the rename
 * @param newName The name the object has after the rename
 *
 * @return true is a name was changed, false if not
 */
//------------------------------------------------------------------------------
bool SignalBase::RenameRefObject(const Gmat::ObjectType type,
      const std::string& oldName, const std::string& newName)
{
   bool retval = false;

   if (oldName == theData.transmitParticipant)
   {
      theData.transmitParticipant = newName;
      retval = true;
   }

   else if (oldName == theData.receiveParticipant)
   {
      theData.receiveParticipant = newName;
      retval = true;
   }
   else retval = GmatBase::RenameRefObject(type, oldName, newName);

   return retval;
}


//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem* ss)
//------------------------------------------------------------------------------
/**
 * Adds the solar system configuration to the signal
 *
 * @param ss The solar system in use in the run
 */
//------------------------------------------------------------------------------
void SignalBase::SetSolarSystem(SolarSystem* ss)
{
   solarSystem = ss;
}


//------------------------------------------------------------------------------
// SignalBase* GetNext()
//------------------------------------------------------------------------------
/**
 * Returns the next node in the list
 *
 * @return The node pointer
 */
//------------------------------------------------------------------------------
SignalBase* SignalBase::GetNext()
{
   return next;
}


//------------------------------------------------------------------------------
// bool Add(SignalBase* signalToAdd)
//------------------------------------------------------------------------------
/**
 * Adds a signal to a signal path
 *
 * @param signalToAdd The signal that is added to the path
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool SignalBase::Add(SignalBase* signalToAdd)
{
   if (next)
      next->Add(signalToAdd);
   else
   {
      next = signalToAdd;
      theData.next = &(signalToAdd->GetSignalData());
   }

   return true;
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepared the signal for processing by validating internal members
 *
 * @return true if the signal can be processed, false if not
 */
//------------------------------------------------------------------------------
bool SignalBase::Initialize()
{
   bool retval = false;

   if (GmatBase::Initialize())
   {
      if (navLog)
         logLevel = navLog->GetLogLevel("Signal");

      if (theData.tNode && theData.rNode)
      {
         theData.stationParticipant =
               ((theData.tNode->IsOfType(Gmat::GROUND_STATION)) ||
                (theData.rNode->IsOfType(Gmat::GROUND_STATION)));
         retval = true;
      }

      if (next)
         retval = next->Initialize() && retval;

      // Unset the init flag so InitializeSignal will get called
      isInitialized = false;
   }

   return retval;
}


//------------------------------------------------------------------------------
// std::string GetPathDescription()
//------------------------------------------------------------------------------
/**
 * Returns the path description
 *
 * @return The description, enclosed in curly braces
 */
//------------------------------------------------------------------------------
std::string SignalBase::GetPathDescription(bool fullList)
{
   std::string retval = "{";
   if (theData.transmitParticipant != "")
      retval += theData.transmitParticipant;
   else
      retval += "transmit participant not set";
   retval += " -> ";
   if (theData.receiveParticipant != "")
      retval += theData.receiveParticipant;
   else
      retval += "receive participant not set";
   retval += "}";

   if (fullList)
   {
      if (next)
         retval += next->GetPathDescription();
   }

   return retval;
}


SignalData& SignalBase::GetSignalData()
{
   return theData;
}


//------------------------------------------------------------------------------
// bool SignalBase::IsSignalFeasible()
//------------------------------------------------------------------------------
/**
 * Retuns feasibility for the last signal calculated
 *
 * @return true if the signal is feasible, false if not
 */
//------------------------------------------------------------------------------
bool SignalBase::IsSignalFeasible()
{
   bool retval = signalIsFeasible;

   if (next)
      retval = retval && next->IsSignalFeasible();

   return retval;
}


//------------------------------------------------------------------------------
// void InitializeSignal()
//------------------------------------------------------------------------------
/**
 * Prepares the signal for use
 */
//------------------------------------------------------------------------------
void SignalBase::InitializeSignal()
{
   if (isInitialized)
   {
      return;
   }

   SpaceObject *spObj  = NULL;
   SpacePoint  *origin = NULL;
   GmatEpoch   satEpoch = 0.0;
   CelestialBody *earth = solarSystem->GetBody("Earth");

   if (theData.tNode->IsOfType(Gmat::SPACEOBJECT))
   {
      satEpochID = theData.tNode->GetParameterID("A1Epoch");
      satEpoch   = theData.tNode->GetRealParameter(satEpochID);
   }
   else if (theData.rNode->IsOfType(Gmat::SPACEOBJECT))
   {
      satEpochID = theData.rNode->GetParameterID("A1Epoch");
      satEpoch   = theData.rNode->GetRealParameter(satEpochID);
   }

   theData.tTime = theData.rTime = satEpoch;

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Initializing with epoch %.12lf\n",
            satEpoch);
   #endif

   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      BodyFixedPoint *bf = (BodyFixedPoint*)theData.tNode;
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("   bf  = %p\n", bf);
      #endif
      tcs = bf->GetBodyFixedCoordinateSystem();
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("   tcs = %p\n", rcs);
         MessageInterface::ShowMessage("   ss  = %p\n", solarSystem);
      #endif

      spObj  = (SpaceObject*) theData.rNode;
      origin = spObj->GetOrigin();
      rcs    = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
               origin, NULL, NULL, earth, solarSystem);

      ocs     = CoordinateSystem::CreateLocalCoordinateSystem("OCS","Topocentric",
               bf, NULL, NULL, bf->GetJ2000Body(), solarSystem);
      j2k    = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
               tcs->GetOrigin(), NULL, NULL, earth, solarSystem);
   }
   else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
   {
      BodyFixedPoint *bf = (BodyFixedPoint*)theData.rNode;
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("   bf  = %p\n", bf);
      #endif
      rcs = bf->GetBodyFixedCoordinateSystem();
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("   rcs = %p\n", rcs);
         MessageInterface::ShowMessage("   ss  = %p\n", solarSystem);
      #endif


      spObj  = (SpaceObject*) theData.tNode;
      origin = spObj->GetOrigin();

      tcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
               origin, NULL, NULL, earth, solarSystem);
      // Set up Fo, centered at the GroundStation
      ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS","Topocentric",
               bf, NULL, NULL, earth, solarSystem);
      j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
               rcs->GetOrigin(), NULL, NULL, earth, solarSystem);
   }
   else
   {
      origin = ((SpaceObject*)theData.tNode)->GetOrigin();
      rcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
              origin, NULL, NULL, earth, solarSystem);
      tcs = CoordinateSystem::CreateLocalCoordinateSystem("TCS", "MJ2000Eq",
              origin, NULL, NULL, earth, solarSystem);
      ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS","MJ2000Eq",
              origin, NULL, NULL, earth, solarSystem);
      j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
              origin, NULL, NULL, earth, solarSystem);
   }

   std::string updateAll = "All";
   UpdateRotationMatrix(satEpoch, updateAll);

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Late Binding Initialization complete:\n"
            "   SolarSystem pointer:  %p\n"
            "   Transmitter CS:       %p\n"
            "   Receiver CS:          %p\n"
            "   Observation CS:       %p\n"
            "   J2000 CS:             %p\n",
            solarSystem, tcs, rcs, ocs, j2k);
   #endif

   isInitialized = true;
}


//------------------------------------------------------------------------------
// void CalculateRangeVectorInertial()
//------------------------------------------------------------------------------
/**
 * Computes the range vector in MJ2000 Equatorial coordinates.
 *
 * The default implementation here performs the instantaneous range vector
 * calculation.  Override this implementation for the light time corrected
 * vector.
 */
//------------------------------------------------------------------------------
void SignalBase::CalculateRangeVectorInertial()
{
   if (theData.tNode->IsOfType(Gmat::SPACECRAFT))
      satEpoch = theData.tNode->GetRealParameter(satEpochID);
   else if (theData.rNode->IsOfType(Gmat::SPACECRAFT))
      satEpoch = theData.rNode->GetRealParameter(satEpochID);
   else
      throw MeasurementException("Error in CoreMeasurement::"
            "CalculateRangeVectorInertial; neither participant is a "
            "spacecraft.");

   theData.tTime = theData.rTime = satEpoch;

   std::string updateAll = "All";
   UpdateRotationMatrix(satEpoch, updateAll);
   SpacePoint *origin1 = tcs->GetOrigin();
   SpacePoint *origin2 = rcs->GetOrigin();

   theData.j2kOriginSep        = origin2->GetMJ2000Position(satEpoch) -
                         origin1->GetMJ2000Position(satEpoch);
   theData.tLoc = theData.tNode->GetMJ2000Position(satEpoch);
   theData.rLoc = theData.rNode->GetMJ2000Position(satEpoch);

   theData.rangeVecInertial = theData.rLoc - theData.j2kOriginSep -
         theData.tLoc;
}

//------------------------------------------------------------------------------
// void CalculateRangeVectorObs()
//------------------------------------------------------------------------------
/**
 * Computes the range vector in the observation coordinate system
 *
 * This method calls CalculateRangeVectorInertial() to generate a range vector,
 * then transforms that vector into the coordinate system needed for the
 * observation data.
 */
//------------------------------------------------------------------------------
void SignalBase::CalculateRangeVectorObs()
{
   CalculateRangeVectorInertial();

   // compute the positions of the participants in their own frames
   theData.tLocTcs = R_Transmitter_j2k * theData.tLoc;
   theData.rLocRcs = R_Receiver_j2k * theData.rLoc;

   // @todo: handle this for arbitrary participants; currently rotates if there
   // are any GS's in the participant list
   if (theData.stationParticipant)
      theData.rangeVecObs = R_Obs_j2k * theData.rangeVecInertial;
   else
      theData.rangeVecObs = theData.rangeVecInertial;  // same, really, but skip
                                                       // multiplication by I33
}

//------------------------------------------------------------------------------
// void CalculateRangeRateVectorObs(Integer p1index, Integer p2index)
//------------------------------------------------------------------------------
/**
 * Computes the range rate vector in the observation coordinate system
 *
 * This method calls CalculateRangeVectorInertial() to generate a range vector,
 * then transforms that vector into the coordinate system needed for the
 * observation data.
 */
//------------------------------------------------------------------------------
void SignalBase::CalculateRangeRateVectorObs()
{
   Real satEpoch;
   if (theData.tNode->IsOfType(Gmat::SPACECRAFT))
      satEpoch = theData.tNode->GetRealParameter(satEpochID);
   else if (theData.rNode->IsOfType(Gmat::SPACECRAFT))
      satEpoch = theData.rNode->GetRealParameter(satEpochID);
   else
      throw MeasurementException("Error in CoreMeasurement::"
            "CalculateRangeVectorInertial; neither participant is a "
            "spacecraft.");

   theData.tTime = theData.rTime = satEpoch;

   SpacePoint *origin1 = tcs->GetOrigin();
   SpacePoint *origin2 = rcs->GetOrigin();

   theData.j2kOriginVel = origin2->GetMJ2000Velocity(satEpoch) -
                          origin1->GetMJ2000Velocity(satEpoch);

   theData.tVel = theData.tNode->GetMJ2000Velocity(satEpoch);
   theData.rVel = theData.rNode->GetMJ2000Velocity(satEpoch);

   // compute the velocities of the participants in their own frames

   // need p1Loc, p2Loc, and r12_j2k, as well as rotation matrices
   CalculateRangeVectorObs();
   theData.tVelTcs = R_Transmitter_j2k * theData.tVel;
   theData.rVelRcs = R_Receiver_j2k * theData.rVel;
   if (theData.stationParticipant)
   {
      theData.rangeRateVecObs = // Receiver terms
                        (R_Obs_Receiver * theData.rVelRcs) +
                        (RDot_Obs_Receiver * theData.rLocRcs) -
                        // Origin terms
                        (R_Obs_j2k * theData.j2kOriginVel) -
                        (RDot_Obs_j2k * theData.j2kOriginSep) -
                        // Transmitter terms
                        (R_Obs_Transmitter * theData.tVelTcs) -
                        (RDot_Obs_Transmitter * theData.tLocTcs);
   }
   else // Rs are I33, RDots are 033
      theData.rangeRateVecObs = theData.rVelRcs - theData.j2kOriginVel -
            theData.tVelTcs;
}

//------------------------------------------------------------------------------
// void UpdateRotationMatrix(Real atEpoch, const std::string& whichOne)
//------------------------------------------------------------------------------
/**
 * Updates the rotation matrices for the passed-in epoch
 *
 * @param atEpoch The epoch of the update
 * @param whichOne Identity of the matrix to update, or "All" for all
 */
//------------------------------------------------------------------------------
void SignalBase::UpdateRotationMatrix(Real atEpoch, const std::string& whichOne)
{
   if ((theData.tNode->IsOfType(Gmat::GROUND_STATION)) ||
       (theData.rNode->IsOfType(Gmat::GROUND_STATION)))
   {
      Rvector6   dummyIn(1.0,2.0,3.0,  4.0,5.0,6.0);
      Rvector6   dummyOut;
      A1Mjd      itsEpoch(atEpoch);

      if ((whichOne == "All") || (whichOne == "j2k_2"))
      {
         converter.Convert(itsEpoch,dummyIn,rcs,dummyOut,j2k);
         R_j2k_Receiver = converter.GetLastRotationMatrix();
         //               RDot_j2k_2 = converter.GetLastRotationDotMatrix();
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,rcs);
         R_Receiver_j2k    = converter.GetLastRotationMatrix();
      }
      if ((whichOne == "All") || (whichOne == "j2k_1"))
      {
         converter.Convert(itsEpoch,dummyIn,tcs,dummyOut,j2k);
         R_j2k_Transmitter    = converter.GetLastRotationMatrix();
         //               RDot_j2k_1 = converter.GetLastRotationDotMatrix();
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,tcs);
         R_Transmitter_j2k    = converter.GetLastRotationMatrix();
      }
      if ((whichOne == "All") || (whichOne == "o_2"))
      {
         converter.Convert(itsEpoch,dummyIn,rcs,dummyOut,ocs);
         R_Obs_Receiver      = converter.GetLastRotationMatrix();
         RDot_Obs_Receiver   = converter.GetLastRotationDotMatrix();
      }
      if ((whichOne == "All") || (whichOne == "o_1"))
      {
         converter.Convert(itsEpoch,dummyIn,tcs,dummyOut,ocs);
         R_Obs_Transmitter      = converter.GetLastRotationMatrix();
         RDot_Obs_Transmitter   = converter.GetLastRotationDotMatrix();
      }
      if ((whichOne == "All") || (whichOne == "o_j2k"))
      {
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,ocs);
         R_Obs_j2k    = converter.GetLastRotationMatrix();
         RDot_Obs_j2k = converter.GetLastRotationDotMatrix();
      }
   }
   else
   {
      // When both participants are spacecraft, all frames are inertial, so all
      // rotation matrices are I33 (which is the default on creation) and all
      // Rotation Dot matrices are zero33
      RDot_Obs_Receiver    = zero33;
      RDot_Obs_Transmitter = zero33;
      RDot_Obs_j2k         = zero33;
   }
}


//-----------------------------------------------------------------------------
// Integer GetParmIdFromEstID(Integer forId, GmatBase *obj)
//-----------------------------------------------------------------------------
/**
 * Uses a measurement ID and object to convert that id into a parameter ID
 *
 * @param forId The measurement ID
 * @param obj The object that owns the id
 *
 * @return The parameter ID on the object
 */
//-----------------------------------------------------------------------------
Integer SignalBase::GetParmIdFromEstID(Integer forId, GmatBase *obj)
{
   return forId - obj->GetType() * 250;
}

