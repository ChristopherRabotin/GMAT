//$Id: RunSimulator.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             RunSimulator
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
// Created: 2009/06/21
//
/**
 * Mission Control Sequence Command that runs data simulation
 */
//------------------------------------------------------------------------------


#include "RunSimulator.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_INITIALIZATION
//#define DEBUG_SIMULATOR_EXECUTION
//#define DEBUG_EVENT_LOCATION
//#define DEBUG_STATE_BUFFERING
//#define DEBUG_EVENT_STATE


//------------------------------------------------------------------------------
// RunSimulator()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
RunSimulator::RunSimulator() :
   RunSolver               ("RunSimulator"),
   theSimulator            (NULL),
   commandRunning          (false),
   commandComplete         (false),
   bufferFilled            (false),
   currentEvent            (NULL),
   eventProcessComplete    (false),
   eventMan                (NULL),
   includeSTMPropagation   (false)
{
   overridePropInit = true;
}


//------------------------------------------------------------------------------
// ~RunSimulator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RunSimulator::~RunSimulator()
{
   if (theSimulator)
      delete theSimulator;
}


//------------------------------------------------------------------------------
// RunSimulator(const RunSimulator & rs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rs The command that is copied into the new one
 */
//------------------------------------------------------------------------------
RunSimulator::RunSimulator(const RunSimulator & rs) :
   RunSolver               (rs),
   theSimulator            (NULL),
   commandRunning          (false),
   commandComplete         (false),
   bufferFilled            (false),
   currentEvent            (NULL),
   eventProcessComplete    (false),
   eventMan                (NULL),
   includeSTMPropagation   (rs.includeSTMPropagation)
{
   overridePropInit = true;
}

