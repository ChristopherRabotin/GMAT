//$Id$
//------------------------------------------------------------------------------
//                                EarthTide
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: John P. Downing/GSFC/595
// Created: 2010.10.28
// (modified for style, etc. by Wendy Shoan/GSFC/583 2011.05.27)
//
/**
 * This class contains constants and methods for computing Earth tides.
 * NOTE: this class is completely static, and cannot be instantiated or
 * copied. 
 */
//------------------------------------------------------------------------------
#ifndef EarthTide_hpp
#define EarthTide_hpp

#include "gmatdefs.hpp"

class EarthTide
{
public:
   static void SetTide(const Real &jday,      const bool &removepermtide,
                       const Real sunpos[3],  const Real moonpos[3],
                       const Real &sunMass,   const Real &moonMass,
                       const Real &earthMass, const Real &earthRadius,
                       const Real &xp,        const Real &yp);

   static const Integer NNTide = 5;
   static Real          CTide[NNTide][NNTide];
   static Real          STide[NNTide][NNTide];

private:  // No creating or copying allowed
   EarthTide();
   EarthTide(const EarthTide& x);
   EarthTide& operator=(const EarthTide& x);
   virtual ~EarthTide();

   static void CartesianToPolar(const Real pos[3], Real out[3]);
   static void PolarToLegendre(const Real polar[3], Real P[EarthTide::NNTide][EarthTide::NNTide]);

   // Love numbers, k(n,m), IERS p.71, Table 6.3
   static const Real    k[4][4];
   static const Real    k_plus[3];

   // Table 6.3a, IERS p.64 (for C21,S21 coefficients)
   static const Integer Table63aDim1 = 48;
   static const Real    Table63a[Table63aDim1][7];

   // Table 6.3b, IERS p.66 (for C20 coefficient)
   static const Integer Table63bDim1 = 21;
   static const Real    Table63b[Table63bDim1][7];

   // Table 6.3c, IERS p.66 (for C22 coefficient)
//   static const Integer Table63cDim1 = 21;    // 21?/ He's only setting 2!!!
   static const Integer Table63cDim1 = 2;
   static const Real    Table63c[Table63cDim1][6];
};

#endif // EarthTide_hpp
