//$Id$
//------------------------------------------------------------------------------
//                         DifferentialCorrector
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/29
//
/**
 * Implementation for the differential corrector targeter.
 *
 * @todo Rework the mathematics using Rvector code.
 */
//------------------------------------------------------------------------------


#include "DifferentialCorrector.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"     // for GmatMathUtil::Abs()
#include "MessageInterface.hpp"

#include <cmath>
#include <sstream>

//#define DEBUG_STATE_MACHINE
//#define DEBUG_DC_INIT 1
//#define DEBUG_JACOBIAN
//#define DEBUG_VARIABLES_CALCS
//#define DEBUG_TARGETING_MODES

// Turn on other debug if working on modes
#ifdef DEBUG_TARGETING_MODES
   #define DEBUG_STATE_MACHINE
#endif

//---------------------------------
// static data
//---------------------------------

const std::string
DifferentialCorrector::PARAMETER_TEXT[DifferentialCorrectorParamCount -
                                      SolverParamCount] =
{
   "Goals",
   "DerivativeMethod"
};

const Gmat::ParameterType
DifferentialCorrector::PARAMETER_TYPE[DifferentialCorrectorParamCount -
                                      SolverParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::ENUMERATION_TYPE
};


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// DifferentialCorrector(std::string name)
//------------------------------------------------------------------------------
DifferentialCorrector::DifferentialCorrector(std::string name) :
   Solver                  ("DifferentialCorrector", name),
   goalCount               (0),
   goal                    (NULL),
   tolerance               (NULL),
   nominal                 (NULL),
   achieved                (NULL),
   backAchieved            (NULL),
   jacobian                (NULL),
   inverseJacobian         (NULL),
   indx                    (NULL),
   b                       (NULL),
   derivativeMethod        ("ForwardDifference"),
   diffMode                (1),
   firstPert               (true),
   incrementPert           (true)
{
   #if DEBUG_DC_INIT
   MessageInterface::ShowMessage
      ("DifferentialCorrector::DC(constructor) entered\n");
   #endif
   objectTypeNames.push_back("BoundaryValueSolver");
   objectTypeNames.push_back("DifferentialCorrector");
   parameterCount = DifferentialCorrectorParamCount;
}


//------------------------------------------------------------------------------
// DifferentialCorrector::~DifferentialCorrector()
//------------------------------------------------------------------------------
DifferentialCorrector::~DifferentialCorrector()
{
   FreeArrays();
}


//------------------------------------------------------------------------------
// DifferentialCorrector(const DifferentialCorrector &dc) :
//------------------------------------------------------------------------------
DifferentialCorrector::DifferentialCorrector(const DifferentialCorrector &dc) :
   Solver                  (dc),
   goalCount               (dc.goalCount),
   goal                    (NULL),
   tolerance               (NULL),
   nominal                 (NULL),
   achieved                (NULL),
   backAchieved            (NULL),
   jacobian                (NULL),
   inverseJacobian         (NULL),
   indx                    (NULL),
   b                       (NULL),
   derivativeMethod        (dc.derivativeMethod),
   diffMode                (dc.diffMode),
   firstPert               (dc.firstPert),
   incrementPert           (dc.incrementPert)
{
   #if DEBUG_DC_INIT
   MessageInterface::ShowMessage
      ("DifferentialCorrector::DC(COPY constructor) entered\n");
   #endif
   goalNames.clear();

   parameterCount = dc.parameterCount;
}


