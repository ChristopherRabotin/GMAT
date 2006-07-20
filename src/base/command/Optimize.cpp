
#include "Optimize.hpp"

Optimize::Optimize() :
   BranchCommand      ("Optimize"),
   optimizerName       (""),
   optimizer           (NULL),
   optimizerConverged  (false),
   optimizerNameID     (parameterCount),
   OptimizerConvergedID(parameterCount+1),
   optimizerInDebugMode(false)
{
	MessageInterface::ShowMessage("Optimize: Default Constructor\n");
   parameterCount += 2;
}

Optimize::~Optimize()
{
   if (optimizer)
      delete optimizer;
}

Optimize::Optimize(const Optimize& o) :
   BranchCommand       (o),
   optimizerName        (o.optimizerName),
   optimizer            (NULL),
   optimizerConverged   (false),
   optimizerNameID      (o.optimizerNameID),
   OptimizerConvergedID (o.OptimizerConvergedID),
   optimizerInDebugMode (o.optimizerInDebugMode)
{
	parameterCount = o.parameterCount;
   localStore.clear();
}

Optimize& Optimize::operator=(const Optimize& o)
{
   if (this == &o)
      return *this;
    
   GmatCommand::operator=(o);

   optimizerName        = o.optimizerName;
   optimizer            = NULL;
   optimizerConverged   = false;
   optimizerNameID      = o.optimizerNameID;
   OptimizerConvergedID = o.OptimizerConvergedID;
   optimizerInDebugMode = o.optimizerInDebugMode;
   localStore.clear();

   return *this;
}

