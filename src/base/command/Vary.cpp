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
#include "Parameter.hpp"
#include "StringUtil.hpp"  // for Replace()
#include "MessageInterface.hpp"
#include "TextParser.hpp"
#include <sstream>         // for std::stringstream


//#define DEBUG_VARIABLE_RANGES
//#define DEBUG_VARY_EXECUTE 1
//#define DEBUG_VARY_PARAMS
//#define DEBUG_VARY_PARSING

//------------------------------------------------------------------------------
//  static data
//------------------------------------------------------------------------------
const std::string Vary::PARAMETER_TEXT[VaryParamCount -
                                              GmatCommandParamCount] = 
{
   "SolverName",
   "Variable",
   "InitialValue",
   "Perturbation",
   "MinimumValue",
   "MaximumValue",
   "MaximumChange",
   "AdditiveScaleFactor",
   "MutiplicativeScaleFactor"   
};

const Gmat::ParameterType Vary::PARAMETER_TYPE[VaryParamCount -
                                               GmatCommandParamCount] = 
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
};
 

//------------------------------------------------------------------------------
//  Vary(void)
//------------------------------------------------------------------------------
/**
 * Creates a Vary command.  (default constructor)
 */
//------------------------------------------------------------------------------
Vary::Vary() :
    GmatCommand               ("Vary"),
    solverName                (""),
    solver                    (NULL),
    variableID                (-1)//,
    //solverNameID            (parameterCount),
    //variableNameID          (parameterCount+1),
    //initialValueID          (parameterCount+2),
    //perturbationID          (parameterCount+3),
    //variableMinimumID       (parameterCount+4),
    //variableMaximumID       (parameterCount+5),
    //variableMaximumStepID   (parameterCount+6)
{
    //parameterCount += 7;
    additiveScaleFactor.push_back(0.0);
    multiplicativeScaleFactor.push_back(1.0);
    parameterCount = VaryParamCount;
}


    
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
    GmatCommand               (t),
    solverName                (t.solverName),
    variableName              (t.variableName),
    solver                    (NULL),
    variableID                (-1),
    solverDataFinalized       (t.solverDataFinalized)//,
    //solverNameID            (t.solverNameID),
    //variableNameID          (t.variableNameID),
    //initialValueID          (t.initialValueID),
    //perturbationID          (t.perturbationID),
    //variableMinimumID       (t.variableMinimumID),
    //variableMaximumID       (t.variableMaximumID),
    //variableMaximumStepID   (t.variableMaximumStepID)
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
   //additiveScaleFactor.clear();
   //multiplicativeScaleFactor.clear();
   additiveScaleFactor       = t.additiveScaleFactor;
   multiplicativeScaleFactor = t.multiplicativeScaleFactor;
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
        
    solverName = t.solverName;
    variableName = t.variableName;
    solver = NULL;
    variableID = -1;
    solverDataFinalized = t.solverDataFinalized;
    
    initialValue.clear();
    currentValue.clear();
    perturbation.clear();
    variableMinimum.clear();
    variableMaximum.clear();
    variableMaximumStep.clear();
    variableId.clear();
    pobject.clear();
    parmId.clear();
   //additiveScaleFactor.clear();
   //multiplicativeScaleFactor.clear();
   additiveScaleFactor       = t.additiveScaleFactor;
   multiplicativeScaleFactor = t.multiplicativeScaleFactor;
    
    return *this;
}

//------------------------------------------------------------------------------
//  ~Vary(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Vary command.  (destructor)
 */
//------------------------------------------------------------------------------
Vary::~Vary()
{
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
   std::string gen = prefix + "Vary " + solverName + "(";

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

   //if (solver && (solver->IsOfType("Optimizer")))
   //{
      details << ", AdditiveScaleFactor =";
      for (UnsignedInt i = 0; i < additiveScaleFactor.size(); ++i)
      {
         details << " ";
         details << additiveScaleFactor[i];
      }
   
      details << ", MultiplicativeScaleFactor =";
      for (UnsignedInt i = 0; i < multiplicativeScaleFactor.size(); ++i)
      {
         details << " ";
         details << multiplicativeScaleFactor[i];
      }
   //}
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
   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Vary::RenameRefObject() type=%d, oldName=%s, newName=%s\n",
       type, oldName.c_str(), newName.c_str());
   #endif
   
   if (type == Gmat::SOLVER)
   {
      if (solverName == oldName)
         solverName = newName;
   }
   else if (type == Gmat::BURN)
   {
      for (unsigned int i=0; i<variableName.size(); i++)
      {
         if (variableName[i].find(oldName) != std::string::npos)
            variableName[i] =
               GmatStringUtil::Replace(variableName[i], oldName, newName);
      }
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
   if ((id >= GmatCommandParamCount) && (id < VaryParamCount))
      return PARAMETER_TEXT[id - GmatCommandParamCount];
      /*
    if (id == solverNameID)
        return "SolverName";
        
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
   */
    return GmatCommand::GetParameterText(id);
}


