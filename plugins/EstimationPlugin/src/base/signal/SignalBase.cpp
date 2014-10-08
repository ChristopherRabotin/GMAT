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
#include "PropSetup.hpp"
#include "Propagator.hpp"
#include "ODEModel.hpp"
#include "SpaceObject.hpp"
#include <sstream>

//#define DEBUG_INITIALIZATION
//#define DEBUG_LIGHTTIME
//#define DEBUG_MOVETOEPOCH

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
   GmatBase             (Gmat::GENERIC_OBJECT, typeStr, name),
   next                 (NULL),
   previous             (NULL),
   tcs                  (NULL),
   rcs                  (NULL),
   ocs                  (NULL),
   j2k                  (NULL),
#ifdef USE_PRECISION_TIME
   satPrecEpoch         (21545.0),            // made changes by TUAN NGUYEN
#else
   satEpoch             (21545.0),
#endif
   satEpochID           (-1),
   signalIsFeasible     (false),       // Not feasible until calculated!
   includeLightTime     (true),
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

   if (tcs) 
      delete tcs;
   if (rcs)
      delete rcs;
   if (ocs)
      delete ocs;
   if (j2k)
      delete j2k;

   theData.CleanUp();              // made changes by TUAN NGUYEN
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
   previous             (NULL),
   theData              (sb.theData),
   tcs                  (NULL),
   rcs                  (NULL),
   ocs                  (NULL),
   j2k                  (NULL),
#ifdef USE_PRECISION_TIME
   satPrecEpoch         (21545.0),      // made changes by TUAN NGUYEN
#else
   satEpoch             (21545.0),
#endif
   satEpochID           (-1),
   signalIsFeasible     (false),       // Never feasible until calculated!
   includeLightTime     (sb.includeLightTime),
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
      
      if (tcs) 
         delete tcs;
      tcs = NULL;
      if (rcs)
         delete rcs;
      rcs = NULL;
      if (ocs)
         delete ocs;
      ocs = NULL;
      if (j2k)
         delete j2k;
      j2k = NULL;

#ifdef USE_PRECISION_TIME
      satPrecEpoch        = 21545.0;            // made changes by TUAN NGUYEN
#else
      satEpoch            = 21545.0;
#endif
      satEpochID          = -1;
      signalIsFeasible    = false;     // Never feasible until calculated!
      includeLightTime    = sb.includeLightTime;
      solarSystem         = sb.solarSystem;
      navLog              = sb.navLog;
      logLevel            = sb.logLevel;

      if (next)
      {
         delete next;
         next = NULL;
      }
      previous = NULL;
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
         satEpochID = obj->GetParameterID("A1Epoch");
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
// SignalBase* GetStart(bool epochIsAtEnd)
//------------------------------------------------------------------------------
/**
 * Finds either the first or last node in the signal path
 *
 * @param epochIsAtEnd true to get the last node, false to get the first
 *
 * @return
 */
//------------------------------------------------------------------------------
SignalBase* SignalBase::GetStart(bool epochIsAtEnd)
{
   SignalBase *retval = this;
   if (epochIsAtEnd)
   {
      if (next)
         retval = next->GetStart(epochIsAtEnd);
   }
   else
   {
      if (previous)
         retval = previous->GetStart(epochIsAtEnd);
   }
   return retval;
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
      signalToAdd->SetPrevious(this);
   }

   return true;
}


//------------------------------------------------------------------------------
// void SetPropagator(PropSetup* propagator, GmatBase* forObj)
//------------------------------------------------------------------------------
/**
 * Clones propagators for use moving objects while finding light time solutions
 *
 * This method is used to assign propagators to signal participants so that
 * the iteration needed for light time computations can be performed.  The
 * derived signal classes initialize the propagator for the specific participant
 * it uses.
 *
 * This code is not likely to change when support for multiple propagators is
 * implemented.  Dense output propagators may impact this code, because the
 * current approach involves cloning the propagator, resulting in a reset of the
 * Internal data.
 *
 * @param propagator The propagator being passed to the signal
 * @param forObj The participant that uses this propagator; if NULL it is set
 *               for both the transmitter and receiver
 */
//------------------------------------------------------------------------------
void SignalBase::SetPropagator(PropSetup* propagator, GmatBase* forObj)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Setting a propagator <%p> in the Signal "
            "class for %s\n", propagator, (forObj == NULL ?
            "both participants" : forObj->GetName().c_str()));
   #endif
   if (propagator == NULL)
      throw MeasurementException("The propagator passed in for the object " +
            forObj->GetName() + " is NULL, so the object cannot be propagated "
                  "for light time evaluation");

   if (theData.solveLightTime)
   {
      if ((theData.tNode == forObj) || (forObj == NULL))
      {
         theData.tPropagator = propagator;
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("  Transmitter propagator set\n");
         #endif
      }
      if ((theData.rNode == forObj) || (forObj == NULL))
      {
         theData.rPropagator = propagator;
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("  Receiver propagator set\n");
         #endif
      }

      if (next)
         next->SetPropagator(propagator, forObj);
   }
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
      else
         logLevel = 32767;

      if (theData.tNode && theData.rNode)
      {
         // 1. Set value for theData.stationParticipant
         theData.stationParticipant =
               ((theData.tNode->IsOfType(Gmat::GROUND_STATION)) ||
                (theData.rNode->IsOfType(Gmat::GROUND_STATION)));

         #ifdef USE_PRECISION_TIME
         // 2. Update theData.tPrecTime and theData.rPrecTime
         theData.tPrecTime = theData.tNode->GetEpoch();                // made changes by TUAN NGUYEN
         theData.rPrecTime = theData.rNode->GetEpoch();                // made changes by TUAN NGUYEN
         // Initially, ground station epoch is set the same at epoch for spacecraft
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))            // made changes by TUAN NGUYEN
            theData.tPrecTime = theData.rPrecTime;                     // made changes by TUAN NGUYEN
         else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))       // made changes by TUAN NGUYEN
            theData.rPrecTime = theData.tPrecTime;                     // made changes by TUAN NGUYEN
         #else
         // 2. Update theData.tTime and theData.rTime
         theData.tTime = theData.tNode->GetEpoch();                    // made changes by TUAN NGUYEN
         theData.rTime = theData.rNode->GetEpoch();                    // made changes by TUAN NGUYEN
         #endif

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


