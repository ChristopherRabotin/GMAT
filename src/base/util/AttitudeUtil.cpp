//$Header$
//------------------------------------------------------------------------------
//                              RealUtilities
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
#include "RealTypes.hpp"
#include "RealUtilities.hpp"
#include "AttitudeUtil.hpp"

using namespace GmatMathUtil;
using namespace GmatAttUtil;


//------------------------------------------------------------------------------
//  Rvector3 ToEuler(Real rotAngle, Rvector3 rotAxis, Integer seq1, ...)
//------------------------------------------------------------------------------
/**
 * @return Euler angles for input sequence
 */
//------------------------------------------------------------------------------
Rvector3 GmatAttUtil::ToEulerAngles(Real rotAngle, const Rvector3 &rotAxis,
                                    Integer seq1, Integer seq2, Integer seq3)
{
   // convert to Quaternion
   Rvector3 unitVector = rotAxis.GetUnitVector();
   const Real sinThetaOverTwo = Sin(rotAngle / 2.0);
   
   Rvector3 qvec;
   qvec = unitVector * sinThetaOverTwo;
   Real qscalar = Cos(rotAngle / 2.0);
   
   // convert it to CosineMatrix
   Rmatrix33 mat = ToCosineMatrix(qvec, qscalar);
   
   // convert it to Euler angles
   Rvector3 eulerAngles = ToEulerAngles(mat, seq1, seq2, seq3);
   return eulerAngles;
}


//------------------------------------------------------------------------------
//  Rvector3 ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
//                         Integer seq2, Integer seq3)
//------------------------------------------------------------------------------
/**
 * @return Euler angles converted from the CosineMatrix and sequence
 */
//------------------------------------------------------------------------------
Rvector3 GmatAttUtil::ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
                                    Integer seq2, Integer seq3)
{
   const Integer i = seq1;
   const Integer j = seq2;
   const Integer k = seq3;
   
   Integer l;
   Rvector3 retVector(0.0, 0.0, 0.0);
   
//    if (((i == j) || (j == k)) && (j != 0))
//    {
//       throw AttitudeExceptions::InvalidEulerSequence();
//    }
   
   if (k != i)           // first and last axis different
   {
      if (j != 0)
      {
         retVector(1) = ASin(cosMat(k-1,i-1) * Real (((k-i+3) % 3) * 2 -3));
      }
      
      if (i != 0)
      {
         if (cosMat(k-1,j-1) != 0.0 || cosMat(k-1,k-1) != 0.0)
         {
            retVector(0) = ATan(cosMat(k-1,j-1) * 
                                Real (((k-j+3) % 3) * 2 - 3),
                                cosMat(k-1,k-1));
         }
      }
      if (k != 0)
      {
         if (cosMat(j-1,i-1) != 0.0 || cosMat(i-1,i-1) != 0.0)
         {
            retVector(2) = ATan(cosMat(j-1,i-1) * 
                                Real (((j-i+3) % 3) * 2 - 3),
                                cosMat(i-1,i-1));
         }
      }
   }
   else     //  First and last axes are the same
   {
      l = 6 - i - j; 
      if (j != 0)
      {
         retVector(1) = ACos(cosMat(k-1,k-1));
      }
      if (i != 0)
      {
         if (cosMat(k-1,j-1) != 0.0 || cosMat(k-1,l-1) != 0.0)
         {
            retVector(0) = ATan(cosMat(k-1,j-1),
                                cosMat(k-1,l-1) * Real(((k-l+3) % 3) * 2 -3));
         }
      }
      if (k != 0)
      {
         if (cosMat(j-1,i-1) != 0.0 || cosMat(l-1,i-1) != 0.0)
         {
            retVector(2) = ATan(cosMat(j-1,i-1),
                                cosMat(l-1,i-1) * Real(((l-i+3) % 3) * 2 -3));
         }
      }
   }
   
   return retVector;
}


//------------------------------------------------------------------------------
//  Rmatrix33 ToCosineMatrix(const Rvector3 &qVec, Real qScalar)
//------------------------------------------------------------------------------
/**
 * @return CosineMatrix converted form the Quaternion
 */
//------------------------------------------------------------------------------
Rmatrix33 GmatAttUtil::ToCosineMatrix(const Rvector3 &qVec, Real qScalar)
{
   Rmatrix33 mat;
   Rvector3 vec = qVec;
   
   Real q00, q01, q02, q03;
   Real      q11, q12, q13;
   Real           q22, q23;
   Real                q33;
   Real factor;
   
   // Set up terms to create matrix elements
   q00 = vec(0) * vec(0);
   q01 = vec(0) * vec(1);
   q02 = vec(0) * vec(2);
   q03 = vec(0) * qScalar;
   q11 = vec(1) * vec(1);
   q12 = vec(1) * vec(2);
   q13 = vec(1) * qScalar;
   q22 = vec(2) * vec(2);
   q23 = vec(2) * qScalar;
   q33 = qScalar * qScalar;
   factor = 1.0 / (q00 + q11 + q22 + q33);
   
   // Compute matrix elements
   mat(0,0) = (q00 - q11 - q22 + q33) * factor;
   mat(0,1) = 2.0 * (q01 + q23) * factor;
   mat(0,2) = 2.0 * (q02 - q13) * factor;
   mat(1,0) = 2.0 * (q01 - q23) * factor;
   mat(1,1) = (-q00 + q11 - q22 + q33) * factor;
   mat(1,2) = 2.0 * (q12 + q03) * factor;
   mat(2,0) = 2.0 * (q02 + q13) * factor;
   mat(2,1) = 2.0 * (q12 - q03) * factor;
   mat(2,2) = (-q00 - q11 + q22 + q33) * factor;

   return mat;
   
}


//------------------------------------------------------------------------------
//  Rmatrix33 ToCosineMatrix(eal rotAngle, const Rvector3 &rotAxis)
//------------------------------------------------------------------------------
/**
 * @return CosineMatrix converted form rotation angle and axis
 */
//------------------------------------------------------------------------------
Rmatrix33 GmatAttUtil::ToCosineMatrix(Real rotAngle, const Rvector3 &rotAxis)
{
   // convert to Quaternion
   Rvector3 unitVector = rotAxis.GetUnitVector();
   const Real sinThetaOverTwo = Sin(rotAngle / 2.0);
   
   Rvector3 qVec;
   qVec = unitVector * sinThetaOverTwo;
   Real qScalar = Cos(rotAngle / 2.0);
   
   // convert it to CosineMatrix
   Rmatrix33 mat = ToCosineMatrix(qVec, qScalar);

   return mat;
}


