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
    Command                 ("Vary"),
    targeterName            (""),
    targeter                (NULL),
    targeterNameID          (parameterCount),
    variableNameID          (parameterCount+1),
    initialValueID          (parameterCount+2),
    perturbationID          (parameterCount+3),
    variableMinimumID       (parameterCount+4),
    variableMaximumID       (parameterCount+5),
    variableMaximumStepID   (parameterCount+6)
{
    parameterCount += 7;
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
    Command                 (t),
    targeterName            (t.targeterName),
    targeter                (NULL),
    targeterNameID          (t.targeterNameID),
    variableNameID          (t.variableNameID),
    initialValueID          (t.initialValueID),
    perturbationID          (t.perturbationID),
    variableMinimumID       (t.variableMinimumID),
    variableMaximumID       (t.variableMaximumID),
    variableMaximumStepID   (t.variableMaximumStepID)
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
        
    targeterName = t.targeterName;
    return *this;
}


std::string Vary::GetParameterText(const Integer id) const
{
    if (id == targeterNameID)
        return "TargeterName";
        
    if (id == variableNameID)
        return "Variable";
        
    if (id == initialValueID)
        return "InitialValue";
        
    if (id == perturbationID)
        return "Perturbation";
        
    if (id == variableMinimumID)
        return "MinimumValue";
        
    if (id == variableMaximumID)
        return "MaximumValue";
        
    if (id == variableMaximumStepID)
        return "MaximumChange";

    return Command::GetParameterText(id);
}


Integer Vary::GetParameterID(const std::string &str) const
{
    if (str == "TargeterName")
        return targeterNameID;
        
    if (str == "Variable")
        return variableNameID;
        
    if (str == "InitialValue")
        return initialValueID;
        
    if (str == "Perturbation")
        return perturbationID;
        
    if (str == "MinimumValue")
        return variableMinimumID;
        
    if (str == "MaximumValue")
        return variableMaximumID;
        
    if (str == "MaximumChange")
        return variableMaximumStepID;

    return Command::GetParameterID(str);
}


Gmat::ParameterType Vary::GetParameterType(const Integer id) const
{
    if (id == targeterNameID)
        return Gmat::STRING_TYPE;
        
    if (id == variableNameID)
        return Gmat::STRING_TYPE;
        
    if (id == initialValueID)
        return Gmat::REAL_TYPE;
        
    if (id == perturbationID)
        return Gmat::REAL_TYPE;
        
    if (id == variableMinimumID)
        return Gmat::REAL_TYPE;
        
    if (id == variableMaximumID)
        return Gmat::REAL_TYPE;
        
    if (id == variableMaximumStepID)
        return Gmat::REAL_TYPE;

    return Command::GetParameterType(id);
}


std::string Vary::GetParameterTypeString(const Integer id) const
{
    if (id == targeterNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == targeterNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == variableNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == initialValueID)
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
        
    if (id == perturbationID)
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
        
    if (id == variableMinimumID)
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
        
    if (id == variableMaximumID)
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];
        
    if (id == variableMaximumStepID)
        return PARAM_TYPE_STRING[Gmat::REAL_TYPE];


    return Command::GetParameterTypeString(id);
}


Real Vary::GetRealParameter(const Integer id) const
{
    if (id == initialValueID)
        if (!initialValue.empty())
            return initialValue[0];
        
    if (id == perturbationID)
        if (!perturbation.empty())
            return perturbation[0];
        
    if (id == variableMinimumID)
        if (!variableMinimum.empty())
            return variableMinimum[0];
        
    if (id == variableMaximumID)
        if (!variableMaximum.empty())
            return variableMaximum[0];
        
    if (id == variableMaximumStepID)
        if (!variableMaximumStep.empty())
            return variableMaximumStep[0];

    return Command::GetRealParameter(id);
}


Real Vary::SetRealParameter(const Integer id, const Real value)
{
    if (id == initialValueID) {
        if (!initialValue.empty())
            initialValue[0] = value;
        else
            initialValue.push_back(value);
        return initialValue[0];
    }
            
        
    if (id == perturbationID) {
        if (!perturbation.empty())
            perturbation[0] = value;
        else
            perturbation.push_back(value);
        return perturbation[0];
    }
        
    if (id == variableMinimumID) {
        if (!variableMinimum.empty())
            variableMinimum[0] = value;
        else
            variableMinimum.push_back(value);
        return variableMinimum[0];
    }
        
    if (id == variableMaximumID) {
        if (!variableMaximum.empty())
            variableMaximum[0] = value;
        else
            variableMaximum.push_back(value);
        return variableMaximum[0];
    }
        
    if (id == variableMaximumStepID) {
        if (!variableMaximumStep.empty())
            variableMaximumStep[0] = value;
        else
            variableMaximumStep.push_back(value);
        return variableMaximumStep[0];
    }

    return Command::SetRealParameter(id, value);
}


std::string Vary::GetStringParameter(const Integer id) const
{
    if (id == targeterNameID)
        return targeterName;
        
    if (id == variableNameID)
        if (!variableName.empty())
            return variableName[0];
    
    return Command::GetStringParameter(id);
}


bool Vary::SetStringParameter(const Integer id, const std::string &value)
{
    if (id == targeterNameID) {
        targeterName = value;
        return true;
    }
    
    if (id == variableNameID) {
        if (!variableName.empty())
            variableName[0] = value;
        else
            variableName.push_back(value);
        return true;
    }
    
    return Command::SetStringParameter(id, value);
}



//const StringArray& Vary::GetStringArrayParameter(const Integer id) const; 

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
    Integer loc = generatingString.find("Vary", 0) + 4, end, strend;
    const char *str = generatingString.c_str();

    // Skip white space
    while (str[loc] == ' ')
        ++loc;

    // Stop at the opening paren
    end = generatingString.find("(", loc);

    std::string component = generatingString.substr(loc, end-loc);
    if (component == "")
        throw CommandException("Vary string does not identify the targeter");
    SetStringParameter(targeterNameID, component);
    
    // Find the variable
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
    variableName.push_back(component);

    // Find the variable
    loc = end + 1;
    
    Real value = atof(&str[loc]);
    SetRealParameter(initialValueID, value);

    // Find perts
    loc = generatingString.find("Pert", strend);
    end = generatingString.find("=", loc);
    value = atof(&str[end+1]);
    SetRealParameter(perturbationID, value);
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
    // Find the targeter
        if (objectMap->find(targeterName) == objectMap->end()) {
        std::string errorString = "Target command cannot find targeter \"";
        errorString += targeterName;
        errorString += "\"";
        throw CommandException(errorString);
    }

    targeter = (Solver *)((*objectMap)[targeterName]);
    Integer id = targeter->GetParameterID("Variables");
    if (!variableName.empty())
        targeter->SetStringParameter(id, variableName[0]);
    
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
