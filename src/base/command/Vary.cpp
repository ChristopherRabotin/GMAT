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
//#define DEBUG_WRAPPER_CODE
//#define DEBUG_RENAME

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
//   "MinimumValue",
   "Lower",
//   "MaximumValue",
   "Upper",
//   "MaximumChange",
   "MaxStep",
   "AdditiveScaleFactor",
   "MultiplicativeScaleFactor"   
};

const Gmat::ParameterType Vary::PARAMETER_TYPE[VaryParamCount -
                                               GmatCommandParamCount] = 
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
};
 

//------------------------------------------------------------------------------
//  Vary(void)
//------------------------------------------------------------------------------
/**
 * Creates a Vary command.  (default constructor)
 */
//------------------------------------------------------------------------------
Vary::Vary() :
   GmatCommand                   ("Vary"),
   solverName                    (""),
   solver                        (NULL),
   variableName                  (""),
   variable                      (NULL),
   initialValueName              ("0.0"),
   initialValue                  (NULL),
   currentValue                  (0.0),
   perturbationName              ("0.001"),
   perturbation                  (NULL),
   variableMinimumName           ("-9.999999e300"),
   variableMinimum               (NULL),
   variableMaximumName           ("9.999999e300"),
   variableMaximum               (NULL),
   variableMaximumStepName       ("9.999999e300"),
   variableMaximumStep           (NULL),
   additiveScaleFactorName       ("0.0"),
   additiveScaleFactor           (NULL),
   multiplicativeScaleFactorName ("1.0"),
   multiplicativeScaleFactor     (NULL),
   variableID                    (-1),
   solverDataFinalized           (false)
{
   settables.push_back("Perturbation"); 
   settables.push_back("MaxStep");
   settables.push_back("Lower");  
   settables.push_back("Upper");  
   settables.push_back("AdditiveScaleFactor");
   settables.push_back("MultiplicativeScaleFactor");
//    //parameterCount += 7;
//    additiveScaleFactor.push_back(0.0);
//    multiplicativeScaleFactor.push_back(1.0);
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
   GmatCommand                   (t),
   solverName                    (t.solverName),
   solver                        (NULL),
   variableName                  (t.variableName),
   variable                      (NULL),
   initialValueName              (t.initialValueName),
   initialValue                  (NULL),
   currentValue                  (0.0),
   perturbationName              (t.perturbationName),
   perturbation                  (NULL),
   variableMinimumName           (t.variableMinimumName),
   variableMinimum               (NULL),
   variableMaximumName           (t.variableMaximumName),
   variableMaximum               (NULL),
   variableMaximumStepName       (t.variableMaximumStepName),
   variableMaximumStep           (NULL),
   additiveScaleFactorName       (t.additiveScaleFactorName),
   additiveScaleFactor           (NULL),
   multiplicativeScaleFactorName (t.multiplicativeScaleFactorName),
   multiplicativeScaleFactor     (NULL),
   variableID                    (-1),
   solverDataFinalized           (t.solverDataFinalized)
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
        
   solverName                    = t.solverName;
   variableName                  = t.variableName;
   initialValueName              = t.initialValueName;
   currentValue                  = 0.0;
   perturbationName              = t.perturbationName;
   variableMinimumName           = t.variableMinimumName;
   variableMaximumName           = t.variableMaximumName;
   variableMaximumStepName       = t.variableMaximumStepName;
   additiveScaleFactorName       = t.additiveScaleFactorName;
   multiplicativeScaleFactorName = t.multiplicativeScaleFactorName;

   // Initialize wrappers to NULL
   ClearWrappers();

   solver = NULL;
   variableID = -1;
   solverDataFinalized = t.solverDataFinalized;
    
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
   ClearWrappers();
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
   

//   // Iterate through the variables
   details << variable->GetDescription() << " = " << initialValue->GetDescription() <<  ", ";
//
   // figure out if this is inside a Target or an Optimize branch command, to
   // determine which things should be added to the generatingString
   std::string targOpt  = "";
   GmatCommand *prevCmd = GetPrevious();
   while (prevCmd != NULL)
   {
      if (prevCmd->IsOfType("Target"))
      {
         targOpt = "Target";
         break;
      }
      if (prevCmd->IsOfType("Optimize"))
      {
         targOpt = "Optimize";
         break;
      }
      prevCmd = prevCmd->GetPrevious();
   }
   // add perturbation and max step for Target
   //if (solver && (solver->IsOfType("Targeter")))
   if (targOpt == "Target")
   {
      details << "{Perturbation = ";
      details << perturbation->GetDescription();
      details << ", MaxStep = ";
      details << variableMaximumStep->GetDescription();
      details << ", ";
   }
   else if (targOpt == "Optimize")
   {
      details << "{";
   }
   
   details << "Lower = ";
   details << variableMinimum->GetDescription();
   
   details << ", Upper = ";
   details << variableMaximum->GetDescription();
   
   // add the scale factors for Optimize
   //if (solver && (solver->IsOfType("Optimizer")))
   if (targOpt == "Optimize")
   {
      details << ", AdditiveScaleFactor = ";
      details << additiveScaleFactor->GetDescription();
      details << ", MultiplicativeScaleFactor = ";
      details << multiplicativeScaleFactor->GetDescription();
    }
   
   gen += details.str();
   generatingString = gen + "});";
   
   // Then call the base class method
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool Vary::RenameRefObject(const Gmat::ObjectType type,
                           const std::string &oldName,
                           const std::string &newName)
{
   #ifdef DEBUG_RENAME
   MessageInterface::ShowMessage
      ("Vary::RenameRefObject() type=%d, oldName=%s, newName=%s\n",
       type, oldName.c_str(), newName.c_str());
   #endif

   if (type == Gmat::SOLVER)
   {
      if (solverName == oldName)
         solverName = newName;
   }
   
   //else if ((type == Gmat::BURN) || (type == Gmat::PARAMETER)) // do I need this?
   //{
   //   if (variableName == oldName)
    //     variableName = newName;
   //}

   // make sure the wrappers know to rename any objects they may be using
   if (variable)
   {
      variable->RenameObject(oldName, newName);
      variableName           = variable->GetDescription();
   }
   if (initialValue)        
   {
      initialValue->RenameObject(oldName, newName);
      initialValueName       = initialValue->GetDescription();
   }
   if (perturbation)        
   {
      perturbation->RenameObject(oldName, newName);
      perturbationName       = perturbation->GetDescription();
   }
   if (variableMinimum)     
   {
      variableMinimum->RenameObject(oldName, newName);
      variableMinimumName    = variableMinimum->GetDescription();
   }
   if (variableMaximum)     
   {
      variableMaximum->RenameObject(oldName, newName);
      variableMaximumName    = variableMaximum->GetDescription();
   }
   if (variableMaximumStep) 
   {
      variableMaximumStep->RenameObject(oldName, newName);
      variableMaximumStepName= variableMaximumStep->GetDescription();
   }
   if (additiveScaleFactor) 
   {    
      additiveScaleFactor->RenameObject(oldName, newName);
      additiveScaleFactorName  = additiveScaleFactor->GetDescription();
   }
   if (multiplicativeScaleFactor) 
   {
      multiplicativeScaleFactor->RenameObject(oldName, newName);
       multiplicativeScaleFactorName    
                      = multiplicativeScaleFactor->GetDescription();
   }
   
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Vary.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Vary::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SOLVER);
   //refObjectTypes.push_back(Gmat::PARAMETER);
   return refObjectTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref objects used by the Vary.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 * 
 * @return the list of object names.
 * 
 */
//------------------------------------------------------------------------------
const StringArray& Vary::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   refObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::SOLVER)
   {
      refObjectNames.push_back(solverName);
   }

