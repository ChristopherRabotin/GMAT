//------------------------------------------------------------------------------
//                               SystemTest_Analysis
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Wendy Shoan
// Created: 2016.05.31
//
/**
 * System tester for analysis
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <ctime>
#include <cmath>
#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "Rvector6.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "AbsoluteDate.hpp"
#include "Spacecraft.hpp"
#include "Earth.hpp"
#include "KeyValueStatistics.hpp"
#include "VisiblePOIReport.hpp"
#include "OrbitState.hpp"
#include "PointGroup.hpp"
#include "Propagator.hpp"
#include "ConicalSensor.hpp"
#include "CoverageChecker.hpp"
#include "TimeTypes.hpp"


using namespace std;
using namespace GmatMathUtil;
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
      // Test the PointGroup
      MessageInterface::ShowMessage("*** TEST*** Analysis!!!!\n");
      
      // This is a usage example that drives O-C code and computes standard
      //  statistical products typical of O-C analysis.  This script is an example
      //  of how R-M might use the O-C data.
      
      AbsoluteDate  *date;
      OrbitState    *state;
      ConicalSensor *sensor;
      Spacecraft    *sat1;
      Propagator    *prop;
      PointGroup    *pGroup;
      std::vector<VisiblePOIReport> coverageEvents;

      
      clock_t t0 = clock();
      Integer numIter = 1;

      for (Integer ii = 0; ii < numIter; ii++) // **********
      {
      
      bool showPlots = false;

      // Create the epoch object and set the initial epoch
      date = new AbsoluteDate();
      date->SetGregorianDate(2017,1,15,22,30,20.111);
      
//      MessageInterface::ShowMessage(" --- date created\n");
      
      // Create the spacecraft state object and set Keplerian elements
      state = new OrbitState();
      state->SetKeplerianState(6700.0,0.002,90.0*RAD_PER_DEG,
                               PI/4.0 + PI/6.0,0.2345,PI/6.0);
      
//      MessageInterface::ShowMessage(" --- state created\n");
      
      // Create a conical sensor
      sensor = new ConicalSensor(PI);
      
//      MessageInterface::ShowMessage(" --- conical sensor created\n");
      
      // Create a spacecraft giving it a state and epoch
      sat1 = new Spacecraft(date,state);
      sat1->AddSensor(sensor);
      
//      MessageInterface::ShowMessage(" --- spacecraft created\n");
      
      // Create the propagator
      prop = new Propagator(sat1);
      
//      MessageInterface::ShowMessage(" --- propagator created\n");
      
      // Create the point group and initialize the coverage checker
      pGroup = new PointGroup();
      pGroup->AddHelicalPointsByNumPoints(200);
      CoverageChecker *covChecker = new CoverageChecker(pGroup,sat1);
      
//      MessageInterface::ShowMessage(" --- point group created\n");

      if (showPlots)
         ; // don't have this in C++
//         figHandle = pGroup.PlotAllTestPoints();
  
      // Propagate for a duration and collect data
      Real    startDate = date->GetJulianDate();
      Integer count     = 0;
      // Over 1 day
      while (date->GetJulianDate() < ((Real)startDate + 1.0))
      {
//         MessageInterface::ShowMessage(" --- about to advance the time and propagate\n");
         // Propagate
         date->Advance(120.0);
         prop->Propagate(*date);
//         MessageInterface::ShowMessage(" --- done advancing the time and propagating\n");
      
         // Compute points in view
         IntegerArray accessPoints = covChecker->AccumulateCoverageData();
      
         // Plotting stuff used for testing and visualization
         if (showPlots)
         {
            ;
//            // Save orbit for plotting. Kludge cause output classes not done yet.
//            count++;
//            Rvector6 intertialState = sat1->GetCartesianState();
//            Real     jDate          = sat1.GetJulianDate();
//            Earth *cb = new Earth();
//            orbitState(count,:) = cb.GetBodyFixedState(intertialState(1:3,1),jDate)';
//            pGroup.PlotSelectedTestPoints(accessPoints); drawnow;
//            if count >= 2
//               figure(figHandle); hold on;
//            plot3(orbitState(count-1:count,1)/6378,orbitState(count-1:count,2)/6378,...
//                  orbitState(count-1:count,3)/6378,'k-','MarkerSize',3)
         }
      }
      
//      MessageInterface::ShowMessage(" --- propagation completed\n");
      
      // Compute coverage data
      coverageEvents = covChecker->ProcessCoverageData();
      
//      MessageInterface::ShowMessage(" --- ProcessCoverageData completed (%d reports)\n",
//                                    (Integer) coverageEvents.size());

      } // *****
      Real timeSpent = ((Real) (clock() - t0)) / CLOCKS_PER_SEC;
      MessageInterface::ShowMessage("TIME SPENT in %d iterations is %12.10f seconds\n",
                                    numIter,timeSpent);

      
      RealArray lonVec;
      RealArray latVec;
      // Compute coverate stats.  Shows how R-M might use data for coverage analysis
      // Create Lat\Lon Grid
      for (Integer pointIdx = 0; pointIdx < pGroup->GetNumPoints(); pointIdx++)
      {
         Rvector3 *vec = pGroup->GetPointPositionVector(pointIdx);
         lonVec.push_back(ATan(vec->GetElement(1),vec->GetElement(0))*DEG_PER_RAD);
         latVec.push_back(ASin(vec->GetElement(2)/vec->GetMagnitude())*DEG_PER_RAD);
      }
      
      MessageInterface::ShowMessage(" --- lat/long set-up completed\n");
      
      // Compute total coverage statistics from all coverage events
      Rvector totalCoverageDuration(pGroup->GetNumPoints());
      IntegerArray numPassVec;  // (pGroup->GetNumPoints());
      for (Integer ii = 0; ii < pGroup->GetNumPoints(); ii++)
         numPassVec.push_back(0);
      Rvector minPassVec(pGroup->GetNumPoints());
      Rvector maxPassVec(pGroup->GetNumPoints());
      for (Integer eventIdx = 0; eventIdx < coverageEvents.size(); eventIdx++)
      {
         VisiblePOIReport currEvent      = coverageEvents.at(eventIdx);
         Integer          poiIndex       = currEvent.GetPOIIndex();
         Real             eventDuration  = (currEvent.GetEndDate().GetJulianDate() -
                                            currEvent.GetStartDate().GetJulianDate()) * 24.0;
         totalCoverageDuration(poiIndex) = totalCoverageDuration(poiIndex) + eventDuration;
//         Rvector3 vec = pGroup->GetPointPositionVector(pointIdx); // ?
         
//      %     lat = atan2(vec(2),vec(1))*180/pi;
//      %     lon = asin(vec(3)/norm(vec));
//      %     Z(lat,lon) = totalCoverageDuration;
      
      // Save the maximum duration if necessary
         if (eventDuration > maxPassVec(poiIndex))
            maxPassVec(poiIndex) = eventDuration;

      
         if (minPassVec(poiIndex) == 0 ||  (eventDuration< maxPassVec(poiIndex)))
            minPassVec(poiIndex) = eventDuration;

         numPassVec.at(poiIndex) = numPassVec.at(poiIndex) + 1;
      }
      
      // *********** display stuff ***********
      // Write the simple coverage report to the MATLAB command window
      MessageInterface::ShowMessage("       =======================================================================\n");
      MessageInterface::ShowMessage("       ==================== Brief Coverage Analysis Report ===================\n");
      MessageInterface::ShowMessage("       lat (deg): Latitude of point in degrees                  \n");
      MessageInterface::ShowMessage("       lon (deg): Longitude of point in degrees                  \n");
      MessageInterface::ShowMessage("       numPasses: Number of total passes seen by a point                           \n");
      MessageInterface::ShowMessage("       totalDur : Total duration point was observed in hours                         \n");
      MessageInterface::ShowMessage("       minDur   : Duration of the shortest pass in minutes                         \n");
      MessageInterface::ShowMessage("       maxDur   : Duration of the longest pass in hours                            \n");
      MessageInterface::ShowMessage("       =======================================================================\n");
      MessageInterface::ShowMessage("       =======================================================================\n");
      MessageInterface::ShowMessage("  ");
      
//      data = [latVec,lonVec, numPassVec, totalCoverageDuration, minPassVec, maxPassVec];  <<<<<<<<<
      Integer headerCount = 1;
      Integer dataEnd     = 0;
      for (Integer passIdx = 0; passIdx < pGroup->GetNumPoints(); passIdx+= 10)
      {
         MessageInterface::ShowMessage("       lat (deg)     lon (deg)       numPasses  totalDur    minDur      maxDur\n");
         dataEnd = passIdx + 10;  // 9;
         for (Integer ii = 0; ii < 10; ii++)  // 9
            MessageInterface::ShowMessage("       %le    %le    %d    %le    %le    %le \n",
//            MessageInterface::ShowMessage("       %12.10f    %12.10f    %d    %12.10f    %12.10f    %12.10f \n",
                                          latVec.at(passIdx+ii),
                                          lonVec.at(passIdx+ii),
                                          numPassVec.at(passIdx+ii),
                                          totalCoverageDuration(passIdx+ii),
                                          minPassVec(passIdx+ii),
                                          maxPassVec(passIdx+ii));

      }

      if (dataEnd + 1 < pGroup->GetNumPoints())
      {
         MessageInterface::ShowMessage("       lat (deg)    lon (deg)     numPasses   totalDur    minDur      maxDur\n");
         for (Integer ii = dataEnd; ii < pGroup->GetNumPoints(); ii++)
            MessageInterface::ShowMessage("       %le    %le    %d    %le    %le    %le \n",
//            MessageInterface::ShowMessage("       %12.10f    %12.10f    %d    %12.10f    %12.10f    %12.10f \n",
                                          latVec.at(ii),
                                          lonVec.at(ii),
                                          numPassVec.at(ii),
                                          totalCoverageDuration(ii),
                                          minPassVec(ii),
                                          maxPassVec(ii));
      }

      
      cout << endl;
      cout << "Hit enter to end" << endl;
      cin.get();
      
      // delete pointers here
      
      MessageInterface::ShowMessage("*** END TEST ***\n");
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("Exception caught: %s\n", be.GetFullMessage().c_str());
   }
   
}
