//$Header$
//------------------------------------------------------------------------------
//                                 Minimize
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Daniel Hunter/ Wendy Shoan (GSFC/MAB)
// Created: 2006.08.11
//
/**
 * Definition for the Minimize command class
 */
//------------------------------------------------------------------------------


#include "Minimize.hpp"
/// @todo Rework command so it doesn't need the Moderator!!!
#include "Moderator.hpp" 
#include "StringUtil.hpp"  // for ToDouble()

//#define DEBUG_MINIMIZE_PARSE 1
//#define DEBUG_MINIMIZE_INIT 1
//#define DEBUG_MINIMIZE_EXEC 1


//---------------------------------
// static data
//---------------------------------
const std::string
   Minimize::PARAMETER_TEXT[MinimizeParamCount - GmatCommandParamCount] =
   {
      "OptimizerName",
      "MinimizedVariableName",
   };
   
const Gmat::ParameterType
   Minimize::PARAMETER_TYPE[MinimizeParamCount - GmatCommandParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
   };


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Minimize()
//------------------------------------------------------------------------------
/**
 * Creates an Minimize command.  (default constructor)
 */
//------------------------------------------------------------------------------
Minimize::Minimize() :
   GmatCommand             ("Minimize"),
   optimizerName           (""),
   minName                 (""),
   minVar                  (NULL),
   varValue                (-999.99),
   optimizer               (NULL),
   optimizerDataFinalized  (false),
   isMinimizeParm          (false),
   varId                   (-1)
{
   parameterCount = MinimizeParamCount;
}


//------------------------------------------------------------------------------
//  Minimize(const Minimize& t)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a Minimize command.  (Copy constructor)
 *
 * @param t Command that is replicated here.
 */
//------------------------------------------------------------------------------
Minimize::Minimize(const Minimize& m) :
   GmatCommand             (m),
   optimizerName           (m.optimizerName),
   minName                 (m.minName),
   minVar                  (NULL),
   varValue                (m.varValue),
   optimizer               (NULL),
   optimizerDataFinalized  (false),
   isMinimizeParm          (m.isMinimizeParm),
   varId                   (m.varId)
{
   parameterCount = MinimizeParamCount;
}


//------------------------------------------------------------------------------
//  Minimize& operator=(const Minimize& m)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Minimize command.
 *
 * @param t Command that is replicated here.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Minimize& Minimize::operator=(const Minimize& m)
{
   if (this == &m)
      return *this;
    
   GmatCommand::operator=(m);
   optimizerName          = m.optimizerName;
   minName                = m.minName;
   minVar                 = NULL;
   varValue               = m.varValue;
   optimizer              = NULL;
   optimizerDataFinalized = false;
   isMinimizeParm         = m.isMinimizeParm;
   varId                  = m.varId;

   return *this;
}

//------------------------------------------------------------------------------
//  ~Minimize()
//------------------------------------------------------------------------------
/**
 * Destroys the Minimize command.  (destructor)
 */
//------------------------------------------------------------------------------
Minimize::~Minimize()
{
   delete minVar;  // yes?
}

    


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Minimize.
 *
 * @return clone of the Minimize.
 */
