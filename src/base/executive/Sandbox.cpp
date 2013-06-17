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
 * Implementation for the GMAT Sandbox class
 */
//------------------------------------------------------------------------------

#include "Sandbox.hpp"
#include "Moderator.hpp"
#include "SandboxException.hpp"
#include "Parameter.hpp"
#include "FiniteThrust.hpp"
#include "Function.hpp"
#include "CallFunction.hpp"
#include "Assignment.hpp"
#include "BranchCommand.hpp"
#include "SubscriberException.hpp"
#include "CommandUtil.hpp"         // for GetCommandSeqString()
#include "MessageInterface.hpp"

#include <algorithm>       // for find

//#define DISABLE_SOLAR_SYSTEM_CLONING

//#define DISALLOW_NESTED_GMAT_FUNCTIONS

//#define DEBUG_SANDBOX_INIT
//#define DEBUG_MODERATOR_CALLBACK
//#define DEBUG_SANDBOX_GMATFUNCTION
//#define DEBUG_SANDBOX_OBJ_INIT
//#define DEBUG_SANDBOX_OBJ_ADD
//#define DEBUG_SANDBOX_OBJECT_MAPS
//#define DBGLVL_SANDBOX_RUN 2
//#define DEBUG_SANDBOX_CLEAR
//#define DEBUG_SANDBOX_CLONING
//#define DEBUG_SS_CLONING
//#define DEBUG_EVENTLOCATION
//#define DEBUG_CLONE_UPDATES
//#define REPORT_CLONE_UPDATE_STATUS

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

#ifdef DEBUG_SANDBOX_INIT
   std::map<std::string, GmatBase *>::iterator omIter;
#endif


//------------------------------------------------------------------------------
// Sandbox::Sandbox()
//------------------------------------------------------------------------------
/**
 *  Default constructor.
 */
//------------------------------------------------------------------------------
Sandbox::Sandbox() :
   solarSys          (NULL),
   internalCoordSys  (NULL),
   publisher         (NULL),
   sequence          (NULL),
   current           (NULL),
   moderator         (NULL),
   state             (IDLE),
   interruptCount    (45),
   pollFrequency     (50),
   objInit           (NULL),
//   cloneUpdateStyle  (SKIP_UPDATES)
   cloneUpdateStyle  (PASS_TO_ALL)
{
}


//------------------------------------------------------------------------------
// ~Sandbox()
//------------------------------------------------------------------------------
/**
 *  Destructor.
 */
//------------------------------------------------------------------------------
Sandbox::~Sandbox()
{
   #ifndef DISABLE_SOLAR_SYSTEM_CLONING   
      if (solarSys)
      {
         #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (solarSys, solarSys->GetName(), "Sandbox::~Sandbox()",
             " deleting cloned solarSys");
         #endif
         delete solarSys;
      }
   #endif
   
   for (UnsignedInt i = 0; i < triggerManagers.size(); ++i)
      delete triggerManagers[i];

   if (sequence)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (sequence, "sequence", "Sandbox::~Sandbox()",
          " deleting mission sequence");
      #endif
      delete sequence;
   }
   
   if (objInit)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (objInit, "objInit", "Sandbox::~Sandbox()", " deleting objInit");
      #endif
      delete objInit;
   }
   
   // Delete the local objects
   Clear();
}


//------------------------------------------------------------------------------
// Setup methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// GmatBase* AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 *  Adds an object to the Sandbox's object container.
 *
 *  Objects are added to the Sandbox by cloning the objects.  That way local
 *  copies can be manipulated without affecting the objects managed by the
 *  ConfigurationManager.
 *
 *  @param <obj> The object that needs to be included in the Sandbox.
 *
 *  @return Cloned object pointer if the object was added to the Sandbox's
 *          container, NULL if it was not.
 */
//------------------------------------------------------------------------------
//Changed to return GmatBase* (loj: 2008.11.06)
GmatBase* Sandbox::AddObject(GmatBase *obj)
{
   if (obj == NULL)
      return NULL;
   
   #ifdef DEBUG_SANDBOX_OBJ_ADD
      MessageInterface::ShowMessage
         ("Sandbox::AddObject() objTypeName=%s, objName=%s\n",
          obj->GetTypeName().c_str(), obj->GetName().c_str());
   #endif
      
   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

   state = IDLE;
   
   std::string name = obj->GetName();
   if (name == "")
      return NULL;  // No unnamed objects in the Sandbox tables
   
   GmatBase *cloned = obj;
   
   // Check to see if the object is already in the map
   if (FindObject(name) == NULL)
   {
      // If not, store the new object pointer
      #ifdef DEBUG_SANDBOX_CLONING
         MessageInterface::ShowMessage("Cloning %s <%p> -> ",
               obj->GetName().c_str(), obj);
      #endif
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
      MessageInterface::ShowMessage(
         "Cloning object %s of type %s\n", obj->GetName().c_str(),
         obj->GetTypeName().c_str());
      #endif

      cloned = obj->Clone();

      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Add
         (cloned, obj->GetName(), "Sandbox::AddObject()",
          "*cloned = obj->Clone()");
      #endif

      SetObjectByNameInMap(name, cloned);

      #ifdef DEBUG_SANDBOX_CLONING
         MessageInterface::ShowMessage("<%p>\n", cloned);

         if (cloned->IsOfType(Gmat::PROP_SETUP))
            MessageInterface::ShowMessage("   PropSetup propagator <%p> -> "
                  "<%p>\n", ((PropSetup*)(obj))->GetPropagator(),
                  ((PropSetup*)(cloned))->GetPropagator());
      #endif

      // Register owned Subscribers with the Publisher
      Integer count = cloned->GetOwnedObjectCount();

      for (Integer i = 0; i < count; ++i)
      {
         GmatBase* oo = cloned->GetOwnedObject(i);
         if (oo->IsOfType(Gmat::SUBSCRIBER))
         {
            AddOwnedSubscriber((Subscriber*)oo);
         }
      }
   }
   else
   {
      MessageInterface::ShowMessage
         ("in Sandbox::AddObject() %s is already in the map\n", name.c_str());
   }
   
   return cloned;
}


//------------------------------------------------------------------------------
// bool AddCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 *  Adds a command to the Sandbox's command sequence.
 *
 *  Command are added to the command srquence by appending them ti the command
 *  list, using the GmatCommand::Append() method.
 *
 *  @param <cmd> The command that needs to be added to this Sandbox's sequence.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddCommand(GmatCommand *cmd)
{

   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

  state = IDLE;


   if (!cmd)
      return false;

   if (cmd == sequence)
      return true;

   if (sequence)
      return sequence->Append(cmd);


   sequence = cmd;
   return true;
}


