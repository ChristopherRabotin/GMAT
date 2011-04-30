//$Id$
//------------------------------------------------------------------------------
//                              AttitudeUtil
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
// Author: Linda Jun
// Created: 2005/09/27
//
/**
 * This file provides coversion between different attitude representation.
 * (The code is reused from GSS.)
 */
//------------------------------------------------------------------------------
#ifndef AttitudeUtil_hpp
#define AttitudeUtil_hpp

#include "BaseException.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"

namespace GmatAttUtil
{
   Rvector3 GMAT_API ToEulerAngles(Real rotAngle, const Rvector3 &rotAxis,
                          Integer seq1, Integer seq2, Integer seq3);
   Rvector3 GMAT_API ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
                          Integer seq2, Integer seq3);
   Rmatrix33 GMAT_API ToCosineMatrix(const Rvector3 &qVec, Real qScalar);
   Rmatrix33 GMAT_API ToCosineMatrix(Real rotAngle, const Rvector3 &rotAxis);

}

namespace FloatAttUtil
{
   // for float quaternion
   void GMAT_API ToQuat(float q[4], float p1x, float p1y, float p2x, float p2y);
   void GMAT_API ToQuat(float q[4], float mat[16]);
   void GMAT_API IdentityMat(float m[16]);
   void GMAT_API MultMat(float mat1[16], float mat2[16], float mat3[16]);
   void GMAT_API AddQuats(float *q1, float *q2, float *dest);
   void GMAT_API NormalizeQuat(float q[4]);
   void GMAT_API BuildRotMatrix(float m[4][4], float q[4]);
   void GMAT_API AxisToQuat(float a[3], float phi, float q[4]);
   float GMAT_API ToSphereProjection(float r, float x, float y);

   // for float vector util
   void GMAT_API SetZero(float *v);
   void GMAT_API Set(float *v, float x, float y, float z);
   void GMAT_API Subtract(const float *src1, const float *src2, float *dst);
   void GMAT_API Copy(const float *v1, float *v2);
   void GMAT_API Cross(const float *v1, const float *v2, float *cross);
   void GMAT_API Scale(float *v, float div);
   void GMAT_API Normal(float *v);
   void GMAT_API Add(const float *src1, const float *src2, float *dst);
   float GMAT_API Length(const float *v);
   float GMAT_API Dot(const float *v1, const float *v2);
}

#endif // AttitudeUtil_hpp
