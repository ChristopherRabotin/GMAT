//$Header$
//------------------------------------------------------------------------------
//                                While
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Wendy Shoan/GSFC/MAB
// Created: 2004/08/10
//
/**
 * Definition for the While command class
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "gmatdefs.hpp"
#include "While.hpp"
#include "Parameter.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
While::PARAMETER_TEXT[WhileParamCount - ConditionalBranchParamCount] =
{
   "NestLevel",
};

const Gmat::ParameterType
While::PARAMETER_TYPE[WhileParamCount - ConditionalBranchParamCount] =
{
   Gmat::INTEGER_TYPE,     
};

//------------------------------------------------------------------------------
//  While()
//------------------------------------------------------------------------------
/**
 * Creates a While command.  (default constructor)
 */
//------------------------------------------------------------------------------
While::While() :
ConditionalBranch  ("While"),
nestLevel          (0)
{
}


//------------------------------------------------------------------------------
//  While(const While &wc)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates an While command.  (Copy constructor)
 */
//------------------------------------------------------------------------------
While::While(const While &wc) :
ConditionalBranch  (wc),
nestLevel          (0)
{
}


//------------------------------------------------------------------------------
//  While& operator=(const While &wc)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the While command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
While& While::operator=(const While &wc)
{
   if (this == &wc)
      return *this;
   ConditionalBranch::operator=(wc);
   // do I need to assign numberofConditions, all of the conditions, etc. here?
   return *this;
}


//------------------------------------------------------------------------------
//  ~While()
//------------------------------------------------------------------------------
/**
 * Destroys the While command.  (destructor)
 */
//------------------------------------------------------------------------------
While::~While()
{
}

    
//------------------------------------------------------------------------------
//  bool Append(GmatCommand *cmd)
//------------------------------------------------------------------------------
/**
 * Adds a command to the While statement.
 *
 * This method calls the BranchCommand base class method that adds a command
 * to the command sequence that branches off of the main mission sequence.  This
 * extension was needed so that the EndWhile command can be set to point back 
 * to the head of the While statement.
 *
 * @return true if the Command is appended, false if an error occurs.
 */
//------------------------------------------------------------------------------
bool While::Append(GmatCommand *cmd)
{
    if (!ConditionalBranch::Append(cmd))
        return false;

    // Check for the end of "While" branch, point that end back to this command
    if (cmd->GetTypeName() == "EndWhile")
    {
       if ((nestLevel== 0) && (branchToFill != -1))
       {
          cmd->Append(this);
          // While statement is complete; -1 points us back to the main sequence.
          branchToFill = -1;
       }
       else
       {
          --nestLevel;
       }
    }

    if (cmd->GetTypeName() == "While")
    {
       ++nestLevel;
    }

    return true;
}

//------------------------------------------------------------------------------
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Run the branch for this While statement, based on the condition
 * evaluation.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool While::Execute()
{
   bool retval = true;
   
   ConditionalBranch::Execute();
   //commandComplete  = false;
   //commandExecuting = true;
   
   if (EvaluateAllConditions()) // must deal with multiple conditions later
   {
      retval = ExecuteBranch();
   }
   else  // fails condition, so while loop is done
   {
      commandComplete  = true;
      commandExecuting = false;
   }
   
   return retval;
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
std::string While::GetParameterText(const Integer id) const
{
   if (id >= ConditionalBranchParamCount && id < WhileParamCount)
      return PARAMETER_TEXT[id - ConditionalBranchParamCount];
   return ConditionalBranch::GetParameterText(id);
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
Integer While::GetParameterID(const std::string &str) const
{
   for (Integer i = ConditionalBranchParamCount; i < WhileParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ConditionalBranchParamCount])
         return i;
   }
   
   return ConditionalBranch::GetParameterID(str);
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
Gmat::ParameterType While::GetParameterType(const Integer id) const
{
   if (id >= ConditionalBranchParamCount && id < WhileParamCount)
      return PARAMETER_TYPE[id - ConditionalBranchParamCount];
   
   return ConditionalBranch::GetParameterType(id);
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
std::string While::GetParameterTypeString(const Integer id) const
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
Integer While::GetIntegerParameter(const Integer id) const
{
   if (id == NEST_LEVEL)          return nestLevel;
   
   return ConditionalBranch::GetIntegerParameter(id); 
}

//------------------------------------------------------------------------------
//  Integer  SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
* This method sets the Integer parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Integer value for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer While::SetIntegerParameter(const Integer id,
                                const Integer value) 
{
   if (id == NEST_LEVEL)          return (nestLevel  = value);
   
   return ConditionalBranch::SetIntegerParameter(id,value);  // add others in later
}

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
Integer While::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Integer  SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
* This method sets the Integer parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 * @param <value> Integer value for the requested parameter.
 *
 * @return  Integer value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer While::SetIntegerParameter(const std::string &label,
                                const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the While.
 *
 * @return clone of the While.
 *
 */
//------------------------------------------------------------------------------
GmatBase* While::Clone() const
{
   return (new While(*this));
}



//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
