//$Header$
//------------------------------------------------------------------------------
//                                ConditionalBranch
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Wendy Shoan/GSFC
// Created: 2004/09/27
//
/**
 * Definition for the ConditionalBranch command class
 */
//------------------------------------------------------------------------------

#include <sstream>
#include <ctype.h>      // for isalpha
#include "gmatdefs.hpp"
#include "ConditionalBranch.hpp"
#include "Parameter.hpp"


//---------------------------------
// static data
//---------------------------------
const std::string
ConditionalBranch::PARAMETER_TEXT[ConditionalBranchParamCount - BranchCommandParamCount] =
{
   "NumberOfConditions",
   "NumberOfLogicalOperators",
   "LeftHandStrings",
   "OperatorStrings",
   "RightHandStrings",
   "LogicalOperators",
   "NumberOfRefParams",
   //"RefParameterNames",
};

const Gmat::ParameterType
ConditionalBranch::PARAMETER_TYPE[ConditionalBranchParamCount - BranchCommandParamCount] =
{
   Gmat::INTEGER_TYPE,     
   Gmat::INTEGER_TYPE, 
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::STRINGARRAY_TYPE,
   Gmat::INTEGER_TYPE,
   //Gmat::STRINGARRAY_TYPE,
};

const std::string
ConditionalBranch::OPTYPE_TEXT[NumberOfOperators] =
{
   "==",
   "~=",
   ">",
   "<",
   ">=",
   "<="
};

const std::string
ConditionalBranch::LOGICAL_OPTYPE_TEXT[NumberOfLogicalOperators] =
{
   "&",
   "|"
};

//------------------------------------------------------------------------------
//  ConditionalBranch(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Creates a ConditionalBranch command.  (constructor)
 */
//------------------------------------------------------------------------------
ConditionalBranch::ConditionalBranch(const std::string &typeStr) :
BranchCommand      (typeStr),
numberOfConditions (0),
numberOfLogicalOps (0)
{
}


//------------------------------------------------------------------------------
//  ConditionalBranch(const ConditionalBranch &cb)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates a ConditionalBranch command.  (Copy constructor)
 */
//------------------------------------------------------------------------------
ConditionalBranch::ConditionalBranch(const ConditionalBranch &cb) :
BranchCommand      (cb),
numberOfConditions (cb.numberOfConditions),
numberOfLogicalOps (cb.numberOfLogicalOps)
{
   Integer i = 0;
   for (i=0; i < numberOfConditions; i++)
   {
      lhsList.push_back((cb.lhsList).at(i));
      opStrings.push_back((cb.opStrings).at(i));
      opList.push_back((cb.opList).at(i));
      rhsList.push_back((cb.rhsList).at(i));
   }
   for (i=0; i < numberOfLogicalOps; i++)
   {
      logicalOpStrings.push_back((cb.logicalOpStrings).at(i));
      logicalOpList.push_back((cb.logicalOpList).at(i));
   }
   
   Integer sz = (Integer) params.size();
   for (i=0; i < sz; i++)
   {
      params.push_back((Parameter*)((cb.params).at(i))->Clone());
   }
   
   initialized = false;
}


//------------------------------------------------------------------------------
//  ConditionalBranch& operator=(const ConditionalBranch &cb)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ConditionalBranch command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ConditionalBranch& ConditionalBranch::operator=(const ConditionalBranch &cb)
{
   if (this == &cb)
      return *this;
   BranchCommand::operator=(cb);
   numberOfConditions = cb.numberOfConditions;
   numberOfLogicalOps = cb.numberOfLogicalOps;
   lhsList.clear();
   opList.clear();
   rhsList.clear();
   Integer i = 0;
   for (i=0; i < numberOfConditions; i++)
   {
      lhsList.push_back((cb.lhsList).at(i));
      opStrings.push_back((cb.opStrings).at(i));
      opList.push_back((cb.opList).at(i));
      rhsList.push_back((cb.rhsList).at(i));
   }
   logicalOpList.clear();
   for (i=0; i < numberOfLogicalOps; i++)
   {
      logicalOpStrings.push_back((cb.logicalOpStrings).at(i));
      logicalOpList.push_back((cb.logicalOpList).at(i));
   }

   params.clear();
   Integer sz = (Integer) params.size();
   for (i=0; i < sz; i++)
   {
      params.push_back((Parameter*)((cb.params).at(i))->Clone());
   }
   
   initialized = false;
   return *this;
}


