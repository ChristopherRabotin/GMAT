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
#include "MessageInterface.hpp"

//#define DEBUG_MATCHING_END 1
//#define DEBUG_GET_PARENT 1
//#define DEBUG_COMMAND_SEQ_STRING 1

//------------------------------------------------------------------------------
// GmatCommand* GetLastCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
GmatCommand* GmatCommandUtil::GetLastCommand(GmatCommand *cmd)
{
   //GmatCommand *prevCmd = NULL;
   GmatCommand *prevCmd = cmd;
   
   while (cmd != NULL)
   {
      #if DEBUG_GET_LAST
      ShowCommand("===> GmatCommandUtil::GetLastCommand() cmd = ", cmd);
      #endif
      
      cmd = cmd->GetNext();
      
      if (cmd != NULL)
         prevCmd = cmd;
   }
   
   return prevCmd;
}


//------------------------------------------------------------------------------
// GmatCommand* GetParentCommand(GmatCommand *top, GmatCommand *cmd)
//------------------------------------------------------------------------------
GmatCommand* GmatCommandUtil::GetParentCommand(GmatCommand *top, GmatCommand *cmd)
{
   #if DEBUG_GET_PARENT
   ShowCommand
      ("===> GmatCommandUtil::GetParentCommand() top = ", top, ", cmd = ", cmd);
   #endif
   
   GmatCommand *current = top;
   GmatCommand *parent = NULL;
   
   while (current != NULL)
   {
      #if DEBUG_GET_PARENT
      ShowCommand("     current = ", current);
      #endif
      
      if (current == cmd)
      {
         parent = top;
         break;
      }
      
      if ((current->GetChildCommand(0)) != NULL)
      {
         parent = GetSubParent(current, cmd);
         
         #if DEBUG_GET_PARENT
         ShowCommand("     parent = ", current);
         #endif
         
         if (parent != NULL)
            break;
      }
      
      current = current->GetNext();
   }
   
   #if DEBUG_GET_PARENT
   ShowCommand("===> GmatCommandUtil::GetParentCommand() returning ", parent);
   #endif
   
   return parent;
}


//------------------------------------------------------------------------------
// GmatCommand* GetSubParent(GmatCommand *brCmd, GmatCommand *cmd)
//------------------------------------------------------------------------------
GmatCommand* GmatCommandUtil::GetSubParent(GmatCommand *brCmd, GmatCommand *cmd)
{
   #if DEBUG_GET_PARENT
   ShowCommand
      ("     GmatCommandUtil::GetSubParent() brCmd = ", brCmd, ", cmd = ", cmd);
   #endif
   
   GmatCommand *current = brCmd;
   GmatCommand *child;
   GmatCommand *subParent = NULL;
   Integer childNo = 0;
   
   while((child = current->GetChildCommand(childNo)) != NULL)
   {
      while ((child != NULL) && (child != current))
      {
         if (child == cmd)
         {
            #if DEBUG_GET_PARENT
            ShowCommand
               ("     GmatCommandUtil::GetSubParent() returning ", current);
            #endif
            return current;
         }
         
         if (child->GetChildCommand() != NULL)
            subParent = GetSubParent(child, cmd);
         
         if (subParent != NULL)
            return subParent;
         
         child = child->GetNext();
         
         #if DEBUG_GET_PARENT
         ShowCommand
            ("     GmatCommandUtil::GetSubParent() child = ", child);
         #endif
      }
      
      ++childNo;
   }
   
   #if DEBUG_GET_PARENT
   MessageInterface::ShowMessage
      ("     GmatCommandUtil::GetSubParent() returning NULL\n");
   #endif
   
   return NULL;
}


//------------------------------------------------------------------------------
// GmatCommand* GetNextCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
/*
 * Returns next non-ScriptEvent command. If command is BeginScript, it will
 * return next command of matching EndScrpt.
 *
 * @param  cmd  Command which search begins from
 * @return next non-ScriptEvent command
 */
