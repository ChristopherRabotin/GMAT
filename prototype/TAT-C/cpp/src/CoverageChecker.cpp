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
 * Implementation of the CoverageChecker class
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "CoverageChecker.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "Rmatrix33.hpp"
#include "TATCException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_COV_CHECK
//#define DEBUG_COV_CHECK_FOV

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  CoverageChecker(PointGroup *ptGroup, Spacecraft *sat)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param ptGroup  pointer to the PointGroup object to use
 * @param sat      pointer to the SPacecraft object to use
 */
//---------------------------------------------------------------------------
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

   centralBody    = new Earth();
   Integer numPts = pointGroup->GetNumPoints();
   IntegerArray emptyIntArray;  // empty array
   for (Integer ii = 0; ii < numPts; ii++)
   {
      timeSeriesData.push_back(emptyIntArray);
//      dateData.push_back(noDate); // want to accumulate these as we go along
      numEventsPerPoint.push_back(0);
      
      /// @TODO This should not be set here - we should store both
      /// positions and unitized positions in the PointGroup and
      /// then access those arrays when needed <<<<<<<<<<<<<<
      Rvector3 *ptPos  = pointGroup->GetPointPositionVector(ii);
      Rvector3 *posUnit = new Rvector3(ptPos->GetUnitVector());
//#ifdef DEBUG_COV_CHECK
//      MessageInterface::ShowMessage("PUSHING BACK pos unit vector: %s\n",
//                                    posUnit->ToString(12).c_str());
//#endif
      pointArray.push_back(posUnit);
      feasibilityTest.push_back(false);
   }
}

//------------------------------------------------------------------------------
//  CoverageChecker(const CoverageChecker &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param copy  the object to copy
 * 
 */
//---------------------------------------------------------------------------
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
//  CoverageChecker& operator=(const CoverageChecker &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the CoverageChecker object
 *
 * @param copy  the object to copy
 * 
 */
//---------------------------------------------------------------------------
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
//  ~CoverageChecker()
//------------------------------------------------------------------------------
/**
 * Destructor
 * 
 */
//---------------------------------------------------------------------------
CoverageChecker::~CoverageChecker()
{
   delete centralBody;
}


//------------------------------------------------------------------------------
// IntegerArray CheckPointCoverage()
//------------------------------------------------------------------------------
/**
 * Checks the point coverage.
 *
 * @return  array of indexes 
 * 
 */
