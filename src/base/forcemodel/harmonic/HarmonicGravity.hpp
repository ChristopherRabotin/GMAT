//$Id$
//------------------------------------------------------------------------------
//                           HarmonicGravity
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: John P. Downing/GSFC/595
// Created: 2010.10.28
// (modified for style, etc. by Wendy Shoan/GSFC/583 2011.05.31)
//
/**
 * This is the base class for the HarmonicGravity set of classes
 * (HarmonicGravityCof, etc.).
 */
//------------------------------------------------------------------------------
#ifndef HarmonicGravity_hpp
#define HarmonicGravity_hpp

#include "gmatdefs.hpp"
#include "Harmonic.hpp"
#include "Rmatrix33.hpp"

class HarmonicGravity : public Harmonic
{
public:
   HarmonicGravity (const std::string& filename);
   HarmonicGravity(const HarmonicGravity& hg);
   HarmonicGravity& operator=(const HarmonicGravity& hg);
   virtual ~HarmonicGravity();

   std::string              GetFilename();

   virtual Real Cnm(const Real& jday, const Integer& n, const Integer& m) const;
   virtual Real Snm(const Real& jday, const Integer& n, const Integer& m) const;
   virtual void CalculatePointField(const Real& jday,         const Real pos[3],
                                    const Integer& nn,        const Integer& mm,
                                    const bool& usetides,     const Real sunpos[3], const Real moonpos[3],
                                    const bool& fillgradient, Real  acc[3],          Rmatrix33& gradient);
   virtual void CalculateFullField(const Real& jday,         const Real pos[3],
                                   const Integer& nn,        const Integer& mm,
                                   const bool& usetides,     const Real sunpos[3], const Real moonpos[3],
                                   const Real& sunMass,      const Real& moonMass,
                                   const Real &xp,           const Real &yp,
                                   const bool& fillgradient, Real  acc[3],          Rmatrix33& gradient);
protected:
   std::string gravityFilename;
   bool        useTideModel;

   /// Earth tide coefficients
   Real          CTide[5][5];
   Real          STide[5][5];

   /// Methods useful in Earth Tide computations
   void SetTide(const Real &jday,      const bool &removepermtide,
                const Real sunpos[3],  const Real moonpos[3],
                const Real &sunMass,   const Real &moonMass,
                const Real &earthMass, const Real &earthRadius,
                const Real &xp,        const Real &yp);
   void CartesianToPolar(const Real pos[3], Real out[3]);
   void PolarToLegendre(const Real polar[3], Real P[5][5]);

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
   static const Integer Table63cDim1 = 2;  // 21;  // 21??! should be 2
   static const Real    Table63c[Table63cDim1][6];

};

#endif // HarmonicGravity_hpp
