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
class Achieve : public Command
{
public:
    Achieve(void);
    virtual ~Achieve(void);
    
    Achieve(const Achieve& t);
    Achieve&             operator=(const Achieve& t);
    
    // Inherited methods overridden from the base class
    virtual void        InterpretAction(void);
    virtual bool        Initialize(void);
    virtual bool        Execute(void);
    
protected:
    /// The name of the spacecraft that gets maneuvered
    std::string         targeterName;
    /// The Achieveer instance used to manage the Achieveer state machine
    Solver              *targeter;
    
    // Parameter IDs 
    /// ID for the burn object
    Integer             targeterNameID;
    
};


#endif  // Achieve_hpp