/**
 * The following is no longer needed with the wrapper code:   
//   if (type == Gmat::UNKNOWN_OBJECT ||
//       type == Gmat::PARAMETER)
//   {
//      refObjectNames.insert(refObjectNames.end(), variableName.begin(),
//                            variableName.end());
//   }
 */
 
   return refObjectNames;
}


//---------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//---------------------------------------------------------------------------
std::string Vary::GetParameterText(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < VaryParamCount))
      return PARAMETER_TEXT[id - GmatCommandParamCount];

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

   return GmatCommand::GetParameterID(str);
}


//---------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
Gmat::ParameterType Vary::GetParameterType(const Integer id) const
{
   if ((id >= GmatCommandParamCount) && (id < VaryParamCount))
      return PARAMETER_TYPE[id - GmatCommandParamCount];

    return GmatCommand::GetParameterType(id);
}


//---------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//---------------------------------------------------------------------------
std::string Vary::GetParameterTypeString(const Integer id) const
{
   return GmatCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//---------------------------------------------------------------------------
Real Vary::GetRealParameter(const Integer id) const
{
   if (id == INITIAL_VALUE)
      if (initialValue)
         return initialValue->EvaluateReal();
        
   if (id == PERTURBATION)
      if (perturbation)
         return perturbation->EvaluateReal();
        
   if (id == VARIABLE_MINIMUM)
      if (variableMinimum)
         return variableMinimum->EvaluateReal();
        
   if (id == VARIABLE_MAXIMUM)
      if (variableMaximum)
         return variableMaximum->EvaluateReal();
        
   if (id == VARIABLE_MAXIMUM_STEP)
      if (variableMaximumStep)
         return variableMaximumStep->EvaluateReal();
            
   if (id == ADDITIVE_SCALE_FACTOR)
      if (additiveScaleFactor)
         return additiveScaleFactor->EvaluateReal();
         
   if (id == MULTIPLICATIVE_SCALE_FACTOR)
      if (multiplicativeScaleFactor)
         return multiplicativeScaleFactor->EvaluateReal();
         
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
      return variableName;
    
   if (id == INITIAL_VALUE)
      return initialValueName;
    
   if (id == PERTURBATION)
      return perturbationName;
    
   if (id == VARIABLE_MINIMUM)
      return variableMinimumName;
    
   if (id == VARIABLE_MAXIMUM)
      return variableMaximumName;
    
   if (id == VARIABLE_MAXIMUM_STEP)
      return variableMaximumStepName;
    
   if (id == ADDITIVE_SCALE_FACTOR)
      return additiveScaleFactorName;
    
   if (id == MULTIPLICATIVE_SCALE_FACTOR)
      return multiplicativeScaleFactorName;
    
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
      variableName = value;
      return true;
   }

   if ((id == VARIABLE_NAME) || (id == INITIAL_VALUE) || 
       (id == PERTURBATION) || (id == VARIABLE_MINIMUM) || 
       (id == VARIABLE_MAXIMUM) || (id == VARIABLE_MAXIMUM_STEP) || 
       (id == ADDITIVE_SCALE_FACTOR) || (id == MULTIPLICATIVE_SCALE_FACTOR))
   {
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);

      // Also need to store the names for later mapping
      switch (id)
      {
         case VARIABLE_NAME:
            variableName = value;
            break;
         
         case INITIAL_VALUE:
            initialValueName = value;
            break;
         
         case PERTURBATION:
            perturbationName = value;
            break;
         
         case VARIABLE_MINIMUM:
            variableMinimumName = value;
            break;
         
         case VARIABLE_MAXIMUM:
            variableMaximumName = value;
            break;
         
         case VARIABLE_MAXIMUM_STEP:
            variableMaximumStepName = value;
            break;
         
         case ADDITIVE_SCALE_FACTOR:
            additiveScaleFactorName = value;
            break;
         
         case MULTIPLICATIVE_SCALE_FACTOR:
            multiplicativeScaleFactorName = value;
            break;
         
         default:
            throw GmatBaseException("Unknown property for Vary command\n");
      }
      
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
   // Clean out any old data
   wrapperObjectNames.clear();
   ClearWrappers();

   StringArray chunks = InterpretPreface();

   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage("Preface chunks as\n");
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
      MessageInterface::ShowMessage("\n");
   #endif

   if (chunks.size() <= 1)
      throw CommandException("Missing information for Vary command.\n");
   
   if (chunks[1].at(0) == '(')
      throw CommandException("Missing solver name for Vary command.\n");
   
   if ((chunks[1].find("[") != chunks[1].npos) || (chunks[1].find("]") != chunks[1].npos))
      throw CommandException("Brackets not allowed in Vary command");

   if (!GmatStringUtil::AreAllBracketsBalanced(chunks[1], "({)}"))      
      throw CommandException
         ("Parentheses, braces, or brackets are unbalanced");

      // Find and set the solver object name
   // This is the only setting in Vary that is not in a wrapper
   StringArray currentChunks = parser.Decompose(chunks[1], "()", false);
   SetStringParameter(SOLVER_NAME, currentChunks[0]);
   
   // The remaining text in the instruction is the variable definition and 
   // parameters, all contained in currentChunks[1].  Deal with those next.
   //currentChunks = parser.SeparateBrackets(currentChunks[1], "()", ", ");
   //std::string noLeftBrace  = GmatStringUtil::RemoveAll(currentChunks[1],'{');
   //std::string noRightBrace = GmatStringUtil::RemoveAll(noLeftBrace,'}');
   //std::string noSpaces     = GmatStringUtil::RemoveAll(noRightBrace,' ');
   std::string noSpaces2     = GmatStringUtil::RemoveAll(currentChunks[1],' ');
   currentChunks = parser.Decompose(noSpaces2, "()", true, true);
   //currentChunks = parser.Decompose(currentChunks[1], "()", true, true);
   
   #ifdef DEBUG_VARY_PARSING
      //MessageInterface::ShowMessage(
      //   "Vary:  noSpaces = %s\n", noSpaces.c_str());
      MessageInterface::ShowMessage(
         "Vary: after Decompose, current chunks = \n");
      for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
         MessageInterface::ShowMessage("   %s\n",
                                       currentChunks[jj].c_str());
   #endif

   // First chunk is the variable and initial value
   std::string lhs, rhs;
   if (!SeparateEquals(currentChunks[0], lhs, rhs, true))
      // Variable takes default initial value
      //rhs = "0.0";
   {
      throw CommandException("The variable \"" + lhs + 
         "\" is missing the \"=\" operator or an initial value required for a " + typeName + 
         " command.\n");
   }
      
   variableName = lhs;
   variableID = -1;

   variableValueString = rhs;
   initialValueName = rhs;
   
   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage(
         "Vary:  setting variableName to %s\n", variableName.c_str());
      MessageInterface::ShowMessage(
         "Vary:  setting initialValueName to %s\n", initialValueName.c_str());
   #endif
   
   if (currentChunks.size() > 1)
   {
      std::string noSpaces     = GmatStringUtil::RemoveAll(currentChunks[1],' ');
      // Now deal with the settable parameters
      //currentChunks = parser.SeparateBrackets(currentChunks[1], "{}", ",", false);
      currentChunks = parser.SeparateBrackets(noSpaces, "{}", ",", true);
      
      #ifdef DEBUG_VARY_PARSING
         MessageInterface::ShowMessage(
            "Vary: After SeparateBrackets, current chunks = \n");
         for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
            MessageInterface::ShowMessage("   %s\n",
                                          currentChunks[jj].c_str());
      #endif
      
      // currentChunks now holds all of the pieces - no need for more separation  
      
      for (StringArray::iterator i = currentChunks.begin(); 
           i != currentChunks.end(); ++i)
      {
         bool isOK = SeparateEquals(*i, lhs, rhs, true);
         #ifdef DEBUG_VARY_PARSING
            MessageInterface::ShowMessage("Setting Vary properties\n");
            MessageInterface::ShowMessage("   \"%s\" = \"%s\"\n", lhs.c_str(), rhs.c_str());
         #endif
         if (!isOK || lhs.empty() || rhs.empty())
            throw CommandException("The setting \"" + lhs + 
               "\" is missing the \"=\" operator or a value required for a " + typeName + 
               " command.\n");
         
         if (IsSettable(lhs))
            SetStringParameter(GetParameterID(lhs), rhs);
         else
            throw CommandException("The setting \"" + lhs + 
               "\" is not a valid setting for a " + typeName + 
               " command.\n");
      }
   }
   return true;
}


