//$Header$
//------------------------------------------------------------------------------
//                                 Achieve
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/26
//
/**
 * Definition for the Achieve command class
 */
//------------------------------------------------------------------------------


#include "Achieve.hpp"


//------------------------------------------------------------------------------
//  Achieve(void)
//------------------------------------------------------------------------------
/**
 * Creates a Achieve command.  (default constructor)
 */
//------------------------------------------------------------------------------
Achieve::Achieve(void) :
    Command         ("Achieve"),
    targeterName    (""),
    targeter        (NULL),
    targeterNameID  (parameterCount)
{
    parameterCount += 1;
}


//------------------------------------------------------------------------------
//  ~Achieve(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Achieve command.  (destructor)
 */
//------------------------------------------------------------------------------
Achieve::~Achieve(void)
{}

    
//------------------------------------------------------------------------------
//  Achieve(const Achieve& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Achieve command.  (Copy constructor)
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Achieve::Achieve(const Achieve& t) :
    Command         (t),
    targeterName    (t.targeterName),
    targeter        (NULL),
    targeterNameID  (t.parameterCount)
{
    parameterCount = t.parameterCount;
}


//------------------------------------------------------------------------------
//  Achieve& operator=(const Achieve& t)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Achieve command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Achieve& Achieve::operator=(const Achieve& t)
{
    if (this == &t)
        return *this;
    
    return *this;
}


//------------------------------------------------------------------------------
//  void Achieve::InterpretAction(void)
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Achieve command has the following syntax:
 *
 *     Achieve myDC;
 *
 * where myDC is a Solver used to Achieve a set of variables to achieve the
 * corresponding goals.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
void Achieve::InterpretAction(void)
{
    /// @todo: Clean up this hack for the Achieve::InterpretAction method
    // Sample string:  "Achieve myDC;"
    
    // Set starting location to the space following the command string
    Integer loc = generatingString.find("Achieve", 0) + 6, end;
    const char *str = generatingString.c_str();
    
    // Skip white space
    while (str[loc] == ' ')
        ++loc;
    
    // Stop at the semicolon if it is there
    end = generatingString.find(";", loc);
    
    
    std::string component = generatingString.substr(loc, end-loc);
    if (component == "")
        throw CommandException("Achieve string does not the Achieveer");
    SetStringParameter(targeterNameID, component);
}


//------------------------------------------------------------------------------
//  bool Initialize(void)
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Achieveer.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Achieve::Initialize(void)
{
    bool retval = Command::Initialize();

    // Achieve specific initialization goes here:
    
    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Achieve the variables defined for this Achieveing loop.
 *
 * This method (will eventually) runs the Achieveer state machine in order to
 * determine the variable values needed to achieve the user specified 
 * goals.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Achieve::Execute(void)
{
    bool retval = true;
    return retval;
}
