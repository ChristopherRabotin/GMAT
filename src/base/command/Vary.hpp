//$Header$
//------------------------------------------------------------------------------
//                                   Vary
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/22
//
/**
 * Definition for the Vary command class
 */
//------------------------------------------------------------------------------


#ifndef Vary_hpp
#define Vary_hpp
 

#include "BranchCommand.hpp"
#include "Solver.hpp"


/**
 * Command that manages processing for entry to the Varyer loop
 *
 * The Vary command manages the Varyer loop.  All Varyers implement a state
 * machine that evaluates the current state of the Varying process, and provides 
 * data to the command sequence about the next step to be taken in the Varying
 * process.  As far as the Vary command is concerned, there are 3 possible steps 
 * to take: 
 *
 * 1.  Fire the Varyer to perform a calculation.
 *
 * 2.  Run through the Commands in the Varyer loop.
 *
 * 3.  On convergence, continue with the command sequence following the Varyer loop. 
 *
 * 
 *
 */
class Vary : public GmatCommand
{
public:
    Vary(void);
    virtual ~Vary(void);
    
    Vary(const Vary& t);
    Vary&             operator=(const Vary& t);

    // inherited from GmatBase
    virtual GmatBase* Clone(void) const;

    // Parameter accessors
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer     GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;

    virtual Real        GetRealParameter(const Integer id) const;
    virtual Real        SetRealParameter(const Integer id,
                                         const Real value);
    virtual std::string GetStringParameter(const Integer id) const;
    virtual bool        SetStringParameter(const Integer id, 
                                           const std::string &value);

// Multiple variables specified on the same line are not allowed in build 2
//    virtual const StringArray& 
//                        GetStringArrayParameter(const Integer id) const; 

    // Inherited methods overridden from the base class
    virtual void        InterpretAction(void);
    virtual bool        Initialize(void);
    virtual bool        Execute(void);
    
protected:
    /// The name of the spacecraft that gets maneuvered
    std::string         targeterName;
    /// Name(s) of the variable(s)
    StringArray         variableName;
    /// Initial variable value(s)
    std::vector<Real>   initialValue;
    /// Current (nominal) variable value(s)
    std::vector<Real>   currentValue;
    /// Variable perturbation(s)
    std::vector<Real>   perturbation;
    /// Absolute minimum value
    std::vector<Real>   variableMinimum;
    /// Absolute maximum value
    std::vector<Real>   variableMaximum;
    /// Maximum step allowed
    std::vector<Real>   variableMaximumStep;
    /// Targeter ID for the parameters
    std::vector<Integer> variableId;
    /// Pointers to the objects that the variables affect
    std::vector<GmatBase*> pobject;
    /// Object ID for the parameters
    std::vector<Integer> parmId;
    /// The Targeter instance used to manage the state machine
    Solver              *targeter;
    /// The integer ID assigned to the variable
    Integer             variableID;
    /// Flag used to finalize the targeter data during execution
    bool                targeterDataFinalized;
    
    // Parameter IDs 
    /// ID for the burn object
    const Integer       targeterNameID;
    /// ID for the burn object
    const Integer       variableNameID;
    /// ID for the burn object
    const Integer       initialValueID;
    /// ID for the burn object
    const Integer       perturbationID;
    /// ID for the burn object
    const Integer       variableMinimumID;
    /// ID for the burn object
    const Integer       variableMaximumID;
    /// ID for the burn object
    const Integer       variableMaximumStepID;
};


#endif  // Vary_hpp
