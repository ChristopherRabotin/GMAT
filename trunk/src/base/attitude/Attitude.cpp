//$Header$
//------------------------------------------------------------------------------
//                               Attitude
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan/GSFC
// Created: 2006.03.02
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class implementation for the Attitude base class.
 * 
 * @note  The attitude is stored and maintained as a direction cosine matrix,
 *        and the angular velocity.
 *        Other representations are computed from that and returned when
 *        requested.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <sstream>
#include <iomanip>
#include "Attitude.hpp"
#include "AttitudeException.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_REF_SETTING
//#define DEBUG_ATTITUDE_GEN_STRING
//#define DEBUG_ATTITUDE_GET_REAL
//#define DEBUG_ATTITUDE_SET_REAL
//#define DEBUG_ATTITUDE_READ_ONLY
//#define DEBUG_EULER_ANGLE_RATES
//#define DEBUG_TO_DCM

//---------------------------------
// static data
//---------------------------------
const std::string
Attitude::PARAMETER_TEXT[AttitudeParamCount - GmatBaseParamCount] =
{
   "AttitudeDisplayStateType",
   "AttitudeRateDisplayStateType",
   "AttitudeCoordinateSystem",
   "Epoch",
   "Q1",
   "Q2",
   "Q3",
   "Q4",
   "EulerAngleSequence",
   "EulerAngle1",
   "EulerAngle2",
   "EulerAngle3",
   "DCM11",
   "DCM12",
   "DCM13",
   "DCM21",
   "DCM22",
   "DCM23",
   "DCM31",
   "DCM32",
   "DCM33",
   "EulerAngleRate1",
   "EulerAngleRate2",
   "EulerAngleRate3",
   "AngularVelocityX",
   "AngularVelocityY",
   "AngularVelocityZ",
};

const Gmat::ParameterType
Attitude::PARAMETER_TYPE[AttitudeParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,        // A1Mjd - should this be time_type?
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
};

const std::string
Attitude::OTHER_REP_TEXT[EndOtherReps - 7000] =
{
   //"ReferenceCoordinateSystemName",
   //"Epoch",
   "EulerSequenceList",
   //"EulerSequenceString", 
   "EulerSequenceArray",     // was "InitialEulerSequence",
   "EulerAngles",            // was "InitialEulerAngles",
   "EulerAngleRates",        // was "InitialEulerAngleRates",
   "Quaternion",             // was "InitialQuaternion",
   "DirectionCosineMatrix",  // was "InitialDirectionCosineMatrix",
   "AngularVelocity",        // was "InitialAngularVelocity",
};

const Gmat::ParameterType
Attitude::OTHER_REP_TYPE[EndOtherReps - 7000] =
{
   //Gmat::STRING_TYPE,
   //Gmat::REAL_TYPE,        // A1Mjd - should this be time_type?
   Gmat::STRINGARRAY_TYPE,
   //Gmat::STRING_TYPE,
   Gmat::UNSIGNED_INTARRAY_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RVECTOR_TYPE,
   Gmat::RMATRIX_TYPE,
   Gmat::RVECTOR_TYPE,
};

const std::string Attitude::EULER_SEQ_LIST[12] = 
{
   "123",
   "231",
   "312",
   "132",
   "321",
   "213",
   "121",
   "232",
   "313",
   "131",
   "323",
   "212"
};

const Real    Attitude::TESTACCURACY            = 1.19209290E-07;
const Real    Attitude::ATTITUDE_TIME_TOLERANCE = 1.0E-09;
const Integer Attitude::OTHER_REPS_OFFSET       = 7000;

//------------------------------------------------------------------------------
// static methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Rmatrix33   ToCosineMatrix(const Rvector &quat1)  [static]
//------------------------------------------------------------------------------
 /**
 * Converts the input quaternion to a direction cosine matrix.
 *
 * @param quat1  the input quaternion.
 *
 * @return the cosine direction matrix representation of the input attitude.  
 */
//------------------------------------------------------------------------------
Rmatrix33 Attitude::ToCosineMatrix(const Rvector &quat1)
{
   #ifdef DEBUG_TO_DCM
   MessageInterface::ShowMessage("ENTERING ToDCM(q) ... %.12f  %.12f  %.12f  %.12f\n",
   quat1[0], quat1[1], quat1[2], quat1[3]);
   #endif
   if (quat1.GetSize() != 4)
      throw AttitudeException(
            "Cannot convert quaternion of incorrect size to a cosine matrix.");
   Rmatrix33 I3;  // identity matrix, by default
   Rvector3  q1_3(quat1(0), quat1(1), quat1(2));
   Rmatrix33 q_x(     0.0, -q1_3(2),  q1_3(1), 
                q1_3(2),      0.0, -q1_3(0),
               -q1_3(1),  q1_3(0),     0.0);
   Real c = 1.0 / ((quat1(0) * quat1(0)) + (quat1(1) * quat1(1)) +
                   (quat1(2) * quat1(2)) + (quat1(3) * quat1(3)));
   return Rmatrix33(c * ( (((quat1(3) * quat1(3)) - (q1_3 * q1_3)) * I3) 
          + (2.0 * Outerproduct(q1_3,q1_3)) - (2.0 * quat1(3) * q_x)));
}

//------------------------------------------------------------------------------
//  Rmatrix33   ToCosineMatrix(const Rvector3 &eulerAngles,   [static]
//                             Integer seq1, Integer seq2, 
 //                            Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input euler angles and sequence to a direction cosine matrix.
 *
 * @param eulerAngles  the input euler angles (radians)
 * @param seq1         first entry of the euler sequence
 * @param seq2         second entry of the euler sequence
 * @param seq3         third entry of the euler sequence
 *
 * @return the cosine direction matrix representation of the input attitude.  
 */
//------------------------------------------------------------------------------
Rmatrix33 Attitude::ToCosineMatrix(const Rvector3 &eulerAngles, 
                                   Integer seq1, Integer seq2, 
                                   Integer seq3)
{
   #ifdef DEBUG_TO_DCM
   MessageInterface::ShowMessage("ENTERING ToDCM(eulerangles) ... %.12f  %.12f  %.12f\n",
   eulerAngles[0], eulerAngles[1], eulerAngles[2]);
   #endif
   if ((seq1 == 0) | (seq2 == 0) | (seq3 == 0))
      throw AttitudeException(
         "Euler sequence ill-defined for conversion to cosine matrix.");
   Real s1 = GmatMathUtil::Sin(eulerAngles(0));
   Real s2 = GmatMathUtil::Sin(eulerAngles(1));
   Real s3 = GmatMathUtil::Sin(eulerAngles(2));
   Real c1 = GmatMathUtil::Cos(eulerAngles(0));
   Real c2 = GmatMathUtil::Cos(eulerAngles(1));
   Real c3 = GmatMathUtil::Cos(eulerAngles(2));
   bool validSequence = true;
   if (seq1 == 1)
   {
      if (seq2 == 2)
      {
         if (seq3 == 1)      //  1-2-1
            return Rmatrix33(
                      c2,          s2*s1,           -s2*c1,
                   s3*s2, c3*c1-s3*c2*s1,   c3*s1+s3*c2*c1,
                   c3*s2, -s3*c1-c3*c2*s1, -s3*s1+c3*c2*c1);
         else if (seq3 == 3)  //  1-2-3
            return Rmatrix33(
                    c3*c2,  c3*s2*s1+s3*c1, -c3*s2*c1+s1*s3,
                   -s3*c2, -s3*s2*s1+c3*c1,  s3*s2*c1+c3*s1,
                       s2,          -c2*s1,           c2*c1);
         else validSequence = false;
      }  // seq2 == 2
      else if (seq2 == 3)
      {
         if (seq3 == 1)      //  1-3-1
            return Rmatrix33(
                       c2,            s2*c1,            s2*s1,
                   -c3*s2,   c3*c2*c1-s3*s1,   c3*c2*s1+s3*c1,
                    s3*s2,  -s3*c2*c1-c3*s1,  -s3*c2*s1+c3*c1);
         else if (seq3 == 2) //  1-3-2
            return Rmatrix33(
                   c3*c2, c3*s2*c1+s1*s3, c3*s2*s1-s3*c1,
                     -s2,          c2*c1,          c2*s1,
                   s3*c2, s3*s2*c1-c3*s1, s3*s2*s1+c3*c1);
         else  validSequence = false;
      } // seq2 == 3
      else  validSequence = false;
   }  // seq1 == 1
   else if (seq1 == 2)
   {
      if (seq2 == 1)
      {
         if (seq3 == 2)      //  2-1-2
            return Rmatrix33(
                   c3*c1-s3*c2*s1,  s3*s2, -c3*s1-s3*c2*c1,
                            s2*s1,     c2,           s2*c1,
                   s3*c1+c3*c2*s1, -c3*s2, -s3*s1+c3*c2*c1);
         else if (seq3 == 3) //  2-1-3
            return Rmatrix33(
                    c3*c1+s3*s2*s1, s3*c2, -c3*s1+s3*s2*c1,
                   -s3*c1+c3*s2*s1, c3*c2,  s3*s1+c3*s2*c1,
                             c2*s1,   -s2,           c2*c1);
                   
         else validSequence = false;
      } // seq2 == 1
      else if (seq2 == 3)
      {
         if (seq3 == 1)      //  2-3-1
            return Rmatrix33(
                             c2*c1,     s2,          -c2*s1,
                   -c3*s2*c1+s3*s1,  c3*c2,  c3*s2*s1+s3*c1,
                    s3*s2*c1+c3*s1, -s3*c2, -s3*s2*s1+c3*c1);
         else if (seq3 == 2) //  2-3-2
            return Rmatrix33(
                   c3*c2*c1-s3*s1, c3*s2, -c3*c2*s1-s3*c1,
                           -s2*c1,    c2,           s2*s1,
                   s3*c2*c1+c3*s1, s3*s2, -s3*c2*s1+c3*c1);
         else validSequence = false;
      } // seq2 == 3
      else validSequence = false;
   } // seq1 == 2
   else if (seq1 == 3)
   {
      if (seq2 == 1)
      {
         if (seq3 == 2)      //  3-1-2
            return Rmatrix33(
                   c3*c1-s3*s2*s1, c3*s1+s3*s2*c1, -s3*c2,
                           -c2*s1,          c2*c1,     s2,
                   s3*c1+c3*s2*s1, s3*s1-c3*s2*c1,  c3*c2);
         else if (seq3 == 3) //  3-1-3
            return Rmatrix33(
                    c3*c1-s3*c2*s1,  c3*s1+s3*c2*c1, s3*s2,
                   -s3*c1-c3*c2*s1, -s3*s1+c3*c2*c1, c3*s2,
                             s2*s1,          -s2*c1,    c2);
         else validSequence = false;
      } // seq2 == 1
      else if (seq2 == 2)
      {
         if (seq3 == 1)      //  3-2-1
            return Rmatrix33(
                             c2*c1,           c2*s1,   -s2,
                   -c3*s1+s3*s2*c1,  c3*c1+s3*s2*s1, s3*c2,
                    s3*s1+c3*s2*c1, -s3*c1+c3*s2*s1, c3*c2);
         else if (seq3 == 3) //  3-2-3
            return Rmatrix33(
                    c3*c2*c1-s3*s1,  c3*c2*s1+s3*c1,   -c3*s2,
                   -s3*c2*c1-c3*s1, -s3*c2*s1+c3*c1,    s3*s2,
                             s2*c1,           s2*s1,       c2);
         else validSequence = false;
      } // seq2 == 2
      else validSequence = false;
   }  // seq1 == 3
     
   if (!validSequence) throw AttitudeException(
      "Invalid euler sequence - cannot convert to cosine matrix.");
   // return zero matrix - it should never reach this point, though
   return Rmatrix33(false);
         
}  
                                          
