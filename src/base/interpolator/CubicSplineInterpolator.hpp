//$Id$
//------------------------------------------------------------------------------
//                            CubicSplineInterpolator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/02
//
/**
 * Defines interpolation class using the cubic spline algorithm described in 
 * Numerical Recipes in C, 2nd Ed., pp 113 ff
 */
//------------------------------------------------------------------------------


#ifndef CubicSplineInterpolator_hpp
#define CubicSplineInterpolator_hpp


#include "Interpolator.hpp"

class GMAT_API CubicSplineInterpolator : public Interpolator
{
public:
   CubicSplineInterpolator(const std::string &name = "", Integer dim = 1);
   virtual ~CubicSplineInterpolator();
   CubicSplineInterpolator(const CubicSplineInterpolator &csi);
   CubicSplineInterpolator&    operator=(const CubicSplineInterpolator &csi);

   virtual bool                Interpolate(const Real ind, Real *results);

   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

protected:
   /// Array of ordered independent variables used to construct the splines.
   Real                        x[5];
   /// Array of ordered independent variables used to construct the splines.
   Real                        *y[5];
   /// Array of second derivatives used to evaluate the spline coefficients.
   Real                        *y2[5];     // Assumes 5-point splines
   /// Value of the last point, to determine if the splines need updating
   Real                        lastX;

   // Inherited methods that need some revision for cubic splines
   virtual void                AllocateArrays(void);
   virtual void                CleanupArrays(void);
   virtual void                CopyArrays(const CubicSplineInterpolator &i);

   bool                        BuildSplines(void);
   void                        LoadArrays(void);
   bool                        Estimate(const Real ind, Real *results);
};


#endif // CubicSplineInterpolator_hpp
