//------------------------------------------------------------------------------
//                           CoverageChecker
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.05.10
//
/**
 * Implementation of the the visibility report base class
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "CoverageChecker.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "Rmatrix33.hpp"
#include "TATCException.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const IntegerArray  CoverageChecker::noInts;
const Real          CoverageChecker::noDate = -999.99;


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
CoverageChecker::CoverageChecker(PointGroup *ptGroup, Spacecraft *sat) :
   pointGroup        (ptGroup),
   sc                (sat),
   centralBody       (NULL),
   timeIdx           (-1)  
{
   timeSeriesData.clear();
   dateData.clear();
   numEventsPerPoint.clear();
   pointArray.clear();
   feasibilityTest.clear();

   centralBody = new Earth();
   Integer numPts = pointGroup->GetNumPoints();
   for (Integer ii = 0; ii < numPts; ii++)
   {
//      timeSeriesData.push_back(noInts);
//      dateData.push_back(noDate); // want to accumulate these as we go along
      numEventsPerPoint.push_back(0);
      Rvector3 *ptPos  = pointGroup->GetPointPositionVector(ii);
      Rvector3 posUnit = ptPos->GetUnitVector();
      pointArray.push_back(&posUnit);
      feasibilityTest.push_back(false);
   }
   
//   timeSeriesData = cell(1,obj.pointGroup.GetNumPoints());
//   obj.dateData = cell(1,obj.pointGroup.GetNumPoints());
//   obj.numEventsPerPoint = zeros(1,obj.pointGroup.GetNumPoints());
//   obj.pointArray = zeros(obj.pointGroup.GetNumPoints(),3);
//   for pointIdx = 1:obj.pointGroup.GetNumPoints()
//       vec = obj.pointGroup.GetPointPositionVector(pointIdx);
//       obj.pointArray(pointIdx,:) = vec/norm(vec);
//   end
//   obj.feasibilityTest = zeros(obj.pointGroup.GetNumPoints(),1);
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
CoverageChecker::CoverageChecker(const CoverageChecker &copy) :
   pointGroup        (copy.pointGroup),
   sc                (copy.sc),
   centralBody       (copy.centralBody),
   timeIdx           (copy.timeIdx)  
{
   timeSeriesData.clear();
   for (Integer ii = 0; ii < copy.timeSeriesData.size(); ii++)
   {
      IntegerArray ia = copy.timeSeriesData.at(ii);
      timeSeriesData.push_back(ia);
   }

   dateData.clear();
   for (Integer dd = 0; dd < copy.dateData.size(); dd++)
      dateData.push_back(copy.dateData.at(dd));
   
   numEventsPerPoint.clear();
   for (Integer nn = 0; nn < copy.numEventsPerPoint.size(); nn++)
      numEventsPerPoint.push_back(copy.numEventsPerPoint.at(nn));

   for (Integer ii = 0; ii < pointArray.size(); ii++)
      delete pointArray.at(ii);
   pointArray.clear();
   for (Integer ii = 0; ii < copy.pointArray.size(); ii++)
   {
      // these Rvector3s are coordinates (x,y,z)
      Rvector3 *rv = new Rvector3(*copy.pointArray.at(ii));
      pointArray.push_back(rv);
   }
   feasibilityTest.clear();
   for (Integer ff = 0; ff < copy.feasibilityTest.size(); ff++)
      feasibilityTest.push_back(copy.feasibilityTest.at(ff));

}

//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
CoverageChecker& CoverageChecker::operator=(const CoverageChecker &copy)
{
   if (&copy == this)
      return *this;
   
   pointGroup        = copy.pointGroup;
   sc                = copy.sc;
   centralBody       = copy.centralBody;
   timeIdx           = copy.timeIdx; 

   timeSeriesData.clear();
   for (Integer ii = 0; ii < copy.timeSeriesData.size(); ii++)
   {
      IntegerArray ia = copy.timeSeriesData.at(ii);
      timeSeriesData.push_back(ia);
   }

   dateData.clear();
   for (Integer dd = 0; dd < copy.dateData.size(); dd++)
      dateData.push_back(copy.dateData.at(dd));
   
   numEventsPerPoint.clear();
   for (Integer nn = 0; nn < copy.numEventsPerPoint.size(); nn++)
      numEventsPerPoint.push_back(copy.numEventsPerPoint.at(nn));

   for (Integer ii = 0; ii < pointArray.size(); ii++)
      delete pointArray.at(ii);
   pointArray.clear();
   for (Integer ii = 0; ii < copy.pointArray.size(); ii++)
   {
      // these Rvector3s are coordinates (x,y,z)
      Rvector3 *rv = new Rvector3(*copy.pointArray.at(ii));
      pointArray.push_back(rv);
   }
   feasibilityTest.clear();
   for (Integer ff = 0; ff < copy.feasibilityTest.size(); ff++)
      feasibilityTest.push_back(copy.feasibilityTest.at(ff));

   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
CoverageChecker::~CoverageChecker()
{
   delete centralBody;
}


//------------------------------------------------------------------------------
// IntegerArray CheckPointCoverage()
//------------------------------------------------------------------------------
IntegerArray CoverageChecker::CheckPointCoverage()
{
   // Check coverage given a spacecraft location in body fixed coordinates
   IntegerArray result;  
   Real currentDate = sc->GetJulianDate();
   Rvector3 bodyFixedState = GetEarthFixedSatState(currentDate);
   Integer covCount = 0;

   Rvector3 rangeVector;
   bool     isFeasible;
   Integer  numPts = pointGroup->GetNumPoints();
   CheckGridFeasibility(bodyFixedState, rangeVector, isFeasible);
   for ( Integer pointIdx = 0; pointIdx < numPts; pointIdx++)
   {
      // Compute range vector
      // %pointVec = obj.pointGroup.GetPointPositionVector(pointIdx);
       
      // Simple line of site test
      if (feasibilityTest.at(pointIdx)) //  > 0)
      {
         // Simple nadir conical sensor test
         Integer  sensorNum = 1;
         Real     sensorFOV = GmatMathConstants::PI;
         if (sc->HasSensors())
               sensorFOV = sc->GetSensorFOV(sensorNum);

         Rvector3 rangeVec   = bodyFixedState - (*pointGroup->GetPointPositionVector(pointIdx));
         Real     rangeMag = rangeVec.GetMagnitude();
         Real     bfMag    = bodyFixedState.GetMagnitude();
         
         Real     cosineOffNadirAngle = (rangeVec * bodyFixedState)/rangeMag/bfMag;
         
         Real     offNadirAngle = GmatMathUtil::ACos(cosineOffNadirAngle);
         if (offNadirAngle < sensorFOV)
         {
            result.push_back(pointIdx);   // covCount'th entry
            covCount++;
            numEventsPerPoint.at(pointIdx) = numEventsPerPoint.at(pointIdx) + 1;
            timeSeriesData.at(pointIdx).push_back(timeIdx);
// ????            timeSeriesData{pointIdx,numEventsPerPoint(pointIdx)} = obj.timeIdx;
         }
      }
   }
   return result;
}

void CoverageChecker::CheckGridFeasibility(const Rvector3& bodyFixedState,
                                           Rvector3& rangeVector, bool& isFeasible)
{
   // TODO: Don't hard code body radii.
   Rvector3 bfState  = bodyFixedState/6378.1363;
   Rvector3 bodyUnit = bfState.GetUnitVector();
   
   RealArray feasibilityReals;
   
   for (Integer ii = 0; ii < pointArray.size(); ii++)
   {
      Rvector3 ptPos = *(pointArray.at(ii));
      feasibilityReals.push_back(ptPos * bodyUnit);  // dot product
   }
//   
//   // ? 
//   RealArray bfs;
//   Integer   sz = pointArray.size();
//   for (Integer ii = 0; ii < sz; ii++)
//      bfs.push_back(
//   rangeArray = repmat(bodyFixedState',size(obj.pointArray,1),1) - obj.pointArray;
//         
//   testArray = rangeArray.*obj.pointArray;
//   obj.feasibilityTest = sum(testArray,2);
//   return
//   %  The code above is very MATLAB specific.  Probably need to do
//   %  this in C++.
   
   IntegerArray feasibilityIdxs;
   for (Integer ii = 0; ii < feasibilityReals.size(); ii++)
   {
      if (feasibilityReals.at(ii) > 0.0)
         feasibilityIdxs.push_back(ii);
   }
   for (Integer testIdx = 0; testIdx < feasibilityIdxs.size(); testIdx++)
   {
      Integer pointIdx = feasibilityIdxs.at(testIdx);
      if (feasibilityReals.at(pointIdx) > 0.0)  // always true becasue of how we set it up?
      {
         Rvector3 *pointVec = pointArray.at(pointIdx);
         Rvector3 rangeVec(bfState - (*pointVec));
         Real     dot       = rangeVec * (*pointVec);
         if (dot > 0)
            feasibilityTest.at(pointIdx) = true;
         else
            feasibilityTest.at(pointIdx) = false;
      }
   }
   
////   [maybe] = find(obj.feasibilityTest>0);
//   for testIdx = 1:size(maybe,1)
//       pointIdx = maybe(testIdx);
//       if obj.feasibilityTest(pointIdx) > 0;
//           pointVec = obj.pointArray(pointIdx,:)';
//           rangeVec = bodyFixedState - pointVec;
//           dotProd = rangeVec(1)*pointVec(1)+ rangeVec(2)*pointVec(2)+ ...
//               rangeVec(3)*pointVec(3);
//           if dotProd > 0
//               obj.feasibilityTest(pointIdx) = true;
//           else
//               obj.feasibilityTest(pointIdx) = false;
//           end
//       end
//   end
}

IntegerArray CoverageChecker::AccumulateCoverageData()
{
   // Accumulates coverage data after propagation update
   timeIdx++;
   dateData.at(timeIdx) = sc->GetJulianDate();
   return CheckPointCoverage();
}

Rvector3 CoverageChecker::GetEarthFixedSatState(Real jd)
{
   // Converts state from Earth interial to Earth fixed
   Rvector6 inertialState = sc->GetCartesianState();
   Rvector3 inertialPos   = inertialState.GetR();
   // TODO.  Handle differences in units of points and states.
   return centralBody->GetBodyFixedState(inertialPos, jd);
}

std::vector<VisiblePOIReport> CoverageChecker::ProcessCoverageData()
{
   std::vector<VisiblePOIReport> reports;
   
//   coverageEvents = {};
   Integer numCoverageEvents = 0;
   Integer numPts            = pointGroup->GetNumPoints();
   Integer numEvents         = 0;
   Real    startTime;
   Real    endTime;
   
   for (Integer pointIdx = 0; pointIdx < numPts; pointIdx++)
   {

      // Only perform if there are interval events (2 or more events)
       bool isEnd = false;
       numEvents  = numEventsPerPoint.at(pointIdx);
       if (numEvents >= 2)
       {
          startTime = dateData.at((timeSeriesData.at(pointIdx)).at(0));
       
           for (Integer dateIdx = 1; dateIdx < numEvents; dateIdx++)
           {
               // Test for end of an interval
              Integer atIdx     = (timeSeriesData.at(pointIdx)).at(dateIdx);
              Integer atPrevIdx = (timeSeriesData.at(pointIdx)).at(dateIdx-1);
              if ((atIdx - atPrevIdx) != 1)
              {
                  endTime = dateData.at(atPrevIdx);
                  isEnd = true;
              }
              // Test for the last event for this point
               else if (dateIdx == numEvents)
               {
                   endTime = dateData.at(atIdx);
                   isEnd = true;
               }
              // otherwise, endTime is not set!
               if (isEnd)
               {
                   VisiblePOIReport poiReport = CreateNewPOIReport(startTime,endTime,pointIdx);
                   numCoverageEvents++;
                   reports.push_back(poiReport);
                   startTime = dateData.at(atIdx);
                   isEnd = false;
               }
           }
       }
   }
   return reports;
}

VisiblePOIReport CoverageChecker::CreateNewPOIReport(Real startJd, Real endJd,
                                                     Integer poiIdx)
{
   // Creates VisiblePOIReport given point indeces and start/end dates
   VisiblePOIReport poiReport;
   AbsoluteDate     startEpoch;
   AbsoluteDate     endEpoch;
   
   poiReport.SetPOIIndex(poiIdx);
   startEpoch.SetJulianDate(startJd);
   endEpoch.SetJulianDate(endJd);
   poiReport.SetStartDate(startEpoch);
   poiReport.SetEndDate(endEpoch);
   return poiReport;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

