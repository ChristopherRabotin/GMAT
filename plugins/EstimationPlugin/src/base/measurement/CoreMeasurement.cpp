//$Id: CoreMeasurement.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         CoreMeasurement
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/01
//
/**
 * The base class for the measurement fundamentals in GMAT.
 */
//------------------------------------------------------------------------------


#include "CoreMeasurement.hpp"
#include "MessageInterface.hpp"
#include "SpaceObject.hpp"
#include "BodyFixedPoint.hpp"
#include "MeasurementException.hpp"
#include <sstream>                  // for stringstream

#include "GroundstationInterface.hpp"				// made changes by TUAN NGUYEN
#include "Spacecraft.hpp"					// made changes by TUAN NGUYEN


//#define DEBUG_MEASUREMENT_INITIALIZATION
//#define DEBUG_CALC_RANGE
//#define DEBUG_HARDWARE
//#define DEBUG_INIT
//#define DEBUG_DELAY

//---------------------------------
// static data
//---------------------------------

const Rmatrix33  CoreMeasurement::I33        = Rmatrix33(true);
const Rmatrix33  CoreMeasurement::zero33     = Rmatrix33(false);

const Rvector3   CoreMeasurement::zeroVector = Rvector3(0.0,0.0,0.0);
const Rvector3   CoreMeasurement::xUnit      = Rvector3(1.0,0.0,0.0);
const Rvector3   CoreMeasurement::yUnit      = Rvector3(0.0,1.0,0.0);
const Rvector3   CoreMeasurement::zUnit      = Rvector3(0.0,0.0,1.0);


//-----------------------------------------------------------------------------
// CoreMeasurement(const std::string &type, const std::string &nomme)
//-----------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param type The scripted type of the object
 * @param nomme The object's name
 */
//-----------------------------------------------------------------------------
CoreMeasurement::CoreMeasurement(const std::string &type,
	  const std::string &nomme) :
   GmatBase           (Gmat::CORE_MEASUREMENT, type, nomme),
   uniqueId           (-1),
   measurementSize    (1),
   eventCount         (0),
   anchorPoint        (NULL),
   initialized        (false),
   stationParticipant (false),
   F1                 (NULL),
   F2                 (NULL),
   Fo                 (NULL),
   j2k                (NULL),
   solarSystem        (NULL),
   satEpochID         (-1)
{
   objectTypes.push_back(Gmat::CORE_MEASUREMENT);
   objectTypeNames.push_back("CoreMeasurement");
}


//-----------------------------------------------------------------------------
// ~CoreMeasurement()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
CoreMeasurement::~CoreMeasurement()
{
   // delete the local coordinate systems, if necessary
   if (F1)
      delete F1;
   if (F2)
      delete F2;
   if (Fo)
      delete Fo;
   if (j2k)
      delete j2k;
}


//-----------------------------------------------------------------------------
// CoreMeasurement(const CoreMeasurement& core)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param core The CoreMeasurement that is copied to the new one
 */
//-----------------------------------------------------------------------------
CoreMeasurement::CoreMeasurement(const CoreMeasurement& core) :
   GmatBase           (core),
   uniqueId           (core.uniqueId),
   measurementSize    (core.measurementSize),
   eventCount         (core.eventCount),
   participantNames   (core.participantNames),
   anchorPoint        (NULL),
   participants       (core.participants),
   initialized        (false),
   stationParticipant (core.stationParticipant),
   F1                 (NULL),
   F2                 (NULL),
   Fo                 (NULL),
   j2k                (NULL),
   solarSystem        (core.solarSystem),
   satEpochID         (core.satEpochID)
{
   std::vector<Hardware*> hv;
   participantHardware.assign(core.participantHardware.size(), hv);

   for (UnsignedInt i = 0; i < core.participantHardware.size(); ++i)
   {
      participantHardware[i].assign(core.participantHardware[i].size(), NULL);
      for (UnsignedInt j = 0; j < core.participantHardware[i].size(); ++j)
         if (core.participantHardware[i][j] != NULL)
         {
            participantHardware[i][j] =
                  (Hardware*)(core.participantHardware[i][j]->Clone());
         }
   }
}


//-----------------------------------------------------------------------------
// CoreMeasurement& operator=(const CoreMeasurement& core)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param core The CoreMeasurement that is provides parameters for this one
 *
 * @return This CoreMeasurement, configured to match core
 */
//-----------------------------------------------------------------------------
CoreMeasurement& CoreMeasurement::operator=(const CoreMeasurement& core)
{
   if (this != &core)
   {
      GmatBase::operator=(core);

      uniqueId           = core.uniqueId;
      measurementSize    = core.measurementSize;
      eventCount         = core.eventCount;
      participantNames   = core.participantNames;
      anchorPoint        = NULL;
      participants       = core.participants;
      initialized        = false;
      stationParticipant = core.stationParticipant;
      F1                 = NULL;
      F2                 = NULL;
      Fo                 = NULL;
      j2k                = NULL;
      solarSystem        = core.solarSystem;
      satEpochID         = core.satEpochID;

      // Clear the old hardware out
      for (UnsignedInt i = 0; i < participantHardware.size(); ++i)
         for (UnsignedInt j = 0; j < participantHardware[i].size(); ++j)
            if (participantHardware[i][j] != NULL)
               delete participantHardware[i][j];
      participantHardware.clear();
      // And clone in the new
      std::vector<Hardware*> hv;
      participantHardware.assign(core.participantHardware.size(), hv);

      for (UnsignedInt i = 0; i < core.participantHardware.size(); ++i)
      {
         participantHardware[i].assign(core.participantHardware[i].size(), NULL);
         for (UnsignedInt j = 0; j < core.participantHardware[i].size(); ++j)
            if (core.participantHardware[i][j] != NULL)
            {
               participantHardware[i][j] =
                     (Hardware*)(core.participantHardware[i][j]->Clone());
            }
      }
   }

   return *this;
}


