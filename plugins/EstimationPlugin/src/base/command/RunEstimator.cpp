//$Id: RunEstimator.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         RunEstimator
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
// Created: 2009/08/03
//
/**
 * Implementation of the Mission Control Sequence command that drives estimation
 */
//------------------------------------------------------------------------------


#include "RunEstimator.hpp"

#include "MessageInterface.hpp"
#include "ODEModel.hpp"

//#define DEBUG_INITIALIZATION
//#define DEBUG_EXECUTION


//------------------------------------------------------------------------------
// RunEstimator()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
RunEstimator::RunEstimator() :
   RunSolver               ("RunEstimator"),
   theEstimator            (NULL),
   commandRunning          (false),
   commandComplete         (false),
   propPrepared            (false),
   estimationOffset        (0.0),
   bufferFilled            (false),
   currentEvent            (NULL),
   eventProcessComplete    (false),
   eventMan                (NULL)
{
   overridePropInit = true;
}


//------------------------------------------------------------------------------
// ~RunEstimator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
RunEstimator::~RunEstimator()
{
   if (theEstimator)
      delete theEstimator;
}


//------------------------------------------------------------------------------
// RunEstimator(const RunEstimator & rs)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rs The command that is copied into the new one
 */
//------------------------------------------------------------------------------
RunEstimator::RunEstimator(const RunEstimator & rs) :
   RunSolver               (rs),
   theEstimator            (NULL),
   commandRunning          (false),
   commandComplete         (false),
   propPrepared            (false),
   estimationOffset        (rs.estimationOffset),
   bufferFilled            (false),
   currentEvent            (NULL),
   eventProcessComplete    (false),
   eventMan                (NULL)
{
   overridePropInit = true;
}

//------------------------------------------------------------------------------
// RunEstimator& operator=(const RunEstimator & rs)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rs The RunEstimator object that supplies properties this one needs
 *
 * @return A reference to this instance
 */
