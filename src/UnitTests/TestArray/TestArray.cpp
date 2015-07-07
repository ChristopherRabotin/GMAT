//$Header$
//------------------------------------------------------------------------------
//                                  TestArray
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2007/09/26
//
/**
 * Test driver for testing array.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"

using namespace std;

int GetCharArray(TestOutput &out, int numChar, char outArray[], std::string &outStr)
{
   char charArray[20];
   charArray[0] = 'H';
   charArray[1] = 'a';
   charArray[2] = 'p';
   charArray[3] = 'p';
   charArray[4] = 'y';
   std::string str;
   str.assign(charArray, numChar);
   out.Put("str = ", str);
   
   outStr = str;
}


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   char chArray[20];
   std::string outStr;
   GetCharArray(out, 5, chArray, outStr);
   out.Put("outStr = ", outStr);
   
   GetCharArray(out, 3, chArray, outStr);
   out.Put("outStr = ", outStr);
   
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   MessageInterface::SetLogFile("../../../test/TestUtil/GmatLog.txt");
   TestOutput out("../../../test/TestUtil/TestArrayOut.txt");
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of Array!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
