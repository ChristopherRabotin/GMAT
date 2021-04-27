//------------------------------------------------------------------------------
//                           HyperSensitiveDriver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 10, 2017
/**
 * Hypersensitive test case driver
 */
//------------------------------------------------------------------------------

#include "csalt.hpp"

#include "HyperSensitiveDriver.hpp"
#include "HyperSensitivePointObject.hpp"
#include "HyperSensitivePathObject.hpp"

static const Real INF = std::numeric_limits<Real>::infinity();

using namespace std;
using namespace GmatMathConstants;

//#define DEBUG_SHOWRESULTS

HyperSensitiveDriver::HyperSensitiveDriver() :
      CsaltTestDriver("HyperSensitive")
{
}

HyperSensitiveDriver::~HyperSensitiveDriver()
{
}

void HyperSensitiveDriver::SetPointPathAndProperties()
{
   pathObject = new HyperSensitivePathObject();
   pointObject = new HyperSensitivePointObject();

   costLowerBound = 0.0;
   maxMeshRefinementCount = 20;
   majorOptimalityTolerances.SetSize(3);
   majorOptimalityTolerances(0) = 0.01;
   majorOptimalityTolerances(1) = 0.01;
   majorOptimalityTolerances(2) = 1e-4;

   totalIterationsLimits.resize(2);
   totalIterationsLimits[0] = 3000;
   totalIterationsLimits[1] = 10000;

   feasibilityTolerances.SetSize(4);
   feasibilityTolerances(0) = 0.001;
   feasibilityTolerances(1) = 0.0001;
   feasibilityTolerances(2) = 0.00001;
   feasibilityTolerances(3) = 0.000001;

   majorIterationsLimits.resize(3);
   majorIterationsLimits[0] = 75;
   majorIterationsLimits[1] = 100;
   majorIterationsLimits[2] = 1000;

}

void HyperSensitiveDriver::SetupPhases()
{
   RadauPhase *phase1           = new RadauPhase();
   std::string initialGuessMode = "LinearNoControl";

   // for MR; YK
   Rvector meshIntervalFractions(5, -1.0, -0.98, 0.0, 0.98, 1.0);
   IntegerArray meshIntervalNumPoints;
   meshIntervalNumPoints.push_back(7);
   meshIntervalNumPoints.push_back(5);
   meshIntervalNumPoints.push_back(5);
   meshIntervalNumPoints.push_back(7);

//   Rvector meshIntervalFractions(5, -1.0, -0.8, 0.0, 0.8, 1.0);
//   IntegerArray meshIntervalNumPoints;
//   meshIntervalNumPoints.push_back(15);
//   meshIntervalNumPoints.push_back(10);
//   meshIntervalNumPoints.push_back(10);
//   meshIntervalNumPoints.push_back(15);

   // Set time properties
   Real timeLowerBound   = 0.0;
   Real timeUpperBound   = 10000.0;
   Real initialGuessTime = 0.0;
   Real finalGuessTime   = 10000.0;
   // Set state properties
   Integer numStateVars              = 1;
   Rvector stateLowerBound(1, -50.0);
   Rvector stateUpperBound(1, 50.0);
   Rvector initialGuessState(1, 1.0);
   Rvector finalGuessState(1, 1.5);
   // Set control properties
   Integer numControlVars  = 1;
   Rvector controlUpperBound(1,  50.0);
   Rvector controlLowerBound(1, -50.0);

   phase1->SetInitialGuessMode(initialGuessMode);
   phase1->SetNumStateVars(numStateVars);
   phase1->SetNumControlVars(numControlVars);
   phase1->SetMeshIntervalFractions(meshIntervalFractions);
   phase1->SetMeshIntervalNumPoints(meshIntervalNumPoints);
   phase1->SetStateLowerBound(stateLowerBound);
   phase1->SetStateUpperBound(stateUpperBound);
   phase1->SetStateInitialGuess(initialGuessState);
   phase1->SetStateFinalGuess(finalGuessState);
   phase1->SetTimeLowerBound(timeLowerBound);
   phase1->SetTimeUpperBound(timeUpperBound);
   phase1->SetTimeInitialGuess(initialGuessTime);
   phase1->SetTimeFinalGuess(finalGuessTime);
   phase1->SetControlLowerBound(controlLowerBound);
   phase1->SetControlUpperBound(controlUpperBound);
   phase1->SetRelativeErrorTol(1e-6);

   phaseList.push_back(phase1);
}
