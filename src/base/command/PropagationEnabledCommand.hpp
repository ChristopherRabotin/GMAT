//$Id$
//------------------------------------------------------------------------------
//                       PropagationEnabledCommand
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
// Created: 2009/07/20
//
/**
 * Definition of the PropagationEnabledCommand base class
 */
//------------------------------------------------------------------------------


#ifndef PropagationEnabledCommand_hpp
#define PropagationEnabledCommand_hpp

#include "GmatCommand.hpp"
#include "PropSetup.hpp"
#include "Propagator.hpp"

#include "Spacecraft.hpp"
#include "FormationInterface.hpp"
#include "RootFinder.hpp"


/// A convenient typedef used in this code
typedef std::vector<GmatBase*> PropObjectArray;


#define TIME_ROUNDOFF 1.0e-6
#define DEFAULT_STOP_TOLERANCE 1.0e-7



/**
 * PropagationEnabledCommand is a base class used for commands that perform
 * propagation.  It provides the methods and interfaces needed perform basic
 * time-based propagation.  It does not provide interfaces for more complicated
 * stopping conditions; derived classes provide those interfaces.
 */
class GMAT_API PropagationEnabledCommand : public GmatCommand
{
public:
   PropagationEnabledCommand(const std::string &typeStr);
   // Abstract to force a derived class to instantiate
   virtual ~PropagationEnabledCommand() = 0;
   PropagationEnabledCommand(const PropagationEnabledCommand& pec);
   PropagationEnabledCommand& operator=(const PropagationEnabledCommand& pec);

   virtual void SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual bool Initialize();

   virtual bool HasLocalClones();
   virtual void UpdateClonedObject(GmatBase *obj);
   virtual void UpdateClonedObjectParameter(GmatBase *obj,
         Integer updatedParameterId);

protected:
   // todo: Merge the propagator objects in the Propagate command into this code

   /// Names of the PropSetups used by this command, as set in a derived class
   StringArray                   propagatorNames;
   /// Step direction multipliers used to switch btwn forwards & backwards prop
   Real                          direction;
   /// The PropSetup used by this command, as set in a derived class
   std::vector<PropSetup*>       propagators;
   /// Flag used to indicate that the PropSetups were built in a derived class
   bool                          overridePropInit;
   /// The objects that are propagated; one StringArray per PropSetup
   std::vector<StringArray>      propObjectNames;
   /// The objects that are propagated; one PropObjectArray per PropSetup
   std::vector<PropObjectArray*> propObjects;

   /// Flag indicating that the command has been executed once, so that some
   /// pieces of initialization can be skipped
   bool hasFired;
   /// Flag indicating the command is currently executing; used for reentrance
   bool inProgress;

   /// The size of the propagation state vector
   Integer dim;

   /// ID for the spacecraft epoch parameter
   Integer                      epochID;
   /// Starting epoch for the propagation
   std::vector<GmatEpoch>       baseEpoch;
   /// Epoch where a derivative zero was located
   GmatEpoch                    derivativeEpoch;

   /// Time elapsed during this Propagate
   RealArray                    elapsedTime;
   /// Start epoch for the step
   RealArray                    currEpoch;
   /// The Propagators
   std::vector<Propagator*>     p;
   /// The ForceModels
   std::vector<ODEModel*>       fm;
   /// The Propagation State Managers
   std::vector<PropagationStateManager*>  psm;
   /// List of forces that can be turned on or off by other commands
   std::vector<PhysicalModel*>  *transientForces;

   /// The Mean-of-J2000 propagation state vector data
   Real                         *j2kState;
   /// Data sent to the Publisher
   Real                         *pubdata;

   /// Stopping condition evaluation requires propagation; the satBuffer and
   /// formBuffer let us restore the Spacecraft and Formations to the state
   /// needed for the last step
   std::vector<Spacecraft *>    satBuffer;
   std::vector<FormationInterface *>     formBuffer;

   // Event location management structures
   /// Number of active events in the current propagation
   Integer              activeLocatorCount;
   /// Indices of the active events
   std::vector<Integer> activeEventIndices;
   /// Start index in the previous event buffer for the function data
   std::vector<Integer> eventStartIndices;
   /// Values of event location data last time called
   Real                 *previousEventData;
   /// Values of event location data in the current call
   Real                 *currentEventData;
   /// Values of event location data used while searching
   Real                 *tempEventData;
   /// Total number of elements in the data buffers
   UnsignedInt          eventBufferSize;
   /// Root finder used in event location
   RootFinder           *finder;
   /// Flag used to turn off publishing during event location
   bool                 publishOnStep;

   bool                 PrepareToPropagate();
   bool                 AssemblePropagators();
   bool                 Step(Real dt);
   virtual bool         TakeAStep(Real propStep = 0.0);

   virtual void         AddToBuffer(SpaceObject *so);
   virtual void         EmptyBuffer();
   virtual void         BufferSatelliteStates(bool fillingBuffer = true);

   virtual void         SetPropagationProperties(PropagationStateManager *psm);

   virtual void         LocateObjectEvents(const GmatBase *obj,
                              ObjectArray &els);
   virtual void         AddLocators(PropagationStateManager *currentPSM,
                              ObjectArray &els);
   virtual void         InitializeForEventLocation();
   virtual void         CheckForEvents();
   virtual bool         LocateEvent(EventLocator* el, Integer index = 0,
                              bool forDerivative = false);
   virtual void         UpdateEventTable(EventLocator* el, Integer index);
};

#endif /* PropagationEnabledCommand_hpp */