//---------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//---------------------------------------------------------------------------
Integer Vary::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < VaryParamCount; ++i)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }
   /*
    if (str == "SolverName")
        return solverNameID;
        
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
   */
    return GmatCommand::GetParameterID(str);
}


//---------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
Gmat::ParameterType Vary::GetParameterType(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < VaryParamCount))
      return PARAMETER_TYPE[id - GmatCommandParamCount];
    /*
    if (id == solverNameID)
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
   */
    return GmatCommand::GetParameterType(id);
}


//---------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//---------------------------------------------------------------------------
std::string Vary::GetParameterTypeString(const Integer id) const
{
   return GmatCommand::PARAM_TYPE_STRING[GetParameterType(id)];
   /*
    if (id == solverNameID)
        return PARAM_TYPE_STRING[Gmat::STRING_TYPE];
        
    if (id == solverNameID)
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
    */
}


//---------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
Real Vary::GetRealParameter(const Integer id) const
{
    //if (id == initialValueID)
    if (id == INITIAL_VALUE)
        if (!initialValue.empty())
            return initialValue[0];
        
    //if (id == perturbationID)
    if (id == PERTURBATION)
        if (!perturbation.empty())
            return perturbation[0];
        
    //if (id == variableMinimumID)
    if (id == VARIABLE_MINIMUM)
        if (!variableMinimum.empty())
            return variableMinimum[0];
        
    //if (id == variableMaximumID)
    if (id == VARIABLE_MAXIMUM)
        if (!variableMaximum.empty())
            return variableMaximum[0];
        
    //if (id == variableMaximumStepID)
    if (id == VARIABLE_MAXIMUM_STEP)
        if (!variableMaximumStep.empty())
            return variableMaximumStep[0];
            
   if (id == ADDITIVE_SCALE_FACTOR)
      if (!additiveScaleFactor.empty())
         return additiveScaleFactor[0];
         
   if (id == MULTIPLICATIVE_SCALE_FACTOR)
      if (!multiplicativeScaleFactor.empty())
         return multiplicativeScaleFactor[0];
         

    return GmatCommand::GetRealParameter(id);
}


//---------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//---------------------------------------------------------------------------
Real Vary::SetRealParameter(const Integer id, const Real value)
{
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage(
      "Vary::Setting value of %f for parameter %s\n", value,
      (GetParameterText(id)).c_str());
   #endif
    //if (id == initialValueID) 
    if (id == INITIAL_VALUE)
    {
        if (!initialValue.empty())
            initialValue[0] = value;
        else
            initialValue.push_back(value);
        return initialValue[0];
    }
            
    //if (id == perturbationID)
    if (id == PERTURBATION) 
    {
        if (!perturbation.empty())
            perturbation[0] = value;
        else
            perturbation.push_back(value);
        return perturbation[0];
    }
        
    //if (id == variableMinimumID) 
    if (id == VARIABLE_MINIMUM)
    {
        if (!variableMinimum.empty())
            variableMinimum[0] = value;
        else
            variableMinimum.push_back(value);
        return variableMinimum[0];
    }
        
    //if (id == variableMaximumID) 
    if (id == VARIABLE_MAXIMUM)
    {
        if (!variableMaximum.empty())
            variableMaximum[0] = value;
        else
            variableMaximum.push_back(value);
        return variableMaximum[0];
    }
        
    //if (id == variableMaximumStepID) 
    if (id == VARIABLE_MAXIMUM_STEP)
    {
        if (!variableMaximumStep.empty())
            variableMaximumStep[0] = value;
        else
            variableMaximumStep.push_back(value);
        return variableMaximumStep[0];
    }
    
   if (id == ADDITIVE_SCALE_FACTOR)
   {
      if (!additiveScaleFactor.empty())
         additiveScaleFactor[0] = value;
      else
         additiveScaleFactor.push_back(value);
      return additiveScaleFactor[0];
   }

   if (id == MULTIPLICATIVE_SCALE_FACTOR)
   {
      if (!multiplicativeScaleFactor.empty())
         multiplicativeScaleFactor[0] = value;
      else
         multiplicativeScaleFactor.push_back(value);
      return multiplicativeScaleFactor[0];
   }

    return GmatCommand::SetRealParameter(id, value);
}