//------------------------------------------------------------------------------
// SignalData& GetSignalData()
//------------------------------------------------------------------------------
/**
 * Retrieves the most recently calculated data set for the signal
 *
 * @return The data set
 */
//------------------------------------------------------------------------------
SignalData& SignalBase::GetSignalData()
{
   return theData;
}


//------------------------------------------------------------------------------
// SignalData* GetSignalDataObject()
//------------------------------------------------------------------------------
/**
 * Retrieves the reference to the SignalData object of the signal
 *
 * @return The data set
 */
//------------------------------------------------------------------------------
SignalData* SignalBase::GetSignalDataObject()
{
   return &theData;
}


//------------------------------------------------------------------------------
// void SetSignalData(const SignalData& newData)
//------------------------------------------------------------------------------
/**
 * Receives signal data from an outside source
 *
 * @param newData The new signal data
 */
//------------------------------------------------------------------------------
void SignalBase::SetSignalData(const SignalData& newData)
{
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("<%p>: Receiving data to update %s and %s...",
            this, theData.transmitParticipant.c_str(),
            theData.receiveParticipant.c_str());
      Integer count = 0;
   #endif

   // Pass in the current computed data
   if (theData.receiveParticipant == newData.receiveParticipant)
   {
#ifdef USE_PRECISION_TIME
      theData.rPrecTime = newData.rPrecTime;                     // made changes by TUAN NGUYEN
#else
      theData.rTime = newData.rTime;
#endif
      theData.rLoc  = newData.rLoc;
      theData.rVel  = newData.rVel;
      #ifdef DEBUG_LIGHTTIME
         ++count;
      #endif
   }
   if (theData.receiveParticipant == newData.transmitParticipant)
   {
#ifdef USE_PRECISION_TIME
      theData.rPrecTime = newData.tPrecTime;                     // made changes by TUAN NGUYEN
#else
      theData.rTime = newData.tTime;
#endif
      theData.rLoc  = newData.tLoc;
      theData.rVel  = newData.tVel;
      #ifdef DEBUG_LIGHTTIME
         ++count;
      #endif
   }
   if (theData.transmitParticipant == newData.receiveParticipant)
   {
#ifdef USE_PRECISION_TIME
      theData.tPrecTime = newData.rPrecTime;                     // made changes by TUAN NGUYEN
#else
      theData.tTime = newData.rTime;
#endif
      theData.tLoc  = newData.rLoc;
      theData.tVel  = newData.rVel;
      #ifdef DEBUG_LIGHTTIME
         ++count;
      #endif
   }
   if (theData.transmitParticipant == newData.transmitParticipant)
   {
#ifdef USE_PRECISION_TIME
      theData.tPrecTime = newData.tPrecTime;                     // made changes by TUAN NGUYEN
#else
      theData.tTime = newData.tTime;
#endif
      theData.tLoc  = newData.tLoc;
      theData.tVel  = newData.tVel;
      #ifdef DEBUG_LIGHTTIME
         ++count;
      #endif
   }

   #ifdef DEBUG_LIGHTTIME
      #ifdef USE_PRECISION_TIME
      MessageInterface::ShowMessage("%d matching data sets updated; epochs "                        // made changes by TUAN NGUYEN
            "%.12lf -> %.12lf\n", count, theData.tPrecTime.GetMjd(), theData.rPrecTime.GetMjd());   // made changes by TUAN NGUYEN
      #else
      MessageInterface::ShowMessage("%d matching data sets updated; epochs "
            "%.12lf -> %.12lf\n", count, theData.tTime, theData.rTime);
      #endif
   #endif
}

//------------------------------------------------------------------------------
// bool IsSignalFeasible()
//------------------------------------------------------------------------------
/**
 * Returns feasibility for the last signal calculated
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
// void SignalBase::UsesLighttime(const bool tf)
//------------------------------------------------------------------------------
/**
 * Manages the light time computation flag
 *
 * @param tf true to include light time solutions, false to omit them
 */
//------------------------------------------------------------------------------
void SignalBase::UsesLighttime(const bool tf)
{
   includeLightTime = tf;
   if (next)
      next->UsesLighttime(tf);

}


//------------------------------------------------------------------------------
// void InitializeSignal()
//------------------------------------------------------------------------------
/**
 * Prepares the signal for use
 */
