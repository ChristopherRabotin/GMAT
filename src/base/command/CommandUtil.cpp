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
// GmatCommand* GetNextCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
/*
 * Returns next non-ScriptEvent command.
 *
 * @param  cmd  Command which search begins from
 * @return next non-ScriptEvent command
 */
//------------------------------------------------------------------------------
GmatCommand* GmatCommandUtil::GetNextCommand(GmatCommand *cmd)
{
   if (cmd->GetTypeName() != "BeginScript")
      return cmd->GetNext();
   
   GmatCommand *current = cmd->GetNext();
   
   while (current != NULL)
   {
      if (current->GetTypeName() == "EndScript")
         break;
      
      current = current->GetNext();
   }
   
   return current->GetNext();
}


//------------------------------------------------------------------------------
// std::string GetCommandSeqString(GmatCommand *cmd)
//------------------------------------------------------------------------------
std::string GmatCommandUtil::GetCommandSeqString(GmatCommand *cmd)
{
   char buf[10];
   std::string cmdseq;
   cmdseq.append("\n---------- Mission Sequence ----------\n");
   
   while (cmd != NULL)
   {
      sprintf(buf, "(%p)", cmd);
      cmdseq.append("--- " + std::string(buf) + cmd->GetTypeName() + "\n");
      
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
   char buf[10];
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
         
         sprintf(buf, "(%p)", nextInBranch);
         cmdseq.append("--- " + std::string(buf) + nextInBranch->GetTypeName() + "\n");
         
         if (nextInBranch->GetChildCommand() != NULL)
            GetSubCommands(nextInBranch, level+1, cmdseq);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
}

