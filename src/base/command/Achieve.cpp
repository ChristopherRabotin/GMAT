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
    targeterNameID  (parameterCount),
    goalNameID      (parameterCount+1),
    goalID          (parameterCount+2),
    toleranceID     (parameterCount+3)
{
    parameterCount += 4;
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
    targeterNameID  (t.targeterNameID),
    goalNameID      (t.goalNameID),
    goalID          (t.goalID),
    toleranceID     (t.toleranceID)
    
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


    // Parameter accessors
std::string Achieve::GetParameterText(const Integer id) const
{
    if (id == targeterNameID)
        return "TargeterName";
        
    if (id == goalNameID)
        return "Goal";
        
    if (id == goalID)
        return "GoalValue";
        
    if (id == toleranceID)
        return "Tolerance";
    
    return Command::GetParameterText(id);
}


Integer Achieve::GetParameterID(const std::string &str) const
{
    if (str == "TargeterName")
        return targeterNameID;
        
    if (str == "Goal")
        return goalNameID;
        
    if (str == "GoalValue")
        return goalID;
        
    if (str == "Tolerance")
        return toleranceID;
    
    return Command::GetParameterID(str);
}


Gmat::ParameterType Achieve::GetParameterType(const Integer id) const
{
    if (id == targeterNameID)
        return Gmat::STRING_TYPE;
        
    if (id == goalNameID)
        return Gmat::STRING_TYPE;
        
    if (id == goalID)
        return Gmat::REAL_TYPE;
        
    if (id == toleranceID)
        return Gmat::REAL_TYPE;
    
    return Command::GetParameterType(id);
}


std::string Achieve::GetParameterTypeString(const Integer id) const
{
    if (id == targeterNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == goalNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == goalID)
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
        
    if (id == toleranceID)
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
    
    return Command::GetParameterTypeString(id);
}


Real Achieve::GetRealParameter(const Integer id) const
{
    if (id == goalID)
        return goal;
        
    if (id == toleranceID)
        return tolerance;
    
    return Command::GetRealParameter(id);
}


Real Achieve::SetRealParameter(const Integer id, const Real value)
{
    if (id == goalID) {
        goal = value;
        return goal;
    }
        
    if (id == toleranceID) {
        tolerance = value;
        return tolerance;
    }
    
    return Command::SetRealParameter(id, value);
}


std::string Achieve::GetStringParameter(const Integer id) const
{
    if (id == targeterNameID)
        return targeterName;
        
    if (id == goalNameID)
        return goalName;
        
    return Command::GetStringParameter(id);
}


bool Achieve::SetStringParameter(const Integer id, const std::string &value)
{
    if (id == targeterNameID) {
        targeterName = value;
        return true;
    }

    if (id == goalNameID) {
        goalName = value;
        return true;
    }

    return Command::SetStringParameter(id, value);
}


// Multiple variables specified on the same line are not allowed in build 2
// const StringArray& Achieve::GetStringArrayParameter(const Integer id) const; 

//------------------------------------------------------------------------------
//  void InterpretAction(void)
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
    // Sample string:  "Achieve myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});"
    
    // Set starting location to the space following the command string
    Integer loc = generatingString.find("Achieve", 0) + 7, end, strend;
    const char *str = generatingString.c_str();
    
    // Skip white space
    while (str[loc] == ' ')
        ++loc;
    
    // Stop at the opening paren
    end = generatingString.find("(", loc);
    
    
    std::string component = generatingString.substr(loc, end-loc);
    if (component == "")
        throw CommandException("Achieve string does specify the targeter");
    SetStringParameter(targeterNameID, component);
    
    // Find the goal
    loc = end + 1;
    // Skip white space
    while (str[loc] == ' ')
        ++loc;
    // Stop at the opening paren
    end = generatingString.find("=", loc);
    strend = end-1;
    // Drop trailing white space
    while (str[strend] == ' ')
        --strend;
    
    component = generatingString.substr(loc, strend-loc+1);
    goalName = component;

    // Find the value
    loc = end + 1;
    
    Real value = atof(&str[loc]);
    SetRealParameter(goalID, value);

    // Find perts
    loc = generatingString.find("Tolerance", strend);
    end = generatingString.find("=", loc);
    value = atof(&str[end+1]);
    SetRealParameter(toleranceID, value);
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
    
    if (objectMap->find(targeterName) == objectMap->end()) {
        std::string errorString = "Target command cannot find targeter \"";
        errorString += targeterName;
        errorString += "\"";
        throw CommandException(errorString);
    }

    targeter = (Solver *)((*objectMap)[targeterName]);
    Integer id = targeter->GetParameterID("Goals");
    targeter->SetStringParameter(id, goalName);

    // The targeter cannot be finalized until all of the loop is initialized
    targeterDataFinalized = false;
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
    
    if (!targeterDataFinalized) {
        // Tell the targeter about the goals and tolerances
        Real goalData[2];
        goalData[0] = goal;
        goalData[1] = tolerance;
        targeter->SetSolverResults(goalData, goalName);
      
        targeterDataFinalized = true;
        return retval;
    }
    
    return retval;
}
