//$Id: ExtendedKalmanInv.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         ExtendedKalmanInv
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
// Created: 2009/09/03
//
/**
 * Definition for a basic extended Kalman filter
 */
//------------------------------------------------------------------------------


#ifndef ExtendedKalmanInv_hpp
#define ExtendedKalmanInv_hpp

#include "SequentialEstimator.hpp"


/**
 * This implementation provides a basic extended Kalman filter (EKF)
 *
 * ExtendedKalmanInv follows the EKF described in the flowchart on page 212 of
 * Tapley, Schutz and Born (2004).  Several refinements, suggested by R.
 * Carpenter, are included in this code:
 *
 * 1.  The state noise covariance matrix and the time updated covariance both
 * have symmetry enforced explicitly.
 *
 * 2.  The state covariance matrix update can be performed two different ways;
 * either using the simple form in the flow chart:
 *
 *    P = (I - K Htilde) Pbar
 *
 * or using the form derived by Bucy and Joseph (equation 4.7.19 on page 205).
 * This choice is made at compile time in the UpdateElements() method.  The
 * current default selection is the Bucy-Joseph update.
 */
class ESTIMATION_API ExtendedKalmanInv : public SequentialEstimator
{
public:
   ExtendedKalmanInv(const std::string name);
   virtual ~ExtendedKalmanInv();
   ExtendedKalmanInv(const ExtendedKalmanInv &ekf);
   ExtendedKalmanInv&      operator=(const ExtendedKalmanInv &ekf);

   GmatBase*               Clone() const;
   virtual void            Copy(const GmatBase*);

protected:
   /// The time updates covariance matrix
   Rmatrix                 pBar;
   /// The O-C measurements
   RealArray               yi;
   /// The Kalman gain
   Rmatrix                 kalman;
   /// Size of the measurement
   UnsignedInt             measSize;

   virtual void            CompleteInitialization();
   virtual void            Estimate();

   void                    TimeUpdate();
   void                    ComputeObs();
   void                    ComputeGain();
   void                    UpdateElements();

   void                    Symmetrize(Covariance& mat);
   void                    Symmetrize(Rmatrix& mat);
   void                    UpdateCovarianceSimple();
   void                    UpdateCovarianceJoseph();
};

#endif /* ExtendedKalmanInv_hpp */
