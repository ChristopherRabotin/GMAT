//$Header$
//------------------------------------------------------------------------------
//                            CubicSplineInterpolator
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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


#ifndef CUBICSPLINEINTERPOLATOR_H
#define CUBICSPLINEINTERPOLATOR_H


#include "Interpolator.hpp"

class CubicSplineInterpolator : public Interpolator
{
public:
	CubicSplineInterpolator(Integer dim = 1);
	virtual ~CubicSplineInterpolator();
    CubicSplineInterpolator(const CubicSplineInterpolator &csi);
    CubicSplineInterpolator&    operator=(const CubicSplineInterpolator &csi);

    virtual bool                Interpolate(const Real ind, Real *results);
    
protected:
    /// Array of ordered independent variables used to construct the splines,
    Real                        x[5];
    /// Array of ordered independent variables used to construct the splines,
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


#endif // CUBICSPLINEINTERPOLATOR_H
