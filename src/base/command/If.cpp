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
    if (cmd->GetTypeName() == "EndIf" || cmd->GetTypeName() == "Else")
    {
       if ((nestLevel== 0) && (branchToFill != -1))
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
