//$Id$
//------------------------------------------------------------------------------
//                                Optimize 
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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

//Added __USE_EXTERNAL_OPTIMIZER__ so that header will not be compiled
#ifdef __USE_EXTERNAL_OPTIMIZER__
#include "ExternalOptimizer.hpp"
#endif

#if defined __USE_MATLAB__
   #include "MatlabInterface.hpp"
   #include "GmatInterface.hpp"
   
   static GmatInterface *gmatInt = GmatInterface::Instance();
#endif

//#define DEBUG_OPTIMIZER_PARSING
//#define DEBUG_OPTIMIZE_COMMANDS
//#define DEBUG_CALLBACK
//#define DEBUG_OPTIMIZE_CONSTRUCTION
//#define DEBUG_OPTIMIZE_INIT

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
   SolverBranchCommand ("Optimize"),
   optimizerName       (""),
   optimizer           (NULL),
   optimizerConverged  (false),
   optimizerInDebugMode(false)
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
   SolverBranchCommand  (o),
   optimizerName        (o.optimizerName),
   optimizer            (NULL),
   optimizerConverged   (false),
   optimizerInDebugMode (o.optimizerInDebugMode)
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

   optimizerName        = o.optimizerName;
   optimizer            = NULL;
   optimizerConverged   = false;
   optimizerInDebugMode = o.optimizerInDebugMode;
   localStore.clear();

   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
Optimize::~Optimize()
{
   if (optimizer)
      delete optimizer;
}


//------------------------------------------------------------------------------
// Append
//------------------------------------------------------------------------------
bool Optimize::Append(GmatCommand *cmd)
{
   #ifdef DEBUG_OPTIMIZER_PARSING
       MessageInterface::ShowMessage("\nOptimize::Append received \"%s\" command",
                                     cmd->GetTypeName().c_str());
   #endif
    
   if (!SolverBranchCommand::Append(cmd))
        return false;
    
   // If at the end of a optimizer branch, point that end back to this comand.
   if (cmd->GetTypeName() == "EndOptimize") 
   {
      if ((nestLevel == 0) && (branchToFill != -1))  
      {
         cmd->Append(this);
         // Optimizer loop is complete; -1 pops to the next higher sequence.
         branchToFill = -1;
         #ifdef DEBUG_OPTIMIZER_PARSING
             MessageInterface::ShowMessage("\nOptimize::Append closing \"%s\"",
                                           generatingString.c_str());
         #endif
      }
      else
         --nestLevel;
   }

   // If it's a nested optimizer branch, add to the nest level.
   // 2006.09.13 wcs - as of today, nested optimizers are not allowed
   if (cmd->GetTypeName() == "Optimize")
      ++nestLevel;

   #ifdef DEBUG_OPTIMIZER_PARSING
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
   if (mode == Gmat::NO_COMMENTS)
   {
      generatingString = "Optimize " + optimizerName + ";";
      return generatingString;
   }
   
   generatingString = prefix + "Optimize " + optimizerName + ";";
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
      if (optimizerName == oldName)
         optimizerName = newName;
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
      return optimizerName;
    
   return SolverBranchCommand::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// SetStringParameter
//------------------------------------------------------------------------------
bool Optimize::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == OPTIMIZER_NAME) 
   {
      optimizerName = value;
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
      return optimizerName;
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
      optimizerName = name;
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
   
   if (objectMap->find(optimizerName) == objectMap->end()) 
   {
      std::string errorString = "Optimize command cannot find optimizer \"";
      errorString += optimizerName;
      errorString += "\"";
      throw CommandException(errorString);
   }

   // Clone the optimizer for local use
   optimizer = (Solver *)((*objectMap)[optimizerName])->Clone();
   optimizer->TakeAction("IncrementInstanceCount");
   ((*objectMap)[optimizerName])->TakeAction("IncrementInstanceCount");
   
   if (optimizer->GetStringParameter("ReportStyle") == "Debug")
      optimizerInDebugMode = true;      
    
   // Set the local copy of the optimizer on each node
   std::vector<GmatCommand*>::iterator node;
   GmatCommand *currentCmd;

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
            currentCmd->SetRefObject(optimizer, Gmat::SOLVER, optimizerName);
         currentCmd = currentCmd->GetNext();
      }
   }

   bool retval = SolverBranchCommand::Initialize();

   if (retval == true) 
   {
      // Optimizer specific initialization goes here: // wcs - why is this done twice?
      if (objectMap->find(optimizerName) == objectMap->end()) 
      {
         std::string errorString = "Optimize command cannot find optimizer \"";
         errorString += optimizerName;
         errorString += "\"";
         throw CommandException(errorString);
      }

      retval = optimizer->Initialize();
   }
   // NOTE that in the future we may have a callback to/from a non_MATLAB
   // external optimizer
   #if defined __USE_MATLAB__
      if (optimizer->IsOfType("ExternalOptimizer") &&
         (optimizer->GetStringParameter("SourceType") == "MATLAB"))
         gmatInt->RegisterCallbackServer(this);
   #endif

   return retval;
}

