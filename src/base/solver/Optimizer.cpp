//$Header$
//------------------------------------------------------------------------------
//                         Optimizer
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy C. Shoan/GSFC
// Created: 2006.07.14
//
/**
 * Implementation for the optimizer base class. 
 *
 */
//------------------------------------------------------------------------------


#include <sstream>
#include "Optimizer.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"     // for GmatMathUtil::Abs()
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string
Optimizer::PARAMETER_TEXT[OptimizerParamCount -SolverParamCount] =
{
   "ObjectiveFunction",
   "Tolerance",
   "EqualityConstraintNames",
   "InequalityConstraintNames",
};

const Gmat::ParameterType
Optimizer::PARAMETER_TYPE[OptimizerParamCount - SolverParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

Optimizer::Optimizer(std::string typeName, std::string name) :
   Solver                  (typeName, name),
   objectiveFnName         ("Objective"),
   cost                    (0.0),   // valid value?
   tolerance               (0.0),   // valid value?
   converged               (false),
   eqConstraintCount       (0),
   ineqConstraintCount     (0),
   objectiveValue          (0.0)
 {
   objectTypeNames.push_back("Optimizer");
   parameterCount = OptimizerParamCount;
}


Optimizer::~Optimizer()
{
   FreeArrays();
}


Optimizer::Optimizer(const Optimizer &opt) :
   Solver                  (opt),
   objectiveFnName         ("Objective"),
   cost                    (opt.cost), 
   tolerance               (opt.tolerance), 
   converged               (false),
   eqConstraintCount       (opt.eqConstraintCount),
   ineqConstraintCount     (opt.ineqConstraintCount),
   objectiveValue          (opt.objectiveValue)
{
   eqConstraintNames    = opt.eqConstraintNames;
   ineqConstraintNames  = opt.ineqConstraintNames;
   eqConstraintValues   = opt.eqConstraintValues;
   ineqConstraintValues = opt.ineqConstraintValues;
   parameterCount       = opt.parameterCount;
}


Optimizer& 
    Optimizer::operator=(const Optimizer& opt)
{
    if (&opt == this)
        return *this;

   Solver::operator=(opt);
   
   objectiveFnName  = opt.objectiveFnName;
   cost             = opt.cost;
   tolerance        = opt.tolerance;
   converged        = opt.converged;
   
   FreeArrays();
   eqConstraintCount    = opt.eqConstraintCount;
   ineqConstraintCount  = opt.ineqConstraintCount;
   objectiveValue       = opt.objectiveValue;
   eqConstraintNames    = opt.eqConstraintNames;
   ineqConstraintNames  = opt.ineqConstraintNames;
   eqConstraintValues   = opt.eqConstraintValues;
   ineqConstraintValues = opt.ineqConstraintValues;
   parameterCount       = opt.parameterCount;
 
   return *this;
}

bool Optimizer::Initialize()
{
   if (variableNames.size() == 0)  // constraints are not required
   {
      std::string errorMessage = "Optimizer cannot initialize: ";
      errorMessage += "No variables are set.\n";
      throw SolverException(errorMessage);
   }
   
   FreeArrays();

   Solver::Initialize();
   
   return true;
}

//------------------------------------------------------------------------------
// Integer SetSolverResults(Real *data, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets up the data fields used for the results of an iteration.
 * 
 * @param <data> An array of data appropriate to the results used in the
 *               algorithm (for instance, constraints).
 * @param <name> A label for the data parameter.  Defaults to the empty
 *               string.
 * @param <type> string indicating type of result to set
 * 
 * @return The ID used for this variable.
 */
//------------------------------------------------------------------------------
Integer Optimizer::SetSolverResults(Real *data,
                                    const std::string &name,
                                    const std::string &type)
{
   if (type == "Objective")
   {
      objectiveValue = data[0];
      return 0;
   }
   else if (type == " EqConstraint")
   {
      if (eqConstraintNames[eqConstraintCount] != name)
        throw SolverException(
           "Mismatch between parsed and configured equality constraint");
     eqConstraintValues[eqConstraintCount] = data[0];
     ++eqConstraintCount;
     return eqConstraintCount - 1;
    }
    else if (type == "IneqConstraint")
    {
       if (ineqConstraintNames[ineqConstraintCount] != name)
        throw SolverException(
           "Mismatch between parsed and configured inequality constraint");
     ineqConstraintValues[ineqConstraintCount] = data[0];
     ++ineqConstraintCount;
     return ineqConstraintCount - 1;
    }
    // add Gradient and Jacobian later ...
    else
    {
       throw SolverException(
           "Unknown type passed in to SetSolverResults");
    }
}

//------------------------------------------------------------------------------
// void SetResultValue(Integer id, Real value, const std::string resultType = "")
//------------------------------------------------------------------------------
/**
 * Passes in the results obtained from a run in the Optimizer loop.
 * 
 * @param <id>    The ID used for this result.
 * @param <value> The corresponding result.
 */
//------------------------------------------------------------------------------
void Optimizer::SetResultValue(Integer id, Real value, 
                               const std::string resultType)
{
   if (resultType == "Objective")
   {
      objectiveValue = value;
   }
   else if (resultType == " EqConstraint")
   {
      if (id > eqConstraintCount)
        throw SolverException(
           "id range error for equality constraint");
     eqConstraintValues[id] = value;
    }
    else if (resultType == "IneqConstraint")
    {
       if (id > ineqConstraintCount)
        throw SolverException(
           "id range error for inequality constraint");
     ineqConstraintValues[id] = value;
    }
    // add Gradient and Jacobian later ...
    else
    {
       throw SolverException(
           "Unknown result type passed in to SetResultValue");
    }
}

//------------------------------------------------------------------------------
//  std::string GetProgressString()
//------------------------------------------------------------------------------
/**
 * Generates a string that reporting the current differential corrector state.
 */
//------------------------------------------------------------------------------
std::string Optimizer::GetProgressString()
{
   StringArray::iterator current;
   //Integer i;
   std::stringstream progress;
   progress.str("");
   progress.precision(12);

   if (initialized)
   {
      // ******** TBD ******** see DC for example
   }
   else
      return Solver::GetProgressString();
      
   return progress.str();
}


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
std::string Optimizer::GetParameterText(const Integer id) const
{
   if ((id >= SolverParamCount) && (id < OptimizerParamCount))
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
Integer Optimizer::GetParameterID(const std::string &str) const
{
   for (Integer i = SolverParamCount; i < OptimizerParamCount; ++i)
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
Gmat::ParameterType Optimizer::GetParameterType(
                                              const Integer id) const
{
   if ((id >= SolverParamCount) && (id < OptimizerParamCount))
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
std::string Optimizer::GetParameterTypeString(
                                      const Integer id) const
{
   return Solver::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Optimizer::GetRealParameter(const Integer id) const
{
   if (id == OPTIMIZER_TOLERANCE)               return tolerance;

   return Solver::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return  Real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Optimizer::SetRealParameter(const Integer id, const Real value)
{
   if (id == OPTIMIZER_TOLERANCE)
   {
      tolerance = value;
      return tolerance;
   }
   return Solver::SetRealParameter(id, value);
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
std::string Optimizer::GetStringParameter(const Integer id) const
{
    if (id == OBJECTIVE_FUNCTION)
        return objectiveFnName;
        
    return Solver::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  Integer SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets a string or string array parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> string value for the parameter.
 *
 * @return  The value of the parameter at the completion of the call.
 */
//------------------------------------------------------------------------------
bool Optimizer::SetStringParameter(const Integer id,
                                   const std::string &value)
{
    if (id == OBJECTIVE_FUNCTION) {
        objectiveFnName = value;
        return true;
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
const StringArray& Optimizer::GetStringArrayParameter(
                                                        const Integer id) const
{
        
    if (id == EQUALITY_CONSTRAINT_NAMES)
        return eqConstraintNames;
    if (id == INEQUALITY_CONSTRAINT_NAMES)
        return ineqConstraintNames;
        
    return Solver::GetStringArrayParameter(id);
}



//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void FreeArrays()
//------------------------------------------------------------------------------
/**
 * Frees (clears) the constraints used by the optimizer.  This method is also 
 * called by the destructor when the script is cleared.
 */
//------------------------------------------------------------------------------
void Optimizer::FreeArrays()
{
   Solver::FreeArrays();
   //eqConstraintNames.clear(); ?????
   //ineqConstraintNames.clear(); ?????
   //eqConstraintCount - 0; ?????
   //ineqConstraintCount - 0; ?????
   eqConstraintValues.clear();
   ineqConstraintValues.clear();
    
} 