//------------------------------------------------------------------------------
void SignalBase::InitializeSignal(bool chainForwards)
{
   #ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("SignalBase::InitializeSignal() for leg %s to %s\n", theData.tNode->GetName().c_str(), theData.rNode->GetName().c_str());
   #endif

   if (isInitialized)
   {
      return;
   }

   SpaceObject *spObj  = NULL;
   SpacePoint  *origin = NULL;
   CelestialBody *earth = solarSystem->GetBody("Earth");

#ifdef USE_PRECISION_TIME
   GmatTime gsPrecEpoch = theData.tPrecTime;                  // made changes by TUAN NGUYEN
#else
   GmatEpoch gsEpoch = theData.tTime;
#endif

   #ifdef DEBUG_INITIALIZATION
#ifdef USE_PRECISION_TIME
      MessageInterface::ShowMessage("Initializing with epoch %.12lf\n", satPrecEpoch.GetMjd());      // made changes by TUAN NGUYEN
#else
      MessageInterface::ShowMessage("Initializing with epoch %.12lf\n", satEpoch);
#endif
   #endif


   // 2. Set tcs, rcs, ocs, and j2k coordinate systems
   if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
   {
      BodyFixedPoint *bf = (BodyFixedPoint*)theData.tNode;
      tcs = bf->GetBodyFixedCoordinateSystem();

      spObj  = (SpaceObject*) theData.rNode;
      origin = spObj->GetOrigin();

      if (rcs)
         delete rcs;
      rcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
               origin, NULL, NULL, earth, solarSystem);
      if (ocs)
         delete ocs;
      ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS","Topocentric",
               bf, NULL, NULL, bf->GetJ2000Body(), solarSystem);
      
      if (j2k)
         delete j2k;
      j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
               tcs->GetOrigin(), NULL, NULL, earth, solarSystem);
   }
   else if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
   {
#ifdef USE_PRECISION_TIME
      gsPrecEpoch = theData.rPrecTime;                           // made changes by TUAN NGUYEN
#else
      gsEpoch = theData.rTime;
#endif
      BodyFixedPoint *bf = (BodyFixedPoint*)theData.rNode;
      rcs = bf->GetBodyFixedCoordinateSystem();

      spObj  = (SpaceObject*) theData.tNode;
      origin = spObj->GetOrigin();
      
      if (tcs)
         delete tcs;
      tcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
               origin, NULL, NULL, earth, solarSystem);
      
      if (ocs)
         delete ocs;
      ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS","Topocentric",
               bf, NULL, NULL, earth, solarSystem);
      
      if (j2k)
         delete j2k;
      j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
               rcs->GetOrigin(), NULL, NULL, earth, solarSystem);
   }
   else
   {
      origin = ((SpaceObject*)theData.tNode)->GetOrigin();
      if (rcs)
         delete rcs;
      rcs = CoordinateSystem::CreateLocalCoordinateSystem("RCS", "MJ2000Eq",
              origin, NULL, NULL, earth, solarSystem);
      
      if (tcs)
         delete tcs;
      tcs = CoordinateSystem::CreateLocalCoordinateSystem("TCS", "MJ2000Eq",
              origin, NULL, NULL, earth, solarSystem);
      
      if (ocs)
         delete ocs;
      ocs = CoordinateSystem::CreateLocalCoordinateSystem("OCS","MJ2000Eq",
              origin, NULL, NULL, earth, solarSystem);

      if (j2k)
         delete j2k;
      j2k = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
              origin, NULL, NULL, earth, solarSystem);
   }

   // 3. Update all rotation matrixes at gs time (or at transmit time when both nodes are spacecrafts
   std::string updateAll = "All";

#ifdef USE_PRECISION_TIME
   UpdateRotationMatrix(gsPrecEpoch.GetMjd(), updateAll);                  // made changes by TUAN NGUYEN
#else
   UpdateRotationMatrix(gsEpoch, updateAll);
#endif
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Late Binding Initialization complete:\n"
            "   SolarSystem pointer:  %p\n"
            "   Transmitter CS:       %p\n"
            "   Receiver CS:          %p\n"
            "   Observation CS:       %p\n"
            "   J2000 CS:             %p\n",
            solarSystem, tcs, rcs, ocs, j2k);
   #endif

   if (chainForwards)
   {
      if (next)
      {
         next->InitializeSignal(chainForwards);
      }
   }
   else
   {
      if (previous)
      {
         previous->InitializeSignal(chainForwards);
      }
   }

   isInitialized = true;
}


//------------------------------------------------------------------------------
// void CalculateRangeVectorInertial()
//------------------------------------------------------------------------------
/**
 * 1. Computes the range vector in MJ2000 Equatorial coordinates.
 * 2. Update roation matrix at ground station's time. If both nodes are 
 *    spacecrafts, time is at transmit node.
 *
 * The default implementation here performs the instantaneous range vector
 * calculation.  Override this implementation for the light time corrected
 * vector.
 */
//------------------------------------------------------------------------------
void SignalBase::CalculateRangeVectorInertial()
{

   // 1. Update all rotation matrixes at time of ground station. If both tnode and rNode are spacecrafts, update at transmit time
   std::string updateAll = "All";

#ifdef USE_PRECISION_TIME
   GmatTime gsPrecEpoch = theData.tPrecTime;                     // made changes by TUAN NGUYEN
   if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
      gsPrecEpoch = theData.rPrecTime;                           // made changes by TUAN NGUYEN
   UpdateRotationMatrix(gsPrecEpoch.GetMjd(), updateAll);        // made changes by TUAN NGUYEN
#else
   GmatEpoch gsEpoch = theData.tTime;
   if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
      gsEpoch = theData.rTime;
   UpdateRotationMatrix(gsEpoch, updateAll);
#endif

   // 2. Compute range vector in MJ2000 coordinate system 
   SpacePoint *origin1 = tcs->GetOrigin();
   SpacePoint *origin2 = rcs->GetOrigin();
   SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");

#ifdef USE_PRECISION_TIME
   theData.tOStateSSB = origin1->GetMJ2000PrecState(theData.tPrecTime) - ssb->GetMJ2000PrecState(theData.tPrecTime);
   theData.rOStateSSB = origin2->GetMJ2000PrecState(theData.rPrecTime) - ssb->GetMJ2000PrecState(theData.rPrecTime);
   theData.j2kOriginSep = (theData.rOStateSSB.GetR() - theData.tOStateSSB.GetR());
   theData.j2kOriginVel = (theData.rOStateSSB.GetV() - theData.tOStateSSB.GetV());
//   theData.j2kOriginSep        = origin2->GetMJ2000PrecPosition(theData.rPrecTime) -
//                                 origin1->GetMJ2000PrecPosition(theData.tPrecTime);
#else
   theData.tOStateSSB = origin1->GetMJ2000PrecState(theData.tTime) - ssb->GetMJ2000PrecState(theData.tTime);
   theData.rOStateSSB = origin2->GetMJ2000PrecState(theData.rTime) - ssb->GetMJ2000PrecState(theData.rTime);
   theData.j2kOriginSep = (theData.rOStateSSB.GetR() - theData.tOStateSSB.GetR());
   theData.j2kOriginVel = (theData.rOStateSSB.GetV() - theData.tOStateSSB.GetV());
//   theData.j2kOriginSep        = origin2->GetMJ2000Position(theData.rTime) -
//                                 origin1->GetMJ2000Position(theData.tTime);
#endif
   
//   theData.rangeVecInertial = theData.rLoc - theData.j2kOriginSep -
//         theData.tLoc;                                       // GMAT MathSpec Eq. 6.12
   theData.rangeVecInertial = theData.rLoc + theData.j2kOriginSep -
         theData.tLoc;                                       // GMAT MathSpec Eq. 6.12

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Origin Sep: %s\n tLoc: %s\n rLoc: %s\n",
            (theData.j2kOriginSep.ToString(8).c_str()),
            (theData.tLoc.ToString(8).c_str()),
            (theData.rLoc.ToString(8)).c_str());
   #endif
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

   // 1. Update R_Transmitter_j2k, R_receiver_j2k, and R_Obs_j2k matrixes
