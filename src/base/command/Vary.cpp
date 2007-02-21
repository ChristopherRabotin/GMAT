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
   variableID                    (-1)
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
   details << "{Perturbation = ";
   details << perturbation->GetDescription();
 
   details << ", MaxStep = ";
   details << variableMaximumStep->GetDescription();
   details << ", Lower = ";
   details << variableMinimum->GetDescription();


   details << ", Upper = ";
   details << variableMaximum->GetDescription();
   
   if (solver && (solver->IsOfType("Optimizer")))
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

/**
 * Old code:


//   if (id == INITIAL_VALUE)
//   {
//      if (initialValue)
//         initialValue->SetReal(value);
//      else
//         throw CommandException("Wrapper not set for \"" + initialValueName +
//            "\" in the command \"" + generatingString + "\"\n");
//      return value;
//   }
//            
//   if (id == PERTURBATION) 
//   {
//      if (perturbation)
//         perturbation->SetReal(value);
//      else
//         throw CommandException("Wrapper not set for \"" + perturbationName +
//            "\" in the command \"" + generatingString + "\"\n");
//      return value;
//   }
//        
//   if (id == VARIABLE_MINIMUM)
//   {
//      if (variableMinimum)
//         variableMinimum->SetReal(value);
//      else
//         throw CommandException("Wrapper not set for \"" + variableMinimumName +
//            "\" in the command \"" + generatingString + "\"\n");
//      return value;
//   }
//        
//    //if (id == variableMaximumID) 
//    if (id == VARIABLE_MAXIMUM)
//    {
//        if (variableMaximum)
//            variableMaximum->SetReal(value);
//      else
//         throw CommandException("Wrapper not set for \"" + variableMaximumName +
//            "\" in the command \"" + generatingString + "\"\n");
//      return value;
//    }
//        
//    //if (id == variableMaximumStepID) 
//    if (id == VARIABLE_MAXIMUM_STEP)
//    {
//        if (variableMaximumStep)
//            variableMaximumStep->SetReal(value);
//      else
//         throw CommandException("Wrapper not set for \"" + 
//            variableMaximumStepName + "\" in the command \"" + 
//            generatingString + "\"\n");
//      return value;
//    }
//    
//   if (id == ADDITIVE_SCALE_FACTOR)
//   {
//      if (additiveScaleFactor)
//         additiveScaleFactor->SetReal(value);
//      else
//         throw CommandException("Wrapper not set for \"" + 
//            additiveScaleFactorName + "\" in the command \"" + 
//            generatingString + "\"\n");
//      return value;
//   }
//
//   if (id == MULTIPLICATIVE_SCALE_FACTOR)
//   {
//      if (multiplicativeScaleFactor)
//         multiplicativeScaleFactor->SetReal(value);
//      else
//         throw CommandException("Wrapper not set for \"" + 
//            multiplicativeScaleFactorName + "\" in the command \"" + 
//            generatingString + "\"\n");
//      return value;
//   }

*/

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
   
   // Find and set the solver object name
   // This is the only setting in Vary that is not in a wrapper
   StringArray currentChunks = parser.Decompose(chunks[1], "()", false);
   SetStringParameter(SOLVER_NAME, currentChunks[0]);
   
   // The remaining text in the instruction is the variable definition and 
   // parameters, all contained in currentChunks[1].  Deal with those next.
   //currentChunks = parser.SeparateBrackets(currentChunks[1], "()", ", ");
   std::string noLeftBrace  = GmatStringUtil::RemoveAll(currentChunks[1],'{');
   std::string noRightBrace = GmatStringUtil::RemoveAll(noLeftBrace,'}');
   std::string noSpaces     = GmatStringUtil::RemoveAll(noRightBrace,' ');
   currentChunks = parser.Decompose(noSpaces, "()", true, true);
   //currentChunks = parser.Decompose(currentChunks[1], "()", true, true);
   
   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage(
         "Vary: after Decompose, current chunks = \n");
      for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
         MessageInterface::ShowMessage("   %s\n",
                                       currentChunks[jj].c_str());
   #endif

   // First chunk is the variable and initial value
   std::string lhs, rhs;
   if (!SeparateEquals(currentChunks[0], lhs, rhs))
      // Variable takes default initial value
      rhs = "0.0";
      
   variableName = lhs;
   variableID = -1;

   variableValueString = rhs;
   initialValueName = rhs;
   
   // Now deal with the settable parameters
   //currentChunks = parser.SeparateBrackets(currentChunks[1], "{}", ", ", false);
   
   //#ifdef DEBUG_VARY_PARSING
   //   MessageInterface::ShowMessage(
   //      "Vary: After SeparateBrackets, current chunks = \n");
   //   for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
   //      MessageInterface::ShowMessage("   %s\n",
   //                                    currentChunks[jj].c_str());
   //#endif
   
   // currentChunks now holds all of the pieces - no need for more separation  
   
   for (StringArray::iterator i = currentChunks.begin() + 1; 
        i != currentChunks.end(); ++i)
   {
      SeparateEquals(*i, lhs, rhs);
      #ifdef DEBUG_VARY_PARSING
         MessageInterface::ShowMessage("Setting Vary properties\n");
         MessageInterface::ShowMessage("   \"%s\" = \"%s\"\n", lhs.c_str(), rhs.c_str());
      #endif

      if (IsSettable(lhs))
         SetStringParameter(GetParameterID(lhs), rhs);
      else
         throw CommandException("Setting \"" + lhs + 
            "\" is missing a value required for a " + typeName + 
            " command.\nSee the line \"" + generatingString +"\"\n");
   }
   
   return true;
}