//------------------------------------------------------------------------------
GmatCommand* GmatCommandUtil::GetNextCommand(GmatCommand *cmd)
{
   if (cmd == NULL)
      return NULL;
   
   if (cmd->GetTypeName() != "BeginScript")
      return cmd->GetNext();
   
   GmatCommand *endScript = GetMatchingEnd(cmd);
   
   if (endScript == NULL)
      return NULL;
   else
      return endScript->GetNext();
   
}


//------------------------------------------------------------------------------
// GmatCommand* GetPreviousCommand(GmatCommand *cmd)
//------------------------------------------------------------------------------
GmatCommand* GmatCommandUtil::GetPreviousCommand(GmatCommand *cmd)
{
   if (cmd == NULL)
      return NULL;
   
   GmatCommand *current = cmd->GetNext();
   GmatCommand *prevCmd = NULL;
   
   while (current != NULL)
   {
      if (current == cmd)
         break;

      prevCmd = current;
      current = current->GetNext();
   }
   
   return prevCmd;
}


//------------------------------------------------------------------------------
// GmatCommand* GetMatchingEnd(GmatCommand *cmd)
//------------------------------------------------------------------------------
/*
 * Returns EndScript of matching BeginScrpt.
 *
 * @param  cmd  BeginScript command which search begins from
 * @return next Matching EndScript, NULL if matching EndScript not found
 */
//------------------------------------------------------------------------------
GmatCommand* GmatCommandUtil::GetMatchingEnd(GmatCommand *cmd)
{
   if (cmd == NULL)
      return NULL;

   #if DEBUG_MATCHING_END
   ShowCommand
      ("===> GmatCommandUtil::GetMatchingEnd() cmd = ", cmd,
      cmd->GetTypeName().c_str());
   #endif
   
   if (cmd->GetTypeName() != "BeginScript" && !cmd->IsOfType("BranchCommand"))
      return NULL;
   
   GmatCommand *current = cmd;

   if (cmd->GetTypeName() == "BeginScript")
   {
      Integer scriptEventCount = 0;
   
      while (current != NULL)
      {
         if (current->GetTypeName() == "BeginScript")
            scriptEventCount++;
      
         if (current->GetTypeName() == "EndScript")
            scriptEventCount--;

         #if DEBUG_MATCHING_END
         MessageInterface::ShowMessage
            ("     scriptEventCount=%d, current=(%p)%s\n", scriptEventCount, current,
             current->GetTypeName().c_str());
         #endif
      
         if (scriptEventCount == 0)
            break;
      
         current = current->GetNext();
      }
      
      #if DEBUG_MATCHING_END
      ShowCommand("===> GmatCommandUtil::GetMatchingEnd() returning ", current);
      #endif
   
      return current;
   }
   else
   {
      GmatCommand *child = NULL;
      if ((current->GetChildCommand(0)) != NULL)
      {
         child = current->GetChildCommand(0);
            
         while (child != NULL)
         {
            if (child->IsOfType("BranchEnd"))
               break;
            
            child = child->GetNext();
         }
      }
      
      #if DEBUG_MATCHING_END
      ShowCommand("===> GmatCommandUtil::GetMatchingEnd() returning ", child);
      #endif
      
      return child;
   }
}


//------------------------------------------------------------------------------
// std::string GetCommandSeqString(GmatCommand *cmd)
//------------------------------------------------------------------------------
std::string GmatCommandUtil::GetCommandSeqString(GmatCommand *cmd)
{
   char buf[10];
   GmatCommand *current = cmd;
   std::string cmdseq, cmdstr;
   cmdstr = "\n---------- Mission Sequence ----------\n";
   cmdseq.append(cmdstr);

   #if DEBUG_COMMAND_SEQ_STRING
   MessageInterface::ShowMessage
      ("===> GmatCommandUtil::GetCommandSeqString(%p)\n", cmd);
   MessageInterface::ShowMessage("%s", cmdstr.c_str());
   #endif
   
   
   while (current != NULL)
   {      
      sprintf(buf, "(%p)", current);
      cmdstr = "--- " + std::string(buf) + current->GetTypeName() + "\n";
      cmdseq.append(cmdstr);
      
      #if DEBUG_COMMAND_SEQ_STRING
      MessageInterface::ShowMessage("%s", cmdstr.c_str());
      #endif
      
      if ((current->GetChildCommand(0)) != NULL)
         GetSubCommandString(current, 0, cmdseq);
      
      current = current->GetNext();
   }
   
   cmdseq.append("\n");
   
   return cmdseq;
}


