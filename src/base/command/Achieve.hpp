//$Header$
//------------------------------------------------------------------------------
//                                  Achieve
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
 * Definition for the Achieve command class
 */
//------------------------------------------------------------------------------


#ifndef Achieve_hpp
#define Achieve_hpp
 

#include "Command.hpp"
#include "Solver.hpp"


/**
 * Command that manages processing for entry to the Achieveer loop
 *
 * The Achieve command manages the Achieveer loop.  All Achieveers implement a state
 * machine that evaluates the current state of the Achieveing process, and provides 
 * data to the command sequence about the next step to be taken in the Achieveing
 * process.  As far as the Achieve command is concerned, there are 3 possible steps 
 * to take: 
 *
 * 1.  Fire the Achieveer to perform a calculation.
 *
 * 2.  Run through the Commands in the Achieveer loop.
 *
 * 3.  On convergence, continue with the command sequence following the Achieveer loop. 
 *
 * 
 *
 */
class Achieve : public GmatCommand
{
public:
    Achieve(void);
    virtual ~Achieve(void);
    
    Achieve(const Achieve& t);
    Achieve&             operator=(const Achieve& t);
    
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

    // Inherited methods overridden from the base class
    virtual void        InterpretAction(void);
    virtual bool        Initialize(void);
    virtual bool        Execute(void);
    
protected:
    /// The name of the spacecraft that gets maneuvered
    std::string         targeterName;
    /// Name of the goal
    std::string         goalName;
    /// Target value for the goal
    Real                goal;
    /// Accuracy needed for the goal
    Real                tolerance;
    /// Pointer to the object that owns the goal
    GmatBase            *goalObject;
    /// Object ID for the parameter
    Integer             parmId;
    /// Targeter ID for the goal 
    Integer             goalId;
    /// The targeter instance used to manage the targeter state machine
    Solver              *targeter;
    /// Flag used to finalize the targeter data during execution
    bool                targeterDataFinalized;
    
    // Parameter IDs 
    /// ID for the burn object
    const Integer       targeterNameID;
    /// ID for the goal name
    const Integer       goalNameID;
    /// ID for the goal
    const Integer       goalID;
    /// ID for the tolerance
    const Integer       toleranceID;
};


#endif  // Achieve_hpp