//------------------------------------------------------------------------------
// bool AddSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 *  Sets the SolarSystem for this Sandbox by cloning the input solar system.
 *
 *  @param <ss> The SolarSystem this Sandbox's will use.
 *
 *  @return true if the solar system was added to the Sandbox, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddSolarSystem(SolarSystem *ss)
{
   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");
   state = IDLE;
   
   if (!ss)
      return false;
   
#ifdef DISABLE_SOLAR_SYSTEM_CLONING
   solarSys = ss;
#else
   if (GmatGlobal::Instance()->GetRunMode() == GmatGlobal::TESTING)
      MessageInterface::LogMessage("Cloning the solar system in the Sandbox\n");
   
   solarSys = (SolarSystem*)(ss->Clone());
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (solarSys, solarSys->GetName(), "Sandbox::AddSolarSystem()",
       "solarSys = (SolarSystem*)(ss->Clone())");
   #endif
   
   #ifdef DEBUG_SS_CLONING
   MessageInterface::ShowMessage("Sandbox cloned the solar system: %p\n", solarSys);
   #endif
#endif
   return true;
}


//------------------------------------------------------------------------------
// bool AddTriggerManagers(const std::vector<TriggerManager*> *trigs)
//------------------------------------------------------------------------------
/**
 * This method...
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
bool Sandbox::AddTriggerManagers(const std::vector<TriggerManager*> *trigs)
{
   bool retval = true;

   for (UnsignedInt i = 0; i < triggerManagers.size(); ++i)
      delete triggerManagers[i];
   triggerManagers.clear();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Sandbox received %d trigger managers\n",
            trigs->size());
   #endif
   
   for (UnsignedInt i = 0; i < trigs->size(); ++i)
   {
      TriggerManager *trigMan = (*trigs)[i]->Clone();
      if (trigMan != NULL)
         triggerManagers.push_back(trigMan);
      else
      {
         MessageInterface::ShowMessage("Unable to clone a TriggerManager -- "
               "please check the copy constructor and assignment operator");
         retval = false;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 *  Sets the internal coordinate system used by the Sandbox.
 *
 *  @param <cs> The internal coordinate system.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::SetInternalCoordSystem(CoordinateSystem *cs)
{
   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

   state = IDLE;

   if (!cs)
      return false;

   /// @todo Check initialization and cloning for the internal CoordinateSystem.
   //internalCoordSys = (CoordinateSystem*)(cs->Clone());
   internalCoordSys = cs;
   return true;
}


//------------------------------------------------------------------------------
// bool SetPublisher(Publisher *pub)
//------------------------------------------------------------------------------
/**
 *  Sets the Publisher so the Sandbox can pipe data to the rest of GMAT.
 *
 *  @param <pub> The GMAT Publisher.
 *
 *  @return true if the command was added to the sequence, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::SetPublisher(Publisher *pub)
{
   if ((state != INITIALIZED) && (state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");
   state = IDLE;
   
   if (pub)
   {
      publisher = pub;
      // Now publisher needs internal coordinate system
      publisher->SetInternalCoordSystem(internalCoordSys);
      return true;
   }
   
   if (!publisher)
      return false;
   
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* GetInternalObject(std::string name, Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 *  Accesses objects managed by this Sandbox.
 *
 *  @param <name> The name of the object.
 *  @param <name> type of object requested.
 *
 *  @return The pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* Sandbox::GetInternalObject(std::string name, Gmat::ObjectType type)
{
   #ifdef DEBUG_INTERNAL_OBJ
   MessageInterface::ShowMessage
      ("Sandbox::GetInternalObject() name=%s, type=%d\n", name.c_str(), type);
   #endif
   
   GmatBase* obj = NULL;
   
   if ((obj = FindObject(name)) != NULL) 
   {
      if (type != Gmat::UNKNOWN_OBJECT)
      {
         if (obj->GetType() != type) 
         {
            std::string errorStr = "GetInternalObject type mismatch for ";
            errorStr += name;
            throw SandboxException(errorStr);
         }
      }
   }
   else 
   {
      std::string errorStr = "Sandbox::GetInternalObject(" + name +
                             "...) Could not find \"";
      errorStr += name;
      errorStr += "\" in the Sandbox.";
      
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("Here is the current object map:\n");
         for (std::map<std::string, GmatBase *>::iterator i = objectMap.begin();
              i != objectMap.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->first.c_str());
         MessageInterface::ShowMessage("Here is the current global object map:\n");
         for (std::map<std::string, GmatBase *>::iterator i = globalObjectMap.begin();
              i != globalObjectMap.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->first.c_str());
      #endif
      
      throw SandboxException(errorStr);
   }

   return obj;
}


//------------------------------------------------------------------------------
// Execution methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 *  Established the internal linkages between objects needed prior to running a
 *  mission sequence.
 *
 *  @return true if everything was connected properly, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::Initialize()
{
   #ifdef DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage("Initializing the Sandbox\n");
      MessageInterface::ShowMessage
			("At the start, the Sandbox Object Map contains %d objects:\n", objectMap.size());
      for (omIter = objectMap.begin(); omIter != objectMap.end(); ++omIter)
         MessageInterface::ShowMessage("   %s of type %s\n",
               (omIter->first).c_str(), ((omIter->second)->GetTypeName()).c_str());
      MessageInterface::ShowMessage
			("At the start, the Global Object Map contains %d objects:\n", globalObjectMap.size());
      for (omIter = globalObjectMap.begin(); omIter != globalObjectMap.end(); ++omIter)
         MessageInterface::ShowMessage("   %s of type %s\n",
               (omIter->first).c_str(), ((omIter->second)->GetTypeName()).c_str());
      MessageInterface::ShowMessage(" ........ \n");
   #endif

   bool rv = false;


   if (moderator == NULL)
      moderator = Moderator::Instance();
   
   transientForces.clear();
   events.clear();
   
   // Set transient force vector on Parameters that need it
   for (std::map<std::string,GmatBase*>::iterator i = objectMap.begin();
         i != objectMap.end(); ++i)
   {
      GmatBase *obj = (*i).second;
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         if (((Parameter*)obj)->NeedsForces())
            ((Parameter*)obj)->SetTransientForces(&transientForces);
      }
   }
   // Set transient force vector on Parameters that need it
   for (std::map<std::string,GmatBase*>::iterator i = globalObjectMap.begin();
         i != globalObjectMap.end(); ++i)
   {
      GmatBase* obj = (*i).second;
      if (obj->IsOfType(Gmat::PARAMETER))
      {
         if (((Parameter*)obj)->NeedsForces())
            ((Parameter*)obj)->SetTransientForces(&transientForces);
      }
   }

   // Already initialized
   if (state == INITIALIZED)
      return true;


   current = sequence;
   if (!current)
      throw SandboxException("No mission sequence defined in the Sandbox!");


   if (!internalCoordSys)
      throw SandboxException(
         "No reference (internal) coordinate system defined in the Sandbox!");


   std::map<std::string, GmatBase *>::iterator omi;
   GmatBase *obj = NULL;
   std::string oName;
   std::string j2kName;


   // Set the solar system references
   if (solarSys == NULL)
      throw SandboxException("No solar system defined in the Sandbox!");
   
   // Initialize the solar system, internal coord system, etc.

   // Set J2000 Body for all SpacePoint derivatives before anything else
   // NOTE - at this point, everything should be in the SandboxObjectMap,
   // and the GlobalObjectMap should be empty
   #ifdef DEBUG_SANDBOX_OBJ_INIT
      MessageInterface::ShowMessage("About to create the ObjectInitializer ... \n");
      MessageInterface::ShowMessage(" and the objInit pointer is %s\n",
            (objInit? "NOT NULL" : "NULL!!!"));
   #endif

   if (objInit)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (objInit, "objInit", "Sandbox::Initialize()", " deleting objInit");
      #endif
      delete objInit;  // if Initialize is called more than once, delete 'old' objInit
   }
   
   objInit = new ObjectInitializer(solarSys, &objectMap, &globalObjectMap,
         internalCoordSys);
   
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (objInit, "objInit", "Sandbox::Initialize()", "objInit = new ObjectInitializer");
   #endif
   try
   {
      #ifdef DEBUG_SANDBOX_OBJ_INIT
         MessageInterface::ShowMessage(
               "About to call the ObjectInitializer::InitializeObjects ... \n");
      #endif
      objInit->InitializeObjects();
   }
   catch (BaseException &be)
   {
      SandboxException se("");
      se.SetDetails("Error initializing objects in Sandbox.\n%s\n",
                    be.GetFullMessage().c_str());
      throw se;
      //throw SandboxException("Error initializing objects in Sandbox");
   }
   
   // Move global objects to the Global Object Store
   combinedObjectMap = objectMap;
   StringArray movedObjects;
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;
      #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage(
            "Sandbox::checking object %s (of type %s) \n",
            (omi->first).c_str(), (obj->GetTypeName()).c_str());
      #endif
      // Check the isGlobal flag
      if (obj->GetIsGlobal())
      {
         #ifdef DEBUG_SANDBOX_INIT
            MessageInterface::ShowMessage(
               "Sandbox::moving object <%p>'%s' to the Global Object Store\n",
               omi->second, (omi->first).c_str());
         #endif
         globalObjectMap.insert(*omi);
         movedObjects.push_back(omi->first);
      }
   }
   for (unsigned int ii = 0; ii < movedObjects.size(); ii++)
      objectMap.erase(movedObjects.at(ii));
   movedObjects.clear();  
   
   #ifdef DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage("--- Right AFTER moving things to the GOS --- \n");
      MessageInterface::ShowMessage("The Sandbox Object Map contains:\n");
      for (omIter = objectMap.begin(); omIter != objectMap.end(); ++omIter)
         MessageInterface::ShowMessage("   %s of type %s\n",
               (omIter->first).c_str(), ((omIter->second)->GetTypeName()).c_str());
      MessageInterface::ShowMessage("The Global Object Map contains:\n");
      for (omIter = globalObjectMap.begin(); omIter != globalObjectMap.end(); ++omIter)
         MessageInterface::ShowMessage("   %s of type %s\n",
               (omIter->first).c_str(), ((omIter->second)->GetTypeName()).c_str());
   #endif
   
   #ifdef DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage(
         "Sandbox::Initialize() Initializing Commands...\n");
   #endif
   
   StringArray exceptions;
   IntegerArray exceptionTypes;
   UnsignedInt exceptionCount = 0;

   // Set the EventLocators
   for (omi = objectMap.begin(); omi != objectMap.end(); ++omi)
   {
      obj = omi->second;
      if (obj->IsOfType(Gmat::EVENT_LOCATOR))
      {
         #ifdef DEBUG_EVENTLOCATION
            MessageInterface::ShowMessage("Testing event locator %s...\n",
                  obj->GetName().c_str());
         #endif
         if (find(events.begin(), events.end(), (EventLocator*)obj) == events.end())
         {
            #ifdef DEBUG_EVENTLOCATION
               MessageInterface::ShowMessage("Adding event locator %s to the "
                     "event list\n", obj->GetName().c_str());
            #endif
            events.push_back((EventLocator*)obj);
         }
      }
   }
   for (omi = globalObjectMap.begin(); omi != globalObjectMap.end(); ++omi)
   {
      obj = omi->second;
      if (obj->IsOfType(Gmat::EVENT_LOCATOR))
      {
         if (find(events.begin(), events.end(), (EventLocator*)obj) == events.end())
         {
            #ifdef DEBUG_EVENTLOCATION
               MessageInterface::ShowMessage("Adding event locator %s to the "
                     "event list\n", obj->GetName().c_str());
            #endif
            events.push_back((EventLocator*)obj);
         }
      }
   }

   // Initialize commands
   while (current)
   {
      try
      {
         #ifdef DEBUG_SANDBOX_INIT
         MessageInterface::ShowMessage
            ("Initializing %s command\n   \"%s\"\n",
             current->GetTypeName().c_str(),
             current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
         #endif

         current->SetTriggerManagers(&triggerManagers);

         #ifdef DEBUG_SANDBOX_GMATFUNCTION
            MessageInterface::ShowMessage("Sandbox Initializing %s command\n",
               current->GetTypeName().c_str());
         #endif

         current->SetObjectMap(&objectMap);
         current->SetGlobalObjectMap(&globalObjectMap);
         SetGlobalRefObject(current);

         // Handle GmatFunctions
         if ((current->IsOfType("CallFunction")) ||
             (current->IsOfType("Assignment")))
         {
            #ifdef DEBUG_SANDBOX_GMATFUNCTION
               MessageInterface::ShowMessage(
                  "CallFunction or Assignment found in MCS: calling HandleGmatFunction \n");
            #endif
            HandleGmatFunction(current, &combinedObjectMap);
         }
         if (current->IsOfType("BranchCommand"))
         {
            std::vector<GmatCommand*> cmdList = ((BranchCommand*) current)->GetCommandsWithGmatFunctions();
            Integer sz = (Integer) cmdList.size();
            #ifdef DEBUG_SANDBOX_GMATFUNCTION
               MessageInterface::ShowMessage("... returning %d functions with GmatFunctions\n", sz);
            #endif
            for (Integer jj = 0; jj < sz; jj++)
            {
               HandleGmatFunction(cmdList.at(jj), &combinedObjectMap);
               (cmdList.at(jj))->SetInternalCoordSystem(internalCoordSys);
            }
         }

         try
         {
            rv = current->Initialize();
            if (!rv)
               throw SandboxException("The Mission Control Sequence command\n\n" +
                     current->GetGeneratingString(Gmat::SCRIPTING, "   ") +
                     "\n\nfailed to initialize correctly.  Please correct the error "
                     "and try again.");
         }
         catch (BaseException &)
         {
            // Call ValidateCommand to create wrappers and Initialize.(LOJ: 2010.08.24)
            // This will fix bug 1918 for the following scenario in ScriptEvent.
            // In ScriptEvent, x = 1 where x is undefined, save it.
            // Add x from the ResourceTree and run the mission.
            moderator->ValidateCommand(current);
            rv = current->Initialize();
         }

         // Check to see if the command needs a server startup
         if (current->NeedsServerStartup())
            if (moderator->StartMatlabServer() == false)
               throw SandboxException("Unable to start the server needed by the " +
                        (current->GetTypeName()) + " command");
      }
      catch (BaseException &be)
      {
         ++exceptionCount;
         exceptionTypes.push_back(be.GetMessageType());
         exceptions.push_back(be.GetFullMessage());
      }
      current = current->GetNext();
   }
   
   if (exceptionCount > 0)
   {
      for (UnsignedInt i = 0; i < exceptionCount; ++i)
      {
         // Add error count only if message type is Gmat::ERROR_ (Bug 2272 fix)
         if (exceptionTypes[i] == Gmat::ERROR_)
             MessageInterface::ShowMessage("%d: %s\n", i+1, exceptions[i].c_str());
         else
            MessageInterface::ShowMessage("%s\n", exceptions[i].c_str());
      }
      throw SandboxException("Errors were found in the mission control "
            "sequence; please correct the errors listed in the message window");
   }

   #ifdef DEBUG_SANDBOX_INIT
      MessageInterface::ShowMessage(
         "Sandbox::Initialize() Successfully initialized\n");
   #endif

   state = INITIALIZED;
   
   //MessageInterface::ShowMessage("=====> Initialize successful\n");
   return rv;
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 *  Runs the mission sequence.
 *
 *  This method walks through the command linked list, firing each GmatCommand
 *  as it is encountered by calling Execute() on the commands.  Between command
 *  executions, the method check with the Moderator to see if the user has
 *  requested that the sequence be paused or halted.
 *
 *  @return true if the mission sequence was executed, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::Execute()
{

   #if DBGLVL_SANDBOX_RUN > 1
   MessageInterface::ShowMessage("Sandbox::Execute() Here is the current object map:\n");
   for (std::map<std::string, GmatBase *>::iterator i = objectMap.begin();
        i != objectMap.end(); ++i)
      MessageInterface::ShowMessage("   (%p) %s\n", i->second, i->first.c_str());
   MessageInterface::ShowMessage("Sandbox::Execute() Here is the current global object map:\n");
   for (std::map<std::string, GmatBase *>::iterator i = globalObjectMap.begin();
        i != globalObjectMap.end(); ++i)
      MessageInterface::ShowMessage("   (%p) %s\n", i->second, i->first.c_str());

   MessageInterface::ShowMessage("Sandbox::Execute() Here is the mission sequence:\n");
   std::string seq = GmatCommandUtil::GetCommandSeqString(sequence);
   MessageInterface::ShowMessage(seq);
   #endif
   
   bool rv = true;
   Integer cloneIndex;

   state = RUNNING;
   Gmat::RunState runState = Gmat::IDLE, currentState = Gmat::RUNNING;
   
   current = sequence;
   if (!current)
      return false;

   try
   {
      while (current)
      {
         // First check to see if the run should be interrupted
         if (Interrupt())
         {
            #ifdef DEBUG_MODERATOR_CALLBACK
            MessageInterface::ShowMessage("   Interrupted in %s command\n",
                                          current->GetTypeName().c_str());
            #endif
            
            
            if (state == PAUSED)
            {
               continue;
            }
            else
            {
               //MessageInterface::ShowMessage("Sandbox::Execution interrupted.\n");
               sequence->RunComplete();
               
               // notify subscribers end of run
               currentState = Gmat::IDLE;
               publisher->SetRunState(currentState);
               publisher->NotifyEndOfRun();
               
               // Write out event data, if any
               for (UnsignedInt i = 0; i < events.size(); ++i)
                  events[i]->ReportEventData("Execution was interrupted; the "
                     "event list may be incomplete");

               throw SandboxException("Execution interrupted");
               //return rv;
            }
         }
         
         #if DBGLVL_SANDBOX_RUN
         if (current != NULL)
         {
            MessageInterface::ShowMessage
               ("Sandbox::Execution running %s\n", current->GetTypeName().c_str());
            
            #if DBGLVL_SANDBOX_RUN > 1
            MessageInterface::ShowMessage
               ("command = \n<%s>\n", current->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
            #endif
         }
         #endif
         
         if (currentState != runState)
         {
            publisher->SetRunState(currentState);
            runState = currentState;
         }
         
         if (current->AffectsClones())
         {
            // Manage owned clones
//            GmatBase *obj = current->GetUpdatedObject();

            cloneIndex = current->GetUpdatedObjectParameterIndex();

            #ifdef DEBUG_CLONE_UPDATES
               MessageInterface::ShowMessage("   Index %d retrieved from %s "
                     "command\n", cloneIndex, current->GetTypeName().c_str());
            #endif
         }
         else
            cloneIndex = -1;

         rv = current->Execute();
         
         if (!rv)
         {
            std::string str = "\"" + current->GetTypeName() +
               "\" Command failed to run to completion\n";
            
            #if DBGLVL_SANDBOX_RUN > 1
            MessageInterface::ShowMessage
               ("%sCommand Text is\n\"%s\n", str.c_str(),
                current->GetGeneratingString().c_str());
            #endif
            
            throw SandboxException(str);
         }
         
         if (current->AffectsClones())
         {
            // Manage owned clones
            GmatBase *obj = current->GetUpdatedObject();
            cloneIndex = current->GetUpdatedObjectParameterIndex();

            if (obj != NULL)
            {
               runMode retState = state;
               state = REFRESH;
               UpdateClones(obj, cloneIndex);
               state = retState;

               if (obj->IsInitialized() == false)
               {
                  #ifdef DEBUG_CLONE_UPDATES
                     bool retval =
                  #endif

                  obj->Initialize();

                  #ifdef DEBUG_CLONE_UPDATES
                     if (retval == false)
                        MessageInterface::ShowMessage("%s needs initialization"
                              ", but initialization fails\n", 
                              obj->GetName().c_str());
                  #endif
               }
            }
         }

         current = current->GetNext();
      }
   }
   catch (BaseException &e)
   {
      // Use exception to remove Visual C++ warning
      e.GetMessageType();
      sequence->RunComplete();
      state = STOPPED;
      
      #if DBGLVL_SANDBOX_RUN
         MessageInterface::ShowMessage
            ("   Sandbox rethrowing %s\n", e.GetFullMessage().c_str());
      #endif
      
      throw;
   }
   
   sequence->RunComplete();
   state = STOPPED;
   
   // notify subscribers end of run
   currentState = Gmat::IDLE;
   publisher->SetRunState(currentState);
   publisher->NotifyEndOfRun();
   
   // Write out event data, if any
   for (UnsignedInt i = 0; i < events.size(); ++i)
      events[i]->ReportEventData();

   return rv;
}



//------------------------------------------------------------------------------
// bool Interrupt()
//------------------------------------------------------------------------------
/**
 *  Tests to see if the mission sequence should be interrupted.
 *
 *  @return true if the Moderator wants to interrupt execution, false if not.
 */