#ifdef USE_PRECISION_TIME
   UpdateRotationMatrix(theData.tPrecTime.GetMjd(),"j2k_1");         // made changes by TUAN NGUYEN
   UpdateRotationMatrix(theData.rPrecTime.GetMjd(),"j2k_2");         // made changes by TUAN NGUYEN
   if (theData.stationParticipant)                                   // made changes by TUAN NGUYEN
   {                                                                 // made changes by TUAN NGUYEN
      if (theData.tNode->IsOfType(Gmat::GROUND_STATION))             // made changes by TUAN NGUYEN
         UpdateRotationMatrix(theData.tPrecTime.GetMjd(),"o_j2k");   // made changes by TUAN NGUYEN
      else                                                           // made changes by TUAN NGUYEN
         UpdateRotationMatrix(theData.rPrecTime.GetMjd(),"o_j2k");   // made changes by TUAN NGUYEN
   }                                                                 // made changes by TUAN NGUYEN
   else                                                              // made changes by TUAN NGUYEN
      R_Obs_j2k = I33;                                               // made changes by TUAN NGUYEN
#else
   UpdateRotationMatrix(theData.tTime,"j2k_1");                      // made changes by TUAN NGUYEN
   UpdateRotationMatrix(theData.rTime,"j2k_2");                      // made changes by TUAN NGUYEN
#endif

   // 2. Compute the positions of the participants in their own frames
   theData.tLocTcs = theData.tJ2kRotation * theData.tLoc;
   theData.rLocRcs = theData.rJ2kRotation * theData.rLoc;

   // 3. Compute range vector observation
   // @todo: handle this for arbitrary participants; currently rotates if there
   // are any GS's in the participant list
   if (theData.stationParticipant)
      theData.rangeVecObs = R_Obs_j2k * theData.rangeVecInertial;      // R_Obs_j2k matrix was computed in CalculateRangeVectorInertial() function
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
   SpacePoint *origin1 = tcs->GetOrigin();
   SpacePoint *origin2 = rcs->GetOrigin();
   SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");

   // 1. Compute tVel, rVel, j2kOriginVel, rangeRateVecInertial
#ifdef USE_PRECISION_TIME
   // Velocity of origin2 at time rPrecTime w.r.t. origin1 at time tPrecTime
   theData.j2kOriginVel = (origin1->GetMJ2000PrecVelocity(theData.tPrecTime) - ssb->GetMJ2000PrecVelocity(theData.tPrecTime)) -      // velocity of origin1 w.r.t SSB at transmit time tPrecTime in MJ2000Eq coordinate system
                          (origin2->GetMJ2000PrecVelocity(theData.rPrecTime) - ssb->GetMJ2000PrecVelocity(theData.rPrecTime));       // velocity of origin2 w.r.t SSB at receive time rPrecTime in MJ2000Eq coordinate system
//   theData.j2kOriginVel = origin2->GetMJ2000PrecVelocity(theData.rPrecTime) -
//                          origin1->GetMJ2000PrecVelocity(theData.tPrecTime);

   theData.tVel = theData.tNode->GetMJ2000PrecVelocity(theData.tPrecTime);            // made changes by TUAN NGUYEN
   theData.rVel = theData.rNode->GetMJ2000PrecVelocity(theData.rPrecTime);            // made changes by TUAN NGUYEN
#else
   // Velocity of origin2 at time rTime w.r.t. origin1 at time tTime
   theData.j2kOriginVel = (origin1->GetMJ2000Velocity(theData.tTime) - ssb->GetMJ2000Velocity(theData.tTime)) -      // velocity of origin1 w.r.t SSB at transmit time tTime in MJ2000Eq coordinate system
                          (origin2->GetMJ2000Velocity(theData.rTime) - ssb->GetMJ2000Velocity(theData.rTime));       // velocity of origin2 w.r.t SSB at receive time rTime in MJ2000Eq coordinate system   
//   theData.j2kOriginVel = origin2->GetMJ2000Velocity(theData.rTime) -
//                          origin1->GetMJ2000Velocity(theData.tTime);

   theData.tVel = theData.tNode->GetMJ2000Velocity(theData.tTime);
   theData.rVel = theData.rNode->GetMJ2000Velocity(theData.rTime);
