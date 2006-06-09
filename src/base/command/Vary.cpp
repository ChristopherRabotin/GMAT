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
#include "DifferentialCorrector.hpp"
#include "MessageInterface.hpp"
#include <sstream>         // for std::stringstream


//#define DEBUG_VARIABLE_RANGES
//#define DEBUG_VARY_EXECUTE 1

//------------------------------------------------------------------------------
//  Vary(void)
//------------------------------------------------------------------------------
/**
 * Creates a Vary command.  (default constructor)
 */
//------------------------------------------------------------------------------
Vary::Vary() :
    GmatCommand             ("Vary"),
    targeterName            (""),
    targeter                (NULL),
    variableID              (-1),
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
Vary::~Vary()
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
    GmatCommand             (t),
    targeterName            (t.targeterName),
    variableName            (t.variableName),
    targeter                (NULL),
    variableID              (-1),
    targeterDataFinalized   (t.targeterDataFinalized),
    targeterNameID          (t.targeterNameID),
    variableNameID          (t.variableNameID),
    initialValueID          (t.initialValueID),
    perturbationID          (t.perturbationID),
    variableMinimumID       (t.variableMinimumID),
    variableMaximumID       (t.variableMaximumID),
    variableMaximumStepID   (t.variableMaximumStepID)
{
    parameterCount = t.parameterCount;
    
    initialValue.clear();
    currentValue.clear();
    perturbation.clear();
    variableMinimum.clear();
    variableMaximum.clear();
    variableMaximumStep.clear();
    variableId.clear();
    pobject.clear();
    parmId.clear();
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
    variableName = t.variableName;
    targeter = NULL;
    variableID = -1;
    targeterDataFinalized = t.targeterDataFinalized;
    
    initialValue.clear();
    currentValue.clear();
    perturbation.clear();
    variableMinimum.clear();
    variableMaximum.clear();
    variableMaximumStep.clear();
    variableId.clear();
    pobject.clear();
    parmId.clear();
    
    return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Vary.
 *
 * @return clone of the Vary.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Vary::Clone() const
{
   return (new Vary(*this));
}


//------------------------------------------------------------------------------
//  const std::string& GetGeneratingString()
//------------------------------------------------------------------------------
/**
 * Method used to retrieve the string that was parsed to build this GmatCommand.
 *
 * This method is used to retrieve the GmatCommand string from the script that
 * was parsed to build the GmatCommand.  It is used to save the script line, so
 * that the script can be written to a file without inverting the steps taken to
 * set up the internal object data.  As a side benefit, the script line is
 * available in the GmatCommand structure for debugging purposes.
 *
 * @param mode    Specifies the type of serialization requested.
 * @param prefix  Optional prefix appended to the object's name.  (Used to
 *                indent commands)
 * @param useName Name that replaces the object's name.  (Not used in
 *                commands)
 *
 * @return The script line that, when interpreted, defines this Vary command.
 */
//------------------------------------------------------------------------------
const std::string& Vary::GetGeneratingString(Gmat::WriteMode mode,
                                             const std::string &prefix,
                                             const std::string &useName)
{
   // Build the local string
   std::stringstream details;
   std::string gen = prefix + "Vary " + targeterName + "(";

   // Iterate through the variables
   for (UnsignedInt i = 0; i < variableName.size(); ++i)
   {
      details << variableName[i] << " = " << initialValue[i] <<  ", ";
   }

   details << "{Pert =";
   // Toss in the perturbations
   for (UnsignedInt i = 0; i < perturbation.size(); ++i)
   {
      details << " " << perturbation[i];
   }
   
   details << ", MaxStep =";
   for (UnsignedInt i = 0; i < variableMaximumStep.size(); ++i)
   {
      details << " ";
      details << variableMaximumStep[i];
   }

   details << ", Lower =";
   for (UnsignedInt i = 0; i < variableMinimum.size(); ++i)
   {
      details << " ";
      details << variableMinimum[i];
   }

   details << ", Upper =";
   for (UnsignedInt i = 0; i < variableMaximum.size(); ++i)
   {
      details << " ";
      details << variableMaximum[i];
   }

   gen += details.str();
   generatingString = gen + "});";
   // Then call the base class method
   return GmatCommand::GetGeneratingString();
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Vary::RenameRefObject(const Gmat::ObjectType type,
                           const std::string &oldName,
                           const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (targeterName == oldName)
         targeterName = newName;
   }
   else if (type == Gmat::PARAMETER)
   {
      for (unsigned int i=0; i<variableName.size(); i++)
      {
         if (variableName[i] == oldName)
            variableName[i] = newName;
      }
   }
   
   return true;
}

//---------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//---------------------------------------------------------------------------
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

    return GmatCommand::GetParameterText(id);
}


