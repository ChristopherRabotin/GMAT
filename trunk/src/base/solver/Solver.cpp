//$Header$
//------------------------------------------------------------------------------
//                                Solver
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/29
//
/**
 * Base class for Targeters, Optimizers, and other parametric scanning tools. 
 */
//------------------------------------------------------------------------------


#include <sstream>
#include "Solver.hpp"
#include "MessageInterface.hpp"
#include "FileManager.hpp"

//#define DEBUG_SOLVER_INIT
//#define DEBUG_SOLVER_CALC

//---------------------------------
// static data
//---------------------------------

const std::string
Solver::PARAMETER_TEXT[SolverParamCount - GmatBaseParamCount] =
{
   "ShowProgress",
   "ReportStyle",
   "TargeterTextFile", // should be "SolverTextFile",
   "Variables",
   "MaximumIterations",
   "NumberOfVariables",
   "RegisteredVariables",
   "RegisteredComponents",
   "AllowScaleSetting",
   "AllowRangeSettings",
   "AllowStepsizeSetting",
   "AllowVariablePertSetting",
};

const Gmat::ParameterType
Solver::PARAMETER_TYPE[SolverParamCount - GmatBaseParamCount] =
{
   Gmat::BOOLEAN_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::INTEGER_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,
   Gmat::BOOLEAN_TYPE,
};

