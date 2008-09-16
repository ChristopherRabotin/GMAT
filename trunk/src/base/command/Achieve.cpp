//$Id$
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
/// @todo Rework command so it doesn't need the Moderator!!!
///#include "Moderator.hpp" 
#include "StringUtil.hpp"  // for ToReal()
#include <sstream>
#include "MessageInterface.hpp"

//#define DEBUG_ACHIEVE_PARSE 1
//#define DEBUG_ACHIEVE_INIT 1
//#define DEBUG_ACHIEVE_EXEC 1
//#define DEBUG_ACHIEVE_PARAMS
//#define DEBUG_WRAPPER_CODE


//---------------------------------
// static data
//---------------------------------
const std::string
   Achieve::PARAMETER_TEXT[AchieveParamCount - GmatCommandParamCount] =
   {
      "TargeterName",
      "Goal",
      "GoalValue",
      "Tolerance"
   };
   
const Gmat::ParameterType
   Achieve::PARAMETER_TYPE[AchieveParamCount - GmatCommandParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      //Gmat::REAL_TYPE
   };


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Achieve()
//------------------------------------------------------------------------------
/**
 * Creates an Achieve command.  (default constructor)
 */
//------------------------------------------------------------------------------
Achieve::Achieve() :
   GmatCommand             ("Achieve"),
   targeterName            (""),
   goalName                (""),
   goal                    (NULL),
   achieveName             (""),
   //achieveArrName          (""),
   //achieveArrRowStr        (""),
   //achieveArrColStr        (""),
   //achieveArrRow           (-1),
   //achieveArrCol           (-1),
   //achieveArrRowParm       (NULL),
   //achieveArrColParm       (NULL),   
   achieve                 (NULL),
   toleranceName           ("0.1"),
   tolerance               (NULL),
   //goalObject              (NULL),
   //parmId                  (-1),
   goalId                  (-1),
   targeter                (NULL),
   targeterDataFinalized   (false)//,
   //isAchieveParm           (false),
   //isAchieveArray          (false),
   //goalParm                (NULL)
{
   settables.push_back("Tolerance"); 
   parameterCount = AchieveParamCount;
}


//------------------------------------------------------------------------------
//  ~Achieve()
//------------------------------------------------------------------------------
/**
 * Destroys the Achieve command.  (destructor)
 */
//------------------------------------------------------------------------------
Achieve::~Achieve()
{
   ClearWrappers();
}

    
//------------------------------------------------------------------------------
//  Achieve(const Achieve& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Achieve command.  (Copy constructor)
 *
 * @param t Command that is replicated here.
 */
//------------------------------------------------------------------------------
Achieve::Achieve(const Achieve& t) :
   GmatCommand             (t),
   targeterName            (t.targeterName),
   goalName                (t.goalName),
   goal                    (NULL),
   achieveName             (t.achieveName),
   //achieveArrName          (t.achieveArrName),
   //achieveArrRowStr        (t.achieveArrRowStr),
   //achieveArrColStr        (t.achieveArrColStr),
   //achieveArrRow           (t.achieveArrRow),
   //achieveArrCol           (t.achieveArrCol),
   //achieveArrRowParm       (NULL),
   //achieveArrColParm       (NULL),   
   achieve                 (NULL),
   toleranceName           (t.toleranceName),
   tolerance               (NULL),
   //goalObject              (NULL),
   //parmId                  (t.parmId),
   goalId                  (t.goalId),
   targeter                (NULL),
   targeterDataFinalized   (false)//,
   //isAchieveParm           (t.isAchieveParm),
   //isAchieveArray          (t.isAchieveArray),
   //goalParm                (NULL)
{
   parameterCount = AchieveParamCount;
}


//------------------------------------------------------------------------------
//  Achieve& operator=(const Achieve& t)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Achieve command.
 *
 * @param t Command that is replicated here.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Achieve& Achieve::operator=(const Achieve& t)
{
   if (this == &t)
      return *this;
    
   GmatCommand::operator=(t);
   targeterName          = t.targeterName;
   goalName              = t.goalName;
   goal                  = NULL;
   achieveName           = t.achieveName;
   achieve               = NULL;
   toleranceName         = t.toleranceName;
   tolerance             = NULL;
   goalId                = t.goalId;
   targeter              = NULL;
   targeterDataFinalized = false;
   //achieveArrName = t.achieveArrName;
   //achieveArrRowStr = t.achieveArrRowStr;
   //achieveArrColStr = t.achieveArrColStr;
   //achieveArrRow = t.achieveArrRow;
   //achieveArrCol = t.achieveArrCol;
   //achieveArrRowParm = NULL;
   //achieveArrColParm = NULL;
   //achieveParm = NULL;
   //goalObject = NULL;
   //parmId = t.parmId;
   //isAchieveParm = t.isAchieveParm;
   //isAchieveArray = t.isAchieveArray;
   //goalParm = NULL;

   return *this;
}