#endif
   theData.rangeRateVecInertial = theData.rVel - theData.j2kOriginVel - theData.tVel;

   // 2. Compute the velocities of the participants in their own frames
   CalculateRangeVectorObs();                              // R_Transmitter_j2k, R_receiver_j2k, and R_Obs_j2k matrixes are updated in CalculateRangevectorObs() function
   theData.tVelTcs = theData.tJ2kRotation * theData.tVel;
   theData.rVelRcs = theData.rJ2kRotation * theData.rVel;

   // 3. Compute rangeRateVecObs
   if (theData.stationParticipant)
      theData.rangeRateVecObs = RDot_Obs_j2k*theData.rangeVecInertial + R_Obs_j2k*theData.rangeRateVecInertial;
   else
      theData.rangeRateVecObs = theData.rangeRateVecInertial;

}


//------------------------------------------------------------------------------
// void GetRangeDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
//       Rvector &deriv)
//------------------------------------------------------------------------------
/**
 * Calculates the range derivative of a signal in a measurement
 *
 * @param forObj Pointer for the participants that hold the w.r.t field
 * @param wrtR Flag indicating if derivative with respect to position is desired
 * @param wrtV Flag indicating if derivative with respect to velocity is desired
 * @param deriv The structure that gets filled with derivative data
 */
//------------------------------------------------------------------------------
void SignalBase::GetRangeDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
      Rvector &deriv)
{
   Rmatrix derivMatrix;
   if (wrtR && wrtV)
      derivMatrix.SetSize(6,6);
   else
      derivMatrix.SetSize(3,3);

   GetRangeVectorDerivative(forObj, wrtR, wrtV, derivMatrix);

   Rvector3 rangeVec = theData.rangeVecInertial;
   Rvector3 temp;
   Rmatrix33 mPart;
   Rvector3 unitRange = rangeVec / rangeVec.GetMagnitude();

   if (wrtR)
   {
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            mPart(i,j) = derivMatrix(i,j);

      temp = unitRange * mPart;
      for (Integer i = 0; i < 3; ++i)
         deriv[i] = temp(i);
   }
   if (wrtV)
   {
      Integer offset = (wrtR ? 3 : 0);
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            mPart(i,j) = derivMatrix(i+offset, j+offset);

      temp = unitRange * mPart;
      for (Integer i = 0; i < 3; ++i)
         deriv[i+offset] = temp(i);
   }
}


//------------------------------------------------------------------------------
// void GetRangeVectorDerivative(GmatBase *forObj, bool wrtR, bool wrtV,
//       Rmatrix& derivMatrix)
//------------------------------------------------------------------------------
/**
 * Calculates the range vector derivative of a signal in a measurement
 *
 * @param forObj Pointer for the participants that hold the w.r.t field
 * @param usingData SignalData for the signal being evaluated
 * @param wrtR Flag indicating if derivative with respect to position is desired
 * @param wrtV Flag indicating if derivative with respect to velocity is desired
 * @param derivMatrix The structure that gets filled with derivative data
 */