const std::string    
Solver::STYLE_TEXT[MaxStyle - NORMAL_STYLE] =
{
   "Normal",
   "Concise",
   "Verbose",
   "Debug"
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Solver(const std::string &type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Core constructor for Solver objects.
 * 
 * @param type Text description of the type of solver constructed 
 *             (e.g. "DifferentialCorrector")
 * @param name The solver's name
 */
//------------------------------------------------------------------------------
Solver::Solver(const std::string &type, const std::string &name) :
   GmatBase                (Gmat::SOLVER, type, name),
   isInternal              (true),   
   currentState            (INITIALIZING),
   nestedState             (INITIALIZING),
   textFileMode            ("Normal"),
   showProgress            (true),
   progressStyle           (NORMAL_STYLE),
   debugString             (""),
   variableCount           (0),
   //variable                (NULL),
   iterationsTaken         (0),
   maxIterations           (25),
   //perturbation            (NULL),
   //variableMinimum         (NULL),
   //variableMaximum         (NULL),
   //variableMaximumStep     (NULL),
   pertNumber              (-999), // is this right?
   initialized             (false),
   instanceNumber          (0),    // 0 indicates 1st instance w/ this name
   registeredVariableCount (0),
   registeredComponentCount(0),
   AllowScaleFactors       (true),
   AllowRangeLimits        (true),
   AllowStepsizeLimit      (true),
   AllowIndependentPerts   (true)
{
   objectTypes.push_back(Gmat::SOLVER);
   objectTypeNames.push_back("Solver");
   //solverTextFile = "solver_";
   solverTextFile  = type;
   solverTextFile += instanceName;
   solverTextFile += ".data";
}


//------------------------------------------------------------------------------
//  Solver(std::string type, std::string name)
//------------------------------------------------------------------------------
/**
 *  Solver destructor.
 */
//------------------------------------------------------------------------------
Solver::~Solver()
{
   // Added per Linda, 2/7/07
   if (textFile.is_open())
      textFile.close();
}


//------------------------------------------------------------------------------
//  Solver(const Solver &sol)
//------------------------------------------------------------------------------
/**
 * Copy constructor for Solver objects.
 * 
 * @param sol The solver that is copied
 */
//------------------------------------------------------------------------------
Solver::Solver(const Solver &sol) :
   GmatBase                (sol),
   isInternal              (sol.isInternal),   
   currentState            (sol.currentState),
   nestedState             (sol.currentState),
   textFileMode            (sol.textFileMode),
   showProgress            (sol.showProgress),
   progressStyle           (sol.progressStyle),
   debugString             (sol.debugString),
   variableCount           (sol.variableCount),
   //variable                (NULL),
   iterationsTaken         (0),
   maxIterations           (sol.maxIterations),
   //perturbation            (NULL),
   //variableMinimum         (NULL),
   //variableMaximum         (NULL),
   //variableMaximumStep     (NULL),
   pertNumber              (sol.pertNumber),
   initialized             (false),
   solverTextFile          (sol.solverTextFile),
   instanceNumber          (sol.instanceNumber),
   registeredVariableCount (sol.registeredVariableCount),
   registeredComponentCount(sol.registeredComponentCount),
   AllowRangeLimits        (sol.AllowRangeLimits),
   AllowStepsizeLimit      (sol.AllowStepsizeLimit),
   AllowIndependentPerts   (sol.AllowIndependentPerts)
{
   #ifdef DEBUG_SOLVER_INIT
      MessageInterface::ShowMessage(
         "In Solver::Solver (copy constructor)\n");
   #endif
   variableNames.clear();
   //variable.clear();
   //perturbation.clear();
   //variableMinimum.clear();
   //variableMaximum.clear();
   //variableMaximumStep.clear();
}


//------------------------------------------------------------------------------
//  Solver& operator=(const Solver &sol)
//------------------------------------------------------------------------------
/**
 * Assignment operator for solvers
 * 
 * @return this Solver, set to the same parameters as the input solver.
 */
//------------------------------------------------------------------------------
Solver& Solver::operator=(const Solver &sol)
{
   if (&sol == this)
      return *this;

   registeredVariableCount  = sol.registeredVariableCount;
   registeredComponentCount = sol.registeredComponentCount;
   variableCount            = sol.variableCount;
   iterationsTaken          = 0;
   maxIterations            = sol.maxIterations;
   initialized              = false;
   solverTextFile           = sol.solverTextFile;
   
   variableNames.clear();
   //variable.clear();
   //perturbation.clear();
   //variableMinimum.clear();
   //variableMaximum.clear();
   //variableMaximumStep.clear();
   
   currentState          = sol.currentState;
   nestedState           = sol.currentState;
   textFileMode          = sol.textFileMode;
   showProgress          = sol.showProgress;
   progressStyle         = sol.progressStyle;
   debugString           = sol.debugString;
   instanceNumber        = sol.instanceNumber;
   pertNumber            = sol.pertNumber;

   return *this;
}

//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Derived classes implement this method to set object pointers and validate
 * internal data structures.
 * 
 *  @return true on success, false (or throws a SolverException) on failure
 */
//------------------------------------------------------------------------------
bool Solver::Initialize()
{
   // Setup the variable data structures
   Integer localVariableCount = variableNames.size();
   
   #ifdef DEBUG_SOLVER_INIT
      MessageInterface::ShowMessage(
         "In Solver::Initialize with localVariableCount = %d\n", 
         localVariableCount);
   #endif

   try
   {
   #ifdef DEBUG_SOLVER_INIT
      MessageInterface::ShowMessage(
         "In Solver::Initialize - about to set default values\n");
   #endif
      for (Integer i = 0; i < localVariableCount; ++i)
      {
         variable.push_back(0.0);
         variableMinimum.push_back(-9.999e300);
         variableMaximum.push_back(9.999e300);
         variableMaximumStep.push_back(9.999e300);
         perturbation.push_back(1.0e-04);
         pertDirection.push_back(1.0);
      }
   }
   catch(const std::exception &re)
   {
      throw SolverException("Range error initializing Solver object %s\n",
            instanceName.c_str());
   }

   #ifdef DEBUG_SOLVER_INIT
      MessageInterface::ShowMessage(
         "In Solver::Initialize - about to prepare text file for output\n");
   #endif
   // Prepare the text file for output
   if (solverTextFile != "")
   {
      // Added per Linda, 2/7/07
      FileManager *fm;
      fm = FileManager::Instance();
      std::string outPath = fm->GetFullPathname(FileManager::OUTPUT_PATH);
      std::string fullSolverTextFile = outPath + solverTextFile;
   
      if (textFile.is_open())
         textFile.close();
      
      if (instanceNumber == 1)
         textFile.open(fullSolverTextFile.c_str());
      else
         textFile.open(fullSolverTextFile.c_str(), std::ios::app);
      if (!textFile.is_open())
         throw SolverException("Error opening targeter text file " +
                               solverTextFile);
      textFile.precision(16);
      WriteToTextFile();
   }
   initialized = true; 
   iterationsTaken = 0;
   #ifdef DEBUG_SOLVER_INIT
      MessageInterface::ShowMessage(
         "In Solver::Initialize completed\n");
   #endif
   return true;
}


bool Solver::Finalize()
{
   // Close the solver text file
   if (textFile.is_open())
   {
      textFile.flush();
      textFile.close();
   }
   return true;
}


//------------------------------------------------------------------------------
//  Integer SetSolverVariables(Real *data, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Derived classes use this method to pass in parameter data specific to
 * the algorithm implemented.
 * 
 * @param <data> An array of data appropriate to the variables used in the
 *               algorithm.
 * @param <name> A label for the data parameter.  Defaults to the empty
 *               string.
 * 
 * @return The ID used for the variable.
 */
//------------------------------------------------------------------------------
Integer Solver::SetSolverVariables(Real *data, const std::string &name)
{
   if (variableNames[variableCount] != name)
      throw SolverException("Mismatch between parsed and configured variable");

   //variable[variableCount] = data[0];
   //perturbation[variableCount] = data[1];
   try
   {
      variable.at(variableCount) = data[0];
      perturbation.at(variableCount) = data[1];
   }
   catch(const std::exception &re)
   {
      throw SolverException(
              "Range error setting variable or perturbation in "
              "SetSolverVariables\n");
   }
   // Sanity check min and max
   if (data[2] >= data[3])
   {
      std::stringstream errMsg;
      errMsg << "Minimum allowed variable value (received " << data[2]
             << ") must be less than maximum (received " << data[3] << ")";
      throw SolverException(errMsg.str());
   }
   if (data[4] <= 0.0)
   {
      std::stringstream errMsg;
      errMsg << "Largest allowed step must be positive! (received "
             << data[4] << ")";
      throw SolverException(errMsg.str());
   }

   //variableMinimum[variableCount] = data[2];
   //variableMaximum[variableCount] = data[3];
   //variableMaximumStep[variableCount] = data[4];
   try
   {
      variableMinimum.at(variableCount)           = data[2];
      variableMaximum.at(variableCount)           = data[3];
      variableMaximumStep.at(variableCount)       = data[4];
   }
   catch(const std::exception &re)
   {
      throw SolverException(
            "Range error setting variable min/max in SetSolverVariables\n");
   }
   
   ++variableCount;

   return variableCount-1;
}


//------------------------------------------------------------------------------
//  Real GetSolverVariable(Integer id)
//------------------------------------------------------------------------------
/**
 * Interface used to access Variable values.
 * 
 * @param <id> The ID used for the variable.
 * 
 * @return The value used for this variable
 */
//------------------------------------------------------------------------------
Real Solver::GetSolverVariable(Integer id)
{
   if (id >= variableCount)
      throw SolverException(
         "Solver member requested a parameter outside the range "
         "of the configured variables.");

   //return variable[id];
   return variable.at(id); 
}

//------------------------------------------------------------------------------
//  SolverState GetState()
//------------------------------------------------------------------------------
/**
 * Determine the state-machine state of this instance of the Solver.
 *
 * @return current state 
 */
//------------------------------------------------------------------------------
Solver::SolverState Solver::GetState()
{
   return currentState;
}

//------------------------------------------------------------------------------
//  SolverState GetNestedState()
//------------------------------------------------------------------------------
/**
 * Determine the state-machine nested state of this instance of the Solver.
 *
 * @return nested State
 */
//------------------------------------------------------------------------------
Solver::SolverState Solver::GetNestedState()
{
   return nestedState;
}

//------------------------------------------------------------------------------
//  Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
/**
 * The method used to iterate until a solution is found.  Derived classes 
 * use this method to implement their solution technique.
 * 
 * @return solver state at the end of the process.
 */
//------------------------------------------------------------------------------
Solver::SolverState Solver::AdvanceState()
{
    switch (currentState) {
        case INITIALIZING:
            CompleteInitialization();
            break;
        
        case NOMINAL:
            RunNominal();
            break;
        
        case PERTURBING:
            RunPerturbation();
            break;
        
        case ITERATING:
            RunIteration();
            break;
        
        case CALCULATING:
            CalculateParameters();
            break;
        
        case CHECKINGRUN:
            CheckCompletion();
            break;
        
        case RUNEXTERNAL:
            RunExternal();
            break;
        
        case FINISHED:
            RunComplete();
            break;
        
        default:
            throw SolverException("Undefined Solver state");
    };
    
    ReportProgress();
    return currentState; 
}
    
//------------------------------------------------------------------------------
//  StringArray AdvanceNestedState(std::vector<Real> vars)
//------------------------------------------------------------------------------
/**
 * The method used to iterate until a solution is found.  Derived classes 
 * must implement this method (this default method throws an exception).
 * 
 * @return TBD
 */
//------------------------------------------------------------------------------
StringArray Solver::AdvanceNestedState(std::vector<Real> vars)
{
   std::string errorStr = "AdvanceNestedState not implemented for solver "
      + instanceName;
   throw SolverException(errorStr);
}

//------------------------------------------------------------------------------
//  bool UpdateSolverGoal(Integer id, Real newValue)
//------------------------------------------------------------------------------
/**
 * Updates the targeter goals, for floating end point problems.
 * 
 * This default method just returns false.
 * 
 * @param id Id for the goal that is being reset.
 * @param newValue The new goal value.
 * 
 * @return The ID used for this variable.
 */
//------------------------------------------------------------------------------
bool Solver::UpdateSolverGoal(Integer id, Real newValue)
{
   return false;
}


// Access methods overriden from the base class

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Solver::GetParameterText(const Integer id) const
{
   if ((id >= GmatBaseParamCount) && (id < SolverParamCount))
   {
      //MessageInterface::ShowMessage("'%s':\n", 
      //   PARAMETER_TEXT[id - GmatBaseParamCount].c_str());
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   }
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer Solver::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < SolverParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Solver::GetParameterType(const Integer id) const
{
   if ((id >= GmatBaseParamCount) && (id < SolverParamCount))
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Solver::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Solver::IsParameterReadOnly(const Integer id) const
{
   if ((id == NUMBER_OF_VARIABLES) ||
       (id == variableNamesID) ||
       (id == RegisteredVariables) ||
       (id == RegisteredComponents) ||
       (id == AllowRangeSettings) ||
       (id == AllowStepsizeSetting) ||
       (id == AllowScaleSetting) ||
       (id == AllowVariablePertSetting))
      return true;

   return false;//GmatBase::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool Solver::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns an Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 */
//------------------------------------------------------------------------------
Integer Solver::GetIntegerParameter(const Integer id) const
{
   if (id == maxIterationsID)
      return maxIterations;
   if (id == NUMBER_OF_VARIABLES)
      return variableCount;
        
   return GmatBase::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
//  Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * This method sets an Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Integer value for the parameter.
 *
 * @return  The value of the parameter at the completion of the call.
 */
//------------------------------------------------------------------------------
Integer Solver::SetIntegerParameter(const Integer id,
                                    const Integer value)
{
   if (id == maxIterationsID)
   {
      if (value > 0)
         maxIterations = value;
      else
         throw SolverException(
            "The value entered for the maximum iterations on " + instanceName +
            " is not an allowed value. The allowed value is: [Integer > 0].");
      return maxIterations;
   }
   
   if (id == RegisteredVariables)
   {
      registeredVariableCount = value;
      return registeredVariableCount;
   }
   
   if (id == RegisteredComponents)
   {
      registeredComponentCount = value;
      return registeredComponentCount;
   }
    
   return GmatBase::SetIntegerParameter(id, value);
}



//------------------------------------------------------------------------------
//  bool  GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Boolean parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Boolean value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
bool Solver::GetBooleanParameter(const Integer id) const
{
   if (id == ShowProgressID)
       return showProgress;
   
   if (id == AllowScaleSetting)
      return AllowScaleFactors;
    
   if (id == AllowRangeSettings)
      return AllowRangeLimits;
   
   if (id == AllowStepsizeSetting)
      return AllowStepsizeLimit;
   
   if (id == AllowVariablePertSetting)
      return AllowIndependentPerts;
   
   return GmatBase::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
//  Integer SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
/**
 * This method sets a Boolean parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> Boolean value for the parameter.
 *
 * @return  The value of the parameter at the completion of the call.
 */
//------------------------------------------------------------------------------
bool Solver::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == ShowProgressID)
   {
      showProgress = value;
      return showProgress;
   }

   return GmatBase::SetBooleanParameter(id, value);
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or throw ab=n exception if 
 *         there is no string association.
 */
std::string Solver::GetStringParameter(const Integer id) const
{
   if (id == ReportStyle)
      return textFileMode;
    if (id == solverTextFileID)
      return solverTextFile;

   return GmatBase::GetStringParameter(id);
}


//---------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Retrieve a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The string stored for this parameter, or throw ab=n exception if 
 *         there is no string association.
 */
std::string Solver::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, throw if the parameter is not stored.
 */
bool Solver::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == ReportStyle)
   {
//      std::string stylelist ;
      
      for (Integer i = NORMAL_STYLE; i < MaxStyle; ++i)
      {
//         stylelist += ", " + STYLE_TEXT[i-NORMAL_STYLE];
         
         if (value == STYLE_TEXT[i-NORMAL_STYLE])
         {
            textFileMode = value;
            progressStyle = i;
            return true;
         }
      }
      throw SolverException(
         "The value of \"" + value + "\" for field \"Report Style\""
         " on object \"" + instanceName + "\" is not an allowed value.\n"
         "The allowed values are: [Normal, Concise, Verbose, Debug].");
//         "The allowed values are: [ " + stylelist + " ]. ");
//      throw SolverException("Requested solver report style, " + value + 
//         ", is nor supported for " + typeName + " solvers.");
   }
   
   if (id == solverTextFileID) 
   {
      solverTextFile = value;
      return true;
   }

   if (id == variableNamesID) 
   {
      variableNames.push_back(value);
      return true;
   }

   return GmatBase::SetStringParameter(id, value);
}
//---------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
/**
 * Change the value of a string parameter.
 *
 * @param <id> The integer ID for the parameter.
 * @param <value> The new string for this parameter.
 *
 * @return true if the string is stored, throw if the parameter is not stored.
 */
bool Solver::SetStringParameter(const std::string &label, 
                                const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


// compiler complained again - so here they are ....
std::string Solver::GetStringParameter(const Integer id,
                                                  const Integer index) const
{
   return GmatBase::GetStringParameter(id, index);
}

bool Solver::SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index)
{
   return GmatBase::SetStringParameter(id, value, index);
}

std::string Solver::GetStringParameter(const std::string &label,
                                                  const Integer index) const
{
   return GmatBase::GetStringParameter(label, index);
}

bool Solver::SetStringParameter(const std::string &label, 
                                           const std::string &value,
                                           const Integer index)
{
   return GmatBase::SetStringParameter(label, value, index);
}

//------------------------------------------------------------------------------
//  std::string  GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 */
//------------------------------------------------------------------------------
const StringArray& Solver::GetStringArrayParameter(const Integer id) const
{
    if (id == variableNamesID)
        return variableNames;
        
        
    return GmatBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
//  void ReportProgress()
//------------------------------------------------------------------------------
/**
 * Shows the progress string to the user.
 *
 * This default version just passes the progress string to the MessageInterface.
 */
//------------------------------------------------------------------------------
void Solver::ReportProgress()
{
   if (showProgress)
   {
      MessageInterface::ShowMessage("%s\n", GetProgressString().c_str());
   }
}

//------------------------------------------------------------------------------
//  void SetDebugString(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Fills the buffer with run data for (user space) debug mode in the Solvers.
 *
 * @param <str> The data passed from the command stream.
 */
//------------------------------------------------------------------------------
void Solver::SetDebugString(const std::string &str)
{
   debugString = str;
}



//------------------------------------------------------------------------------
//  void CompleteInitialization()
//------------------------------------------------------------------------------
/**
 * Finalized the initialization process by setting the current state for the
 * state machine to the entry state for the solver.  The default method provided
 * here sets the state to the NOMINAL state.
 */
//------------------------------------------------------------------------------
void Solver::CompleteInitialization()
{
    currentState = NOMINAL;
}


//------------------------------------------------------------------------------
//  void RunNominal()
//------------------------------------------------------------------------------
/**
 * Executes a nominal run and then advances the state machine to the next state.
 * 
 * This default method just advances the state.
 */
//------------------------------------------------------------------------------
void Solver::RunNominal()
{
    currentState = (SolverState)(currentState+1);
}


//------------------------------------------------------------------------------
//  void RunPerturbation()
//------------------------------------------------------------------------------
/**
 * Executes a perturbation run and then advances the state machine to the next 
 * state.
 * 
 * This default method just advances the state.
 */
//------------------------------------------------------------------------------
void Solver::RunPerturbation()
{
    currentState = (SolverState)(currentState+1);
}


//------------------------------------------------------------------------------
//  void RunIteration()
//------------------------------------------------------------------------------
/**
 * Executes an iteration run and then advances the state machine to the next 
 * state.
 * 
 * This default method just advances the state.
 */
//------------------------------------------------------------------------------
void Solver::RunIteration()
{
    currentState = (SolverState)(currentState+1);
}


//------------------------------------------------------------------------------
//  void CalculateParameters()
//------------------------------------------------------------------------------
/**
 * Executes a Calculates parameters needed by the state machine for the next
 * nominal run, and then advances the state machine to the next state.
 * 
 * This default method just advances the state.
 */
//------------------------------------------------------------------------------
void Solver::CalculateParameters()
{
    currentState = (SolverState)(currentState+1);
}


//------------------------------------------------------------------------------
//  void CheckCompletion()
//------------------------------------------------------------------------------
/**
 * Checks to see if the Solver has converged.
 * 
 * This default method just advances the state.
 */
//------------------------------------------------------------------------------
void Solver::CheckCompletion()
{
    currentState = (SolverState)(currentState+1);
}

//------------------------------------------------------------------------------
//  void RunExternal()
//------------------------------------------------------------------------------
/**
 * Launhes an external process that drives the Solver.
 * 
 * This default method just ???? (not a clue).
 */
//------------------------------------------------------------------------------
void Solver::RunExternal()
{
   //currentState = FINISHED;  // what to do here?
   currentState = (SolverState)(currentState+1);
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
/**
 * Finalized the data at the end of a run.
 * 
 * This default method just sets the state to FINISHED.
 */
//------------------------------------------------------------------------------
void Solver::RunComplete()
{
    currentState = FINISHED;
}


//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string that is written out by solvers when showProgress is true.
 */
//------------------------------------------------------------------------------
std::string Solver::GetProgressString()
{
   return "Solver progress string not yet implemented for " + typeName;
}

//------------------------------------------------------------------------------
//  void FreeArrays()
//------------------------------------------------------------------------------
/**
 * Frees the memory used by the targeter, so it can be reused later in the 
 * sequence.  This method is also called by the destructor when the script is 
 * cleared.
 */
//------------------------------------------------------------------------------
void Solver::FreeArrays()
{
   /*
   if (textFile.is_open())
   {
      textFile.flush();
      textFile.close();
   }
   
   if (variable)
   {
      delete [] variable;
      variable = NULL;
   }

    if (perturbation)
   {
      delete [] perturbation;
      perturbation = NULL;
   }
            
   if (variableMinimum)
   {
      delete [] variableMinimum;
      variableMinimum = NULL;
   }

   if (variableMaximum)
   {
      delete [] variableMaximum;
      variableMaximum = NULL;
   }

   if (variableMaximumStep)
   {
      delete [] variableMaximumStep;
      variableMaximumStep = NULL;
   }
   */
   //variableNames.clear(); // ????
   variable.clear();
   perturbation.clear();
   variableMinimum.clear();
   variableMaximum.clear();
   variableMaximumStep.clear();
   pertDirection.clear();
}