//------------------------------------------------------------------------------
// Execute
//------------------------------------------------------------------------------
bool Optimize::Execute()
{
   // We need to re-initialize since only one MatlabEngine is running per GMAT
   // session. This will allow to run back to back optimization.
   if (!commandExecuting)
      Initialize();
   
   bool retval = true;
   
   // Drive through the state machine.
   Solver::SolverState state = optimizer->GetState();
   
   #ifdef DEBUG_OPTIMIZE_COMMANDS
      MessageInterface::ShowMessage("Optimize::Execute(%c%c%d)\n",
         (commandExecuting?'Y':'N'),
         (commandComplete?'Y':'N'),
         state);
   #endif

   // Attempt to reset if recalled   
   if (commandComplete)
   {
      commandComplete  = false;
      commandExecuting = false;
   }  

   if (!commandExecuting) 
   {
      #ifdef DEBUG_OPTIMIZE_COMMANDS
         MessageInterface::ShowMessage(
         "Entered Optimizer while command is not executing\n");
      #endif

      FreeLoopData();
      StoreLoopData();

      retval = SolverBranchCommand::Execute();

      #ifdef DEBUG_OPTIMIZE_COMMANDS
         MessageInterface::ShowMessage("Resetting the Optimizer\n");
      #endif

      optimizer->TakeAction("Reset");
      state = optimizer->GetState();
      
   }
   /*  Revisit this if/when we add non-external optimizers !!!!!!!!!!!!!!!!!!!!!
    * This should be commented out for the fmincon optimizer 
    * (branch should be executed from ExecuteCallback)
   */
/*
   if (branchExecuting)
   {
      //retval = ExecuteBranch(); */
      //callbackData = "10.0 20.0"; // *************** TEMPORARY testing *********
      //ExecuteCallback();  // *************** TEMPORARY testing *****************
     /* if (!branchExecuting && (state == Solver::FINISHED))
      {
         commandComplete = true;
      }  
   }
   else
   {
*/
      GmatCommand *currentCmd;
      publisher->SetRunState(Gmat::SOLVING);
      
      switch (state) 
      {
         case Solver::INITIALIZING:
            // Finalize initialization of the optimizer data
            #ifdef DEBUG_OPTIMIZE_COMMANDS
               MessageInterface::ShowMessage(
               "Optimize::Execute - solver in INITIALIZING state\n");
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
            #ifdef DEBUG_OPTIMIZE_COMMANDS
               MessageInterface::ShowMessage(
               "Optimize::Execute - solver in RUNEXTERNAL state\n");
            #endif
//            // Execute the nominal sequence
//            if (!commandComplete) 
//            {
//               branchExecuting = true;
//               ResetLoopData();
//            }
            break;
               
          /*     
         case Solver::NOMINAL:
            // Execute the nominal sequence
            if (!commandComplete) 
            {
               branchExecuting = true;
               ResetLoopData();
            }
            break;
               
         case Solver::CHECKINGRUN:
            // Check for convergence; this is done in the optimizer state
            // machine, so this case is a NoOp for the Optimize command
            break;
   
         case Solver::PERTURBING:
            branchExecuting = true;
            ResetLoopData();
            break;
               
         case Solver::CALCULATING:
            // Calculate the next set of variables to use; this is performed in
            // the optimizer -- nothing to be done here
            break;
         */      
         case Solver::FINISHED:
            // Final clean-up
            commandComplete = true;
            #ifdef DEBUG_OPTIMIZE_COMMANDS
               MessageInterface::ShowMessage(
               "Optimize::Execute - solver in FINISHED state\n");
            #endif
            optimizerConverged = true;

            // Run once more to publish the data from the converged state
            if (!commandComplete)
            {
               ResetLoopData();
               branchExecuting = true;
               publisher->SetRunState(Gmat::SOLVEDPASS);
            }
            break;
          /*     
         case Solver::ITERATING:     // Intentional fall-through
         */
         default:
         MessageInterface::ShowMessage("Optimize:: invalid state %d\n");
            //throw CommandException(
            //   "Invalid state in the Optimize state machine");

      }
   //}

   if (!branchExecuting)
   {
      #ifdef DEBUG_OPTIMIZE_COMMANDS
         MessageInterface::ShowMessage(
            "Optimize::Execute - about to advance the state\n");
      #endif
      optimizer->AdvanceState();

      if (optimizer->GetState() == Solver::FINISHED) 
      {
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
      optimizer->SetDebugString(dbgData);
   }
   BuildCommandSummary(true);
   return retval;
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void Optimize::RunComplete()
{
   if (optimizer != NULL)
      optimizer->Finalize();
   
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
      (!(optimizer->IsOfType("ExternalOptimizer"))) || 
      (((ExternalOptimizer*)optimizer)->GetStringParameter("SourceType")
      != "MATLAB"))
   {
      throw CommandException(
      "Optimize::ExecuteCallback not yet implemented for non_MATLAB optimizers");
   }
   #endif
   
   #ifndef __USE_MATLAB__ 
      throw CommandException("Optimize: ERROR - MATLAB required for Callback");
   #endif
   #ifdef __USE_MATLAB__
   if (!MatlabInterface::IsOpen())
      throw CommandException(
         "Optimize:: ERROR - Matlab Interface not yet open");
   
   callbackExecuting = true;
   // ask Matlab for the value of X
   Integer     n = optimizer->GetIntegerParameter(
                   optimizer->GetParameterID("NumberOfVariables"));
   
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
   Solver::SolverState nState = optimizer->GetNestedState(); 
   if (nState == Solver::INITIALIZING)
   {
      #ifdef DEBUG_CALLBACK
      MessageInterface::ShowMessage(
         "Optimize::ExecuteCallback - state is INITIALIZING\n");
      #endif
      StoreLoopData();
      // advance to NOMINAL
      callbackResults = optimizer->AdvanceNestedState(vars);
      nState          = optimizer->GetNestedState();
   }
   if (nState != Solver::NOMINAL)
      throw CommandException(
         "Optimize::ExecuteCallback - error in optimizer state");

   // this call should just advance the state to CALCULATING
   #ifdef DEBUG_CALLBACK
      MessageInterface::ShowMessage(
         "Optimize::ExecuteCallback - state is NOMINAL\n");
   #endif
   callbackResults = optimizer->AdvanceNestedState(vars);
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
   callbackResults = optimizer->AdvanceNestedState(vars); 
   #ifdef DEBUG_CALLBACK
      MessageInterface::ShowMessage("after CALCULATING, data from callback are: \n");
      for (Integer ii = 0; ii < (Integer) callbackResults.size(); ii++)
         MessageInterface::ShowMessage("(%d) -> %s\n",
            ii, (callbackResults.at(ii)).c_str());
   #endif
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