//---------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//---------------------------------------------------------------------------
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

    return GmatCommand::GetParameterID(str);
}


//---------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
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

    return GmatCommand::GetParameterType(id);
}


//---------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//---------------------------------------------------------------------------
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


    return GmatCommand::GetParameterTypeString(id);
}


//---------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
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

    return GmatCommand::GetRealParameter(id);
}


//---------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
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

    return GmatCommand::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string Vary::GetStringParameter(const Integer id) const
{
    if (id == targeterNameID)
        return targeterName;
        
    if (id == variableNameID)
        if (!variableName.empty())
            return variableName[0];
    
    return GmatCommand::GetStringParameter(id);
}


//---------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
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
    
    return GmatCommand::SetStringParameter(id, value);
}



//const StringArray& Vary::GetStringArrayParameter(const Integer id) const; 

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                                     const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets referenced objects.
 *
 * @param obj reference object pointer.
 * @param type type of the reference object.
 * @param name name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool Vary::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                        const std::string &name)
{
   if (type == Gmat::SOLVER) {
      if (targeterName == obj->GetName()) {
         targeter = (Solver*)obj;
         return true;
      }
      return false;
   }
   return GmatCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  void Vary::InterpretAction()
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
bool Vary::InterpretAction()
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
    variableId.push_back(-1);
    
    // Find the variable
    loc = end + 1;
    
    Real value = atof(&str[loc]);
    SetRealParameter(initialValueID, value);

    // Find perts
    loc = generatingString.find("Pert", strend);
    end = generatingString.find("=", loc);
    value = atof(&str[end+1]);
    SetRealParameter(perturbationID, value);
    
    // Min, max and step get default values unless they are specified
    value = 9.999e300;
    SetRealParameter(variableMinimumID, -value);
    SetRealParameter(variableMaximumID, value);
    SetRealParameter(variableMaximumStepID, value);
    
    #ifdef DEBUG_VARIABLE_RANGES
       MessageInterface::ShowMessage(
          "Default Min, Max, Step = [%le  %le  %le]\n",
          variableMinimum[0], variableMaximum[0], variableMaximumStep[0]);
    #endif
    
    loc = generatingString.find("MaxStep", strend);
    if ((UnsignedInt)loc != std::string::npos)
    {
       end = generatingString.find("=", loc);
       value = atof(&str[end+1]);
       SetRealParameter(variableMaximumStepID, value);
    }

    loc = generatingString.find("Lower", strend);
    if ((UnsignedInt)loc != std::string::npos)
    {
       end = generatingString.find("=", loc);
       value = atof(&str[end+1]);
       SetRealParameter(variableMinimumID, value);
    }

    loc = generatingString.find("Upper", strend);
    if ((UnsignedInt)loc != std::string::npos)
    {
       end = generatingString.find("=", loc);
       value = atof(&str[end+1]);
       SetRealParameter(variableMaximumID, value);
    }
    
    #ifdef DEBUG_VARIABLE_RANGES
       MessageInterface::ShowMessage("Min, Max, Step = [%le  %le  %le]\n",
          variableMinimum[0], variableMaximum[0], variableMaximumStep[0]);
    #endif
    
    return true;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Varyer.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Vary::Initialize()
{
    bool retval = GmatCommand::Initialize();

//    // Vary specific initialization (no pun intended) goes here:
//    // Find the targeter
//    if (objectMap->find(targeterName) == objectMap->end()) {
//        std::string errorString = "Target command cannot find targeter \"";
//        errorString += targeterName;
//        errorString += "\"";
//        throw CommandException(errorString);
//    }
//
//    targeter = (Solver *)((*objectMap)[targeterName]);

    if (targeter == NULL)
       throw CommandException("Targeter not initialized for Vary command\n  \""
                              + generatingString + "\"\n");

    Integer id = targeter->GetParameterID("Variables");
    if (!variableName.empty())
        targeter->SetStringParameter(id, variableName[0]);
        
    // The targeter cannot be finalized until all of the loop is initialized
    targeterDataFinalized = false;

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

    #if DEBUG_VARY_EXECUTE
    MessageInterface::ShowMessage
       ("Vary::Execute() targeterDataFinalized=%d\n", targeterDataFinalized);
    #endif
    
    if (!targeterDataFinalized) {
        // First time through, tell the targeter about the variables
        Real varData[5];
        //for (Integer i = 0; i < variableName.size(); ++i) {
        Integer i = 0;
        {
            varData[0] = initialValue[i];           // Initial value
            varData[1] = perturbation[i];           // pert
            varData[2] = variableMinimum[i];        // minimum
            varData[3] = variableMaximum[i];        // maximum
            varData[4] = variableMaximumStep[i];    // largest allowed step
            
            if (variableId.empty())
               variableId.push_back(-1);
            
            variableId[i] = targeter->SetSolverVariables(varData, variableName[i]);
        }
        
        // Break component into the object and its parameter
        std::string objectName, parmName, varName;
        varName = variableName[0];
        Integer loc = varName.find(".");
        objectName = varName.substr(0, loc);
        parmName = varName.substr(loc+1, varName.length() - (loc+1));
        GmatBase *obj = (*objectMap)[objectName];
        
        #if DEBUG_VARY_EXECUTE
        MessageInterface::ShowMessage
           ("Vary::Execute() varName=%s, objectName=%s, parmName=%s\n",
            varName.c_str(), objectName.c_str(), parmName.c_str());
        #endif
        
        if (obj == NULL) {
            std::string errorstr = "Could not find object ";
            errorstr += objectName;
            throw CommandException(errorstr);
        }
        
        Integer id = obj->GetParameterID(parmName);

        if (id == -1) {
            std::string errorstr = "Could not find parameter ";
            errorstr += parmName;
            errorstr += " on object ";
            errorstr += objectName;
            throw CommandException(errorstr);
        }
        
        Gmat::ParameterType type = obj->GetParameterType(id);
        if (type != Gmat::REAL_TYPE) {
            std::string errorstr = "The targeter variable ";
            errorstr += parmName;
            errorstr += " on object ";
            errorstr += objectName;
            errorstr += " is not Real.";
            throw CommandException(errorstr);
        }
        
        pobject.push_back(obj);
        parmId.push_back(id);

        targeterDataFinalized = true;
        BuildCommandSummary(true);
        return retval;
    }
    
    Real var = targeter->GetSolverVariable(variableId[0]);
    
//    // Just a check here -- the targeter handles all of these now
//    if (variableMinimum[0] >= variableMaximum[0])
//       throw CommandException("Invalid variable minimum and maximum for " +
//          variableName[0]);
//
//    if (var > variableMaximum[0])
//       var = variableMaximum[0];
//    if (var < variableMinimum[0])
//       var = variableMinimum[0];

    #ifdef DEBUG_VARIABLE_RANGES
       MessageInterface::ShowMessage(
          "Setting %s to %lf; allowed range is [%lf, %lf]\n",
          variableName[0].c_str(), var, variableMinimum[0], variableMaximum[0]);
    #endif

    pobject[0]->SetRealParameter(parmId[0], var);

    BuildCommandSummary(true);
    return retval;
}
