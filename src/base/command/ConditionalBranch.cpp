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
#include "gmatdefs.hpp"
#include "ConditionalBranch.hpp"
#include "Parameter.hpp"

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
      opList.push_back((cb.opList).at(i));
      rhsList.push_back((cb.rhsList).at(i));
   }
   for (i=0; i < numberOfLogicalOps; i++)
   {
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
      opList.push_back((cb.opList).at(i));
      rhsList.push_back((cb.rhsList).at(i));
   }
   logicalOpList.clear();
   for (i=0; i < numberOfLogicalOps; i++)
   {
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
//  bool SetCondition(std::string lhs, std::string operation,
//                    std::string rhs)
//------------------------------------------------------------------------------
/**
 * This method sets a condition for the ConditionalBranch Command.
 *
 * @return true if successful; false otherwise.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetCondition(std::string lhs, std::string operation,
                                     std::string rhs)
{
   bool retVal = false;
   for (Integer i = 0; i < NumberOfOperators; i++)
   {
      if (operation == OPTYPE_TEXT[i])
      {
         opList.push_back((OpType)i);
         retVal = true;
         break;
      }
   }
   if (retVal)
   {
      lhsList.push_back(lhs);
      rhsList.push_back(rhs);
      numberOfConditions++;
   }
   return retVal;
}

//------------------------------------------------------------------------------
//  bool SetConditionOperator(std::string& op)
//------------------------------------------------------------------------------
/**
 * This method sets a logical operator for the ConditionalBranch Command.
 *
 * @return true if successful; false otherwise.
 *
 */
//------------------------------------------------------------------------------
bool ConditionalBranch::SetConditionOperator(std::string& op)
{
   bool retVal = false;
   for (Integer i = 0; i < NumberOfLogicalOperators; i++)
   {
      if (op == LOGICAL_OPTYPE_TEXT[i])
      {
         logicalOpList.push_back((LogicalOpType)i);
         retVal = true;
         break;
      }
   }
   if (retVal)
      numberOfLogicalOps++;
   return retVal;
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
         if (index < (Integer)params.size())
         {
            return params[index];
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
            params[index] = (Parameter *)obj;
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
   //if ((which < 0) || (which >= (Integer) (lhsList.size())))
   if ((which < 0) || (which >= numberOfConditions))
   {
      return false;
   }
   Real        lhsValue;
   Real        rhsValue;
   std::string theParmName = lhsList.at(which);
   std::istringstream rhsStr(rhsList.at(which)); // todo: change to a parameter
   rhsStr >> rhsValue;

   // iterate over the list of reference objects to find the parameter
   std::vector<Parameter*>::iterator p = params.begin();
   while (p != params.end())
   {
      if ((*p)->GetName() == theParmName)
      {
         lhsValue = (*p)->EvaluateReal();
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
      }
      ++p;
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

