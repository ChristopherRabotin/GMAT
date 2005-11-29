//$Header$
//------------------------------------------------------------------------------
//                                 CommandUtil
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/11/28
//
/**
 * This file provides methods to get whole mission sequence string.
 */
//------------------------------------------------------------------------------

#include "CommandUtil.hpp"

//------------------------------------------------------------------------------
// GmatCommand* GetLastCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
GmatCommand* GmatCommandUtil::GetLastCommand(GmatCommand *cmd)
{
   GmatCommand *prevCmd = NULL;
   
   while (cmd != NULL)
   {
      cmd = cmd->GetNext();
      if (cmd != NULL)
         prevCmd = cmd;
   }
   
   return prevCmd;
}


//------------------------------------------------------------------------------
// std::string GetCommandSeqString(GmatCommand *cmd)
//------------------------------------------------------------------------------
std::string GmatCommandUtil::GetCommandSeqString(GmatCommand *cmd)
{
   std::string cmdseq;
   cmdseq.append("\n---------- Mission Sequence ----------\n");
   
   while (cmd != NULL)
   {
      cmdseq.append("--- " + cmd->GetTypeName() + "\n");

      if ((cmd->GetChildCommand(0)) != NULL)
         GetSubCommands(cmd, 0, cmdseq);
      
      cmd = cmd->GetNext();
   }
   
   cmdseq.append("\n");
   
   return cmdseq;
}


//------------------------------------------------------------------------------
// void GetSubCommands(GmatCommand* brCmd, Integer level, std::string &cmdseq)
//------------------------------------------------------------------------------
void GmatCommandUtil::GetSubCommands(GmatCommand* brCmd, Integer level,
                                     std::string &cmdseq)
{
   GmatCommand* current = brCmd;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   
   while((child = current->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      while ((nextInBranch != NULL) && (nextInBranch != current))
      {
         for (int i=0; i<=level; i++)
         {
            cmdseq.append("---");
         }
         
         cmdseq.append("--- " + nextInBranch->GetTypeName() + "\n");
         
         if (nextInBranch->GetChildCommand() != NULL)
            GetSubCommands(nextInBranch, level+1, cmdseq);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
}

