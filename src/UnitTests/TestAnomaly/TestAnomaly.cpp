//$Id$
//------------------------------------------------------------------------------
//                              TestAnomaly
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// Author: Linda Jun
// Created: 2005/02/14
// Modification:
//   2005/02/14  L. Jun - Moved element type related test cases from TestUtil.cpp
//
/**
 * Test driver for converting Anomaly between different types (TA, MA, EA).
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include "gmatdefs.hpp"
#include "Anomaly.hpp"
#include "TestOutput.hpp"

using namespace std;


//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{

   out.Put("\n============================== test Anomaly");
   
   Real sma = 10000;
   Real ecc = 0.2;
   Real IincNC = 28;
   Real raan = 45;
   Real aop = 45;
   Real ta = 45;
   Real realVal = 0.0;
   Real expVal = 0.0;
   
   Anomaly anomalyTa(sma, ecc, ta, Anomaly::TA);
   realVal = anomalyTa.GetValue(Anomaly::MA);
   out.Put("=============== Convert MA\n");
   expVal = 30.415998354;
   out.Validate(realVal, expVal);
   
   realVal = anomalyTa.GetValue(Anomaly::EA);
   out.Put("=============== Convert EA\n");
   expVal = 37.371480701;
   out.Validate(realVal, expVal);
   
   //-----------------------------------------------------------------
   //ta = 356.055279734     ma = 357.422640892     ea = 356.778725303
   //-----------------------------------------------------------------
   ta = 356.055279734;
   anomalyTa.SetValue(ta);
   
   realVal = anomalyTa.GetValue(Anomaly::MA);
   out.Put("=============== Convert MA\n");
   expVal = 357.422640892;
   out.Validate(realVal, expVal);
   
   realVal = anomalyTa.GetValue(Anomaly::EA);
   out.Put("=============== Convert EA\n");
   expVal = 356.778725303;
   out.Validate(realVal, expVal);
   
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("..\\..\\Test\\TestUtil\\TestAnomaly\\TestAnomaly.out");
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of Anomaly conversion!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