//------------------------------------------------------------------------------
bool Sandbox::Interrupt()
{
   // Ask the moderator for the current RunState; only check at fixed frequency
   if (++interruptCount == pollFrequency)
   {
      Gmat::RunState interruptType =  moderator->GetUserInterrupt();
   
      switch (interruptType)
      {
         case Gmat::PAUSED:   // Pause
            state = PAUSED;
            break;
   
         case Gmat::IDLE:     // Stop puts GMAT into the Idle state
            state = STOPPED;
            break;
   
         case Gmat::RUNNING:   // MCS is running
            state = RUNNING;
            break;
   
         default:
            break;
      }
      interruptCount = 0;
   }
   
   if ((state == PAUSED) || (state == STOPPED))
      return true;

   return false;
}


//------------------------------------------------------------------------------
// void Clear()
//------------------------------------------------------------------------------
/**
 *  Cleans up the local object store.
 */
//------------------------------------------------------------------------------
void Sandbox::Clear()
{
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage("Sandbox::Clear() entered\n");
   #endif
   
   sequence  = NULL;
   current   = NULL;
   
   // Delete the all cloned objects
   std::map<std::string, GmatBase *>::iterator omi;
   
   #ifdef DEBUG_SANDBOX_OBJECT_MAPS
   MessageInterface::ShowMessage("Sandbox OMI List\n");
   for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
   {
      MessageInterface::ShowMessage("   %s", (omi->first).c_str());
      MessageInterface::ShowMessage(" of type %s\n",
         (omi->second)->GetTypeName().c_str());
   }
   MessageInterface::ShowMessage("Sandbox GOMI List\n");
   for (omi = globalObjectMap.begin(); omi != globalObjectMap.end(); omi++)
   {
      MessageInterface::ShowMessage("   %s", (omi->first).c_str());
      MessageInterface::ShowMessage(" of type %s\n",
         (omi->second)->GetTypeName().c_str());
   }
   #endif
   
   #ifdef DEBUG_SANDBOX_CLEAR
   ShowObjectMap(objectMap, "Sandbox::Clear() clearing objectMap\n");
   #endif
   
   for (omi = objectMap.begin(); omi != objectMap.end(); omi++)
   {
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("Sandbox clearing <%p>'%s'\n", omi->second,
            (omi->first).c_str());
      #endif

      // if object is a SUBSCRIBER, let's unsubscribe it first
      if ((omi->second != NULL) && (omi->second)->GetType() == Gmat::SUBSCRIBER)
         publisher->Unsubscribe((Subscriber*)(omi->second));
      
      // Unsubscribe owned Subscribers too
      Integer count = (omi->second)->GetOwnedObjectCount();
      for (Integer i = 0; i < count; ++i)
      {
         if ((omi->second)->GetOwnedObject(i)->IsOfType(Gmat::SUBSCRIBER))
            publisher->Unsubscribe((Subscriber*)
                  ((omi->second)->GetOwnedObject(i)));
      }

      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("   Deleting <%p>'%s'\n", omi->second,
            (omi->second)->GetName().c_str());
      #endif
      #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (omi->second, omi->first, "Sandbox::Clear()",
             " deleting cloned obj from objectMap");
      #endif
      delete omi->second;
      omi->second = NULL;
      //objectMap.erase(omi);
   }
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage
      ("--- Sandbox::Clear() deleting objects from objectMap done\n");
   ShowObjectMap(globalObjectMap, "Sandbox::Clear() clearing globalObjectMap\n");
   #endif
   for (omi = globalObjectMap.begin(); omi != globalObjectMap.end(); omi++)
   {
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("Sandbox clearing <%p>%s\n", omi->second,
            (omi->first).c_str());
      #endif

      // if object is a SUBSCRIBER, let's unsubscribe it first
      if ((omi->second != NULL) && (omi->second)->GetType() == Gmat::SUBSCRIBER)
         publisher->Unsubscribe((Subscriber*)(omi->second));
      
      // Unsubscribe owned Subscribers too
      Integer count = (omi->second)->GetOwnedObjectCount();
      for (Integer i = 0; i < count; ++i)
      {
         if ((omi->second)->GetOwnedObject(i)->IsOfType(Gmat::SUBSCRIBER))
            publisher->Unsubscribe((Subscriber*)
                  ((omi->second)->GetOwnedObject(i)));
      }

      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
         MessageInterface::ShowMessage("   Deleting <%p>'%s'\n", omi->second,
            (omi->second)->GetName().c_str());
      #endif
      #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove
            (omi->second, omi->first, "Sandbox::Clear()",
             " deleting cloned obj from globalObjectMap");
      #endif
      delete omi->second;
      omi->second = NULL;
      //globalObjectMap.erase(omi);
   }
   
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage
      ("--- Sandbox::Clear() deleting objects from globalObjectMap done\n");
   #endif
   
   // Clear published data
   if (publisher)
   {
      publisher->ClearPublishedData();
      #ifdef DEBUG_SANDBOX_CLEAR
      MessageInterface::ShowMessage
         ("--- Sandbox::Clear() publisher cleared published data\n");
      #endif
   }
   
   // Set publisher to NULL. The publisher is set before the run and this method
   // Sandbox::Clear() can be called multiple times from the Moderator
   publisher = NULL;
   
