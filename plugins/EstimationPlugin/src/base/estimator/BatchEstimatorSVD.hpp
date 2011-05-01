//$Id: BatchEstimatorSVD.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                      BatchEstimatorSVD
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
