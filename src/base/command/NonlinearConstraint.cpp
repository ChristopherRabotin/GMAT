//$Header$
//------------------------------------------------------------------------------
//                                 NonlinearConstraint
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
 * Definition for the NonlinearConstraint command class
 */
//------------------------------------------------------------------------------


#include "NonlinearConstraint.hpp"
/// @todo Rework command so it doesn't need the Moderator!!!
#include "Moderator.hpp" 
#include "StringUtil.hpp"  // for ToReal()

//#define DEBUG_NONLINEAR_CONSTRAINT_PARSE 1
//#define DEBUG_NONLINEAR_CONSTRAINT_INIT 1
//#define DEBUG_NONLINEAR_CONSTRAINT_EXEC 1
//#define DEBUG_NLC_VALUES
//#define DEBUG_NONLINEAR_CONSTRAINT_PARAM


//---------------------------------
// static data
//---------------------------------
const std::string
   NonlinearConstraint::PARAMETER_TEXT[NonlinearConstraintParamCount - GmatCommandParamCount] =
   {
      "OptimizerName",
      "ConstraintVariableName",
      "Operator",
      "ConstraintValue",
      "Tolerance",
   };
   
const Gmat::ParameterType
   NonlinearConstraint::PARAMETER_TYPE[NonlinearConstraintParamCount - GmatCommandParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::REAL_TYPE,
   };


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  NonlinearConstraint()
//------------------------------------------------------------------------------
/**
 * Creates an NonlinearConstraint command.  (default constructor)
 */
//------------------------------------------------------------------------------
NonlinearConstraint::NonlinearConstraint() :
   GmatCommand             ("NonlinearConstraint"),
   optimizerName           (""),
   optimizer               (NULL),
   constraintName          (""),
   constraint              (NULL),
   constraintValue         (-999.99),
   nlcParmName             (""),
   nlcParm                 (NULL),
   nlcArrName              (""),
   nlcArrRowStr            (""),
   nlcArrColStr            (""),
   nlcArrRow               (0),
   nlcArrCol               (0),
   nlcArrRowParm           (NULL),
   nlcArrColParm           (NULL),
   isInequality            (false),
   isIneqString            ("EqConstraint"),
   desiredValue            (0.0),
   op                      (EQUAL),
   tolerance               (0.0),
   optimizerDataFinalized  (false),
   constraintId            (-1),
   isNlcParm               (false),
   isNlcArray              (false),
   interpreted             (false)
{
   parameterCount = NonlinearConstraintParamCount;
}


//------------------------------------------------------------------------------
//  NonlinearConstraint(const NonlinearConstraint& nlc)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a NonlinearConstraint command.  (Copy constructor)
 *
 * @param t Command that is replicated here.
 */
//------------------------------------------------------------------------------
NonlinearConstraint::NonlinearConstraint(const NonlinearConstraint& nlc) :
   GmatCommand             (nlc),
   optimizerName           (nlc.optimizerName),
   optimizer               (NULL),
   constraintName          (nlc.constraintName),
   constraint              (NULL),
   constraintValue         (nlc.constraintValue),
   nlcParmName             (nlc.nlcParmName),
   nlcParm                 (NULL),
   nlcArrName              (nlc.nlcArrName),
   nlcArrRowStr            (nlc.nlcArrRowStr),
   nlcArrColStr            (nlc.nlcArrColStr),
   nlcArrRow               (nlc.nlcArrRow),
   nlcArrCol               (nlc.nlcArrCol),
   nlcArrRowParm           (NULL),
   nlcArrColParm           (NULL),
   isInequality            (nlc.isInequality),
   isIneqString            (nlc.isIneqString),
   desiredValue            (nlc.desiredValue),
   op                      (nlc.op),
   tolerance               (nlc.tolerance),
   optimizerDataFinalized  (false),
   constraintId            (-1),
   isNlcParm               (nlc.isNlcParm),   // right?
   isNlcArray              (nlc.isNlcArray),  // right?
   interpreted             (false)
{
   parameterCount = NonlinearConstraintParamCount;
}


//------------------------------------------------------------------------------
//  NonlinearConstraint& operator=(const NonlinearConstraint& nlc)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the NonlinearConstraint command.
 *
 * @param t Command that is replicated here.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