const StringArray& Vary::GetWrapperObjectNameArray()
{
   wrapperObjectNames.clear();

   wrapperObjectNames.push_back(variableName);
   
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       initialValueName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(initialValueName);

   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       perturbationName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(perturbationName);
       
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       variableMinimumName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(variableMinimumName);
       
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       variableMaximumName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(variableMaximumName);
       
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       variableMaximumStepName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(variableMaximumStepName);
       
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       additiveScaleFactorName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(additiveScaleFactorName);
       
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       multiplicativeScaleFactorName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(multiplicativeScaleFactorName);
   
   return wrapperObjectNames;
}


bool Vary::SetElementWrapper(ElementWrapper *toWrapper, const std::string &withName)
{
   bool retval = false;

   if (toWrapper == NULL) return false;
   
   if (toWrapper->GetWrapperType() == Gmat::ARRAY)
   {
      throw CommandException("A value of type \"Array\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }
   /*
   if (toWrapper->GetWrapperType() == Gmat::STRING_OBJECT)
   {
      throw CommandException("A value of type \"String Object\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }

   try
   {
       if ( ((toWrapper->GetDataType()) != Gmat::REAL_TYPE) &&
            ((toWrapper->GetDataType()  != Gmat::INTEGER_TYPE)) )
       {
           throw CommandException("A value of base type \"non-Real\" on command \"" + 
                       typeName + 
                       "\" is not an allowed value.\nThe allowed values are:"
                       " [ Real Number, Variable, Array Element, or Parameter ]. "); 
       }
   }
   catch (BaseException &be)
   {
       // just ignore it here - will need to check data type on initialization
   }
   */
   CheckDataType(toWrapper, Gmat::REAL_TYPE, "Vary", true);


   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage("   Setting wrapper \"%s\" on Vary command\n", 
      withName.c_str());
   #endif

   if (variableName == withName)
   {
      variable = toWrapper;
      retval = true;
   }
   
   if (initialValueName == withName)
   {
      initialValue = toWrapper;
      retval = true;
   }
   
   if (perturbationName == withName)
   {
      perturbation = toWrapper;
      retval = true;
   }
   
   if (variableMinimumName == withName)
   {
      variableMinimum = toWrapper;
      retval = true;
   }
   
   if (variableMaximumName == withName)
   {
      variableMaximum = toWrapper;
      retval = true;
   }
   
   if (variableMaximumStepName == withName) 
   {
      variableMaximumStep = toWrapper;
      retval = true;
   }
   
   if (additiveScaleFactorName == withName)
   {
      additiveScaleFactor = toWrapper;
      retval = true;
   }
   
   if (multiplicativeScaleFactorName == withName)
   {
      multiplicativeScaleFactor = toWrapper;
      retval = true;
   }
      
   return retval;
}

void Vary::ClearWrappers()
{
   std::vector<ElementWrapper*> temp;
   if (variable)
   {
      temp.push_back(variable);
      variable = NULL;
   }
   if (initialValue)
   {
      if (find(temp.begin(), temp.end(), initialValue) == temp.end())
      {
         temp.push_back(initialValue);
         initialValue = NULL;
      }
   }
   if (perturbation)
   {
      if (find(temp.begin(), temp.end(), perturbation) == temp.end())
      {
         temp.push_back(perturbation);
         perturbation = NULL;
      }
   }
   if (variableMinimum)
   {
      if (find(temp.begin(), temp.end(), variableMinimum) == temp.end())
      {
         temp.push_back(variableMinimum);
         variableMinimum = NULL;
      }
   }
   if (variableMaximum)
   {
      if (find(temp.begin(), temp.end(), variableMaximum) == temp.end())
      {
         temp.push_back(variableMaximum);
         variableMaximum = NULL;
      }
   }
   if (variableMaximumStep)
   {
      if (find(temp.begin(), temp.end(), variableMaximumStep) == temp.end())
      {
         temp.push_back(variableMaximumStep);
         variableMaximumStep = NULL;
      }
   }
   if (additiveScaleFactor)
   {
      if (find(temp.begin(), temp.end(), additiveScaleFactor) == temp.end())
      {
         temp.push_back(additiveScaleFactor);
         additiveScaleFactor = NULL;
      }
   }
   if (multiplicativeScaleFactor)
   {
      if (find(temp.begin(), temp.end(), multiplicativeScaleFactor) == temp.end())
      {
         temp.push_back(multiplicativeScaleFactor);
         multiplicativeScaleFactor = NULL;
      }
   }
   
   ElementWrapper *wrapper;
   for (UnsignedInt i = 0; i < temp.size(); ++i)
   {
      wrapper = temp[i];
      delete wrapper;
   }
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

   if (solver == NULL)
      throw CommandException("solver not initialized for Vary command\n  \""
                             + generatingString + "\"\n");

   Integer id = solver->GetParameterID("Variables");
   solver->SetStringParameter(id, variableName);
        
   // The solver cannot be finalized until all of the loop is initialized
   solverDataFinalized = false;

   // Set references for the wrappers   
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for variable\n");
   #endif
   if (SetWrapperReferences(*variable) == false)
      return false;
   CheckDataType(variable, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for initial value\n");
   #endif
   if (SetWrapperReferences(*initialValue) == false)
      return false;
   CheckDataType(initialValue, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for perturbation\n");
   #endif
   if (SetWrapperReferences(*perturbation) == false)
      return false;
   CheckDataType(perturbation, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for minimum\n");
   #endif
   if (SetWrapperReferences(*variableMinimum) == false)
      return false;
   CheckDataType(variableMinimum, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for maximum\n");
   #endif
   if (SetWrapperReferences(*variableMaximum) == false)
      return false;
   CheckDataType(variableMaximum, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for max step\n");
   #endif
   if (SetWrapperReferences(*variableMaximumStep) == false)
      return false;
   CheckDataType(variableMaximumStep, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for additive scale factor\n");
   #endif
   if (SetWrapperReferences(*additiveScaleFactor) == false)
      return false;
   CheckDataType(additiveScaleFactor, Gmat::REAL_TYPE, "Vary");
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for mult. scale factor\n");
   #endif
   if (SetWrapperReferences(*multiplicativeScaleFactor) == false)
      return false;
   CheckDataType(multiplicativeScaleFactor, Gmat::REAL_TYPE, "Vary");

   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Vary command Initialization complete\n");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
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
bool Vary::Execute()
{
   bool retval = true;

   #ifdef DEBUG_VARY_EXECUTE
      MessageInterface::ShowMessage
         ("Vary::Execute() solverDataFinalized=%d\n", solverDataFinalized);
   #endif
    
   if (!solverDataFinalized) 
   {
      #ifdef DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage
            ("Vary::Execute() - running code for the first time through <<<\n");
      #endif
      // First time through, tell the solver about the variables
      Real varData[5], asf, msf;
      asf = additiveScaleFactor->EvaluateReal();
      msf = multiplicativeScaleFactor->EvaluateReal();

      varData[0] = initialValue->EvaluateReal();                // Initial value

      // scale by using Eq. 13.5 of Architecture document
      varData[0] = (varData[0] + asf) / msf;
      varData[1] = (perturbation->EvaluateReal()) / msf;           // pert
      varData[2] = (variableMinimum->EvaluateReal() + asf) / msf;  // minimum
      varData[3] = (variableMaximum->EvaluateReal() + asf) / msf;  // maximum
      varData[4] = (variableMaximumStep->EvaluateReal()) / msf; // largest step
            
      #ifdef DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage(
            "For variable \"%s\", data is [%15.9lf %15.9lf %15.9lf %15.9lf "
            "%15.9lf]\n", variableName.c_str(), varData[0], varData[1], varData[2], 
            varData[3], varData[4]);
      #endif      
   
      variableID = solver->SetSolverVariables(varData, variableName);

      #ifdef DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage("Solver variables were set\n");
      #endif

      solverDataFinalized = true;
      BuildCommandSummary(true);
      #if DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage
            ("Vary::Execute - exiting with retval = %s\n",
            (retval? "true" : "False"));
      #endif
      return retval;
   }

    Real var = solver->GetSolverVariable(variableID);
    // scale using Eq. 13.6 of Architecture document
    var = var * multiplicativeScaleFactor->EvaluateReal() - 
          additiveScaleFactor->EvaluateReal();
    
    #ifdef DEBUG_VARIABLE_RANGES
       MessageInterface::ShowMessage(
          "Setting %s to %.12le; allowed range is [%.12le, %.12le]\n",
          variableName.c_str(), var, variableMinimum->EvaluateReal(), 
          variableMaximum->EvaluateReal());
          //variableName[0].c_str(), var, variableMinimum[0], variableMaximum[0]);
    #endif

    variable->SetReal(var);

    BuildCommandSummary(true);
      #ifdef DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage
            ("Vary::Execute - exiting with retval = %s\n",
            (retval? "true" : "False"));
      #endif
    return retval;
}


void Vary::RunComplete()
{
   #ifdef DEBUG_VARY_EXECUTE
      MessageInterface::ShowMessage(
      "In Vary::RunComplete, solverDataFinalized = %s, ... now setting it to false\n",
      (solverDataFinalized? "true" : "false"));
   #endif
   solverDataFinalized = false;
   //ClearWrappers();
   GmatCommand::RunComplete();
}
