//$Header$
//------------------------------------------------------------------------------
//                             CoordUtil
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/04/19
// Modified:
//   2004/04/19 Linda Jun - Modified Swnigby code to reuse for GMAT project
//
/**
 * Declares conversion functions between Cartesian and Keplerian elements.
 */
//------------------------------------------------------------------------------
#ifndef CoordUtil_hpp
#define CoordUtil_hpp

#include "gmatdefs.hpp"
#include "Rvector6.hpp"
#include "Anomaly.hpp"

class Anomaly; 

class CoordUtil
{
public:
   static const Real ORBIT_TOL;// = 1.0E-10;
   static const Real ORBIT_TOL_SQ;// = 1.0E-20;
   static const Integer MAX_ITERATIONS = 75;

   enum AnomalyType
   {
      TA,
      MA,
      EA,
      HA
   };

   // static functions
   static bool IsRvValid(Real *r, Real *v);
   static Real MeanToTrueAnomaly(Real ma, Real ecc, Real tol = 1.0e-08);
   static Integer ComputeMeanToTrueAnomaly(Real ma, Real ecc, Real tol,
                                           Real *ta, Integer *iter);
   static Integer ComputeCartToKepl(Real grav, Real r[3], Real v[3], Real *tfp,
                                    Real elem[6], Real *ma);
   static Integer ComputeKeplToCart(Real grav, Real elem[6], Real r[3],
                                    Real v[3], AnomalyType anomalyType);

   static Rvector6 CartesianToKeplerian(const Rvector6 &cartVec, 
                                        const Real grav,
                                        Anomaly &anomaly);
   static Rvector6 CartesianToKeplerian(const Rvector6 &cartVec, Real grav,
                                        Real *ma);

   static Rvector6 KeplerianToCartesian(const Rvector6 &keplVec, 
                                        const Real grav,
                                        Anomaly anomaly);
   static Rvector6 KeplerianToCartesian(const Rvector6 &keplVec, Real grav,
                                        AnomalyType anomalyType);
};

#endif // CoordUtil_hpp
