//$Header$
//------------------------------------------------------------------------------
//                                 NonLinearConstraint
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Wendy Shoan (GSFC/MAB)
// Created: 2006.08.14
//
/**
 * Definition for the NonLinearConstraint command class
 */
//------------------------------------------------------------------------------


#include "NonLinearConstraint.hpp"
/// @todo Rework command so it doesn't need the Moderator!!!
#include "Moderator.hpp" 
#include "StringUtil.hpp"  // for ToDouble()

//#define DEBUG_NONLINEAR_CONSTRAINT_PARSE 1
//#define DEBUG_NONLINEAR_CONSTRAINT_INIT 1
//#define DEBUG_NONLINEAR_CONSTRAINT_EXEC 1


//---------------------------------
// static data
//---------------------------------
const std::string
   NonLinearConstraint::PARAMETER_TEXT[NonLinearConstraintParamCount - GmatCommandParamCount] =
   {
      "OptimizerName",
      "ConstrainedVariableName",
      "ConstrainedValue",
   };
   
const Gmat::ParameterType
   NonLinearConstraint::PARAMETER_TYPE[NonLinearConstraintParamCount - GmatCommandParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::REAL_TYPE,
   };


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  NonLinearConstraint()
//------------------------------------------------------------------------------
/**
 * Creates an NonLinearConstraint command.  (default constructor)
 */
//------------------------------------------------------------------------------
NonLinearConstraint::NonLinearConstraint() :
   GmatCommand             ("NonLinearConstraint"),
   optimizerName           (""),
   //minName                 (""),
   //minVar                  (NULL),
   //varValue                (-999.99),
   optimizer               (NULL),
   optimizerDataFinalized  (false)//,
   //isNonLinearConstraintParm          (false),
   //varId                   (-1)
{
   parameterCount = NonLinearConstraintParamCount;
}


//------------------------------------------------------------------------------
//  NonLinearConstraint(const NonLinearConstraint& nlc)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a NonLinearConstraint command.  (Copy constructor)
 *
 * @param t Command that is replicated here.
 */
//------------------------------------------------------------------------------
NonLinearConstraint::NonLinearConstraint(const NonLinearConstraint& nlc) :
   GmatCommand             (nlc),
   optimizerName           (nlc.optimizerName),
   //minName                 (nlc.minName),
   //minVar                  (NULL),
   //varValue                (nlc.varValue),
   optimizer               (NULL),
   optimizerDataFinalized  (false)//,
   //isNonLinearConstraintParm          (nlc.isNonLinearConstraintParm),
   //varId                   (nlc.varId)
{
   parameterCount = NonLinearConstraintParamCount;
}


//------------------------------------------------------------------------------
//  NonLinearConstraint& operator=(const NonLinearConstraint& nlc)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the NonLinearConstraint command.
 *
 * @param t Command that is replicated here.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
NonLinearConstraint& NonLinearConstraint::operator=(const NonLinearConstraint& nlc)
{
   if (this == &nlc)
      return *this;
    
   GmatCommand::operator=(nlc);
   optimizerName          = nlc.optimizerName;
   //minName                = nlc.minName;
   //minVar                 = NULL;
   //varValue               = nlc.varValue;
   optimizer              = NULL;
   optimizerDataFinalized = false;
   //isNonLinearConstraintParm         = nlc.isNonLinearConstraintParm;
   //varId                  = nlc.varId;

   return *this;
}

//------------------------------------------------------------------------------
//  ~NonLinearConstraint()
//------------------------------------------------------------------------------
/**
 * Destroys the NonLinearConstraint command.  (destructor)
 */
//------------------------------------------------------------------------------
NonLinearConstraint::~NonLinearConstraint()
{
   //delete minVar;  // yes?
}

    


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the NonLinearConstraint.
 *
 * @return clone of the NonLinearConstraint.
 */
