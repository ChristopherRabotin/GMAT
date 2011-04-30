//$Id$
//------------------------------------------------------------------------------
//                                  PrintUtility
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2004/11/23
//
/**
 * Contains static methods for printing, including a method that prints out the
 * entire sequence, given the first command.
 *
 * @note - Currently prints to standard output only.
 */
//------------------------------------------------------------------------------


#ifndef PrintUtility_hpp
#define PrintUtility_hpp


#include "gmatdefs.hpp"
#include "GmatCommand.hpp"

// No GMAT_API here because this class is used in the exe, not in a DLL
class PrintUtility
{
public:

   static PrintUtility* Instance();
   bool PrintEntireSequence(GmatCommand* firstCmd);
   ~PrintUtility();

protected:

private:
   PrintUtility();
   PrintUtility(const PrintUtility &pu);
   PrintUtility& operator=(const PrintUtility &pu);
   
   bool PrintBranch(GmatCommand* brCmd, Integer level);
   
   static PrintUtility* onlyInstance;

};
#endif // PrintUtility_hpp
