//$Header$
//------------------------------------------------------------------------------
//                                IfCommand
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus/GSFC
// Created: 2004/01/30
// Modified: 2004.07.08 Wendy Shoan/GSFC
//
/**
 * Definition for the IfCommand command class
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "gmatdefs.hpp"
#include "IfCommand.hpp"
#include "Parameter.hpp"

const std::string
IfCommand::OPTYPE_TEXT[NumberOfOperators] =
{
   "==",
   "~=",
   ">",
   "<",
   ">=",
   "<="
};

//------------------------------------------------------------------------------
//  IfCommand()
//------------------------------------------------------------------------------
/**
 * Creates a IfCommand command.  (default constructor)
 */
//------------------------------------------------------------------------------
IfCommand::IfCommand() :
BranchCommand      ("If"),
numberOfConditions (0),
nestLevel          (0)
{
}


//------------------------------------------------------------------------------
//  IfCommand(const IfCommand &ic)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates an If command.  (Copy constructor)
 */
//------------------------------------------------------------------------------
IfCommand::IfCommand(const IfCommand &ic) :
BranchCommand      (ic),
numberOfConditions (0),  // or do I want to copy them?
nestLevel          (0)
{
}


//------------------------------------------------------------------------------
//  IfCommand& operator=(const IfCommand &ic)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the If command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
IfCommand& IfCommand::operator=(const IfCommand &ic)
{
   if (this == &ic)
      return *this;
   BranchCommand::operator=(ic);
   // do I need to assign numberofConditions, all of the conditions, etc. here?
   return *this;
}


//------------------------------------------------------------------------------
//  ~IfCommand()
//------------------------------------------------------------------------------
/**
 * Destroys the If command.  (destructor)
 */
//------------------------------------------------------------------------------
IfCommand::~IfCommand()
{
}

    
//------------------------------------------------------------------------------
//  bool Append(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the IF statement.
 *
 * This method calls the BranchCommand base class method that adds a command
 * to the command sequence that branches off of the main mission sequence.  This
 * extension was needed so that the EndIf command can be set to point back 
 * to the head of the IF statement.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool IfCommand::Append(GmatCommand *cmd)
{
    if (!BranchCommand::Append(cmd))
        return false;

    // Check for the end of "If" branch, point that end back to this command
    if (cmd->GetTypeName() == "EndIf" || cmd->GetTypeName() == "Else")
    {
       if (nestLevel == 0)
       {
          cmd->Append(this);
          // IF statement is complete; -1 points us back to the main sequence.
          if (cmd->GetTypeName() == "EndIf")
             branchToFill = -1;
          else // "Else" starts another branch
             ++branchToFill;
       }
       else
       {
          --nestLevel;
       }
    }

    if (cmd->GetTypeName() == "If")
    {
       ++nestLevel;
    }

    return true;
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs the initialization needed to run the IF statement.
 *
 * @return true if the Command is initialized, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool IfCommand::Initialize()
{
    bool retval = BranchCommand::Initialize();

    // if specific initialization goes here:
    
    return retval;
}


//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Target the variables defined for this for IF statement.
 *
 * This method (will eventually) runs the state machine in order to
 * determine the variable values needed to achieve the user specified 
 * goals.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool IfCommand::Execute()
{
   bool retval = true;
   
   BranchCommand::Execute(); // ??
   commandComplete  = false;
   commandExecuting = true;
   
   if (EvaluateCondition(0)) // must deal with multiple conditions later
   {
      retval = ExecuteBranch();
      commandComplete  = true;
      commandExecuting = false;
   }
   else if ((Integer)branch.size() > 1)  // for now, there could be an 'Else'
   {
      retval = ExecuteBranch(1);
      commandComplete  = true;
      commandExecuting = false;
   }
   
   return retval;
}

//------------------------------------------------------------------------------
//  bool SetCondition(std::string lhs, std::string operation,
//                    std::string rhs)
//------------------------------------------------------------------------------
/**
 * This method sets a condition for the If Command.
 *
 * @return true if successful; false otherwise.
 *
 */
//------------------------------------------------------------------------------
bool IfCommand::SetCondition(std::string lhs, std::string operation,
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
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the IfCommand.
 *
 * @return clone of the IfCommand.
 *
 */
//------------------------------------------------------------------------------
GmatBase* IfCommand::Clone() const
{
   return (new IfCommand(*this));
}


//std::string IfCommand::GetRefObjectName(const Gmat::ObjectType type) const;
//bool IfCommand::SetRefObjectName(const Gmat::ObjectType type,
//                      const std::string &name);
//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                         const std::string &name,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
* This method returns a reference object from the If Command.
 *
 * @param <type>  type of the reference object requested
 * @param <name>  name of the reference object requested
 * @param <index> index into the array of reference objects
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* IfCommand::GetRefObject(const Gmat::ObjectType type,
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
            throw CommandException("IfCommand::GetRefObject() invalid index\n");
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
 * This method sets a reference object for the If Command.
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
bool IfCommand::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
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
//ObjectArray& IfCommand::GetRefObjectArray(const Gmat::ObjectType type);


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  bool EvaluateCondition(Integer which)
//------------------------------------------------------------------------------
/**
 * This method evaluates the spscified condition.
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
bool IfCommand::EvaluateCondition(Integer which)
{
   if ((which < 0) || (which >= (Integer) (lhsList.size())))
   {
      return false;
   }
   Real        lhsValue;
   Real        rhsValue;
   std::string theParmName = lhsList.at(which);
   std::istringstream rhsStr(rhsList.at(which));
   //rhsStr.str(rhsList.at(which));
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
   return false; // what should be done here, really?
   
}