//---------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//---------------------------------------------------------------------------
std::string Vary::GetStringParameter(const Integer id) const
{
    //if (id == solverNameID)
    if (id == SOLVER_NAME)
        return solverName;
        
    //if (id == variableNameID)
    if (id == VARIABLE_NAME)
        if (!variableName.empty())
            return variableName[0];
    
    return GmatCommand::GetStringParameter(id);
}


//---------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//---------------------------------------------------------------------------
bool Vary::SetStringParameter(const Integer id, const std::string &value)
{
    //if (id == solverNameID) 
    if (id == SOLVER_NAME)
    {
        solverName = value;
        return true;
    }
    
    //if (id == variableNameID) 
    if (id == VARIABLE_NAME)
    {
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
      if (solverName == obj->GetName()) 
      {
         solver = (Solver*)obj;
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
 *     Vary myDC(Burn1.V = 0.5, {Pert = 0.0001, MaxStep = 0.05, 
 *               Lower = 0.0, Upper = 3.14159);
 *
 * where 
 *
 * 1. myDC is a Solver used to Vary a set of variables to achieve the
 * corresponding goals, 
 * 2. Burn1.V is the parameter that is varied, and
 * 3. The settings in the braces specify features about how the variable can
 * be changed.
 * 
 * This method breaks the script line into the corresponding pieces, and stores 
 * the name of the Solver so it can be set to point to the correct object 
 * during initialization.
 */
//------------------------------------------------------------------------------
bool Vary::InterpretAction()
{
   TextParser tp;
   StringArray cmds;
   cmds.push_back("Vary");
   tp.Initialize(cmds);
   
   tp.EvaluateBlock(generatingString);
   StringArray blocks = tp.DecomposeBlock(generatingString);
   StringArray chunks = tp.ChunkLine();
   
   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage("Vary::InterpretAction() block list:\n");
      for (StringArray::iterator i = blocks.begin(); i != blocks.end(); ++i)
         MessageInterface::ShowMessage("   %s\n", i->c_str());
      MessageInterface::ShowMessage("Vary::InterpretAction() chunk list:\n");
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   %s\n", i->c_str());
   #endif      

   // First comes the keyword, "Vary"
   if (chunks[0] != typeName)
      throw CommandException(
         "Line '" + generatingString + 
         "'\n should be a Vary command, but the '" + typeName + 
         "' keyword is not the opening token in the line.");  

   StringArray currentChunks = tp.Decompose(chunks[1], "()");
   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage("   Vary::InterpretAction() 1st level:\n");
      for (StringArray::iterator i = currentChunks.begin(); 
           i != currentChunks.end(); ++i)
         MessageInterface::ShowMessage("      %s\n", i->c_str());
   #endif
   // The leading string is the Solver name
   SetStringParameter(SOLVER_NAME, currentChunks[0]);
   
   // Next break out the variables from the settings
   currentChunks = tp.SeparateBrackets(currentChunks[1], "()", ", ");
   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage(
         "      Vary::InterpretAction() 2nd level:\n");
      for (StringArray::iterator i = currentChunks.begin(); 
           i != currentChunks.end(); ++i)
         MessageInterface::ShowMessage("         %s\n", i->c_str());
   #endif
   
   // First chunk is the variable and initial value
   StringArray nameval = tp.SeparateBy(currentChunks[0], "= ");
   variableName.push_back(nameval[0]);
   variableId.push_back(-1);
   // This part needs to be changed to handle all of the types
   Real value = atof(nameval[1].c_str());
   SetRealParameter(INITIAL_VALUE, value);
   
   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage(
         "         Vary::InterpretAction() Variable:\n");
      for (StringArray::iterator i = nameval.begin(); 
           i != nameval.end(); ++i)
         MessageInterface::ShowMessage("            %s\n", i->c_str());
   #endif
   
   // then the bracketed list of settings
   currentChunks = tp.SeparateBrackets(currentChunks[1], "{}", ", ");

   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage(
         "         Vary::InterpretAction() Settings:\n");
      for (StringArray::iterator i = currentChunks.begin(); 
           i != currentChunks.end(); ++i)
         MessageInterface::ShowMessage("            %s\n", i->c_str());
   #endif

   // For each case here, the atof needs to be reset to handle the allowed types
   for (StringArray::iterator i = currentChunks.begin(); 
        i != currentChunks.end(); ++i)
   {
      nameval = tp.SeparateBy(*i, "= ");
      if (nameval[0] == "Pert")
      {
         value = atof(nameval[1].c_str());
         SetRealParameter(PERTURBATION, value);
      }
      else if (nameval[0] == "Lower")
      {
         value = atof(nameval[1].c_str());
         SetRealParameter(VARIABLE_MINIMUM, value);
      }
      else if (nameval[0] == "Upper")
      {
         value = atof(nameval[1].c_str());
         SetRealParameter(VARIABLE_MAXIMUM, value);
      }
      else if (nameval[0] == "MaxStep")
      {
         value = atof(nameval[1].c_str());
         SetRealParameter(VARIABLE_MAXIMUM_STEP, value);
      }
      else if (nameval[0] == "AdditiveScaleFactor")
      {
         value = atof(nameval[1].c_str());
         SetRealParameter(ADDITIVE_SCALE_FACTOR, value);
      }
      else if (nameval[0] == "MultiplicativeScaleFactor")
      {
         value = atof(nameval[1].c_str());
         SetRealParameter(MULTIPLICATIVE_SCALE_FACTOR, value);
      }
      else
      {
         std::string msg = "On the line \n'" + generatingString +
            "'\nthe setting '" + (*i) + 
            "' does not match the available options for the Vary command.";
         MessageInterface::ShowMessage(msg);
         throw CommandException(msg); 
      }
   }
   
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
//    // Find the solver
//    if (objectMap->find(solverName) == objectMap->end()) {
//        std::string errorString = "Target command cannot find solver \"";
//        errorString += solverName;
//        errorString += "\"";
//        throw CommandException(errorString);
//    }
//
//    solver = (Solver *)((*objectMap)[solverName]);

    if (solver == NULL)
       throw CommandException("solver not initialized for Vary command\n  \""
                              + generatingString + "\"\n");

    Integer id = solver->GetParameterID("Variables");
    if (!variableName.empty())
        solver->SetStringParameter(id, variableName[0]);
        
    // The solver cannot be finalized until all of the loop is initialized
    solverDataFinalized = false;

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
       ("Vary::Execute() solverDataFinalized=%d\n", solverDataFinalized);
    #endif
    
    if (!solverDataFinalized) 
    {
        // First time through, tell the solver about the variables
        Real varData[5];
        //for (Integer i = 0; i < variableName.size(); ++i) {
        Integer i = 0;
        { 
            varData[0] = initialValue[i];              // Initial value
            // scale by using Eq. 13.5 of Architecture document
            varData[0] = (varData[0] + additiveScaleFactor[i]) / 
                         multiplicativeScaleFactor[i];
            varData[1] = (perturbation[i]) / 
                         multiplicativeScaleFactor[i];   // pert
            varData[2] = (variableMinimum[i] + additiveScaleFactor[i]) / 
                         multiplicativeScaleFactor[i];   // minimum
            varData[3] = (variableMaximum[i] + additiveScaleFactor[i]) / 
                         multiplicativeScaleFactor[i];   // maximum
            varData[4] = (variableMaximumStep[i]) / 
                         multiplicativeScaleFactor[i];   // largest allowed step
            
            if (variableId.empty())
               variableId.push_back(-1);
            
            variableId[i] = solver->SetSolverVariables(varData, variableName[i]);
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
        
        Integer id;
        
        if (obj->GetTypeName() == "Variable")
        {
            id = -1;
        }
        else
        {
            id = obj->GetParameterID(parmName);
    
            if (id == -1) {
                std::string errorstr = "Could not find parameter ";
                errorstr += parmName;
                errorstr += " on object ";
                errorstr += objectName;
                throw CommandException(errorstr);
            } 
        
            Gmat::ParameterType type = obj->GetParameterType(id);
            if (type != Gmat::REAL_TYPE) {
                std::string errorstr = "The solver variable ";
                errorstr += parmName;
                errorstr += " on object ";
                errorstr += objectName;
                errorstr += " is not Real.";
                throw CommandException(errorstr);
            }
        }
         
        pobject.push_back(obj);
        parmId.push_back(id);
        
        solverDataFinalized = true;
        BuildCommandSummary(true);
        return retval;
    }
    
    Real var = solver->GetSolverVariable(variableId[0]);
    // scale using Eq. 13.6 of Architecture document
    var = var * multiplicativeScaleFactor[0] - additiveScaleFactor[0];
    
//    // Just a check here -- the solver handles all of these now
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

    if (pobject[0]->GetTypeName() == "Variable")
       ((Parameter*)pobject[0])->SetReal(var);
    else
       pobject[0]->SetRealParameter(parmId[0], var);

    BuildCommandSummary(true);
    return retval;
}


void Vary::RunComplete()
{
   solverDataFinalized = false;
   pobject.clear();
   parmId.clear();
   GmatCommand::RunComplete();
}
