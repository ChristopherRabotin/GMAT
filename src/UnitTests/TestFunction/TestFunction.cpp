//$Id$
//------------------------------------------------------------------------------
//                                  TestFunction
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2008/08/19
//
/**
 * Test driver for GmatFunction.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "StringUtil.hpp"
#include "UtilityException.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;
using namespace GmatStringUtil;


//------------------------------------------------------------------------------
// int Factorial(int n)
//------------------------------------------------------------------------------
int Factorial(int n)
{
   if (n <= 1)
      return 1;
   else
      return n * Factorial(n-1);
   
}

//------------------------------------------------------------------------------
// int FactorialA(int n)
//------------------------------------------------------------------------------
int FactorialA(int n)
{
   if (n <= 1)
   {
      return 1;
   }
   else
   {
      int tmp = Factorial(n-1);
      return n * tmp;
   }
}

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   MessageInterface::SetLogFile("../../TestFunction/GmatLog.txt");
   MessageInterface::ShowMessage("=========== TestFunction\n");
   
   Integer ival, expIval;
   
   out.Put("");
   
   out.Put("============================== test Factorial(2)");
   //---------------------------------------------
   
   ival = Factorial(2);
   expIval = 2;
   out.Validate(ival, expIval);
   
   out.Put("============================== test Factorial(6)");
   //---------------------------------------------
   
   ival = Factorial(6);
   expIval = 720;
   out.Validate(ival, expIval);
   
   out.Put("============================== test FactorialA(6)");
   //---------------------------------------------
   
   ival = FactorialA(6);
   expIval = 720;
   out.Validate(ival, expIval);
   
   return 0;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string outPath = "../../TestFunction/";
   std::string outFile = outPath + "TestFunctionOut.txt";   
   TestOutput out(outFile);
   
   char *buffer;
   buffer = getenv("OS");
   if (buffer  != NULL)
      printf("Current OS is %s\n", buffer);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of GMAT Function!!");
   }
   catch (UtilityException &e)
   {
      out.Put(e.GetFullMessage());
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