//------------------------------------------------------------------------------
void SignalBase::GetRangeVectorDerivative(GmatBase *forObj, bool wrtR,
      bool wrtV, Rmatrix& derivMatrix)
{
   bool forTransmitter = true;
   if (theData.rNode == forObj)
      forTransmitter = false;
   else
   {
      if (theData.tNode != forObj)
         throw MeasurementException("Range vector derivative requested, but "
               "neither participant is the \"for\" object");
   }

   // For now ignoring the barycenter piece -- we'll add it once core code works
   Rvector3 rangeVec = theData.rangeVecInertial;      // Check sign?

   // phi(t1,tm) = phi(t1, t0)* Inv(phi(tm, t0))   where: t0 is initial epoch; tm is measurement time; t1 is either transmit time or receive time
   // Rmatrix phi = (forTransmitter ? theData.tSTM : theData.rSTM);
   Rmatrix phi = (forTransmitter ? (theData.tSTM*theData.tSTMtm.Inverse()) : (theData.rSTM*theData.rSTMtm.Inverse()));

   // Check: Not using old STM inverse approach -- STMs computed from initial
   // (measurement epoch based) state, so STM inverse is identity
   Rmatrix33 A, B;
   for (Integer i = 0; i < 3; ++i)
   {
      for (Integer j = 0; j < 3; ++j)
      {
         if (wrtR)
            A(i,j) = phi(i,j);
         if(wrtV)
            B(i,j) = phi(i,j+3);
      }
   }
   Real sign = (forTransmitter ? -1.0 : 1.0);

   Rmatrix33 temp;
   Rmatrix33 body2FK5_matrix = (forTransmitter ? theData.tJ2kRotation :
         theData.rJ2kRotation);

   if (wrtR)
   {
      // temp = dataToUse.rInertial2obj * A;
      temp = body2FK5_matrix * A;    // made change by TUAN NGUYEN

      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            derivMatrix(i,j) = sign * temp(i,j);
   }
   if (wrtV)
   {
      // temp = dataToUse.rInertial2obj * B;
     temp = body2FK5_matrix * B;    // made change by TUAN NGUYEN

      Integer offset = (wrtR ? 3 : 0);
      for (Integer i = 0; i < 3; ++i)
         for (Integer j = 0; j < 3; ++j)
            derivMatrix(i+offset,j+offset) = sign * temp(i,j);
   }
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
      bool updated = false;

      if ((whichOne == "All") || (whichOne == "j2k_2"))
      {
         converter.Convert(itsEpoch,dummyIn,rcs,dummyOut,j2k);
         R_j2k_Receiver = converter.GetLastRotationMatrix();
         //               RDot_j2k_2 = converter.GetLastRotationDotMatrix();
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,rcs);
         theData.rJ2kRotation    = converter.GetLastRotationMatrix();
         updated = true;
      }
      if ((whichOne == "All") || (whichOne == "j2k_1"))
      {
         converter.Convert(itsEpoch,dummyIn,tcs,dummyOut,j2k);
         R_j2k_Transmitter    = converter.GetLastRotationMatrix();
         //               RDot_j2k_1 = converter.GetLastRotationDotMatrix();
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,tcs);
         theData.tJ2kRotation    = converter.GetLastRotationMatrix();
         updated = true;
      }
      if ((whichOne == "All") || (whichOne == "o_2"))
      {
         converter.Convert(itsEpoch,dummyIn,rcs,dummyOut,ocs);
         R_Obs_Receiver      = converter.GetLastRotationMatrix();
         RDot_Obs_Receiver   = converter.GetLastRotationDotMatrix();
         updated = true;
      }
      if ((whichOne == "All") || (whichOne == "o_1"))
      {
         converter.Convert(itsEpoch,dummyIn,tcs,dummyOut,ocs);
         R_Obs_Transmitter      = converter.GetLastRotationMatrix();
         RDot_Obs_Transmitter   = converter.GetLastRotationDotMatrix();
         updated = true;
      }
      if ((whichOne == "All") || (whichOne == "o_j2k"))
      {
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,ocs);
         R_Obs_j2k    = converter.GetLastRotationMatrix();
         RDot_Obs_j2k = converter.GetLastRotationDotMatrix();
         updated = true;
      }

     if (updated == false)
     {
        std::stringstream ss;
        ss << "Error: UpdateRotationMatrix() input parameter whichOne (" << whichOne << ") has an invalid value\n";
        throw MeasurementException(ss.str());
      }
   }
   else
   {
      // When both participants are spacecraft, all frames are inertial, so all
      // rotation matrices are I33 (which is the default on creation) and all
      // Rotation Dot matrices are zero33
      R_j2k_Receiver       = I33;                        // made changes by TUAN NGUYEN
      theData.rJ2kRotation = I33;                        // made changes by TUAN NGUYEN
      R_j2k_Transmitter    = I33;                        // made changes by TUAN NGUYEN
      theData.tJ2kRotation = I33;                        // made changes by TUAN NGUYEN
      R_Obs_Receiver       = I33;                        // made changes by TUAN NGUYEN
      R_Obs_Transmitter    = I33;                        // made changes by TUAN NGUYEN
      R_Obs_j2k            = I33;                        // made changes by TUAN NGUYEN

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

#ifndef USE_PRECISION_TIME 
//------------------------------------------------------------------------------
// void MoveToEpoch(const GmatEpoch theEpoch, bool epochAtReceive, bool moveAll)
//------------------------------------------------------------------------------
/**
 * Moves participants to a specified epoch
 *
 * @param theEpoch The desired epoch
 * @param epochAtReceive Flag indicating that desired epoch is for receiver
 * @param moveAll true to move both participants
 */
//------------------------------------------------------------------------------
void SignalBase::MoveToEpoch(const GmatEpoch theEpoch, bool epochAtReceive,
      bool moveAll)
{
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Called MoveToEpoch(%.12lf, %s%s)\n",
            theEpoch,
            (epochAtReceive ? "Epoch at Receive" : "Epoch at Transmit"),
            (moveAll ? ", Moving all participants" : ""));
   #endif

   // 1. Propagate receive node at time theEpoch and update it's SignalData
   if (epochAtReceive || moveAll)
   {
      Real dt = (theEpoch - theData.rTime) * GmatTimeConstants::SECS_PER_DAY;

      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   dt = (%.12lf - %.12lf)*86400 => %.12le\n",
               theEpoch, theData.rTime, dt);
      #endif

      if (dt != 0.0)
         StepParticipant(dt, false);
      else
      {
         Rvector6 state;
         if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
            state = theData.rNode->GetMJ2000State(theEpoch);
         else // Retrieve spacecraft data from the propagator
         {
            const Real *pstate =
                  theData.rPropagator->GetPropagator()->AccessOutState();
            state.Set(pstate);
         }

         theData.rLoc = state.GetR();
         theData.rVel = state.GetV();

         SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");   // made changes by TUAN NGUYEN
         theData.rOStateSSB = rcs->GetOrigin()->GetMJ2000State(theData.rTime) -                // made changes by TUAN NGUYEN
                            ssb->GetMJ2000State(theData.rTime);                                // made changes by TUAN NGUYEN
      }
   }

   // 2. Propagate transmit node at time theEpoch and update it's SignalData
   if (!epochAtReceive || moveAll)
   {
      Real dt = (theEpoch - theData.tTime) * GmatTimeConstants::SECS_PER_DAY;

      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   dt = (%.12lf - %.12lf)*86400 => %.12le\n",
               theEpoch, theData.rTime, dt);
      #endif

      if (dt != 0.0)
         StepParticipant(dt, true);
      else
      {
         Rvector6 state;
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
            state = theData.tNode->GetMJ2000State(theEpoch);
         else // Retrieve spacecraft data from the propagator
         {
            const Real *pstate =
                  theData.tPropagator->GetPropagator()->AccessOutState();
            state.Set(pstate);
         }
         theData.tLoc = state.GetR();
         theData.tVel = state.GetV();

         SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");   // made changes by TUAN NGUYEN
         theData.tOStateSSB = tcs->GetOrigin()->GetMJ2000State(theData.tTime) -                // made changes by TUAN NGUYEN
                            ssb->GetMJ2000State(theData.tTime);                                // made changes by TUAN NGUYEN
      }
   }

   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("MoveToEpoch complete\n");
   #endif
}