//------------------------------------------------------------------------------
//  Rmatrix33   ToCosineMatrix(const Real *eulerAngles,   [static]
//                             Integer seq1, Integer seq2, 
 //                            Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input euler angles and sequence to a direction cosine matrix.
 *
 * @param eulerAngles  the input euler angles (radians)
 * @param seq1         first entry of the euler sequence
 * @param seq2         second entry of the euler sequence
 * @param seq3         third entry of the euler sequence
 *
 * @return the cosine direction matrix representation of the input attitude.  
 */
//------------------------------------------------------------------------------
Rmatrix33 Attitude::ToCosineMatrix(const Real *eulerAngles, 
                                   Integer seq1, Integer seq2, 
                                   Integer seq3)
{
   if ((seq1 == 0) | (seq2 == 0) | (seq3 == 0))
      throw AttitudeException(
         "Euler sequence ill-defined for conversion to cosine matrix.");
   Real s1 = GmatMathUtil::Sin(eulerAngles[0]);
   Real s2 = GmatMathUtil::Sin(eulerAngles[1]);
   Real s3 = GmatMathUtil::Sin(eulerAngles[2]);
   Real c1 = GmatMathUtil::Cos(eulerAngles[0]);
   Real c2 = GmatMathUtil::Cos(eulerAngles[1]);
   Real c3 = GmatMathUtil::Cos(eulerAngles[2]);
   bool validSequence = true;
   if (seq1 == 1)
   {
      if (seq2 == 2)
      {
         if (seq3 == 1)      //  1-2-1
            return Rmatrix33(
                      c2,          s2*s1,           -s2*c1,
                   s3*s2, c3*c1-s3*c2*s1,   c3*s1+s3*c2*c1,
                   c3*s2, -s3*c1-c3*c2*s1, -s3*s1+c3*c2*c1);
         else if (seq3 == 3)  //  1-2-3
            return Rmatrix33(
                    c3*c2,  c3*s2*s1+s3*c1, -c3*s2*c1+s1*s3,
                   -s3*c2, -s3*s2*s1+c3*c1,  s3*s2*c1+c3*s1,
                       s2,          -c2*s1,           c2*c1);
         else validSequence = false;
      }  // seq2 == 2
      else if (seq2 == 3)
      {
         if (seq3 == 1)      //  1-3-1
            return Rmatrix33(
                       c2,            s2*c1,            s2*s1,
                   -c3*s2,   c3*c2*c1-s3*s1,   c3*c2*s1+s3*c1,
                    s3*s2,  -s3*c2*c1-c3*s1,  -s3*c2*s1+c3*c1);
         else if (seq3 == 2) //  1-3-2
            return Rmatrix33(
                   c3*c2, c3*s2*c1+s1*s3, c3*s2*s1-s3*c1,
                     -s2,          c2*c1,          c2*s1,
                   s3*c2, s3*s2*c1-c3*s1, s3*s2*s1+c3*c1);
         else  validSequence = false;
      } // seq2 == 3
      else  validSequence = false;
   }  // seq1 == 1
   else if (seq1 == 2)
   {
      if (seq2 == 1)
      {
         if (seq3 == 2)      //  2-1-2
            return Rmatrix33(
                   c3*c1-s3*c2*s1,  s3*s2, -c3*s1-s3*c2*c1,
                            s2*s1,     c2,           s2*c1,
                   s3*c1+c3*c2*s1, -c3*s2, -s3*s1+c3*c2*c1);
         else if (seq3 == 3) //  2-1-3
            return Rmatrix33(
                    c3*c1+s3*s2*s1, s3*c2, -c3*s1+s3*s2*c1,
                   -s3*c1+c3*s2*s1, c3*c2,  s3*s1+c3*s2*c1,
                             c2*s1,   -s2,           c2*c1);
                   
         else validSequence = false;
      } // seq2 == 1
      else if (seq2 == 3)
      {
         if (seq3 == 1)      //  2-3-1
            return Rmatrix33(
                             c2*c1,     s2,          -c2*s1,
                   -c3*s2*c1+s3*s1,  c3*c2,  c3*s2*s1+s3*c1,
                    s3*s2*c1+c3*s1, -s3*c2, -s3*s2*s1+c3*c1);
         else if (seq3 == 2) //  2-3-2
            return Rmatrix33(
                   c3*c2*c1-s3*s1, c3*s2, -c3*c2*s1-s3*c1,
                           -s2*c1,    c2,           s2*s1,
                   s3*c2*c1+c3*s1, s3*s2, -s3*c2*s1+c3*c1);
         else validSequence = false;
      } // seq2 == 3
      else validSequence = false;
   } // seq1 == 2
   else if (seq1 == 3)
   {
      if (seq2 == 1)
      {
         if (seq3 == 2)      //  3-1-2
            return Rmatrix33(
                   c3*c1-s3*s2*s1, c3*s1+s3*s2*c1, -s3*c2,
                           -c2*s1,          c2*c1,     s2,
                   s3*c1+c3*s2*s1, s3*s1-c3*s2*c1,  c3*c2);
         else if (seq3 == 3) //  3-1-3
            return Rmatrix33(
                    c3*c1-s3*c2*s1,  c3*s1+s3*c2*c1, s3*s2,
                   -s3*c1-c3*c2*s1, -s3*s1+c3*c2*c1, c3*s2,
                             s2*s1,          -s2*c1,    c2);
         else validSequence = false;
      } // seq2 == 1
      else if (seq2 == 2)
      {
         if (seq3 == 1)      //  3-2-1
            return Rmatrix33(
                             c2*c1,           c2*s1,   -s2,
                   -c3*s1+s3*s2*c1,  c3*c1+s3*s2*s1, s3*c2,
                    s3*s1+c3*s2*c1, -s3*c1+c3*s2*s1, c3*c2);
         else if (seq3 == 3) //  3-2-3
            return Rmatrix33(
                    c3*c2*c1-s3*s1,  c3*c2*s1+s3*c1,   -c3*s2,
                   -s3*c2*c1-c3*s1, -s3*c2*s1+c3*c1,    s3*s2,
                             s2*c1,           s2*s1,       c2);
         else validSequence = false;
      } // seq2 == 2
      else validSequence = false;
   }  // seq1 == 3
     
   if (!validSequence) throw AttitudeException(
      "Invalid euler sequence - cannot convert to cosine matrix.");
   // return zero matrix - it should never reach this point, though
   return Rmatrix33(false);
         
}  
                                          
//------------------------------------------------------------------------------
//  Rvector3   ToEulerAngles(const Rvector &quat1, Integer seq1,  [static]
//                           Integer seq2,         Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input quaternion to a set of euler angles, using the euler
 * sequence provided.
 *
 * @param quat1  the input quaternion.
 * @param seq1   first entry of the euler sequence
 * @param seq2   second entry of the euler sequence
 * @param seq3   third entry of the euler sequence
 *
 * @return the euler angles representation of the input attitude (radians) 
 */
//------------------------------------------------------------------------------
Rvector3 Attitude::ToEulerAngles(const Rvector &quat1, Integer seq1,
                                 Integer seq2,         Integer seq3)
{
   return ToEulerAngles(ToCosineMatrix(quat1),seq1, seq2, seq3);  
}
                                        
//------------------------------------------------------------------------------
//  Rvector3   ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,  [static]
//                           Integer seq2,            Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input quaternion to a set of euler angles, using the euler
 * sequence provided.
 *
 * @param cosMat  the input cosine matrix.
 * @param seq1    first entry of the euler sequence
 * @param seq2    second entry of the euler sequence
 * @param seq3    third entry of the euler sequence
 *
 * @return the euler angles representation (radians) of the input attitude.  
 */
//------------------------------------------------------------------------------
Rvector3 Attitude::ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
                                 Integer seq2,            Integer seq3)
{
   Real theta1, theta2, sin1, cos1;

   Real R11  = cosMat(0,0);
   Real R12  = cosMat(0,1);
   Real R13  = cosMat(0,2);
   Real R21  = cosMat(1,0);
   Real R22  = cosMat(1,1);
   Real R23  = cosMat(1,2);
   Real R31  = cosMat(2,0);
   Real R32  = cosMat(2,1);
   Real R33  = cosMat(2,2);

   if ((seq1 == 1) && (seq2 == 2) && (seq3 == 3))          // 1-2-3
   {
      theta1 = GmatMathUtil::ATan(-R32, R33);
      theta2 = GmatMathUtil::ASin(R31);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((R13*sin1 + R12*cos1),(R23*sin1 + R22*cos1)));
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 2))     // 1-3-2
   {
      theta1 = GmatMathUtil::ATan(R23, R22);
      theta2 = GmatMathUtil::ASin(-R21);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((R12*sin1 - R13*cos1),(-R32*sin1 + R33*cos1)));
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 1))     // 2-3-1
   {
      theta1 = GmatMathUtil::ATan(-R13, R11);
      theta2 = GmatMathUtil::ASin(R12);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((R21*sin1 + R23*cos1),(R31*sin1 + R33*cos1)));
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 3))     // 2-1-3
   {
      theta1 = GmatMathUtil::ATan(R31, R33);
      theta2 = GmatMathUtil::ASin(-R32);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((R23*sin1 - R21*cos1)/(-R13*sin1 + R11*cos1)));
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 2))     // 3-1-2
   {
      theta1 = GmatMathUtil::ATan(-R21, R22);
      theta2 = GmatMathUtil::ASin(R23);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((R32*sin1 + R31*cos1),(R12*sin1 + R11*cos1)));
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 1))     // 3-2-1
   {
      theta1 = GmatMathUtil::ATan(R12, R11);
      theta2 = GmatMathUtil::ASin(-R13);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((R31*sin1 - R32*cos1),(-R21*sin1 + R22*cos1)));
   }
   else if ((seq1 == 1) && (seq2 == 2) && (seq3 == 1))     // 1-2-1
   {
      theta1 = GmatMathUtil::ATan(R12, -R13);
      theta2 = GmatMathUtil::ACos(R11);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((-R33*sin1 - R32*cos1),(R23*sin1 + R22*cos1)));
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 1))     // 1-3-1
   {
      theta1 = GmatMathUtil::ATan(R13, R12);
      theta2 = GmatMathUtil::ACos(R11);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((-R22*sin1 + R23*cos1),(-R32*sin1 + R33*cos1)));
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 2))     // 2-1-2
   {
      theta1 = GmatMathUtil::ATan(R21, R23);
      theta2 = GmatMathUtil::ACos(R22);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((-R33*sin1 + R31*cos1),(-R13*sin1 + R11*cos1)));
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 2))     // 2-3-2
   {
      theta1 = GmatMathUtil::ATan(R23, -R21);
      theta2 = GmatMathUtil::ACos(R22);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((-R11*sin1 - R13*cos1),(R31*sin1 + R33*cos1)));
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 3))     // 3-1-3
   {
      theta1 = GmatMathUtil::ATan(R31, -R32);
      theta2 = GmatMathUtil::ACos(R33);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((-R22*sin1 - R21*cos1),(R12*sin1 + R11*cos1)));
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 3))     // 3-2-3
   {
      theta1 = GmatMathUtil::ATan(R32, R31);
      theta2 = GmatMathUtil::ACos(R33);
      sin1   = GmatMathUtil::Sin(theta1);
      cos1   = GmatMathUtil::Cos(theta1);  
      return Rvector3(theta1, theta2, 
            GmatMathUtil::ATan((-R11*sin1 + R12*cos1),(-R21*sin1 + R22*cos1)));
   }
   else
      throw AttitudeException(
      "Invalid Euler sequence - cannot convert cosine matrix to euler angles.");
}
                                        
