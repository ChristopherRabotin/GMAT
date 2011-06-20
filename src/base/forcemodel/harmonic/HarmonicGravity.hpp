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
   virtual ~HarmonicGravity();

   std::string              GetFilename();
//   virtual HarmonicGravity* Clone();
   virtual bool             HasVariableCS() const;

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
   std::string Filename;
   bool        UseTides;

private:
   // No copying allowed, so these are declared as private, but not implemented
   HarmonicGravity(const HarmonicGravity& x);
   HarmonicGravity& operator=(const HarmonicGravity& x);

};

#endif // HarmonicGravity_hpp