#else
// made changes by TUAN NGUYEN
void SignalBase::MoveToEpoch(const GmatTime theEpoch, bool epochAtReceive,
      bool moveAll)
{
   GmatTime t = theEpoch;
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Called MoveToEpoch(%.12lf, %s%s)\n",
            t.GetMjd(),
            (epochAtReceive ? "Epoch at Receive" :  "Epoch at Transmit"),
            (moveAll ? ", Moving all participants" : ""));
   #endif

   // 1. Propagate receive node at time theEpoch and update it's SignalData
   if (epochAtReceive || moveAll)
   {
      Real dt = (theEpoch - theData.rPrecTime).GetTimeInSec();
      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   dt = (%.12lf - %.12lf)*86400 => %.12le\n",
               t.GetMjd(), theData.rPrecTime.GetMjd(), dt);
      #endif
      if (dt != 0.0)
         StepParticipant(dt, false);
      else
      {
         Rvector6 state;
         Rmatrix66 stm;         // state transition matrix. Its default value is I matrix      // made changes by TUAN NGUYEN
         if (theData.rNode->IsOfType(Gmat::GROUND_STATION))
         {
            state = theData.rNode->GetMJ2000PrecState(theEpoch);
            // For a ground station, its STM is I matrix
         }
         else
         {
            // Retrieve spacecraft data from the propagator
            const Real *pstate =
                  theData.rPropagator->GetPropagator()->AccessOutState();

            // set value for state and STM
            state.Set(pstate);
            for (UnsignedInt i = 0; i < 6; ++i)                                                 // made changes by TUAN NGUYEN
               for (UnsignedInt j = 0; j < 6; ++j)                                              // made changes by TUAN NGUYEN
                  stm(i,j) = pstate[6 + i*6 + j];                                               // made changes by TUAN NGUYEN
         }

         theData.rLoc = state.GetR();
         theData.rVel = state.GetV();
         // Whenever updating participant's state in signal data, it needs to update participant's STM
         theData.rSTM = stm;                                                                    // made changes by TUAN NGUYEN

         SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");    // made changes by TUAN NGUYEN
         theData.rOStateSSB = rcs->GetOrigin()->GetMJ2000PrecState(theData.rPrecTime) -         // made changes by TUAN NGUYEN
                            ssb->GetMJ2000PrecState(theData.rPrecTime);                         // made changes by TUAN NGUYEN
      }
      #ifdef DEBUG_MOVETOEPOCH
      MessageInterface::ShowMessage("Move to epoch: %.12lf  participant %s  position in EarthMJ2000Eq [%.12lf   %.12lf   %.12lf]\n", t.GetMjd(), theData.rNode->GetName().c_str(), theData.rLoc[0], theData.rLoc[1], theData.rLoc[2]);
      #endif
   }

   // 2. Propagate transmit node at time theEpoch and update it's SignalData
   if (!epochAtReceive || moveAll)
   {
     Real dt = (theEpoch - theData.tPrecTime).GetTimeInSec();
     #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   dt = (%.12lf - %.12lf)*86400 => %.12le\n",
               t.GetMjd(), theData.tPrecTime.GetMjd(), dt);
      #endif
      if (dt != 0.0)
         StepParticipant(dt, true);
      else
      {
         Rvector6 state;
         Rmatrix66 stm;         // state transition matrix. Its default value is I matrix      // made changes by TUAN NGUYEN
         if (theData.tNode->IsOfType(Gmat::GROUND_STATION))
         {
            state = theData.tNode->GetMJ2000PrecState(theEpoch);
            // For a ground station, its STM is I matrix
         }
         else
         {
            // Retrieve spacecraft data from the propagator
            const Real *pstate =
                  theData.tPropagator->GetPropagator()->AccessOutState();

            // set value for state and STM
            state.Set(pstate);
            for (UnsignedInt i = 0; i < 6; ++i)                                                 // made changes by TUAN NGUYEN
               for (UnsignedInt j = 0; j < 6; ++j)                                              // made changes by TUAN NGUYEN
                  stm(i,j) = pstate[6 + i*6 + j];                                               // made changes by TUAN NGUYEN
         }

         theData.tLoc = state.GetR();
         theData.tVel = state.GetV();
         // Whenever updating participant's state in signal data, it needs to update participant's STM
         theData.tSTM = stm;                                                                    // made changes by TUAN NGUYEN

         SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");    // made changes by TUAN NGUYEN
         theData.tOStateSSB = tcs->GetOrigin()->GetMJ2000PrecState(theData.tPrecTime) -         // made changes by TUAN NGUYEN
                            ssb->GetMJ2000PrecState(theData.tPrecTime);                         // made changes by TUAN NGUYEN
      }
      #ifdef DEBUG_MOVETOEPOCH
      MessageInterface::ShowMessage("Move to epoch: %.12lf  participant %s  position in EarthMJ2000Eq [%.12lf   %.12lf   %.12lf]\n", t.GetMjd(), theData.tNode->GetName().c_str(), theData.tLoc[0], theData.tLoc[1], theData.tLoc[2]);
      #endif
   }
}
#endif

//------------------------------------------------------------------------------
// void SetPrevious(SignalBase* prev)
//------------------------------------------------------------------------------
/**
 * Build the backwards link for the doubly linked list
 *
 * @param prev Link to the owner of this signal in the list
 */
//------------------------------------------------------------------------------
void SignalBase::SetPrevious(SignalBase* prev)
{
   previous = prev;
}


//------------------------------------------------------------------------------
// bool StepParticipant(Real stepToTake, bool forTransmitter)
//------------------------------------------------------------------------------
/**
 * Steps a participant in time, and updates the J2k state buffer accordingly
 *
 * @param stepToTake The amount of time needed for the propagation
 * @param forTransmitter True to step the transmitter participant, false to
 *                       step the receiver
 *
 * @return true is a step succeeded, false if it failed
 */