//------------------------------------------------------------------------------
RunEstimator& RunEstimator::operator=(const RunEstimator & rs)
{
   if (&rs != this)
   {
      RunSolver::operator=(rs);

      theEstimator     = NULL;
      commandRunning   = false;
      commandComplete  = false;
      overridePropInit = true;
      propPrepared     = false;
      estimationOffset = rs.estimationOffset;
      bufferFilled     = false;
      currentEvent     = NULL;
      eventProcessComplete = false;
      eventMan         = NULL;
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase *Clone() const
//------------------------------------------------------------------------------
/**
 * Cleates a duplicate of a RunEstimator object
 *
 * @return a clone of the object
 */
//------------------------------------------------------------------------------
GmatBase *RunEstimator::Clone() const
{
   return new RunEstimator(*this);
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
std::string RunEstimator::GetRefObjectName(const Gmat::ObjectType type) const
{
   switch (type)
   {
      case Gmat::SOLVER:
         #ifdef DEBUG_RUN_Estimator
            MessageInterface::ShowMessage
               ("Getting RunEstimator reference solver name\n");
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
bool RunEstimator::SetRefObjectName(const Gmat::ObjectType type,
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
bool RunEstimator::RenameRefObject(const Gmat::ObjectType type,
                                    const std::string &oldName,
                                    const std::string &newName)
{
   // RunEstimator needs to know about Solver name only
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
//  const std::string GetGeneratingString(Gmat::WriteMode mode,
//        const std::string &prefix, const std::string &useName)
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
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name. (Used for
 *                indentation)
 * @param useName Name that replaces the object's name (Not yet used
 *                in commands).
 *
 * @return The script line that defines this GmatCommand.
 */
//------------------------------------------------------------------------------
const std::string& RunEstimator::GetGeneratingString(Gmat::WriteMode mode,
      const std::string &prefix, const std::string &useName)
{
   generatingString = prefix + "RunEstimator " + solverName + ";";

   return RunSolver::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepares the command for execution
 *
 * This method prepares the Estimator and associated measurement manager and
 * measurements for the simulation process.  Referenced objects are cloned or
 * set as needed in this method.
 *
 * The propagation subsystem is prepared in the base class components of the
 * command.  RunEstimator generaqtes teh PropSetup clones at this level, but
 * leaves the rest of the initialization process for the PropSetups in the base
 * class method, which is called from this method.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RunEstimator::Initialize()
{
   bool retval = false;

   // First set the Estimator object
   if (solverName == "")
      throw CommandException("Cannot initialize RunEstimator command -- the "
            "Estimator name is not specified.");

   // Clear the old clone if it was set
   if (theEstimator != NULL)
      delete theEstimator;

   GmatBase *simObj = FindObject(solverName);
   if (simObj == NULL)
      throw CommandException("Cannot initialize RunEstimator command -- the "
            "Estimator named " + solverName + " cannot be found.");

   if (!simObj->IsOfType("Estimator"))
      throw CommandException("Cannot initialize RunEstimator command -- the "
            "object named " + solverName + " is not a Estimator.");

   theEstimator = (Estimator*)(simObj->Clone());

   theEstimator->TakeAction("ResetInstanceCount");
   simObj->TakeAction("ResetInstanceCount");
   theEstimator->TakeAction("IncrementInstanceCount");
   simObj->TakeAction("IncrementInstanceCount");

   // Set the streams for the measurement manager
   MeasurementManager *measman = theEstimator->GetMeasurementManager();
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

   // Next initialize the estimation subsystem
   EstimationStateManager *esm = theEstimator->GetEstimationStateManager();
   // Load the participant list into the esm
   StringArray participants = measman->GetParticipantList();
   esm->SetParticipantList(participants);

   // Pass in the objects
   StringArray objList = esm->GetObjectList("");
   for (UnsignedInt i = 0; i < objList.size(); ++i)
   {
      GmatBase* obj = FindObject(objList[i]);
      if (obj != NULL)
      {
         esm->SetObject(obj);
      }
   }

   esm->BuildState();

   // Find the event manager and store its pointer
   if (triggerManagers == NULL)
      throw CommandException("The Event Manager pointer was not set on the "
            "RunEstimator command");

   for (UnsignedInt i = 0; i < triggerManagers->size(); ++i)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage("RunEstimator has an TriggerManager of "
               "type %s, id %d\n",
               (*triggerManagers)[i]->GetTriggerTypeString().c_str(),
               (*triggerManagers)[i]->GetTriggerType());
      #endif
      if ((*triggerManagers)[i]->GetTriggerType() == Gmat::EVENT)
      {
         eventMan = (EventManager*)(*triggerManagers)[i];
         #ifdef DEBUG_INITIALIZATION
            MessageInterface::ShowMessage("RunEstimator has an EventManager of "
                  "type %s\n", eventMan->GetTriggerTypeString().c_str());
         #endif
      }
   }
   if (eventMan == NULL)
      throw CommandException("The EventManager pointer was not set on the "
            "RunEstimator command");

   // Next comes the propagator
   PropSetup *obj = theEstimator->GetPropagator();

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

         // RunEstimator only manages one PropSetup.  If that changes, so
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
         eventMan->SetObject(ps);

         retval = true;
      }
   }
   else
      throw CommandException("Cannot initialize RunEstimator command; the "
            "propagator pointer in the Estimator " +
            theEstimator->GetName() + " is NULL.");

   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("RunEstimator command found %d "
            "participants\n", participants.size());
   #endif

   propObjectNames.clear();
   propObjectNames.push_back(participants);
   propPrepared = false;

   // Now we can initialize the propagation subsystem by calling up the
   // inheritance tree.
   if (retval)
      retval = RunSolver::Initialize();

   #ifdef DEBUG_INITIALIZATION
      if (retval == false)
         MessageInterface::ShowMessage("RunEstimator command failed to "
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
void RunEstimator::SetPropagationProperties(PropagationStateManager *psm)
{
   ObjectArray pObjects;
   psm->GetStateObjects(pObjects, Gmat::UNKNOWN_OBJECT);

   for (ObjectArray::iterator p = pObjects.begin(); p != pObjects.end(); ++p)
   {
      if ((*p)->IsOfType(Gmat::SPACEOBJECT))
      {
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
 * This method calls the Estimator to determine the state of the Simulation
 * state machine and responds to that state as needed.  Typical command side
 * responses are to propagate as needed, to clean up memory, or to reset flags
 * based on the state machine.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RunEstimator::Execute()
{
   #ifdef DEBUG_EXECUTION
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
   Solver::SolverState state = theEstimator->GetState();

   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("\nEstimator state is %d\n", state);
   #endif
   switch (state)
   {
      case Solver::INITIALIZING:
         PrepareToEstimate();
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

      case Solver::ACCUMULATING:
         Accumulate();
         break;

      case Solver::ESTIMATING:
         Estimate();
         break;

      case Solver::CHECKINGRUN:
         CheckConvergence();
         break;

      case Solver::FINISHED:
         Finalize();
         break;

      default:
         throw CommandException("Unknown state "
               " encountered in the RunEstimator command");
   }

   state = theEstimator->AdvanceState();

   return true;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
/**
 * Completes processing so that subsequent mission runs can be executed.
 */
//------------------------------------------------------------------------------
void RunEstimator::RunComplete()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::RunComplete()\n");
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
bool RunEstimator::TakeAction(const std::string &action,
                              const std::string &actionData)
{
   if (action == "Reset")
   {
      theEstimator->TakeAction("Reset");
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
 * This method returns a pointer to the current RunEstimator command while the
 * simulation state machine is running.  It returns the next pointer after the
 * simulation has finished execution.
 *
 * @return The next command that should Execute()
 */
//------------------------------------------------------------------------------
GmatCommand* RunEstimator::GetNext()
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
bool RunEstimator::HasLocalClones()
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
void RunEstimator::UpdateClonedObject(GmatBase *obj)
{
   throw CommandException("To do: implement Propagate::UpdateClonedObject");
}

//------------------------------------------------------------------------------
// Methods triggered by the finite state machine
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// void PrepareToEstimate()
//------------------------------------------------------------------------------
/**
 * Responds to the INITIALIZING state of the finite state machine
 *
 * Performs the final stages of initialization that need to be performed prior
 * to running the estimation.  This includes the final ODEModel preparation and
 * the setting for the flags that indicate that an estimation is in process.
 */
//------------------------------------------------------------------------------
void RunEstimator::PrepareToEstimate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage(
            "Entered RunEstimator::PrepareToEstimate()\n");
   #endif

   if (!propPrepared)
   {
      PrepareToPropagate();  // ?? Test return value here?
      commandRunning  = true;
      commandComplete = false;
      propPrepared    = true;
   }

   estimationOffset = fm[0]->GetTime();
}


//------------------------------------------------------------------------------
// void Propagate()
//------------------------------------------------------------------------------
/**
 * Responds to the PROPAGATING state of the finite state machine.
 *
 * Propagation from the current epoch to the next estimation epoch is performed
 * in this method.
 */
//------------------------------------------------------------------------------
void RunEstimator::Propagate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Propagate()\n");
   #endif

   // If state reset at current epoch - e.g. during sequential estimation -
   // reload the prop vector
   if (theEstimator->ResetState())
   {
      #ifdef DEBUG_STATE_RESETS
         MessageInterface::ShowMessage("Calling UpdateFromSpaceObject()\n");
         Real* oldState = fm[0]->GetState();
         MessageInterface::ShowMessage("   Old state[0] element:  %.12lf\n", oldState[0]);
      #endif

      fm[0]->UpdateFromSpaceObject();

      #ifdef DEBUG_STATE_RESETS
         Real* newState = fm[0]->GetState();
         MessageInterface::ShowMessage("   New state[0] element:  %.12lf\n", newState[0]);
      #endif
   }

   // If state reset to initial epoch - e.g. during batch estimation -
   // reload prop vector and reset the epoch information
   if (startNewPass == true)
   {
      fm[0]->UpdateFromSpaceObject();
      fm[0]->SetTime(estimationOffset);
      startNewPass = false;
   }

   Real dt = theEstimator->GetTimeStep();

   // todo: This is a temporary fix; need to evaluate to find a more elegant
   //       solution here
   Real maxStep = 600.0;
   if (fabs(dt) > maxStep)
      dt = (dt > 0.0 ? maxStep : -maxStep);
   Step(dt);
   bufferFilled = false;

   theEstimator->UpdateCurrentEpoch(currEpoch[0]);
}


//------------------------------------------------------------------------------
// void Calculate()
//------------------------------------------------------------------------------
/**
 * Responds to the CALCULATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::Calculate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Calculate() "
            "epoch = %.12lf\n", currEpoch[0]);
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
void RunEstimator::LocateEvent()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::LocateEvent()\n");
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
      eventList = theEstimator->GetRefObjectArray(Gmat::EVENT);

      #ifdef DEBUG_STATE_BUFFERING
         MessageInterface::ShowMessage("Event list has %d events\n",
               eventList.size());
      #endif
      // Set status of the events to ITERATING
      for (UnsignedInt i = 0; i < eventList.size(); ++i)
      {
         ((Event*)eventList[i])->CheckStatus(ITERATING);
         ((Event*)eventList[i])->Initialize();
      }

      if (eventList.size() == 0)
      {
         currentEvent = NULL;
         #ifdef DEBUG_EXECUTION
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

   if (currentEvent == NULL)
      return;

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
// void Accumulate()
//------------------------------------------------------------------------------
/**
 * Performs command side actions taken during accumulation
 */
//------------------------------------------------------------------------------
void RunEstimator::Accumulate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Accumulate()\n");
   #endif

   CleanUpEvents();
}


//------------------------------------------------------------------------------
// void Estimate()
//------------------------------------------------------------------------------
/**
 * Responds to the ESTIMATING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::Estimate()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Estimate()\n");
   #endif

   CleanUpEvents();
}

//------------------------------------------------------------------------------
// void CheckConvergence()
//------------------------------------------------------------------------------
/**
 * Performs command actions needed when testing for convergence
 */
//------------------------------------------------------------------------------
void RunEstimator::CheckConvergence()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::CheckConvergence() "
            "epoch = %.12lf\n", currEpoch[0]);
   #endif

   startNewPass = true;
}

//------------------------------------------------------------------------------
// void Finalize()
//------------------------------------------------------------------------------
/**
 * Responds to the FINALIZING state of the finite state machine
 */
//------------------------------------------------------------------------------
void RunEstimator::Finalize()
{
   #ifdef DEBUG_EXECUTION
      MessageInterface::ShowMessage("Entered RunEstimator::Finalize() "
            "epoch = %.12lf\n", currEpoch[0]);
   #endif

   // Do cleanup here

   // Finalize the Estimator
   if (theEstimator->Finalize() == false)
      MessageInterface::ShowMessage(
            "The Estimator %s reported a problem completing processing\n",
            theEstimator->GetName().c_str());

   BuildCommandSummary(true);

   commandComplete = true;
   commandRunning  = false;
   propPrepared    = false;
}


//------------------------------------------------------------------------------
// void CleanUpEvents()
//------------------------------------------------------------------------------
/**
 * Restores the buffered satellite states after event processing
 */
//------------------------------------------------------------------------------
void RunEstimator::CleanUpEvents()
{
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

         MessageInterface::ShowMessage("State in the Accumulate method:\n  ");
         fm[0]->ReportEpochData();
         MessageInterface::ShowMessage("  ");
         for (Integer i = 0; i < dim; ++i)
            MessageInterface::ShowMessage("   %.12lf", odeState[i]);
         MessageInterface::ShowMessage("\n");
      #endif
   }
}
