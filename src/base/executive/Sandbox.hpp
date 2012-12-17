//$Id$
//------------------------------------------------------------------------------
//                                 Sandbox
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/10/08
//
/**
 * Definition for the GMAT Sandbox class
 */
//------------------------------------------------------------------------------


#ifndef Sandbox_hpp
#define Sandbox_hpp

#include "gmatdefs.hpp"

// Core classes
#include "GmatBase.hpp"
#include "GmatCommand.hpp"
#include "Publisher.hpp"

#include "Spacecraft.hpp"
#include "PropSetup.hpp"
#include "Subscriber.hpp"
#include "Parameter.hpp"            // Remove when temporary code satisfied
#include "SolarSystem.hpp"
#include "CoordinateSystem.hpp"
#include "Burn.hpp"
#include "Function.hpp"
#include "ObjectInitializer.hpp"
#include "EventLocator.hpp"

//#define DEBUG_SANDBOX_CLONING

class Moderator;        // Forward reference for the moderator pointer
class BranchCommand;    // For owned clone management with branch commands

/**
 * The GMAT workspace for running missions
 */
class GMAT_API Sandbox
{
public:
   Sandbox();
   ~Sandbox();
   
   // Setup methods
   GmatBase* AddObject(GmatBase *obj);   
   bool AddCommand(GmatCommand *cmd);
   bool AddSolarSystem(SolarSystem *ss);
   bool AddTriggerManagers(const std::vector<TriggerManager*> *trigs);
   bool AddSubscriber(Subscriber *sub);
   bool SetInternalCoordSystem(CoordinateSystem *ss);
   bool SetPublisher(Publisher *pub = NULL);
   
   GmatBase* GetInternalObject(std::string name,
                               Gmat::ObjectType type = Gmat::UNKNOWN_OBJECT);
   
   // Execution methods
   bool Initialize();
   bool Execute();
   bool Interrupt();
   void Clear();

protected:
    
private:
   /// Enumerated values for the current Sandbox state
   enum runMode
   {
      IDLE = 7001,
      INITIALIZED,
      RUNNING,
      PAUSED,
      STOPPED,
      REFRESH,
      RESET
   };

   enum updateMethod
   {
      PASS_TO_ALL,            // The "brute force" clone update method
      PASS_TO_REGISTERED,     // Use this method when all clones are registered
      SKIP_UPDATES            // Set this method to revert to pre-clone updates
   };
    
   /// Object store for this run
   std::map<std::string, GmatBase *> objectMap;
   /// Global object store for this run
   std::map<std::string, GmatBase *> globalObjectMap;
   /// Combined object store for passing to interpreter (via Moderator)
   std::map<std::string, GmatBase *> combinedObjectMap;
   /// Solar System model for this Sandbox
   SolarSystem                       *solarSys;
   /// CoordinateSystem used internally
   CoordinateSystem                  *internalCoordSys;
   /// GMAT Publisher singleton
   Publisher                         *publisher;
   /// Command sequence for the run
   GmatCommand                       *sequence;
   /// The executing Command
   GmatCommand                       *current;
   /// Pointer to the moderator, used to check Pause/Stop status
   Moderator                         *moderator;
   /// Execution mode for the sandbox
   runMode                           state;
   /// Counter for interrupt polling
   Integer                           interruptCount;
   /// Polling frequency
   Integer                           pollFrequency;
   /// The object initializer
   ObjectInitializer                 *objInit;
   /// Update method used for owned clones
   updateMethod                      cloneUpdateStyle;
   
   /// List of FiniteThrust objects that are currently available
   std::vector<PhysicalModel *>      transientForces;
   /// List of EventLocation objects used to find event entry and exit
   std::vector<EventLocator *>       events;
   /// Trigger managers for this Sandbox
   std::vector<TriggerManager*>      triggerManagers;

   Sandbox(const Sandbox&);
   Sandbox& operator=(const Sandbox&);
   
   GmatBase* FindObject(const std::string &name);
   bool      SetObjectByNameInMap(const std::string &name, GmatBase *obj);
   bool      HandleGmatFunction(GmatCommand *cmd,
                                std::map<std::string, GmatBase *> *usingMap);
   void      SetGlobalRefObject(GmatCommand *cmd);
   void      ShowObjectMap(ObjectMap &om, const std::string &title);
   bool      AddOwnedSubscriber(Subscriber *sub);
   
   void      UpdateClones(GmatBase *obj, Integer updatedParameterIndex);
   void      PassToAll(GmatBase *obj, Integer updatedParameterIndex);
   void      PassToRegisteredClones(GmatBase *obj, Integer updatedParameterIndex);

   void      PassToBranchCommand(GmatBase *theClone,
                                 BranchCommand *theBranchCommand,
                                 Integer updatedParameterIndex);

   void      UpdateAndInitializeCloneOwner(GmatBase *theClone,
                                           GmatBase *theOwner,
                                           Integer updatedParameterIndex);

   #ifdef DEBUG_SANDBOX_CLONING
      std::vector<Gmat::ObjectType>  clonable;
   #endif
};

#endif