//------------------------------------------------------------------------------
bool SignalBase::StepParticipant(Real stepToTake, bool forTransmitter)
{
   #ifdef DEBUG_LIGHTTIME
      MessageInterface::ShowMessage("Called StepParticipant(%.12le, %s)\n",
            stepToTake, (forTransmitter ? "stepping transmitter" :
            "stepping receiver"));
   #endif
   bool retval = false;
   
   // 1. Get associated propagator for transmiter node (or receiver node) 
   Propagator *prop = NULL;

   if (forTransmitter)
   {
      if (theData.tNode->IsOfType(Gmat::SPACEOBJECT))
      {
         if (theData.tPropagator != NULL)
            prop = theData.tPropagator->GetPropagator();
         else
            throw MeasurementException("The propagator for the transmitting "
                  "participant \"" + theData.transmitParticipant +
                  "\" has not been set in the signal that needs it");
      }
   }
   else
   {
      if (theData.rNode->IsOfType(Gmat::SPACEOBJECT))
      {
         if (theData.rPropagator != NULL)
            prop = theData.rPropagator->GetPropagator();
         else
            throw MeasurementException("The propagator for the receiving "
                  "participant \"" + theData.transmitParticipant +
                  "\" has not been set in the signal that needs it");
      }
   }

   
   // 2. Propagate transmiter node (or receiver node) for stepToTake and specify it's state
   Rvector6 state;
   Rmatrix66 stm;
   if (prop)      // Handle spacecraft
   {
      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("Propagating numerically\n");
      #endif

      const Real *outState = prop->AccessOutState();

      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   ---> Before: %.12lf\n", state[0]);
      #endif

      retval = prop->Step(stepToTake);
      if (retval == false)
      {
         MessageInterface::ShowMessage("Failed to step %s by %le secs\n",
               (forTransmitter ? theData.transmitParticipant.c_str() :
                theData.receiveParticipant.c_str()), stepToTake);
      }

      state.Set(outState);
      for (UnsignedInt i = 0; i < 6; ++i)
         for (UnsignedInt j = 0; j < 6; ++j)
            stm(i,j) = outState[6 + i*6 + j];

      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("   ---> After %.12lf : %.12lf\n",
               prop->GetStepTaken(), state(0));
      #endif

      #ifdef DEBUG_DERIVATIVES
         MessageInterface::ShowMessage("   STM = %s\n", stm.ToString().c_str());
      #endif
   }
   else           // Handle ground stations and other analytic props
   {
      #ifdef DEBUG_LIGHTTIME
         MessageInterface::ShowMessage("Propagating analytically\n");
      #endif
#ifdef USE_PRECISION_TIME
      if (forTransmitter)
      {
         state = theData.tNode->GetMJ2000PrecState(theData.tPrecTime +
               stepToTake / GmatTimeConstants::SECS_PER_DAY);
      }
      else
      {
         state = theData.rNode->GetMJ2000PrecState(theData.rPrecTime +
               stepToTake / GmatTimeConstants::SECS_PER_DAY);
      }
#else
      if (forTransmitter)
      {
         state = theData.tNode->GetMJ2000State(theData.tTime +
               stepToTake / GmatTimeConstants::SECS_PER_DAY);
      }
      else
      {
         state = theData.rNode->GetMJ2000State(theData.rTime +
               stepToTake / GmatTimeConstants::SECS_PER_DAY);
      }
#endif
      // For ground station, its STM is I matrix
   }

   
   // 3. Set value for SignalData object associated to transmiter node (or receiver node)
   SpecialCelestialPoint* ssb = solarSystem->GetSpecialPoint("SolarSystemBarycenter");
   if (forTransmitter)
   {
      theData.tLoc = state.GetR();
      theData.tVel = state.GetV();
      // whenever updating participant's state, it needs to update its STM             // made changes by TUAN NGUYEN
      theData.tSTM = stm;                                                              // made changes by TUAN NGUYEN

#ifdef USE_PRECISION_TIME
      theData.tPrecTime += stepToTake / GmatTimeConstants::SECS_PER_DAY;               // made changes by TUAN NGUYEN
      theData.tOStateSSB = tcs->GetOrigin()->GetMJ2000PrecState(theData.tPrecTime) -   // made changes by TUAN NGUYEN
                         ssb->GetMJ2000PrecState(theData.tPrecTime);                   // made changes by TUAN NGUYEN
#else
      theData.tTime += stepToTake / GmatTimeConstants::SECS_PER_DAY;
      theData.tOStateSSB = tcs->GetOrigin()->GetMJ2000State(theData.tTime) -           // made changes by TUAN NGUYEN
                         ssb->GetMJ2000State(theData.tTime);                           // made changes by TUAN NGUYEN
#endif
   }
   else
   {
      theData.rLoc = state.GetR();
      theData.rVel = state.GetV();
      // whenever updating participant's state, it needs to update its STM             // made changes by TUAN NGUYEN
      theData.rSTM = stm;                                                              // made changes by TUAN NGUYEN

#ifdef USE_PRECISION_TIME
      theData.rPrecTime += stepToTake / GmatTimeConstants::SECS_PER_DAY;               // made changes by TUAN NGUYEN
      theData.rOStateSSB = rcs->GetOrigin()->GetMJ2000PrecState(theData.rPrecTime) -   // made changes by TUAN NGUYEN
                         ssb->GetMJ2000PrecState(theData.rPrecTime);                   // made changes by TUAN NGUYEN
#else
      theData.rTime += stepToTake / GmatTimeConstants::SECS_PER_DAY;
      theData.rOStateSSB = rcs->GetOrigin()->GetMJ2000State(theData.rTime) -           // made changes by TUAN NGUYEN
                         ssb->GetMJ2000PrecState(theData.rTime);                       // made changes by TUAN NGUYEN
#endif
   }
   
   retval = true;
   return retval;
}