//------------------------------------------------------------------------------
//  Rvector   ToQuaternion(const Rvector3 &eulerAngles, Integer seq1,  [static]
//                         Integer seq2,                Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input set of euler angles to a quaternion, using the euler
 * sequence provided.
 *
 * @param eulerAngles  the input euler angles (radians)
 * @param seq1         first entry of the euler sequence
 * @param seq2         second entry of the euler sequence
 * @param seq3         third entry of the euler sequence
 *
 * @return the quaternion representation of the input attitude.  
 */
//------------------------------------------------------------------------------
Rvector Attitude::ToQuaternion(const Rvector3 &eulerAngles, 
                               Integer seq1, Integer seq2, Integer seq3)
{
   return ToQuaternion(ToCosineMatrix(eulerAngles, seq1, seq2, seq3));
}
                                      
//------------------------------------------------------------------------------
//  Rvector   ToQuaternion(const Rmatrix33 &cosMat)             [static]
//------------------------------------------------------------------------------
 /**
 * Converts the input cosince matrix to a quaternion.
 *
 * @param cosMat  the input cosine matrix.
 *
 * @return the quaternion representation of the input attitude.  
 */
//------------------------------------------------------------------------------
Rvector Attitude::ToQuaternion(const Rmatrix33 &cosMat)
{
   Real R11  = cosMat(0,0);
   Real R12  = cosMat(0,1);
   Real R13  = cosMat(0,2);
   Real R21  = cosMat(1,0);
   Real R22  = cosMat(1,1);
   Real R23  = cosMat(1,2);
   Real R31  = cosMat(2,0);
   Real R32  = cosMat(2,1);
   Real R33  = cosMat(2,2);
   
   Real    q1, q2, q3, q4;
   Real    matT   = cosMat.Trace();
   Real    v[4]   = {R11, R22, R33, matT};
   // find index of maximum component of v
   Integer maxI   = 0;
   Real    maxNum = v[0];
   for (Integer i = 1; i < 4; i++)
      if (v[i] > maxNum)
      {
         maxNum = v[i];
         maxI   = i;
      }
   if (maxI == 0)
   {
      q1 = 2.0 * v[0] + 1.0 - matT;
      q2 = R12 + R21;
      q3 = R13 + R31;
      q4 = R23 - R32;
   }
   else if (maxI == 1)
   {
      q1 = R21 + R12;
      q2 = 2.0 * v[1] + 1.0 - matT;
      q3 = R23 + R32;
      q4 = R31 - R13;
   }
   else if (maxI == 2)
   {
      q1 = R31 + R13;
      q2 = R32 + R23;
      q3 = 2.0 * v[2] + 1.0 - matT;
      q4 = R12 - R21;
   }
   else // maxI == 4
   {
      q1 = R23 - R32;
      q2 = R31 - R13;
      q3 = R12 - R21;
      q4 = 1.0 + matT;
   }
   
   return (Rvector(4,q1,q2,q3,q4)).Normalize();
}

   
//------------------------------------------------------------------------------
//  Rvector3   ToEulerAngleRates(const Rvector3 &angVel,         [static]
//                               const Rvector3 &eulerAngles,
//                               Integer seq1, Integer seq2, 
//                               Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input angular velocity to a set of euler angle rates, 
 * using the euler sequence provided.
 *
 * @param angVel       the input angular velocity (radians/sec)
 * @param eulerAngles  the input euler angles (radians)
 * @param seq1         first entry of the euler sequence
 * @param seq2         second entry of the euler sequence
 * @param seq3         third entry of the euler sequence
 *
 * @return the euler angle rates representation (radians/second).
 * @note Obviously, the angular velocity and euler angles passed in must have 
 *       been computed at the same time.
 */
//------------------------------------------------------------------------------
Rvector3 Attitude::ToEulerAngleRates(const Rvector3 &angularVel, 
                                     const Rvector3 &eulerAngles,
                                     Integer seq1, Integer seq2, 
                                     Integer seq3)
{
   bool      singularity = false;
   Real      s2          = GmatMathUtil::Sin(eulerAngles(1));
   Real      c2          = GmatMathUtil::Cos(eulerAngles(1));
   Real      s3          = GmatMathUtil::Sin(eulerAngles(2));
   Real      c3          = GmatMathUtil::Cos(eulerAngles(2));
   Rmatrix33 Si;
   
   if ((seq1 == 1) && (seq2 == 2) && (seq3 == 3))        // 1-2-3
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(    c3/c2,   -s3/c2, 0.0,
                       s3,       c3, 0.0,
                -c3*s2/c2, s3*s2/c2, 1.0);
      }
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 2))  // 1-3-2
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(   c3/c2, 0.0,    s3/c2,
                     -s3, 0.0,       c3,
                c3*s2/c2, 1.0, s3*s2/c2);
      }
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 1))  // 2-3-1
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(0.0,     c3/c2,   -s3/c2,
                0.0,        s3,       c3,
                1.0, -c3*s2/c2, s3*s2/c2);
      }
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 3))  // 2-1-3
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(   s3/c2,    c3/c2, 0.0,
                      c3,      -s3, 0.0,
                s3*s2/c2, c3*s2/c2, 1.0);
      }
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 2))  // 3-1-2
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(  -s3/c2, 0.0,     c3/c2,
                      c3, 0.0,        s3,
                s3*s2/c2, 1.0, -c3*s2/c2);
      }
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 1))  // 3-2-1
   {
      if (c2 == 0.0)   singularity = true;
      else
      {
         Si.Set(0.0,    s3/c2,    c3/c2,
                0.0,       c3,      -s3,
                1.0, s3*s2/c2, c3*s2/c2);
      }
   }
   else if ((seq1 == 1) && (seq2 == 2) && (seq3 == 1))  // 1-2-1
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(0.0,     s3/s2,     c3/s2,
                0.0,        c3,       -s3,
                1.0, -s3*c2/s2, -c3*c2/s2);
      }
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 1))  // 1-3-1
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(0.0,   -c3/s2,     s3/s2,
                0.0,       s3,        c3,
                1.0, c3*c2/s2, -s3*c2/s2);
      }
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 2))  // 2-1-2
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(    s3/s2, 0.0,   -c3/s2,
                       c3, 0.0,       s3,
                -s3*c2/s2, 1.0, c3*c2/s2);
      }
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 2))  // 2-3-2
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(    c3/s2, 0.0,     s3/s2,
                      -s3, 0.0,        c3,
                -c3*c2/s2, 1.0, -s3*c2/s2);
      }
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 3))  // 3-1-3
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(    s3/s2,     c3/s2, 0.0,
                       c3,       -s3, 0.0,
                -s3*c2/s2, -c3*c2/s2, 1.0);
      }
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 3))  // 3-2-3
   {
      if (s2 == 0.0)   singularity = true;
      else
      {
         Si.Set(   -c3/s2,     s3/s2, 0.0,
                       s3,        c3, 0.0,
                 c3*c2/s2, -s3*c2/s2, 1.0);
      }
   }
   else
       throw AttitudeException(
      "Invalid Euler sequence - cannot compute euler angle rates.");
   if (singularity)
   {
      MessageInterface::ShowMessage(
      "Singularity detected - using zero vector for euler angle rates\n");
      return Rvector3(); // return zero vector
   }
   return Si * angularVel;
 }
                                            
//------------------------------------------------------------------------------
//  Rvector3   ToAngularVelocity(const Rvector3 &eulerRates,       [static]
//                               const Rvector3 &eulerAngles,
//                               Integer seq1, Integer seq2, 
//                               Integer seq3)
//------------------------------------------------------------------------------
 /**
 * Converts the input euler angle rates to an angular velocity, 
 * using the euler sequence provided.
 *
 * @param eulerRates   the input euler angle rates (radians/sec)
 * @param eulerAngles  the input euler angles (radians)
 * @param seq1         first entry of the euler sequence
 * @param seq2         second entry of the euler sequence
 * @param seq3         third entry of the euler sequence
 *
 * @return the angular velocity representation (radians/second).
 * @note Obviously, the euler rates and euler angles passed in must have been
 *       computed at the same time.
 */
//------------------------------------------------------------------------------
Rvector3 Attitude::ToAngularVelocity(const Rvector3 &eulerRates, 
                                     const Rvector3 &eulerAngles,
                                     Integer seq1, Integer seq2, 
                                     Integer seq3)
{
   Real s2 = GmatMathUtil::Sin(eulerAngles(1));
   Real c2 = GmatMathUtil::Cos(eulerAngles(1));
   Real s3 = GmatMathUtil::Sin(eulerAngles(2));
   Real c3 = GmatMathUtil::Cos(eulerAngles(2));
   Rmatrix33 S;
   
   if ((seq1 == 1) && (seq2 == 2) && (seq3 == 3))        // 1-2-3
   {
      S.Set( c3*c2,  s3, 0.0,
            -s3*c2,  c3, 0.0,
                s2, 0.0, 1.0);
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 2))  // 1-3-2
   {
      S.Set(c3*c2, -s3, 0.0,
              -s2, 0.0, 1.0,
            s3*c2,  c3, 0.0);
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 1))  // 2-3-1
   {
      S.Set(    s2, 0.0, 1.0,
             c3*c2,  s3, 0.0,
            -s3*c2,  c3, 0.0);
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 3))  // 2-1-3
   {
      S.Set(s3*c2,  c3, 0.0,
            c3*c2, -s3, 0.0,
              -s2, 0.0, 1.0);
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 2))  // 3-1-2
   {
      S.Set(-s3*c2,  c3, 0.0,
                s2, 0.0, 1.0,
             c3*c2,  s3, 0.0);
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 1))  // 3-2-1
   {
      S.Set(  -s2, 0.0, 1.0,
            s3*c2,  c3, 0.0,
            c3*c2, -s3, 0.0);
   }
   else if ((seq1 == 1) && (seq2 == 2) && (seq3 == 1))  // 1-2-1
   {
      S.Set(   c2, 0.0, 1.0,
            s3*s2,  c3, 0.0,
            c3*s2, -s3, 0.0);
   }
   else if ((seq1 == 1) && (seq2 == 3) && (seq3 == 1))  // 1-3-1
   {
      S.Set(    c2, 0.0, 1.0,
            -c3*s2,  s3, 0.0,
             s3*s2,  c3, 0.0);
   }
   else if ((seq1 == 2) && (seq2 == 1) && (seq3 == 2))  // 2-1-2
   {
      S.Set( s3*s2,  c3, 0.0,
                c2, 0.0, 1.0,
            -c3*s2,  s3, 0.0);
   }
   else if ((seq1 == 2) && (seq2 == 3) && (seq3 == 2))  // 2-3-2
   {
      S.Set(c3*s2, -s3, 0.0,
               c2, 0.0, 1.0,
            s3*s2,  c3, 0.0);
   }
   else if ((seq1 == 3) && (seq2 == 1) && (seq3 == 3))  // 3-1-3
   {
      S.Set(s3*s2,  c3, 0.0,
            c3*s2, -s3, 0.0,
               c2, 0.0, 1.0);
   }
   else if ((seq1 == 3) && (seq2 == 2) && (seq3 == 3))  // 3-2-3
   {
      S.Set(-c3*s2,  s3, 0.0,
             s3*s2,  c3, 0.0,
                 c2, 0.0, 1.0);
   }
   else
       throw AttitudeException(
      "Invalid Euler sequence - cannot compute euler angle rates.");
   
   return S * eulerRates;
}
  
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
StringArray Attitude::GetEulerSequenceStrings()
{
   static StringArray eulerStrings;  // compiler didn't like this in the header
   if (eulerStrings.size() == 0)  // set it, if it hasn't been already
   {
      for (Integer i = 0; i < 12; i++)
         eulerStrings.push_back(EULER_SEQ_LIST[i]);
   }
   return eulerStrings;
}