//------------------------------------------------------------------------------
// RunSimulator& operator=(const RunSimulator & rs)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rs The RunSimulator object that supplies properties this one needs
 *
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
RunSimulator & RunSimulator::operator=(const RunSimulator & rs)
{
   if (&rs != this)
   {
      theSimulator    = NULL;
      commandRunning  = false;
      commandComplete = false;
      currentEvent    = NULL;
      eventProcessComplete = false;
      eventMan        = NULL;
      bufferFilled    = false;
      overridePropInit = true;
      includeSTMPropagation = rs.includeSTMPropagation;

   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Cleates a duplicate of a RunSimulator object
 *
 * @return a clone of the object
 */
//------------------------------------------------------------------------------
GmatBase* RunSimulator::Clone() const
{
   return new RunSimulator(*this);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Accesses names for referenced objects.
 *
 * @param type Type of object requested.
 *
 * @return the referenced object's name.
 */
//------------------------------------------------------------------------------
std::string RunSimulator::GetRefObjectName(const Gmat::ObjectType type) const
{
   switch (type)
   {
      case Gmat::SOLVER:
         #ifdef DEBUG_RUN_SIMULATOR
            MessageInterface::ShowMessage
               ("Getting RunSimulator reference solver name\n");
         #endif
         return solverName;

      default:
         ;
   }

   return RunSolver::GetRefObjectName(type);
}



//------------------------------------------------------------------------------
// bool SetRefObjectName(const Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets names for referenced objects.
 *
 * @param type Type of the object.
 * @param name Name of the object.
 *
 * @return true if the name was set, false if not.
 */
//------------------------------------------------------------------------------
bool RunSimulator::SetRefObjectName(const Gmat::ObjectType type,
                                     const std::string &name)
{
   if (type == Gmat::SOLVER)
   {
      solverName = name;
      return true;
   }

   return RunSolver::SetRefObjectName(type, name);
}



//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool RunSimulator::RenameRefObject(const Gmat::ObjectType type,
      const std::string &oldName, const std::string &newName)
{
   // RunSimulator needs to know about Solver name only
   if (type != Gmat::SOLVER)
      return RunSolver::RenameRefObject(type, oldName, newName);

   if (solverName == oldName)
   {
      solverName = newName;
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
//  const std::string GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param mode     Specifies the type of serialization requested.
 * @param prefix   Optional prefix appended to the object's name. (Used for
 *                 indentation)
 * @param useName  Name that replaces the object's name (Not yet used
 *                 in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& RunSimulator::GetGeneratingString(Gmat::WriteMode mode,
                                                    const std::string &prefix,
                                                    const std::string &useName)
{
   generatingString = prefix + "RunSimulator " + solverName + ";";

   return RunSolver::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the command for execution
 *
 * This method prepares the simulator and associated measurement manager and
 * measurements for the simulation process.  Referenced objects are cloned or
 * set as needed in this method.
 *
 * The propagation subsystem is prepared in the base class components of the
 * command.  RunSimulator generates the PropSetup clones at this level, but
 * leaves the rest of the initialization process for the PropSetups in the base
 * class method, which is called from this method.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RunSimulator::Initialize()
{
   bool retval = false;

   // First set the simulator object
   if (solverName == "")
      throw CommandException("Cannot initialize RunSimulator command -- the "
            "simulator name is not specified.");

   // Clear the old clone if it was set
   if (theSimulator != NULL)
      delete theSimulator;

   GmatBase *simObj = FindObject(solverName);
   if (simObj == NULL)
      throw CommandException("Cannot initialize RunSimulator command -- the "
            "simulator named " + solverName + " cannot be found.");

   if (!simObj->IsOfType("Simulator"))
      throw CommandException("Cannot initialize RunSimulator command -- the "
            "object named " + solverName + " is not a simulator.");

   theSimulator = (Simulator*)(simObj->Clone());

   // Set the streams for the measurement manager
   MeasurementManager *measman = theSimulator->GetMeasurementManager();
   StringArray streamList = measman->GetStreamList();
   for (UnsignedInt ms = 0; ms < streamList.size(); ++ms)
   {
      GmatBase *obj = FindObject(streamList[ms]);
      if (obj != NULL)
      {
         if (obj->IsOfType(Gmat::DATASTREAM))
         {
            DataFile *df = (DataFile*)obj;
            measman->SetStreamObject(df);
         }
      }
      else
         throw CommandException("Did not find the object named " +
               streamList[ms]);
   }

   // Find the event manager and store its pointer
   if (triggerManagers == NULL)
      throw CommandException("The Event Manager pointer was not set on the "
            "RunSimulator command");

   for (UnsignedInt i = 0; i < triggerManagers->size(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("RunSimulator has an TriggerManager of "
               "type %s, id %d\n",
               (*triggerManagers)[i]->GetTriggerTypeString().c_str(),
               (*triggerManagers)[i]->GetTriggerType());
      #endif
      if ((*triggerManagers)[i]->GetTriggerType() == Gmat::EVENT)
      {
         eventMan = (EventManager*)(*triggerManagers)[i];
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("RunSimulator has an EventManager of "
                  "type %s\n", eventMan->GetTriggerTypeString().c_str());
         #endif
      }
   }
   if (eventMan == NULL)
      throw CommandException("The EventManager pointer was not set on the "
            "RunSimulator command");

   // Next comes the propagator
   PropSetup *obj = theSimulator->GetPropagator();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Propagator at address %p ", obj);
      if (obj != NULL)
         MessageInterface::ShowMessage("is named %s\n",
               obj->GetName().c_str());
      else
         MessageInterface::ShowMessage("is not yet set\n");
   #endif

   if (obj != NULL)
   {
      if (obj->IsOfType(Gmat::PROP_SETUP))
      {
         PropSetup *ps = (PropSetup*)obj->Clone();

         // RunSimulator only manages one PropSetup.  If that changes, so
         // does this code
         if (propagators.size() > 0)
         {
            for (std::vector<PropSetup*>::iterator pp = propagators.begin();
                  pp != propagators.end(); ++pp)
            {
               delete (*pp);
            }
            propagators.clear();
            p.clear();
            fm.clear();
         }
         propagators.push_back(ps);
         p.push_back(ps->GetPropagator());
         fm.push_back(ps->GetODEModel());
         eventMan->SetObject(ps);  // todo <-- Check this -- added 6/28

//         PropagationStateManager *psm = ps->GetPropStateManager();
//         StringArray propObjects = ps->GetStringArrayParameter("");

         retval = true;
      }
   }
   else
      throw CommandException("Cannot initialize RunSimulator command; the "
            "propagator pointer in the Simulator " +
            theSimulator->GetName() + " is NULL.");

   // Now set the participant list
   MeasurementManager *mm = theSimulator->GetMeasurementManager();
   StringArray participants = mm->GetParticipantList();

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunSimulator command found %d "
            "participants\n", participants.size());
   #endif

   propObjectNames.clear();
   propObjectNames.push_back(participants);

   // Now we can initialize the propagation subsystem by calling up the
   // inheritance tree.
   if (retval)
      retval = RunSolver::Initialize();

   #ifdef DEBUG_INITIALIZATION
      if (retval == false)
         MessageInterface::ShowMessage("RunSimulator command failed to "
               "initialize; RunSolver::Initialize() call failed.\n");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// void SetPropagationProperties(PropagationStateManager *psm)
//------------------------------------------------------------------------------
/**
 * An interface used to set specific additional propagation properties as
 * needed.
 *
 * @param psm A PropagationStateManager that controls the propagation state
 *            vector
 */
//------------------------------------------------------------------------------
void RunSimulator::SetPropagationProperties(PropagationStateManager *psm)
{
   ObjectArray pObjects;
   psm->GetStateObjects(pObjects, Gmat::UNKNOWN_OBJECT);

   for (ObjectArray::iterator p = pObjects.begin(); p != pObjects.end(); ++p)
   {
      if ((*p)->IsOfType(Gmat::SPACEOBJECT))
      {
         if (includeSTMPropagation)
            psm->SetProperty("STM", *p);
      }
   }
}


//------------------------------------------------------------------------------
// bool Execute()
//------------------------------------------------------------------------------
/**
 * Performs the command side processing for the Simulation
 *
 * This method calls the Simulator to determine the state of the Simulation
 * state machine and responds to that state as needed.  Typical command side
 * responses are to propagate as needed, to clean up memory, or to reset flags
 * based on the state machine.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RunSimulator::Execute()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("\n\nThe \"%s\" command is running...\n",
            GetTypeName().c_str());
   #endif

   // Reset the command if called after it has completed execution
   // todo: Debug this piece; reentrance in a For loop doesn't work yet
//   if (commandComplete)
//      TakeAction("Reset");


   // Here we should check to see if the command is currently propagating and
   // finish that first...

   // Respond to the state in the state machine
   Solver::SolverState state = theSimulator->GetState();

   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("\nSimulator state is %d\n", state);
   #endif
   switch (state)
   {
      case Solver::INITIALIZING:
         PrepareToSimulate();
         break;

      case Solver::PROPAGATING:
         Propagate();
         break;

      case Solver::CALCULATING:
         Calculate();
         break;

      case Solver::LOCATING:
         LocateEvent();
         break;

      case Solver::SIMULATING:
         Simulate();
         break;

      case Solver::FINISHED:
         Finalize();
         break;

      default:
         throw CommandException("Unknown state "
               " encountered in the RunSimulator command");
   }

   state = theSimulator->AdvanceState();

   return true;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Completes processing so that subsequent commands can be run.
 */
//------------------------------------------------------------------------------
void RunSimulator::RunComplete()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Entered RunSimulator::RunComplete()\n");
   #endif
   commandRunning = false;

   RunSolver::RunComplete();
}


//------------------------------------------------------------------------------
// bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * Performs actions at prompting from higher level structures
 *
 * @param action The action that needs to be taken
 * @param actionData Optional additional data the action needs
 *
 * @return true if an action was taken, false if not
 */
//------------------------------------------------------------------------------
bool RunSimulator::TakeAction(const std::string &action,
                              const std::string &actionData)
{
   if (action == "Reset")
   {
      theSimulator->TakeAction("Reset");
      commandRunning = false;
      commandComplete = false;
      return true;
   }

   return RunSolver::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// GmatCommand* GetNext()
//------------------------------------------------------------------------------
/**
 * Retrieves the pointer to the next command that the Sandbox needs to run
 *
 * This method returns a pointer to the current RunSimulator command while the
 * simulation state machine is running.  It returns the next pointer after the
 * simulation has finished execution.
 *
 * @return The next comamnd that should Execute()
 */
//------------------------------------------------------------------------------
GmatCommand* RunSimulator::GetNext()
{
   if (commandRunning)
      return this;
   return next;
}


//------------------------------------------------------------------------------
// bool HasLocalClones()
//------------------------------------------------------------------------------
/**
 * Method to check for the presence of local clones
 *
 * @return true if there are local clones, false if not
 */
//------------------------------------------------------------------------------
bool RunSimulator::HasLocalClones()
{
   return true;
}


//------------------------------------------------------------------------------
// void UpdateClonedObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Updates cloned objects that are copies of the object passed in
 *
 * @param obj The object with setting updates
 */
//------------------------------------------------------------------------------
void RunSimulator::UpdateClonedObject(GmatBase *obj)
{
   throw CommandException("To do: implement Propagate::UpdateClonedObject");
}



//------------------------------------------------------------------------------
// Methods triggered by the finite state machine
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void PrepareToSimulate()
//------------------------------------------------------------------------------
/**
 * Responds to the CALCULATING state of the finite state machine
 *
 * Performs the final stages of initialization that need to be performed prior
 * to running the simulation.  This includes the final ODEModel preparation and
 * the setting for the flags that indicate that a simulation is in process.
 */
//------------------------------------------------------------------------------
void RunSimulator::PrepareToSimulate()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Entered RunSimulator::PrepareToSimulate()\n");
   #endif

   // Prep the measurement manager
   MeasurementManager *measman = theSimulator->GetMeasurementManager();
   if (measman->PrepareForProcessing(true) == false)
      throw CommandException(
            "Measurement Manager was unable to prepare for processing");

   PrepareToPropagate();  // ?? Test return value here?

   theSimulator->UpdateCurrentEpoch(baseEpoch[0]);
   commandRunning  = true;
   commandComplete = false;
}


//------------------------------------------------------------------------------
// void Propagate()
//------------------------------------------------------------------------------
/**
 * Responds to the PROPAGATING state of the finite state machine.
 *
 * Propagation from the current epoch to the next simulation epoch is performed
 * in this method.
 */
//------------------------------------------------------------------------------
void RunSimulator::Propagate()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Entered RunSimulator::Propagate()\n");
   #endif
   Real dt = theSimulator->GetTimeStep();

   #ifdef DEBUG_EVENT_STATE
      Integer dim = fm[0]->GetDimension();
      Real *odeState = fm[0]->GetState();

      MessageInterface::ShowMessage("State before prop\n");
      fm[0]->ReportEpochData();
      MessageInterface::ShowMessage("  ");
      for (Integer i = 0; i < dim; ++i)
         MessageInterface::ShowMessage("   %.12lf", odeState[i]);
      MessageInterface::ShowMessage("\n");
   #endif

   // todo: This is a temporary fix; need to evaluate to find a more elegant
   //       solution here
   Real maxStep = 600.0;
   if (dt > maxStep)
      dt = maxStep;
   Step(dt);
   bufferFilled = false;

   theSimulator->UpdateCurrentEpoch(currEpoch[0]);

   #ifdef DEBUG_EVENT_STATE
      dim = fm[0]->GetDimension();
      odeState = fm[0]->GetState();

      MessageInterface::ShowMessage("State after prop:\n  ");
      fm[0]->ReportEpochData();
      MessageInterface::ShowMessage("  ");
      for (Integer i = 0; i < dim; ++i)
         MessageInterface::ShowMessage("   %.12lf", odeState[i]);
      MessageInterface::ShowMessage("\n");
   #endif
}


//------------------------------------------------------------------------------
// void Calculate()
//------------------------------------------------------------------------------
/**
 * Responds to the CALCULATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunSimulator::Calculate()
{
#ifdef DEBUG_SIMULATOR_EXECUTION
   MessageInterface::ShowMessage("Entered RunSimulator::Calculate()\n");
#endif

   bufferFilled = false;
}


//------------------------------------------------------------------------------
// void LocateEvent()
//------------------------------------------------------------------------------
/**
 * Responds to the LOCATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunSimulator::LocateEvent()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Entered RunSimulator::LocateEvent()\n   "
            "bufferFilled is %s\n", (bufferFilled ? "true" : "false"));
   #endif

//   bool newEvent = false;
   EventStatus status;

   // First time through, buffer the objects that get propagated
   if (!bufferFilled)
   {
      dt = fm[0]->GetTime();
      #ifdef DEBUG_EVENT_STATE
         Integer dim = fm[0]->GetDimension();
         Real *odeState = fm[0]->GetState();

         MessageInterface::ShowMessage("State at event location start:\n  ");
         fm[0]->ReportEpochData();
         MessageInterface::ShowMessage("  dt = %.12lf\n  ", dt);
         for (Integer i = 0; i < dim; ++i)
            MessageInterface::ShowMessage("   %.12lf", odeState[i]);
         MessageInterface::ShowMessage("\n");
      #endif
      BufferSatelliteStates(true);
      eventMan->ClearObject(NULL);

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("   Buffer has %d objects; contents:\n",
               satBuffer.size());

         for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
              i != satBuffer.end(); ++i)
         {
            MessageInterface::ShowMessage(
                  "   Epoch of '%s' is %.12lf\n", (*i)->GetName().c_str(),
                  (*i)->GetRealParameter("A1Epoch"));
            MessageInterface::ShowMessage(
                  "   Position is [%.12lf  %.12lf  %.12lf]\n",
                  (*i)->GetRealParameter("X"), (*i)->GetRealParameter("Y"),
                  (*i)->GetRealParameter("Z"));
         }
      #endif

      bufferFilled = true;
      eventList.clear();
      eventList = theSimulator->GetRefObjectArray(Gmat::EVENT);

      // Set status of the events to ITERATING
      for (UnsignedInt i = 0; i < eventList.size(); ++i)
      {
         ((Event*)eventList[i])->CheckStatus(ITERATING);
         ((Event*)eventList[i])->Initialize();
      }

      if (eventList.size() == 0)
      {
         currentEvent = NULL;
         #ifdef DEBUG_SIMULATOR_EXECUTION
            MessageInterface::ShowMessage("   No events were found\n");
         #endif
         return;
      }
      else
      {
         currentEvent = ((Event*)eventList[0]);
//         newEvent = true;
         eventIndex = 0;
         // Will need to be updated when multiple propagators are enabled:
         eventMan->SetObject(propagators[0]);
         eventMan->SetObject(currentEvent);
         eventMan->SetFixedState(currentEvent);

         // Reset the state data to the starting states
         BufferSatelliteStates(false);
         propagators[0]->GetODEModel()->UpdateFromSpaceObject();
         fm[0]->SetTime(dt);

         #ifdef DEBUG_EVENT_LOCATION
            MessageInterface::ShowMessage("   Locating Event %d, of type %s\n",
                  eventIndex, currentEvent->GetTypeName().c_str());
         #endif
      }

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("Setting flag so state data will "
               "reset\n");
      #endif

      eventProcessComplete = true;
   }

   status = currentEvent->CheckStatus();

   if (status == LOCATED)
   {
      #ifdef DEBUG_EVENT_LOCATION
         MessageInterface::ShowMessage("Located the event %s (index %d)\n",
               currentEvent->GetTypeName().c_str(), eventIndex);
      #endif

      ++eventIndex;
      if (eventIndex < eventList.size())
      {
         #ifdef DEBUG_EVENT_LOCATION
            MessageInterface::ShowMessage("   Locating Event %d, of type %s\n",
                  eventIndex, currentEvent->GetTypeName().c_str());
         #endif

         // Prepare to work with the next active event
         eventMan->ClearObject(currentEvent);
         currentEvent = (Event*)eventList[eventIndex];
//         newEvent = true;

         // Eventually, set propagators associated with specific events here
         eventMan->SetObject(currentEvent);
         eventMan->SetFixedState(currentEvent);
         // Reset the state data to the starting states
         BufferSatelliteStates(false);
         propagators[0]->GetODEModel()->UpdateFromSpaceObject();
         fm[0]->SetTime(dt);

         #ifdef DEBUG_EXECUTION
            MessageInterface::ShowMessage("   Event %d, of type %s\n", j,
                  currentEvent->GetTypeName().c_str());
         #endif
      }
      else
      {
         eventMan->ClearObject(currentEvent);
         currentEvent = NULL;
         #ifdef DEBUG_EVENT_STATE
            Integer dim = fm[0]->GetDimension();
            Real *odeState = fm[0]->GetState();

            MessageInterface::ShowMessage("State at event location end:\n  ");
            fm[0]->ReportEpochData();
            MessageInterface::ShowMessage("  ");
            for (Integer i = 0; i < dim; ++i)
               MessageInterface::ShowMessage("   %.12lf", odeState[i]);
            MessageInterface::ShowMessage("\n");
         #endif
      }
   }

   if (currentEvent != NULL)
   {
      #ifdef DEBUG_EVENT_LOCATION
         MessageInterface::ShowMessage("   Finding root for the event\n");
      #endif
      eventMan->FindRoot(0);
   }

   // Reset the state data to the starting states
   BufferSatelliteStates(false);
   propagators[0]->GetODEModel()->UpdateFromSpaceObject();
   fm[0]->SetTime(dt);
}


//------------------------------------------------------------------------------
// void Simulate()
//------------------------------------------------------------------------------
/**
 * Responds to the SIMULATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunSimulator::Simulate()
{
   #ifdef DEBUG_SIMULATOR_EXECUTION
      MessageInterface::ShowMessage("Entered RunSimulator::Simulate()\n");
   #endif

   // If events were processed, we need to reset the state data
   if (eventProcessComplete == true)
   {
      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("Resetting state data after event "
               "location\n");
      #endif

      // Reset the state data to the starting states
      BufferSatelliteStates(false);
      propagators[0]->GetODEModel()->UpdateFromSpaceObject();
      eventProcessComplete = false;

      #ifdef DEBUG_EVENT_STATE
         Integer dim = fm[0]->GetDimension();
         Real *odeState = fm[0]->GetState();

         MessageInterface::ShowMessage("State in the Simulate method:\n  ");
         fm[0]->ReportEpochData();
         MessageInterface::ShowMessage("  ");
         for (Integer i = 0; i < dim; ++i)
            MessageInterface::ShowMessage("   %.12lf", odeState[i]);
         MessageInterface::ShowMessage("\n");
      #endif
   }

   #ifdef DEBUG_STATE_BUFFERING
      MessageInterface::ShowMessage("State changed to Simulate\n"
            "   Buffer has %d objects; contents:\n", satBuffer.size());

      for (std::vector<Spacecraft *>::iterator i = satBuffer.begin();
           i != satBuffer.end(); ++i)
      {
         MessageInterface::ShowMessage(
               "   Epoch of '%s' is %.12lf\n", (*i)->GetName().c_str(),
               (*i)->GetRealParameter("A1Epoch"));
         MessageInterface::ShowMessage(
               "   Position is [%.12lf  %.12lf  %.12lf]\n",
               (*i)->GetRealParameter("X"), (*i)->GetRealParameter("Y"),
               (*i)->GetRealParameter("Z"));
      }
   #endif
}


//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/**
 * Responds to the FINALIZING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunSimulator::Finalize()
{
#ifdef DEBUG_SIMULATOR_EXECUTION
   MessageInterface::ShowMessage("Entered RunSimulator::Finalize()\n");
#endif
   // Do cleanup here

   // Finalize the measurement manager
   MeasurementManager *measman = theSimulator->GetMeasurementManager();
   if (measman->ProcessingComplete() == false)
      MessageInterface::ShowMessage(
            "Measurement Manager reported a problem completing processing\n");

   commandComplete = true;
   commandRunning  = false;
}