/**
 * Here is the old code:

bool Vary::InterpretAction()
{
   parser.EvaluateBlock(generatingString);
   StringArray blocks = parser.DecomposeBlock(generatingString);
   StringArray chunks = parser.ChunkLine();
   
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
         "' keyword is not the opening token in the line.\n");  

   StringArray currentChunks = parser.Decompose(chunks[1], "()");
   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage("   Vary::InterpretAction() 1st level:\n");
      for (StringArray::iterator i = currentChunks.begin(); 
           i != currentChunks.end(); ++i)
         MessageInterface::ShowMessage("      %s\n", i->c_str());
   #endif
   // The leading string is the Solver name
   SetStringParameter(SOLVER_NAME, currentChunks[0]);
   
   // Next break out the variables from the settings
   currentChunks = parser.SeparateBrackets(currentChunks[1], "()", ", ");
   #ifdef DEBUG_VARY_PARSING
      MessageInterface::ShowMessage(
         "      Vary::InterpretAction() 2nd level:\n");
      for (StringArray::iterator i = currentChunks.begin(); 
           i != currentChunks.end(); ++i)
         MessageInterface::ShowMessage("         %s\n", i->c_str());
   #endif
   
   // First chunk is the variable and initial value
   StringArray nameval = parser.SeparateBy(currentChunks[0], "= ");
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
   
   // then the bracketed list of settings, if not using defaults
   if (currentChunks.size() > 1)
   {
      currentChunks = parser.SeparateBrackets(currentChunks[1], "{}", ", ");
   
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
         nameval = parser.SeparateBy(*i, "= ");
         if ((nameval[0] == "Pert") || (nameval[0] == "Perturbation")) 
         {
            if (nameval[0] == "Pert")
            {
               MessageInterface::ShowMessage( "\"Pert\" is deprecated as the "
                  "string specifying variable perturbations, and will be "
                  "removed from a future build; please use \"Perturbation\" "
                  "instead.\n" ); 
            }
            bool retval = GmatStringUtil::ToReal(nameval[1], value);
            if ((retval == false) || (value <= 0.0))
               throw CommandException(
                  "The value of \"" + nameval[1] + "\" for field \"" + 
                  nameval[0] + "\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ] "
                  "which evaluates > 0. ");
            SetRealParameter(PERTURBATION, value);
         }
         else if (nameval[0] == "Lower")
         {
            if (GmatStringUtil::ToReal(nameval[1], value) == false)
               throw CommandException(
                  "The value of \"" + nameval[1] + "\" for field \"" + 
                  nameval[0] + "\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ] "
                  "which evaluates such that Lower < Upper. ");
            SetRealParameter(VARIABLE_MINIMUM, value);
         }
         else if (nameval[0] == "Upper")
         {
            if (GmatStringUtil::ToReal(nameval[1], value) == false)
               throw CommandException(
                  "The value of \"" + nameval[1] + "\" for field \"" + 
                  nameval[0] + "\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ] "
                  "which evaluates such that Lower < Upper. ");
            SetRealParameter(VARIABLE_MAXIMUM, value);
         }
         else if (nameval[0] == "MaxStep")
         {
            bool retval = GmatStringUtil::ToReal(nameval[1], value);
            if ((retval == false) || (value <= 0.0))
               throw CommandException(
                  "The value of \"" + nameval[1] + "\" for field \"" + 
                  nameval[0] + "\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ] "
                  "which evaluates > 0. ");
            SetRealParameter(VARIABLE_MAXIMUM_STEP, value);
         }
         else if (nameval[0] == "AdditiveScaleFactor")
         {
            if (GmatStringUtil::ToReal(nameval[1], value) == false)
               throw CommandException(
                  "The value of \"" + nameval[1] + "\" for field \"" + 
                  nameval[0] + "\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [  Real Number, Variable, Array Element, or Parameter ]. ");
            SetRealParameter(ADDITIVE_SCALE_FACTOR, value);
         }
         else if (nameval[0] == "MultiplicativeScaleFactor")
         {
            if (GmatStringUtil::ToReal(nameval[1], value) == false)
               throw CommandException(
                  "The value of \"" + nameval[1] + "\" for field \"" + 
                  nameval[0] + "\" on command \"" + typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. ");
            SetRealParameter(MULTIPLICATIVE_SCALE_FACTOR, value);
         }
         else
         {
            //std::string msg = "On the line \n'" + generatingString +
            //   "'\nthe setting '" + (*i) + 
            //   "' does not match the available options for the Vary command";
            std::string msg = "The setting \"" + (*i) + 
               "\" does not match any field name for the Vary command";
            //MessageInterface::ShowMessage(msg);
            throw CommandException(msg); 
         }
      }
   }
   
   // Ensure that there is a value for each component the variable might use,
   // because these values are passed generically to the Solvers
   UnsignedInt varCount = initialValue.size();
   
   if (perturbation.size() < varCount)
      SetRealParameter(PERTURBATION, 1.0e-6);

   if (variableMinimum.size() < varCount)
      SetRealParameter(VARIABLE_MINIMUM, -9.999e300);

   if (variableMaximum.size() < varCount)
      SetRealParameter(VARIABLE_MAXIMUM, 9.999e300);
   
   if (variableMaximumStep.size() < varCount)
      SetRealParameter(VARIABLE_MAXIMUM_STEP, 9.999e300);
   
   if (additiveScaleFactor.size() < varCount)
      SetRealParameter(ADDITIVE_SCALE_FACTOR, 0.0);
   
   if (multiplicativeScaleFactor.size() < varCount)
      SetRealParameter(MULTIPLICATIVE_SCALE_FACTOR, 1.0);
      
   return true;
}
*/

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
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for initial value\n");
   #endif
   if (SetWrapperReferences(*initialValue) == false)
      return false;
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for perturbation\n");
   #endif
   if (SetWrapperReferences(*perturbation) == false)
      return false;
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for minimum\n");
   #endif
   if (SetWrapperReferences(*variableMinimum) == false)
      return false;
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for maximum\n");
   #endif
   if (SetWrapperReferences(*variableMaximum) == false)
      return false;
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for max step\n");
   #endif
   if (SetWrapperReferences(*variableMaximumStep) == false)
      return false;
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for additive scale factor\n");
   #endif
   if (SetWrapperReferences(*additiveScaleFactor) == false)
      return false;
   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Setting refs for mult. scale factor\n");
   #endif
   if (SetWrapperReferences(*multiplicativeScaleFactor) == false)
      return false;

   #ifdef DEBUG_VARY_PARAMS
      MessageInterface::ShowMessage("Vary command Initialization complete\n");
   #endif

   return retval;
}