#ifndef DISABLE_SOLAR_SYSTEM_CLONING
   if (solarSys != NULL)
   {
      #ifdef DEBUG_SS_CLONING
      MessageInterface::ShowMessage
         ("--- Sandbox::Clear() deleting the solar system clone: %p\n", solarSys);
      #endif
      
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (solarSys, solarSys->GetName(), "Sandbox::Clear()", " deleting solarSys");
      #endif
      delete solarSys;
   }
   
   solarSys = NULL;
#endif
   
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage
      ("--- Sandbox::Clear() now about to delete triggerManagers\n");
   #endif
   // Remove the TriggerManager clones
   for (UnsignedInt i = 0; i < triggerManagers.size(); ++i)
      delete triggerManagers[i];
   triggerManagers.clear();
   
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage
      ("--- Sandbox::Clear() triggerManagers are cleared\n");
   #endif
   
   // Don't delete objects.  ConfigManager::RemoveAllItems() deletes them
   objectMap.clear();
   globalObjectMap.clear();
   
   #ifdef DEBUG_TRANSIENT_FORCES
   MessageInterface::ShowMessage
      ("Sandbox::Clear() transientForces<%p> has %d transient forces\n",
       &transientForces, transientForces.size());
   #endif

   // BeginFiniteBurn pushes burn forces onto the transient force vector.
   // We don't delete transient forces here; the burn forces are deleted in
   // the BeginFiniteBurn destructor.
   #if 0
   for (std::vector<PhysicalModel*>::iterator tf = transientForces.begin();
        tf != transientForces.end(); ++tf)
   {
      #ifdef DEBUG_TRANSIENT_FORCES
         MessageInterface::ShowMessage("   tf=<%p>\n", (*tf));
      #endif
      
      #ifdef DEBUG_MEMORY
         MemoryTracker::Instance()->Remove((*tf), (*tf)->GetName(),
               "Sandbox::Clear()", "deleting transient force");
      #endif
      delete (*tf);
   }
   #endif
   
   transientForces.clear();
   
   events.clear();

   // Update the sandbox state
   if ((state != STOPPED) && (state != IDLE))
          MessageInterface::ShowMessage(
             "Unexpected state transition in the Sandbox\n");

   state     = IDLE;
   
   #ifdef DEBUG_SANDBOX_CLEAR
   MessageInterface::ShowMessage("Sandbox::Clear() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// bool AddSubscriber(Subscriber *sub)
//------------------------------------------------------------------------------
/**
 *  Add Subcribers to the Sandbox and registers them with the Publisher.
 *
 *  @param sub The subscriber.
 *
 *  @return true if the Subscriber was registered.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddSubscriber(Subscriber *sub)
{
   // add subscriber to sandbox by AddObject() so that cloned subscribers
   // can be deleted when clear (loj: 2008.11.06)
   Subscriber *newSub = (Subscriber*)AddObject(sub);
   if (newSub != NULL)
   {
      publisher->Subscribe(newSub);
      return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool AddOwnedSubscriber(Subscriber *sub)
//------------------------------------------------------------------------------
/**
 *  Registers owned Subcribers with the Publisher.
 *
 *  @param <sub> The subscriber.
 *
 *  @return true if the Subscriber was registered.
 */
//------------------------------------------------------------------------------
bool Sandbox::AddOwnedSubscriber(Subscriber *sub)
{
   publisher->Subscribe(sub);
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* Sandbox::FindObject(const std::string &name)
//------------------------------------------------------------------------------
/**
 *  Finds an object by name, searching through the SandboxObjectMap first,
 *  then the GlobalObjectMap
 *
 *  @param spname The name of the SpacePoint.
 *
 *  @return A pointer to the SpacePoint, or NULL if it does not exist in the
 *          Sandbox.
 */
//------------------------------------------------------------------------------
GmatBase* Sandbox::FindObject(const std::string &name)
{
   if (objectMap.find(name) == objectMap.end())
   {
     // If not found in the LOS, check the Global Object Store (GOS)
      if (globalObjectMap.find(name) == globalObjectMap.end())
         return NULL;
      else return globalObjectMap[name];
   }
   else
      return objectMap[name];
}


//------------------------------------------------------------------------------
// bool Sandbox::SetObjectByNameInMap(const std::string &name,
//                                    GmatBase *obj)
//------------------------------------------------------------------------------
/**
 *  Sets the object pointer for the given name in the object map(s).  NOTE that
 *  an object should only exist in one of the object maps, so both IFs should
 *  not evaluate to TRUE.
 *
 *  @param <name> The name of the object.
 *  @param <obj>  The object pointer.
 *
 *  @return true if successful; flase otherwise
 */
//------------------------------------------------------------------------------
bool Sandbox::SetObjectByNameInMap(const std::string &name,
                                   GmatBase *obj)
{
   #ifdef DEBUG_SANDBOX_OBJECT_MAPS
   MessageInterface::ShowMessage
      ("Sandbox::SetObjectByNameInMap() name = %s\n",
       name.c_str());
   #endif
   bool found = false;
   // if it's already in a map, set the object pointer for the name
   if (objectMap.find(name) != objectMap.end())
   {
      objectMap[name] = obj;
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
      MessageInterface::ShowMessage
         ("Sandbox::SetObjectByNameInMap() set object name = %s in objectMap\n",
          name.c_str());
      #endif
      found = true;
   }
   if (globalObjectMap.find(name) != globalObjectMap.end())
   {
      globalObjectMap[name] = obj;
      #ifdef DEBUG_SANDBOX_OBJECT_MAPS
      MessageInterface::ShowMessage
         ("Sandbox::SetObjectByNameInMap() set object name = %s in globalObjectMap\n",
          name.c_str());
      #endif      
      found = true;
   }
   // if not already in the map, add it to the objectMap
   // (globals are added to the globalObjectMap later)
   if (!found)
      objectMap.insert(std::make_pair(name,obj));
   
   #ifdef DEBUG_SANDBOX_OBJECT_MAPS
   MessageInterface::ShowMessage
      ("Sandbox::SetObjectByNameInMap() returning found = %s\n",
       (found? "TRUE" : "FALSE"));
   #endif   
   return found;
}

//------------------------------------------------------------------------------
// bool Sandbox::HandleGmatFunction(GmatCommand *cmd,
//                                  std::map<std::string, GmatBase *> *usingMap)
//------------------------------------------------------------------------------
/**
 *  Handles any GmatFunctions included in the sequence.  The input cmd is the 
 *  CallFunction or Assignment command to process - it may itself contain a nested
 *  GmatFunction.  If it does, this method willbe called recursively to process
 *  the nested GmatFunctions.
 *
 *  @param <name>     The cmd.
 *  @param <usingMap> The map to send to the Interpreter (via the Moderator).
 *
 *  @return true if successful; flase otherwise
 */
//------------------------------------------------------------------------------
bool Sandbox::HandleGmatFunction(GmatCommand *cmd, std::map<std::string,
                                 GmatBase *> *usingMap)
{
   #ifdef DEBUG_SANDBOX_GMATFUNCTION
      MessageInterface::ShowMessage(
         "Now entering HandleGmatFunction with command of type %s, '%s'\n",
         (cmd->GetTypeName()).c_str(), cmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
   #endif
      
   bool OK = false;
   GmatGlobal *global = GmatGlobal::Instance();
   std::string matlabExt = global->GetMatlabFuncNameExt();
   StringArray gfList;
   bool        isMatlabFunction = false;
   
   SetGlobalRefObject(cmd);
   
   if (cmd->IsOfType("CallFunction"))
   {
      std::string cfName = cmd->GetStringParameter("FunctionName");
      gfList.push_back(cfName);
   }
   else if (cmd->IsOfType("Assignment"))  
   {
      gfList = ((Assignment*) cmd)->GetGmatFunctionNames();
   }
   
   #ifdef DEBUG_SANDBOX_GMATFUNCTION
   MessageInterface::ShowMessage("   Has %d GmatFunctions\n", gfList.size());
   #endif
   
   for (unsigned int ii = 0; ii < gfList.size(); ii++)
   {
      std::string fName = gfList.at(ii);
      Function *f;
      isMatlabFunction = false;
      // if it's a Matlab Function, remove the extension from the name before looking in the GOS
      // (as Matlab function names are placed into the GOS without the extension)
      if (fName.find(matlabExt) != fName.npos)
      {
         fName = fName.substr(0, fName.find(matlabExt));
         #ifdef DEBUG_SANDBOX_GMATFUNCTION
         MessageInterface::ShowMessage
            ("   actual matlab function name='%s'\n", fName.c_str());
         #endif
         isMatlabFunction = true;
      }
      #ifdef DEBUG_SANDBOX_GMATFUNCTION
         MessageInterface::ShowMessage("Now searching GOS for object %s\n",
            (gfList.at(ii)).c_str());
      #endif
      // if there is not already a function of that name, create it
      if (globalObjectMap.find(fName) == globalObjectMap.end())
      {
         if (isMatlabFunction)
            f = moderator->CreateFunction("MatlabFunction",fName, 0);
         else
            f = moderator->CreateFunction("GmatFunction",fName, 0);
         if (!f) 
            throw SandboxException("Sandbox::HandleGmatFunction - error creating new function\n");
         #ifdef DEBUG_SANDBOX_GMATFUNCTION
         MessageInterface::ShowMessage
            ("Adding function <%p>'%s' to the Global Object Store\n", f, fName.c_str());
         #endif
         globalObjectMap.insert(std::make_pair(fName,f));
      }
      else // it's already in the GOS, so just grab it
         f = (Function*) globalObjectMap[fName];
      
      if (cmd->IsOfType("CallFunction"))
      {
         ((CallFunction*)cmd)->SetRefObject(f,Gmat::FUNCTION,fName);
         cmd->SetStringParameter("FunctionName", fName);
      }
      else if (cmd->IsOfType("Assignment"))
         ((Assignment*) cmd)->SetFunction(f);
      
      #ifdef DEBUG_SANDBOX_GMATFUNCTION
      MessageInterface::ShowMessage(
         "Now handling function \"%s\", whose fcs is %s set, ",
         (f->GetStringParameter("FunctionName")).c_str(), 
         ((f->IsFunctionControlSequenceSet())? "already" : "NOT"));
      MessageInterface::ShowMessage
         ("script errors were %sfound.\n", f->ScriptErrorFound() ? "" : "not ");
      #endif
      
      // if function is GmatFunction and no FCS has built and no script error found,
      // build FCS
      if ((f->GetTypeName() == "GmatFunction") &&
          (!(f->IsFunctionControlSequenceSet())) &&
          (!(f->ScriptErrorFound())))
      {
         #ifdef DEBUG_SANDBOX_GMATFUNCTION
         MessageInterface::ShowMessage(
            "About to call InterpretGmatFunction for function %s\n",
            (f->GetStringParameter("FunctionName")).c_str());
         #endif
         GmatCommand* fcs = moderator->InterpretGmatFunction(f, usingMap, solarSys);

         // If FCS not created, throw an exception with Gmat::GENERAL_ so that it will not
         // write error count again for function in Initialize()(Bug 2272 fix)
         if (fcs == NULL)
            throw SandboxException("Sandbox::HandleGmatFunction - error creating FCS\n",
                                   Gmat::GENERAL_);
         
         f->SetFunctionControlSequence(fcs);
         GmatCommand* fcsCmd = fcs;
         while (fcsCmd)
         {
            #ifdef DISALLOW_NESTED_GMAT_FUNCTIONS
            if (fcsCmd->HasAFunction())
            {
               std::string errMsg = "Sandbox::HandleGmatFunction (";
               errMsg += fName + ") - nested or recursive GmatFunctions not yet supported.\n";
               throw SandboxException(errMsg);
            }
            #endif
            
            // todo: Figure out what makes sense for the "events" container and functions

            if ((fcsCmd->IsOfType("CallFunction")) ||
                (fcsCmd->IsOfType("Assignment")))
            {
               #ifdef DEBUG_SANDBOX_GMATFUNCTION
               MessageInterface::ShowMessage(
                  "CallFunction or Assignment (%s)'%s' detected in FCS... now processing\n",
                  (fcsCmd->GetTypeName()).c_str(),
                  fcsCmd->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
               #endif
               
               //Let's handle GmatFunction first
               //compiler warning: '+=' : unsafe use of type 'bool' in operation
               //OK += HandleGmatFunction(fcsCmd, &combinedObjectMap);
               OK = HandleGmatFunction(fcsCmd, &combinedObjectMap) && OK;
               // do not set the non-global object map here; it will need to be
               // setup by the FunctionManager at execution
               fcsCmd->SetGlobalObjectMap(&globalObjectMap);
            }
            if (fcsCmd->IsOfType("BranchCommand"))
            {
               std::vector<GmatCommand*> cmdList =
                  ((BranchCommand*) fcsCmd)->GetCommandsWithGmatFunctions();
               Integer sz = (Integer) cmdList.size();
               for (Integer jj = 0; jj < sz; jj++)
               {
                  GmatCommand *childCmd = cmdList.at(jj);
                  HandleGmatFunction(childCmd, &combinedObjectMap);
               }
            }
            fcsCmd = fcsCmd->GetNext();
         }
      }
   }
   return OK;
}


//------------------------------------------------------------------------------
// void SetGlobalRefObject(GmatCommand *cmd)
//------------------------------------------------------------------------------
/*
 * Sets globally used object pointers to command
 *
 * @param cmd The command to set global object pointers to
 */
//------------------------------------------------------------------------------
void Sandbox::SetGlobalRefObject(GmatCommand *cmd)
{
   #ifdef DEBUG_SANDBOX_GLOBAL_REF_OBJ
   MessageInterface::ShowMessage
      ("Sandbox::SetGlobalRefObject() Setting solarSystem <%p>, transientForces <%p>\n   "
       "internalCoordSystem <%p>, publisher <%p>, to <%p>'%s'\n", solarSys,
       &transientForces, internalCoordSys, publisher, cmd, cmd->GetTypeName().c_str());
   #endif
   cmd->SetSolarSystem(solarSys);
   cmd->SetTransientForces(&transientForces);
   cmd->SetEventLocators(&events);
   cmd->SetInternalCoordSystem(internalCoordSys);
   cmd->SetPublisher(publisher);
}


//------------------------------------------------------------------------------
// void ShowObjectMap(ObjectMap &om, const std::string &title)
//------------------------------------------------------------------------------
void Sandbox::ShowObjectMap(ObjectMap &om, const std::string &title)
{   
   MessageInterface::ShowMessage(title);
   MessageInterface::ShowMessage("object map = <%p>\n", &om);
   if (om.size() > 0)
   {
      for (ObjectMap::iterator i = om.begin(); i != om.end(); ++i)
      MessageInterface::ShowMessage
         ("   %30s  <%p><%s>\n", i->first.c_str(), i->second,
          i->second == NULL ? "NULL" : (i->second)->GetTypeName().c_str());
   }
   else
   {
      MessageInterface::ShowMessage("The object map is empty\n");
   }
}


//------------------------------------------------------------------------------
// void UpdateClones(GmatBase *obj, Integer updatedParameterIndex)
//------------------------------------------------------------------------------
/**
 * Entry method for updates to owned clone objects
 *
 * @param obj The updates object that needs to pass parameters to owned clones,
 *            if there are any
 * @param updatedParameterIndex The index of the parameter that was updated
 */
//------------------------------------------------------------------------------
void Sandbox::UpdateClones(GmatBase *obj, Integer updatedParameterIndex)
{
   switch (cloneUpdateStyle)
   {
      case PASS_TO_ALL:
         PassToAll(obj, updatedParameterIndex);
         break;

      case PASS_TO_REGISTERED:
         PassToRegisteredClones(obj, updatedParameterIndex);
         break;

      case SKIP_UPDATES:
         #ifdef REPORT_CLONE_UPDATE_STATUS
            MessageInterface::ShowMessage("Clone update requested for %s; "
                  "skipping because the update method is set to "
                  "SKIP_UPDATES.\n", obj->GetName().c_str());
         #endif
         break;

      default:
         throw SandboxException("The requested clone update method is not "
               "recognized");
         break;
   }
}


//------------------------------------------------------------------------------
// void PassToAll(GmatBase *obj, Integer updatedParameterIndex)
//------------------------------------------------------------------------------
/**
 * Brute force method for updating owned clones
 *
 * @param obj The updates object that needs to pass parameters to owned clones,
 *            if there are any
 * @param updatedParameterIndex The index of the parameter that was updated
 */
//------------------------------------------------------------------------------
void Sandbox::PassToAll(GmatBase *obj, Integer updatedParameterIndex)
{
   #ifdef REPORT_CLONE_UPDATE_STATUS
      MessageInterface::ShowMessage("Clone update requested for %s index %d; "
            "the update method is set to PASS_TO_ALL.\n, updatedParameterIndex",
            obj->GetName().c_str());
   #endif

   // Walk the global object store
   GmatBase *listObj;
   std::map<std::string, GmatBase *>::iterator current=globalObjectMap.begin();

   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("PassToAll updates: Walking the global "
            "object store\n");
   #endif

   while (current != globalObjectMap.end())
   {
      listObj = current->second;

      #ifdef DEBUG_CLONE_UPDATES
         MessageInterface::ShowMessage("   <%p> %s %s\n", listObj,
               listObj->GetName().c_str(), (listObj->HasLocalClones() ?
               "has local clones" : "has no local clones"));
      #endif

      if (listObj->HasLocalClones())
         UpdateAndInitializeCloneOwner(obj, listObj, updatedParameterIndex);

      ++current;
   }

   // Walk the local object store
   current = objectMap.begin();

   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("PassToAll updates: Walking the local "
            "object store\n");
   #endif

   while (current != objectMap.end())
   {
      listObj = current->second;

      #ifdef DEBUG_CLONE_UPDATES
         MessageInterface::ShowMessage("   <%p> %s %s\n", listObj,
               listObj->GetName().c_str(), (listObj->HasLocalClones() ?
               "has local clones" : "has no local clones"));
      #endif

      if (listObj->HasLocalClones())
         UpdateAndInitializeCloneOwner(obj, listObj, updatedParameterIndex);

      ++current;
   }


   // Walk the command list
   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("PassToAll updates: Walking the "
            "command list\n");
   #endif
   GmatCommand *currentCmd = sequence;
   while (currentCmd != NULL)
   {
      #ifdef DEBUG_CLONE_UPDATES
         MessageInterface::ShowMessage("   %s %s\n",
               currentCmd->GetTypeName().c_str(),
               (currentCmd->HasLocalClones() ?
               "has local clones" : "has no local clones"));
      #endif
      if (currentCmd->HasLocalClones())
         UpdateAndInitializeCloneOwner(obj, currentCmd, updatedParameterIndex);

      if (currentCmd->IsOfType("BranchCommand"))
      {
         PassToBranchCommand(obj, (BranchCommand*)currentCmd,
               updatedParameterIndex);
         currentCmd = ((BranchCommand*)currentCmd)->GetNextWhileExecuting();
      }
      else
         currentCmd = currentCmd->GetNext();
   }
}


//------------------------------------------------------------------------------
// void PassToRegisteredClones(GmatBase *obj, Integer updatedParameterIndex)
//------------------------------------------------------------------------------
/**
 * Owned clone update method that only updates registered clones
 *
 * @param obj The updated object that needs to pass parameters to owned clones,
 *            if there are any
 * @param updatedParameterIndex The index of the parameter that was updated
 */
//------------------------------------------------------------------------------
void Sandbox::PassToRegisteredClones(GmatBase *obj,
      Integer updatedParameterIndex)
{
   #ifdef REPORT_CLONE_UPDATE_STATUS
      MessageInterface::ShowMessage("Clone update requested for %s; "
            "the update method is set to PASS_TO_REGISTERED.\n",
            obj->GetName().c_str());
   #endif

}


//------------------------------------------------------------------------------
// void PassToBranchCommand(GmatBase *theClone, GmatCommand* theBranchCommand,
//       Integer updatedParameterIndex)
//------------------------------------------------------------------------------
/**
 * Passes potential owned clones into branch command branches.
 *
 * This method calls into branches of branches recursively.
 *
 * @param theClone The object that may be an owned clone
 * @param theBranchCommand The branch command that that manages the branch
 *                         control sequence
 * @param updatedParameterIndex The index of the parameter that was updated
 */
//------------------------------------------------------------------------------
void Sandbox::PassToBranchCommand(GmatBase *theClone,
      BranchCommand* theBranchCommand, Integer updatedParameterIndex)
{
   #ifdef DEBUG_CLONE_UPDATES
      MessageInterface::ShowMessage("Clone management for a %s branch "
            "command\n", theBranchCommand->GetTypeName().c_str());
   #endif

   // Loop through the branch command, updating each branch member
   Integer branch = 0;
   GmatCommand *cmd = NULL;
   do
   {
      cmd = theBranchCommand->GetChildCommand(branch);
      while ((cmd != NULL) && (cmd != theBranchCommand))
      {
         #ifdef DEBUG_CLONE_UPDATES
            MessageInterface::ShowMessage("   Branch[%d] member: a %s "
                  "command\n", branch, cmd->GetTypeName().c_str());
         #endif
         if (cmd->HasLocalClones())
            UpdateAndInitializeCloneOwner(theClone, cmd, updatedParameterIndex);

         if ((cmd->IsOfType("BranchCommand")) && (cmd != theBranchCommand))
         {
            #ifdef DEBUG_CLONE_UPDATES
               MessageInterface::ShowMessage("Nesting PassToBranchCommands; "
                     "passing %p from %p\n", cmd, this);
            #endif
            PassToBranchCommand(theClone, (BranchCommand*)cmd,
                  updatedParameterIndex);
         }

         if (cmd->IsOfType("BranchCommand"))
            cmd = ((BranchCommand*)cmd)->GetNextWhileExecuting();
         else
            cmd = cmd->GetNext();
      }
      ++branch;
   } while (cmd != NULL);
}


//------------------------------------------------------------------------------
// void UpdateAndInitializeCloneOwner(GmatBase *theClone, GmatBase* theOwner)
//------------------------------------------------------------------------------
/**
 * Gives an owned clone owner the opportunity to update its clone
 *
 * @param theClone The object that may affect an owned clone
 * @param theOwner The object that might own the clone
 * @param updatedParameterIndex The index of the parameter that was updated
 */
//------------------------------------------------------------------------------
void Sandbox::UpdateAndInitializeCloneOwner(GmatBase *theClone,
         GmatBase* theOwner, Integer updatedParameterIndex)
{
   bool incomingInitState = theOwner->IsInitialized();

   if (updatedParameterIndex >= 0)
   {
      #ifdef DEBUG_CLONE_UPDATES
         MessageInterface::ShowMessage("In Sandbox::UpdateAndInitializeClone"
               "Owner(%s, %s, %d), %s may not need reinitialization\n",
               theClone->GetName().c_str(), theOwner->GetName().c_str(),
               updatedParameterIndex, theOwner->GetName().c_str());
      #endif

      theOwner->UpdateClonedObjectParameter(theClone, updatedParameterIndex);
   }
   else
   {
      theOwner->UpdateClonedObject(theClone);

      if ((theOwner->IsInitialized() == false) && incomingInitState)
      {
         #ifdef DEBUG_CLONE_UPDATES
            MessageInterface::ShowMessage("In Sandbox::UpdateAndInitializeClone"
                  "Owner(%s, %s, %d), %s must be reinitialized\n",
                  theClone->GetName().c_str(), theOwner->GetName().c_str(),
                  updatedParameterIndex, theOwner->GetName().c_str());
         #endif

         ObjectInitializer *objInit = new ObjectInitializer(solarSys, &objectMap, &globalObjectMap,
                  internalCoordSys, true);
         objInit->BuildReferences(theOwner);
         objInit->BuildAssociations(theOwner);
         delete objInit;

         #ifdef DEBUG_CLONE_UPDATES
            bool retval =
         #endif

         theOwner->Initialize();

         #ifdef DEBUG_CLONE_UPDATES
            if (retval == false)
               MessageInterface::ShowMessage("%s needs initialization, but "
                     "initialization fails\n", theOwner->GetName().c_str());
         #endif
      }
   }
}