//------------------------------------------------------------------------------
// operator=(const DifferentialCorrector& dc)
//------------------------------------------------------------------------------
DifferentialCorrector&
DifferentialCorrector::operator=(const DifferentialCorrector& dc)
{
   if (&dc == this)
      return *this;

   Solver::operator=(dc);

   FreeArrays();
   goalNames.clear();

   goalCount        = dc.goalCount;
   derivativeMethod = dc.derivativeMethod;
   diffMode         = dc.diffMode;
   firstPert        = dc.firstPert;
   incrementPert    = dc.incrementPert;

   return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the DifferentialCorrector.
 *
 * @return clone of the DifferentialCorrector.
 */
//------------------------------------------------------------------------------
GmatBase* DifferentialCorrector::Clone() const
{
   GmatBase *clone = new DifferentialCorrector(*this);
   return (clone);
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void DifferentialCorrector::Copy(const GmatBase* orig)
{
   operator=(*((DifferentialCorrector *)(orig)));
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
std::string DifferentialCorrector::GetParameterText(const Integer id) const
{
   if ((id >= SolverParamCount) && (id < DifferentialCorrectorParamCount))
      return PARAMETER_TEXT[id - SolverParamCount];
   return Solver::GetParameterText(id);
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
Integer DifferentialCorrector::GetParameterID(const std::string &str) const
{
   // Write deprecated message per GMAT session
   static bool writeDeprecatedMsg = true;

   // 1. This part will be removed for a future build:
   if (str == "UseCentralDifferences")
   {
      if (writeDeprecatedMsg)
      {
         MessageInterface::ShowMessage
            (deprecatedMessageFormat.c_str(), "UseCentralDifferences", GetName().c_str(),
             "DerivativeMethod");
         writeDeprecatedMsg = false;
      }
      return derivativeMethodID;
   }

   // 2. This part is kept for a future build:
   for (Integer i = SolverParamCount; i < DifferentialCorrectorParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - SolverParamCount])
         return i;
   }

   return Solver::GetParameterID(str);
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
Gmat::ParameterType DifferentialCorrector::GetParameterType(
                                              const Integer id) const
{
   if ((id >= SolverParamCount) && (id < DifferentialCorrectorParamCount))
      return PARAMETER_TYPE[id - SolverParamCount];

   return Solver::GetParameterType(id);
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
std::string DifferentialCorrector::GetParameterTypeString(
                                      const Integer id) const
{
   return Solver::PARAM_TYPE_STRING[GetParameterType(id)];
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
Integer DifferentialCorrector::GetIntegerParameter(const Integer id) const
{
   //if (id == maxIterationsID)
   //   return maxIterations;

   return Solver::GetIntegerParameter(id);
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
Integer DifferentialCorrector::SetIntegerParameter(const Integer id,
                                                   const Integer value)
{
   //if (id == maxIterationsID)
   //{
   //   if (value > 0)
   //      maxIterations = value;
   //   else
   //      MessageInterface::ShowMessage(
   //         "Iteration count for %s must be > 0; requested value was %d\n",
   //         instanceName.c_str(), value);
   //   return maxIterations;
   //}

   return Solver::SetIntegerParameter(id, value);
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
bool DifferentialCorrector::GetBooleanParameter(const Integer id) const
{
//    if (id == useCentralDifferencingID)
//        return useCentralDifferences;

    return Solver::GetBooleanParameter(id);
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
bool DifferentialCorrector::SetBooleanParameter(const Integer id,
                                                const bool value)
{
//   if (id == useCentralDifferencingID)
//   {
//      useCentralDifferences = value;
//      return useCentralDifferences;
//   }

   return Solver::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string DifferentialCorrector::GetStringParameter(const Integer id) const
{
    //if (id == solverTextFileID)
    //    return solverTextFile;

   if (id == derivativeMethodID)
      return derivativeMethod;

   return Solver::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  Integer SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets a string or string array parameter value, given the input
 * parameter ID.
 *
 * @param id    ID for the requested parameter.
 * @param value string value for the parameter.
 *
 * @return  The value of the parameter at the completion of the call.
 */
//------------------------------------------------------------------------------
bool DifferentialCorrector::SetStringParameter(const Integer id,
                                               const std::string &value)
{
   if (id == goalNamesID)
   {
      goalNames.push_back(value);
      return true;
   }

   if (id == derivativeMethodID)
   {
      bool retval = true;
      //   This is to handle deprecated value UseCentralDifferences = true
      if (value == "true")
         derivativeMethod = "CentralDifference";
      //   This is to handle deprecated value UseCentralDifferences = false
      else if (value == "false")
         derivativeMethod = "ForwardDifference";
      // Allowed values for DerivativeMethod
      else if (value == "ForwardDifference" || value == "CentralDifference" ||
               value == "BackwardDifference")
      {
         derivativeMethod = value;
         if (derivativeMethod == "ForwardDifference")
         {
            diffMode = 1;
         }
         else if(derivativeMethod == "CentralDifference")
         {
            diffMode = 0;
         }
         else if(derivativeMethod == "BackwardDifference")
         {
            diffMode = -1;
         }
      }
      //  All other values are not allowed!
      else
         retval = false;

      return retval;
   }

   return Solver::SetStringParameter(id, value);
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
const StringArray& DifferentialCorrector::GetStringArrayParameter(
                                                        const Integer id) const
{
    //if (id == variableNamesID)
    //    return variableNames;

    if (id == goalNamesID)
        return goalNames;

    return Solver::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
//  bool TakeAction(const std::string &action, const std::string &actionData)
//------------------------------------------------------------------------------
/**
 * This method performs an action on the instance.
 *
 * TakeAction is a method overridden from GmatBase.  The only action defined for
 * a DifferentialCorrector is "IncrementInstanceCount", which the Sandbox uses
 * to tell an instance if if it is a reused instance (i.e. a clone) of the
 * configured instance of the DifferentialCorrector.
 *
 * @param <action>      Text label for the action.
 * @param <actionData>  Related action data, if needed.
 *
 * @return  The value of the parameter at the completion of the call.
 */
//------------------------------------------------------------------------------
bool DifferentialCorrector::TakeAction(const std::string &action,
                                       const std::string &actionData)
{
   if (action == "ResetInstanceCount")
   {
      instanceNumber = 0;
      return true;
   }

   if (action == "IncrementInstanceCount")
   {
      ++instanceNumber;
      return true;
   }

   if (action == "Reset")
   {
      currentState = INITIALIZING;
      // initialized = false;
      // Set nominal out of range to force retarget when in a loop
      for (Integer i = 0; i < goalCount; ++i)
      {
         nominal[i] = goal[i] + 10.0 * tolerance[i];
      }
   }

   if (action == "SetMode")
   {
      currentState = INITIALIZING;
      // initialized = false;
      // Set nominal out of range to force retarget when in a loop
      for (Integer i = 0; i < goalCount; ++i)
      {
         nominal[i] = goal[i] + 10.0 * tolerance[i];
      }
   }

   return Solver::TakeAction(action, actionData);
}


//------------------------------------------------------------------------------
// Integer SetSolverResults(Real *data, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets up the data fields used for the results of an iteration.
 *
 * @param <data> An array of data appropriate to the results used in the
 *               algorithm (for instance, tolerances for targeter goals).
 * @param <name> A label for the data parameter.  Defaults to the empty
 *               string.
 *
 * @return The ID used for this variable.
 */
//------------------------------------------------------------------------------
Integer DifferentialCorrector::SetSolverResults(Real *data,
                                                const std::string &name,
                                                const std::string &type)
{
    if (goalNames[goalCount] != name)
        throw SolverException("Mismatch between parsed and configured goal");
    goal[goalCount] = data[0];
    tolerance[goalCount] = data[1];
    ++goalCount;
    return goalCount-1;
}


//------------------------------------------------------------------------------
// bool UpdateSolverGoal(Integer id, Real newValue)
//------------------------------------------------------------------------------
/**
 * Updates the targeter goals, for floating end point targeting.
 *
 * @param <id>       Id for the goal that is being reset.
 * @param <newValue> The new goal value.
 *
 * @return true on success, throws on failure.
 */
//------------------------------------------------------------------------------
bool DifferentialCorrector::UpdateSolverGoal(Integer id, Real newValue)
{
   // Only update during nominal runs
   if (currentState == NOMINAL) {
      if (id >= goalCount)
         throw SolverException(
            "DifferentialCorrector member requested a parameter outside the "
            "range of the configured goals.");

      goal[id] = newValue;
   }
   return true;
}


//------------------------------------------------------------------------------
// bool UpdateSolverTolerance(Integer id, Real newValue)
//------------------------------------------------------------------------------
/**
 * Updates the targeter tolerances, for floating end point targeting.
 *
 * @param <id>       Id for the tolerance that is being reset.
 * @param <newValue> The new tolerance value.
 *
 * @return true on success, throws on failure.
 */
//------------------------------------------------------------------------------
bool DifferentialCorrector::UpdateSolverTolerance(Integer id, Real newValue)
{
   // Only update during nominal runs
   if (currentState == NOMINAL) {
      if (id >= goalCount)
         throw SolverException(
            "DifferentialCorrector member requested a parameter outside the "
            "range of the configured goals.");

      tolerance[id] = newValue;
   }
   return true;
}


//------------------------------------------------------------------------------
// void SetResultValue(Integer id, Real value)
//------------------------------------------------------------------------------
/**
 * Passes in the results obtained from a run in the DifferentialCorrector loop.
 *
 * @param <id>    The ID used for this result.
 * @param <value> The corresponding result.
 */
//------------------------------------------------------------------------------
void DifferentialCorrector::SetResultValue(Integer id, Real value,
                                           const std::string &resultType)
{
   #ifdef DEBUG_STATE_MACHINE
      MessageInterface::ShowMessage(
            "   State %d received id %d    value = %.12lf\n", currentState, id,
            value);
   #endif
    if (currentState == NOMINAL)
    {
        nominal[id] = value;
    }

    if (currentState == PERTURBING)
    {
       if (firstPert)
          achieved[pertNumber][id] = value;
       else
          backAchieved[pertNumber][id] = value;
    }
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initializes the DifferentialCorrector prior to targeting.
 */
//------------------------------------------------------------------------------
bool DifferentialCorrector::Initialize()
{
   // Setup the variable data structures
   Integer localVariableCount = variableNames.size();
   Integer localGoalCount = goalNames.size();

   #if DEBUG_DC_INIT
   MessageInterface::ShowMessage
      ("DifferentialCorrector::Initialize() localVariableCount=%d, "
       "localGoalCount=%d\n", localVariableCount, localGoalCount);
   #endif

   if (localVariableCount == 0 || localGoalCount == 0)
   {
      std::string errorMessage = "Targeter cannot initialize: ";
      errorMessage += "No goals or variables are set.\n";
      throw SolverException(errorMessage);
   }

   FreeArrays();

   // Setup the goal data structures
   goal      = new Real[localGoalCount];
   tolerance = new Real[localGoalCount];
   nominal   = new Real[localGoalCount];

   // And the sensitivity matrix
   Integer i;
   achieved        = new Real*[localVariableCount];
   backAchieved    = new Real*[localVariableCount];
   jacobian        = new Real*[localVariableCount];
   for (i = 0; i < localVariableCount; ++i)
   {
      jacobian[i]        = new Real[localGoalCount];
      achieved[i]        = new Real[localGoalCount];
      backAchieved[i]    = new Real[localGoalCount];
   }

   inverseJacobian = new Real*[localGoalCount];
   for (i = 0; i < localGoalCount; ++i)
   {
      inverseJacobian[i] = new Real[localVariableCount];
   }

   Solver::Initialize();

   // Allocate the LU arrays
   indx = new Integer[variableCount];
   b = new Real[variableCount];

   #if DEBUG_DC_INIT
      MessageInterface::ShowMessage
            ("DifferentialCorrector::Initialize() completed\n");
   #endif

   return true;
}


//------------------------------------------------------------------------------
//  Solver::SolverState AdvanceState()
//------------------------------------------------------------------------------
/**
 * The method used to walk the DifferentialCorrector through its state machine.
 *
 * @return solver state at the end of the process.
 */
//------------------------------------------------------------------------------
Solver::SolverState DifferentialCorrector::AdvanceState()
{
   switch (currentMode)
   {
      case INITIAL_GUESS:
         #ifdef DEBUG_TARGETING_MODES
            MessageInterface::ShowMessage(
                  "Running in INITIAL_GUESS mode; state = %d\n", currentState);
         #endif
            switch (currentState)
            {
               case INITIALIZING:
                  #ifdef DEBUG_STATE_MACHINE
                     MessageInterface::ShowMessage(
                           "Entered state machine; INITIALIZING\n");
                  #endif
                  iterationsTaken = 0;
                  WriteToTextFile();
                  ReportProgress();
                  CompleteInitialization();
                  status = INITIALIZED;
                  break;

               case NOMINAL:
                  #ifdef DEBUG_STATE_MACHINE
                     MessageInterface::ShowMessage(
                           "Entered state machine; NOMINAL\n");
                  #endif
                  WriteToTextFile();
                  currentState = FINISHED;
                  status = RUN;
                  break;

               case FINISHED:
               default:
                  #ifdef DEBUG_STATE_MACHINE
                     MessageInterface::ShowMessage(
                           "Entered state machine; FINISHED\n");
                  #endif
                  RunComplete();
                  ReportProgress();
                  break;
            }
         break;

      case SOLVE:
      default:
         #ifdef DEBUG_TARGETING_MODES
            MessageInterface::ShowMessage(
                  "Running in SOLVE or default mode; state = %d\n",
                  currentState);
         #endif
         switch (currentState)
         {
            case INITIALIZING:
               #ifdef DEBUG_STATE_MACHINE
                  MessageInterface::ShowMessage(
                        "Entered state machine; INITIALIZING\n");
               #endif
               iterationsTaken = 0;
               WriteToTextFile();
               ReportProgress();
               CompleteInitialization();
               status = INITIALIZED;
               break;

            case NOMINAL:
               #ifdef DEBUG_STATE_MACHINE
                  MessageInterface::ShowMessage(
                        "Entered state machine; NOMINAL\n");
               #endif
               ReportProgress();
               RunNominal();
               ReportProgress();
               status = RUN;
               break;

            case PERTURBING:
               #ifdef DEBUG_STATE_MACHINE
                  MessageInterface::ShowMessage(
                        "Entered state machine; PERTURBING\n");
               #endif
               ReportProgress();
               RunPerturbation();
               break;

            case CALCULATING:
               #ifdef DEBUG_STATE_MACHINE
                  MessageInterface::ShowMessage(
                        "Entered state machine; CALCULATING\n");
               #endif
               ReportProgress();
               CalculateParameters();
               break;

            case CHECKINGRUN:
                #ifdef DEBUG_STATE_MACHINE
                  MessageInterface::ShowMessage(
                        "Entered state machine; CHECKINGRUN\n");
               #endif
               CheckCompletion();
               ++iterationsTaken;
               if (iterationsTaken >= maxIterations)
               {
                  MessageInterface::ShowMessage(
                        "Differential corrector %s %s\n", instanceName.c_str(),
                        "has exceeded the maximum number of allowed "
                        "iterations.");
                  currentState = FINISHED;
               }
               break;

            case FINISHED:
               #ifdef DEBUG_STATE_MACHINE
                  MessageInterface::ShowMessage(
                        "Entered state machine; FINISHED\n");
               #endif
               RunComplete();
               ReportProgress();
               break;

            case ITERATING:             // Intentional drop-through
            default:
               #ifdef DEBUG_STATE_MACHINE
                  MessageInterface::ShowMessage("Entered state machine; "
                     "Bad state for a differential corrector.\n");
               #endif
               throw SolverException(
                     "Solver state not supported for the targeter");
         }
         break;
   }

   return currentState;
}


//------------------------------------------------------------------------------
//  void RunNominal()
//------------------------------------------------------------------------------
/**
 * Run out the nominal sequence, generating the "current" targeter data.
 */
//------------------------------------------------------------------------------
void DifferentialCorrector::RunNominal()
{
   // On success, set the state to the next machine state
   WriteToTextFile();
   currentState = CHECKINGRUN;
}


//------------------------------------------------------------------------------
//  void RunPerturbation()
//------------------------------------------------------------------------------
/**
 * Run out a perturbation, generating data used to evaluate the Jacobian.
 */
//------------------------------------------------------------------------------
void DifferentialCorrector::RunPerturbation()
{
   // Calculate the perts, one at a time
   if (pertNumber != -1)
      // Back out the last pert applied
      variable.at(pertNumber) = lastUnperturbedValue;
   if (incrementPert)
      ++pertNumber;

   if (pertNumber == variableCount)  // Current set of perts have been run
   {
      currentState = CALCULATING;
      pertNumber = -1;
      return;
   }

   lastUnperturbedValue = variable.at(pertNumber);
   if (diffMode == 1)      // Forward difference
   {
      firstPert = true;
      variable.at(pertNumber) += perturbation.at(pertNumber);
      pertDirection.at(pertNumber) = 1.0;
   }
   else if (diffMode == 0) // Central difference
   {
      if (incrementPert)
      {
         firstPert = true;
         incrementPert = false;
         variable.at(pertNumber) += perturbation.at(pertNumber);
         pertDirection.at(pertNumber) = 1.0;
      }
      else
      {
         firstPert = false;
         incrementPert = true;
         variable.at(pertNumber) -= perturbation.at(pertNumber);
         pertDirection.at(pertNumber) = -1.0;
      }
   }
   else                    // Backward difference
   {
      firstPert = true;
      variable.at(pertNumber) -= perturbation.at(pertNumber);
      pertDirection.at(pertNumber) = -1.0;
   }

   if (variable[pertNumber] > variableMaximum[pertNumber])
   {
      if (diffMode == 0)
      {
         // Warn user that central differencing violates constraint and continue
         MessageInterface::ShowMessage("Warning!  Perturbation violates the "
               "maximum value for variable %s, but is being applied anyway to "
               "perform central differencing in the differential corrector "
               "%s\n", variableNames[pertNumber].c_str(), instanceName.c_str());
      }
      else
      {
         pertDirection.at(pertNumber) = -1.0;
         variable[pertNumber] -= 2.0 * perturbation[pertNumber];
      }
   }

   if (variable[pertNumber] < variableMinimum[pertNumber])
   {
      if (diffMode == 0)
      {
         // Warn user that central differencing violates constraint and continue
         MessageInterface::ShowMessage("Warning!  Perturbation violates the "
               "minimum value for variable %s, but is being applied anyway to "
               "perform central differencing in the differential corrector "
               "%s\n", variableNames[pertNumber].c_str(), instanceName.c_str());
      }
      else
      {
         pertDirection.at(pertNumber) = -1.0;
         variable[pertNumber] -= 2.0 * perturbation[pertNumber];
      }
   }

   WriteToTextFile();
}


//------------------------------------------------------------------------------
//  void CalculateParameters()
//------------------------------------------------------------------------------
/**
 * Updates the values for the variables based on the inverted Jacobian.
 */
//------------------------------------------------------------------------------
void DifferentialCorrector::CalculateParameters()
{
   // Build and invert the sensitivity matrix
   CalculateJacobian();
   InvertJacobian();

   std::vector<Real> delta;

   // Apply the inverse Jacobian to build the next set of variables
   for (Integer i = 0; i < variableCount; ++i)
   {
      delta.push_back(0.0);
      for (Integer j = 0; j < goalCount; j++)
         delta[i] += inverseJacobian[j][i] * (goal[j] - nominal[j]);
   }

   Real multiplier = 1.0, maxDelta;

   // First validate the variable changes
   for (Integer i = 0; i < variableCount; ++i)
   {
      if (fabs(delta.at(i)) > variableMaximumStep.at(i))
      {
         maxDelta = fabs(variableMaximumStep.at(i) / delta.at(i));
         if (maxDelta < multiplier)
            multiplier = maxDelta;
      }
   }

   #ifdef DEBUG_VARIABLES_CALCS
      MessageInterface::ShowMessage("Variable Values; Multiplier = %.15lf\n",
            multiplier);
   #endif

   for (Integer i = 0; i < variableCount; ++i)
   {
      // Ensure that delta is not larger than the max allowed step
      try
      {
         #ifdef DEBUG_VARIABLES_CALCS
            MessageInterface::ShowMessage(
                  "   %d:  %.15lf  +  %.15lf  *  %.15lf", i, variable.at(i),
                  delta.at(i), multiplier);
         #endif
         variable.at(i) += delta.at(i) * multiplier;
         #ifdef DEBUG_VARIABLES_CALCS
            MessageInterface::ShowMessage("  ->  %.15lf\n", variable.at(i));
         #endif

         // Ensure that variable[i] is in the allowed range
         if (variable.at(i) < variableMinimum.at(i))
            variable.at(i) = variableMinimum.at(i);
         if (variable.at(i) > variableMaximum.at(i))
            variable.at(i) = variableMaximum.at(i);
      }
      catch(std::exception &)
      {
         throw SolverException("Range error in Solver::CalculateParameters\n");
      }
   }

   WriteToTextFile();
   currentState = NOMINAL;
}


//------------------------------------------------------------------------------
//  void CheckCompletion()
//------------------------------------------------------------------------------
/**
 * Determine whether or not the targeting run has converged.
 */
//------------------------------------------------------------------------------
void DifferentialCorrector::CheckCompletion()
{
   WriteToTextFile();
   bool converged = true;          // Assume convergence

   // check for lack of convergence
   for (Integer i = 0; i < goalCount; ++i)
   {
      if (GmatMathUtil::Abs(nominal[i] - goal[i]) > tolerance[i])
         converged = false;
   }

   if (!converged)
   {
      if (iterationsTaken < maxIterations-1)
      {
         // Set to run perts if not converged
         pertNumber = -1;
         // Build the first perturbation
         currentState = PERTURBING;
         RunPerturbation();
      }
      else
      {
         currentState = FINISHED;
         status = EXCEEDED_ITERATIONS;
      }
   }
   else
   {
      // If converged, we're done
      currentState = FINISHED;
      status = CONVERGED;
   }
}


//------------------------------------------------------------------------------
//  void RunComplete()
//------------------------------------------------------------------------------
/**
 * Updates the targeter text file at the end of a targeter run.
 */
//------------------------------------------------------------------------------
void DifferentialCorrector::RunComplete()
{
    WriteToTextFile();
    hasFired = true;
}


//------------------------------------------------------------------------------
//  void CalculateJacobian()
//------------------------------------------------------------------------------
/**
 * Calculates the matrix of derivatives of the goals with respect to the
 * variables.
 */
//------------------------------------------------------------------------------
void DifferentialCorrector::CalculateJacobian()
{
   Integer i, j;

   if (diffMode != 0)
   {
      for (i = 0; i < variableCount; ++i)
      {
         for (j = 0; j < goalCount; ++j)
         {
             jacobian[i][j] = achieved[i][j] - nominal[j];
             jacobian[i][j] /= (pertDirection.at(i) * perturbation.at(i));
         }
      }
   }
   else        // Central differencing
   {
      for (i = 0; i < variableCount; ++i)
      {
         for (j = 0; j < goalCount; ++j)
         {
             jacobian[i][j] = achieved[i][j] - backAchieved[i][j]; // nominal[j];
             jacobian[i][j] /= (2.0 * perturbation.at(i));
         }
      }
   }
}


//------------------------------------------------------------------------------
//  void InvertJacobian()
//------------------------------------------------------------------------------
/**
 * Inverts the matrix of derivatives so that the change in the variables can be
 * estimated.
 */
//------------------------------------------------------------------------------
void DifferentialCorrector::InvertJacobian()
{
   #ifdef DEBUG_JACOBIAN
      MessageInterface::ShowMessage("Inverting %d by %d Jacobian\n",
            variableCount, goalCount);
   #endif
   Rmatrix jac(variableCount, goalCount);
   for (Integer i = 0; i < variableCount; ++i)
      for (Integer j = 0; j < goalCount; ++j)
      {
         #ifdef DEBUG_JACOBIAN
            MessageInterface::ShowMessage("   jacobian[%d][%d] = %.14lf\n", i,
                  j, jacobian[i][j]);
         #endif
         jac(i,j) = jacobian[i][j];
      }

   Rmatrix inv;
   try
   {
      if (variableCount == goalCount)
         inv = jac.Inverse();
      else
         inv = jac.Pseudoinverse();
   }
   catch (BaseException &ex)
   {
      throw SolverException("Error inverting the Differential Corrector "
            "Jacobian; it appears that the variables in the Vary command(s) do "
            "not affect the target parameters in the Achieve command(s)");
   }

   #ifdef DEBUG_JACOBIAN
      MessageInterface::ShowMessage("Inverse Jacobian is %d by %d\n",
            variableCount, goalCount);
   #endif

   #ifdef DEBUG_DC_INVERSIONS
      std::string preface = "   ";
      if (variableCount == goalCount)
         MessageInterface::ShowMessage("Inverse:\n%s\n",
               (inv.ToString(16, false, preface).c_str()));
      else
         MessageInterface::ShowMessage("PseudoInverse:\n%s\n",
               inv.ToString(16, false, preface).c_str());
   #endif

   for (Integer i = 0; i < goalCount; ++i)
      for (Integer j = 0; j < variableCount; ++j)
      {
         inverseJacobian[i][j] = inv(i,j);
         #ifdef DEBUG_JACOBIAN
            MessageInterface::ShowMessage(
                  "   inverseJacobian[%d][%d] = %.14lf\n", i, j,
                  inverseJacobian[i][j]);
         #endif
      }
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
void DifferentialCorrector::FreeArrays()
{
   Solver::FreeArrays();

   if (goal)
   {
      delete [] goal;
      goal = NULL;
   }

   if (tolerance)
   {
      delete [] tolerance;
      tolerance = NULL;
   }

   if (nominal)
   {
      delete [] nominal;
      nominal = NULL;
   }

   if (achieved)
   {
      for (Integer i = 0; i < variableCount; ++i)
         delete [] achieved[i];
      delete [] achieved;
      achieved = NULL;
   }

   if (backAchieved)
   {
      for (Integer i = 0; i < variableCount; ++i)
         delete [] backAchieved[i];
      delete [] backAchieved;
      backAchieved = NULL;
   }

   if (jacobian)
   {
      for (Integer i = 0; i < variableCount; ++i)
         delete [] jacobian[i];
      delete [] jacobian;
      jacobian = NULL;
   }

   if (inverseJacobian)
   {
      for (Integer i = 0; i < goalCount; ++i)
         delete [] inverseJacobian[i];
      delete [] inverseJacobian;
      inverseJacobian = NULL;
   }

   if (indx)
   {
      delete [] indx;
      indx = NULL;
   }

   if (b)
   {
      delete [] b;
      b = NULL;
   }
}


//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string that reporting the current differential corrector state.
 */
//------------------------------------------------------------------------------
std::string DifferentialCorrector::GetProgressString()
{
   StringArray::iterator current;
   Integer i;
   std::stringstream progress;
   progress.str("");
   progress.precision(12);

   if (isInitialized)
   {
      switch (currentState)
      {
         case INITIALIZING:
            // This state is basically a "paused state" used for the Target
            // command to finalize the initial data for the variables and
            // goals.  All that is written here is the header information.
            {
               Integer localVariableCount = variableNames.size(),
                       localGoalCount = goalNames.size();
               progress << "************************************************"
                        << "********\n"
                        << "*** Performing Differential Correction "
                        << "(using \"" << instanceName << "\")\n";

               // Write out the setup data
               progress << "*** " << localVariableCount << " variables; "
                        << localGoalCount << " goals\n   Variables:  ";

               // Iterate through the variables and goals, writing them to
               // the file
               for (current = variableNames.begin(), i = 0;
                    current != variableNames.end(); ++current)
               {
                  if (current != variableNames.begin())
                     progress << ", ";
                  progress << *current;
               }

               progress << "\n   Goals:  ";

               for (current = goalNames.begin(), i = 0;
                    current != goalNames.end(); ++current)
               {
                  if (current != goalNames.begin())
                     progress << ", ";
                  progress << *current;
               }

               if (solverMode != "")
                  progress << "\n   SolverMode:  "
                           << solverMode;


               progress << "\n****************************"
                        << "****************************";
            }
            break;

         case NOMINAL:
            progress << instanceName << " Iteration " << iterationsTaken+1
                     << "; Nominal Pass\n   Variables:  ";
            // Iterate through the variables, writing them to the string
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               if (current != variableNames.begin())
                  progress << ", ";
               progress << *current << " = " << unscaledVariable.at(i);
               if (textFileMode == "Verbose")
                  progress << "; targeter scaled value: " << variable[i];
               ++i;
            }
            break;

         case PERTURBING:
            progress << "   Completed iteration " << iterationsTaken
                     << ", pert " << pertNumber+1 << " ("
                     << variableNames[pertNumber] << " = "
                     << unscaledVariable.at(pertNumber);
            if (textFileMode == "Verbose")
               progress << "; targeter scaled value: " << variable[pertNumber];
            progress << ")";
            break;

         case CALCULATING:
            // Just forces a blank line
            break;

         case CHECKINGRUN:
            // Iterate through the goals, writing them to the file
            progress << "   Goals and achieved values:\n";

            for (current = goalNames.begin(), i = 0;
                 current != goalNames.end(); ++current)
            {
               progress << "      " << *current
                        << "  Desired: " << goal[i]
                        << "  Achieved: " << nominal[i]
                        << "  Variance: " << (goal[i] - nominal[i])
                        << "\n";
               ++i;
            }

            break;

         case FINISHED:
            switch (currentMode)
            {
               case INITIAL_GUESS:
                  progress << "\n*** Targeting Completed Initial Guess Run\n"
                           << "***\n   Variable Values:\n";
                  for (current = variableNames.begin(), i = 0;
                       current != variableNames.end(); ++current)
                     progress << "      " << *current
                              << " = " << unscaledVariable.at(i++) << "\n";
                  progress << "\n   Goal Values:\n";
                  for (current = goalNames.begin(), i = 0;
                       current != goalNames.end(); ++current)
                  {
                     progress << "      " << *current
                              << "  Desired: " << goal[i]
                              << "  Achieved: " << nominal[i]
                              << "  Variance: " << (goal[i] - nominal[i])
                              << "\n";
                     ++i;
                  }
                  break;

               case SOLVE:
               default:
                  if (status == CONVERGED)
                     progress << "\n*** Targeting Completed in "
                              << iterationsTaken << " iterations.\n"
                              << "*** The Targeter converged!" ;
                  else
                     progress << "\n*** Targeting did not converge in "
                              << iterationsTaken << " iterations";

                  if ((iterationsTaken >= maxIterations) &&
                      (status != CONVERGED))
                     progress << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                           << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                           << "!!! WARNING: Targeter did NOT converge!"
                           << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
                           << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

                  progress << "\nFinal Variable values:\n";
                  // Iterate through the variables, writing them to the string
                  for (current = variableNames.begin(), i = 0;
                       current != variableNames.end(); ++current)
                     progress << "   " << *current << " = "
                              << unscaledVariable.at(i++) << "\n";
            }
            break;

         case ITERATING:     // Intentional fall through
         default:
            throw SolverException(
               "Solver state not supported for the targeter");
      }
   }
   else
      return Solver::GetProgressString();

   return progress.str();
}


//------------------------------------------------------------------------------
//  void WriteToTextFile()
//------------------------------------------------------------------------------
/**
 * Writes state data to the targeter text file.
 */
//------------------------------------------------------------------------------
void DifferentialCorrector::WriteToTextFile(SolverState stateToUse)
{
   #ifdef DEBUG_SOLVER_WRITE
   MessageInterface::ShowMessage
      ("DC::WriteToTextFile() entered, stateToUse=%d, solverTextFile='%s', "
       "textFileOpen=%d, initialized=%d\n", stateToUse, solverTextFile.c_str(),
       textFile.is_open(), initialized);
   #endif

   if (!showProgress)
      return;

   if (!textFile.is_open())
      OpenSolverTextFile();

   StringArray::iterator current;
   Integer i, j;
   if (isInitialized)
   {
      switch (currentState)
      {
         case INITIALIZING:
            // This state is basically a "paused state" used for the Target
            // command to finalize the initial data for the variables and
            // goals.  All that is written here is the header information.
            {
               Integer localVariableCount = variableNames.size(),
                       localGoalCount = goalNames.size();
               textFile << "************************************************"
                        << "********\n"
                        << "*** Targeter Text File\n"
                        << "*** \n"
                        << "*** Using Differential Correction\n***\n";

               // Write out the setup data
               textFile << "*** " << localVariableCount << " variables\n*** "
                        << localGoalCount << " goals\n***\n*** "
                        << "Variables:\n***    ";

               // Iterate through the variables and goals, writing them to
               // the file
               for (current = variableNames.begin(), i = 0;
                    current != variableNames.end(); ++current)
               {
                  textFile << *current << "\n***    ";
               }

               textFile << "\n*** Goals:\n***    ";

               for (current = goalNames.begin(), i = 0;
                    current != goalNames.end(); ++current)
               {
                  textFile << *current << "\n***    ";
               }

               if (solverMode != "")
                  textFile << "\n*** SolverMode:  "
                           << solverMode
                           <<"\n***    ";

               textFile << "\n****************************"
                        << "****************************\n"
                        << std::endl;
            }
            break;

         case NOMINAL:
            textFile << "Iteration " << iterationsTaken+1
                     << "\nRunning Nominal Pass\nVariables:\n   ";
            // Iterate through the variables, writing them to the file
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               textFile << *current << " = " << unscaledVariable.at(i);
               if ((textFileMode == "Verbose") || (textFileMode == "Debug"))
                     textFile << "; targeter scaled value: " << variable.at(i);
               textFile << "\n   ";
               ++i;
            }
            textFile << std::endl;
            break;

         case PERTURBING:
            if ((textFileMode == "Verbose") || (textFileMode == "Debug"))
            {
               if (pertNumber != 0)
               {
                  // Iterate through the goals, writing them to the file
                  textFile << "Goals and achieved values:\n   ";

                  for (current = goalNames.begin(), i = 0;
                       current != goalNames.end(); ++current)
                  {
                     textFile << *current << "  Desired: " << goal[i]
                              << " Achieved: " << achieved[pertNumber-1][i]
                              << "\n   ";
                     ++i;
                  }
                  textFile << std::endl;
               }
               textFile << "Perturbing with variable values:\n   ";
               for (current = variableNames.begin(), i = 0;
                    current != variableNames.end(); ++current)
               {
                  textFile << *current << " = " << unscaledVariable.at(i);
                  if ((textFileMode == "Verbose") || (textFileMode == "Debug"))
                        textFile << "; targeter scaled value: " << variable.at(i);
                  textFile << "\n   ";
                  ++i;
               }
               textFile << std::endl;
            }

            if (textFileMode == "Debug")
            {
               textFile << "------------------------------------------------\n"
                        << "Command stream data:\n"
                        << debugString << "\n"
                        << "------------------------------------------------\n";
            }

            break;

         case CALCULATING:
            if (textFileMode == "Verbose")
            {
               textFile << "Calculating" << std::endl;

               // Iterate through the goals, writing them to the file
               textFile << "Goals and achieved values:\n   ";

               for (current = goalNames.begin(), i = 0;
                    current != goalNames.end(); ++current)
               {
                   textFile << *current << "  Desired: " << goal[i]
                            << " Achieved: " << achieved[variableCount-1][i]
                            << "\n    ";
                   ++i;
               }
               textFile << std::endl;
            }

            textFile << "\nJacobian (Sensitivity matrix):\n";
            for (i = 0; i < variableCount; ++i)
            {
               for (j = 0; j < goalCount; ++j)
               {
                  textFile << "   " << jacobian[i][j];
               }
               textFile << "\n";
            }

            textFile << "\n\nInverse Jacobian:\n";
            for (i = 0; i < goalCount; ++i)
            {
               for (j = 0; j < variableCount; ++j)
               {
                  textFile << "   " << inverseJacobian[i][j];
               }
               textFile << "\n";
            }

            textFile << "\n\nNew scaled variable estimates:\n   ";
            for (current = variableNames.begin(), i = 0;
                 current != variableNames.end(); ++current)
            {
               //textFile << *current << " = " << variable[i++] << "\n   ";
               textFile << *current << " = " << variable.at(i++) << "\n   ";
            }
            textFile << std::endl;
            break;

         case CHECKINGRUN:
            // Iterate through the goals, writing them to the file
            textFile << "Goals and achieved values:\n   ";

            for (current = goalNames.begin(), i = 0;
                 current != goalNames.end(); ++current)
            {
               textFile << *current << "  Desired: " << goal[i]
                        << " Achieved: " << nominal[i]
                        << "\n   Tolerance: " << tolerance[i]
                        << "\n   ";
               ++i;
            }

            textFile << "\n*****************************"
                     << "***************************\n"
                     << std::endl;
            break;

         case FINISHED:
            textFile << "\n****************************"
                     << "****************************\n"
                     << "*** Targeting Completed in " << iterationsTaken
                     << " iterations"
                     << "\n****************************"
                     << "****************************\n"
                     << std::endl;

            break;

         case ITERATING:     // Intentional fall through
         default:
            throw SolverException(
               "Solver state not supported for the targeter");
      }
   }
}