//------------------------------------------------------------------------------
//  UnsignedIntArray  ExtractEulerSequence(const std::string &seqStr)
//------------------------------------------------------------------------------
/**
 * This method sets the state data for the euler sequence array, given
 * the input string representation of the euler sequnce. 
 *
 * @param <eulerArray>  string representation of euler sequence.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
UnsignedIntArray Attitude::ExtractEulerSequence(const std::string &seqStr)
{
   UnsignedIntArray intSeq;
   const char *tmpStr = seqStr.c_str();
   if (tmpStr[0] == '1')      intSeq.push_back(1);
   else if (tmpStr[0] == '2') intSeq.push_back(2);
   else if (tmpStr[0] == '3') intSeq.push_back(3);
   else
   throw AttitudeException("Invalid character in euler sequence string.");
   if (tmpStr[1] == '1')      intSeq.push_back(1);
   else if (tmpStr[1] == '2') intSeq.push_back(2);
   else if (tmpStr[1] == '3') intSeq.push_back(3);
   else
   throw AttitudeException("Invalid character in euler sequence string.");
   if (tmpStr[2] == '1')      intSeq.push_back(1);
   else if (tmpStr[2] == '2') intSeq.push_back(2);
   else if (tmpStr[2] == '3') intSeq.push_back(3);
   else
   throw AttitudeException("Invalid character in euler sequence string.");
   return intSeq;
}

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Attitude(const std::string &typeStr, const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Attitude class (Constructor).
 * The default value is the (0,0,0,1) quaternion.
 */
//------------------------------------------------------------------------------
Attitude::Attitude(const std::string &typeStr, const std::string &itsName) : 
   GmatBase(Gmat::ATTITUDE, typeStr, itsName),
   inputAttType            (GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE),
   inputAttRateType        (GmatAttitude::ANGULAR_VELOCITY_TYPE),
   attitudeDisplayType     ("Quaternion"),
   attitudeRateDisplayType ("AngularVelocity"),
   isInitialized           (false),
   epoch                   (0.0),
   refCSName               ("EarthMJ2000Eq"),
   refCS                   (NULL),
   eulerSequence           ("312"),
   attitudeTime            (0.0),
   quaternionTime          (0.0),
   quaternion              (Rvector(4,0.0,0.0,0.0,1.0)),
   eulerAngleTime          (0.0),
   eulerAngleRatesTime     (0.0),
   attitudeModelName       ("")
{
   parameterCount = AttitudeParamCount;
   objectTypes.push_back(Gmat::ATTITUDE);
   objectTypeNames.push_back("Attitude");
   
   // push the default sequence to the array
   unsigned int defSeq[3] = {3, 1, 2};
   eulerSequenceArray.push_back(defSeq[0]);
   eulerSequenceArray.push_back(defSeq[1]);
   eulerSequenceArray.push_back(defSeq[2]);
   // create the list of all the possible euler sequences
   for (Integer i = 0; i < 12; i++)
      eulerSequenceList.push_back(EULER_SEQ_LIST[i]);
 }
 
//------------------------------------------------------------------------------
//  Attitude(const Attitude &att)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Attitude class as a copy of the
 * specified Attitude class (copy constructor).
 *
 * @param <att> Attitude object to copy.
 */
//------------------------------------------------------------------------------
Attitude::Attitude(const Attitude& att) :
   GmatBase(att),
   inputAttType            (att.inputAttType),
   inputAttRateType        (att.inputAttRateType),
   attitudeDisplayType     (att.attitudeDisplayType),
   attitudeRateDisplayType (att.attitudeRateDisplayType),
   isInitialized           (false),
   eulerSequenceList       (att.eulerSequenceList),
   epoch                   (att.epoch),
   refCSName               (att.refCSName),
   refCS                   (NULL),
   eulerSequence           (att.eulerSequence),
   eulerSequenceArray      (att.eulerSequenceArray),
   RBi                     (att.RBi),
   wIBi                    (att.wIBi),
   cosMat                  (att.cosMat),
   angVel                  (att.angVel),
   attitudeTime            (att.attitudeTime),
   quaternionTime          (att.quaternionTime),
   quaternion              (att.quaternion),
   eulerAngleTime          (att.eulerAngleTime),
   eulerAngles             (att.eulerAngles),
   eulerAngleRatesTime     (att.eulerAngleRatesTime),
   eulerAngleRates         (att.eulerAngleRates),
   attitudeModelName       (att.attitudeModelName)
{
}
 
//------------------------------------------------------------------------------
//  Attitude& operator= (const Attitude& att)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Attitude class.
 *
 * @param att the Attitude object whose data to assign to "this"
 *            Attitude.
 *
 * @return "this" Attitude with data of input Attitude att.
 */
//------------------------------------------------------------------------------
Attitude& Attitude::operator=(const Attitude& att)
{
   GmatBase::operator=(att);
   inputAttType            = att.inputAttType;
   inputAttRateType        = att.inputAttRateType;
   attitudeDisplayType     = att.attitudeDisplayType;
   attitudeRateDisplayType = att.attitudeRateDisplayType;
   isInitialized           = false;
   eulerSequenceList       = att.eulerSequenceList;
   epoch                   = att.epoch;
   refCSName               = att.refCSName;
   refCS                   = att.refCS;
   eulerSequence           = att.eulerSequence;
   eulerSequenceArray      = att.eulerSequenceArray;
   RBi                     = att.RBi;
   wIBi                    = att.wIBi;
   cosMat                  = att.cosMat;
   angVel                  = att.angVel;
   attitudeTime            = att.attitudeTime;
   quaternionTime          = att.quaternionTime;
   quaternion              = att.quaternion;
   eulerAngleTime          = att.eulerAngleTime;
   eulerAngles             = att.eulerAngles;
   eulerAngleRatesTime     = att.eulerAngleRatesTime;
   eulerAngleRates         = att.eulerAngleRates;
   attitudeModelName       = att.attitudeModelName;
   return *this;
}


//------------------------------------------------------------------------------
//  ~Attitude()
//------------------------------------------------------------------------------
/**
 * Destructor for the Attitude class.
 */
//------------------------------------------------------------------------------
Attitude::~Attitude()
{
   eulerSequenceArray.clear();
   // nothing else to do here ... la la la la la
}

//---------------------------------------------------------------------------
//  bool Initialize() 
//---------------------------------------------------------------------------
 /**
 * Initializes the attitude, including computing the initial RBi and
 * wIBi data members.
 * 
 * @return Success flag.
 *
  */
//---------------------------------------------------------------------------
bool Attitude::Initialize()
{ 
   if (isInitialized) return true;
   GmatBase::Initialize();
   std::string attEx  = "Reference coordinate system not defined for attitude of type \"";
   attEx             += typeName + "\"";
   if (!refCS) throw AttitudeException(attEx);
    
   // compute cosine matrix and angular velocity from inputs and
   // synchronize all the values at the epoch time

   switch (inputAttType)
   {
      case GmatAttitude::QUATERNION_TYPE:
         quaternion.Normalize(); 
         cosMat = Attitude::ToCosineMatrix(quaternion);
         break;
      case GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE: 
         ValidateCosineMatrix(cosMat);
         break;
      case GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE:
         ValidateEulerSequence(eulerSequence);
         cosMat = Attitude::ToCosineMatrix( // wcs 2007.06.12 - to radians on input
                            //eulerAngles * GmatMathUtil::RAD_PER_DEG, 
                            eulerAngles, 
                            (Integer) eulerSequenceArray.at(0),
                            (Integer) eulerSequenceArray.at(1), 
                            (Integer) eulerSequenceArray.at(2));
         break;
      default:
         break;
   };

   switch (inputAttRateType)
   {
      case GmatAttitude::ANGULAR_VELOCITY_TYPE:
         //angVel = angVel * GmatMathUtil::RAD_PER_DEG;
         break;
      case GmatAttitude::EULER_ANGLE_RATES_TYPE:
         ValidateEulerSequence(eulerSequence);
         angVel = Attitude::ToAngularVelocity(
                            //eulerAngleRates * GmatMathUtil::RAD_PER_DEG, 
                            //eulerAngles * GmatMathUtil::RAD_PER_DEG, 
                            eulerAngleRates, 
                            eulerAngles, 
                            (Integer) eulerSequenceArray.at(0), 
                            (Integer) eulerSequenceArray.at(1),
                            (Integer) eulerSequenceArray.at(2));
      default:
         break;
   }

   RBi          = cosMat;
   wIBi         = angVel;
   attitudeTime = epoch;

   isInitialized = true;
   return true;
}

//---------------------------------------------------------------------------
//  const Real GetEpoch() const
//---------------------------------------------------------------------------
 /**
 * Returns the epoch for the attitude.
 * 
 * @return toEpoch the epoch for this attitude.
 *
  */
//---------------------------------------------------------------------------
Real Attitude::GetEpoch() const
{
   return epoch;
}

//---------------------------------------------------------------------------
//  void SetEpoch(Real toEpoch)
//---------------------------------------------------------------------------
 /**
 * Sets the epoch for the attitude.
 * 
 * @param toEpoch the epoch to use for this attitude.
 *
  */
//---------------------------------------------------------------------------
void Attitude::SetEpoch(Real toEpoch)
{
   epoch         = toEpoch;
   isInitialized = false;  // is this correct?
}


//---------------------------------------------------------------------------
//  bool    SetReferenceCoordinateSystemName(
//          const std::string &refName) const
//---------------------------------------------------------------------------
 /**
 * sets the name of the reference coordinate system for the Attitude object.
 * 
 * @param refName name of the reference coordinate system.
 *
 * @return success flag.  
 */
//---------------------------------------------------------------------------
bool Attitude::SetReferenceCoordinateSystemName(
                            const std::string &refName)
{
   refCSName = refName;
   isInitialized = false;  // is this correct?
   return true;
}

//---------------------------------------------------------------------------
//  const StringArray&   GetEulerSequenceList() const
//---------------------------------------------------------------------------
 /**
 * Returns the list of valid euler sequences, as a vector of strings.
 *
 * @return the list of valid euler sequences.  
 */
//---------------------------------------------------------------------------
const StringArray& Attitude::GetEulerSequenceList() const
{
   return eulerSequenceList;
}

 
//---------------------------------------------------------------------------
//  const Rvector&   GetQuaternion(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as a quaternion.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the quaternion representation of the attitude, computed at
 *         time atTime.  
 */
//---------------------------------------------------------------------------
const Rvector&   Attitude::GetQuaternion(Real atTime)
{
   if (GmatMathUtil::Abs(atTime - attitudeTime) > 
       ATTITUDE_TIME_TOLERANCE)
   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
   }
   if (GmatMathUtil::Abs(atTime - quaternionTime) > ATTITUDE_TIME_TOLERANCE)
   {
      quaternion       = Attitude::ToQuaternion(cosMat);
      quaternionTime   = atTime;
   }
   return quaternion;
}

//---------------------------------------------------------------------------
//  const Rvector3&   GetEulerAngles(Real tTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as an array of Euler angles.  This
 * method assumes the previously set Euler sequence.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the euler angle representation of the attitude, computed at
 *         time atTime (radians).  
 */
