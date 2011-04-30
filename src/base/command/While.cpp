//$Id$
//------------------------------------------------------------------------------
//                                While
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "MessageInterface.hpp"


//#define DEBUG_WHILE
//#define DEBUG_WHILE_RERUN
//#define DEBUG_WHILE_END

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
   //nestLevel = 0;
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

    
//------------------------------------------------------------------------------10001
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
   #ifdef DEBUG_WHILE_END // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
      "Entering Append (%s) of while command   %s   %s  %s\n",
      (cmd->GetTypeName()).c_str(), lhsList[0].c_str(), opStrings[0].c_str(), 
      rhsList[0].c_str());
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
    if (!ConditionalBranch::Append(cmd))
    {
      #ifdef DEBUG_WHILE_END // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "    and ConditionalBranch::Append() returned false\n");
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
        return false;
    }

    // Check for the end of "While" branch, point that end back to this command
    if (cmd->GetTypeName() == "EndWhile")
    {
       if ((nestLevel== 0) && (branchToFill != -1))
       {
          cmd->Append(this);
          // While statement is complete; -1 points us back to the main sequence.
          branchToFill = -1;
         #ifdef DEBUG_WHILE_END // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
            "      -->Appending EndWhile command to while   %s   %s  %s\n",
            lhsList[0].c_str(), opStrings[0].c_str(), rhsList[0].c_str());
            MessageInterface::ShowMessage("----> that is, appending object %p to object %p\n",
            this, cmd);
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
       }
       else
       {
         #ifdef DEBUG_WHILE_END // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
            "         Is an EndWhile command for while   %s   %s  %s, but not appending\n",
            lhsList[0].c_str(), opStrings[0].c_str(), rhsList[0].c_str());
            MessageInterface::ShowMessage(
            "         and nestLevel = %d\n", nestLevel);
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
          --nestLevel;
       }
    }

    if (cmd->GetTypeName() == "While")
    {
      ++nestLevel;
      #ifdef DEBUG_WHILE_END // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
         MessageInterface::ShowMessage(
         "      +++Increasing nestLevel for while   %s   %s  %s to %d\n",
         lhsList[0].c_str(), opStrings[0].c_str(), rhsList[0].c_str(), nestLevel);
      #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
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
   #ifdef DEBUG_WHILE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
      MessageInterface::ShowMessage(
         "While::Executing(%s %s %s) status: commandComplete = %s, "
         "commandExecuting = %s, branchExecuting = %s\n",
         lhsList[0].c_str(), opStrings[0].c_str(), rhsList[0].c_str(),
         ((commandComplete) ? "true" : "false"),
         ((commandExecuting) ? "true" : "false"),
         ((branchExecuting) ? "true" : "false") );
   #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~

   bool retval = true;

   // First see if we're in a branch run
   if (branchExecuting)
   {
      retval = ExecuteBranch();
   }
   else 
   {
      // If not, check to see what to do and do it.
      if (!commandExecuting) 
      {
         #ifdef DEBUG_WHILE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage("Starting command\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         ConditionalBranch::Execute();
         commandComplete  = false;
      }

      if (EvaluateAllConditions()) // must deal with multiple conditions later
      {
         #ifdef DEBUG_WHILE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
               "   Conditions true, running while loop\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         branchExecuting = true;
         return true;
      }
      else  // fails condition, so while loop is done
      {
         #ifdef DEBUG_WHILE // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ debug ~~~~
            MessageInterface::ShowMessage(
               "   Conditions false; command complete\n");
         #endif // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end debug ~~~~
         publisher->FlushBuffers();
         commandComplete  = true;
         commandExecuting = false;
         branchExecuting  = false;
      }
   }
   
   BuildCommandSummary(true);   
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
 * @return The script line that, when interpreted, defines this While command.
 */
//------------------------------------------------------------------------------
const std::string& While::GetGeneratingString(Gmat::WriteMode mode,
                                           const std::string &prefix,
                                           const std::string &useName)
{
   if (mode == Gmat::NO_COMMENTS)
   {
      generatingString = "While " + GetConditionalString();
      return generatingString;
   }
   
   // Build the local string
   generatingString = prefix + "While " + GetConditionalString();
   return ConditionalBranch::GetGeneratingString(mode, prefix, useName);
}



//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