//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Achieve.
 *
 * @return clone of the Achieve.
 */
//------------------------------------------------------------------------------
GmatBase* Achieve::Clone() const
{
   return (new Achieve(*this));
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames referenced objects.
 *
 * @param type Type of the object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The name the object has when this operation is complete.
 *
 * @return true on success.
 */
//------------------------------------------------------------------------------
bool Achieve::RenameRefObject(const Gmat::ObjectType type,
                              const std::string &oldName,
                              const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (targeterName == oldName)
         targeterName = newName;
   }
   //else if (type == Gmat::SPACECRAFT)
   //{
   //   // set new goal name using object and property
   //   std::string::size_type pos = goalName.find(oldName);
   //   if (pos != goalName.npos)
   //      goalName.replace(pos, oldName.size(), newName);
   //
   //   // achieve name
   //   if (isAchieveParm)
   //   {
   //      pos = achieveName.find(oldName);
   //      if (pos != goalName.npos)
   //         achieveName.replace(pos, oldName.size(), newName);
   //   }
   //}
   //else if (type == Gmat::PARAMETER)
   //{
   //   if (goalName == oldName)
   //      goalName = newName;
   //
   //   if (achieveName == oldName)
   //      achieveName = newName;
   //
   //   if (achieveArrName == oldName)
   //      achieveArrName = newName;
   //}
   // make sure the wrappers know to rename any objects they may be using
   if (goal)
   {
      goal->RenameObject(oldName, newName);
      goalName           = goal->GetDescription();
   }
   if (achieve)
   {
      achieve->RenameObject(oldName, newName);
      achieveName           = achieve->GetDescription();
   }
   if (tolerance)
   {
      tolerance->RenameObject(oldName, newName);
      toleranceName           = tolerance->GetDescription();
   }
   
   return true;
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Achieve.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& Achieve::GetRefObjectTypeArray()
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
 * Retrieves the list of ref objects used by the Achieve.
 *
 * @param <type> The type of object desired, or Gmat::UNKNOWN_OBJECT for the
 *               full list.
 * 
 * @return the list of object names.
 * 
 */
//------------------------------------------------------------------------------
const StringArray& Achieve::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   refObjectNames.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT ||
       type == Gmat::SOLVER)
   {
      refObjectNames.push_back(targeterName);
   }
   
   //if (type == Gmat::UNKNOWN_OBJECT ||
   //    type == Gmat::PARAMETER)
   //{
   //   refObjectNames.push_back(goalName);
   //   
   //   if (isAchieveParm)
   //   {
   //      if (isAchieveArray)
   //      {
   //         refObjectNames.push_back(achieveArrName);
   //         if (achieveArrRow == -1)
   //            refObjectNames.push_back(achieveArrRowStr);
   //         if (achieveArrCol == -1)
   //            refObjectNames.push_back(achieveArrColStr);
   //      }
   //      else
   //      {
   //         refObjectNames.push_back(achieveName);
   //      }
   //   }
   //}
   
   return refObjectNames;
}


