//$Id$
//------------------------------------------------------------------------------
//                                Optimize 
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Daniel Hunter/GSFC/MAB (CoOp)
// Created: 2006.07.20
//
/**
 * Implementation for the Optimize command class
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "Optimize.hpp"
#include "MessageInterface.hpp"

//Added __USE_EXTERNAL_OPTIMIZER__ so that header will not be compiled
#ifdef __USE_EXTERNAL_OPTIMIZER__
#include "ExternalOptimizer.hpp"
#endif

#include "GmatInterface.hpp"
static GmatInterface *gmatInt = GmatInterface::Instance();

//#define DEBUG_OPTIMIZE_PARSING
//#define DEBUG_CALLBACK
//#define DEBUG_OPTIMIZE_CONSTRUCTION
//#define DEBUG_OPTIMIZE_INIT
//#define DEBUG_OPTIMIZE_EXEC
//#define DEBUG_STATE_TRANSITIONS

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif
#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string
Optimize::PARAMETER_TEXT[OptimizeParamCount - SolverBranchCommandParamCount] =
{
   "OptimizerName",
   "OptimizerConverged",
};

const Gmat::ParameterType
Optimize::PARAMETER_TYPE[OptimizeParamCount - SolverBranchCommandParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::BOOLEAN_TYPE,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// constructor
//------------------------------------------------------------------------------
Optimize::Optimize() :
   SolverBranchCommand              ("Optimize"),
   optimizerConverged               (false),
   optimizerInFunctionInitialized   (false),
   optimizerInDebugMode             (false),
   minimizeCount                    (0)
{
   #ifdef DEBUG_OPTIMIZE_CONSTRUCTION
      MessageInterface::ShowMessage("NOW creating Optimize command ...");
   #endif
   parameterCount = OptimizeParamCount;
   objectTypeNames.push_back("Optimize");
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
Optimize::Optimize(const Optimize& o) :
   SolverBranchCommand              (o),
   optimizerConverged               (false),
   optimizerInFunctionInitialized   (false),
   optimizerInDebugMode             (o.optimizerInDebugMode),
   minimizeCount                    (0)
{
   //parameterCount = OptimizeParamCount;  // this is set in GmatBase copy constructor
   #ifdef DEBUG_OPTIMIZE_CONSTRUCTION
      MessageInterface::ShowMessage("NOW creating (copying) Optimize command ...");
   #endif
   localStore.clear();
}

//------------------------------------------------------------------------------
// operator =
//------------------------------------------------------------------------------
Optimize& Optimize::operator=(const Optimize& o)
{
   if (this == &o)
      return *this;
    
   GmatCommand::operator=(o);

   optimizerConverged   = false;
   optimizerInFunctionInitialized = false;
   optimizerInDebugMode = o.optimizerInDebugMode;
   localStore.clear();
   minimizeCount = 0;

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
Optimize::~Optimize()
{
}


//------------------------------------------------------------------------------
// Append
//------------------------------------------------------------------------------
bool Optimize::Append(GmatCommand *cmd)
{
   #ifdef DEBUG_OPTIMIZE_PARSING
       MessageInterface::ShowMessage("\nOptimize::Append received \"%s\" command",
                                     cmd->GetTypeName().c_str());
   #endif
    
   if (!SolverBranchCommand::Append(cmd))
        return false;
    
   // If at the end of a optimizer branch, point that end back to this command.
   if (cmd->GetTypeName() == "EndOptimize") 
   {
      if ((nestLevel == 0) && (branchToFill != -1))  
      {
         cmd->Append(this);
         // Optimizer loop is complete; -1 pops to the next higher sequence.
         branchToFill = -1;
         #ifdef DEBUG_OPTIMIZE_PARSING
             MessageInterface::ShowMessage("\nOptimize::Append closing \"%s\"",
                                           generatingString.c_str());
         #endif
      }
      else
      {
         --nestLevel;
         if (minimizeCount > 0)
            --minimizeCount;
      }
   }

   // If it's a nested optimizer branch, add to the nest level.
   // 2006.09.13 wcs - as of today, nested optimizers are not allowed
   if (cmd->GetTypeName() == "Optimize")
      ++nestLevel;

   if (cmd->GetTypeName() == "Minimize")
   {
      // Code for nesting, currently disabled
      // if (solverName == cmd->GetStringParameter("OptimizerName"))
      {
         #ifdef DEBUG_OPTIMIZE_PARSING
            MessageInterface::ShowMessage("MinCount: %d     nest level: %d\n",
                  minimizeCount, nestLevel);
         #endif
         ++minimizeCount;
         if (minimizeCount > nestLevel + 1)
            throw CommandException("Optimization control sequences are only "
                  "allowed one Minimize command");
      }
   }

   #ifdef DEBUG_OPTIMIZE_PARSING
       MessageInterface::ShowMessage("\nOptimize::Append for \"%s\" nest level = %d",
                                     generatingString.c_str(), nestLevel);
   #endif

   return true;
}

//------------------------------------------------------------------------------
// Clone
//------------------------------------------------------------------------------
GmatBase* Optimize::Clone() const
{
   return (new Optimize(*this));
}

//------------------------------------------------------------------------------
// GetGeneratingString
//------------------------------------------------------------------------------
const std::string& Optimize::GetGeneratingString(Gmat::WriteMode mode,
                                                 const std::string &prefix,
                                                 const std::string &useName)
{
   generatingString = "";
   
   if (mode != Gmat::NO_COMMENTS)
   {
      generatingString = prefix;
   }
   
   generatingString += "Optimize " + solverName;
   
   // Handle the option strings
   generatingString += GetSolverOptionText();
   
   generatingString += ";";

   if (mode == Gmat::NO_COMMENTS)
      return generatingString;

   return SolverBranchCommand::GetGeneratingString(mode, prefix, useName);
}

//------------------------------------------------------------------------------
// RenameRefObject
//------------------------------------------------------------------------------
bool Optimize::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (solverName == oldName)
         solverName = newName;
   }
   
   return true;
}

//------------------------------------------------------------------------------
// GetParameterText
//------------------------------------------------------------------------------
std::string Optimize::GetParameterText(const Integer id) const
{
   if (id >= SolverBranchCommandParamCount && id < OptimizeParamCount)
      return PARAMETER_TEXT[id - SolverBranchCommandParamCount];
    
   return SolverBranchCommand::GetParameterText(id);
}

//------------------------------------------------------------------------------
// GetParameterID
//------------------------------------------------------------------------------
Integer Optimize::GetParameterID(const std::string &str) const
{
   for (Integer i = SolverBranchCommandParamCount; i < OptimizeParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SolverBranchCommandParamCount])
         return i;
   }
    
   return SolverBranchCommand::GetParameterID(str);
}

//------------------------------------------------------------------------------
// GetParameterType
//------------------------------------------------------------------------------
Gmat::ParameterType Optimize::GetParameterType(const Integer id) const
{
   if (id >= SolverBranchCommandParamCount && id < OptimizeParamCount)
      return PARAMETER_TYPE[id - SolverBranchCommandParamCount];
    
   return SolverBranchCommand::GetParameterType(id);
}

//------------------------------------------------------------------------------
// GetParameterTypeString
//------------------------------------------------------------------------------
std::string Optimize::GetParameterTypeString(const Integer id) const
{    
   return SolverBranchCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// GetStringParameter
//------------------------------------------------------------------------------
std::string Optimize::GetStringParameter(const Integer id) const
{
   if (id == OPTIMIZER_NAME)
      return solverName;
    
   return SolverBranchCommand::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// SetStringParameter
//------------------------------------------------------------------------------
bool Optimize::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == OPTIMIZER_NAME) 
   {
      solverName = value;
      return true;
   }
    
   return SolverBranchCommand::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// GetBooleanParameter
//------------------------------------------------------------------------------
bool Optimize::GetBooleanParameter(const Integer id) const
{
   if (id == OPTIMIZER_CONVERGED)
      return optimizerConverged;
      
   return SolverBranchCommand::GetBooleanParameter(id);
}

//------------------------------------------------------------------------------
// GetRefObjectName
//------------------------------------------------------------------------------
std::string Optimize::GetRefObjectName(const Gmat::ObjectType type) const
{
   if (type == Gmat::SOLVER)
      return solverName;
   return SolverBranchCommand::GetRefObjectName(type);
}

//------------------------------------------------------------------------------
// SetRefObjectName
//------------------------------------------------------------------------------
bool Optimize::SetRefObjectName(const Gmat::ObjectType type,
                                const std::string &name)
{
   if (type == Gmat::SOLVER) 
   {
      solverName = name;
      return true;
   }
   return SolverBranchCommand::SetRefObjectName(type, name);
}

//------------------------------------------------------------------------------
// Initialize
//------------------------------------------------------------------------------
bool Optimize::Initialize()
{
   #ifdef DEBUG_OPTIMIZE_INIT
   ShowCommand("", "Initialize() this = ", this);
   #endif
   
   GmatBase *mapObj = NULL;
   if ((mapObj = FindObject(solverName)) == NULL) 
   {
      std::string errorString = "Optimize command cannot find optimizer \"";
      errorString += solverName;
      errorString += "\"";
      throw CommandException(errorString);
   }
   
   if (mapObj->IsOfType("Optimizer") == false)
      throw CommandException("The object " + solverName +
            " is not an Optimizer, so the Optimize command cannot proceed "
            "with initialization.");

   // Delete the old cloned solver
   if (theSolver)
   {
      #ifdef DEBUG_MEMORY
      MemoryTracker::Instance()->Remove
         (theSolver, "local solver", "Optimize::Initialize()",
          "deleting local cloned solver");
      #endif
      delete theSolver;
   }
   
   // Clone the optimizer for local use
   theSolver = (Solver *)(mapObj->Clone());
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (theSolver, theSolver->GetName(), "Optimize::Initialize()",
       "theSolver = (Solver *)(mapObj->Clone())");
   #endif
   
   theSolver->TakeAction("ResetInstanceCount");
   mapObj->TakeAction("ResetInstanceCount");
   
   theSolver->TakeAction("IncrementInstanceCount");
   mapObj->TakeAction("IncrementInstanceCount");
   
   if (theSolver->GetStringParameter("ReportStyle") == "Debug")
      optimizerInDebugMode = true;      
    
   // Set the local copy of the optimizer on each node
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *currentCmd;
   specialState = Solver::INITIALIZING;

   Integer constraintCount = 0, variableCount = 0, objectiveCount = 0;
   for (node = branch.begin(); node != branch.end(); ++node)
   {
      currentCmd = *node;

      #ifdef DEBUG_OPTIMIZE_COMMANDS
         Integer nodeNum = 0;
      #endif
      while ((currentCmd != NULL) && (currentCmd != this))
      {
         #ifdef DEBUG_OPTIMIZE_COMMANDS
            MessageInterface::ShowMessage(
               "   Optimize Command %d:  %s\n", ++nodeNum, 
               currentCmd->GetTypeName().c_str());       
         #endif
         
         if ((currentCmd->GetTypeName() == "Vary") || 
             (currentCmd->GetTypeName() == "Minimize") ||
             (currentCmd->GetTypeName() == "NonlinearConstraint"))
         {
            currentCmd->SetRefObject(theSolver, Gmat::SOLVER, solverName);
            if (theSolver->IsSolverInternal())
            {
               if (currentCmd->GetTypeName() == "Minimize")
                  ++objectiveCount;
               if (currentCmd->GetTypeName() == "NonlinearConstraint")
               {
                  ++constraintCount;
               }
               if (currentCmd->GetTypeName() == "Vary")
                  ++variableCount;
               
               #ifdef DEBUG_OPTIMIZE_EXEC
                  MessageInterface::ShowMessage(
                     "   *** COUNTS: %d Costs, %d Variables, %d Constraints ***\n",
                     objectiveCount, variableCount, constraintCount);
               #endif
            }               
         }
         
         currentCmd = currentCmd->GetNext();
      }
   }

   bool retval = SolverBranchCommand::Initialize();

   if (retval == true) 
   {
      if (theSolver->IsSolverInternal())
      {
         theSolver->SetIntegerParameter(
               theSolver->GetParameterID("RegisteredVariables"), variableCount);
         theSolver->SetIntegerParameter(
               theSolver->GetParameterID("RegisteredComponents"), 
               constraintCount);
      }
      retval = theSolver->Initialize();
   }
   
   // Register callbacks for external optimizers
   if (theSolver->IsOfType("ExternalOptimizer"))
   {
      // NOTE that in the future we may have a callback to/from a non_MATLAB
      // external optimizer
      if (GmatGlobal::Instance()->IsMatlabAvailable())
         if (theSolver->GetStringParameter("SourceType") == "MATLAB")
            gmatInt->RegisterCallbackServer(this);
   }
   
   optimizerInFunctionInitialized = false;
   return retval;
}

//------------------------------------------------------------------------------
// Execute
//------------------------------------------------------------------------------
bool Optimize::Execute()
{
   #ifdef DEBUG_OPTIMIZE_EXEC
   MessageInterface::ShowMessage("Optimize::Execute() <%p> entered\n", this);
   #endif
   
   // We need to re-initialize since only one MatlabEngine is running per GMAT
   // session. This will allow to run back to back optimization.
   if (!commandExecuting)
      Initialize();
   
   // If optimizing inside a function, we need to reinitialize since the local solver is
   // cloned in Initialize(). All object data setting are done through assignment command
   // which happens after Optimize::Initialize(). (LOJ: 2009.03.19)
   if (currentFunction != NULL && !optimizerInFunctionInitialized)
   {
      Initialize();
      optimizerInFunctionInitialized = true;
   }
   
   bool retval = true;
   
   // Drive through the state machine.
   Solver::SolverState state = theSolver->GetState();
   
   #ifdef DEBUG_OPTIMIZE_EXEC
      MessageInterface::ShowMessage("Optimize::Execute(%s, %s, solver state is %d)\n",
         (commandExecuting ? "command executing" :"command NOT executing"),
         (commandComplete ? "command complete" : "command NOT complete"), state);
   #endif
   
   // Attempt to reset if recalled   
   if (commandComplete)
   {
      commandComplete  = false;
      commandExecuting = false;
      specialState = Solver::INITIALIZING;
   }  

   if (!commandExecuting) 
   {
      #ifdef DEBUG_OPTIMIZE_EXEC
         MessageInterface::ShowMessage(
         "Entered Optimizer while command is not executing\n");
      #endif

      FreeLoopData();
      StoreLoopData();

      retval = SolverBranchCommand::Execute();

      #ifdef DEBUG_OPTIMIZE_EXEC
         MessageInterface::ShowMessage("Resetting the Optimizer\n");
      #endif

      theSolver->TakeAction("Reset");
      state = theSolver->GetState();
      
   }
   
   // Branch based on the optimizer model; handle internal optimizers first
   if (theSolver->IsSolverInternal())
      retval = RunInternalSolver(state);
   else
      retval = RunExternalSolver(state);
   
   // Advance the state
   if (!branchExecuting)
   {
      #ifdef DEBUG_OPTIMIZE_EXEC
         MessageInterface::ShowMessage(
            "Optimize::Execute - about to advance the state\n");
      #endif
      theSolver->AdvanceState();

      if (theSolver->GetState() == Solver::FINISHED) 
      {
         publisher->FlushBuffers();
         optimizerConverged = true;
      }
   }
   
   // Pass spacecraft data to the optimizer for reporting in debug mode
   if (optimizerInDebugMode)
   {
      std::string dbgData = "";
      for (ObjectArray::iterator i = localStore.begin(); i < localStore.end(); ++i)
      {
         dbgData += (*i)->GetGeneratingString() + "\n---\n";
      }
      theSolver->SetDebugString(dbgData);
   }
   
   BuildCommandSummary(true);
   
   #ifdef DEBUG_OPTIMIZE_EXEC
   MessageInterface::ShowMessage
      ("Optimize::Execute() <%p> returning %d\n", this, retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void Optimize::RunComplete()
{
   if (theSolver != NULL)
      theSolver->Finalize();
   
   // Free local data (LOJ: 2009.03.19)
   FreeLoopData();
   
   SolverBranchCommand::RunComplete();
}


//------------------------------------------------------------------------------
// bool ExecuteCallback()
//------------------------------------------------------------------------------
bool Optimize::ExecuteCallback()
{
   #ifdef DEBUG_CALLBACK
      MessageInterface::ShowMessage("Entering Optimize::ExecuteCallback\n");
   #endif
   // NOTE that in the future we may have a callback to/from a non_MATLAB
   // external optimizer

   #ifdef __USE_EXTERNAL_OPTIMIZER__
   if (!optimizer || 
      (!(theSolver->IsOfType("ExternalOptimizer"))) || 
      (((ExternalOptimizer*)optimizer)->GetStringParameter("SourceType")
      != "MATLAB"))
   {
      throw CommandException(
      "Optimize::ExecuteCallback not yet implemented for non_MATLAB optimizers");
   }
   #endif
   
   // Source type is MATLAB
   if (!GmatGlobal::Instance()->IsMatlabAvailable())
      throw CommandException("Optimize: ERROR - MATLAB required for Callback");
   
   callbackExecuting = true;
   // ask Matlab for the value of X
   Integer     n = theSolver->GetIntegerParameter(
                   theSolver->GetParameterID("NumberOfVariables"));
   
   //Real X[n];
   Real *X = new Real[n];
   
   // read X values from the callback data string here
   std::stringstream ins(callbackData.c_str());
   for (Integer i=0; i<n; i++)
      ins >> X[i];
   std::vector<Real> vars;
   for (Integer i=0;i<n;i++)
      vars.push_back(X[i]);
   
   delete [] X;
   
   // get the state of the Optimizer
   Solver::SolverState nState = theSolver->GetNestedState(); 
   if (nState == Solver::INITIALIZING)
   {
      #ifdef DEBUG_CALLBACK
      MessageInterface::ShowMessage(
         "Optimize::ExecuteCallback - state is INITIALIZING\n");
      #endif
      StoreLoopData();
      // advance to NOMINAL
      callbackResults = theSolver->AdvanceNestedState(vars);
      nState          = theSolver->GetNestedState();
   }
   if (nState != Solver::NOMINAL)
      throw CommandException(
         "Optimize::ExecuteCallback - error in optimizer state");

   // this call should just advance the state to CALCULATING
   #ifdef DEBUG_CALLBACK
      MessageInterface::ShowMessage(
         "Optimize::ExecuteCallback - state is NOMINAL\n");
   #endif
   callbackResults = theSolver->AdvanceNestedState(vars);
   ResetLoopData();
   
   try
   {
      branchExecuting = true;
      while (branchExecuting) 
      {
         if (!ExecuteBranch())
            throw CommandException("Optimize: ERROR executing branch");
      } 
   }
   catch (BaseException &be)
   {
      // Use exception to remove Visual C++ warning
      be.GetMessageType();
      //Just rethrow since message is getting too long (loj: 2007.05.11)
      //std::string errorStr = "Optimize:ExecuteCallback: ERROR - " +
      //   be.GetFullMessage() + "\n";
      //throw CommandException(errorStr);
      throw;
   }
   
   // this call should just advance the state back to NOMINAL
   // and return results
   #ifdef DEBUG_CALLBACK
      MessageInterface::ShowMessage(
         "Optimize::ExecuteCallback - about to CALCULATE\n");
      MessageInterface::ShowMessage(
         "Optimize::ExecuteCallback - calling AdvanceNestedState with vars:\n");
         for (Integer ii=0;ii<(Integer)vars.size();ii++)
            MessageInterface::ShowMessage("--- vars[%d] = %.15f\n",
               ii, vars.at(ii));
   #endif
   callbackResults = theSolver->AdvanceNestedState(vars); 
   #ifdef DEBUG_CALLBACK
      MessageInterface::ShowMessage("after CALCULATING, data from callback are: \n");
      for (Integer ii = 0; ii < (Integer) callbackResults.size(); ii++)
         MessageInterface::ShowMessage("(%d) -> %s\n",
            ii, (callbackResults.at(ii)).c_str());
   #endif
      
   callbackExecuting = false;
   return true;
}

//------------------------------------------------------------------------------
// bool PutCallbackData(std::string &data)
//------------------------------------------------------------------------------
bool Optimize::PutCallbackData(std::string &data)
{
   callbackData = data;
   #ifdef DEBUG_CALLBACK
      MessageInterface::ShowMessage("Entering Optimize::PutCallbackData\n");
      MessageInterface::ShowMessage("-- callback data are: %s\n", data.c_str());
   #endif
   return true;
}

//------------------------------------------------------------------------------
// std::string GetCallbackResults()
//------------------------------------------------------------------------------
std::string Optimize::GetCallbackResults()
{
   std::string allResults;

   for (Integer i=0; i < (Integer) callbackResults.size(); ++i)
   {
      allResults += callbackResults.at(i) + ";";
   }
   
   #ifdef DEBUG_CALLBACK
      MessageInterface::ShowMessage("Exiting Optimize::GetCallbackResults\n");
      MessageInterface::ShowMessage("-- callback results are: %s\n",
      allResults.c_str());
   #endif
   return allResults;
}


//------------------------------------------------------------------------------
// bool RunInternalSolver(Solver::SolverState state)
//------------------------------------------------------------------------------
bool Optimize::RunInternalSolver(Solver::SolverState state)
{
   #ifdef DEBUG_OPTIMIZE_EXEC
   MessageInterface::ShowMessage
      ("Optimize::RunInternalSolver() entered, branch %s executing\n",
       branchExecuting ? "is" : "is not");
   #endif
   
   bool retval = true;
   
   if (branchExecuting)
   {
      retval = ExecuteBranch();
      if (!branchExecuting)
      {
         if (state == Solver::FINISHED)
         {
//            PenDownSubscribers();
//            LightenSubscribers(1);
            commandComplete = true;
         }
         else
         {
//            PenUpSubscribers();
         }
      }
   }
   else
   {
      #ifdef DEBUG_OPTIMIZE_EXEC
      MessageInterface::ShowMessage
         ("Executing the Internal Optimizer %s\n", theSolver->GetName().c_str());
      #endif
      
      GmatCommand *currentCmd;
      // Set GMAT run state to SOLVING (LOJ: 2010.01.12)
      publisher->SetRunState(Gmat::SOLVING);
      
      switch (startMode)
      {
      case RUN_INITIAL_GUESS:
         #ifdef DEBUG_OPTIMIZE_EXEC
         MessageInterface::ShowMessage
            ("Running as RUN_INITIAL_GUESS, specialState = %d\n", specialState);
         #endif
         switch (specialState) 
         {
            case Solver::INITIALIZING:
               #ifdef DEBUG_STATE_TRANSITIONS
               MessageInterface::ShowMessage("Mode: %d  State: %d  Special state: INITIALIZING (%d)",
                     startMode, state, specialState);
               #endif
               currentCmd = branch[0];
               optimizerConverged = false;
               while (currentCmd != this)
               {
                  std::string type = currentCmd->GetTypeName();
                  if ((type == "Optimize") || (type == "Vary") ||
                      (type == "Minimize") || (type == "NonlinearConstraint"))
                     currentCmd->Execute();
                  currentCmd = currentCmd->GetNext();
               }
               StoreLoopData();
               specialState = Solver::NOMINAL;
//               specialState = Solver::RUNSPECIAL;

               #ifdef DEBUG_STATE_TRANSITIONS
               MessageInterface::ShowMessage(" -> Mode: %d  Special state: "
                     "(%d)\n", startMode, specialState);
               #endif
               break;

            case Solver::NOMINAL:
               #ifdef DEBUG_STATE_TRANSITIONS
               MessageInterface::ShowMessage("Mode: %d  State: %d  Special "
                     "state: NOMINAL (%d)", startMode, state, specialState);
               #endif
               // Execute the nominal sequence
               if (!commandComplete) {
                  branchExecuting = true;
                  ResetLoopData();
               }
               specialState = Solver::RUNSPECIAL;
               #ifdef DEBUG_STATE_TRANSITIONS
               MessageInterface::ShowMessage(" -> Mode: %d  Special state: "
                     "(%d)\n", startMode, specialState);
               #endif
               break;

            case Solver::RUNSPECIAL:
               #ifdef DEBUG_STATE_TRANSITIONS
               MessageInterface::ShowMessage("Mode: %d  State: %d  Special "
                     "state: RUNSPECIAL (%d)", startMode, state, specialState);
               #endif
               #ifdef DEBUG_OPTIMIZE_EXEC
               MessageInterface::ShowMessage
                  ("Optimize::Execute - internal solver in RUNSPECIAL state\n");
               #endif
               // Run once more to publish the data from the converged state
               if (!commandComplete)
               {
                  #ifdef DEBUG_OPTIMIZE_EXEC
                  MessageInterface::ShowMessage
                     ("Optimize::Execute - internal solver setting publisher with SOLVEDPASS\n");
                  #endif
                  ResetLoopData();
                  branchExecuting = true;
                  //publisher->SetRunState(Gmat::SOLVEDPASS);
               }
               theSolver->Finalize();
               specialState = Solver::FINISHED;

               // Final clean-up
               optimizerConverged = true;
               #ifdef DEBUG_STATE_TRANSITIONS
               MessageInterface::ShowMessage(" -> Mode: %d  Special state: "
                     "(%d)\n", startMode, specialState);
               #endif
               break;

            case Solver::FINISHED:
               #ifdef DEBUG_STATE_TRANSITIONS
               MessageInterface::ShowMessage("Mode: %d  State: %d  Special "
                     "state: FINISHED (%d)", startMode, state, specialState);
               #endif
               commandComplete = true;
               #ifdef DEBUG_OPTIMIZE_EXEC
               MessageInterface::ShowMessage
                  ("Optimize::Execute - internal solver in FINISHED state\n");
               #endif
               optimizerConverged = true;
//               branchExecuting = true;

               specialState = Solver::INITIALIZING;
               #ifdef DEBUG_STATE_TRANSITIONS
               MessageInterface::ShowMessage(" -> Mode: %d  Special state: "
                     "(%d)\n", startMode, specialState);
               #endif
               break;

            default:
               break;
         }                     
         break;
         
      case RUN_SOLUTION:
         #ifdef DEBUG_OPTIMIZE_EXEC
         MessageInterface::ShowMessage
            ("Running as RUN_SOLUTION, state = %d\n", state);
         #endif
         throw SolverException
            ("Run Solution is not yet implemented for the Optimize command\n");
         break;
         
      case RUN_AND_SOLVE:
      default:
         #ifdef DEBUG_OPTIMIZE_EXEC
         MessageInterface::ShowMessage
            ("Running as RUN_AND_SOLVE or default, state = %d\n", state);
         #endif
         
         // Here is the usual state machine for the optimizer   
         switch (state) 
         {
         case Solver::INITIALIZING:
            #ifdef DEBUG_OPTIMIZE_EXEC
            MessageInterface::ShowMessage
               ("Optimize::Execute - internal solver in INITIALIZING state\n");
            #endif
            currentCmd = branch[0];
            optimizerConverged = false;
            while (currentCmd != this)  
            {
               std::string type = currentCmd->GetTypeName();
               if ((type == "Optimize") || (type == "Vary") ||
                   (type == "Minimize") || (type == "NonlinearConstraint"))
                  currentCmd->Execute();
               currentCmd = currentCmd->GetNext();
            }
            StoreLoopData();
//            GetActiveSubscribers();
//            SetSubscriberBreakpoint();
            break;
            
         case Solver::NOMINAL:
            #ifdef DEBUG_OPTIMIZE_EXEC
            MessageInterface::ShowMessage
               ("Optimize::Execute - internal solver in NOMINAL state\n");
            #endif
            // Execute the nominal sequence
            if (!commandComplete)
            {
               branchExecuting = true;
//               ApplySubscriberBreakpoint();
//               PenDownSubscribers();
//               LightenSubscribers(1);
               ResetLoopData();
            }
            break;
            
         case Solver::PERTURBING:
            #ifdef DEBUG_OPTIMIZE_EXEC
            MessageInterface::ShowMessage
               ("Optimize::Execute - internal solver in PERTURBING state\n");
            #endif
            branchExecuting = true;
//            ApplySubscriberBreakpoint();
//            PenDownSubscribers();
//            LightenSubscribers(4);
            ResetLoopData();
            break;
            
         case Solver::CALCULATING:
            #ifdef DEBUG_OPTIMIZE_EXEC
            MessageInterface::ShowMessage
               ("Optimize::Execute - internal solver in CALCULATING state\n");
            #endif
            break;
            
         case Solver::CHECKINGRUN:
            #ifdef DEBUG_OPTIMIZE_EXEC
            MessageInterface::ShowMessage
               ("Optimize::Execute - internal solver in CHECKINGRUN state\n");
            #endif
            break;
            
         case Solver::FINISHED:
            #ifdef DEBUG_OPTIMIZE_EXEC
            MessageInterface::ShowMessage
               ("Optimize::Execute - internal solver in FINISHED state\n");
            #endif
            // Final clean-up
            // Commented out to run once more below (LOJ: 2010.01.08)
            //commandComplete = true;
            optimizerConverged = true;
            
            // Run once more to publish the data from the converged state
            if (!commandComplete)
            {
               #ifdef DEBUG_OPTIMIZE_EXEC
               MessageInterface::ShowMessage
                  ("Optimize::Execute - internal solver setting publisher with SOLVEDPASS\n");
               #endif
               ResetLoopData();
               branchExecuting = true;
//               ApplySubscriberBreakpoint();
//               PenDownSubscribers();
//               LightenSubscribers(1);
               publisher->SetRunState(Gmat::SOLVEDPASS);
            }
            break;
            
         default:
            MessageInterface::ShowMessage("Optimize::invalid state %d\n", state);
            branchExecuting = false;
            commandComplete  = true;
            optimizerConverged = true;
         }
         break;
      }
   }
   
   #ifdef DEBUG_OPTIMIZE_EXEC
   MessageInterface::ShowMessage
      ("Optimize::RunInternalSolver() returning %d, branch %s executing\n", retval,
       branchExecuting ? "is" : "is not");
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// bool RunExternalSolver(Solver::SolverState state)
//------------------------------------------------------------------------------
bool Optimize::RunExternalSolver(Solver::SolverState state)
{
   #ifdef DEBUG_OPTIMIZE_EXEC
   MessageInterface::ShowMessage
      ("Optimize::RunExternalSolver() entered, branch %s executing\n",
       branchExecuting ? "is" : "is not");
   #endif
   
   bool retval = true;
   
   if (branchExecuting)
   {
      retval = ExecuteBranch();
      if (!branchExecuting && (state == Solver::FINISHED))
      {
         commandComplete = true;
      }  
   }
   else
   {
      #ifdef DEBUG_OPTIMIZE_EXEC
      MessageInterface::ShowMessage
         ("Executing the External Optimizer %s\n", theSolver->GetName().c_str());
      #endif
      
      GmatCommand *currentCmd;
      publisher->SetRunState(Gmat::SOLVING);
      
      switch (state) 
      {
      case Solver::INITIALIZING:
         // Finalize initialization of the optimizer data
         #ifdef DEBUG_OPTIMIZE_EXEC
         MessageInterface::ShowMessage
            ("Optimize::Execute - external solver in INITIALIZING state\n");
         #endif
         currentCmd = branch[0];
         optimizerConverged = false;
         while (currentCmd != this)  
         {
            std::string type = currentCmd->GetTypeName();
            if ((type == "Optimize") || (type == "Vary") ||
                (type == "Minimize") || (type == "NonlinearConstraint"))
               currentCmd->Execute();
            currentCmd = currentCmd->GetNext();
         }
         StoreLoopData();
         break;
         
      case Solver::RUNEXTERNAL:
         #ifdef DEBUG_OPTIMIZE_EXEC
         MessageInterface::ShowMessage
            ("Optimize::Execute - external solver in RUNEXTERNAL state\n");
         #endif
         break;
         
      case Solver::FINISHED:
         // Final clean-up
         // Commented out to run once more below (LOJ: 2010.01.08)
         //commandComplete = true;
         #ifdef DEBUG_OPTIMIZE_EXEC
         MessageInterface::ShowMessage
            ("Optimize::Execute - external solver in FINISHED state\n");
         #endif
         optimizerConverged = true;
         
         // Run once more to publish the data from the converged state
         if (!commandComplete)
         {
            #ifdef DEBUG_OPTIMIZE_EXEC
            MessageInterface::ShowMessage
               ("Optimize::Execute - external solver setting publisher with SOLVEDPASS\n");
            #endif
            ResetLoopData();
            branchExecuting = true;
            publisher->SetRunState(Gmat::SOLVEDPASS);
         }
         break;
         
      default:
         MessageInterface::ShowMessage("Optimize::invalid state %d\n",state);
      }
   }
   
   #ifdef DEBUG_OPTIMIZE_EXEC
   MessageInterface::ShowMessage
      ("Optimize::RunExternalSolver() returning %d, branch %s executing\n", retval,
       branchExecuting ? "is" : "is not");
   #endif
   
   return retval;
}

