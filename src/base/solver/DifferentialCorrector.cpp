//$Header$
//------------------------------------------------------------------------------
//                         DifferentialCorrector
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
 * Implementation for the differential corrector targeter. 
 */
//------------------------------------------------------------------------------


#include "DifferentialCorrector.hpp"


DifferentialCorrector::DifferentialCorrector(std::string name) :
    Solver                  ("DifferentialCorrector", name), 
    variableCount           (0),
    goalCount               (0),
    iterationsTaken         (0),
    maxIterations           (25),
    variable                (NULL),
    perturbation            (NULL),
    variableMinimum         (NULL),
    variableMaximum         (NULL),
    variableMaximumStep     (NULL),
    goal                    (NULL),
    tolerance               (NULL),
    nominal                 (NULL),
    achieved                (NULL),
    jacobian                (NULL),
    inverseJacobian         (NULL),
    useCentralDifferences   (false),
    initialized             (false),
    solverTextFile          ("targeter.data"),
    solverTextFileID        (parameterCount),
    variableNamesID         (parameterCount+1),
    goalNamesID             (parameterCount+2)
{
}


DifferentialCorrector::~DifferentialCorrector()
{
    FreeArrays();
}


DifferentialCorrector::DifferentialCorrector(const DifferentialCorrector &dc) :
    Solver                  (dc), 
    variableCount           (dc.variableCount),
    goalCount               (dc.goalCount),
    iterationsTaken         (0),
    maxIterations           (dc.maxIterations),
    variable                (NULL),
    perturbation            (NULL),
    variableMinimum         (NULL),
    variableMaximum         (NULL),
    variableMaximumStep     (NULL),
    goal                    (NULL),
    tolerance               (NULL),
    nominal                 (NULL),
    achieved                (NULL),
    jacobian                (NULL),
    inverseJacobian         (NULL),
    useCentralDifferences   (dc.useCentralDifferences),
    initialized             (false),
    solverTextFile          (dc.solverTextFile),
    solverTextFileID        (dc.solverTextFileID),
    variableNamesID         (dc.variableNamesID),
    goalNamesID             (dc.goalNamesID)
{
    parameterCount = dc.parameterCount;
}


DifferentialCorrector& 
    DifferentialCorrector::operator=(const DifferentialCorrector& dc)
{
    if (&dc == this)
        return *this;
        
    return *this;
}


// Access methods overriden from the base class

std::string DifferentialCorrector::GetParameterText(const Integer id) const
{
    if (id == solverTextFileID)
        return "TargeterTextFile";
        
    if (id == variableNamesID)
        return "Variables";
        
    if (id == goalNamesID)
        return "Goals";
        
    return Solver::GetParameterText(id);
}


Integer DifferentialCorrector::GetParameterID(const std::string &str) const
{
    if (str == "TargeterTextFile")
        return solverTextFileID;
        
    if (str == "Variables")
        return variableNamesID;
        
    if (str == "Goals")
        return goalNamesID;
        
    return Solver::GetParameterID(str);
}


Gmat::ParameterType DifferentialCorrector::GetParameterType(const Integer id) const
{
    if (id == solverTextFileID)
        return Gmat::STRING_TYPE;
        
    if (id == variableNamesID)
        return Gmat::STRINGARRAY_TYPE;
        
    if (id == goalNamesID)
        return Gmat::STRINGARRAY_TYPE;
        
    return Solver::GetParameterType(id);
}


std::string DifferentialCorrector::GetParameterTypeString(const Integer id) const
{
    if (id == solverTextFileID)
        return GmatBase::PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == variableNamesID)
        return GmatBase::PARAM_TYPE_STRING[Gmat::STRINGARRAY_TYPE];
        
    if (id == goalNamesID)
        return GmatBase::PARAM_TYPE_STRING[Gmat::STRINGARRAY_TYPE];
        
    return Solver::GetParameterTypeString(id);
}


std::string DifferentialCorrector::GetStringParameter(const Integer id) const
{
    if (id == solverTextFileID)
        return solverTextFile;
        
    return Solver::GetStringParameter(id);
}