//---------------------------------------------------------------------------
const Rvector3&  Attitude::GetEulerAngles(Real atTime)
{
   if (GmatMathUtil::Abs(atTime - attitudeTime) > 
       ATTITUDE_TIME_TOLERANCE)
   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
   }

   if (GmatMathUtil::Abs(atTime - eulerAngleTime) > ATTITUDE_TIME_TOLERANCE)
   {
      eulerAngles = Attitude::ToEulerAngles(cosMat, 
                              (Integer) eulerSequenceArray.at(0),
                              (Integer) eulerSequenceArray.at(1), 
                              (Integer) eulerSequenceArray.at(2));
      eulerAngleTime = atTime;
   }
   return eulerAngles;
}

//---------------------------------------------------------------------------
//  const Rvector3&   GetEulerAngles(Real atTime,  
//                                   Integer seq1, Integer seq2, 
//                                   Integer seq3)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as an array of Euler angles.  This
 * method uses the Euler sequence passed in here.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the euler angle representation of the attitude, computed at
 *         time atTime (radians).  
 * @todo - figure out what to do if the time is the same, but the 
 *         Euler sequence is different that the last time it was computed.
 */
//---------------------------------------------------------------------------
const Rvector3&  Attitude::GetEulerAngles(Real atTime,  Integer seq1, 
                                          Integer seq2, Integer seq3)
{
   if (GmatMathUtil::Abs(atTime - attitudeTime) > 
       ATTITUDE_TIME_TOLERANCE)
   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
   }
   if (GmatMathUtil::Abs(atTime - eulerAngleTime) > ATTITUDE_TIME_TOLERANCE)
   {
      eulerAngles = Attitude::ToEulerAngles(cosMat, seq1, seq2, seq3);
      eulerAngleTime = atTime;
   }
   return eulerAngles;
}
                                          
//---------------------------------------------------------------------------
//  const Rmatrix33&   GetCosineMatrix(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude at time atTime as direction cosine matrix. 
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the direction cosine matrix representation of the attitude, 
 *         computed at time atTime.  
 */
//---------------------------------------------------------------------------
const Rmatrix33& Attitude::GetCosineMatrix(Real atTime)
{
   if (GmatMathUtil::Abs(atTime - attitudeTime) > 
       ATTITUDE_TIME_TOLERANCE)
   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
   }

   return cosMat;
}
 
//---------------------------------------------------------------------------
//  const Rvector3&   GetAngularVelocity(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude rates at time atTime as an angular velocity.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the angular velocity representation of the attitude rates, 
 *         computed at time atTime (radians/second).  
 */
//---------------------------------------------------------------------------
const Rvector3& Attitude::GetAngularVelocity(Real atTime)
{
   if (GmatMathUtil::Abs(atTime - attitudeTime) > 
       ATTITUDE_TIME_TOLERANCE)
   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
   }
   return angVel;
}

//---------------------------------------------------------------------------
//  const Rvector3&   GetEulerAngleRates(Real atTime)
//---------------------------------------------------------------------------
 /**
 * Returns the attitude rates at time atTime as an array of euler 
 * angle rates.
 *
 * @param atTime  time at which to compute the attitude.
 *
 * @return the euler angle rates representation of the attitude rates, 
 *         computed at time atTime (radians/second).  
 */
//---------------------------------------------------------------------------
const Rvector3& Attitude::GetEulerAngleRates(Real atTime)
{
   #ifdef DEBUG_EULER_ANGLE_RATES
   MessageInterface::ShowMessage("Entering Attitude::GetEulerAngleRates ...\n");
   MessageInterface::ShowMessage(
   "   with atTime = %.12f, attitudeTime = %.12f, and eulerAngleRatesTime = %.12f\n",
   atTime, attitudeTime, eulerAngleRatesTime);
   #endif
   if (GmatMathUtil::Abs(atTime - attitudeTime) > 
       ATTITUDE_TIME_TOLERANCE)
   {
      ComputeCosineMatrixAndAngularVelocity(atTime);
      attitudeTime = atTime;
   }
   if (GmatMathUtil::Abs(atTime - eulerAngleRatesTime) > 
       ATTITUDE_TIME_TOLERANCE)
   {
      eulerAngles       = GetEulerAngles(atTime);
      eulerAngleRates   = Attitude::ToEulerAngleRates(angVel, 
                          eulerAngles,
                          (Integer) eulerSequenceArray.at(0),  
                          (Integer) eulerSequenceArray.at(1), 
                          (Integer) eulerSequenceArray.at(2));
      eulerAngleRatesTime = atTime;
   }
   return eulerAngleRates;
}


//---------------------------------------------------------------------------
//  std::string   GetAttitudeModelName()
//---------------------------------------------------------------------------
 /**
 * Returns the name of the attitude model.
 *
 * @return the name of the attitude model (e.g. "Spinner").  
 */
//---------------------------------------------------------------------------
std::string Attitude::GetAttitudeModelName() const
{
   return attitudeModelName;
}

//------------------------------------------------------------------------------
//   std::string GetRefObjectName(const Gmat::ObjectType type) const
//------------------------------------------------------------------------------
/**
 * Returns the name of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.  
 *
 * @return The name of the reference object.
 */
