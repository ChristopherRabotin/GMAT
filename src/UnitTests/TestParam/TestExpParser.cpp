//$Header: /cygdrive/p/dev/cvs/test/TestParam/TestExpParser.cpp,v 1.1 2005/01/07 20:35:31 ljun Exp $
//------------------------------------------------------------------------------
//                                  TestExpParser
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2003/09/18
//
/**
 * Test driver for parameters
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ExpressionParser.hpp"
#include "BaseException.hpp"

#include <iostream>
#include <iomanip>

using namespace std;

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{

   char expstr[80];

   cout << "Enter a period to stop.\n";

   ExpressionParser *ep = ExpressionParser::Instance();

   if (ep == NULL)
   {
      cout << "parser is NULL\n";
      cout << "Hit enter to end" << endl;
      cin.get();
      return 0;
   }

   for (;;)
   {
      cout << "Enter expression: ";
      cin.getline(expstr, 79);
      
      if (*expstr == '.')
         break;

      try
      {
         cout << "Answer is: " << ep->EvalExp(expstr) << "\n\n";
      }
      catch (BaseException &e)
      {
         cout << e.GetMessage();
      }
   }

   return 0;
}
