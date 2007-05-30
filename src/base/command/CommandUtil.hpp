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
#ifndef CommandUtil_hpp
#define CommandUtil_hpp

#include "gmatdefs.hpp"
#include "GmatCommand.hpp"

namespace GmatCommandUtil
{
   GmatCommand* GetLastCommand(GmatCommand *cmd);
   GmatCommand* GetNextCommand(GmatCommand *cmd);
   GmatCommand* GetPreviousCommand(GmatCommand *cmd);
   GmatCommand* GetMatchingEnd(GmatCommand *cmd);
   GmatCommand* GetParentCommand(GmatCommand *top, GmatCommand *cmd);
   GmatCommand* GetSubParent(GmatCommand *brCmd, GmatCommand *cmd);
   std::string GetCommandSeqString(GmatCommand *cmd, bool showAddr = true);
   bool IsAfter(GmatCommand *cmd1, GmatCommand *cmd2);
   void GetSubCommandString(GmatCommand* brCmd, Integer level,
                            std::string &cmdseq, bool showAddr = true);
   void ShowCommand(const std::string &title1, GmatCommand *cmd1,
                    const std::string &title2 = "", GmatCommand *cmd2 = NULL);
}

#endif // CommandUtil_hpp
