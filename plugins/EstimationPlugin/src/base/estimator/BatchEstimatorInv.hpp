//$Id: BatchEstimatorInv.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         BatchEstimatorInv
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
// Created: 2009/08/04
//
/**
 * Batch least squares estimator using direct inversion
 */
//------------------------------------------------------------------------------


#ifndef BatchEstimatorInv_hpp
#define BatchEstimatorInv_hpp


#include "BatchEstimator.hpp"
//#include "PropSetup.hpp"
//#include "MeasurementManager.hpp"


/**
 * A simple batch least squares estimator.
 *
 * This estimator implements the algorithm described in Tapley, Schutz and Born,
 * Statistical Orbit Determination (2004), chapter 4, as illustrated in the
 * flowchart on pages 196-197.  The normal equations are solved through direct
 * inversion of the information matrix.
 */
class ESTIMATION_API BatchEstimatorInv: public BatchEstimator
{
public:
   BatchEstimatorInv(const std::string &name);
   virtual ~BatchEstimatorInv();
   BatchEstimatorInv(const BatchEstimatorInv& est);
   BatchEstimatorInv& operator=(const BatchEstimatorInv& est);

   virtual GmatBase*       Clone() const;
   virtual void            Copy(const GmatBase*);

protected:
   virtual void            Accumulate();
   virtual void            Estimate();
};

#endif /* BatchEstimatorInv_hpp */
