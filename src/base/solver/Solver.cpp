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


#include "Solver.hpp"
#include "MessageInterface.hpp"


//---------------------------------
// static data
//---------------------------------

const std::string
Solver::PARAMETER_TEXT[SolverParamCount - GmatBaseParamCount] =
{
   "ShowProgress",
};

const Gmat::ParameterType
Solver::PARAMETER_TYPE[SolverParamCount - GmatBaseParamCount] =
{
   Gmat::BOOLEAN_TYPE
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
   GmatBase        (Gmat::SOLVER, type, name),
   currentState    (INITIALIZING),
   textFileMode    ("Normal"),
   showProgress    (true)
{
   objectTypes.push_back(Gmat::SOLVER);
   objectTypeNames.push_back("Solver");
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
    GmatBase        (sol),
    currentState    (sol.currentState),
    textFileMode    (sol.textFileMode),
    showProgress    (sol.showProgress)
{
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
        
    currentState = sol.currentState;
    textFileMode = sol.textFileMode;
    showProgress = sol.showProgress;

    return *this;
}


//------------------------------------------------------------------------------
//  SolverState GetState()
//------------------------------------------------------------------------------
/**
 * Determine the state-machine state of this instance of the Solver.
 *
 * @return this Solver, set to the same parameters as the input solver.
 */
//------------------------------------------------------------------------------
Solver::SolverState Solver::GetState()
{
   return currentState;
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
      return PARAMETER_TEXT[id - GmatBaseParamCount];
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
