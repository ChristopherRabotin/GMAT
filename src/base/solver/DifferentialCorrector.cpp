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
    if (variable)
        delete [] variable;
    
    if (perturbation)
        delete [] perturbation;
            
    if (variableMinimum)
        delete [] variableMinimum;

    if (variableMaximum)
        delete [] variableMaximum;

    if (variableMaximumStep)
        delete [] variableMaximumStep;

    if (goal)
        delete [] goal;

    if (tolerance)
        delete [] tolerance;

    if (nominal)
        delete [] nominal;

    if (achieved) {
        for (Integer i = 0; i < goalCount; ++i)
            delete [] achieved[i];
        delete [] achieved;
    }

    if (jacobian) {
        for (Integer i = 0; i < goalCount; ++i)
            delete [] jacobian[i];
        delete [] jacobian;
    }

    if (inverseJacobian) {
        for (Integer i = 0; i < goalCount; ++i)
            delete [] inverseJacobian[i];
        delete [] inverseJacobian;
    }
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
    return false;
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
