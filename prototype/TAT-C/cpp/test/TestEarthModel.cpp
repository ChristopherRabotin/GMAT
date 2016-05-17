//$Id$
//------------------------------------------------------------------------------
//                               TestEarthModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2016.05.11
//
/**
 * Unit-test driver for the Earth model
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Rvector6.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "AbsoluteDate.hpp"
#include "OrbitState.hpp"
#include "Earth.hpp"
#include "TimeTypes.hpp"


using namespace std;
using namespace GmatMathConstants;

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string outFormat = "%16.9f ";
   Real        tolerance = 1e-15;
   
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   std::string outPath = "./";
   MessageInterface::SetLogFile(outPath + "GmatLog.txt");
   MessageInterface::ShowMessage("%s\n",
                                 GmatTimeUtil::FormatCurrentTime().c_str());
   
   // Set global format setting
   GmatGlobal *global = GmatGlobal::Instance();
   global->SetActualFormat(false, false, 16, 1, false);
   
   char *buffer = NULL;
   buffer = getenv("OS");
   if (buffer  != NULL)
   {
      MessageInterface::ShowMessage("Current OS is %s\n", buffer);
   }
   else
   {
      MessageInterface::ShowMessage("Buffer is NULL\n");
   }
   
   MessageInterface::ShowMessage("*** START TEST ***\n");
   
   try
   {
      // Test the AbsoluteDate
      MessageInterface::ShowMessage("*** TEST*** EarthModel\n");
      
      // Create the Earth
      Earth earth;
      Real GMT = earth.ComputeGMT(2457260.12345679);
      MessageInterface::ShowMessage("Calculated GMT = %12.10f\n", GMT);
      
      //  Need to verify the sign. (compares hi-fi output from GMAT/STK with
      //  low-fi model in TAT-C, hence the loose tolerance
      if (GmatMathUtil::Abs(GMT*180/PI - 198.002628503035)/198.002628503035 >= 1e-5)
         MessageInterface::ShowMessage("*** ERROR - error calculating GMT!!\n");
      else
         MessageInterface::ShowMessage("OK - GMT calculation is OK!\n");
      
      cout << endl;
      cout << "Hit enter to end" << endl;
      cin.get();
      
      MessageInterface::ShowMessage("*** END TEST ***\n");
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n", be.GetFullMessage().c_str());
   }
   
}