//------------------------------------------------------------------------------
GmatBase* NonLinearConstraint::Clone() const
{
   return (new NonLinearConstraint(*this));
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
bool NonLinearConstraint::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   if (type == Gmat::SOLVER)
   {
      if (optimizerName == oldName)
         optimizerName = newName;
   }
   //else if (type == Gmat::PARAMETER)
   //{
   //   if (minName == oldName)
   //      minName = newName;
   //}
   
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
std::string NonLinearConstraint::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < NonLinearConstraintParamCount)
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
Integer NonLinearConstraint::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < NonLinearConstraintParamCount; i++)
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
Gmat::ParameterType NonLinearConstraint::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < NonLinearConstraintParamCount)
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
std::string NonLinearConstraint::GetParameterTypeString(const Integer id) const
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
//Real NonLinearConstraint::GetRealParameter(const Integer id) const
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
//Real NonLinearConstraint::SetRealParameter(const Integer id, const Real value)
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
std::string NonLinearConstraint::GetStringParameter(const Integer id) const
{
   if (id == OPTIMIZER_NAME)
      return optimizerName;
        
   //if (id == NONLINEAR_CONSTRAINT_VARIABLE_NAME)
   //   return minName;
        
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
bool NonLinearConstraint::SetStringParameter(const Integer id, const std::string &value)
{
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARAM
   MessageInterface::ShowMessage
      ("NonLinearConstraint::SetStringParameter() id=%d, value=%s\n", id, value.c_str());
   #endif
   
   if (id == OPTIMIZER_NAME) {
      optimizerName = value;
      return true;
   }

   //if (id == NONLINEAR_CONSTRAINT_VARIABLE_NAME) {
   //   minName = value;
    //  return true;
   //}

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
bool NonLinearConstraint::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
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
   //if (type == Gmat::PARAMETER) 
   //{
   //   if (minName == obj->GetName()) 
   //   {
   //      minVar = (Parameter*)obj;
   //      return true;
   //   }
   //   return false;
   //}

   return GmatCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The NonLinearConstraint command has the following syntax:
 *
 *     NonLinearConstraint myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});
 *     NonLinearConstraint myDC(Sat1.SMA = Var1, {Tolerance = 0.1});
 *     NonLinearConstraint myDC(Sat1.SMA = Arr1(1,1), {Tolerance = 0.1});
 *     NonLinearConstraint myDC(Sat1.SMA = Arr1(I,J), {Tolerance = 0.1});
 *
 * where myDC is a Solver used to NonLinearConstraint a set of variables to achieve the
 * corresponding goals.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
