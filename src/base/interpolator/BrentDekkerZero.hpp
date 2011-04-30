//$Id$
//------------------------------------------------------------------------------
//                            BrentDekkerZero
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/04/10
//
/**
 * Defines interpolation class using the cubic spline algorithm described in
 * Numerical Recipes in C, 2nd Ed., pp 113 ff
 */
//------------------------------------------------------------------------------


#ifndef BrentDekkerZero_hpp
#define BrentDekkerZero_hpp

#include "gmatdefs.hpp"

class GMAT_API BrentDekkerZero
{
public:
   BrentDekkerZero();
   virtual ~BrentDekkerZero();
   BrentDekkerZero(const BrentDekkerZero &bdz);
   BrentDekkerZero&  operator=(const BrentDekkerZero &bdz);
   
   void SetInterval(Real a0, Real b0, Real fa0, Real fb0, Real tolerance);
   Real FindStep(Real lastStep, Real lastEval);
   bool CheckConvergence();
   
   // Method used to test the implementation
   Real TestDriver(Real aVal, Real bVal, Real tVal);
   
protected:
   Real a;
   Real b;
   Real macheps;
   Real t;

   Real c;
   Real d;
   Real e;
   Real fa;
   Real fb;
   Real fc;
   Real tol;
   Real m;
   Real p;
   Real q;
   Real r;
   Real s;

   void SwapAC();
   void FindStepParameters();

   // Test function for the implementation
   Real TestFunction(Real value);
};

#endif /*BrentDekkerZero_hpp*/