//-----------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//            const std::string &name)
//-----------------------------------------------------------------------------
/**
 * Passes a reference object to the CoreMeasurement
 *
 * @param obj The reference object
 * @param type The type of the reference object
 * @param name The name of the reference object
 *
 * @return true if the object is set, false if not
 */
//-----------------------------------------------------------------------------
bool CoreMeasurement::SetRefObject(GmatBase *obj,
	  const Gmat::ObjectType type, const std::string &name)
{
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage(
            "CoreMeasurement::SetRefObject(%p, %d, %s == %s)\n", obj, type,
            obj->GetName().c_str(), name.c_str());
   #endif

   if (obj->IsOfType(Gmat::SPACE_POINT))
      if (find(participants.begin(), participants.end(), obj) ==
            participants.end())
      {
         // Cheating here for the moment to be sure GroundStation is 1st object
         std::vector<Hardware*> hv;
         if (obj->IsOfType(Gmat::GROUND_STATION))
         {
            participants.insert(participants.begin(), (SpacePoint*)obj);
            participantHardware.insert(participantHardware.begin(), hv);
            stationParticipant = true;
		    
			// add hardware to participantHardware list:				// made changes by TUAN NGUYEN
			ObjectArray objList = obj->GetRefObjectArray(Gmat::HARDWARE);
			int index = 0;
			for (ObjectArray::iterator i = objList.begin(); i != objList.end(); ++i)
			{
				participantHardware[0].push_back((Hardware*)(*i));
//				MessageInterface::ShowMessage("CoreMeasurement::SetRefObject():  set hardware to participantHardware[0][%d] = '%s'\n", index, participantHardware[0][index]->GetName().c_str());
				++index;
			}

         }
         else
         {
            participants.push_back((SpacePoint*)obj);
            participantHardware.push_back(hv);
			
			// add hardware to participantHardware list:					// made changes by TUAN NGUYEN
			Spacecraft* sc = (Spacecraft*)obj;
			ObjectArray objList = sc->GetRefObjectArray(Gmat::HARDWARE);
			int index1 = participantHardware.size() - 1;
			int index2 = 0;
			for (ObjectArray::iterator i = objList.begin(); i != objList.end(); ++i)
			{
				participantHardware[index1].push_back((Hardware*)(*i));
//				MessageInterface::ShowMessage("CoreMeasurement::SetRefObject():  set hardware to participantHardware[%d][%d] = '%s'\n", index1, index2, participantHardware[index1][index2]->GetName().c_str());
				++index2;
			}

		 }

         // Set IDs
         currentMeasurement.participantIDs.clear();
         for (std::vector<SpacePoint*>::iterator i = participants.begin();
               i != participants.end(); ++i)
         {
            currentMeasurement.participantIDs.push_back((*i)->
                  GetStringParameter("Id"));
         }

         #ifdef DEBUG_MEASUREMENT_INITIALIZATION
            MessageInterface::ShowMessage(
                  "Added %s named %s to a %s CoreMeasurement\n",
                  obj->GetTypeName().c_str(), obj->GetName().c_str(),
                  typeName.c_str());
         #endif
      }

   return true;
}


//-----------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//            const std::string &name, const Integer index)
//-----------------------------------------------------------------------------
/**
 * Passes a reference object that goes in an ObjectArray to the CoreMeasurement
 *
 * @param obj The reference object
 * @param type The type of the reference object
 * @param name The name of the reference object
 * @param index The index for the reference object
 *
 * @return true if the object is set, false if not
 */
//-----------------------------------------------------------------------------
bool CoreMeasurement::SetRefObject(GmatBase *obj,
	  const Gmat::ObjectType type, const std::string &name, const Integer index)
{
   // todo: Manage anchor participant
   return true;
}


//-----------------------------------------------------------------------------
// void SetUniqueId(Integer id)
//-----------------------------------------------------------------------------
/**
 * Sets the unique ID number for the CoreMeasurement
 *
 * @param id The unique ID number
 */
//-----------------------------------------------------------------------------
void CoreMeasurement::SetUniqueId(Integer id)
{
   currentMeasurement.uniqueID = id;
}


//------------------------------------------------------------------------------
// Integer GetMeasurementTypeID()
//------------------------------------------------------------------------------
/**
 * Returns the enumerated type for this measurement
 *
 * @return The type
 */
//------------------------------------------------------------------------------
Integer CoreMeasurement::GetMeasurementTypeID()
{
   return currentMeasurement.type;
}


//-----------------------------------------------------------------------------
// Integer GetMeasurementSize()
//-----------------------------------------------------------------------------
/**
 * retrieved the size of the measurement vector
 *
 * @return The measurement vector's length
 */
//-----------------------------------------------------------------------------
Integer CoreMeasurement::GetMeasurementSize()
{
   return measurementSize;
}


//------------------------------------------------------------------------------
// void CoreMeasurement::SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system used in the coordinate systems
 *
 * @param ss The solar system that gets used
 */
//------------------------------------------------------------------------------
void CoreMeasurement::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}


//-----------------------------------------------------------------------------
// bool Initialize()
//-----------------------------------------------------------------------------
/**
 * Prepares the CoreMeasurement for use in estimation or simulation
 *
 * @return true if initialization succeeded, false if it failed
 */
//-----------------------------------------------------------------------------
bool CoreMeasurement::Initialize()
{
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("CoreMeasurement::Initializing %s\n",
            instanceName.c_str());
   #endif

   bool retval = false;

   if (GmatBase::Initialize())
   {
      retval = true;
   }

   return retval;
}


//-----------------------------------------------------------------------------
// Integer GetEventCount()
//-----------------------------------------------------------------------------
/**
 * Retrieves that number of events that need to be processed to obtain the core
 * measurement
 *
 * Derived classes that use events override this method to report the number of
 * Events that are available for processing.
 *
 * @return The number of events
 */