//------------------------------------------------------------------------------
// void GetSubCommandString(GmatCommand* brCmd, Integer level, std::string &cmdseq)
//------------------------------------------------------------------------------
void GmatCommandUtil::GetSubCommandString(GmatCommand* brCmd, Integer level,
                                         std::string &cmdseq)
{
   char buf[10];
   GmatCommand* current = brCmd;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   std::string cmdstr;
   
   while((child = current->GetChildCommand(childNo)) != NULL)
   {
      nextInBranch = child;
      
      while ((nextInBranch != NULL) && (nextInBranch != current))
      {
         for (int i=0; i<=level; i++)
         {
            cmdseq.append("---");
            
            #if DEBUG_COMMAND_SEQ_STRING
            MessageInterface::ShowMessage("---");
            #endif
         }
         
         sprintf(buf, "(%p)", nextInBranch);
         cmdstr = "--- " + std::string(buf) + nextInBranch->GetTypeName() + "\n";
         cmdseq.append(cmdstr);
         
         #if DEBUG_COMMAND_SEQ_STRING
         MessageInterface::ShowMessage("%s", cmdstr.c_str());
         #endif
         
         if (nextInBranch->GetChildCommand() != NULL)
            GetSubCommandString(nextInBranch, level+1, cmdseq);
         
         nextInBranch = nextInBranch->GetNext();
      }
      
      ++childNo;
   }
}


//------------------------------------------------------------------------------
// bool IsAfter(GmatCommand *cmd1, GmatCommand *cmd2)
//------------------------------------------------------------------------------
/*
 * Returns true if cmd1 is after cmd2 in the sequence.
 *
 * @param  cmd1  First command
 * @param  cmd2  Second command
 *
 * @return true if cmd1 is after cmd2 in the sequence, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatCommandUtil::IsAfter(GmatCommand *cmd1, GmatCommand *cmd2)
{
   if (cmd1 == NULL || cmd2 == NULL)
      return false;
   
   GmatCommand *current = cmd2;
   
   while (current != NULL)
   {
      if (current == cmd1)
         return true;
      
      current = current->GetNext();
   }
   
   return false;
}


//------------------------------------------------------------------------------
// void ShowCommand(const std::string &title1, GmatCommand *cmd1,
//                   const std::string &title2, GmatCommand *cmd2)
//------------------------------------------------------------------------------
void GmatCommandUtil::ShowCommand(const std::string &title1, GmatCommand *cmd1,
                                  const std::string &title2, GmatCommand *cmd2)
{
   if (title2 == "")
   {
      if (cmd1 == NULL)
         MessageInterface::ShowMessage("%s(%p)NULL\n", title1.c_str(), cmd1);
      else
         MessageInterface::ShowMessage
            ("%s(%p)%s\n", title1.c_str(), cmd1, cmd1->GetTypeName().c_str());
   }
   else
   {
      if (cmd2 == NULL)
         MessageInterface::ShowMessage
            ("%s(%p)NULL%s(%p)NULL\n", title1.c_str(), cmd1, title2.c_str(), cmd2);
      else
         MessageInterface::ShowMessage
            ("%s(%p)%s%s(%p)%s\n", title1.c_str(), cmd1, cmd1->GetTypeName().c_str(),
             title2.c_str(), cmd2, cmd2->GetTypeName().c_str());
   }
}