bool DifferentialCorrector::SetStringParameter(const Integer id, 
                                               const std::string &value)
{
    if (id == solverTextFileID) {
        solverTextFile = value;
        return true;
    }
        
    if (id == variableNamesID) {
        variableNames.push_back(value);
        return true;
    }
    
    if (id == goalNamesID) {
        goalNames.push_back(value);
        return true;
    }
    
    return Solver::SetStringParameter(id, value);
}


const StringArray& DifferentialCorrector::GetStringArrayParameter(const Integer id) const
{
    if (id == variableNamesID)
        return variableNames;
        
    if (id == goalNamesID)
        return goalNames;
        
    return Solver::GetStringArrayParameter(id);
}


bool DifferentialCorrector::Initialize(void)
{
    // Setup the variable data structures
    variableCount = variableNames.size();
    goalCount = goalNames.size();
    
    if (goalCount > variableCount) {
        std::string errorMessage = "Targeter cannot initialize: ";
        errorMessage += "More goals than variables";
        throw SolverException(errorMessage);
    }
    
    FreeArrays();
    
    variable            = new Real[variableCount];
    perturbation        = new Real[variableCount];
    variableMinimum     = new Real[variableCount];
    variableMaximum     = new Real[variableCount];
    variableMaximumStep = new Real[variableCount];
    
    // Setup the goal data structures
    goal      = new Real[goalCount];
    tolerance = new Real[goalCount];
    nominal   = new Real[goalCount];
    
    // And the sensitivity matrix
    Integer i;
    achieved        = new Real*[goalCount];
    jacobian        = new Real*[variableCount];
    inverseJacobian = new Real*[variableCount];
    for (i = 0; i < variableCount; ++i) {
        jacobian[i] = new Real[variableCount];
        inverseJacobian[i] = new Real[variableCount];
        achieved[i] = new Real[variableCount];
        
        // Initialize to the identity matrix
        jacobian[i][i] = 1.0;
        inverseJacobian[i][i] = 1.0;
        
        // Set default values for min and max parameters
        variableMinimum[i]     = -9.999e300;
        variableMaximum[i]     =  9.999e300;
        variableMaximumStep[i] =  9.999e300;
    }
    
    return false;
}


void DifferentialCorrector::FreeArrays(void)
{
    if (variable) {
        delete [] variable;
        variable = NULL;
    }
    
    if (perturbation) {
        delete [] perturbation;
        perturbation = NULL;
    }
            
    if (variableMinimum) {
        delete [] variableMinimum;
        variableMinimum = NULL;
    }

    if (variableMaximum) {
        delete [] variableMaximum;
        variableMaximum = NULL;
    }

    if (variableMaximumStep) {
        delete [] variableMaximumStep;
        variableMaximumStep = NULL;
    }

    if (goal) {
        delete [] goal;
        goal = NULL;
    }

    if (tolerance) {
        delete [] tolerance;
        tolerance = NULL;
    }

    if (nominal) {
        delete [] nominal;
        nominal = NULL;
    }
    
    if (achieved) {
        for (Integer i = 0; i < goalCount; ++i)
            delete [] achieved[i];
        delete [] achieved;
        achieved = NULL;
    }

    if (jacobian) {
        for (Integer i = 0; i < goalCount; ++i)
            delete [] jacobian[i];
        delete [] jacobian;
        jacobian = NULL;
    }

    if (inverseJacobian) {
        for (Integer i = 0; i < goalCount; ++i)
            delete [] inverseJacobian[i];
        delete [] inverseJacobian;
        inverseJacobian = NULL;
    }
}


void DifferentialCorrector::RunNominal(void)
{
    // On success, set the state to the next machine state
    
}


void DifferentialCorrector::RunPerturbation(void)
{
}


void DifferentialCorrector::CalculateParameters(void)
{
}


void DifferentialCorrector::CheckCompletion(void)
{
}


void DifferentialCorrector::WriteToTextFile(void)
{
}