//-----------------------------------------------------------------------------
Integer CoreMeasurement::GetEventCount()
{
   return eventCount;
}


//-----------------------------------------------------------------------------
// Event* GetEvent(UnsignedInt whichOne)
//-----------------------------------------------------------------------------
/**
 * Retrieves an event object
 *
 * Derived classes that use events override this method to provide access to
 * those events.
 *
 * @param whichOne The index of the event object
 *
 * @return The Event
 */
//-----------------------------------------------------------------------------
Event* CoreMeasurement::GetEvent(UnsignedInt whichOne)
{
   return NULL;
}


//-----------------------------------------------------------------------------
// bool SetEventData(Event *locatedEvent)
//-----------------------------------------------------------------------------
/**
 * Provides a mechanism for the CoreMeasurement to process an event after it has
 * been located.
 *
 * Some Events require outside processes in order to be evaluated -- for
 * example, the LightTimeCorrection Event needs to iterate on propagated states
 * in order to find the correction.  That iteration is performed by the
 * RootFinder object.  After the Event has been located, this method is called
 * so that the CoreMeasurement can retrieve the event value for use in
 * evaluating the measurement.
 *
 * @param locatedEvent The event that was located
 *
 * @return true ic the event was processed, false if not
 */
//-----------------------------------------------------------------------------
bool CoreMeasurement::SetEventData(Event *locatedEvent)
{
   return true;
}


//-----------------------------------------------------------------------------
// MeasurementData* GetMeasurementDataPointer()
//-----------------------------------------------------------------------------
/**
 * Retrieves a pointer to the current measurement
 *
 * @return The measurement pointer
 */
//-----------------------------------------------------------------------------
MeasurementData* CoreMeasurement::GetMeasurementDataPointer()
{
   return &currentMeasurement;
}


//-----------------------------------------------------------------------------
// std::vector<RealArray>* GetDerivativePointer()
//-----------------------------------------------------------------------------
/**
 * Retrieves the derivatives of the measurement
 *
 * @return The derivative matrix
 */
//-----------------------------------------------------------------------------
std::vector<RealArray>* CoreMeasurement::GetDerivativePointer()
{
   return &currentDerivatives;
}


//-----------------------------------------------------------------------------
// const MeasurementData& CalculateMeasurement(bool withEvents)
//-----------------------------------------------------------------------------
/**
 * Makes the CoreMeasurement calculate the measurement data
 *
 * @param withEvents true if Event data is included, false if it is omitted
 *
 * @return The calculated measurement
 */
//-----------------------------------------------------------------------------
const MeasurementData& CoreMeasurement::CalculateMeasurement(bool withEvents)
{
   if (Evaluate(withEvents) == false)
      // throw here
      ;

   return currentMeasurement;
}


//-----------------------------------------------------------------------------
// Integer CoreMeasurement::GetParmIdFromEstID(Integer id, GmatBase *obj)
//-----------------------------------------------------------------------------
/**
 * Uses a measurement ID and object to convert that id into a parameter ID
 *
 * @param id The measurement ID
 * @param obj The object that owns the id
 *
 * @return The parameter ID on the object
 */
//-----------------------------------------------------------------------------
Integer CoreMeasurement::GetParmIdFromEstID(Integer id, GmatBase *obj)
{
   return id - obj->GetType() * 250;
}


/// @todo Wendy needs to comment the code she added to core measurement.
void CoreMeasurement::CalculateRangeVectorInertial(Integer p1index, Integer p2index)
{
   Real satEpoch;
   if (participants[p1index]->IsOfType(Gmat::SPACECRAFT))
      satEpoch = participants[p1index]->GetRealParameter(satEpochID);
   else if (participants[p2index]->IsOfType(Gmat::SPACECRAFT))
      satEpoch = participants[p2index]->GetRealParameter(satEpochID);
   else
      throw MeasurementException("Error in CoreMeasurement::"
            "CalculateRangeVectorInertial; neither participant is a "
            "spacecraft.");

   currentMeasurement.epoch = satEpoch;
   #ifdef DEBUG_CALC_RANGE
      MessageInterface::ShowMessage("In CalcRVInertial, epoch = %12.10f\n",
            currentMeasurement.epoch);
   #endif
   std::string updateAll = "All";
   UpdateRotationMatrix(satEpoch, updateAll); // <<<<<<<<<< just do the ones we need?
   SpacePoint *origin1 = F1->GetOrigin();
   SpacePoint *origin2 = F2->GetOrigin();
   r12_j2k             = origin2->GetMJ2000Position(currentMeasurement.epoch) -
                         origin1->GetMJ2000Position(currentMeasurement.epoch);
   p1Loc = participants[p1index]->GetMJ2000Position(currentMeasurement.epoch);
   p2Loc = participants[p2index]->GetMJ2000Position(currentMeasurement.epoch);

   rangeVecInertial = p2Loc - r12_j2k - p1Loc;
   #ifdef DEBUG_CALC_RANGE
   MessageInterface::ShowMessage(
         "P1Loc = %12.10f   %12.10f   %12.10f\n", p1Loc[0], p1Loc[1], p1Loc[2]);
   MessageInterface::ShowMessage(
         "P2Loc = %12.10f   %12.10f   %12.10f\n", p2Loc[0], p2Loc[1], p2Loc[2]);
   MessageInterface::ShowMessage(
         "R12j2k = %12.10f   %12.10f   %12.10f\n", r12_j2k[0], r12_j2k[1],
         r12_j2k[2]);
   MessageInterface::ShowMessage(
         "At end of CalcRVInertial, rangeVecInertial = %12.10f   %12.10f   %12.10f\n",
         rangeVecInertial[0],rangeVecInertial[1],rangeVecInertial[2]);
   #endif
}

