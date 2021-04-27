//------------------------------------------------------------------------------
//                              CubicSpline
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2019.04.02
//
/**
 * Original Python prototype:
 * Author: N. Hatten
 *
 * This class is a singleton.  All code that uses it will use the one instance
 * of this class to access its methods.
 * This code was converted from the original python prototype, and includes
 * original commentary.
 */
//------------------------------------------------------------------------------

#ifndef CubicSpline_hpp
#define CubicSpline_hpp

#include "utildefs.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"

class GMATUTIL_API CubicSpline
{
public:
   // Most of the time, GMAT uses the singleton
   static CubicSpline* Instance();
   
   void EvaluateClampedCubicSplineVectorized(const Rmatrix &a, const Rmatrix &b,
                                             const Rmatrix &c, const Rmatrix &d,
                                             const Rvector &xArray, Real x,
                                             Rvector &y, Rvector &dy,
                                             Rvector &ddy);
   
   void CalculateClampedCubicSplineCoefficients(
                           const Rvector &x,    const Rvector &y,
                           Real          dydx0, Real          dydxf,
                           Rvector       &a,    Rvector       &b,
                           Rvector       &c,    Rvector       &d);
   
   Rvector ThomasAlgorithm(const Rvector &a, const Rvector &b,
                           const Rvector &c, const Rvector &d);
   
   Real FiniteDifferenceAtEdge(const Rvector &xArray, const Rvector &yArray,
                               const std::string &leftOrRightEdge);
   
protected:
   CubicSpline();
   CubicSpline(const CubicSpline &copy);
   
   // The singleton
   static CubicSpline *theCubicSpline;
};

#endif // CubicSpline_hpp
