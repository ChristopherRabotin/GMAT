//$Id: BatchEstimatorSVD.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                      BatchEstimatorSVD
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/08/28
//
/**
 * Batch least squares estimator using singular value decomposition.
 *
 * Note: SVD code is not yet implemented
 */
//------------------------------------------------------------------------------


#ifndef BatchEstimatorSVD_hpp
#define BatchEstimatorSVD_hpp

#include "BatchEstimator.hpp"
#include "PropSetup.hpp"
#include "MeasurementManager.hpp"


/**
 * A batch least squares estimator that uses singular value decomposition.
 *
 * Note:  The SVD code is not yet implemented
 */
class ESTIMATION_API BatchEstimatorSVD : public BatchEstimator
{
public:
   BatchEstimatorSVD(const std::string &name);
   virtual ~BatchEstimatorSVD();
   BatchEstimatorSVD(const BatchEstimatorSVD& est);
   BatchEstimatorSVD& operator=(const BatchEstimatorSVD& est);

   virtual GmatBase*       Clone() const;
   virtual void            Copy(const GmatBase*);

protected:
   // Temporary -- remove when math is coded
   virtual void            CompleteInitialization();

   virtual void            Accumulate();
   virtual void            Estimate();
};

#endif /* BatchEstimatorSVD_hpp */