void CoreMeasurement::CalculateRangeVectorObs(Integer p1index, Integer p2index)
{
   Real satEpoch;
   if (participants[p1index]->IsOfType(Gmat::SPACECRAFT))
      satEpoch = participants[p1index]->GetRealParameter(satEpochID);
   else if (participants[p2index]->IsOfType(Gmat::SPACECRAFT))
      satEpoch = participants[p2index]->GetRealParameter(satEpochID);
   else
      throw MeasurementException("Error in CoreMeasurement::"
            "CalculateRangeVectorInertial; neither participant is a "
            "spacecraft.");

   currentMeasurement.epoch = satEpoch;
   CalculateRangeVectorInertial();

   // compute the positions of the participants in their own frames
   p1LocF1 = R_1_j2k * p1Loc;
   p2LocF2 = R_2_j2k * p2Loc;

   // @todo: handle this for arbitrary participants; currently rotates if there
   // are any GS's in the participant list
   if (stationParticipant)
      rangeVecObs = R_o_j2k * rangeVecInertial;
   else
      rangeVecObs = rangeVecInertial;  // same, really, but skip multiplication by I33
   #ifdef DEBUG_CALC_RANGE
   MessageInterface::ShowMessage("stationParticipant = %s\n",
         (stationParticipant? "TRUE" : "false"));
      MessageInterface::ShowMessage(
            "At end of CalcRVObs, rangeVecObs = %12.10f   %12.10f   %12.10f\n",
            rangeVecObs[0],rangeVecObs[1],rangeVecObs[2]);
   #endif
}

void CoreMeasurement::CalculateRangeRateVectorObs(Integer p1index, Integer p2index)
{
   Real satEpoch;
   if (participants[p1index]->IsOfType(Gmat::SPACECRAFT))
      satEpoch = participants[p1index]->GetRealParameter(satEpochID);
   else if (participants[p2index]->IsOfType(Gmat::SPACECRAFT))
      satEpoch = participants[p2index]->GetRealParameter(satEpochID);
   else
      throw MeasurementException("Error in CoreMeasurement::"
            "CalculateRangeVectorInertial; neither participant is a "
            "spacecraft.");

   currentMeasurement.epoch = satEpoch;
   SpacePoint *origin1 = F1->GetOrigin();
   SpacePoint *origin2 = F2->GetOrigin();
   r12_j2k_vel         = origin2->GetMJ2000Velocity(currentMeasurement.epoch) -
                         origin1->GetMJ2000Velocity(currentMeasurement.epoch);
   p1Vel = participants[p1index]->GetMJ2000Velocity(currentMeasurement.epoch);
   p2Vel = participants[p2index]->GetMJ2000Velocity(currentMeasurement.epoch);
   // compute the velocities of the participants in their own frames

   CalculateRangeVectorObs();  // need p1Loc, p2Loc, and r12_j2k, as well as rotation matrices
   p1VelF1 = R_1_j2k * p1Vel;
   p2VelF2 = R_2_j2k * p2Vel;
   if (stationParticipant)
   {
      rangeRateVecObs = (R_o_2 * p2VelF2)       + (RDot_o_2   * p2LocF2) -
                        (R_o_j2k * r12_j2k_vel) - (RDot_o_j2k * r12_j2k) -
                        (R_o_1 * p1VelF1)       - (RDot_o_1   * p1LocF1);
//      rangeRateVecObs = (R_o_j2k * p2Vel)       + (RDot_o_j2k * p2Loc)    -
//      (R_o_j2k * r12_j2k_vel) - (RDot_o_j2k * r12_j2k)  -
//      (R_o_j2k * p1Vel)       - (RDot_o_j2k * p2Loc);
   }
   else // Rs are I33, RDots are 033
      rangeRateVecObs = p2VelF2 - r12_j2k_vel - p1VelF1;

   #ifdef DEBUG_CALC_RANGE
      MessageInterface::ShowMessage("At end of CalcRangeRateVecObs, p1Vel (inertial) = %12.10f   %12.10f   %12.10f\n",
            p1Vel[0], p1Vel[1], p1Vel[2]);
      MessageInterface::ShowMessage("At end of CalcRangeRateVecObs, p2Vel (inertial) = %12.10f   %12.10f   %12.10f\n",
            p2Vel[0], p2Vel[1], p2Vel[2]);
      MessageInterface::ShowMessage("At end of CalcRangeRateVecObs, p1Vel (observation frame) = %12.10f   %12.10f   %12.10f\n",
            p1VelF1[0], p1VelF1[1], p1VelF1[2]);
      MessageInterface::ShowMessage("At end of CalcRangeRateVecObs, p2Vel (observation frame) = %12.10f   %12.10f   %12.10f\n",
            p2VelF2[0], p2VelF2[1], p2VelF2[2]);
      MessageInterface::ShowMessage("    and at end of CalcRangeRateVecObs, rangeRateVecObs = %12.10f   %12.10f   %12.10f\n",
            rangeRateVecObs[0], rangeRateVecObs[1], rangeRateVecObs[2]);
   #endif
}


//------------------------------------------------------------------------------
// void InitializeMeasurement()
//------------------------------------------------------------------------------
/**
 * Prepares coordinate systems for use when building measurements
 */