//---------------------------------------------------------------------------
IntegerArray CoverageChecker::CheckPointCoverage()
{
//   #ifdef DEBUG_COV_CHECK
//      MessageInterface::ShowMessage("In CoverageChecker::CheckPointCoverage\n");
//   #endif
   // Check coverage given a spacecraft location in body fixed coordinates
   IntegerArray result;  
   Real         currentDate    = sc->GetJulianDate();
   Rvector3     bodyFixedState = GetEarthFixedSatState(currentDate);
   Integer      covCount       = 0;

   Rvector3 rangeVector;
   bool     isFeasible;
   Integer  numPts = pointGroup->GetNumPoints();
   
   #ifdef DEBUG_COV_CHECK
      MessageInterface::ShowMessage(" --- numPoints from pointGroup = %d\n",
                                     numPts);
      MessageInterface::ShowMessage(" --- Checking Feasibility ...\n");
   #endif
   CheckGridFeasibility(bodyFixedState, rangeVector, isFeasible);
   for ( Integer pointIdx = 0; pointIdx < numPts; pointIdx++)
   {
      // Compute range vector
      // %pointVec = obj.pointGroup.GetPointPositionVector(pointIdx);
       
      // Simple line of site test
      if (feasibilityTest.at(pointIdx)) //  > 0)
      {
         #ifdef DEBUG_COV_CHECK
            MessageInterface::ShowMessage(" --- feasibilty at point %d is TRUE!\n",
                                           pointIdx);
         #endif
         // Simple nadir conical sensor test
         Integer  sensorNum = 0;   // 1;
         Real     sensorFOV = GmatMathConstants::PI;
         if (sc->HasSensors())
               sensorFOV = sc->GetSensorFOV(sensorNum);

         #ifdef DEBUG_COV_CHECK
            MessageInterface::ShowMessage(" --- In CheckPointCoverage, sensorFOV set to %12.10f\n",
                                          sensorFOV);
            MessageInterface::ShowMessage(" --- In CheckPointCoverage, bodyFixedState = %s\n",
                                          bodyFixedState.ToString(12).c_str());
         #endif
//         Rvector3 rangeVec   = bodyFixedState - (*pointGroup->GetPointPositionVector(pointIdx));
         Rvector3 rangeVec   = bodyFixedState - (*pointArray.at(pointIdx)* 6378.1363);
         Real     rangeMag   = rangeVec.GetMagnitude();
         Real     bfMag      = bodyFixedState.GetMagnitude();
         
         Rvector3 bfsUnit = bodyFixedState.GetUnitVector();
         Rvector3 rangeUnit = rangeVec.GetUnitVector();
         
         Real     cosineOffNadirAngle = rangeUnit * bfsUnit;
//         Real     cosineOffNadirAngle = ((rangeVec * bodyFixedState)/rangeMag)/bfMag;
         
         Real     offNadirAngle = GmatMathUtil::ACos(cosineOffNadirAngle);
#ifdef DEBUG_COV_CHECK_FOV
         MessageInterface::ShowMessage(" --- In CheckPointCoverage, pointArray = %s\n",
                                       (pointArray.at(pointIdx))->ToString(12).c_str());
         MessageInterface::ShowMessage(" --- In CheckPointCoverage, bodyFixedState = %s\n",
                                       bodyFixedState.ToString(12).c_str());
         MessageInterface::ShowMessage(" --- In CheckPointCoverage, rangeVec = %s\n",
                                       rangeVec.ToString(12).c_str());
         MessageInterface::ShowMessage(" --- In CheckPointCoverage, offNadirAngle =  %12.10f\n",
                                       offNadirAngle);
         MessageInterface::ShowMessage(" --- In CheckPointCoverage, fov           =  %12.10f\n",
                                       sensorFOV);
#endif
         if (offNadirAngle < sensorFOV)
         {
            result.push_back(pointIdx);   // covCount'th entry
            covCount++;
            numEventsPerPoint.at(pointIdx) = numEventsPerPoint.at(pointIdx) + 1;
            timeSeriesData.at(pointIdx).push_back(timeIdx);
// ????            timeSeriesData{pointIdx,numEventsPerPoint(pointIdx)} = obj.timeIdx;
            #ifdef DEBUG_COV_CHECK
               MessageInterface::ShowMessage(" --- In CheckPointCoverage, setting numEventsPerPoint (%d) to %d\n",
                                             pointIdx, numEventsPerPoint.at(pointIdx));
               MessageInterface::ShowMessage(" --- Added timeIdx %d to timeSeriesData(%d)\n",
                                             timeIdx, pointIdx);
            #endif
         }
      }
   }
   #ifdef DEBUG_COV_CHECK
      for (Integer ii = 0; ii < pointGroup->GetNumPoints(); ii++)
         MessageInterface::ShowMessage(" --- numEventsPerPoint(%d) = %d\n",
                                       ii, numEventsPerPoint.at(ii));
   #endif

   return result;
}

//------------------------------------------------------------------------------
// IntegerArray CheckPointCoverage()
//------------------------------------------------------------------------------
/**
 * Checks the point coverage.
 *
 * @return  array of indexes 
 * 
 */
//---------------------------------------------------------------------------
void CoverageChecker::CheckGridFeasibility(const Rvector3& bodyFixedState,
                                           Rvector3& rangeVector, bool& isFeasible)
{
   // TODO: Don't hard code body radii.
   Rvector3 bfState  = bodyFixedState/6378.1363;
   Rvector3 bodyUnit = bfState.GetUnitVector();
   #ifdef DEBUG_COV_CHECK
      MessageInterface::ShowMessage(" --- In CheckGridFeasibility, bodyFixedState = %s\n",
                                    bodyFixedState.ToString(12).c_str());
      MessageInterface::ShowMessage(" --- In CheckGridFeasibility, bodyUnit = %s\n",
                                    bodyUnit.ToString(12).c_str());
      MessageInterface::ShowMessage("pointArray size = %d\n", pointArray.size());
      for (Integer ii = 0; ii < pointArray.size(); ii++)
         MessageInterface::ShowMessage("getting the pos unit vector: %s\n",
                                       (pointArray.at(ii))->ToString(12).c_str());
   #endif
   
   RealArray feasibilityReals;
   
   for (Integer ii = 0; ii < pointArray.size(); ii++)
   {
      #ifdef DEBUG_COV_CHECK
//         Rvector3 *p = pointArray.at(ii);
//         MessageInterface::ShowMessage(" --- In CheckGridFeasibility, p (%d) = %s\n",
//                                          ii, p->ToString(12).c_str());
      #endif
      Rvector3 ptPos = *(pointArray.at(ii));
      #ifdef DEBUG_COV_CHECK
         MessageInterface::ShowMessage(" --- In CheckGridFeasibility, ptPos (%d) = %s\n",
                                       ii, ptPos.ToString(12).c_str());
      #endif
      feasibilityReals.push_back(ptPos * bodyUnit);  // dot product
   }
   
   IntegerArray feasibilityIdxs;
   for (Integer ii = 0; ii < feasibilityReals.size(); ii++)
   {
      if (feasibilityReals.at(ii) > 0.0)
         feasibilityIdxs.push_back(ii);
   }
   #ifdef DEBUG_COV_CHECK
      MessageInterface::ShowMessage(" --- In CheckGridFeasibility, feasibilityIdxs are:\n");
      for (Integer ii = 0; ii < feasibilityIdxs.size(); ii++)
         MessageInterface::ShowMessage(" ...    %d\n", feasibilityIdxs.at(ii));
   #endif
   for (Integer testIdx = 0; testIdx < feasibilityIdxs.size(); testIdx++)
   {
      Integer pointIdx = feasibilityIdxs.at(testIdx);
      if (feasibilityReals.at(pointIdx) > 0.0)  // always true because of how we set it up?
      {
         Rvector3 *pointVec = pointArray.at(pointIdx);
         Rvector3 rangeVec(bfState - (*pointVec));
         Real     dot       = rangeVec * (*pointVec);
         if (dot > 0)
            feasibilityTest.at(pointIdx) = true;
         else
            feasibilityTest.at(pointIdx) = false;
         isFeasible  = feasibilityTest.at(pointIdx);
         rangeVector = rangeVec;  // NOT SURE about this!!!!
      }
   }
   
}

