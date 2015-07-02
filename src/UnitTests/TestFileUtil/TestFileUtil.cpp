//$Id$
//------------------------------------------------------------------------------
//                                  TestFileUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2005/02/14
//
/**
 * Test driver for StringUtil.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "FileUtil.hpp"
#include "UtilityException.hpp"
#include "TestOutput.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"

using namespace std;
using namespace GmatFileUtil;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   MessageInterface::ShowMessage("=========== TestFileUtil\n");
   
   std::string str, str1;
   Real rval, expRval;
   Integer ival, expIval;
   bool bval;
   StringArray itemNames;
   
   out.Put("");
   
   out.Put("============================== test GmatFileUtil::IsValidFileName()");
   //---------------------------------------------
   str = "validfilename.txt";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, true);
   
   //---------------------------------------------
   str = "withpath/validfilename.txt";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, true);
   
   //---------------------------------------------
   str = ".\\withpath/validfilename.txt";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, true);
   
   //---------------------------------------------
   str = "in*validfilename.txt";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, false);
   
   //---------------------------------------------
   str = "invalidfile?name.txt";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, false);
   
   //---------------------------------------------
   str = "\"invalidfile\\name.txt\"";
   out.Put(str);
   bval = IsValidFileName(str);
   out.Validate(bval, false);
   
   out.Put("============================== test GmatFileUtil::IsPathAbsolute()");
   //---------------------------------------------
   str = "filename.txt";
   out.Put(str);
   bval = IsPathAbsolute(str);
   out.Validate(bval, false);
   
   //---------------------------------------------
   str = "c:/filename.txt";
   out.Put(str);
   bval = IsPathAbsolute(str);
   out.Validate(bval, false);
   
   //---------------------------------------------
   str = "c:/mydir/filename.txt";
   out.Put(str);
   bval = IsPathAbsolute(str);
   out.Validate(bval, false);
   
   //---------------------------------------------
   str = "..\\filename.txt";
   out.Put(str);
   bval = IsPathAbsolute(str);
   out.Validate(bval, false);
   
   return 0;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   //std::string outPath = "../../TestFileUtil/";
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   std::string outFile = outPath + "TestFileUtilOut.txt";   
   TestOutput out(outFile);
   
   #if 0
   char *buffer;
   buffer = getenv("OS");
   if (buffer  != NULL)
      printf("Current OS is %s\n", buffer);
   #endif
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of StingUtil!!");
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