//------------------------------------------------------------------------------
void CoreMeasurement::InitializeMeasurement()
{
   #ifdef DEBUG_MEASUREMENT_INITIALIZATION
      MessageInterface::ShowMessage(
            "Entering InitializeMeasurement with initialized = %s, and "
            "stationParticipant = %s\n", (initialized? "true" : "false"),
            (stationParticipant? "true" : "false"));
   #endif
   if (initialized)
   {
      return;
   }

   SpaceObject *spObj  = NULL;
   SpacePoint *origin = NULL;

   // get epoch from the second participant, which should always be a spacecraft
   satEpochID = participants[1]->GetParameterID("A1Epoch");

   // grab the participant IDs
   for (UnsignedInt i = 0; i < participants.size(); ++i)
   {
	   currentMeasurement.participantIDs[i] =
               participants[i]->GetStringParameter("Id");
   }

// The following code will eventually replace the hard coded CS's.  Schedule for
// this is TBD.
//
//   // Create the appropriate coordinate systems
//   participantCoords.assign(participants.size(), NULL);
//   if (Fo)
//   {
//      delete Fo;
//      Fo = NULL;
//   }
//
//   for (UnsignedInt i = 0; i < participants.size(); ++i)
//   {
//      if (participants[i]->IsOfType(Gmat::GROUND_STATION))
//      {
//         // For a ground station as participant 1, we want a BodyFixed coordinate
//         // system, with the station's central body as origin, so get the one that
//         // the GroundStation already has
//         BodyFixedPoint   *bf       = (BodyFixedPoint*)participants[i];
//         F1 = bf->GetBodyFixedCoordinateSystem();
//         // Get the SolarSystem pointer from the BodyFixed C.S.
//         solarSystem = F1->GetSolarSystem();
//         // Set up Fo, centered at the GroundStation
//         Fo     = CoordinateSystem::CreateLocalCoordinateSystem("Fo","Topocentric",
//                  bf, NULL, NULL, bf->GetJ2000Body(), solarSystem);
//      }
//      else
//      {
//         // participant 2 must be a Spacecraft
//         spObj  = (SpaceObject*) participants[i];
//         origin = spObj->GetOrigin();
//         F2     = CoordinateSystem::CreateLocalCoordinateSystem("F2", "MJ2000Eq",
//                  origin, NULL, NULL, spObj->GetJ2000Body(), solarSystem);
//      }
//   }

   if (participants[0]->IsOfType(Gmat::GROUND_STATION))
   {
      // For a ground station as participant 1, we want a BodyFixed coordinate
      // system, with the station's central body as origin, so get the one that
      // the GroundStation already has
      BodyFixedPoint   *bf       = (BodyFixedPoint*)participants[0];
      F1 = bf->GetBodyFixedCoordinateSystem();
      // Get the SolarSystem pointer from the BodyFixed C.S.
      solarSystem = F1->GetSolarSystem();
      // participant 2 must be a Spacecraft
      spObj  = (SpaceObject*) participants[1];
      origin = spObj->GetOrigin();
      F2     = CoordinateSystem::CreateLocalCoordinateSystem("F2", "MJ2000Eq",
               origin, NULL, NULL, spObj->GetJ2000Body(), solarSystem);
      // Set up Fo, centered at the GroundStation
      Fo     = CoordinateSystem::CreateLocalCoordinateSystem("Fo","Topocentric",
               bf, NULL, NULL, bf->GetJ2000Body(), solarSystem);
      j2k    = CoordinateSystem::CreateLocalCoordinateSystem("j2k", "MJ2000Eq",
               F1->GetOrigin(), NULL, NULL, bf->GetJ2000Body(), solarSystem);

      #ifdef DEBUG_MEASUREMENT_INITIALIZATION
         MessageInterface::ShowMessage(
               "In InitializeMeasurement, F1 set to an BodyFixed C.S.\n");
         MessageInterface::ShowMessage(
               "                          F2 set to an MJ2000Eq C.S.\n");
         MessageInterface::ShowMessage(
               "                          Fo set to a Topocentric C.S.\n");
      #endif
      // Compute all rotation matrices
   }
//   else
//   {
//      // When both participants are spacecraft, all frames are inertial, so
//      // all rotation matrices are I33 (which is the default on creation) and
//      // all Rotation Dot matrices are zero33
//      RDot_o_2   = zero33;
//      RDot_o_1   = zero33;
//      RDot_o_j2k = zero33;
//   }
   #ifdef DEBUG_MEASUREMENT_INITIALIZATION
      MessageInterface::ShowMessage(
            "In InitializeMeasurement, about to call UpdateRotationMatrix\n");
      if (!participants[1]) MessageInterface::ShowMessage("NULL participant!");
   #endif

   std::string updateAll = "All";
   UpdateRotationMatrix(participants[1]->GetRealParameter(satEpochID),
            updateAll);

   initialized = true;
}


//------------------------------------------------------------------------------
// void UpdateRotationMatrix(Real atEpoch, const std::string &whichOne)
//------------------------------------------------------------------------------
/**
 * Updates the rotation matrices for the passed-in epoch
 *
 * @param atEpoch The epoch of the update
 * @param whichOne Identity of the matrix to update, or "All" for all
 */
