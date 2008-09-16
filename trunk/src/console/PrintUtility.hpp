//$Header$
//------------------------------------------------------------------------------
//                                  PrintUtility
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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

class GMAT_API PrintUtility
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
