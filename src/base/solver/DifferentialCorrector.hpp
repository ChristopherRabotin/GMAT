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


/**
 * This class implements the first targeter in GMAT.
 * 
 * @todo refactor this class with the Solver class so that elements common to
 *       targeting, scanning, and optimizing are all in the base class.  This 
 *       task should be done when the first instance of one of the other 
 *       approaches is implemented.
 */
class DifferentialCorrector : public Solver 
{
public:
    DifferentialCorrector(std::string name);
    virtual ~DifferentialCorrector();
    DifferentialCorrector(const DifferentialCorrector &dc);
    DifferentialCorrector&      operator=(const DifferentialCorrector& dc);
    
    bool                        Initialize(void);

    // Access methods overriden from the base class
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer     GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;

    virtual std::string GetStringParameter(const Integer id) const;
    virtual bool        SetStringParameter(const Integer id, 
                                           const std::string &value);
    virtual const StringArray& 
                        GetStringArrayParameter(const Integer id) const; 

protected:
    // Core elements used for the targeter numerics
    /// The number of variables in the targeting problem
    Integer                     variableCount;
    /// The number of goals in the targeting problem
    Integer                     goalCount;
    /// The number of iterations taken (increments hen the matrix is inverted)
    Integer                     iterationsTaken;
    /// Maximum number of iterations allowed
    Integer                     maxIterations;
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
   
    // Control parameters
    /// Used to turn on central differencing.  Currently not implemented. 
    bool                        useCentralDifferences;
    /// Flag used to ensure the targeter is ready to go
    bool                        initialized;
    
    // Reporting parameters
    /// Name of the targeter text file.  An empty string turns the file off.
    std::string                 solverTextFile;
    /// List of variables
    StringArray                 variableNames;
    /// List of goals
    StringArray                 goalNames;
    
    virtual void                RunNominal(void); 
    virtual void                RunPerturbation(void);
    virtual void                CalculateParameters(void);
    virtual void                CheckCompletion(void);
//    virtual void                RunComplete(void);

    virtual void                WriteToTextFile(void);
    
    // Parameter IDs
    /// ID for the targeter text file name
    const Integer               solverTextFileID;
    /// ID for variable names
    const Integer               variableNamesID;
    /// ID for goal names
    const Integer               goalNamesID;
};

#endif // DifferentialCorrector_hpp
