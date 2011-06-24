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
//#include <math.h>
#include "EarthTide.hpp"
#include "HarmonicGravity.hpp"
#include "ODEModelException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"

//#define DEBUG_GRADIENT

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// n/a

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
HarmonicGravity::HarmonicGravity(const std::string& filename) :
   Harmonic (),
   Filename (filename),
   UseTides (false)
{
}

//------------------------------------------------------------------------------
HarmonicGravity::~HarmonicGravity()
{
//   Deallocate();  // this is done at the Harmonic level!!!!!!
}

//------------------------------------------------------------------------------
std::string HarmonicGravity::GetFilename()
{
   return Filename;
}

//------------------------------------------------------------------------------
//HarmonicGravity* HarmonicGravity::Clone()
//{
//   HarmonicGravity* out = new HarmonicGravity(Filename);
//   out->NN = NN;
//   out->MM = MM;
//   out->Radius = Radius;
//   out->Factor = Factor;
//   out->Allocate();
//   out->Copy(*this);
//   return out;
//}

//------------------------------------------------------------------------------
bool HarmonicGravity::HasVariableCS() const
{
   return UseTides;
}

//------------------------------------------------------------------------------
Real HarmonicGravity::Cnm(const Real& jday, const Integer& n, const Integer& m) const
{
   if (n <= 4)
      return C[n][m] + EarthTide::CTide[n][m];
   else
      return C[n][m];
}

//------------------------------------------------------------------------------
Real HarmonicGravity::Snm(const Real& jday, const Integer& n, const Integer& m) const
{
   if (n <= 4)
      return S[n][m] + EarthTide::STide[n][m];
   else
      return S[n][m];
}

//------------------------------------------------------------------------------
void HarmonicGravity::CalculatePointField(const Real& jday,         const Real pos[3],
                                          const Integer& nn,        const Integer& mm,
                                          const bool& usetides,     const Real sunpos[3],
                                          const Real moonpos[3],    const bool& fillgradient,
                                          Real  acc[3],             Rmatrix33& gradient)
{
   UseTides = usetides;
   Real r = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);
   if (r == 0)
      r = 0.01;   // was0,01 - is this correct?
   Real mu_r_3 = (-Factor) / (r * r * r);   // Factor = -mu
   // Calculate acceleration
   for (Integer i=0;  i<=2;  ++i)
      acc[i] = -mu_r_3 * pos[i];
   // Calculate gradient
   if (fillgradient)
   {
      for (Integer i=0;  i<=2;  ++i)
         for (Integer j=0;  j<=2;  ++j)
         {
            gradient(i,j) = 3*mu_r_3 * pos[i]/r * pos[j]/r; 
            if (i==j)
               gradient(i,j) += -mu_r_3;
         }
   }
#ifdef DEBUG_GRADIENT
   MessageInterface::ShowMessage("In CalPtF, fillgradient = %s\n", (fillgradient? "true" : "false"));
   MessageInterface::ShowMessage("gradientPoint = %s\n", gradient.ToString().c_str());
#endif
}

//------------------------------------------------------------------------------
void HarmonicGravity::CalculateFullField(const Real& jday,         const Real pos[3],
                                         const Integer& nn,        const Integer& mm,
                                         const bool& usetides,     const Real sunpos[3],
                                         const Real moonpos[3],    const Real& sunMass,
                                         const Real& moonMass,     const Real &xp,
                                         const Real &yp,           const bool& fillgradient,
                                         Real  acc[3],             Rmatrix33& gradient)
{
   // Must set so that Cnn,Snm know to call tide function;
   Real earthRadius = Radius;
   Real earthMass   = -Factor/GmatPhysicalConstants::UNIVERSAL_GRAVITATIONAL_CONSTANT;
   UseTides = usetides;
   bool removePermTide = C[2][0] < -4.84167E-04;
   if (UseTides)
      EarthTide::SetTide(jday,removePermTide,sunpos,moonpos,
                         sunMass, moonMass, earthMass, earthRadius, xp, yp);
   Real      accpoint[3];
   Real      accharmonic[3];
   Rmatrix33 gradientpoint;
   Rmatrix33 gradientharmonic;
   CalculatePointField(jday,pos,nn,mm,usetides,sunpos,moonpos,
                       fillgradient,accpoint,gradientpoint);
   CalculateField(jday,pos,nn,mm,fillgradient,accharmonic,gradientharmonic);
   for (Integer i=0;  i<=2;  ++i)
      acc[i] = accpoint[i] + accharmonic[i];
   if (fillgradient)
      gradient = gradientpoint + gradientharmonic;
#ifdef DEBUG_GRADIENT
   MessageInterface::ShowMessage("In CalFullField, fillgradient = %s\n", (fillgradient? "true" : "false"));
   MessageInterface::ShowMessage("gradient = %s\n", gradient.ToString().c_str());
#endif
}
