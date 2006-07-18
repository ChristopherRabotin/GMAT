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
   //"",
};

const Gmat::ParameterType
Optimizer::PARAMETER_TYPE[OptimizerParamCount - SolverParamCount] =
{
   //Gmat::STRING_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

Optimizer::Optimizer(std::string name) :
   Solver                  ("Optimizer", name),
   objectiveFnName         ("Objective"),
   cost                    (0.0),   // valid value?
   tolerance               (0.0),   // valid value?
   converged               (false)
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
   converged               (false)
{
   constraintNames  = opt.constraintNames;
   constraintValues = opt.constraintValues;
   parameterCount   = opt.parameterCount;
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
   constraintNames  = opt.constraintNames;
   constraintValues = opt.constraintValues;
   parameterCount   = opt.parameterCount;
 
   return *this;
}

bool Optimizer::Initialize()
{
   // Setup the variable data structures
   Integer localVariableCount = variableNames.size();
   Integer localConstraintCount = constraintNames.size();

    
   if (localVariableCount == 0 || localConstraintCount == 0)
   {
      std::string errorMessage = "Solver cannot initialize: ";
      errorMessage += "No constraints or variables are set.\n";
      throw SolverException(errorMessage);
   }
   
   FreeArrays();

   // these should be moved up to Solver???  and modified to be std::vectors
   variable            = new Real[localVariableCount];
   perturbation        = new Real[localVariableCount];
   variableMinimum     = new Real[localVariableCount];
   variableMaximum     = new Real[localVariableCount];
   variableMaximumStep = new Real[localVariableCount];


   for (Integer i = 0; i < localVariableCount; ++i)
   {
      // Set default values for min and max parameters
      variable[i]            =  0.0;
      variableMinimum[i]     = -9.999e300;
      variableMaximum[i]     =  9.999e300;
      variableMaximumStep[i] =  9.999e300;
   }
   Solver::Initialize();
   
   initialized = true;  // move these to Solver?
   iterationsTaken = 0;
   return true;
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
   
   constraintValues.clear();
    
} 