//------------------------------------------------------------------------------
//  ~ConditionalBranch()
//------------------------------------------------------------------------------
/**
 * Destroys the ConditionalBranch command.  (destructor)
 */
//------------------------------------------------------------------------------
ConditionalBranch::~ConditionalBranch()
{
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the conditional branch command.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::Initialize()
{
    bool retval = BranchCommand::Initialize();

    // specific initialization goes here
    
    return retval;
}

//------------------------------------------------------------------------------
//  bool SetCondition(const std::string &lhs, const std::string &operation,
//                    const std::string &rhs, Integer atIndex)
//------------------------------------------------------------------------------
/**
 * This method sets a condition for the ConditionalBranch Command.
 *
 * @return true if successful; false otherwise.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetCondition(const std::string &lhs, 
                                     const std::string &operation,
                                     const std::string &rhs,
                                     Integer atIndex)
{
   bool   retVal = false;
   OpType ot     = NumberOfOperators;
   // determine the operator
   for (Integer i = 0; i < NumberOfOperators; i++)
   {
      if (operation == OPTYPE_TEXT[i])
      {
         ot = (OpType) i;
         break;
      }
   }
   if (ot == NumberOfOperators)
   {
      throw CommandException(
             "ConditionalCommand error: invalid operator");
   }
   // put it at the end, if requested (and by default)
   if ((atIndex == -999) || (atIndex == numberOfConditions))
   {
      opStrings.push_back(operation);
      opList.push_back(ot);
      lhsList.push_back(lhs);
      rhsList.push_back(rhs);
      retVal = true;
      numberOfConditions++;
   }
   // assume that logical operators will be added in order
   else if ((atIndex < 0) || (atIndex > numberOfConditions))
   {
      throw CommandException(
            "ConditionalCommand error: condition index out of bounds");
   }
   // otherwise, replace an already-existing condition
   else 
   {
      opStrings.at(atIndex) = operation;
      opList.at(atIndex)    = ot;
      lhsList.at(atIndex)   = lhs;
      rhsList.at(atIndex)   = rhs;
      retVal                = true;
   }
   return retVal;
}

//------------------------------------------------------------------------------
//  bool SetConditionOperator(const std::string &op, Integer atIndex)
//------------------------------------------------------------------------------
/**
 * This method sets a logical operator for the ConditionalBranch Command.
 *
 * @return true if successful; false otherwise.
 *
 * @note This method assumes that condition operators are added in order
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetConditionOperator(const std::string &op, 
                                             Integer atIndex)
{
   bool          retVal = false;
   LogicalOpType ot     = NumberOfLogicalOperators;
   // determine the logical operator
   for (Integer i = 0; i < NumberOfLogicalOperators; i++)
   {
      if (op == LOGICAL_OPTYPE_TEXT[i])
      {
         ot = (LogicalOpType) i;
         break;
      }
   }
   if (ot == NumberOfLogicalOperators)
   {
      throw CommandException(
            "ConditionalCommand error: invalid logical operator");
   }
   if ((atIndex == -999) || (atIndex == numberOfLogicalOps))
   {
      logicalOpStrings.push_back(op);
      logicalOpList.push_back(ot);
      retVal = true;
      numberOfLogicalOps++;
   }
   // assume that logical operators will be added in order
   else if ((atIndex < 0) || (atIndex > numberOfLogicalOps))
   {
      throw CommandException(
            "ConditionalCommand error: logical operator index out of bounds");
   }
   // put it at the end, if requested (and by default)
   // otherwise, replace an already-existing logical operator
   else 
   {
      logicalOpStrings.at(atIndex) = op;
      logicalOpList.at(atIndex)    = ot;
      retVal                       = true;
   }
   return retVal;
}

//------------------------------------------------------------------------------
//  bool RemoveCondition(Integer atIndex)
//------------------------------------------------------------------------------
/**
 * Removes the condition for the command, at index atIndex.
 * 
 * @param <atIndex>   where in the list to remove the condition from.
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::RemoveCondition(Integer atIndex)
{
   if ((atIndex < 0) || (atIndex >= numberOfConditions))
      throw CommandException(
            "RemoveCondition error - condition index out of bounds.");
   lhsList.erase(lhsList.begin() + atIndex);
   opStrings.erase(opStrings.begin() + atIndex);
   opList.erase(opList.begin() + atIndex);
   rhsList.erase(rhsList.begin() + atIndex);
   numberOfConditions--;
   return true;
}

//------------------------------------------------------------------------------
//  bool RemoveConditionOperator(Integer atIndex)
//------------------------------------------------------------------------------
/**
 * Removes the logical operator for the command, at index atIndex.
 * 
 * @param <atIndex>   where in the list to remove the logical operator from.
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::RemoveConditionOperator(Integer atIndex)
{
   if ((atIndex < 0) || (atIndex >= numberOfLogicalOps))
      throw CommandException(
            "RemoveConditionOperator error - condition index out of bounds.");
   logicalOpStrings.erase(logicalOpStrings.begin() + atIndex);
   logicalOpList.erase(logicalOpList.begin() + atIndex);
   numberOfLogicalOps--;
   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                         const std::string &name,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the ConditionalBranch Command.
 *
 * @param <type>  type of the reference object requested
 * @param <name>  name of the reference object requested
 * @param <index> index into the array of reference objects
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ConditionalBranch::GetRefObject(const Gmat::ObjectType type,
                                          const std::string &name,
                                          const Integer index)
{
   switch (type)
   {
      case Gmat::PARAMETER:
         if ((index >= 0) && (index < (Integer)params.size()))
         {
            return params.at(index);
         }
         else
         {
            throw CommandException(
            "ConditionalBranch::GetRefObject() invalid index\n");
         }
      default:
         break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return BranchCommand::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the ConditionalBranch Command.
 *
 * @param <obj>   pointer to the reference object
 * @param <type>  type of the reference object 
 * @param <name>  name of the reference object
 * @param <index> index into the array of reference objects (where to put this
 *                 one)
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name,
                                     const Integer index)
{
   
   switch (type)
   {
      case Gmat::PARAMETER:
      {
         Integer size = params.size();
         if (params.empty() && index == 0)
         {
            params.push_back((Parameter *)obj);
            return true;
         }
         else if (index == size)
         {
            params.push_back((Parameter *)obj);
            return true;
         }
         else if (index < size)
         {
            params.at(index) = (Parameter *)obj;
            return true;
         }
         else
         {
            return false;
         }
      }
      default:
         break;
   }

   // Not handled here -- invoke the next higher SetRefObject call
   return BranchCommand::SetRefObject(obj, type, name, index);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string ConditionalBranch::GetParameterText(const Integer id) const
{
   if (id >= BranchCommandParamCount && id < ConditionalBranchParamCount)
      return PARAMETER_TEXT[id - BranchCommandParamCount];
   return BranchCommand::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer ConditionalBranch::GetParameterID(const std::string &str) const
{
   for (Integer i = BranchCommandParamCount; i < ConditionalBranchParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - BranchCommandParamCount])
         return i;
   }
   
   return BranchCommand::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ConditionalBranch::GetParameterType(const Integer id) const
{
   if (id >= BranchCommandParamCount && id < ConditionalBranchParamCount)
      return PARAMETER_TYPE[id - BranchCommandParamCount];
   
   return BranchCommand::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string ConditionalBranch::GetParameterTypeString(const Integer id) const
{
   return ConditionalBranch::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer ConditionalBranch::GetIntegerParameter(const Integer id) const
{
   if (id == NUMBER_OF_CONDITIONS)          return numberOfConditions;
   if (id == NUMBER_OF_LOGICAL_OPS)         return numberOfLogicalOps;
   if (id == NUMBER_OF_REF_PARAMS)          return (Integer) params.size();
   
   return BranchCommand::GetIntegerParameter(id); 
}

//Integer      ConditionalBranch::SetIntegerParameter(const Integer id, const Integer value);

//------------------------------------------------------------------------------
//  Integer  GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer ConditionalBranch::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

//Integer      ConditionalBranch::SetIntegerParameter(const std::string &label, const Integer value);

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method gets a string parameter value of a StringArray, for the input
 * parameter ID, at the input index into the array.
 *
 * @param <id>    ID for the requested parameter.
 * @param <index> index into the StringArray.
 *
 * @return  string value of the requested StringArray parameter, at the 
 *          requested index.
 *
 */
//------------------------------------------------------------------------------
std::string ConditionalBranch::GetStringParameter(const Integer id,
                                                  const Integer index) const
{
   std::string errorString = "ConditionalCommand error: Requested index ";
   errorString += index;
   errorString += " is out of bounds for ";
   if (id == LEFT_HAND_STRINGS)
   {
      if (index < 0 || index >= (Integer) lhsList.size())
      {
         errorString += "left hand side string list.";
         throw CommandException(errorString);
      }
      return (lhsList.at(index));
   }
   if (id == OPERATOR_STRINGS)   
   {
      if (index < 0 || index >= (Integer) opStrings.size())
      {
         errorString += "operator string list.";
         throw CommandException(errorString);
      }
      return (opStrings.at(index));
   }
   if (id == RIGHT_HAND_STRINGS) 
   {
      if (index < 0 || index >= (Integer) rhsList.size())
      {
         errorString += "right hand side string list.";
         throw CommandException(errorString);
      }
      return (rhsList.at(index));
   }
   if (id == LOGICAL_OPERATORS)  
   {
      if (index < 0 || index >= (Integer) logicalOpStrings.size())
      {
         errorString += "logical operator string list.";
         throw CommandException(errorString);
      }
      return (logicalOpStrings.at(index));
   }
   //if (id == PARAMETER_NAMES)  return ??;
   
   return BranchCommand::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const Integer id, const std::string &value,
//                          const Integer index) const
//------------------------------------------------------------------------------
 /**
 * This method sets a string parameter value of a StringArray, for the input
 * parameter ID, at the input index into the array.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> value to set the parameter to
 * @param <index> index into the StringArray.
 *
 * @return  success or failure of the operation.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetStringParameter(const Integer id, 
                                           const std::string &value,
                                           const Integer index)
{
   std::string errorString = "ConditionalCommand error: Requested index ";
   errorString += index;
   errorString += " is out of bounds for ";
   if (id == LEFT_HAND_STRINGS)
   {
      if (index < 0 || index >= (Integer) lhsList.size())
      {
         errorString += "left hand side string list.";
         throw CommandException(errorString);
      }
      lhsList.at(index) = value;
      return true;
      //return SetStringArrayValue(id, value, index);
   }
   if (id == OPERATOR_STRINGS)   
   {
      if (index < 0 || index >= (Integer) opStrings.size())
      {
         errorString += "operator string list.";
         throw CommandException(errorString);
      }
      opStrings.at(index) = value;
      return true;
      //return SetStringArrayValue(id, value, index);
   }
   if (id == RIGHT_HAND_STRINGS) 
   {
      if (index < 0 || index >= (Integer) rhsList.size())
      {
         errorString += "right hand side string list.";
         throw CommandException(errorString);
      }
      rhsList.at(index) = value;
      return true;
      //return SetStringArrayValue(id, value, index);
   }
   if (id == LOGICAL_OPERATORS)  
   {
      if (index < 0 || index >= (Integer) logicalOpStrings.size())
      {
         errorString += "logical operator string list.";
         throw CommandException(errorString);
      }
      logicalOpStrings.at(index) = value;
      return true;
      //return SetStringArrayValue(id, value, index);
   }
   //if (id == PARAMETER_NAMES)  return ??;
   
   return BranchCommand::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  std::string GetStringParameter(const std::string &label, 
//                                 const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method gets a string parameter value of a StringArray, for the input
 * parameter label, at the input index into the array.
 *
 * @param <label> label for the requested parameter.
 * @param <index> index into the StringArray.
 *
 * @return  string value of the requested StringArray parameter, at the 
 *          requested index.
 *
 */
//------------------------------------------------------------------------------
std::string ConditionalBranch::GetStringParameter(const std::string &label,
                                        const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
//  bool SetStringParameter(const std::string &label, const std::string &value,
//                          const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method gets a string parameter value of a StringArray, for the input
 * parameter label, at the input index into the array.
 *
 * @param <label> label for the requested parameter.
 * @param <value> value to set the parameter to
 * @param <index> index into the StringArray.
 *
 * @return  success or failure of the operation.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetStringParameter(const std::string &label, 
                                        const std::string &value,
                                        const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& 
ConditionalBranch::GetStringArrayParameter(const Integer id) const
{
   if (id == LEFT_HAND_STRINGS)  return lhsList;
   if (id == OPERATOR_STRINGS)   return opStrings;
   if (id == RIGHT_HAND_STRINGS) return rhsList;
   if (id == LOGICAL_OPERATORS)  return logicalOpStrings;
   //if (id == PARAMETER_NAMES)  return ??;
   
   return BranchCommand::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const StringArray&   GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the StringArray parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  StringArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const StringArray& 
ConditionalBranch::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//const StringArray& GetStringArrayParameter(const Integer id, 
//                                           const Integer index) const; 
//const StringArray& ConditionalBranch::GetStringArrayParameter(const std::string &label, 
//                                                              const Integer index) const; 

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  bool EvaluateCondition(Integer which)
//------------------------------------------------------------------------------
/**
 * This method evaluates the specified condition.
 *
 * @param which index into the array(s) of conditions
 *
 * @return false if which is out of bounds; result of evaluating the
 *         condition, otherwise.
 *
 * @note This method currently assumes that the rhs of the condition is a
 *       Real number.  In the future, we will need to allow for the possibility
 *       of the rhs of the condition being another parameter, which will need
 *       to be evaluated.
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::EvaluateCondition(Integer which)
{
   if ((which < 0) || (which >= numberOfConditions))
   {
      return false;
   }
   Real        lhsValue = GmatBase::REAL_PARAMETER_UNDEFINED;
   Real        rhsValue = GmatBase::REAL_PARAMETER_UNDEFINED;
   bool        lhFound = false, rhFound = false;
   bool        rightIsParm = false;
   bool        leftIsParm  = false;
   std::string theLHSParmName = "";
   //std::string theLHSParmName = lhsList.at(which);
   std::string theRHSParmName = "";
   
   char firstChar = (rhsList.at(which)).at(0);
   if (isalpha(firstChar))  // if not a real, assume a Parameter  
   {
      theRHSParmName = rhsList.at(which);
      rightIsParm    = true;
   }
   else
   {
      std::istringstream rhsStr(rhsList.at(which)); 
      rhsStr >> rhsValue;
   }
   firstChar = (lhsList.at(which)).at(0);
   if (isalpha(firstChar))  // if not a real, assume a Parameter  
   {
      theLHSParmName = lhsList.at(which);
      leftIsParm     = true;
   }
   else
   {
      std::istringstream lhsStr(lhsList.at(which)); 
      lhsStr >> lhsValue;
   }
   // iterate over the list of reference objects to find the parameter
   if (rightIsParm || leftIsParm)
   {
      std::vector<Parameter*>::iterator p = params.begin();
      while (p != params.end())
      {
         if (leftIsParm && ((*p)->GetName() == theLHSParmName))
         {
            lhsValue = (*p)->EvaluateReal();
            lhFound  = true;
         }
         if (rightIsParm && ((*p)->GetName() == theRHSParmName))
         {
            rhsValue = (*p)->EvaluateReal();
            rhFound  = true;
         }
         ++p;
      }
      if (leftIsParm && !lhFound) 
         throw CommandException("Parameter " + theLHSParmName +
              " not found for LHS string in condition ");// + which);
      if (rightIsParm && !rhFound)
         throw CommandException("Parameter " + theRHSParmName +
              " not found for RHS string in condition ");// + which);
   }
   switch (opList.at(which))
   {
      case EQUAL_TO:
         return (lhsValue == rhsValue);
         break;
      case NOT_EQUAL:
         return (lhsValue != rhsValue);
         break;
      case GREATER_THAN:
         return (lhsValue > rhsValue);
         break;
      case LESS_THAN:
         return (lhsValue < rhsValue);
         break;
      case GREATER_OR_EQUAL:
         return (lhsValue >= rhsValue);
         break;
      case LESS_OR_EQUAL:
         return (lhsValue <= rhsValue);
         break;
      default:
         return false;
         break;
   }
   return false; 
   
}

//------------------------------------------------------------------------------
//  bool EvaluateAllConditions()
//------------------------------------------------------------------------------
/**
 * This method evaluates the entire list of conditions, using their logical
 * operators.  Evaluation is from left to right.
 *
 * @return result of evaluating the entire list of conditions, from left to
 *         right.
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::EvaluateAllConditions()
{
   if (numberOfConditions == 0)
      throw CommandException(
         "Error in conditional statement - no conditions specified.");
   if (numberOfConditions != (numberOfLogicalOps + 1))
      throw CommandException(
         "conditional statement incorrect - too few/many logical operators");
   bool soFar = EvaluateCondition(0);
   Integer i = 1;
   for (i=1; i < numberOfConditions; i++)
   {
      switch (logicalOpList.at(i-1))
      {
         case AND:
            soFar = soFar && EvaluateCondition(i);
            break;
         case OR:
            soFar = soFar || EvaluateCondition(i);
            break;
         default:
            throw CommandException(
                  "Unknown logical operator in conditional statement.");
            break;
      }
   }
   return soFar;
}
// remove this?
bool ConditionalBranch::SetStringArrayValue(Integer forArray, 
                                            const std::string &toValue,
                                            Integer forIndex)
{
   return true;  // TEMPORARY
}


