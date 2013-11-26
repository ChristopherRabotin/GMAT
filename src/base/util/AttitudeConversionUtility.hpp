//------------------------------------------------------------------------------
//                         AttitudeConversionUtility
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan, GSFC/GSSB
// Created: 2013.11.25
//
/**
 * Definition of the static class containing methods to convert between
 * attitude state representations.
 * This is a static class: No instances of this class may be declared.
 */
//------------------------------------------------------------------------------

#ifndef AttitudeConversionUtil_hpp
#define AttitudeConversionUtil_hpp

#include "gmatdefs.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "Rmatrix33.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"


class GMAT_API AttitudeConversionUtility
{
public:

   static Rmatrix33 ToCosineMatrix(const Rvector &quat1);
   static Rmatrix33 ToCosineMatrix(const Rvector3 &eulerAngles,
                                   Integer seq1, Integer seq2,
                                   Integer seq3);
   static Rmatrix33 ToCosineMatrix(const Real *eulerAngles,
                                   Integer seq1, Integer seq2,
                                   Integer seq3);
   static Rvector3  ToEulerAngles(const Rvector &quat1, Integer seq1,
                                  Integer seq2,         Integer seq3);
   static Rvector3  ToEulerAngles(const Rmatrix33 &cosMat, Integer seq1,
                                  Integer seq2,            Integer seq3);
   static Rvector   ToQuaternion(const Rvector3 &eulerAngles,
                                 Integer seq1, Integer seq2, Integer seq3);
   static Rvector   ToQuaternion(const Rmatrix33 &cosMat);
   static Rvector   ToQuaternion(const Rvector3 &MRPs);
   static Rvector3  ToMRPs(const Rvector &quat1);

   static Rvector3  ToEulerAngleRates(const Rvector3 &angularVel,
                                      const Rvector3 &eulerAngles,
                                      Integer seq1, Integer seq2,
                                      Integer seq3);
   static Rvector3  ToAngularVelocity(const Rvector3 &eulerRates,
                                      const Rvector3 &eulerAngles,
                                      Integer seq1, Integer seq2,
                                      Integer seq3);

   // method to convert an euler axis and angle to a cosine matrix
   static Rmatrix33 EulerAxisAndAngleToDCM(
                        const Rvector3 &eAxis, Real eAngle);
   // method to convert a cosine matrix to an euler axis and angle
   static void      DCMToEulerAxisAndAngle(const Rmatrix33 &cosMat,
                                            Rvector3 &eAxis, Real &eAngle);

private:
   //------------------------------------------------------------------------------
   // private static data
   //------------------------------------------------------------------------------
   // none

   //------------------------------------------------------------------------------
   // private constructors, destructor, operator=
   //------------------------------------------------------------------------------
   AttitudeConversionUtility();
   AttitudeConversionUtility(const AttitudeConversionUtility &copy);
   ~AttitudeConversionUtility();
   AttitudeConversionUtility& operator=(const AttitudeConversionUtility &copy);

};
#endif // AttitudeConversionUtil_hpp
