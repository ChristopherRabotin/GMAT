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
class Vary : public Command
{
public:
    Vary(void);
    virtual ~Vary(void);
    
    Vary(const Vary& t);
    Vary&             operator=(const Vary& t);
    
    // Inherited methods overridden from the base class
    virtual void        InterpretAction(void);
    virtual bool        Initialize(void);
    virtual bool        Execute(void);
    
protected:
    /// The name of the spacecraft that gets maneuvered
    std::string         targeterName;
    /// The Varyer instance used to manage the Varyer state machine
    Solver              *targeter;
    
    // Parameter IDs 
    /// ID for the burn object
    Integer             targeterNameID;
    
};


#endif  // Vary_hpp
