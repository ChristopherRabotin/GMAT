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
    
    bool                        Initialize(void);
    
protected:
    /// The number of variables in the targeting problem
    Integer                     variableCount;
    /// The number of goals in the targeting problem
    Integer                     goalCount;
    /// Array used to track the variables in the targeting run
    Real                        *variable;
    /// Array used to track the perturbations on each variable
    Real                        *perturbation;
    /// Limits on the lowest value of the variables
    Real                        *variableMinimum;
    /// Limits on the lowest value of the variables
    Real                        *variableMaximum;
    /// Limits on individual changes in the variables
    Real                        *variableMaximumStep;
    /// Value desired for the goals
    Real                        *goal;
    /// Accuracy for the goals
    Real                        *tolerance;
    /// Array used to track the achieved value during a nominal run
    Real                        *nominal;
    /// Array used to track the achieved values when variables are perturbed
    Real                        **achieved;    
    /// The sensitivity matrix
    Real                        **jacobian;
    /// The inverted sensitivity matrix
    Real                        **inverseJacobian;
   
    /// Used to turn on central differencing.  Currently not implemented. 
    bool                        useCentralDifferences;
    /// Flag used to ensure the targeter is ready to go
    bool                        initialized;
    
    virtual void                RunNominal(void);
    virtual void                RunPerturbation(void);
    virtual void                CalculateParameters(void);
    virtual void                CheckCompletion(void);
//    virtual void                RunComplete(void);
};

#endif // DifferentialCorrector_hpp