NonlinearConstraint& NonlinearConstraint::operator=(const NonlinearConstraint& nlc)
{
   if (this == &nlc)
      return *this;
    
   GmatCommand::operator=(nlc);
   optimizerName          = nlc.optimizerName;
   optimizer              = NULL;
   constraintName         = nlc.constraintName;
   constraint             = NULL;
   constraintValue        = nlc.constraintValue;
   nlcParmName            = nlc.nlcParmName;
   nlcParm                = NULL;
   nlcArrName             = nlc.nlcArrName;
   nlcArrRowStr           = nlc.nlcArrRowStr;
   nlcArrColStr           = nlc.nlcArrColStr;
   nlcArrRow              = nlc.nlcArrRow;
   nlcArrCol              = nlc.nlcArrCol;
   nlcArrRowParm          = NULL;
   nlcArrColParm          = NULL;
   isInequality           = nlc.isInequality;
   isIneqString           = nlc.isIneqString;
   desiredValue           = nlc.desiredValue;
   op                     = nlc.op;
   tolerance              = nlc.tolerance;
   optimizerDataFinalized = false;
   constraintId           = -1;
   isNlcParm              = nlc.isNlcParm;  // right?
   isNlcArray             = nlc.isNlcArray; // right?
   interpreted            = false;

   return *this;
}

//------------------------------------------------------------------------------
//  ~NonlinearConstraint()
//------------------------------------------------------------------------------
/**
 * Destroys the NonlinearConstraint command.  (destructor)
 */
//------------------------------------------------------------------------------
NonlinearConstraint::~NonlinearConstraint()
{
   //delete constraint;  // yes?
}

    


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the NonlinearConstraint.
 *
 * @return clone of the NonlinearConstraint.
 */
