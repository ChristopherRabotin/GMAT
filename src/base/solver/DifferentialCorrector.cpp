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
    initialized             (false)
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
    variableCount           (0),
    goalCount               (0),
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
    initialized             (false)
{
}


DifferentialCorrector& 
    DifferentialCorrector::operator=(const DifferentialCorrector& dc)
{
    if (&dc == this)
        return *this;
        
    return *this;
}


bool                        Initialize(void)
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
