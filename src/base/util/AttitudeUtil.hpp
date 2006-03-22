//$Header$
//------------------------------------------------------------------------------
//                              AttitudeUtil
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
#include "RealTypes.hpp"
#include "Rvector3.hpp"
#include "Rmatrix33.hpp"

namespace GmatAttUtil
{
   Rvector3 ToEulerAngles(Real rotAngle, const Rvector3 &rotAxis,
                          Integer seq1, Integer seq2, Integer seq3);
   Rvector3 ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
                          Integer seq2, Integer seq3);
   Rmatrix33 ToCosineMatrix(const Rvector3 &qVec, Real qScalar);
   Rmatrix33 ToCosineMatrix(Real rotAngle, const Rvector3 &rotAxis);

}

namespace FloatAttUtil
{
   // for float quaternion
   void ToQuat(float q[4], float p1x, float p1y, float p2x, float p2y);
   void ToQuat(float q[4], float mat[16]);
   void IdentityMat(float m[16]);
   void MultMat(float mat1[16], float mat2[16], float mat3[16]);
   void AddQuats(float *q1, float *q2, float *dest);
   void NormalizeQuat(float q[4]);
   void BuildRotMatrix(float m[4][4], float q[4]);
   void AxisToQuat(float a[3], float phi, float q[4]);
   float ToSphereProjection(float r, float x, float y);

   // for float vector util
   void SetZero(float *v);
   void Set(float *v, float x, float y, float z);
   void Subtract(const float *src1, const float *src2, float *dst);
   void Copy(const float *v1, float *v2);
   void Cross(const float *v1, const float *v2, float *cross);
   void Scale(float *v, float div);
   void Normal(float *v);
   void Add(const float *src1, const float *src2, float *dst);
   float Length(const float *v);
   float Dot(const float *v1, const float *v2);
}

#endif // AttitudeUtil_hpp
