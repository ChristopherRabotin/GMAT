//$Header$
//------------------------------------------------------------------------------
//                                If
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
// Modified: 2004.07.08 Wendy Shoan/GSFC - updated with conditions, etc.
//
/**
 * Definition for the If command class
 */
//------------------------------------------------------------------------------

#include <sstream>
#include "gmatdefs.hpp"
#include "If.hpp"
#include "Parameter.hpp"

//---------------------------------
// static data
//---------------------------------
const std::string
If::PARAMETER_TEXT[IfParamCount - ConditionalBranchParamCount] =
{
   "NestLevel",
};

const Gmat::ParameterType
If::PARAMETER_TYPE[IfParamCount - ConditionalBranchParamCount] =
{
   Gmat::INTEGER_TYPE,     
};

//------------------------------------------------------------------------------
//  If()
//------------------------------------------------------------------------------
/**
 * Creates a If command.  (default constructor)
 */
//------------------------------------------------------------------------------
If::If() :
ConditionalBranch  ("If"),
nestLevel          (0)
{
}


//------------------------------------------------------------------------------
//  If(const If &ic)
//------------------------------------------------------------------------------
/**
 * Constructor that replicates an If command.  (Copy constructor)
 */
//------------------------------------------------------------------------------
If::If(const If &ic) :
ConditionalBranch  (ic),
nestLevel          (0)
{
}


//------------------------------------------------------------------------------
//  If& operator=(const If &ic)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the If command.
 *
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
If& If::operator=(const If &ic)
{
   if (this == &ic)
      return *this;
   ConditionalBranch::operator=(ic);
   // do I need to assign numberofConditions, all of the conditions, etc. here?
   return *this;
}


//------------------------------------------------------------------------------
//  ~If()
//------------------------------------------------------------------------------
/**
 * Destroys the If command.  (destructor)
 */
//------------------------------------------------------------------------------
If::~If()
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
bool If::Append(GmatCommand *cmd)
{
    if (!ConditionalBranch::Append(cmd))
        return false;

    // Check for the end of "If" branch, point that end back to this command
    if (cmd->GetTypeName() == "EndIf" || cmd->GetTypeName() == "Else" ||
        cmd->GetTypeName() == "ElseIf")
    {
       if ((nestLevel== 0) && (branchToFill != -1))
       {
          cmd->Append(this);
          // IF statement is complete; -1 points us back to the main sequence.
          if (cmd->GetTypeName() == "EndIf")
             branchToFill = -1;
          else // "Else" or "ElseIf" starts another branch
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
//  bool Execute()
//------------------------------------------------------------------------------
/**
 * Execute the proper branch for this IF statement.
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool If::Execute()
{
   bool retval = true;
   
   ConditionalBranch::Execute(); 
   
   //if (EvaluateCondition(0)) // must deal with multiple conditions later
   if (EvaluateAllConditions()) 
   {
      retval = ExecuteBranch();
   }
   else if ((Integer)branch.size() > 1)  // there could be an 'Else'
   {
      retval = ExecuteBranch(1);
   }
   commandComplete  = true;
   commandExecuting = false;
   
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
std::string If::GetParameterText(const Integer id) const
{
   if (id >= ConditionalBranchParamCount && id < IfParamCount)
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
Integer If::GetParameterID(const std::string &str) const
{
   for (Integer i = ConditionalBranchParamCount; i < IfParamCount; i++)
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
Gmat::ParameterType If::GetParameterType(const Integer id) const
{
   if (id >= ConditionalBranchParamCount && id < IfParamCount)
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
std::string If::GetParameterTypeString(const Integer id) const
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
Integer If::GetIntegerParameter(const Integer id) const
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
Integer If::SetIntegerParameter(const Integer id,
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
Integer If::GetIntegerParameter(const std::string &label) const
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
Integer If::SetIntegerParameter(const std::string &label,
                                const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the If.
 *
 * @return clone of the If.
 *
 */
//------------------------------------------------------------------------------
GmatBase* If::Clone() const
{
   return (new If(*this));
}




//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