//------------------------------------------------------------------------------
void CoreMeasurement::UpdateRotationMatrix(Real atEpoch,
         const std::string &whichOne)
{
   #ifdef DEBUG_GEOMETRIC_COORD_SYSTEMS
      MessageInterface::ShowMessage("Entering UpdateRotationMatrix with epoch "
               "= %12.10f and whichOne = %s\n", atEpoch, whichOne.c_str());
      MessageInterface::ShowMessage("   and stationParticipant = %s\n",
            (stationParticipant? "true" : "false"));
//      MessageInterface::ShowMessage("F1 is a %s with name %s\n",
//            (F1->GetTypeName()).c_str(), (F1->GetName()).c_str());
//      MessageInterface::ShowMessage("F2 is a %s with name %s\n",
//            (F2->GetTypeName()).c_str(), (F2->GetName()).c_str());
//      MessageInterface::ShowMessage("Fo is a %s with name %s\n",
//            (Fo->GetTypeName()).c_str(), (Fo->GetName()).c_str());
//      MessageInterface::ShowMessage("j2k is a %s with name %s\n",
//            (j2k->GetTypeName()).c_str(), (j2k->GetName()).c_str());
   #endif
   if (participants[0]->IsOfType(Gmat::GROUND_STATION))
   {
      Rvector6   dummyIn(1.0,2.0,3.0,  4.0,5.0,6.0);
      Rvector6   dummyOut;
      A1Mjd      itsEpoch(atEpoch);

      if ((whichOne == "All") || (whichOne == "j2k_2"))
      {
         converter.Convert(itsEpoch,dummyIn,F2,dummyOut,j2k);
         R_j2k_2 = converter.GetLastRotationMatrix();
         //               RDot_j2k_2 = converter.GetLastRotationDotMatrix();
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,F2);
         R_2_j2k    = converter.GetLastRotationMatrix();
      }
      if ((whichOne == "All") || (whichOne == "j2k_1"))
      {
         converter.Convert(itsEpoch,dummyIn,F1,dummyOut,j2k);
         R_j2k_1    = converter.GetLastRotationMatrix();
         //               RDot_j2k_1 = converter.GetLastRotationDotMatrix();
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,F1);
         R_1_j2k    = converter.GetLastRotationMatrix();
      }
      if ((whichOne == "All") || (whichOne == "o_2"))
      {
         converter.Convert(itsEpoch,dummyIn,F2,dummyOut,Fo);
         R_o_2      = converter.GetLastRotationMatrix();
         RDot_o_2   = converter.GetLastRotationDotMatrix();
      }
      if ((whichOne == "All") || (whichOne == "o_1"))
      {
         converter.Convert(itsEpoch,dummyIn,F1,dummyOut,Fo);
         R_o_1      = converter.GetLastRotationMatrix();
         RDot_o_1   = converter.GetLastRotationDotMatrix();
      }
      if ((whichOne == "All") || (whichOne == "o_j2k"))
      {
         converter.Convert(itsEpoch,dummyIn,j2k,dummyOut,Fo);
         R_o_j2k    = converter.GetLastRotationMatrix();
         RDot_o_j2k = converter.GetLastRotationDotMatrix();
      }
   }
   else
   {
      // When both participants are spacecraft, all frames are inertial, so all
      // rotation matrices are I33 (which is the default on creation) and all
      // Rotation Dot matrices are zero33
      RDot_o_2   = zero33;
      RDot_o_1   = zero33;
      RDot_o_j2k = zero33;
   }
   #ifdef DEBUG_GEOMETRIC_COORD_SYSTEMS
   MessageInterface::ShowMessage(
      "R_j2k_1 = \n      %12.10f   %12.10f   %12.10f   %12.10f   %12.10f   "
      "%12.10f   %12.10f   %12.10f   %12.10f   \n", R_j2k_1(0,0),R_j2k_1(0,1),
      R_j2k_1(0,2),R_j2k_1(1,0),R_j2k_1(1,1),R_j2k_1(1,2),R_j2k_1(2,0),
      R_j2k_1(2,1),R_j2k_1(2,2));
   MessageInterface::ShowMessage(
      "R_1_j2k = \n      %12.10f   %12.10f   %12.10f   %12.10f   %12.10f   "
      "%12.10f   %12.10f   %12.10f   %12.10f   \n", R_1_j2k(0,0),R_1_j2k(0,1),
      R_1_j2k(0,2),R_1_j2k(1,0),R_1_j2k(1,1),R_1_j2k(1,2),R_1_j2k(2,0),
      R_1_j2k(2,1),R_1_j2k(2,2));
   MessageInterface::ShowMessage(
      "R_o_1 = \n      %12.10f   %12.10f   %12.10f   %12.10f   %12.10f   "
      "%12.10f   %12.10f   %12.10f   %12.10f   \n\n",R_o_1(0,0),R_o_1(0,1),
      R_o_1(0,2),R_o_1(1,0),R_o_1(1,1),R_o_1(1,2),R_o_1(2,0),R_o_1(2,1),
      R_o_1(2,2));
   MessageInterface::ShowMessage(
      "R_j2k_2 = \n      %12.10f   %12.10f   %12.10f   %12.10f   %12.10f   "
      "%12.10f   %12.10f   %12.10f   %12.10f   \n", R_j2k_2(0,0),R_j2k_2(0,1),
      R_j2k_2(0,2),R_j2k_2(1,0),R_j2k_2(1,1),R_j2k_2(1,2), R_j2k_2(2,0),
      R_j2k_2(2,1),R_j2k_2(2,2));
   MessageInterface::ShowMessage(
      "R_2_j2k = \n      %12.10f   %12.10f   %12.10f   %12.10f   %12.10f   "
      "%12.10f   %12.10f   %12.10f   %12.10f   \n", R_2_j2k(0,0),R_2_j2k(0,1),
      R_2_j2k(0,2),R_2_j2k(1,0),R_2_j2k(1,1),R_2_j2k(1,2),R_2_j2k(2,0),
      R_2_j2k(2,1),R_2_j2k(2,2));
   MessageInterface::ShowMessage(
      "R_o_j2k = \n      %12.10f   %12.10f   %12.10f   %12.10f   %12.10f   "
      "%12.10f   %12.10f   %12.10f   %12.10f   \n\n",R_o_j2k(0,0),R_o_j2k(0,1),
      R_o_j2k(0,2),R_o_j2k(1,0),R_o_j2k(1,1),R_o_j2k(1,2),R_o_j2k(2,0),
      R_o_j2k(2,1),R_o_j2k(2,2));
   #endif
}


// Line of sight checks
//------------------------------------------------------------------------------
// bool CheckLOS(Integer p1index, Integer p2index, SpacePoint *cb)
//------------------------------------------------------------------------------
/**
 * Checks the line of sight between 2
 *
 * Checks the line of sight between 2 participants, using a SpacePoint object as
 * the potential obscuring object
 *
 * @param p1Index Index of the first participant
 * @param p2Index Index of the second participant
 * @param cb The potentially obscuring body
 *
 * @return true if line of sight is available, false if not
 */
