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
       if (nestLevel == 0)
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