bool Optimize::Append(GmatCommand *cmd)
{
	MessageInterface::ShowMessage("Optimize: Append\n");
   #ifdef DEBUG_OPTIMIZER_PARSING
       MessageInterface::ShowMessage("\nOptimize::Append received \"%s\" command",
                                     cmd->GetTypeName().c_str());
   #endif
    
   if (!BranchCommand::Append(cmd))
        return false;
    
   // If at the end of a optimizer branch, point that end back to this comand.
   if (cmd->GetTypeName() == "EndOptimize") {
      if ((nestLevel == 0) && (branchToFill != -1))  {
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
   if (cmd->GetTypeName() == "Optimize")
      ++nestLevel;

   #ifdef DEBUG_OPTIMIZER_PARSING
       MessageInterface::ShowMessage("\nOptimize::Append for \"%s\" nest level = %d",
                                     generatingString.c_str(), nestLevel);
   #endif

   return true;
}

GmatBase* Optimize::Clone() const
{
   return (new Optimize(*this));
}

const std::string& Optimize::GetGeneratingString(Gmat::WriteMode mode,
                                               const std::string &prefix,
                                               const std::string &useName)
{
   generatingString = "Optimize " + optimizerName + ";";
   return BranchCommand::GetGeneratingString(mode, prefix, useName);
}

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

std::string Optimize::GetParameterText(const Integer id) const
{
   if (id == optimizerNameID)
      return "Optimizer";
    
   return BranchCommand::GetParameterText(id);
}

Integer Optimize::GetParameterID(const std::string &str) const
{
   if (str == "Optimizer")
      return optimizerNameID;
   if (str == "OptimizerConverged")
      return OptimizerConvergedID;
    
   return BranchCommand::GetParameterID(str);
}

Gmat::ParameterType Optimize::GetParameterType(const Integer id) const
{
   if (id == optimizerNameID)
      return Gmat::STRING_TYPE;
   if (id == OptimizerConvergedID)
      return Gmat::BOOLEAN_TYPE;
    
   return BranchCommand::GetParameterType(id);
}

std::string Optimize::GetParameterTypeString(const Integer id) const
{
   if (id == optimizerNameID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
   if (id == OptimizerConvergedID)
      return PARAM_TYPE_STRING[Gmat::BOOLEAN_TYPE];
    
   return BranchCommand::GetParameterTypeString(id);
}

std::string Optimize::GetStringParameter(const Integer id) const
{
   if (id == optimizerNameID)
      return optimizerName;
    
   return BranchCommand::GetStringParameter(id);
}

bool Optimize::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == optimizerNameID) {
      optimizerName = value;
      return true;
   }
    
   return BranchCommand::SetStringParameter(id, value);
}

bool Optimize::GetBooleanParameter(const Integer id) const
{
   if (id == OptimizerConvergedID)
      return optimizerConverged;
      
   return BranchCommand::GetBooleanParameter(id);
}

std::string Optimize::GetRefObjectName(const Gmat::ObjectType type) const
{
   if (type == Gmat::SOLVER)
      return optimizerName;
   return BranchCommand::GetRefObjectName(type);
}

bool Optimize::SetRefObjectName(const Gmat::ObjectType type,
                              const std::string &name)
{
   if (type == Gmat::SOLVER) {
      optimizerName = name;
      return true;
   }
   return BranchCommand::SetRefObjectName(type, name);
}

bool Optimize::Initialize()
{
	MessageInterface::ShowMessage("Optimize: Initialize\n");
	
	if (objectMap->find(optimizerName) == objectMap->end()) {
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
   GmatCommand *current;

   for (node = branch.begin(); node != branch.end(); ++node)
   {
      current = *node;

      #ifdef DEBUG_OPTIMIZE_COMMANDS
         Integer nodeNum = 0;
      #endif
      while ((current != NULL) && (current != this))
      {
         #ifdef DEBUG_OPTIMIZE_COMMANDS
            MessageInterface::ShowMessage(
               "   Optimize Command %d:  %s\n", ++nodeNum, 
               current->GetTypeName().c_str());       
         #endif
         if ((current->GetTypeName() == "Vary") || 
             (current->GetTypeName() == "Minimize") ||
             (current->GetTypeName() == "NonLinearConstraint"))
            current->SetRefObject(optimizer, Gmat::SOLVER, optimizerName);
         current = current->GetNext();
      }
   }

   bool retval = BranchCommand::Initialize();

   if (retval == true) {
      // Optimizer specific initialization goes here:
      if (objectMap->find(optimizerName) == objectMap->end()) {
         std::string errorString = "Optimize command cannot find optimizer \"";
         errorString += optimizerName;
         errorString += "\"";
         throw CommandException(errorString);
      }

      retval = optimizer->Initialize();
   }
        
   return retval;
}

bool Optimize::Execute()
{
	MessageInterface::ShowMessage("Optimize: Execute\n");
	
   bool retval = true;

   // Drive through the state machine.
   Solver::SolverState state = optimizer->GetState();
   
   #ifdef DEBUG_OPTIMIZE_COMMANDS
      MessageInterface::ShowMessage("OptimizeExecute(%c%c%d)\n",
         (commandExecuting?'Y':'N'),
         (commandComplete?'Y':'N'),
         state);
   #endif

   // Attempt to reset if recalled   
   if (commandComplete)
   {
      commandComplete = false;
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


      retval = BranchCommand::Execute();

      #ifdef DEBUG_OPTIMIZER
         MessageInterface::ShowMessage("Resetting the Optimizer\n");
      #endif

      optimizer->TakeAction("Reset");
      state = optimizer->GetState();
      
   }
   
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
      GmatCommand *currentCmd;
   
      switch (state) {
         case Solver::INITIALIZING:
            // Finalize initialization of the optimizer data
            currentCmd = branch[0];
            optimizerConverged = false;
            while (currentCmd != this)  {
               std::string type = currentCmd->GetTypeName();
               if ((type == "Optimize") || (type == "Vary") ||
                   (type == "Minimize") || (type == "NonLinearConstraint"))
                  currentCmd->Execute();
               currentCmd = currentCmd->GetNext();
            }
            StoreLoopData();
            break;
               
         case Solver::NOMINAL:
            // Execute the nominal sequence
            if (!commandComplete) {
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
               
         case Solver::FINISHED:
            // Final clean-up
//            commandComplete = true;
            optimizerConverged = true;

            // Run once more to publish the data from the converged state
            if (!commandComplete)
            {
               ResetLoopData();
               branchExecuting = true;
            }
            break;
               
         case Solver::ITERATING:     // Intentional fall-through
         default:
            throw CommandException(
               "Invalid state in the Optimize state machine");
      }
   }

   if (!branchExecuting)
   {
      optimizer->AdvanceState();

      if (optimizer->GetState() == Solver::FINISHED) {
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

void Optimize::StoreLoopData()
{
   // Make local copies of all of the objects that may be affected by optimize
   // loop iterations
   std::map<std::string, GmatBase *>::iterator pair = objectMap->begin();
   GmatBase *obj;
    
   // Loop through the object map, looking for objects we'll need to restore.
   while (pair != objectMap->end()) {
      obj = (*pair).second;
      // Save copies of all of the spacecraft
      if (obj->GetType() == Gmat::SPACECRAFT)
      {
         Spacecraft *orig = (Spacecraft*)(obj);
         Spacecraft *sc = new Spacecraft(*orig);
         // Handle CoordinateSystems
         if (orig->GetInternalCoordSystem() == NULL)
   			MessageInterface::ShowMessage(
      			"Internal CS is NULL on spacecraft %s prior to optimizer cloning\n",
      			orig->GetName().c_str());
			if (orig->GetRefObject(Gmat::COORDINATE_SYSTEM, "") == NULL)
   			MessageInterface::ShowMessage(
      			"Coordinate system is NULL on spacecraft %s prior to optimizer cloning\n",
    			   orig->GetName().c_str());
         sc->SetInternalCoordSystem(orig->GetInternalCoordSystem());
         sc->SetRefObject(orig->GetRefObject(Gmat::COORDINATE_SYSTEM, ""),
            Gmat::COORDINATE_SYSTEM, "");
         
         localStore.push_back(sc);
      }
      ++pair;
   }
}

void Optimize::ResetLoopData()
{
   Spacecraft *sc;
   std::string name;
    
   for (std::vector<GmatBase *>::iterator i = localStore.begin();
        i != localStore.end(); ++i) {
      name = (*i)->GetName();
      GmatBase *gb = (*objectMap)[name];
      if (gb != NULL) {
         sc = (Spacecraft*)gb;
         *sc = *((Spacecraft*)(*i));
      }
   }
}

void Optimize::FreeLoopData()
{
   GmatBase *obj;
   while (!localStore.empty()) {
      obj = *(--localStore.end());
      localStore.pop_back();
      delete obj;
   }
}