//------------------------------------------------------------------------------
GmatBase* Minimize::Clone() const
{
   return (new Minimize(*this));
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
bool Minimize::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (optimizerName == oldName)
         optimizerName = newName;
   }
   else if (type == Gmat::PARAMETER)
   {
      if (minName == oldName)
         minName = newName;
   }
   
   return true;
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
std::string Minimize::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < MinimizeParamCount)
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
Integer Minimize::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < MinimizeParamCount; i++)
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
Gmat::ParameterType Minimize::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < MinimizeParamCount)
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
std::string Minimize::GetParameterTypeString(const Integer id) const
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
//Real Minimize::GetRealParameter(const Integer id) const
//{
//   if (id == toleranceID)
//      return tolerance;
//    
//   return GmatCommand::GetRealParameter(id);
//}


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
//Real Minimize::SetRealParameter(const Integer id, const Real value)
//{
//   if (id == toleranceID) {
//      tolerance = value;
//      return tolerance;
//   }
//    
//   return GmatCommand::SetRealParameter(id, value);
//}


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
std::string Minimize::GetStringParameter(const Integer id) const
{
   if (id == OPTIMIZER_NAME)
      return optimizerName;
        
   if (id == MINIMIZED_VARIABLE_NAME)
      return minName;
        
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
bool Minimize::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_MINIMIZE_PARAM
   MessageInterface::ShowMessage
      ("Minimize::SetStringParameter() id=%d, value=%s\n", id, value.c_str());
   #endif
   
   if (id == OPTIMIZER_NAME) {
      optimizerName = value;
      return true;
   }

   if (id == MINIMIZED_VARIABLE_NAME) {
      minName = value;
      return true;
   }

   return GmatCommand::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                                   const std::string &name = "")
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
bool Minimize::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                            const std::string &name)
{   
   if (type == Gmat::SOLVER) 
   {
      if (optimizerName == obj->GetName()) 
      {
         optimizer = (Solver*)obj;
         return true;
      }
      return false;
   }
   if (type == Gmat::PARAMETER) 
   {
      if (minName == obj->GetName()) 
      {
         minVar = (Parameter*)obj;
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
 * The Minimize command has the following syntax:
 *
 *     Minimize myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});
 *     Minimize myDC(Sat1.SMA = Var1, {Tolerance = 0.1});
 *     Minimize myDC(Sat1.SMA = Arr1(1,1), {Tolerance = 0.1});
 *     Minimize myDC(Sat1.SMA = Arr1(I,J), {Tolerance = 0.1});
 *
 * where myDC is a Solver used to Minimize a set of variables to achieve the
 * corresponding goals.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
bool Minimize::InterpretAction()
{
   /// @todo: Clean up this hack for the Minimize::InterpretAction method
   // Sample string:  "Minimize myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});"
   
   // Set starting location to the space following the command string
   Integer loc = generatingString.find("Minimize", 0) + 7, end, strend;
   const char *str = generatingString.c_str();
   
   // Skip white space
   while (str[loc] == ' ')
      ++loc;
   
   // Stop at the opening paren
   end = generatingString.find("(", loc);
   
   std::string component = generatingString.substr(loc, end-loc);
   if (component == "")
      throw CommandException("Minimize string does specify the optimizer");
   
   SetStringParameter(OPTIMIZER_NAME, component);
   
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
   minName = component;

   // Get an instance if this is a Parameter
   Moderator *mod = Moderator::Instance();

   #if DEBUG_MINIMIZE_PARSE
      MessageInterface::ShowMessage
         ("Minimize::InterpretAction() minName = \"%s\"\n", minName.c_str());
   #endif

   std::string parmObj, parmType, parmSystem;
   InterpretParameter(minName, parmType, parmObj, parmSystem);

   #if DEBUG_MINIMIZE_PARSE
      MessageInterface::ShowMessage
         ("Minimize::InterpretAction() parmObj=%s, parmType=%s, "
          "parmSystem = \"%s\"\n", parmObj.c_str(),
          parmType.c_str(), parmSystem.c_str());
   #endif

   minVar = mod->CreateParameter(parmType, minName, parmObj, parmSystem);

   if (!minVar)
      throw CommandException("Unable to create Variable " + minName);
         

   // Find the value
   loc = end + 1;
   
   // Goal can be either a parameter or array or a number;
   // ConstructGoal determines this.
   Real value;
   //if (ConstructGoal(&str[loc]))
   //{
      // It's a parameter; just set dummy value here -- gets reset on execution
      value = 54321.0;
      isMinimizeParm = true;
   //}
   //else
   //{
   //   value = atof(achieveName.c_str());
   //   isMinimizeParm = false;
   //}
   
   //#ifdef DEBUG_MINIMIZE_PARSE
   //   MessageInterface::ShowMessage
   //      ("Minimize::InterpretAction() GoalString = '%s'\n", achieveName.c_str());
   //#endif
   
   
   #ifdef DEBUG_MINIMIZE_PARSE
      MessageInterface::ShowMessage("Minimize::InterpretAction() exiting\n");
   #endif
      
   return true;
}


//------------------------------------------------------------------------------
// bool ConstructGoal(const char* str)
//------------------------------------------------------------------------------
/**
 * Builds goals -- either as a parameter or as a numeric value, depending on the
 * script contents.
 * 
 * @return true on success, false on failure.
 * @return true if it is a parameter, false otherwise.
 */
//------------------------------------------------------------------------------
/*
bool Minimize::ConstructGoal(const char* str)
{
   #ifdef DEBUG_MINIMIZE_PARSE
      MessageInterface::ShowMessage("%s%s\"\n",
         "Minimize::ConstructGoal() called with string \"", str);
   #endif

   Moderator *mod = Moderator::Instance();
   
   Real rval = 54321.12345;
   // check to see if it is a number first
   if (GmatStringUtil::ToDouble(str, &rval))
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
   
   #ifdef DEBUG_MINIMIZE_PARSE
      MessageInterface::ShowMessage
         ("   start=%d, dot=%d, end=%d\n", start, dot, end);
   #endif

   std::string sstr = str;
   achieveName = sstr.substr(start, end-start);
   
   // Search for 2nd comma for array index
   Integer comma;
   if (str[end] == ',')
   {
      comma = sstr.find(',', end+1);
      if ((UnsignedInt)comma != sstr.npos)
      {
         achieveName = sstr.substr(start, comma-start);
         isMinimizeArray = true;
      }
   }
   
   #ifdef DEBUG_MINIMIZE_PARSE
      MessageInterface::ShowMessage
         ("%s%s\", isMinimizeArray=%d\n", "   achieveName is \"",
          achieveName.c_str(), isMinimizeArray);
      MessageInterface::ShowMessage
         ("%s%s\"\n", "   Examining the substring \"",
          sstr.substr(start, end-start).c_str());
   #endif

   if (isMinimizeArray)
   {
      GmatStringUtil::GetArrayIndex(achieveName, achieveArrRowStr,
                                    achieveArrColStr, achieveArrRow,
                                    achieveArrCol, achieveArrName);

      achieveName = achieveArrName;
      
      #if DEBUG_MINIMIZE_PARSE
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
                                   achieveArrRowStr + "\n");
      
      if (achieveArrCol == -1)
         if (mod->GetParameter(achieveArrColStr) == NULL)
            throw CommandException("Cannot find array column index variable: " +
                                   achieveArrColStr + "\n");
      
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

      #if DEBUG_MINIMIZE_PARSE
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
         #ifdef DEBUG_MINIMIZE_PARSE
         MessageInterface::ShowMessage
            ("   \"%s\" is not a parameter\n", achieveName.c_str());
         #endif
      }
   }

   if (GmatStringUtil::ToDouble(achieveName, &rval))
   {
      #ifdef DEBUG_MINIMIZE_PARSE
      MessageInterface::ShowMessage
         ("   \"%s\" is a number\n", achieveName.c_str());
      #endif
      return false;
   }
   else
   {
      #ifdef DEBUG_MINIMIZE_PARSE
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
bool Minimize::InterpretParameter(const std::string text,
                                 std::string &paramType, 
                                 std::string &paramObj, 
                                 std::string &parmSystem)
{
   
   Real rval = 54321.12345;
   // check to see if it is a number first
   if (GmatStringUtil::ToDouble(text, &rval))
      return true;
   
   Integer start = 0, dotLoc = text.find(".", 0);
   if (dotLoc == (Integer)std::string::npos)
      throw CommandException("Minimize::InterpretParameter: Unable to "
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
   
   #ifdef DEBUG_MINIMIZE_PARSE
      MessageInterface::ShowMessage(
         "Minimize::InterpretParameter() Built parameter %s for object %s "
         "with CS %s\n", paramType.c_str(), paramObj.c_str(), parmSystem.c_str());
   #endif
      
   return true;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the Minimize command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool Minimize::Initialize()
{
   #if DEBUG_MINIMIZE_INIT
   MessageInterface::ShowMessage
      ("Minimize::Initialize() entered. optimizer=%p, minVar=%p\n", 
      optimizer, minVar);
   #endif
   
   bool retval = GmatCommand::Initialize();

   if (optimizer == NULL)
      throw CommandException(
         "Optimizer not initialized for Minimize command\n  \""
         + generatingString + "\"\n");

   //Integer id = optimizer->GetParameterID("Goals");  // no clue
   //optimizer->SetStringParameter(id, goalName);

   // find goalName
   GmatBase *obj = (*objectMap)[minName];
   
   if (obj == NULL) {
      std::string errorstr = "Could not find variable parameter ";
      errorstr += minName;
      throw CommandException(errorstr);
   }

   minVar = (Parameter*)obj;
   
   //goalObject = obj;
   //parmId = id;

   // find achieveParm
   /*
   if (isMinimizeParm)
   {
      #if DEBUG_MINIMIZE_INIT
      MessageInterface::ShowMessage
         ("Minimize::Initialize() Find achieveParm=%s from objectMap\n",
          achieveName.c_str());
      #endif
      if (objectMap->find(achieveName) != objectMap->end())
         achieveParm = (Parameter*)((*objectMap)[achieveName]);

      if (isMinimizeArray)
      {
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
   // The optimizer cannot be finalized until all of the loop is initialized
   optimizerDataFinalized = false;
   
   #if DEBUG_MINIMIZE_INIT
   MessageInterface::ShowMessage
      ("Minimize::Initialize() exiting. optimizer=%p, minVar=%p\n", 
      optimizer, minVar);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Minimize the variables defined for this targeting loop.
 *
 * This method (will eventually) feeds data to the targeter state machine in 
 * order to determine the variable values needed to achieve the user specified
 * goals.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Minimize::Execute()
{
   #ifdef DEBUG_MINIMIZE_EXEC
   MessageInterface::ShowMessage
      ("Minimize::Execute() optimizerDataFinalized=%d\n, optimizer=%s, minVar=%p\n", 
       optimizerDataFinalized, optimizer, minVar);
   MessageInterface::ShowMessage
      ("   minName=%s\n", minName.c_str());
   //if (achieveParm)
   //   MessageInterface::ShowMessage("   achieveParm=%s\n", achieveParm->GetName().c_str());
   if (minVar)
      MessageInterface::ShowMessage("   minVar=%s\n", minVar->GetName().c_str());
   //if (goalObject)
   //   MessageInterface::ShowMessage("   goalObject=%s\n", goalObject->GetName().c_str());
   #endif
   
   bool retval = true;
   if (!optimizerDataFinalized) {
      // Tell the targeter about the goals and tolerances
      Real minData[1];
      minData[0] = varValue;
      //minData[1] = tolerance;
      varId = optimizer->SetSolverResults(minData, minName);

      optimizerDataFinalized = true;
      return retval;
   }
   
   Real val = -999.999;
   /*  no clue what to do here .......
   // Evaluate the floating target (if there is one) and set it on the targeter
   if (achieveParm != NULL)
   {
      if (achieveParm->GetReturnType() == Gmat::REAL_TYPE)
      {
         val = achieveParm->EvaluateReal();
      }
      else if (achieveParm->GetReturnType() == Gmat::RMATRIX_TYPE)
      {
         Integer row = achieveArrRow;
         Integer col = achieveArrCol;
         
         if (achieveArrRow == -1)
            row = (Integer)(achieveArrRowParm->EvaluateReal()) - 1; // index starts at 0

         if (achieveArrCol == -1)
            col = (Integer)(achieveArrColParm->EvaluateReal()) - 1; // index starts at 0

         #ifdef DEBUG_MINIMIZE_EXEC
         MessageInterface::ShowMessage("   row=%d, col=%d\n", row, col);
         #endif
         
         if (row >= 0 || col >= 0)
            val = achieveParm->EvaluateRmatrix().GetElement(row, col);
         else
            throw CommandException("Invalid row and column in Minimize\n");
      }
      
      #ifdef DEBUG_MINIMIZE_EXEC
      MessageInterface::ShowMessage("   Floating target: val = %lf\n", val);
      #endif
      
      targeter->UpdateSolverGoal(goalId, val);
   }
   */

   // Evaluate variable and pass it to the optimizer
   if (minVar != NULL)
   {
      val = minVar->EvaluateReal();
      optimizer->SetResultValue(varId, val);
      #ifdef DEBUG_MINIMIZE_EXEC
         MessageInterface::ShowMessage
            ("   minVar=%s, %p\n", minVar->GetTypeName().c_str(), minVar);
         MessageInterface::ShowMessage("   Parameter target: %s val = %lf\n",
            minVar->GetTypeName().c_str(), val);
      #endif
   }
   else 
   {
      val = -999.999;
      //val = goalObject->GetRealParameter(parmId);  // again,no clue
      optimizer->SetResultValue(varId, val);
      #ifdef DEBUG_MINIMIZE_EXEC
         MessageInterface::ShowMessage("   Object target: val = %lf\n", val);
      #endif
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
 * @return The script line that, when interpreted, defines this Minimize command.
 */
//------------------------------------------------------------------------------
const std::string& Minimize::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   std::stringstream tol;
   //tol << tolerance;
   std::string gen = prefix + "Minimize " + optimizerName + "(" + minName;

   generatingString = gen + ");";
   // Then call the base class method
   return GmatCommand::GetGeneratingString();
}


