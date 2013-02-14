//$Id$
//------------------------------------------------------------------------------
//                           Harmonic
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
 * This is the base class for the Harmonic set of classes (HarmonicGravity,
 * etc.).
 */
//------------------------------------------------------------------------------
//====================================================================
// Normalized Derived Associated Lengendre Polynomials (of the 1st kind)
// per the method of Ref.[1]. "Fully" normalized for compatibility with
// the EGM96 coefficients per Ref[2]
//
// REFERENCES:
// [1] Lundberg, J.B., and Schutz, B.E., "Recursion Formulas of Legendre
//     Functions for Use with Nonsingular Geopotential Models", Journal
//     of Guidance, Dynamics, and Control, Vol. 11, No.1, Jan.-Feb. 1988.
//
// [2] Heiskanen, W.A., and Moritz, H., "Physical Geodesy", W.H. Freeman
//     and Company, San Francisco, 1967.
//
// [3] Pines, S., "Uniform Representation of the Gravitational Potential
//     and its Derivatives", AIAA Journal, Vol. 11, No. 11, 1973.
//
// Steven Queen
// Goddard Space Flight Center
// Flight Dynamics Analysis Branch
// Steven.Z.Queen@nasa.gov
// February 28, 2003
//
// Modification History : 3/17/2003 - D. Conway, Thinking Systems, Inc.
// Under Contract:  P.O.  GSFC S-67521-G
//
// Modification History : May 2010 - John Downing, GSFC.
//====================================================================
#ifndef Harmonic_hpp
#define Harmonic_hpp

#include "gmatdefs.hpp"
#include "Rmatrix33.hpp"

class Harmonic
{
public:
   Harmonic();
   Harmonic(const Harmonic& x);
   Harmonic& operator=(const Harmonic& x);
   virtual ~Harmonic();

   virtual Real Cnm(const Real& jday, const Integer& n, const Integer& m) const;
   virtual Real Snm(const Real& jday, const Integer& n, const Integer& m) const;
   Integer      GetNN() const;
   Integer      GetMM() const;
   Real         GetRadius() const;
   Real         GetFactor() const;
   void         CalculateField(const Real& jday,  const Real pos[3],        const Integer& nn,
                               const Integer& mm, const bool& fillgradient, Real  acc[3],
                               Rmatrix33& gradient) const;

protected:
   Integer     NN;      // Maximum value of n (Jn=J2,J3...)
   Integer     MM;      // Maximum value of m (Jnm=Jn2,Jn3...
   Real        bodyRadius;  // Radius of body
   Real        factor;  // factor = 1 (magnetic) or -mu (gravity)
   Real        C[361][361];
   Real        S[361][361];
   Real**      A;       // Normalized 'derived' Assoc. Legendre Poly
   Real**      V;       // Normalization factor
   Real*       Re;      // powers of projection of pos onto x_ecf (re)
   Real*       Im;      // powers of projection of pos onto y_ecf (im)
   Real**      N1;      // Temporary
   Real**      N2;      // Temporary
   Real        VR01[361][361];    // Temporary
   Real        VR11[361][361];    // Temporary
   /// Flag used to warn about truncating matrix calculations to 20x20 only once
   static bool matrixTruncationWasPosted;

   void Allocate();
   void Deallocate();
   void Copy(Harmonic& x);

   static void AllocateArray(Real**& a,   const Integer& nn, const Integer& excess);
   static void AllocateArray(Real*& a,    const Integer& nn, const Integer& excess);
   static void DeallocateArray(Real**& a, const Integer& nn, const Integer& excess);
   static void DeallocateArray(Real*& a,  const Integer& nn, const Integer& excess);
   static void CopyArray(Real**& a, Real**& b, const Integer& nn, const Integer& excess);
   static void CopyArray(Real*& a,  Real*& b,  const Integer& nn, const Integer& excess);
//--------------------------------------------------------------------


};

#endif // Harmonic_hpp
