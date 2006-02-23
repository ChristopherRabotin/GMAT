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
   std::string GetCommandSeqString(GmatCommand *cmd);
   void GetSubCommands(GmatCommand* brCmd, Integer level, std::string &cmdseq);
}

#endif // CommandUtil_hpp