//------------------------------------------------------------------------------
GmatBase* NonlinearConstraint::Clone() const
{
   return (new NonlinearConstraint(*this));
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
bool NonlinearConstraint::RenameRefObject(const Gmat::ObjectType type,
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
      if (constraintName == oldName)
         constraintName = newName;
      if (nlcParmName == oldName)
         nlcParmName = newName;
      if (nlcArrName == oldName)
         nlcArrName = newName;
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
std::string NonlinearConstraint::GetParameterText(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < NonlinearConstraintParamCount)
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
Integer NonlinearConstraint::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatCommandParamCount; i < NonlinearConstraintParamCount; i++)
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
Gmat::ParameterType NonlinearConstraint::GetParameterType(const Integer id) const
{
   if (id >= GmatCommandParamCount && id < NonlinearConstraintParamCount)
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
std::string NonlinearConstraint::GetParameterTypeString(const Integer id) const
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
Real NonlinearConstraint::GetRealParameter(const Integer id) const
{
   //if (id == CONSTRAINT_VALUE)
   //   return constraintValue;
   if (id == TOLERANCE)
      return tolerance;
    
   return GmatCommand::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *h
 * @param id    ID for the parameter whose value to change.
 * @param value value for the parameter.
 *
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real NonlinearConstraint::SetRealParameter(const Integer id, const Real value)
{
   if (id == TOLERANCE)
   {
      tolerance = value;
      return tolerance;
   }
   //if (id == CONSTRAINT_VALUE)
   //{
   //   constraintValue = value;
   //   return constraintValue;
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
std::string NonlinearConstraint::GetStringParameter(const Integer id) const
{
   if (id == OPTIMIZER_NAME)
      return optimizerName;
        
   if (id == CONSTRAINT_VARIABLE_NAME)
      return constraintName;
      
   if (id == OPERATOR)
   {
      if      (op == LESS_THAN_OR_EQUAL)    return "<=";
      else if (op == GREATER_THAN_OR_EQUAL) return ">=";
      else                                  return "=";
   }
      
   if (id == CONSTRAINT_VALUE)
      return nlcParmName;
        
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
bool NonlinearConstraint::SetStringParameter(const Integer id, 
                                             const std::string &value)
{
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARAM
   MessageInterface::ShowMessage
      ("NonlinearConstraint::SetStringParameter() id=%d, value=%s\n", 
         id, value.c_str());
   #endif
   
   if (id == OPTIMIZER_NAME) 
   {
      optimizerName = value;
      interpreted   = false;
      return true;
   }

   if (id == CONSTRAINT_VARIABLE_NAME) 
   {
      constraintName = value;
      interpreted    = false;
      return true;
   }
   
   if (id == OPERATOR)
   {
      if (value == "<=")      op = LESS_THAN_OR_EQUAL;
      else if (value == ">=") op = GREATER_THAN_OR_EQUAL;
      else if (value == "=")  op = EQUAL;
      else
         throw CommandException("NonlinearConstraint: invalid operator: "
                                + value + "\n");
      interpreted   = false;
      return true;
   }
   
   if (id == CONSTRAINT_VALUE)
   {
      nlcParmName = value;
      Real realValue;
      if (ConstructConstraint(value.c_str()))
      {
         // It's a parameter; just set dummy value here -- gets reset on execution
         realValue = 54321.0;
         isNlcParm = true;
      }
      else
      {
         //realValue = atof(nlcParmName.c_str());
         bool isNum = GmatStringUtil::ToReal(nlcParmName.c_str(), &realValue);
         if (!isNum)
         {
            std::string errMsg = "The value of \"" + nlcParmName + "\" for field \"" + 
               PARAMETER_TEXT[CONSTRAINT_VALUE - GmatCommandParamCount] + "\" on object \"" 
               + instanceName +
               "\" is not an allowed value.  The allowed values are: " +
               " [Real number]."; 
            throw CommandException(errMsg);
         }
         isNlcParm = false;
      }
      
      #ifdef DEBUG_ACHIEVE_PARAM
         MessageInterface::ShowMessage
            ("Achieve::SetStringParameter() GoalString = '%s', realValue=%f\n",
             nlcName.c_str(), realValue);
      #endif
         
      constraintValue = realValue;
      interpreted     = false;
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
bool NonlinearConstraint::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
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
   /* don't want to be able to set these
   if (type == Gmat::PARAMETER) 
   {
      if (constraintName == obj->GetName()) 
      {
         constraint = (Parameter*)obj;
         return true;
      }

      if (nlcParmName == obj->GetName()) 
      {
         nlcParm = (Parameter*)obj;
         return true;
      }
      return false;
   }
   */
   return GmatCommand::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool InterpretAction()
//------------------------------------------------------------------------------
/**
 * Parses the command string and builds the corresponding command structures.
 *
 * The NonlinearConstraint command has the following syntax:
 *
 *     NonlinearConstraint myOpt(Sat1.SMA = 21545.0, {Tolerance = 0.1});
 *     NonlinearConstraint myOpt(Sat1.SMA = Var1, {Tolerance = 0.1});
 *     NonlinearConstraint myOpt(Sat1.SMA = Arr1(1,1), {Tolerance = 0.1});
 *     NonlinearConstraint myOpt(Sat1.SMA = Arr1(I,J), {Tolerance = 0.1});
 *
 * where myOpt is a Solver used to optimize a set of variables to the
 * corresponding constraints.  This method breaks the script line into the 
 * corresponding pieces, and stores the name of the Solver so it can be set to
 * point to the correct object during initialization.
 */
//------------------------------------------------------------------------------
bool NonlinearConstraint::InterpretAction()
{
   /// @todo: Clean up this hack for the NonlinearConstraint::InterpretAction method
   // Sample string:  "NonlinearConstraint myOpt(Sat1.SMA = 21545.0, {Tolerance = 0.1});"
   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_INIT
      MessageInterface::ShowMessage("NLC:InterpretAction: generatingString = %s\n",
      generatingString.c_str());
   #endif
   // Set starting location to the space following the command string
   Integer loc = generatingString.find("NonlinearConstraint", 0) + 19, end, strend;
   const char *str = generatingString.c_str();
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_INIT
      MessageInterface::ShowMessage("NLC:InterpretAction: str = %s\n",
      str);
   #endif
   
   // Skip white space
   while (str[loc] == ' ')
      ++loc;
   
   // Stop at the opening paren
   end = generatingString.find("(", loc);
   
   std::string component = generatingString.substr(loc, end-loc);
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_INIT
      MessageInterface::ShowMessage("NLC:InterpretAction: setting optimizer to = %s\n",
      component.c_str());
   #endif
   if (component == "")
      throw CommandException("NonlinearConstraint string does specify the optimizer");
   
   SetStringParameter(OPTIMIZER_NAME, component);
   
   // Find the constraint
   loc = end + 1;
   // Skip white space
   while (str[loc] == ' ')
      ++loc;
   // Find the operator
   Integer goBack = 1;
   if ((end = generatingString.find(">=", loc)) != (Integer) generatingString.npos)
   {
      op = GREATER_THAN_OR_EQUAL;
      isInequality = true;
      isIneqString = "IneqConstraint";
      goBack       = 2;
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_INIT
         MessageInterface::ShowMessage("NLC:InterpretAction: greater_than_or_equal\n");
      #endif
   }
   else if ((end = generatingString.find("<=", loc)) != (Integer) generatingString.npos)
   {
      op = LESS_THAN_OR_EQUAL;
      isInequality = true;
      isIneqString = "IneqConstraint";
      goBack       = 2;
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_INIT
         MessageInterface::ShowMessage("NLC:InterpretAction: less_than_or_equal\n");
      #endif
   }
   else
   {
      end = generatingString.find("=", loc);
      op = EQUAL;
      isInequality = false;
      isIneqString = "EqConstraint";
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_INIT
         MessageInterface::ShowMessage("NLC:InterpretAction: equal\n");
      #endif
   }
   //strend = end-1;
   strend = end - goBack;
   // Drop trailing white space
   while (str[strend] == ' ')
      --strend;
    
   component = generatingString.substr(loc, strend-loc+1);
   constraintName = component;

   // Get an instance if this is a Parameter
   Moderator *mod = Moderator::Instance();

   #if DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("NonlinearConstraint::InterpretAction() constraintName = \"%s\"\n", 
         constraintName.c_str());
   #endif

   std::string parmObj, parmType, parmSystem;
   InterpretParameter(constraintName, parmType, parmObj, parmSystem);

   #if DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("NonlinearConstraint::InterpretAction() parmObj=%s, parmType=%s, "
          "parmSystem = \"%s\"\n", parmObj.c_str(),
          parmType.c_str(), parmSystem.c_str());
   #endif

   constraint = mod->CreateParameter(parmType, constraintName, parmObj, parmSystem);

   if (!constraint)
      throw CommandException("Unable to create Parameter " + constraintName);
         

   // Find the value
   //loc = end + 1;
   loc = end + goBack;
   
   // Constraint can be either a parameter or array or a number;
   // ConstructConstraint determines this.
   Real value;
   if (ConstructConstraint(&str[loc]))
   {
      // It's a parameter; just set dummy value here -- gets reset on execution
      value = 54321.0;
      isNlcParm = true;
   }
   else
   {
      //value = atof(nlcParmName.c_str());
      bool isNum = GmatStringUtil::ToReal(nlcParmName.c_str(), &value);
      if (!isNum)
      {
         std::string errMsg = "The value of \"" + nlcParmName + "\" for field \"" + 
            PARAMETER_TEXT[CONSTRAINT_VALUE - GmatCommandParamCount] + "\" on object \"" 
            + instanceName +
            "\" is not an allowed value.  The allowed values are: " +
            " [Real number]."; 
         throw CommandException(errMsg);
      }
      isNlcParm = false;
   }
   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("NonlinearConstraint::InterpretAction() parameter (rhs) = '%s'\n", 
         nlcParmName.c_str());
   #endif
   
   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage("NonlinearConstraint::InterpretAction() exiting\n");
   #endif
   
   interpreted = true;
  
   return true;
}



//------------------------------------------------------------------------------
// bool ConstructConstraint(const char* str)
//------------------------------------------------------------------------------
/**
 * Builds constraints -- either as a parameter or as a numeric value, 
 * depending on the script contents.
 * 
 * @return true on success, false on failure.  ??
 * @return true if it is a parameter, false otherwise.
 */
//------------------------------------------------------------------------------
bool NonlinearConstraint::ConstructConstraint(const char* str)
{
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage("%s%s\"\n",
         "NonlinearConstraint::ConstructConstraint() called with string \"", str);
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
   while ((str[end] != ' ') && (str[end] != ',') && (str[end] != '{') &&
          (str[end] != ')'))
   {
      if ((str[end] == '.') && (dot == start))
         dot = end;
      ++end;
   }
   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("   start=%d, dot=%d, end=%d\n", start, dot, end);
   #endif

   std::string sstr = str;
   nlcParmName = sstr.substr(start, end-start);
   
   // Search for 2nd comma for array index
   Integer comma;
   if (str[end] == ',')
   {
      comma = sstr.find(',', end+1);
      if ((UnsignedInt)comma != sstr.npos)
      {
         nlcParmName = sstr.substr(start, comma-start);
         isNlcArray = true;
      }
   }
   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("%s%s\", isNlcArray=%d\n", "   nlcParmName is \"",
          nlcParmName.c_str(), isNlcArray);
      MessageInterface::ShowMessage
         ("%s%s\"\n", "   Examining the substring \"",
          sstr.substr(start, end-start).c_str());
   #endif

   if (isNlcArray)
   {
      GmatStringUtil::GetArrayIndex(nlcParmName, nlcArrRowStr,
                                    nlcArrColStr, nlcArrRow,
                                    nlcArrCol, nlcArrName);

      nlcParmName = nlcArrName;
      
      #if DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("   nlcArrRowStr=%s, nlcArrColStr=%s, nlcArrRow=%d, "
          "nlcArrCol=%d, nlcArrName=%s\n", nlcArrRowStr.c_str(),
          nlcArrColStr.c_str(), nlcArrRow, nlcArrCol,
          nlcArrName.c_str());
      #endif
      
      // if variable index is used, make sure the variable is created
      if (nlcArrRow == -1)
         if (mod->GetParameter(nlcArrRowStr) == NULL)
            throw CommandException("Cannot find array row index variable: " +
                                   nlcArrRowStr + "\n");
      
      if (nlcArrCol == -1)   // this is wrong???
         if (mod->GetParameter(nlcArrColStr) == NULL)
            throw CommandException("Cannot find array column index variable: " +
                                   nlcArrColStr + "\n");
      
      return true;
   }
   else if ((dot > start) && (dot < end)) // Could be a system parameter
   {
      
      // try if Tolerance is missing
      UnsignedInt index = nlcParmName.find(")");
      if (index != nlcParmName.npos)
         nlcParmName = nlcParmName.substr(0, index);
      
      std::string parmType, parmObj, parmSystem;
      InterpretParameter(nlcParmName, parmType, parmObj, parmSystem);

      #if DEBUG_NONLINEAR_CONSTRAINT_PARSE
         MessageInterface::ShowMessage
            ("   parmObj=%s, parmType=%s, parmSystem = \"%s\"\n",
             parmObj.c_str(), parmType.c_str(), parmSystem.c_str());
      #endif

         //Moderator *mod = Moderator::Instance();

      if (mod->IsParameter(parmType))
      {
         nlcParm =
            mod->CreateParameter(parmType, nlcParmName, parmObj, parmSystem);
        
         if (!nlcParm)
            throw CommandException("Unable to create parameter " + nlcParmName);

         return true;
      }
      else
      {
         #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
         MessageInterface::ShowMessage
            ("   \"%s\" is not a parameter\n", nlcParmName.c_str());
         #endif
      }
   }

   if (GmatStringUtil::ToReal(nlcParmName, &rval))
   {
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("   \"%s\" is a number\n", nlcParmName.c_str());
      #endif
      return false;
   }
   else
   {
      if (nlcParmName.size() == 0)
      {
         #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
            MessageInterface::ShowMessage
               ("NLC::Error!  Cannot parse \"%s\"\n", nlcParmName.c_str());
         #endif
         throw CommandException("NonlinearConstraint:: constraint value is empty\n");
      }
         
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage
         ("   \"%s\" is a variable\n", nlcParmName.c_str());
      #endif
      return true;
   }
   
   return false;
}


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
bool NonlinearConstraint::InterpretParameter(const std::string text,
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
      //throw CommandException("NonlinearConstraint::InterpretParameter: Unable to "
      //         "interpret parameter object in the string " +
       //        text);
   {
      // this must be a variable
      paramObj    = "";
      paramType   = "Variable";
      parmSystem  = "";
      return true;
   }
   
   paramObj = text.substr(start, dotLoc - start);
   start = dotLoc + 1;
   dotLoc = text.find(".", start);
   if (dotLoc != (Integer)std::string::npos) 
   {
      parmSystem = text.substr(start, dotLoc - start);
      start = dotLoc + 1;
   }
   else 
   {
      parmSystem = "";
   }
   
   paramType = text.substr(start);
   
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_PARSE
      MessageInterface::ShowMessage(
         "NonlinearConstraint::InterpretParameter() Built parameter %s for object %s "
         "with CS %s\n", paramType.c_str(), paramObj.c_str(), parmSystem.c_str());
   #endif

   return true;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the NonlinearConstraint command.
 *
 * @return true if the GmatCommand is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool NonlinearConstraint::Initialize()
{
   #if DEBUG_NONLINEAR_CONSTRAINT_INIT
   MessageInterface::ShowMessage
      ("NonlinearConstraint::Initialize() entered. optimizer=%p, constraint=%p\n", 
      optimizer, constraint);
   #endif
   
   if (!interpreted)
      if (!InterpretAction())
         throw CommandException(
            "NonlinearConstraint: error interpreting input data\n");
   
   bool retval = GmatCommand::Initialize();

   if (optimizer == NULL)
      throw CommandException(
         "Optimizer not initialized for NonlinearConstraint command\n  \""
         + generatingString + "\"\n");

   //Integer id = optimizer->GetParameterID("Goals");  // no clue
   //optimizer->SetStringParameter(id, goalName);

   // find constraintName
   GmatBase *obj = (*objectMap)[constraintName];
   
   if (obj == NULL) 
   {
      std::string errorstr = "Could not find variable parameter ";
      errorstr += constraintName;
      throw CommandException(errorstr);
   }

   constraint = (Parameter*)obj;
   
   constraintObject = obj;
   //parmId = id;

   // find right-hand-side parameter (or Real value)


   if (isNlcParm)
   {
      #if DEBUG_NONLINEAR_CONSTRAINT_INIT
      MessageInterface::ShowMessage
         ("NonlinearConstraint::Initialize() Find right-hand-side parameter=%s from objectMap\n",
          nlcParmName.c_str());
      #endif
      if (objectMap->find(nlcParmName) != objectMap->end())
         nlcParm = (Parameter*)((*objectMap)[nlcParmName]);

      if (isNlcArray)
      {
         // if variable index is used, get variable from the objectMap
         if (nlcArrRow == -1)
            if (objectMap->find(nlcArrRowStr) != objectMap->end())
               nlcArrRowParm = (Parameter*)((*objectMap)[nlcArrRowStr]);
            else
               throw CommandException("Cannot find array row index variable\n");

         if (nlcArrCol == -1)
            if (objectMap->find(nlcArrColStr) != objectMap->end())
               nlcArrColParm = (Parameter*)((*objectMap)[nlcArrColStr]);
            else
               throw CommandException("Cannot find array column index variable\n");
      }
   }

   // The optimizer cannot be finalized until all of the loop is initialized
   optimizerDataFinalized = false;

   #if DEBUG_NONLINEAR_CONSTRAINT_INIT
   MessageInterface::ShowMessage
      ("NonlinearConstraint::Initialize() exiting. optimizer=%p, constraint=%p\n", 
      optimizer, constraint);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * NonlinearConstraint the variables defined for this targeting loop.
 *
 * This method (will eventually) feeds data to the targeter state machine in 
 * order to determine the variable values needed to achieve the user specified
 * goals.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool NonlinearConstraint::Execute()
{
   #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
   MessageInterface::ShowMessage
      ("NonlinearConstraint::Execute() optimizerDataFinalized=%d\n, optimizer=%s, constraint=%p\n", 
       optimizerDataFinalized, optimizer, constraint);
   MessageInterface::ShowMessage
      ("   constraintName=%s\n", constraintName.c_str());
   //if (nlcParm)
   //   MessageInterface::ShowMessage("   nlcParm=%s\n", nlcParm->GetName().c_str());
   //if (minVar)
   //   MessageInterface::ShowMessage("   minVar=%s\n", minVar->GetName().c_str());
   //if (goalObject)
   //   MessageInterface::ShowMessage("   goalObject=%s\n", goalObject->GetName().c_str());
   #endif
   
   bool retval = true;
   if (!optimizerDataFinalized) 
   {
      // Tell the optimizer about the constraint
      Real conData[1];
      conData[0] = 54321.0;  // objective function value here .........
      constraintId = optimizer->SetSolverResults(conData, constraintName, 
                     isIneqString);

      optimizerDataFinalized = true;
      return retval;
   }
   
   desiredValue = -999.999;
   // Evaluate the (in)equality constraint value and set it on the optimizer
   if (!isNlcParm)
   {
      //desiredValue = atof(nlcParmName.c_str());  
      bool isNum = GmatStringUtil::ToReal(nlcParmName.c_str(), &desiredValue);
      if (!isNum)
      {
         std::string errMsg = "The value of \"" + nlcParmName + "\" for field \"" + 
            PARAMETER_TEXT[CONSTRAINT_VALUE - GmatCommandParamCount] + "\" on object \"" 
            + instanceName +
            "\" is not an allowed value.  The allowed values are: " +
            " [Real number]."; 
         throw CommandException(errMsg);
      }
   }
   else if (nlcParm != NULL)
   {
      if (nlcParm->GetReturnType() == Gmat::REAL_TYPE)
      {
         desiredValue = nlcParm->EvaluateReal();
      }
      // what if it's a vector type??? - need to add that here?
      else if (nlcParm->GetReturnType() == Gmat::RMATRIX_TYPE)
      {
         Integer row = nlcArrRow;
         Integer col = nlcArrCol;
         
         if (nlcArrRow == -1)
            row = (Integer)(nlcArrRowParm->EvaluateReal()) - 1; // index starts at 0

         if (nlcArrCol == -1)
            col = (Integer)(nlcArrColParm->EvaluateReal()) - 1; // index starts at 0

         #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
         MessageInterface::ShowMessage("   row=%d, col=%d\n", row, col);
         #endif
         
         if (row >= 0 || col >= 0)
            desiredValue = nlcParm->EvaluateRmatrix().GetElement(row, col);
         else
            throw CommandException("Invalid row and column in NonlinearConstraint\n");
      }
      
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
      MessageInterface::ShowMessage("   Constraint: desiredValue = %lf\n", 
      desiredValue);
      #endif
      
      //targeter->UpdateSolverGoal(goalId, val);
   }
   else
   {
      throw CommandException(
         "NonlinearConstraint: error computing constraint desired value\n");
   }

   constraintValue = -999.999;
   #ifdef DEBUG_NLC_VALUES
      MessageInterface::ShowMessage("NLC:Execute - desiredValue = %.12f\n",
         desiredValue);
   #endif
   
   // Evaluate variable and pass it to the optimizer
   if (constraint != NULL)
   {
      constraintValue = constraint->EvaluateReal();
      #ifdef DEBUG_NLC_VALUES
         MessageInterface::ShowMessage("NLC:Execute - (1) constraintValue = %.12f\n",
            constraintValue);
      #endif
      switch (op)
      {
         case EQUAL:
         case LESS_THAN_OR_EQUAL:
            constraintValue = constraintValue - desiredValue;
            break;
         case GREATER_THAN_OR_EQUAL:
            constraintValue = desiredValue - constraintValue;
            break;
         default:
            break;
      }
      #ifdef DEBUG_NLC_VALUES
         MessageInterface::ShowMessage("NLC:Execute - (2) constraintValue now = %.12f\n",
            constraintValue);
      #endif
      optimizer->SetResultValue(constraintId, constraintValue, isIneqString);
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
         MessageInterface::ShowMessage
            ("   constraint=%s, %p\n", constraint->GetTypeName().c_str(), constraint);
         MessageInterface::ShowMessage("   Parameter target: %s constraintValue = %lf\n",
            constraint->GetTypeName().c_str(), constraintValue);
      #endif
   }
   else 
   {
      // isn't this an error???
      constraintValue = -999.99;  // what to do here?
      //constraintValue = constraintObject->GetRealParameter(parmId);  // again,no clue
      optimizer->SetResultValue(constraintId, constraintValue, isIneqString);
      #ifdef DEBUG_NONLINEAR_CONSTRAINT_EXEC
         MessageInterface::ShowMessage("   Object target: constraintValue = %lf\n", 
         constraintValue);
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
 * @return The script line that, when interpreted, defines this NonlinearConstraint command.
 */
//------------------------------------------------------------------------------
const std::string& NonlinearConstraint::GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName)
{
   // Build the local string
   //std::stringstream tol;  // may need tolerance in the future
   //tol << tolerance;
   std::string opString;
   if (op == EQUAL)                   opString = "=";
   else if (op == LESS_THAN_OR_EQUAL) opString = "<=";
   else                               opString = ">=";
   //std::string gen = prefix + "NonlinearConstraint " + optimizerName + "(" + 
   //                  constraintName +  opString + nlcParmName + 
   //                  ", {Tolerance = " + tol.str() + "}";
   std::string gen = prefix + "NonlinearConstraint " + optimizerName + "(" + 
                     constraintName +  opString + nlcParmName; 
 
   generatingString = gen + ");";
   // Then call the base class method
   return GmatCommand::GetGeneratingString(mode, prefix, useName);
}


