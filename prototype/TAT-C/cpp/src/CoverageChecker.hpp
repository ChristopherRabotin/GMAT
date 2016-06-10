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
// Created: 2016.05.09
//
/**
 * Definition of the coverage checker class.  This class checks for point
 * coverae and generates reports.
 */
//------------------------------------------------------------------------------
#ifndef CoverageChecker_hpp
#define CoverageChecker_hpp

#include "gmatdefs.hpp"
#include "AbsoluteDate.hpp"
#include "Spacecraft.hpp"
#include "PointGroup.hpp"
#include "Earth.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "VisiblePOIReport.hpp"

class CoverageChecker
{
public:
   
   // class methods
   CoverageChecker(PointGroup *ptGroup, Spacecraft *sat);
   CoverageChecker( const CoverageChecker &copy);
   CoverageChecker& operator=(const CoverageChecker &copy);
   
   virtual ~CoverageChecker();
   
   virtual IntegerArray      CheckPointCoverage();
   virtual void              CheckGridFeasibility(
                             const Rvector3& bodyFixedState,
                             Rvector3& rangeVector, bool& isFeasible);
   virtual IntegerArray      AccumulateCoverageData();
   virtual Rvector3          GetEarthFixedSatState(Real jd);
   virtual std::vector<VisiblePOIReport>
                             ProcessCoverageData();
   virtual VisiblePOIReport  CreateNewPOIReport(Real startJd, Real endJd,
                                                Integer poiIdx);
   
protected:
   
   /// the points to use for coverage
   PointGroup                 *pointGroup;
   /// The spacecraft object
   Spacecraft                 *sc;
   /// the central body; the model of Earth's properties & rotation
   Earth                      *centralBody;
   /// the number of accumulated propagation data points // ???
   Integer                    timeIdx;
   /// times when points are visible
   std::vector<IntegerArray>  timeSeriesData; 
   /// the date of each propagation point
   RealArray                  dateData;
   /// the number of propagation times when each point was visible
   IntegerArray               numEventsPerPoint;
   /// array of all points
   std::vector<Rvector3*>     pointArray;
   /// feasibility values for each point
   std::vector<bool>          feasibilityTest;  
};
#endif // CoverageChecker_hpp







