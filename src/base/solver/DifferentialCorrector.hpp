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
 * Definition for the differential corrector targeter. 
 */
//------------------------------------------------------------------------------


#ifndef DifferentialCorrector_hpp
#define DifferentialCorrector_hpp


#include "Solver.hpp"


class DifferentialCorrector : public Solver {
public:
    DifferentialCorrector(std::string name);
    virtual ~DifferentialCorrector();
    DifferentialCorrector(const DifferentialCorrector &dc);
    DifferentialCorrector&      operator=(const DifferentialCorrector& dc);
    
protected:
    virtual void                RunNominal(void);
    virtual void                RunPerturbation(void);
    virtual void                CalculateParameters(void);
    virtual void                CheckCompletion(void);
//    virtual void                RunComplete(void);
};

#endif // DifferentialCorrector_hpp