//------------------------------------------------------------------------------
std::string Attitude::GetRefObjectName(const Gmat::ObjectType type) const
{
   if ((type == Gmat::UNKNOWN_OBJECT) ||
       (type == Gmat::COORDINATE_SYSTEM))
   {
       return refCSName;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObjectName(type);
}


//------------------------------------------------------------------------------
//   bool SetRefObjectName(const Gmat::ObjectType type,
//                         const std::string &name) 
//------------------------------------------------------------------------------
/**
 * Sets the name of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param type   reference object type.  
 * @param name   name to sue for the object
 *
 * @return Success flag.
 */
//------------------------------------------------------------------------------
bool Attitude::SetRefObjectName(const Gmat::ObjectType type,
                                const std::string &name)
{
   if ((type == Gmat::UNKNOWN_OBJECT) ||
       (type == Gmat::COORDINATE_SYSTEM))
   {
      refCSName = name;
      return true;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return GmatBase::SetRefObjectName(type, name);
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName,
//                       const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Renames a reference object. 
 *
 * @param <type>    reference object type.   
 * @param <oldName> the old name for the object.   
 * @param <newName> the new name for the object.  
 *
 * @return success flag
 */
//------------------------------------------------------------------------------
bool Attitude::RenameRefObject(const Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   bool success = false;
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      if (refCSName == oldName)
      {
         refCSName = newName;
         success = true;
      }
   }
   if (success) return true;
   return GmatBase::RenameRefObject(type, oldName, newName);
}
                               
//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                       const std::string &name)
//------------------------------------------------------------------------------
/**
 * Returns a pointer to a reference object with the input name. 
 *
 * @param <type>    reference object type.   
 * @param <name>    the name for the object.   
  *
 * @return Pointer to the object specified.
 */
//------------------------------------------------------------------------------
GmatBase* Attitude::GetRefObject(const Gmat::ObjectType type,
                                 const std::string &name)
{
   switch (type)
   {
      case Gmat::COORDINATE_SYSTEM:
         if ((refCS) && (name == refCSName))           return refCS;
         break;
      default:
         break;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return GmatBase::GetRefObject(type, name);
}
                                    
//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                       const std::string &name)
//------------------------------------------------------------------------------
/**
 * Returns a pointer to a reference object with the input name. 
 *
 * @param obj     the object
 * @param type    reference object type.   
 * @param name    the name for the object.   
  *
 * @return Success flag.
 */
//------------------------------------------------------------------------------
bool Attitude::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                            const std::string &name)
{
   if (obj->IsOfType("CoordinateSystem"))
   {
      if (name == refCSName)
      {
            #ifdef DEBUG_REF_SETTING
            if (refCS != NULL)
               MessageInterface::ShowMessage(
                  "The reference coordinate system for attitude is %s\n",
                  (refCS->GetName()).c_str());
            else
               MessageInterface::ShowMessage("   refCS == NULL!\n");
            #endif
         if (refCS != (CoordinateSystem*) obj)
         {
            #ifdef DEBUG_REF_SETTING
               MessageInterface::ShowMessage(
                  "Setting %s as reference coordinate system for attitude %s\n",
                  name.c_str(), instanceName.c_str());
            #endif
            isInitialized = false;
            refCS = (CoordinateSystem*) obj;
         }
      }
      return true;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return GmatBase::SetRefObject(obj, type, name);
}

                                    
// methods to get/set parameter values
//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Attitude::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < AttitudeParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else if (id >= OTHER_REPS_OFFSET && id < EndOtherReps)
      return OTHER_REP_TEXT[id - OTHER_REPS_OFFSET];
   return GmatBase::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer Attitude::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < AttitudeParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   // otherwise, check for other reps
   for (Integer i = OTHER_REPS_OFFSET; i < EndOtherReps; i++)
   {
      if (str == OTHER_REP_TEXT[i - OTHER_REPS_OFFSET])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType
            Attitude::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < AttitudeParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else if (id >= OTHER_REPS_OFFSET && id < EndOtherReps)
      return OTHER_REP_TYPE[id - OTHER_REPS_OFFSET];
      
   return GmatBase::GetParameterType(id);
}
            
//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Attitude::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool Attitude::IsParameterReadOnly(const Integer id) const
{
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   "Entering Attitude::ReadOnly with id = %d (%s)\n", id,
   GetParameterText(id).c_str());
   #endif
   if (id == EULER_SEQUENCE_LIST)
      return true;
   if (id == EPOCH)
      return true;
   //if (inputAttType == GmatAttitude::QUATERNION_TYPE)
   if (attitudeDisplayType == "Quaternion")
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly in quaternion section\n");
   #endif
      if ((id == EULER_ANGLE_1) || (id == EULER_ANGLE_2) ||
          (id == EULER_ANGLE_3) || (id == DCM_11)        || (id == DCM_12) ||
          (id == DCM_13)        || (id == DCM_21)        || (id == DCM_22) ||
          (id == DCM_23)        || (id == DCM_31)        || (id == DCM_32) ||
          (id == DCM_33))
         return true;
   }
   //else if (inputAttType == GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE)
   else if (attitudeDisplayType == "DirectionCosineMatrix")
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly in cosMat section\n");
   #endif
      if ((id == Q_1) || (id == Q_2) || (id == Q_3) || (id == Q_4)    ||
          (id == EULER_ANGLE_1) || (id == EULER_ANGLE_2) ||
          (id == EULER_ANGLE_3))
         return true;
   }
   else  // "EulerAngles
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly in euler angles section\n");
   #endif
      if ((id == Q_1) || (id == Q_2) || (id == Q_3) || (id == Q_4)    ||
          (id == DCM_11)        || (id == DCM_12)   || (id == DCM_13) || 
          (id == DCM_21)        || (id == DCM_22)   || (id == DCM_23) || 
          (id == DCM_31)        || (id == DCM_32)   || (id == DCM_33))
         return true;
   }
   
   //if (inputAttRateType == GmatAttitude::ANGULAR_VELOCITY_TYPE)
   if (attitudeRateDisplayType == "AngularVelocity")
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly in angular velocity section\n");
   #endif
      if ((id == EULER_ANGLE_RATE_1) || (id == EULER_ANGLE_RATE_2) ||
          (id == EULER_ANGLE_RATE_3))
         return true;
   }
   else // EulerAngleRates
   {
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly in euler angle rates section\n");
   #endif
      if ((id == ANGULAR_VELOCITY_X) || (id == ANGULAR_VELOCITY_Y) ||
          (id == ANGULAR_VELOCITY_Z))
         return true;
   }
   #ifdef DEBUG_ATTITUDE_READ_ONLY
   MessageInterface::ShowMessage(
   " ....... Attitude::ReadOnly calling GmatBase::IsParameterReadOnly\n");
   #endif

   return GmatBase::IsParameterReadOnly(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool Attitude::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

   
//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 * 
 * @note I am using const_cast here to be able to make changes in the 
 *       object data.  (Just like in the Spacecraft).  Ew.
 *
 */
//------------------------------------------------------------------------------
Real Attitude::GetRealParameter(const Integer id) const
{
   #ifdef DEBUG_ATTITUDE_GET_REAL
   MessageInterface::ShowMessage(
   "Entering Attitude::GetReal with id = %d (%s)\n", id,
   (GetParameterText(id)).c_str());
   #endif
   if (id == EPOCH)      return epoch;
   if (id == Q_1)
   {
      (const_cast<Attitude*>(this))->UpdateState("Quaternion");
      return quaternion(0);
   }
   if (id == Q_2)
   {   
      (const_cast<Attitude*>(this))->UpdateState("Quaternion");
      return quaternion(1);
   }   
   if (id == Q_3)
   {
      (const_cast<Attitude*>(this))->UpdateState("Quaternion");
      return quaternion(2);
   }
   if (id == Q_4)
   {
      (const_cast<Attitude*>(this))->UpdateState("Quaternion");
      return quaternion(3);
   }
   if (id == EULER_ANGLE_1)  
   {              
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      return eulerAngles(0) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_2)
   {              
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      return eulerAngles(1) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_3)
   {              
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      return eulerAngles(2) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == DCM_11)             return cosMat(0,0);
   if (id == DCM_12)             return cosMat(0,1);
   if (id == DCM_13)             return cosMat(0,2);
   if (id == DCM_21)             return cosMat(1,0);
   if (id == DCM_22)             return cosMat(1,1);
   if (id == DCM_23)             return cosMat(1,2);
   if (id == DCM_31)             return cosMat(2,0);
   if (id == DCM_32)             return cosMat(2,1);
   if (id == DCM_33)             return cosMat(2,2);
   if (id == EULER_ANGLE_RATE_1)
   {
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      return eulerAngleRates(0) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_RATE_2)
   {
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      return eulerAngleRates(1) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_RATE_3)
   {
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      return eulerAngleRates(2) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == ANGULAR_VELOCITY_X) return angVel[0] * GmatMathUtil::DEG_PER_RAD;
   if (id == ANGULAR_VELOCITY_Y) return angVel[1] * GmatMathUtil::DEG_PER_RAD;
   if (id == ANGULAR_VELOCITY_Z) return angVel[2] * GmatMathUtil::DEG_PER_RAD;

   #ifdef DEBUG_ATTITUDE_GET_REAL
   MessageInterface::ShowMessage(
   "Exiting Attitude::GetReal - calling parent ...\n");
   #endif
   
   return GmatBase::GetRealParameter(id);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real Attitude::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return  Real value of the requested parameter.
 * 
 * @note Assumes that these are called before initialization and the execution
 *       of the mission sequence.
 *
 */
//------------------------------------------------------------------------------
Real Attitude::SetRealParameter(const Integer id,
                                const Real value)
{
   #ifdef DEBUG_ATTITUDE_SET_REAL
   MessageInterface::ShowMessage(
   "ENTERING Att::SetReal with id = %d (%s) and value = %.12f\n", id,
   GetParameterText(id).c_str(), value);
   #endif
   if (id == EPOCH)  // this should be an A1Mjd time
   {
      if (epoch != value)
      {
         isInitialized = false;  // not really true for CSFixed, though
         epoch         = value;
      }
      return true;
   }
   if (id == Q_1)
   {
      // check for valid value here ..... how?  without all 4?
      quaternion    = Attitude::ToQuaternion(cosMat);
      quaternion(0) = value;
      cosMat = Attitude::ToCosineMatrix(quaternion);
      inputAttType  = GmatAttitude::QUATERNION_TYPE;
      return quaternion(0);
   }
   if (id == Q_2)
   {
      // check for valid value here ..... how?  without all 4?
      quaternion    = Attitude::ToQuaternion(cosMat);
      quaternion(1) = value;
      cosMat = Attitude::ToCosineMatrix(quaternion);
      inputAttType  = GmatAttitude::QUATERNION_TYPE;
      return quaternion(1);
   }
   if (id == Q_3)
   {
      // check for valid value here ..... how?  without all 4?
      quaternion    = Attitude::ToQuaternion(cosMat);
      quaternion(2) = value;
      cosMat = Attitude::ToCosineMatrix(quaternion);
      inputAttType  = GmatAttitude::QUATERNION_TYPE;
      return quaternion(2);
   }
   if (id == Q_4)
   {
      // check for valid value here ..... how?  without all 4?
      quaternion    = Attitude::ToQuaternion(cosMat);
      quaternion(3) = value;
      cosMat = Attitude::ToCosineMatrix(quaternion);
      inputAttType  = GmatAttitude::QUATERNION_TYPE;
      return quaternion(3);
   }
   if (id == EULER_ANGLE_1)
   {
      // check for valid value here ..... how?  without all 4?
      eulerAngles    = Attitude::ToEulerAngles(cosMat,
                       eulerSequenceArray.at(0),
                       eulerSequenceArray.at(1),
                       eulerSequenceArray.at(2));
      eulerAngles(0) = value * GmatMathUtil::RAD_PER_DEG;
      cosMat = Attitude::ToCosineMatrix(eulerAngles,
                       eulerSequenceArray.at(0),
                       eulerSequenceArray.at(1),
                       eulerSequenceArray.at(2));
      inputAttType = GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE;
      return eulerAngles(0) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_2)
   {
      // check for valid value here ..... how?  without all 4?
      eulerAngles    = Attitude::ToEulerAngles(cosMat,
                       eulerSequenceArray.at(0),
                       eulerSequenceArray.at(1),
                       eulerSequenceArray.at(2));
      eulerAngles(1) = value * GmatMathUtil::RAD_PER_DEG;
      cosMat = Attitude::ToCosineMatrix(eulerAngles,
                       eulerSequenceArray.at(0),
                       eulerSequenceArray.at(1),
                       eulerSequenceArray.at(2));
      inputAttType = GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE;
      return eulerAngles(1) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_3)
   {
      // check for valid value here ..... how?  without all 4?
      eulerAngles    = Attitude::ToEulerAngles(cosMat,
                       eulerSequenceArray.at(0),
                       eulerSequenceArray.at(1),
                       eulerSequenceArray.at(2));
      eulerAngles(2) = value * GmatMathUtil::RAD_PER_DEG;
      cosMat = Attitude::ToCosineMatrix(eulerAngles,
                       eulerSequenceArray.at(0),
                       eulerSequenceArray.at(1),
                       eulerSequenceArray.at(2));
      inputAttType = GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE;
      return eulerAngles(2) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == DCM_11)
   {
      cosMat(0,0) = value;
      inputAttType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      return cosMat(0,0);
   }
   if (id == DCM_12)
   {
      cosMat(0,1) = value;
      inputAttType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      return cosMat(0,1);
   }
   if (id == DCM_13)
   {
      cosMat(0,2) = value;
      inputAttType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      return cosMat(0,2);
   }
   if (id == DCM_21)
   {
      cosMat(1,0) = value;
      inputAttType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      return cosMat(1,0);
   }
   if (id == DCM_22)
   {
      cosMat(1,1) = value;
      inputAttType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      return cosMat(1,1);
   }
   if (id == DCM_23)
   {
      cosMat(1,2) = value;
      inputAttType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      return cosMat(1,2);
   }
   if (id == DCM_31)
   {
      cosMat(2,0) = value;
      inputAttType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      return cosMat(2,0);
   }
   if (id == DCM_32)
   {
      cosMat(2,1) = value;
      inputAttType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      return cosMat(2,1);
   }
   if (id == DCM_33)
   {
      cosMat(2,2) = value;
      inputAttType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      return cosMat(2,2);
   }
   if (id == EULER_ANGLE_RATE_1)
   {
      eulerAngleRates = Attitude::ToEulerAngleRates(angVel,
                        eulerAngles,
                        eulerSequenceArray.at(0),
                        eulerSequenceArray.at(1),
                        eulerSequenceArray.at(2));
      eulerAngleRates(0) = value * GmatMathUtil::RAD_PER_DEG;
      angVel          = Attitude::ToAngularVelocity(eulerAngleRates,
                        eulerAngles,
                        eulerSequenceArray.at(0),
                        eulerSequenceArray.at(1),
                        eulerSequenceArray.at(2));
      inputAttRateType = GmatAttitude::EULER_ANGLE_RATES_TYPE;
      return eulerAngleRates(0) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_RATE_2)
   {
      eulerAngleRates = Attitude::ToEulerAngleRates(angVel,
                        eulerAngles,
                        eulerSequenceArray.at(0),
                        eulerSequenceArray.at(1),
                        eulerSequenceArray.at(2));
      eulerAngleRates(1) = value * GmatMathUtil::RAD_PER_DEG;
      angVel          = Attitude::ToAngularVelocity(eulerAngleRates,
                        eulerAngles,
                        eulerSequenceArray.at(0),
                        eulerSequenceArray.at(1),
                        eulerSequenceArray.at(2));
      inputAttRateType = GmatAttitude::EULER_ANGLE_RATES_TYPE;
      return eulerAngleRates(1) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == EULER_ANGLE_RATE_3)
   {
      eulerAngleRates = Attitude::ToEulerAngleRates(angVel,
                        eulerAngles,
                        eulerSequenceArray.at(0),
                        eulerSequenceArray.at(1),
                        eulerSequenceArray.at(2));
      eulerAngleRates(2) = value * GmatMathUtil::RAD_PER_DEG;
      angVel          = Attitude::ToAngularVelocity(eulerAngleRates,
                        eulerAngles,
                        eulerSequenceArray.at(0),
                        eulerSequenceArray.at(1),
                        eulerSequenceArray.at(2));
      inputAttRateType = GmatAttitude::EULER_ANGLE_RATES_TYPE;
      return eulerAngleRates(2) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == ANGULAR_VELOCITY_X)
   {
      angVel(0) = value * GmatMathUtil::RAD_PER_DEG;
      inputAttRateType = GmatAttitude::ANGULAR_VELOCITY_TYPE;
      return angVel(0) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == ANGULAR_VELOCITY_Y)
   {
      angVel(1) = value * GmatMathUtil::RAD_PER_DEG;
      inputAttRateType = GmatAttitude::ANGULAR_VELOCITY_TYPE;
      return angVel(1) * GmatMathUtil::DEG_PER_RAD;
   }
   if (id == ANGULAR_VELOCITY_Z)
   {      // check for valid value here .....   
      angVel(2) = value * GmatMathUtil::RAD_PER_DEG;
      inputAttRateType = GmatAttitude::ANGULAR_VELOCITY_TYPE;
      return angVel(2) * GmatMathUtil::DEG_PER_RAD;
   }
   
   return GmatBase::SetRealParameter(id, value);
}

                                
//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 */
//------------------------------------------------------------------------------
Real Attitude::SetRealParameter(const std::string &label,
                                const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
//  const UnsignedIntArray&  GetUnsignedIntArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the UnsignedIntArray parameter value, 
 * given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return  UnsignedIntArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const UnsignedIntArray& 
            Attitude::GetUnsignedIntArrayParameter(const Integer id) const
{
   if (id == EULER_SEQUENCE) return eulerSequenceArray;
   return GmatBase::GetUnsignedIntArrayParameter(id);
}

//------------------------------------------------------------------------------
//  const UnsignedIntArray&
//        GetUnsignedIntArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the UnsignedIntArray parameter value, 
 * given the input parameter label.
 *
 * @param <label> label for the requested parameter value.
 *
 * @return  UnsignedIntArray value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
const UnsignedIntArray& 
            Attitude::GetUnsignedIntArrayParameter(
            const std::string &label) const
{
   return GetUnsignedIntArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  UnsignedInt
//        SetUnsignedIntParameter(const Integer id,
//                                const UnsignedInt value,
 //                               const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the UnsignedInt parameter value, in tehthe array of values, 
 * given the input parameter id and index.
 *
 * @param <id>    id for the requested parameter value.
 * @param <index> index into the array for the requested parameter value.
 *
 * @return  UnsignedIntArray value of the set parameter.
 *
 */
//------------------------------------------------------------------------------
//UnsignedInt Attitude::SetUnsignedIntParameter(const Integer id,
//                                              const UnsignedInt value,
//                                              const Integer index)
//{
//   if (id == EULER_SEQUENCE)
//   {
//      if (index < 0 || index > 2)
//         throw AttitudeException(
//         "Out-of-bounds index for setting euler sequence.");
//      eulerSequenceArray.at(index) = value;
//      SetEulerSequenceString(eulerSequenceArray);
//      inputAttType  = GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE;
//   }
//   return GmatBase::SetUnsignedIntParameter(id,value,index);
//}

//------------------------------------------------------------------------------
//  const Rvector&  GetRvectorParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * This method gets the Rvector parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  the Rvector parameter (Angles are returned in degrees).
 *
 */
//------------------------------------------------------------------------------
const Rvector&    Attitude::GetRvectorParameter(const Integer id) const
{
   static Rvector3 vec3;
   static Rvector  vec4(4);
   if (id == QUATERNION)
   {          
      (const_cast<Attitude*>(this))->UpdateState("Quaternion");
      vec4 = quaternion;
      return vec4;
   }
   if (id == EULER_ANGLES)   
   {     
      (const_cast<Attitude*>(this))->UpdateState("EulerAngles");
      vec3 =  (eulerAngles * GmatMathUtil::DEG_PER_RAD);
      return vec3;
   }
   if (id == ANGULAR_VELOCITY) 
   {  
      (const_cast<Attitude*>(this))->UpdateState("AngularVelocity");
      vec3 = angVel * GmatMathUtil::DEG_PER_RAD;
      return vec3;
   }
   if (id == EULER_ANGLE_RATES) 
   {  
      (const_cast<Attitude*>(this))->UpdateState("EulerAngleRates");
      vec3 = eulerAngleRates * GmatMathUtil::DEG_PER_RAD;
      return vec3;
      //return (eulerAngleRates * GmatMathUtil::DEG_PER_RAD);
   }

   return GmatBase::GetRvectorParameter(id);
}

//------------------------------------------------------------------------------
//  const Rvector&  GetRvectorParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * This method gets the Rvector parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  the Rvector parameter.
 *
 */
//------------------------------------------------------------------------------
const Rvector& Attitude::GetRvectorParameter(const std::string &label) const
{
   return GetRvectorParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  const Rvector&  SetRvectorParameter(const Integer id, const Rvector& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rvector parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Rvector value for the requested parameter (angles are
 *                expected in degrees).
 *
 * @return  Rvector parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rvector& Attitude::SetRvectorParameter(const Integer id,
                                             const Rvector &value)
{
   Integer sz = value.GetSize();
   Integer i;
   
   if (id == EULER_ANGLES)
   {
      if (sz != 3) throw AttitudeException(
                  "Incorrectly sized Rvector passed in for euler angles.");
      for (i=0;i<3;i++) 
         eulerAngles(i) = value(i) * GmatMathUtil::RAD_PER_DEG;
      cosMat = Attitude::ToCosineMatrix(eulerAngles, 
                         eulerSequenceArray.at(0),
                         eulerSequenceArray.at(1),
                         eulerSequenceArray.at(2));                         
      inputAttType = GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE;
      return eulerAngles;
   }
   if (id == QUATERNION)
   {
      if (sz != 4) throw AttitudeException(
                  "Incorrectly sized Rvector passed in for quaternion.");
      for (i=0;i<4;i++) 
         quaternion(i) = value(i);
      quaternion.Normalize();
      cosMat = Attitude::ToCosineMatrix(quaternion);
      inputAttType = GmatAttitude::QUATERNION_TYPE;
      return quaternion;
   }
   if (id == EULER_ANGLE_RATES)
   {
      if (sz != 3) throw AttitudeException(
                  "Incorrectly sized Rvector passed in for euler angle rates.");
      for (i=0;i<3;i++) 
         eulerAngleRates(i) = value(i) * GmatMathUtil::RAD_PER_DEG;
      angVel = Attitude::ToAngularVelocity(eulerAngleRates, eulerAngles, 
                         eulerSequenceArray.at(0),
                         eulerSequenceArray.at(1),
                         eulerSequenceArray.at(2));                         
      inputAttRateType = GmatAttitude::EULER_ANGLE_RATES_TYPE;
      return eulerAngleRates;
   }
   if (id == ANGULAR_VELOCITY)
   {
      if (sz != 3) throw AttitudeException(
                  "Incorrectly sized Rvector passed in for angular velocity.");
      for (i=0;i<3;i++) 
         angVel(i) = value(i) * GmatMathUtil::RAD_PER_DEG;
      inputAttRateType = GmatAttitude::ANGULAR_VELOCITY_TYPE;
      return angVel;
   }

   return GmatBase::SetRvectorParameter(id,value);
}

                                                 
//------------------------------------------------------------------------------
//  const Rvector&  SetRvectorParameter(const std::string &label, 
//                                      const Rvector& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rvector parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 * @param <value> Rvector value for the requested parameter.
 *
 * @return  Rvector parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rvector& Attitude::SetRvectorParameter(const std::string &label,
                                             const Rvector &value)
{
   return SetRvectorParameter(GetParameterID(label), value);
}
                                    
//------------------------------------------------------------------------------
//  const Rmatrix&  GetRmatrixParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * This method gets the Rmatrix parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return  the Rmatrix parameter.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& Attitude::GetRmatrixParameter(const Integer id) const
{
   if (id == DIRECTION_COSINE_MATRIX)    return cosMat;
   return GmatBase::GetRmatrixParameter(id);
}

//------------------------------------------------------------------------------
//  const Rmatrix&  GetRmatrixParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * This method gets the Rmatrix parameter value, given the input
 * parameter ID.
 *
 * @param <label> string ID for the requested parameter.
 *
 * @return  the Rmatrix parameter.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& Attitude::GetRmatrixParameter(const std::string &label) const
{
   return GetRmatrixParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  const Rmatrix&  SetRmatrixParameter(const Integer id, const Rmatrix& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rmatrix parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> Rmatrix value for the requested parameter.
 *
 * @return  the Rmatrix parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix& Attitude::SetRmatrixParameter(const Integer id,
                                             const Rmatrix &value)
{
   if (id == DIRECTION_COSINE_MATRIX) 
   {
      Integer r,c;
      value.GetSize(r,c);
      if ((r != 3) || (c != 3))
         throw AttitudeException(
         "Incorrectly sized Rmatrix passed in for direction cosine matrix.");
      Rmatrix33 inValue;
      for (Integer i = 0; i < 3; i++) // compiler didn't like op=here ???
         for (Integer j = 0; j < 3; j++)
            inValue(i,j) = value(i,j);
      ValidateCosineMatrix(inValue);
      cosMat = inValue;
      inputAttType = GmatAttitude::DIRECTION_COSINE_MATRIX_TYPE;
      return cosMat;
   }

   return GmatBase::SetRmatrixParameter(id,value);
}
                                                 
                                                 
//------------------------------------------------------------------------------
//  const Rmatrix&  SetRmatrixParameter(const std::string &label,
//                                      const Rmatrix& value)
//------------------------------------------------------------------------------
/**
 * This method sets the Rmatrix parameter value, given the input
 * parameter label.
 *
 * @param <label> string ID for the requested parameter.
 * @param <value> Rmatrix value for the requested parameter.
 *
 * @return  the Rmatrix parameter value.
 *
 */
//------------------------------------------------------------------------------
const Rmatrix&    Attitude::SetRmatrixParameter(const std::string &label,
                                                 const Rmatrix &value)
{
   return SetRmatrixParameter(GetParameterID(label), value);
}

                                                 
//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter given the input parameter ID. 
 *
 * @param id ID for the requested parameter.
 * 
 * @return The requested string.
 */
//------------------------------------------------------------------------------
std::string Attitude::GetStringParameter(const Integer id) const
{
   if (id == ATTITUDE_DISPLAY_STATE_TYPE)      return attitudeDisplayType;
   if (id == ATTITUDE_RATE_DISPLAY_STATE_TYPE)      
                                               return attitudeRateDisplayType;
   if (id == REFERENCE_COORDINATE_SYSTEM)      return refCSName;
   //if (id == EULER_SEQUENCE_STRING)            return eulerSeqString;
   if (id == EULER_ANGLE_SEQUENCE)             return eulerSequence;
   return GmatBase::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter given the input parameter label. 
 *
 * @param label label for the requested parameter.
 * 
 * @return The requested string.
 */
//------------------------------------------------------------------------------
std::string Attitude::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets a value on a string parameter given the input parameter ID, 
 * and the value. 
 *
 * @param id ID for the requested parameter.
 * @param value The new string.
  * 
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool Attitude::SetStringParameter(const Integer id, 
                                  const std::string &value)
{
   if (id == ATTITUDE_DISPLAY_STATE_TYPE)
   {
      if ((value != "Quaternion") && (value != "DirectionCosineMatrix") &&
          (value != "EulerAngles"))
      {
         AttitudeException ae;
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
            value.c_str(), GetParameterText(id).c_str(),
            "\"Quaternion\" \"DirectionCosineMatrix\" \"EulerAngles\"");
         throw ae;
      }
      attitudeDisplayType = value;
      return true;
   }
   if (id == ATTITUDE_RATE_DISPLAY_STATE_TYPE)
   {
      if ((value != "AngularVelocity") && (value != "EulerAngleRates"))
      {
         AttitudeException ae;
         ae.SetDetails(errorMessageFormatUnnamed.c_str(),
            value.c_str(), GetParameterText(id).c_str(),
            "\"AngularVelocity\"  \"EulerAngleRates\"");
         throw ae;
      }
      attitudeRateDisplayType = value;
      return true;
   }
   if (id == REFERENCE_COORDINATE_SYSTEM)
   {
       refCSName = value;
       return true;
   }
   //if (id == EULER_SEQUENCE_STRING)
   if (id == EULER_ANGLE_SEQUENCE)
   {
      ValidateEulerSequence(value);
      UnsignedIntArray newSeq = Attitude::ExtractEulerSequence(value);
      Rvector3 currentEulerAngles = Attitude::ToEulerAngles(cosMat,
                                    eulerSequenceArray.at(0),
                                    eulerSequenceArray.at(1),
                                    eulerSequenceArray.at(2));
      Rvector3 currentEulerAngleRates = 
                                    Attitude::ToEulerAngleRates(angVel,
                                    currentEulerAngles,
                                    eulerSequenceArray.at(0),
                                    eulerSequenceArray.at(1),
                                    eulerSequenceArray.at(2));
      
      // if input type is EulerAngles, then assume we don't want to change
      // them, and get the current ones (which are up-to-date with the cosine
      // matrix); otherwise, if the input type is Quaternion or Cosine
      // Matrix, get the 'new' EulerAngles from the Cosine Matrix.
      if (inputAttType == GmatAttitude::EULER_ANGLES_AND_SEQUENCE_TYPE)
      {
         eulerAngles    = currentEulerAngles;
         cosMat         = Attitude::ToCosineMatrix(eulerAngles,
                          newSeq.at(0),
                          newSeq.at(1),
                          newSeq.at(2));
      }
      else // input type is CosineMatrix or Quaternion
      {
         eulerAngles    = Attitude::ToEulerAngles(cosMat,
                          newSeq.at(0),
                          newSeq.at(1),
                          newSeq.at(2));
      }
      // if input type for rate is EulerAngleRates, then recompute angVel;
      // otherwise, assume user does not want to change angVel and leave them
      // as they are (up-to-date)
      if (inputAttRateType == GmatAttitude::EULER_ANGLE_RATES_TYPE)
      {
         // get 'old' EulerAngleRates
         eulerAngleRates = currentEulerAngleRates;
         // compute the 'new' angVel
         angVel          = Attitude::ToAngularVelocity(eulerAngleRates,
                           eulerAngles,
                           newSeq.at(0),
                           newSeq.at(1),
                           newSeq.at(2));
      }
      // otherwise, don't really care if the EulerAngleRates are up-to-date;
      // the angular velocity is correct already in this case
      eulerSequence = value;
      eulerSequenceArray.clear();
      eulerSequenceArray = newSeq;
      return true;
   }
   return GmatBase::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets a value on a string parameter given the input parameter 
 * label, and the value. 
 *
 * @param label label for the requested parameter.
 * @param value The new string.
  * 
 * @return true if successful; otherwise, false.
 */
//------------------------------------------------------------------------------
bool Attitude::SetStringParameter(const std::string &label, 
                                  const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string array given the input parameter ID. 
 *
 * @param id ID for the requested parameter.
 * 
 * @return The requested string array.
 */
//------------------------------------------------------------------------------
const StringArray& 
            Attitude::GetStringArrayParameter(const Integer id) const
{
   if (id == EULER_SEQUENCE_LIST)  return eulerSequenceList;
   return GmatBase::GetStringArrayParameter(id);
}

//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the string array given the input parameter label. 
 *
 * @param label label for the requested parameter.
 * 
 * @return The requested string array.
 */
//------------------------------------------------------------------------------
const StringArray& 
            Attitude::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// StringArray GetGeneratingString(Gmat::WriteMode mode,
//                const std::string &prefix, const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Produces a string, possibly multi-line, containing the text that produces an
 * object.
 *
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 *
 * @return A string containing the text.
 *
 */
//------------------------------------------------------------------------------
const std::string& Attitude::GetGeneratingString(Gmat::WriteMode mode,
                                                 const std::string &prefix,
                                                 const std::string &useName)
{
   #ifdef DEBUG_ATTITUDE_GEN_STRING
   MessageInterface::ShowMessage
      ("Attitude::GetGeneratingString() this=%p, mode=%d, prefix=%s, "
       "useName=%s\n", this, mode, prefix.c_str(), useName.c_str());
   #endif
   
   std::stringstream data;

   data.precision(18);   // Crank up data precision so we don't lose anything
   std::string preface = "", nomme;

   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::OWNED_OBJECT) ||
       (mode == Gmat::SHOW_SCRIPT))
      inMatlabMode = false;
   if (mode == Gmat::MATLAB_STRUCT || mode == Gmat::EPHEM_HEADER)
      inMatlabMode = true;

   if (useName != "")
      nomme = useName;
   else
      nomme = instanceName;
/* DON'T NEED THIS, I THINK
   if ((mode == Gmat::SCRIPTING) || (mode == Gmat::SHOW_SCRIPT) ||
       (mode == Gmat::EPHEM_HEADER))
   {
      std::string tname = typeName;
      if (mode == Gmat::EPHEM_HEADER)
      {
         data << tname << " = " << "'" << nomme << "';\n";
         preface = "";
      }
      else
      {
         data << "Create " << tname << " " << nomme << ";\n";
         preface = "GMAT ";
      }
      
      //data << "Create " << tname << " " << nomme << ";\n";
      //preface = "GMAT ";
   }
*/
   nomme += ".";

   if (mode == Gmat::OWNED_OBJECT) {
      preface = prefix;
      nomme = "";
   }

   preface += nomme;
   #ifdef DEBUG_ATTITUDE_GEN_STRING
   MessageInterface::ShowMessage
      ("Attitude::GetGeneratingString() about to call WriteParameters ...\n");
   #endif
   WriteParameters(mode, preface, data);
   #ifdef DEBUG_ATTITUDE_GEN_STRING
   MessageInterface::ShowMessage
      ("Attitude::GetGeneratingString() DONE calling WriteParameters ...\n");
   #endif

   generatingString = data.str();
   return generatingString;
}
 
 
 //------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  Rmatrix33  EulerAxisAndAngleToDCM(const Rvector3 &eAxis, Real eAngle)
//------------------------------------------------------------------------------
/**
 * This method computes the direction cosine matrix given the input
 * euler axis and angle.
 *
 * @param <eAxis>  euler axis.
 * @param <eAngle> euler angle.
 *
 * @return  Cosine matrix representation of the attitude.
 *
 */
//------------------------------------------------------------------------------
Rmatrix33 Attitude::EulerAxisAndAngleToDCM(
                    const Rvector3 &eAxis, Real eAngle)
{
   Rmatrix33 a_x(      0.0, -eAxis(2),  eAxis(1),
                  eAxis(2),       0.0, -eAxis(0),
                 -eAxis(1),  eAxis(0),      0.0);
   Rmatrix33 I33(true);
   Real c = GmatMathUtil::Cos(eAngle);
   Real s = GmatMathUtil::Sin(eAngle);
   return Rmatrix33((c*I33) + ((1.0 - c)*Outerproduct(eAxis, eAxis))
                    - s*a_x);
}

//------------------------------------------------------------------------------
//  void  DCMToEulerAxisAndAngle(const Rmatrix33 &cosmat, 
//                               Rvector3 &eAxis, Real &eAngle)
//------------------------------------------------------------------------------
/**
 * This method computes the direction cosine matrix given the input
 * euler axis and angle.
 *
 * @param <cosmat>  cosine matrix.
 * @param <eAxis>   euler axis (output).
 * @param <eAngle>  euler angle (output).
 *
 * @return  Euler Axis/Angle representation of the attitude.
 *
 */
//------------------------------------------------------------------------------
void Attitude::DCMToEulerAxisAndAngle(const Rmatrix33 &cosMat, 
                            Rvector3 &eAxis, Real &eAngle)
{
   static Real TOL = 1.0E-14;
   Real R12  = cosMat(0,1);
   Real R13  = cosMat(0,2);
   Real R21  = cosMat(1,0);
   Real R23  = cosMat(1,2);
   Real R31  = cosMat(2,0);
   Real R32  = cosMat(2,1);
   
   eAngle      = GmatMathUtil::ACos(0.5 * (cosMat.Trace() - 1.0));
   Real s      = GmatMathUtil::Sin(eAngle);
   if (GmatMathUtil::Abs(s)  < TOL)
   {
      eAxis.Set(1.0, 0.0, 0.0);
      return;
   }
   Real mult   = 1.0 / (2.0 * s);
   eAxis.Set(mult*(R23-R32), mult*(R31-R13), mult*(R12-R21));
   return;
}
                            
                            
//------------------------------------------------------------------------------
//  bool  SetEulerSequenceString(const UnsignedIntArray &eulerArray)
//------------------------------------------------------------------------------
/**
 * This method sets the state data for the euler sequence string, given
 * the input array of the euler sequnce. 
 *
 * @param <eulerArray>  array of euler sequence.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
//bool Attitude::SetEulerSequenceString(const UnsignedIntArray &eulerArray)
//{
//   std::ostringstream eulS;
//   eulS.str(eulerSequence);  // may need a char[] here ???????????
//   //eulS << eulerArray.at(0) << " " << eulerArray.at(1)
//   //     << " " << eulerArray.at(2);  // wcs 2007.05.29 leave out blanks
//   eulS << eulerArray.at(0) << eulerArray.at(1) << eulerArray.at(2);
 //  return true;
//}

bool Attitude::ValidateCosineMatrix(const Rmatrix33 &mat)
{
   if (!mat.IsOrthogonal()) //return false;
   {
      AttitudeException attex;
      std::ostringstream matS;
      matS.str("");
      matS << "[";
      for (Integer ii = 0; ii < 3; ii++)
         for (Integer jj = 0; jj < 3; jj++)
         {
            matS << mat(ii,jj) ;
            if (!(ii ==3 && jj==3))  matS << " ";
         }
      matS << "]";
      std::string errMsg = "The value of \"" + matS.str();
      errMsg += "\" for field \"" + OTHER_REP_TEXT[DIRECTION_COSINE_MATRIX - OTHER_REPS_OFFSET];
      errMsg += "\" on an object of type \"" + typeName;
      errMsg += "\" is not an allowed value.\n";
      errMsg += "The allowed values are: [orthogonal matrix].";
      // argh! this is not working ..... why not?????
      //std::string orthogonal = "Orthogonal matrix";
      //attex.SetDetails(errorMessageFormatUnnamed.c_str(),
      //           (matS.str()).c_str(), // tried ToString() on mat as well here
      //           OTHER_REP_TEXT[DIRECTION_COSINE_MATRIX - OTHER_REPS_OFFSET].c_str(), 
      //           orthogonal.c_str());
      attex.SetDetails(errMsg);
      throw attex;
   }
   return true;
}

bool Attitude::ValidateEulerSequence(const std::string &seq)
{
   for (Integer i=0; i<12; i++)
      if (seq == EULER_SEQ_LIST[i])  return true;
   AttitudeException ae;
   std::ostringstream eulSeqs;
   eulSeqs << "One of ";
   for (Integer i=0; i<12; i++)
      eulSeqs << " " << EULER_SEQ_LIST[i];
   ae.SetDetails(errorMessageFormatUnnamed.c_str(),
                 seq.c_str(),
                 "EulerAngleSequence", 
                 (eulSeqs.str()).c_str());
   throw ae;
   //return false;
}

bool Attitude::ValidateEulerSequence(const UnsignedIntArray &eulAng)
{
   if (eulAng.size()  != 3)
      throw AttitudeException(
         "Euler Sequence contains too few/many components - cannot convert input\n");
   std::string        eulStr;
   std::ostringstream eulS;
   eulS.str(eulStr); 
   eulS << eulAng[0] << eulAng[1] << eulAng[2];
   return ValidateEulerSequence(eulStr);
}

void Attitude::UpdateState(const std::string &rep)
{
   if (rep == "Quaternion")
   {
      quaternion       = Attitude::ToQuaternion(cosMat);
   }
   else if (rep == "EulerAngles")
   {
      eulerAngles = Attitude::ToEulerAngles(cosMat, 
                              (Integer) eulerSequenceArray.at(0),
                              (Integer) eulerSequenceArray.at(1), 
                              (Integer) eulerSequenceArray.at(2));
   }
   else if (rep == "EulerAngleRates")
   {
      eulerAngles = Attitude::ToEulerAngles(cosMat, 
                              (Integer) eulerSequenceArray.at(0),
                              (Integer) eulerSequenceArray.at(1), 
                              (Integer) eulerSequenceArray.at(2));
      eulerAngleRates   = Attitude::ToEulerAngleRates(angVel, 
                          eulerAngles,
                          (Integer) eulerSequenceArray.at(0),  
                          (Integer) eulerSequenceArray.at(1), 
                          (Integer) eulerSequenceArray.at(2));
   }
   else
      ; // do nothing - cosMat and angVel are kept up-to-date
}


