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


#include "Vary.hpp"


//------------------------------------------------------------------------------
//  Vary(void)
//------------------------------------------------------------------------------
/**
 * Creates a Vary command.  (default constructor)
 */
//------------------------------------------------------------------------------
Vary::Vary(void) :
    Command         ("Vary"),
    targeterName    (""),
    targeter        (NULL),
    targeterNameID  (parameterCount)
{
    parameterCount += 1;
}


//------------------------------------------------------------------------------
//  ~Vary(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Vary command.  (destructor)
 */
//------------------------------------------------------------------------------
Vary::~Vary(void)
{}

    
//------------------------------------------------------------------------------
//  Vary(const Vary& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Vary command.  (Copy constructor)
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Vary::Vary(const Vary& t) :
    Command         (t),
    targeterName    (t.targeterName),
    targeter        (NULL),
    targeterNameID  (t.parameterCount)
{
    parameterCount = t.parameterCount;
}


//------------------------------------------------------------------------------
//  Vary& operator=(const Vary& t)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Vary command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Vary& Vary::operator=(const Vary& t)
{
    if (this == &t)
        return *this;
    
    return *this;
}


//------------------------------------------------------------------------------
//  void Vary::InterpretAction(void)
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Vary command has the following syntax:
 *
 *     Vary myDC;
 *
 * where myDC is a Solver used to Vary a set of variables to achieve the
 * corresponding goals.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
void Vary::InterpretAction(void)
{
    /// @todo: Clean up this hack for the Vary::InterpretAction method
    // Sample string:  "Vary myDC(Burn1.V = 0.5, {Pert = 0.0001, MaxStep = 0.05, 
    //                  Lower = 0.0, Upper = 3.14159});"

    // Set starting location to the space following the command string
    Integer loc = generatingString.find("Vary", 0) + 6, end;
    const char *str = generatingString.c_str();

    // Skip white space
    while (str[loc] == ' ')
        ++loc;

    // Stop at the semicolon if it is there
    end = generatingString.find(";", loc);

    std::string component = generatingString.substr(loc, end-loc);
    if (component == "")
        throw CommandException("Vary string does identify not the targeter");
    SetStringParameter(targeterNameID, component);
}


//------------------------------------------------------------------------------
//  bool Initialize(void)
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Varyer.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Vary::Initialize(void)
{
    bool retval = Command::Initialize();

    // Vary specific initialization (no pun intended) goes here:
    
    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Vary the variables defined for this Varying loop.
 *
 * This method (will eventually) runs the Varyer state machine in order to
 * determine the variable values needed to achieve the user specified 
 * goals.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Vary::Execute(void)
{
    bool retval = true;
    
    return retval;
}
