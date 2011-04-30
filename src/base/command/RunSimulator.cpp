//$Id$
//------------------------------------------------------------------------------
//                         ClassName
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
// Created: 2009/ /
//
/**
 * File description here.
 */
//------------------------------------------------------------------------------


#include "RunSimulator.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_INITIALIZATION
//#define DEBUG_SIMULATOR_EXECUTION


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
   commandComplete         (false)
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
// RunSimulator::RunSimulator(const RunSimulator & rs)
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
   commandComplete         (false)
{
   overridePropInit = true;
}

//------------------------------------------------------------------------------
// RunSimulator & operator=(const RunSimulator & rs)
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
      overridePropInit = true;
   }

   return *this;
}

//------------------------------------------------------------------------------
// GmatBase *RunSimulator::Clone() const
//------------------------------------------------------------------------------
/**
 * Cleates a duplicate of a RunSimulator object
 *
 * @return a clone of the object
 */
//------------------------------------------------------------------------------
GmatBase *RunSimulator::Clone() const
{
   return new RunSimulator(*this);
}


//------------------------------------------------------------------------------
// std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Accesses names for referenced objects.
 *
 * @param <type> Type of object requested.
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
               ("Getting EndFiniteBurn reference burn names\n");
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
 * @param <type> Type of the object.
 * @param <name> Name of the object.
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
                                    const std::string &oldName,
                                    const std::string &newName)
{
   // EndFiniteBurn needs to know about Burn and Spacecraft only
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
 * @param <mode>    Specifies the type of serialization requested.
 * @param <prefix>  Optional prefix appended to the object's name. (Used for
 *                  indentation)
 * @param <useName> Name that replaces the object's name (Not yet used
 *                  in commands).
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
 * command.  RunSimulator generaqtes teh PropSetup clones at this level, but
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
            Datafile *df = (Datafile*)obj;
            measman->SetStreamObject(df);
         }
      }
      else
         throw CommandException("Did not find the object named " +
               streamList[ms]);
   }

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
   retval = RunSolver::Initialize();

   #ifdef DEBUG_INITIALIZATION
      if (retval == false)
         MessageInterface::ShowMessage("RunSimulator command failed to "
               "initialize; RunSolver::Initialize() call failed.\n");
   #endif

   return retval;
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
         // The LOCATING state shouldn't trigger until we have event location
         // implemented, so this case should not fire.
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

   PrepareToPropagate();
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

   // todo: This is a temporary fix; need to evaluate to find a more elegant
   //       solution here
   Real maxStep = 600.0;
   if (dt > maxStep)
      dt = maxStep;
   Step(dt);

   theSimulator->UpdateCurrentEpoch(currEpoch[0]);
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
   // We might not need anything here -- it's all Simulator side work
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
   MessageInterface::ShowMessage("Entered RunSimulator::LocateEvent()\n");
#endif
   // We'll figure this out later
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
   // We might not need anything here -- it's all Simulator side work
}


//------------------------------------------------------------------------------
// void RunSimulator::Finalize()
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