/**
 * Here is the old code:

{
    bool retval = GmatCommand::Initialize();

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
 */

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

   #if DEBUG_VARY_EXECUTE
      MessageInterface::ShowMessage
         ("Vary::Execute() solverDataFinalized=%d\n", solverDataFinalized);
   #endif
    
   if (!solverDataFinalized) 
   {
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
            
      #if DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage(
            "For variable \"%s\", data is [%15.9lf %15.9lf %15.9lf %15.9lf "
            "%15.9lf]\n", variableName.c_str(), varData[0], varData[1], varData[2], 
            varData[3], varData[4]);
      #endif      
   
      variableID = solver->SetSolverVariables(varData, variableName);

      #if DEBUG_VARY_EXECUTE
         MessageInterface::ShowMessage("Solver variables were set\n");
      #endif

      solverDataFinalized = true;
      BuildCommandSummary(true);
      return retval;
   }

    Real var = solver->GetSolverVariable(variableID);
    // scale using Eq. 13.6 of Architecture document
    var = var * multiplicativeScaleFactor->EvaluateReal() - 
          additiveScaleFactor->EvaluateReal();
    
    #ifdef DEBUG_VARIABLE_RANGES
       MessageInterface::ShowMessage(
          "Setting %s to %lf; allowed range is [%lf, %lf]\n",
          variableName[0].c_str(), var, variableMinimum[0], variableMaximum[0]);
    #endif

    variable->SetReal(var);

    BuildCommandSummary(true);
    return retval;
}

/**
 * Here is the old code:
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
            varData[0] = initialValue[i];                // Initial value
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
            throw CommandException(errorstr + "\n");
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
                throw CommandException(errorstr + "\n");
            } 
        
            Gmat::ParameterType type = obj->GetParameterType(id);
            if (type != Gmat::REAL_TYPE) {
                std::string errorstr = "The solver variable ";
                errorstr += parmName;
                errorstr += " on object ";
                errorstr += objectName;
                errorstr += " is not Real.";
                throw CommandException(errorstr + "\n");
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

*/

void Vary::RunComplete()
{
   solverDataFinalized = false;
   GmatCommand::RunComplete();
}