// Parameter accessors

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Achieve::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < AchieveParamCount)
      return PARAMETER_TEXT[id - GmatCommandParamCount];
   return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer Achieve::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < AchieveParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatCommandParamCount])
         return i;
   }

   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Achieve::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < AchieveParamCount)
      return PARAMETER_TYPE[id - GmatCommandParamCount];

   return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Achieve::GetParameterTypeString(const Integer id) const
{
   return GmatCommand::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param id ID for the requested parameter value.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Achieve::GetRealParameter(const Integer id) const
{
   if (id == goalNameID)
      if (goal) return goal->EvaluateReal();
   if (id == goalValueID)
      if (achieve) return achieve->EvaluateReal();
   if (id == toleranceID)
      if (tolerance) return tolerance->EvaluateReal();
    
   return GmatCommand::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param id    ID for the parameter whose value to change.
 * @param value value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real Achieve::SetRealParameter(const Integer id, const Real value)
{
   //note this code will not be entered since all parameter types are STRING_TYPE
   // so moved this range check to SetTolerance()
   // SetTolerance() is called from SetStringParameter()
   
   //if (id == toleranceID) 
   //{
   //   if (value > 0.0)
   //      if (tolerance)
   //         if (tolerance->SetReal(value)) return value;
   //   else
   //   {
   //      std::stringstream buffer;
   //      buffer << value;
   //      throw CommandException(
   //         "The value of \"" + buffer.str() + "\" for field \"Tolerance\""
   //         " on object \"" + instanceName + "\" is not an allowed value.\n"
   //         "The allowed values are: [ Real > 0.0 ].");
   //   }
   //}
   
   return GmatCommand::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Achieve::GetStringParameter(const Integer id) const
{
   if (id == targeterNameID)
      return targeterName;
        
   if (id == goalNameID)
      return goalName;
        
   if (id == goalValueID) 
      return achieveName;

   if (id == toleranceID) 
      return toleranceName;

   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id    ID for the requested parameter.
 * @param value string value for the requested parameter.
 *
 * @return  success flag.
 */
//------------------------------------------------------------------------------
bool Achieve::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_ACHIEVE_PARAMS
   MessageInterface::ShowMessage
      ("Achieve::SetStringParameter() id=%d, value=%s\n", id, value.c_str());
   #endif
   
   if (id == targeterNameID) 
   {
      targeterName = value;
      return true;
   }

   if (id == goalNameID) 
   {
      goalName = value;
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      return true;
   }
   
   if (id == goalValueID) 
   {
      achieveName = value;
      // Goal value can be either a parameter or a number; ConstructGoal determines this.
      //Real realValue;
      //if (ConstructGoal(value.c_str()))
      //{
      //   // It's a parameter; just set dummy value here -- gets reset on execution
      //   realValue = 54321.0;
      //   isAchieveParm = true;
      //}
      //else
      //{
      //   realValue = atof(achieveName.c_str());
      //   isAchieveParm = false;
      //}
      
      //#ifdef DEBUG_ACHIEVE_PARAMS
      //   MessageInterface::ShowMessage
      //      ("Achieve::SetStringParameter() GoalString = '%s', realValue=%f\n",
      //       achieveName.c_str(), realValue);
      //#endif
         
      //goal = realValue;
   
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      return true;
   }
   
   if (id == toleranceID) 
   {
      toleranceName = value;
      if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), value) == 
          wrapperObjectNames.end())
         wrapperObjectNames.push_back(value);
      
      // Do range check here if value is a real number
      Real tol;
      if (GmatStringUtil::ToReal(value, tol))
         SetTolerance(tol);
      
      return true;
   }

   return GmatCommand::SetStringParameter(id, value);
}


// Multiple variables on the same line are not allowed in the current build.
// const StringArray& Achieve::GetStringArrayParameter(const Integer id) const; 


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
bool Achieve::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                           const std::string &name)
{   
   if (type == Gmat::SOLVER) 
   {
      if (targeterName == obj->GetName()) 
      {
         targeter = (Solver*)obj;
         return true;
      }
      return false;
   }

   return GmatCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The Achieve command has the following syntax:
 *
 *     Achieve myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});
 *     Achieve myDC(Sat1.SMA = Var1, {Tolerance = 0.1});
 *     Achieve myDC(Sat1.SMA = Arr1(1,1), {Tolerance = 0.1});
 *     Achieve myDC(Sat1.SMA = Arr1(I,J), {Tolerance = 0.1});
 *
 * where myDC is a Solver used to Achieve a set of variables to achieve the
 * corresponding goals.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
