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
    Solver          ("DifferentialCorrector", name)
{
}


DifferentialCorrector::~DifferentialCorrector()
{
}


DifferentialCorrector::DifferentialCorrector(const DifferentialCorrector &dc) :
    Solver          (dc)
{
}


DifferentialCorrector& 
    DifferentialCorrector::operator=(const DifferentialCorrector& dc)
{
    if (&dc == this)
        return *this;
        
    return *this;
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