/*
bool NonLinearConstraint::InterpretAction()
{
   /// @todo: Clean up this hack for the NonLinearConstraint::InterpretAction method
   // Sample string:  "NonLinearConstraint myDC(Sat1.SMA = 21545.0, {Tolerance = 0.1});"
   
   // Set starting location to the space following the command string
   Integer loc = generatingString.find("NonLinearConstraint", 0) + 7, end, strend;
   const char *str = generatingString.c_str();
   
   // Skip white space
   while (str[loc] == ' ')
      ++loc;
   
   // Stop at the opening paren
   end = generatingString.find("(", loc);
   
   std::string component = generatingString.substr(loc, end-loc);
   if (component == "")
      throw CommandException("NonLinearConstraint string does specify the optimizer");
   
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

   #if DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("NonLinearConstraint::InterpretAction() minName = \"%s\"\n", minName.c_str());
   #endif

   std::string parmObj, parmType, parmSystem;
   InterpretParameter(minName, parmType, parmObj, parmSystem);

   #if DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("NonLinearConstraint::InterpretAction() parmObj=%s, parmType=%s, "
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
      isNonLinearConstraintParm = true;
   //}
   //else
   //{
   //   value = atof(achieveName.c_str());
   //   isNonLinearConstraintParm = false;
   //}
   
   //#ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
   //   MessageInterface::ShowMessage
   //      ("NonLinearConstraint::InterpretAction() GoalString = '%s'\n", achieveName.c_str());
   //#endif
   
   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage("NonLinearConstraint::InterpretAction() exiting\n");
   #endif
      
   return true;
}
*/


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
bool NonLinearConstraint::ConstructGoal(const char* str)
{
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage("%s%s\"\n",
         "NonLinearConstraint::ConstructGoal() called with string \"", str);
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
   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
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
         isNonLinearConstraintArray = true;
      }
   }
   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("%s%s\", isNonLinearConstraintArray=%d\n", "   achieveName is \"",
          achieveName.c_str(), isNonLinearConstraintArray);
      MessageInterface::ShowMessage
         ("%s%s\"\n", "   Examining the substring \"",
          sstr.substr(start, end-start).c_str());
   #endif

   if (isNonLinearConstraintArray)
   {
      GmatStringUtil::GetArrayIndex(achieveName, achieveArrRowStr,
                                    achieveArrColStr, achieveArrRow,
                                    achieveArrCol, achieveArrName);

      achieveName = achieveArrName;
      
      #if DEBUG_NONLINEAR_CONSTRAINT_PARSE
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

      #if DEBUG_NONLINEAR_CONSTRAINT_PARSE
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
         #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
         MessageInterface::ShowMessage
            ("   \"%s\" is not a parameter\n", achieveName.c_str());
         #endif
      }
   }

   if (GmatStringUtil::ToDouble(achieveName, &rval))
   {
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("   \"%s\" is a number\n", achieveName.c_str());
      #endif
      return false;
   }
   else
   {
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
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
bool NonLinearConstraint::InterpretParameter(const std::string text,
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
      throw CommandException("NonLinearConstraint::InterpretParameter: Unable to "
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
   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage(
         "NonLinearConstraint::InterpretParameter() Built parameter %s for object %s "
         "with CS %s\n", paramType.c_str(), paramObj.c_str(), parmSystem.c_str());
   #endif
      
   return true;
}
*/

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the NonLinearConstraint command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool NonLinearConstraint::Initialize()
{
   #if DEBUG_NONLINEAR_CONSTRAINT_INIT
   MessageInterface::ShowMessage
      ("NonLinearConstraint::Initialize() entered. optimizer=%p, minVar=%p\n", 
      optimizer, minVar);
   #endif
   
   bool retval = GmatCommand::Initialize();

   if (optimizer == NULL)
      throw CommandException(
         "Optimizer not initialized for NonLinearConstraint command\n  \""
         + generatingString + "\"\n");

   //Integer id = optimizer->GetParameterID("Goals");  // no clue
   //optimizer->SetStringParameter(id, goalName);

   // find goalName
   //GmatBase *obj = (*objectMap)[minName];
   
   //if (obj == NULL) {
   //   std::string errorstr = "Could not find variable parameter ";
   //   errorstr += minName;
   //   throw CommandException(errorstr);
   //}

   //minVar = (Parameter*)obj;
   
   //goalObject = obj;
   //parmId = id;

   // find achieveParm
   /*
   if (isNonLinearConstraintParm)
   {
      #if DEBUG_NONLINEAR_CONSTRAINT_INIT
      MessageInterface::ShowMessage
         ("NonLinearConstraint::Initialize() Find achieveParm=%s from objectMap\n",
          achieveName.c_str());
      #endif
      if (objectMap->find(achieveName) != objectMap->end())
         achieveParm = (Parameter*)((*objectMap)[achieveName]);

      if (isNonLinearConstraintArray)
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
   
   #if DEBUG_NONLINEAR_CONSTRAINT_INIT
   MessageInterface::ShowMessage
      ("NonLinearConstraint::Initialize() exiting. optimizer=%p, minVar=%p\n", 
      optimizer, minVar);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * NonLinearConstraint the variables defined for this targeting loop.
 *
 * This method (will eventually) feeds data to the targeter state machine in 
 * order to determine the variable values needed to achieve the user specified
 * goals.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool NonLinearConstraint::Execute()
{
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
   MessageInterface::ShowMessage
      ("NonLinearConstraint::Execute() optimizerDataFinalized=%d\n, optimizer=%s, minVar=%p\n", 
       optimizerDataFinalized, optimizer, minVar);
   MessageInterface::ShowMessage
      ("   minName=%s\n", minName.c_str());
   //if (achieveParm)
   //   MessageInterface::ShowMessage("   achieveParm=%s\n", achieveParm->GetName().c_str());
   //if (minVar)
   //   MessageInterface::ShowMessage("   minVar=%s\n", minVar->GetName().c_str());
   //if (goalObject)
   //   MessageInterface::ShowMessage("   goalObject=%s\n", goalObject->GetName().c_str());
   #endif
   
   bool retval = true;
   if (!optimizerDataFinalized) {
      // Tell the optimizer about the objective function and gradient values??????
      Real conData[1];
      conData[0] = -99.0;  // objective function value here .........
      //Integer varId = optimizer->SetSolverResults(conData, conName, "Objective");
      //conData[0] = ??; // loop over eq and ineq constraits here ...????
      //varId = optimizer->SetSolverResults(conData, conName);

      optimizerDataFinalized = true;
      return retval;
   }
   
   //Real val = -999.999;
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

         #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
         MessageInterface::ShowMessage("   row=%d, col=%d\n", row, col);
         #endif
         
         if (row >= 0 || col >= 0)
            val = achieveParm->EvaluateRmatrix().GetElement(row, col);
         else
            throw CommandException("Invalid row and column in NonLinearConstraint\n");
      }
      
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
      MessageInterface::ShowMessage("   Floating target: val = %lf\n", val);
      #endif
      
      targeter->UpdateSolverGoal(goalId, val);
   }
   */

   // Evaluate variable and pass it to the optimizer
   //if (minVar != NULL)
   //{
   //   val = minVar->EvaluateReal();
   //   optimizer->SetResultValue(varId, val);
   //   #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
   //      MessageInterface::ShowMessage
   //         ("   minVar=%s, %p\n", minVar->GetTypeName().c_str(), minVar);
   //      MessageInterface::ShowMessage("   Parameter target: %s val = %lf\n",
   //         minVar->GetTypeName().c_str(), val);
   //   #endif
   //}
   //else 
   //{
   //   val = -999.999;
   //   //val = goalObject->GetRealParameter(parmId);  // again,no clue
   //   optimizer->SetResultValue(varId, val);
   //   #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
   //      MessageInterface::ShowMessage("   Object target: val = %lf\n", val);
   //   #endif
   //}

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
 * @return The script line that, when interpreted, defines this NonLinearConstraint command.
 */
//------------------------------------------------------------------------------
const std::string& NonLinearConstraint::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   std::stringstream details;

   std::string gen = prefix + "NonLinearConstraint " + optimizerName + "(" + conName;

   details << " = " << conValue;
   // Then call the base class method
   gen+= details.str();
   generatingString = gen + ");";
   return GmatCommand::GetGeneratingString();
}


