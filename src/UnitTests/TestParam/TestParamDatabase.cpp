//$Header: /cygdrive/p/dev/cvs/test/TestParam/TestParamDatabase.cpp,v 1.1 2005/01/07 20:36:08 ljun Exp $
//------------------------------------------------------------------------------
//                                  TestParamDatabase
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2004/09/16
//
/**
 * Test driver for parameter database.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "TimeParameters.hpp"
#include "CartesianParameters.hpp"
#include "ParameterDatabase.hpp"
#include "BaseException.hpp"

#include <iostream>
#include <iomanip>

using namespace std;

//------------------------------------------------------------------------------
// void WriteParameter(const std::string &name, Parameter *param)
//------------------------------------------------------------------------------
void WriteParameter(const std::string &name, Parameter *param)
{
   if (param != NULL)
   {
      cout << "type = " << param->GetTypeName()
           << ", name = " << param->GetName()
           << ", value = " << param->ToString() << endl;
   }
   else
   {
      cout << "name = " << name << " parameter is NULL\n";
   }
}

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{

   const std::string *descs;
   std::string *vals;
   Integer size;
   bool boolVal;
   Real realVal;
   Rvector6 r6Val;
   
   cout << endl;
   cout << "============================== test ParameterDatabase" << endl;
   ParameterDatabase paramdb;
   Integer numParam;
   StringArray paramNames;
   StringArray paramDescs;
   Parameter *param;
   
   CurrA1MJD *paramCurrA1MJD = new CurrA1MJD("paramCurrentTime");
   ElapsedDays *paramElapsedDays = new ElapsedDays("paramElapsedDays");
   ElapsedSecs *paramElapsedSecs = new ElapsedSecs("paramElapsedSecs");
   CartX *paramCartX = new CartX("paramCartX");
   CartY *paramCartY = new CartY("paramCartY");
   CartZ *paramCartZ = new CartZ("paramCartZ");
   
   cout << "-------------------- test paramdb.Add()" << endl;
   paramdb.Add(paramCurrA1MJD);
   paramdb.Add(paramElapsedDays);
   paramdb.Add(paramElapsedSecs);
   paramdb.Add(paramCartX);
   paramdb.Add(paramCartY);
   paramdb.Add(paramCartZ);
   paramdb.Add("paramCartVX");
   paramdb.Add("paramCartVY");
   paramdb.Add("paramCartVZ");
   
   cout << "-------------------- test exception" << endl;
   cout << "-------------------- test paramdb.Add() the same name" << endl;
   
   try
   {
      paramdb.Add(paramCurrA1MJD);
   }
   catch (BaseException &e)
   {
      cout << e.GetMessage() << endl;
   }
   
   cout << "-------------------- test paramdb.Remove() not existing name" << endl;
   try
   {
      paramdb.Remove("Unknown");
   }
   catch (BaseException &e)
   {
      cout << e.GetMessage() << endl;
   }
   
   numParam = paramdb.GetNumParameters();
   cout << "numParam = " << numParam << endl;
   paramNames = paramdb.GetNamesOfParameters();
   //paramDescs = paramdb.GetDescsOfParameters();

   for (int i=0; i<numParam; i++)
   {
      param = paramdb.GetParameter(paramNames[i]);
      WriteParameter(paramNames[i], param);
   }
   
   cout << "-------------------- test Remove(paramCurrA1MJD)" << endl;
   paramdb.Remove(paramCurrA1MJD);
   numParam = paramdb.GetNumParameters();
   cout << "numParam = " << numParam << endl;
   paramNames = paramdb.GetNamesOfParameters();
   //paramDescs = paramdb.GetDescsOfParameters();
   
   for (int i=0; i<numParam; i++)
   {
      param = paramdb.GetParameter(paramNames[i]);
      WriteParameter(paramNames[i], param);
   }      

   cout << "-------------------- test GetFirstParameterName()" << endl;
   cout << paramdb.GetFirstParameterName() << endl;
   
   delete paramCurrA1MJD;
   delete paramElapsedDays;
   delete paramElapsedSecs;
   delete paramCartX;
   delete paramCartY;
   delete paramCartZ;

   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();

}