//------------------------------------------------------------------------------
bool CoreMeasurement::CheckLOS(Integer p1index, Integer p2index, SpacePoint *cb)
{
   bool retval = true;

   if (participants[p1index]->IsOfType(Gmat::GROUND_STATION) ||
       participants[p2index]->IsOfType(Gmat::GROUND_STATION))
   {
      CalculateRangeVectorInertial(p1index, p2index);
      SpacePoint *cb;
      Real epoch;
      if (participants[p1index]->IsOfType(Gmat::GROUND_STATION))
      {
         cb = F1->GetOrigin();
         epoch = participants[p2index]->GetRealParameter(satEpochID);
      }
      else
      {
         cb = F2->GetOrigin();
         // Fix range vector to go from station to sat
         rangeVecInertial *= -1.0;
         epoch = participants[p1index]->GetRealParameter(satEpochID);
      }

      retval = CheckStation2SatLOS(epoch, rangeVecInertial, cb);
   }
   else  // Spacecraft to spacecraft
   {
      Rvector3 p1Loc = participants[p1index]->GetMJ2000Position(
            participants[p1index]->GetRealParameter(satEpochID));
      Rvector3 p2Loc = participants[p2index]->GetMJ2000Position(
            participants[p2index]->GetRealParameter(satEpochID));
      SpacePoint *cb = F1->GetOrigin();

      retval = CheckSat2SatLOS(p1Loc, p2Loc, cb);
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool CheckSat2SatLOS(Rvector3 p1loc, Rvector3 p2loc, SpacePoint *cb)
//------------------------------------------------------------------------------
/**
 * Checks line-of-sight between 2 Spacecraft
 *
 * @param p1loc Location of the first Spacecraft
 * @param p2loc Location of the second Spacecraft
 * @param cb The potentially obscuring body
 *
 * @return true if line of sight is available, false if not
 */
//------------------------------------------------------------------------------
bool CoreMeasurement::CheckSat2SatLOS(Rvector3 p1loc, Rvector3 p2loc,
      SpacePoint *cb)
{
   bool retval = true;

   Rvector3 rho = p2loc - p1loc;
   Real rhoMag = rho.GetMagnitude();
   Real tau = (rho * p2loc) / (rhoMag * rhoMag);

   if (!solarSystem)
      throw MeasurementException("CoreMeasurement::CheckSat2SatLOS: Solar System is NULL; Cannot obtain Earth radius\n");
   CelestialBody *earth= solarSystem->GetBody(GmatSolarSystemDefaults::EARTH_NAME);
   if (!earth)
      throw MeasurementException("CoreMeasurement::CheckSat2SatLOS: Cannot obtain Earth radius\n");
   Real radius = earth->GetEquatorialRadius();

   Real occlusion = 50.0;

   if ((tau > 0.0) && (tau < 1.0)) // tau < 0 or > 1 ==> both on same side of cb
   {
      if (cb != NULL)
         if (cb->IsOfType(Gmat::CELESTIAL_BODY))
         {
            radius = cb->GetRealParameter("EquatorialRadius");
            //occlusion = cb->GetRealParameter("Occlusion");
         }

      Rvector3 closest = p2loc - tau * rho;
      if (closest.GetMagnitude() < radius + occlusion)
         retval = false;

      #ifdef DEBUG_LINE_OF_SIGHT
         MessageInterface::ShowMessage("Sat2Sat LOS is %s; closest = %.4lf, "
               "%s radius = %.4lf, tau = %.4lf\n", (retval ? "true" : "false"),
               closest.GetMagnitude(), (cb == NULL ? "" : cb->GetName().c_str()),
               radius, tau);
      #endif
   }
   #ifdef DEBUG_LINE_OF_SIGHT
      else
         MessageInterface::ShowMessage("Sat2Sat LOS is %s; tau = %.4lf\n",
               (retval ? "true" : "false"), tau);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool CheckStation2SatLOS(Real a1Epoch, Rvector3 rVec, SpacePoint *cb)
//------------------------------------------------------------------------------
/**
 * Checks line-of-sight between a Spacecraft and a ground station
 *
 * @param a1Epoch Epoch for the ground station location
 * @param rVec Location of the Spacecraft
 * @param cb an extra body that could interfere (Not currently used)
 *
 * @return true if line of sight is available, false if not
 */
//------------------------------------------------------------------------------
bool CoreMeasurement::CheckStation2SatLOS(Real a1Epoch, Rvector3 rVec,
      SpacePoint *cb)
{
   bool retval = true;
   Rvector3 outState;

   // Set feasibility off of topocentric horizon, set by the Z value in
   // topocentric coordinates
   std::string updateAll = "All";
   UpdateRotationMatrix(a1Epoch, updateAll);
   outState = R_o_j2k * rVec;
   Real feasibilityValue = outState[2];

   if (feasibilityValue <= 0.0)
      retval = false;

   currentMeasurement.feasibilityValue = outState[2];

   return retval;
}


//------------------------------------------------------------------------------
// void DumpParticipantStates(const std::string& ref)
//------------------------------------------------------------------------------
/**
 * Debug method used to view the MJ2000 states of measurement participants
 *
 * @param ref A string printed prior to the data dump
 */
//------------------------------------------------------------------------------
void CoreMeasurement::DumpParticipantStates(const std::string& ref)
{
   MessageInterface::ShowMessage("%s\n", ref.c_str());

   UnsignedInt size = participants.size();
   MessageInterface::ShowMessage("   %d participants\n", size);

   // Grab an epoch
   GmatEpoch when = -1.0;
   for (UnsignedInt i = 0; i < size; ++i)
      if (participants[i]->IsOfType(Gmat::SPACEOBJECT))
      {
         if (when == -1.0)
            when = participants[i]->GetRealParameter("A1Epoch");
         else
            if (participants[i]->GetRealParameter("A1Epoch") != when)
               MessageInterface::ShowMessage("WARNING: Epochs do not match in "
                     "call to CoreMeasurement::DumpParticipantStates\n");
      }

   for (UnsignedInt i = 0; i < size; ++i)
   {
      MessageInterface::ShowMessage("   Participant %d, Name:  %s\n",
            i, participants[i]->GetName().c_str());
      MessageInterface::ShowMessage("      Epoch: %.12lf\n", when);
      Rvector3 vec = participants[i]->GetMJ2000Position(when);
      MessageInterface::ShowMessage("      J2000 Position:  [%.12lf  %.12lf  "
            "%.12lf]\n", vec[0], vec[1], vec[2]);
      vec = participants[i]->GetMJ2000Velocity(when);
      MessageInterface::ShowMessage("      J2000 Velocity:  [%.12lf  "
            "%.12lf  %.12lf]\n", vec[0], vec[1], vec[2]);
   }
}


//------------------------------------------------------------------------------
// bool SetParticipantHardware(GmatBase *obj, const std::string &hwName)
//------------------------------------------------------------------------------
/**
 * Gets Hardware elements from participants when needed
 *
 * @param obj The participant that has the Hardware
 * @param hwName The name of the element
 *
 * @return true if the Hardware was retrieved, false if not
 */
//------------------------------------------------------------------------------
bool CoreMeasurement::SetParticipantHardware(GmatBase *obj,
      const std::string &hwName, Integer hwIndex)
{
   bool retval = false;

   // Check that obj is a participant
   for (UnsignedInt i = 0; i < participants.size(); ++i)
   {
      if (participants[i] == obj)
      {
         // Retrieve the hardware element from the participant
         GmatBase *gb = obj->GetRefObject(Gmat::HARDWARE, hwName);
         if (gb == NULL)
            throw MeasurementException(obj->GetName() + " does not have a "
                  "hardware element named " + hwName);
         if (gb->IsOfType(Gmat::HARDWARE))
         {
            #ifdef DEBUG_HARDWARE
               MessageInterface::ShowMessage("   Found %s!\n", hwName.c_str());
            #endif

            if (participantHardware[i].size() > (UnsignedInt)hwIndex)
               participantHardware[i][hwIndex] = (Hardware*)gb;
            else if (participantHardware[i].size() == (UnsignedInt)hwIndex)
               participantHardware[i].push_back((Hardware*)gb);
            else
               throw MeasurementException("Measurement Hardware Index is out "
                     "of bounds");
            retval = true;
         }
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Real GetDelay(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * Retrieves a delay value
 *
 * @param forParticipant Index of the participant supporting the object
 *                       supplying the delay
 * @param whichOne Index of the desired delay
 *
 * @return The delay
 */
//------------------------------------------------------------------------------
Real CoreMeasurement::GetDelay(UnsignedInt forParticipant, Integer whichOne)
{
   #ifdef DEBUG_DELAY
      MessageInterface::ShowMessage("CoreMeasurement<%p>::GetDelay(%d)\n", this,
            whichOne);
   #endif

   Real hwDelay = 0.0;

   if (forParticipant > participants.size())
   {
      std::stringstream msg;
      msg << "Attempting to get a delay for participant "
          << forParticipant
          << " but the measurement only has "
          << (participants.size())
          << " participants.";
      throw MeasurementException(msg.str());
   }

   // Old code: earlier builds added up all delays for a participant if -1 was
   // passed in.  I don't think that is what we want, but am leaving it in
   // place (and commented out) just in case.  We'll want to remove this before
   // release.
//   if (whichOne < 0)
//   {
//      for (UnsignedInt i = 0; i < participantHardware[forParticipant].size(); ++i)
//      {
//         #ifdef DEBUG_DELAY
//            if (participantHardware[i] != NULL)
//               MessageInterface::ShowMessage("   +++ %d <- %s <%p>\n", i,
//                     participantHardware[i]->GetName().c_str(),
//                     participantHardware[i]);
//            else
//               MessageInterface::ShowMessage("   +++ %d <- <%p>\n", i,
//                     participantHardware[i]);
//         #endif
//         if (participantHardware[forParticipant][i] != NULL)
//            if (participantHardware[forParticipant][i]->IsOfType("Sensor"))
//            {
//               hwDelay += ((Sensor*)
//                     (participantHardware[forParticipant][i]))->GetDelay();
//            }
//      }
//   }
//   else
//   {
      if (whichOne < (Integer)participantHardware[forParticipant].size())
      {
         #ifdef DEBUG_DELAY
            MessageInterface::ShowMessage("   +++ %d <- %s <%p>\n", whichOne,
                  participantHardware[forParticipant][whichOne]->GetName().c_str(),
                  participantHardware[forParticipant][whichOne]);
         #endif
         if (participantHardware[forParticipant][whichOne] != NULL)
         {
            if (participantHardware[forParticipant][whichOne]->IsOfType("Sensor"))
               hwDelay = ((Sensor*)(participantHardware[forParticipant][whichOne]))->GetDelay();
         }
      }
      // No hardware means zero delay
//      else
//      {
//         std::stringstream msg;
//         msg << "Attempting to get a delay for hardware element "
//             << whichOne
//             << " but the participant only has "
//             << (participantHardware[forParticipant].size())
//             << " hardware elements registered in the measurement for this "
//                "participant.";
//         throw MeasurementException(msg.str());
//      }
//   }

   #ifdef DEBUG_DELAY
      MessageInterface::ShowMessage("Delay: %.6le)\n", hwDelay);
   #endif

   return hwDelay;
}


//------------------------------------------------------------------------------
// Integer GetMeasurementParameterCount() const
//------------------------------------------------------------------------------
/**
 * Retrieves the count of scriptable parameters for the measurement
 *
 * This method is provided so that the MeasurementModel container can query the
 * measurement for any additional parameters that can be scripted specific to
 * that measurement.  Derived classes that need additional parameter support
 * override this method to provide that support.
 *
 * @return The number of additional parameters.  Note that this return value
 *         will not be zero because of the parameters in GmatBase.  For now,
 *         the MeasurementModel will ignore those parameters.  (The only one
 *         in code at this writing is Covariance.)
 */
//------------------------------------------------------------------------------
Integer CoreMeasurement::GetMeasurementParameterCount() const
{
   return CoreMeasurementParamCount;
}