bool Achieve::InterpretAction()
{
   // Clean out any old data
   wrapperObjectNames.clear();
   ClearWrappers();

   StringArray chunks = InterpretPreface();

   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage("Preface chunks as\n");
      for (StringArray::iterator i = chunks.begin(); i != chunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
      MessageInterface::ShowMessage("\n");
   #endif
      
   if (chunks.size() <= 1)
      throw CommandException("Missing information for Achieve command.\n");
   
   if (chunks[1].at(0) == '(')
      throw CommandException("Missing solver name for Achieve command.\n");
      
   if ((chunks[1].find("[") != chunks[1].npos) || (chunks[1].find("]") != chunks[1].npos))
      throw CommandException("Brackets not allowed in Vary command");

   if (!GmatStringUtil::AreAllBracketsBalanced(chunks[1], "({)}"))
   {
      throw CommandException
         ("Parentheses, braces, or brackets are unbalanced or incorrectly placed\n");
   }
   
   // Find and set the solver object name
   // This is the only setting in Achieve that is not in a wrapper
   StringArray currentChunks = parser.Decompose(chunks[1], "()", false);
   SetStringParameter(targeterNameID, currentChunks[0]);
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage("current chunks as\n");
      for (StringArray::iterator i = currentChunks.begin(); i != currentChunks.end(); ++i)
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
      MessageInterface::ShowMessage("\n");
   #endif
   
   // The remaining text in the instruction is the variable definition and 
   // parameters, all contained in currentChunks[1].  Deal with those next.
   //currentChunks = parser.SeparateBrackets(currentChunks[1], "()", ", ");
   //std::string noLeftBrace  = GmatStringUtil::RemoveAll(currentChunks[1],'{');
   //std::string noRightBrace = GmatStringUtil::RemoveAll(noLeftBrace,'}');
   //std::string noSpaces     = GmatStringUtil::RemoveAll(noRightBrace,' ');
   std::string noSpaces2     = GmatStringUtil::RemoveAll(currentChunks[1],' ');
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage(
         "Achieve: noSpaces2 = %s\n", noSpaces2.c_str());
   #endif   
   currentChunks = parser.Decompose(noSpaces2, "()", true, true);
   //currentChunks = parser.Decompose(currentChunks[1], "()", true, true);
   
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage(
         "Achieve: after Decompose, current chunks = \n");
      for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
         MessageInterface::ShowMessage("   %s\n",
                                       currentChunks[jj].c_str());
   #endif

   // First chunk is the goal and achieve (target) value
   std::string lhs, rhs;
   if (!SeparateEquals(currentChunks[0], lhs, rhs, true))
      // Variable takes default initial value
      //rhs = "0.0";
   {
      throw CommandException("The goal \"" + lhs + 
         "\" is missing the \"=\" operator or a goal value required for an " + typeName + 
         " command.\n");
   }
      
   goalName = lhs;
   //variableID = -1;

   //variableValueString = rhs;
   achieveName = rhs;
   
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage(
         "Achieve:  setting goalName to %s\n", goalName.c_str());
      MessageInterface::ShowMessage(
         "Achieve:  setting achieveName to %s\n", achieveName.c_str());
   #endif
   
   // if there are no more chunks, just return
   if (currentChunks.size() == (Integer) 1) return true;
   
   std::string noSpaces     = GmatStringUtil::RemoveAll(currentChunks[1],' ');
   // Now deal with the settable parameters
   //currentChunks = parser.SeparateBrackets(currentChunks[1], "{}", ",", false);
   currentChunks = parser.SeparateBrackets(noSpaces, "{}", ",", true);
   
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage(
         "Achieve: After SeparateBrackets, current chunks = \n");
      for (Integer jj = 0; jj < (Integer) currentChunks.size(); jj++)
         MessageInterface::ShowMessage("   %s\n",
                                       currentChunks[jj].c_str());
   #endif
   
   // currentChunks now holds all of the pieces - no need for more separation  
   
   for (StringArray::iterator i = currentChunks.begin(); 
        i != currentChunks.end(); ++i)
   {
      bool isOK = SeparateEquals(*i, lhs, rhs, true);
      #ifdef DEBUG_ACHIEVE_PARSE
         MessageInterface::ShowMessage("Setting Achieve properties\n");
         MessageInterface::ShowMessage("   \"%s\" = \"%s\"\n", lhs.c_str(), rhs.c_str());
      #endif
      
      if (!isOK || lhs.empty() || rhs.empty())
         throw CommandException("The setting \"" + lhs + 
            "\" is missing the \"=\" operator or a value required for an " + typeName + 
            " command.\n");
      
      if (IsSettable(lhs))
         SetStringParameter(GetParameterID(lhs), rhs);
      else
         throw CommandException("The setting \"" + lhs + 
            "\" is not a valid setting for an " + typeName + 
            " command.\n");
   }
   
   return true;
/* old code
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
      throw CommandException("Achieve string does not specify the targeter");
   
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

   // Get an instance if this is a Parameter
   Moderator *mod = Moderator::Instance();

   #if DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage
         ("Achieve::InterpretAction() goalName = \"%s\"\n", goalName.c_str());
   #endif

   std::string parmObj, parmType, parmSystem;
   InterpretParameter(goalName, parmType, parmObj, parmSystem);

   #if DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage
         ("Achieve::InterpretAction() parmObj=%s, parmType=%s, "
          "parmSystem = \"%s\"\n", parmObj.c_str(),
          parmType.c_str(), parmSystem.c_str());
   #endif

   goalParm = mod->CreateParameter(parmType, goalName, parmObj, parmSystem);

   if (!goalParm)
      throw CommandException("Unable to create parameter " + goalName);
         

   // Find the value
   loc = end + 1;
   
   // Goal can be either a parameter or array or a number;
   // ConstructGoal determines this.
   Real value;
   if (ConstructGoal(&str[loc]))
   {
      // It's a parameter; just set dummy value here -- gets reset on execution
      value = 54321.0;
      isAchieveParm = true;
   }
   else
   {
      value = atof(achieveName.c_str());
      isAchieveParm = false;
   }
   
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage
         ("Achieve::InterpretAction() GoalString = '%s'\n", achieveName.c_str());
   #endif
   
   goal = value;

   // Find tolerance
   loc = generatingString.find("Tolerance", strend);
   if (loc == (Integer)generatingString.npos)
   {
      // Set default tolerance
      value = 0.0001;
   }
   else
   {
      end = generatingString.find("=", loc);
      value = atof(&str[end+1]);
   }
   
   SetRealParameter(toleranceID, value);
    
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage("Achieve::InterpretAction() exiting\n");
   #endif
      
   return true;
   */
}

const StringArray& Achieve::GetWrapperObjectNameArray()
{
   wrapperObjectNames.clear();

   wrapperObjectNames.push_back(goalName);
   
   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       achieveName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(achieveName);

   if (find(wrapperObjectNames.begin(), wrapperObjectNames.end(), 
       toleranceName) == wrapperObjectNames.end())
      wrapperObjectNames.push_back(toleranceName);
   
   return wrapperObjectNames;
}


bool Achieve::SetElementWrapper(ElementWrapper *toWrapper, 
              const std::string &withName)
{
   bool retval = false;

   if (toWrapper == NULL) return false;
   
   if (toWrapper->GetWrapperType() == Gmat::ARRAY)
   {
      throw CommandException("A value of type \"Array\" on command \"" + 
                  typeName + 
                  "\" is not an allowed value.\nThe allowed values are:"
                  " [ Real Number, Variable, Array Element, or Parameter ]. "); 
   }
   /*
   if (toWrapper->GetWrapperType() == Gmat::STRING_OBJECT)
   {
      throw CommandException("A value of type \"String Object\" on command \"" 
                  + typeName + 
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
   CheckDataType(toWrapper, Gmat::REAL_TYPE, "Achieve", true);

   #ifdef DEBUG_WRAPPER_CODE   
   MessageInterface::ShowMessage(
               "   Setting wrapper \"%s\" on Achieve command\n", 
      withName.c_str());
   #endif

   if (goalName == withName)
   {
      if (toWrapper->GetWrapperType() == Gmat::NUMBER)
      {
         std::string errmsg = "The value of \"" + goalName;
         errmsg            += "\" for field \"Goal\" on object \"";
         errmsg            += instanceName + "\" is not an allowed value.\n";
         errmsg            += "The allowed values are: ";
         errmsg            += "[ Object Property, Array Element, Variable, ";
         errmsg            += "or Parameter, excluding numbers].";
         throw CommandException(errmsg);
       }
      goal = toWrapper;
      retval = true;
   }
   
   if (achieveName == withName)
   {
      achieve = toWrapper;
      retval = true;
   }
   
   if (toleranceName == withName)
   {
      tolerance = toWrapper;
      retval = true;
   }
   return retval;
}

void Achieve::ClearWrappers()
{
   std::vector<ElementWrapper*> temp;
   if (goal)
   {
      temp.push_back(goal);
      goal = NULL;
   }
   if (achieve)
   {
      if (find(temp.begin(), temp.end(), achieve) == temp.end())
      {
         temp.push_back(achieve);
         achieve = NULL;
      }
   }
   if (tolerance)
   {
      if (find(temp.begin(), temp.end(), tolerance) == temp.end())
      {
         temp.push_back(tolerance);
         tolerance = NULL;
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
// bool ConstructGoal(const char* str)
//------------------------------------------------------------------------------
/**
 * Builds goals -- either as a parameter or as a numerc value, depending on the
 * script contents.
 * 
 * @return true on success, false on failure.
 * @return true if it is a parameter, false otherwise.
 */
//------------------------------------------------------------------------------
/*
bool Achieve::ConstructGoal(const char* str)
{
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage("%s%s\"\n",
         "Achieve::ConstructGoal() called with string \"", str);
   #endif

   Moderator *mod = Moderator::Instance();
   
   Real rval = 54321.12345;
   // check to see if it is a number first
   if (GmatStringUtil::ToReal(str, &rval))
      return false;
   
   // Skip white space
   Integer start = 0, dot, end;
   while (str[start] == ' ')
      ++start;
   // Find last character in this piece
   end = dot = start;
   while ((str[end] != ' ') && (str[end] != ',') && (str[end] != '{')) {
      if ((str[end] == '.') && (dot == start))
         dot = end;
      ++end;
   }
   
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage
         ("   start=%d, dot=%d, end=%d\n", start, dot, end);
   #endif

   std::string sstr = str;
   achieveName = sstr.substr(start, end-start);
   
   // Search for 2nd comma for array index
   UnsignedInt index1;
   if (str[end] == ',')
   {
      index1 = sstr.find(',', end+1);
      if (index1 != sstr.npos)
      {
         achieveName = sstr.substr(start, index1-start);
         isAchieveArray = true;
      }
      else 
      {
         // check for tolerance
         if (sstr.find('{', end+1) == sstr.npos)
         {
            if (sstr.find(')', end+1) != sstr.npos)
            {
               achieveName = sstr.substr(start, sstr.size()-start-1);
               end = sstr.size()-1;
               isAchieveArray = true;
            }
         }
      }
   }
   
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage
         ("%s%s\", isAchieveArray=%d\n", "   achieveName is \"",
          achieveName.c_str(), isAchieveArray);
      MessageInterface::ShowMessage
         ("%s%s\"\n", "   Examining the substring \"",
          sstr.substr(start, end-start).c_str());
   #endif

   if (isAchieveArray)
   {
      GmatStringUtil::GetArrayIndex(achieveName, achieveArrRowStr,
                                    achieveArrColStr, achieveArrRow,
                                    achieveArrCol, achieveArrName);

      //loj: 9/20/06 Commented out to show array index in the GUI and show script
      //achieveName = achieveArrName;
      
      #if DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage
         ("   achieveArrRowStr=%s, achieveArrColStr=%s, achieveArrRow=%d, "
          "achieveArrCol=%d, achieveArrName=%s\n", achieveArrRowStr.c_str(),
          achieveArrColStr.c_str(), achieveArrRow, achieveArrCol,
          achieveArrName.c_str());
      #endif
      
      // if variable index is used, make sure the variable is created
      if (achieveArrRow == -1)
         if (mod->GetParameter(achieveArrRowStr) == NULL)
            throw CommandException("Cannot find array row index variable: " +
                                   achieveArrRowStr);
      
      if (achieveArrCol == -1)
         if (mod->GetParameter(achieveArrColStr) == NULL)
            throw CommandException("Cannot find array column index variable: " +
                                   achieveArrColStr);
      
      return true;
   }
   else if ((dot > start) && (dot < end)) // Could be a system parameter
   {
      
      // try if Tolerance is missing
      UnsignedInt index = achieveName.find(")");
      if (index != achieveName.npos)
         achieveName = achieveName.substr(0, index);
      
      std::string parmType, parmObj, parmSystem;
      InterpretParameter(achieveName, parmType, parmObj, parmSystem);

      #if DEBUG_ACHIEVE_PARSE
         MessageInterface::ShowMessage
            ("   parmObj=%s, parmType=%s, parmSystem = \"%s\"\n",
             parmObj.c_str(), parmType.c_str(), parmSystem.c_str());
      #endif

         //Moderator *mod = Moderator::Instance();

      if (mod->IsParameter(parmType))
      {
         achieveParm =
            mod->CreateParameter(parmType, achieveName, parmObj, parmSystem);
        
         if (!achieveParm)
            throw CommandException("Unable to create parameter " + achieveName);

         return true;
      }
      else
      {
         #ifdef DEBUG_ACHIEVE_PARSE
         MessageInterface::ShowMessage
            ("   \"%s\" is not a parameter\n", achieveName.c_str());
         #endif
      }
   }

   if (GmatStringUtil::ToReal(achieveName, &rval))
   {
      #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage
         ("   \"%s\" is a number\n", achieveName.c_str());
      #endif
      return false;
   }
   else
   {
      #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage
         ("   \"%s\" is a variable\n", achieveName.c_str());
      #endif
      return true;
   }
   
   
   //return false;
}
*/

//------------------------------------------------------------------------------
//  bool InterpretParameter(const std::string text, std::string &paramType,
//                          std::string &paramObj, std::string &parmSystem)
//------------------------------------------------------------------------------
/**
 * Breaks apart a parameter declaration into its component pieces
 *
 * @param text The string that gets decomposed.
 * @param paramType Type of parameter that is needed.
 * @param paramObj The Object used for the parameter calculations.
 * @param parmSystem The coordinate system or body used for the parameter
 *                   calculations (or the empty string if this piece is
 *                   unspecified).
 *
 * @return true if the decomposition worked.
 */
//------------------------------------------------------------------------------
/*
bool Achieve::InterpretParameter(const std::string text,
                                 std::string &paramType, 
                                 std::string &paramObj, 
                                 std::string &parmSystem)
{
   
   Real rval = 54321.12345;
   // check to see if it is a number first
   if (GmatStringUtil::ToReal(text, &rval))
      return true;
   
   Integer start = 0, dotLoc = text.find(".", 0);
   if (dotLoc == (Integer)std::string::npos)
      throw CommandException("Achieve::InterpretParameter: Unable to "
               "interpret parameter object in the string " +
               text);
   
   paramObj = text.substr(start, dotLoc - start);
   start = dotLoc + 1;
   dotLoc = text.find(".", start);
   if (dotLoc != (Integer)std::string::npos) {
      parmSystem = text.substr(start, dotLoc - start);
      start = dotLoc + 1;
   }
   else {
      parmSystem = "";
   }
   
   paramType = text.substr(start);
   
   #ifdef DEBUG_ACHIEVE_PARSE
      MessageInterface::ShowMessage(
         "Achieve::InterpretParameter() Built parameter %s for object %s "
         "with CS %s\n", paramType.c_str(), paramObj.c_str(), parmSystem.c_str());
   #endif
      
   return true;
}

*/
//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Achieve command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Achieve::Initialize()
{
   #if DEBUG_ACHIEVE_INIT
   //MessageInterface::ShowMessage
   //   ("Achieve::Initialize() entered. targeter=%p, goalParm=%p, goalObject=%p, "
   //    "parmId=%d\n", targeter, goalParm, goalObject, parmId);
   #endif
   
   bool retval = GmatCommand::Initialize();

   if (targeter == NULL)
      throw CommandException(
         "Targeter not initialized for Achieve command\n  \""
         + generatingString + "\"\n");

   Integer id = targeter->GetParameterID("Goals");
   targeter->SetStringParameter(id, goalName);

   // Set references for the wrappers   
   #ifdef DEBUG_ACHIEVE_PARAMS
      MessageInterface::ShowMessage("Setting refs for goal\n");
   #endif
   if (SetWrapperReferences(*goal) == false)
      return false;
   CheckDataType(goal, Gmat::REAL_TYPE, "Achieve");
   #ifdef DEBUG_ACHIEVE_PARAMS
      MessageInterface::ShowMessage("Setting refs for achieve\n");
   #endif
   if (SetWrapperReferences(*achieve) == false)
      return false;
   CheckDataType(achieve, Gmat::REAL_TYPE, "Achieve");
   #ifdef DEBUG_ACHIEVE_PARAMS
      MessageInterface::ShowMessage("Setting refs for tolerance\n");
   #endif
   if (SetWrapperReferences(*tolerance) == false)
      return false;
   CheckDataType(tolerance, Gmat::REAL_TYPE, "Achieve");

   // find goalName
   //GmatBase *obj = (*objectMap)[goalName];
   
   //if (obj == NULL) {
   //   std::string errorstr = "Could not find goal parameter ";
   //   errorstr += goalName;
   //   throw CommandException(errorstr);
   //}

   //goalParm = (Parameter*)obj;
   
   //goalObject = obj;
   //parmId = id;

   // find achieveParm
   /*
   if (isAchieveParm)
   {
      #if DEBUG_ACHIEVE_INIT
      MessageInterface::ShowMessage
         ("Achieve::Initialize() Find achieveParm=%s from objectMap\n",
          achieveName.c_str());
      #endif
      
      if (objectMap->find(achieveName) != objectMap->end())
         achieveParm = (Parameter*)((*objectMap)[achieveName]);
      
      if (isAchieveArray)
      {
         if (objectMap->find(achieveArrName) != objectMap->end())
            achieveParm = (Parameter*)((*objectMap)[achieveArrName]);
         
         // if variable index is used, get variable from the objectMap
         if (achieveArrRow == -1)
            if (objectMap->find(achieveArrRowStr) != objectMap->end())
               achieveArrRowParm = (Parameter*)((*objectMap)[achieveArrRowStr]);
            else
               throw CommandException("Cannot find array row index variable\n");

         if (achieveArrCol == -1)
            if (objectMap->find(achieveArrColStr) != objectMap->end())
               achieveArrColParm = (Parameter*)((*objectMap)[achieveArrColStr]);
            else
               throw CommandException("Cannot find array column index variable\n");
      }
   }
  */ 
   // The targeter cannot be finalized until all of the loop is initialized
   targeterDataFinalized = false;
   
   //#if DEBUG_ACHIEVE_INIT
   //MessageInterface::ShowMessage
   //   ("Achieve::Initialize() exiting. targeter=%p, goalParm=%p, goalObject=%p, "
   //    "parmId=%d\n", targeter, goalParm, goalObject, parmId);
   //#endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Achieve the variables defined for this targeting loop.
 *
 * This method (will eventually) feeds data to the targeter state machine in 
 * order to determine the variable values needed to achieve the user specified
 * goals.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Achieve::Execute()
{
   #ifdef DEBUG_ACHIEVE_EXEC
   //MessageInterface::ShowMessage
   //   ("Achieve::Execute() targeterDataFinalized=%d\n   targeter=%s, addr=%p, "
   //    "achieveParm=%p, goalParm=%p, goalObject=%p\n", targeterDataFinalized,
   //    targeter->GetName().c_str(), targeter, achieveParm, goalParm, goalObject);
   MessageInterface::ShowMessage
      ("   goalName=%s, achieveName=%s\n", goalName.c_str(), achieveName.c_str());
   //if (achieveParm)
   //   MessageInterface::ShowMessage("   achieveParm=%s\n", achieveParm->GetName().c_str());
   //if (goalParm)
   //   MessageInterface::ShowMessage("   goalParm=%s\n", goalParm->GetName().c_str());
   //if (goalObject)
   //   MessageInterface::ShowMessage("   goalObject=%s\n", goalObject->GetName().c_str());
   #endif
   
   bool retval = true;
   if (!targeterDataFinalized) 
   {
      // Tell the targeter about the goals and tolerances
      Real goalData[2];
      goalData[0] = goal->EvaluateReal();
      goalData[1] = tolerance->EvaluateReal();
      goalId = targeter->SetSolverResults(goalData, goalName);

      targeterDataFinalized = true;
      return retval;
   }
   
   Real val = -999.999;
   // Evaluate the floating target (if there is one) and set it on the targeter

   if (achieve != NULL)
   {
   //if (achieveParm != NULL)
   //{
   //   if (achieveParm->GetReturnType() == Gmat::REAL_TYPE)
   //   {
   //      val = achieveParm->EvaluateReal();
   //   }
   //   else if (achieveParm->GetReturnType() == Gmat::RMATRIX_TYPE)
   //   {
   //      Integer row = achieveArrRow;
   //      Integer col = achieveArrCol;
   //      
   //      if (achieveArrRow == -1)
   //         row = (Integer)(achieveArrRowParm->EvaluateReal()) - 1; // index starts at 0
   //
   //      if (achieveArrCol == -1)
   //         col = (Integer)(achieveArrColParm->EvaluateReal()) - 1; // index starts at 0
   //
   //      #ifdef DEBUG_ACHIEVE_EXEC
   //      MessageInterface::ShowMessage("   row=%d, col=%d\n", row, col);
   //      #endif
   //      
   //      if (row >= 0 || col >= 0)
   //         val = achieveParm->EvaluateRmatrix().GetElement(row, col);
   //      else
   //         throw CommandException("Invalid row and column in Achieve\n");
   //   }
      
      val = achieve->EvaluateReal();
      
      #ifdef DEBUG_ACHIEVE_EXEC
      MessageInterface::ShowMessage("   Floating target: val = %lf\n", val);
      #endif
      
      targeter->UpdateSolverGoal(goalId, val);
   }

   // Evaluate goal and pass it to the targeter
   //if (goalParm != NULL)
   if (goal != NULL)
   {
      //val = goalParm->EvaluateReal();
      val = goal->EvaluateReal();
      targeter->SetResultValue(goalId, val);
      #ifdef DEBUG_ACHIEVE_EXEC
      //   MessageInterface::ShowMessage
      //      ("   goalParm=%s, %p\n", goalParm->GetTypeName().c_str(), goalParm);
      //   MessageInterface::ShowMessage("   Parameter target: %s val = %lf\n",
      //      goalParm->GetTypeName().c_str(), val);
      #endif
   }
   else // isn't this an error?
   {
      val = -999.999;
      targeter->SetResultValue(goalId, val);
      //val = goalObject->GetRealParameter(parmId);
      //targeter->SetResultValue(goalId, val);
      //#ifdef DEBUG_ACHIEVE_EXEC
      //   MessageInterface::ShowMessage("   Object target: val = %lf\n", val);
      //#endif
   }

   //targeter->SetResultValue(goalId, val);
   BuildCommandSummary(true);
   
   return retval;
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
 * @param mode    Specifies the type of serialization requested.  (Not yet used
 *                in commands)
 * @param prefix  Optional prefix appended to the object's name.  (Not yet used
 *                in commands)
 * @param useName Name that replaces the object's name.  (Not yet used in
 *                commands)
 *
 * @return The script line that, when interpreted, defines this Achieve command.
 */
//------------------------------------------------------------------------------
const std::string& Achieve::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   std::string gen = prefix + "Achieve " + targeterName + "(" + goalName +
                     " = " + achieveName + ", {Tolerance = " + toleranceName;
   
   generatingString = gen + "});";
   
   // Then call the base class method for preface and inline comments
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


//------------------------------------------------------------------------------
// void RunComplete()
//------------------------------------------------------------------------------
void Achieve::RunComplete()
{
   #ifdef DEBUG_ACHIEVE_EXEC
      MessageInterface::ShowMessage(
      "In Achieve::RunComplete, targeterDataFinalized = %s, ... now setting it to false\n",
      (targeterDataFinalized? "true" : "false"));
   #endif
   targeterDataFinalized = false;
   GmatCommand::RunComplete();
}


//------------------------------------------------------------------------------
// void SetTolerance(Real value)
//------------------------------------------------------------------------------
void Achieve::SetTolerance(Real value)
{
   if (value > 0.0)
   {
      if (tolerance)
         tolerance->SetReal(value);
   }
   else
   {
      CommandException ce;
      ce.SetDetails(errorMessageFormat.c_str(),
                    GmatStringUtil::ToString(value, GetDataPrecision()).c_str(),
                    "Tolerance",
                    "Real Number, Array element, Variable, or Parameter > 0.0");
      throw ce;
   }
}