//------------------------------------------------------------------------------
// IntegerArray AccumulateCoverageData()
//------------------------------------------------------------------------------
/**
 * Accumulates the coverage data after the propagation update
 *
 * @return  array of indexes 
 * 
 */
//---------------------------------------------------------------------------
IntegerArray CoverageChecker::AccumulateCoverageData()
{
   // Accumulates coverage data after propagation update
//   dateData.at(timeIdx) = sc->GetJulianDate();
   dateData.push_back(sc->GetJulianDate());
   timeIdx++;
   return CheckPointCoverage();
}

//------------------------------------------------------------------------------
// Rvector3 GetEarthFixedSatState(Real jd)
//------------------------------------------------------------------------------
/**
 * Returns the Earth-Fixed state at the specified time
 * 
 * @param jd  Julian date 
 *
 * @return  earth-fixes state at the input time
 * 
 */
//---------------------------------------------------------------------------
Rvector3 CoverageChecker::GetEarthFixedSatState(Real jd)
{
   // Converts state from Earth interial to Earth fixed
   Rvector6 inertialState = sc->GetCartesianState();
   Rvector3 inertialPos   = inertialState.GetR();
   // TODO.  Handle differences in units of points and states.
   return centralBody->GetBodyFixedState(inertialPos, jd);
}

//------------------------------------------------------------------------------
// std::vector<VisiblePOIReport> ProcessCoverageData()
//------------------------------------------------------------------------------
/**
 * Returns an array of reports of coverage
 * 
 * @return  array of reports of coverage
 * 
 */
//---------------------------------------------------------------------------
std::vector<VisiblePOIReport> CoverageChecker::ProcessCoverageData()
{
   #ifdef DEBUG_COV_CHECK
      MessageInterface::ShowMessage(" --- In ProcessCoverageData, feasibilityTest:\n");
      for (Integer ii = 0; ii < feasibilityTest.size(); ii++)
         MessageInterface::ShowMessage(" ... %d ...    %s\n", ii, (feasibilityTest.at(ii)? "TRUE" : "false"));
   #endif
   std::vector<VisiblePOIReport> reports;
   
   Integer numCoverageEvents = 0;
   Integer numPts            = pointGroup->GetNumPoints();
   Integer numEvents         = 0;
   Real    startTime;
   Real    endTime;
   
#ifdef DEBUG_COV_CHECK
   MessageInterface::ShowMessage(" --- In ProcessCoverageData, numPts = %d\n",
                                 numPts);
#endif
   for (Integer pointIdx = 0; pointIdx < numPts; pointIdx++)
   {

      // Only perform if there are interval events (2 or more events)
       bool isEnd = false;
       numEvents  = numEventsPerPoint.at(pointIdx);
#ifdef DEBUG_COV_CHECK
      MessageInterface::ShowMessage(" --- In ProcessCoverageData, numEvents (%d) = %d\n",
                                    pointIdx, numEvents);
#endif
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
               else if (dateIdx == (numEvents-1))
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

//------------------------------------------------------------------------------
// VisiblePOIReport CreateNewPOIReport(Real startJd, Real endJd, Integer poiIdx)
//------------------------------------------------------------------------------
/**
 * Creates a new report of coverage data.
 * 
 * @param startJd start Julian date for the report
 * @param endJd   end Julian date for the report
 * @param poiIndex POI index for the created report
 * 
 * @return  report of coverage
 * 
 */
//---------------------------------------------------------------------------
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
// none
