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
 * entire sequence, given the first command - Implementation.
 *
 * @note - Currently prints to standard output only.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatCommand.hpp"
#include "PrintUtility.hpp"

#include <iostream>
using namespace std;

PrintUtility* PrintUtility::onlyInstance = NULL;

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Instance()
//------------------------------------------------------------------------------
/**
 * Returns (and creates initially) the only instance allowed for this singleton
 * class. 
 *
 * @return Instance of the PrintUtility.
 */
//------------------------------------------------------------------------------
PrintUtility* PrintUtility::Instance()
{
   if (onlyInstance == NULL)
      onlyInstance = new PrintUtility();

   return onlyInstance;
}

bool PrintUtility::PrintEntireSequence(GmatCommand* firstCmd)
{
   cout << ".................... Print out the whole sequence ........................................" << endl;
   GmatCommand *current = firstCmd;
   while (current != NULL)
   {
      cout << "   Command::" << current->GetTypeName() << endl;
      if ((current->GetChildCommand(0))!=NULL)
         PrintBranch(current, 0);
      current = current->GetNext();
   }
   cout << ".................... End sequence ........................................................" << endl;
   return true;
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

//------------------------------------------------------------------------------
//  PrintUtility()
//------------------------------------------------------------------------------
/**
 * Constructs a PrintUtility object (default constructor).
 */
//------------------------------------------------------------------------------
PrintUtility::PrintUtility()
{
}

// copy constructor and operator= not implemented

bool PrintUtility::PrintBranch(GmatCommand* brCmd, Integer level)
{
   GmatCommand* current = brCmd;
   Integer childNo = 0;
   GmatCommand* nextInBranch;
   GmatCommand* child;
   
   while((child = current->GetChildCommand(childNo))!=NULL)
   {
      nextInBranch = child;
      while ((nextInBranch != NULL) && (nextInBranch != current))
      {
         cout << "   ";
         for (int i = 0; i <= level ; i++)
            cout << "...";
         cout << " branch " << childNo << "::" << nextInBranch->GetTypeName() << endl;
         if (nextInBranch->GetChildCommand() != NULL)
         {
            PrintBranch(nextInBranch, level+1);
         }
         nextInBranch = nextInBranch->GetNext();
      }
      ++childNo;
   }
   
   return true;
}

