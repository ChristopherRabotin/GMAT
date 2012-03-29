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
#include "HarmonicGravity.hpp"
#include "ODEModelException.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "TimeSystemConverter.hpp"

//#define DEBUG_GRADIENT

using namespace GmatMathUtil;

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// Love numbers, k(n,m), IERS p.71, Table 6.3
const Real HarmonicGravity::k[4][4] =
{ { 0.0,     0.0,     0.0,     0.0   },
  { 0.0,     0.0,     0.0,     0.0   },
  { 0.29525, 0.29470, 0.29801, 0.0   },
  { 0.093,   0.093,   0.093,   0.094 } };
const Real HarmonicGravity::k_plus[3] =
   { -0.00087, -0.00079, -0.00057 };
//------------------------------------------------------------------------------
// Table 6.3a, IERS p.64 (for C21,S21 coefficients)
//     l   l'  F   D   O     ip    op
const Real HarmonicGravity::Table63a[Table63aDim1][7] =
{ { 2,  0,  2,  0,  2,   -0.1,  0   },
  { 0,  0,  2,  2,  2,   -0.1,  0   },
  { 1,  0,  2,  0,  1,   -0.1,  0   },
  { 1,  0,  2,  0,  2,   -0.7,  0.1 },
  {-1,  0,  2,  2,  2,   -0.1,  0   },
  { 0,  0,  2,  0,  1,   -1.3,  0.1 },
  { 0,  0,  2,  0,  2,   -6.8,  0.6 },
  { 0,  0,  0,  2,  0,    0.1,  0   },
  { 1,  0,  2, -2,  2,    0.1,  0   },
  {-1,  0,  2,  0,  1,    0.1,  0   },
  {-1,  0,  2,  0,  2,    0.4,  0   },
  { 1,  0,  0,  0,  0,    1.3, -0.1 },
  { 1,  0,  0,  0,  1,    0.3,  0   },
  {-1,  0,  0,  2,  0,    0.3,  0   },
  {-1,  0,  0,  2,  1,    0.1,  0   },
  { 0,  1,  2, -2,  2,   -1.9,  0.1 },
  { 0,  0,  2, -2,  1,    0.5,  0   },
  { 0,  0,  2, -2,  2,  -43.4,  2.9 },
  { 0, -1,  2, -2,  2,    0.6,  0   },
  { 0,  1,  0,  0,  0,    1.6, -0.1 },
  {-2,  0,  2,  0,  1,    0.1,  0   },
  { 0,  0,  0,  0, -2,    0.1,  0   },
  { 0,  0,  0,  0, -1,   -8.8,  0.5 },
  { 0,  0,  0,  0,  0,  470.9,-30.2 },
  { 0,  0,  0,  0,  1,   68.1, -4.6 },
  { 0,  0,  0,  0,  2,   -1.6,  0.1 },
  {-1,  0,  0,  1,  0,    0.1,  0   },
  { 0, -1,  0,  0, -1,   -0.1,  0   },
  { 0, -1,  0,  0,  0,  -20.6, -0.3 },
  { 0,  1, -2,  2, -2,    0.3,  0   },
  { 0, -1,  0,  0,  1,   -0.3,  0   },
  {-2,  0,  0,  2,  0,   -0.2,  0   },
  {-2,  0,  0,  2,  1,   -0.1,  0   },
  { 0,  0, -2,  2, -2,   -5.0,  0.3 },
  { 0,  0, -2,  2, -1,    0.2,  0   },
  { 0, -1, -2,  2, -2,   -0.2,  0   },
  { 1,  0,  0, -2,  0,   -0.5,  0   },
  { 1,  0,  0, -2,  1,   -0.1,  0   },
  {-1,  0,  0,  0, -1,    0.1,  0   },
  {-1,  0,  0,  0,  0,   -2.1,  0.1 },
  {-1,  0,  0,  0,  1,   -0.4,  0   },
  { 0,  0,  0, -2,  0,   -0.2,  0   },
  {-2,  0,  0,  0,  0,   -0.1,  0   },
  { 0,  0, -2,  0, -2,   -0.6,  0   },
  { 0,  0, -2,  0, -1,   -0.4,  0   },
  { 0,  0, -2,  0,  0,   -0.1,  0   },
  {-1,  0, -2,  0, -2,   -0.1,  0   },
  {-1,  0, -2,  0, -1,   -0.1,  0   } };
//------------------------------------------------------------------------------
// Table 6.3b, IERS p.66 (for C20 coefficient)
//     l   l'  F   D   O     ip    op
const Real HarmonicGravity::Table63b[Table63bDim1][7] =
{ { 0,  0,  0,  0,  1,   16.6, -6.7 },
  { 0,  0,  0,  0,  2,   -0.1,  0.1 },
  { 0, -1,  0,  0,  0,   -1.2,  0.8 },
  { 0,  0, -2,  2, -2,   -5.5,  4.3 },
  { 0,  0, -2,  2, -1,    0.1, -0.1 },
  { 0, -1, -2,  2, -2,   -0.3,  0.2 },
  { 1,  0,  0, -2,  0,   -0.3,  0.7 },
  {-1,  0,  0,  0, -1,    0.1, -0.2 },
  {-1,  0,  0,  0,  0,   -1.2,  3.7 },
  {-1,  0,  0,  0,  1,    0.1, -0.2 },
  { 1,  0, -2,  0, -2,    0.1, -0.2 },
  { 0,  0,  0, -2,  0,    0.0,  0.6 },
  {-2,  0,  0,  0,  0,    0.0,  0.3 },
  { 0,  0, -2,  0, -2,    0.6,  6.3 },
  { 0,  0, -2,  0, -1,    0.2,  2.6 },
  { 0,  0, -2,  0,  0,    0.0,  0.2 },
  { 1,  0, -2, -2, -2,    0.1,  0.2 },
  {-1,  0, -2,  0, -2,    0.4,  1.1 },
  {-1,  0, -2,  0, -1,    0.2,  0.5 },
  { 0,  0, -2, -2, -2,    0.1,  0.2 },
  {-2,  0, -2,  0, -2,    0.1,  0.1 } };
//------------------------------------------------------------------------------
// Table 6.3c, IERS p.66 (for C22 coefficient)
//     l   l'  F   D   O     amp
const Real HarmonicGravity::Table63c[Table63cDim1][6] =
{ { 1,  0,  2,  0,  2,  -0.3 },
  { 0,  0,  2,  0,  2,  -1.2 } };
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
HarmonicGravity::HarmonicGravity(const std::string& filename) :
   Harmonic        (),
   gravityFilename (filename),
   useTideModel    (false)
{
   for (Integer ii = 0; ii < 5; ii++)
   {
      for (Integer jj = 0; jj < 5; jj++)
      {
         CTide[ii][jj] = 0.0;
         STide[ii][jj] = 0.0;
      }
   }
}

HarmonicGravity::HarmonicGravity(const HarmonicGravity& hg) :
   Harmonic        (hg),
   gravityFilename (hg.gravityFilename),
   useTideModel    (hg.useTideModel)
{
   for (Integer ii = 0; ii < 5; ii++)
   {
      for (Integer jj = 0; jj < 5; jj++)
      {
         CTide[ii][jj] = hg.CTide[ii][jj];
         STide[ii][jj] = hg.STide[ii][jj];
      }
   }
}

HarmonicGravity& HarmonicGravity::operator=(const HarmonicGravity& hg)
{
   if (&hg == this)
      return *this;

   Harmonic::operator=(hg);
   gravityFilename = hg.gravityFilename;
   useTideModel    = hg.useTideModel;

   for (Integer ii = 0; ii < 5; ii++)
   {
      for (Integer jj = 0; jj < 5; jj++)
      {
         CTide[ii][jj] = hg.CTide[ii][jj];
         STide[ii][jj] = hg.STide[ii][jj];
      }
   }

   return *this;
}

//------------------------------------------------------------------------------
HarmonicGravity::~HarmonicGravity()
{
//   Deallocate();  // this is done at the Harmonic level!!!!!!
}

//------------------------------------------------------------------------------
std::string HarmonicGravity::GetFilename()
{
   return gravityFilename;
}

//------------------------------------------------------------------------------
Real HarmonicGravity::Cnm(const Real& jday, const Integer& n, const Integer& m) const
{
   if (useTideModel && (n <= 4) && (m <= 4))
      return C[n][m] + CTide[n][m];
   else
      return C[n][m];
}

//------------------------------------------------------------------------------
Real HarmonicGravity::Snm(const Real& jday, const Integer& n, const Integer& m) const
{
   if (useTideModel && (n <= 4) && (m <= 4))
      return S[n][m] + STide[n][m];
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
   useTideModel = usetides;
   Real r = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);
   if (r == 0)
      r = 0.01;   // was0,01 - is this correct?
   Real mu_r_3 = (-factor) / (r * r * r);   // factor = -mu
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
   Real earthRadius = bodyRadius;
   Real earthMass   = -factor/GmatPhysicalConstants::UNIVERSAL_GRAVITATIONAL_CONSTANT;
   useTideModel = usetides;
   bool removePermTide = C[2][0] < -4.84167E-04;
   if (useTideModel)
      SetTide(jday,removePermTide,sunpos,moonpos, sunMass, moonMass, earthMass, earthRadius, xp, yp);
   Real      accpoint[3];
   Real      accharmonic[3];
   Rmatrix33 gradientpoint;
   Rmatrix33 gradientharmonic;
   CalculatePointField(jday,pos,nn,mm,usetides,sunpos,moonpos,fillgradient,accpoint,gradientpoint);
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


void HarmonicGravity::SetTide (const Real& jday,      const bool& removepermtide,
                               const Real sunpos[3],  const Real moonpos[3],
                               const Real &sunMass,   const Real &moonMass,
                               const Real &earthMass, const Real &earthRadius,
                               const Real &xp,        const Real &yp)

{
   // Solid Earth Tide Model
   // Ref: IERS Technical Note 32, Chapter 6 (updated version, 22 March 2006)
   // Blair F. Thompson
   // last modified: 16 FEB 2009
   // function [dC,dS] = solid_tides(JD,r_moon,r_sun,xp,yp,perm_tide_flag,norm_coeff_flag,k,k_plus,table_63a,table_63b,table_63c,Re)
   // OUTPUT:
   //   dC = 5x5 matrix of the C gravity coefficient deltas
   //   dS = 5x5 matrix of the S gravity coefficient deltas
   //
   // INPUT:
   //   JD = (days) julian date
   //   r_moon = (meters) in earth centered fixed (ECF) coordinates (NOT ECI!)
   //   r_sun  = (meters) in earth centered fixed (ECF) coordinates (NOT ECI!)
   //   xp,yp  = (arcsec) polar motion parameters
   //   perm_tide_flag  = 1 if the gravity coefficient C20 includes the permanent tide
   //                  0 if it does not
   //   norm_coeff_flag = 1 if the gravity coefficients are normalized
   //                  0 if not

   // k         = elastic Love numbers
   // k_plus    = elastic Love numbers
   // table_63a = data from TechNote 32, p.64
   // table_63b = data from TechNote 32, p.66
   // table_63c = data from TechNote 32, p.66

   // Re = (meters) equatorial radius of Earth

   // data table lengths (fixed values)
   // length_table_63a = 48;
   // length_table_63b = 21;
   // length_table_63c = 2;

   // Compute needed celestial body quantities from inputs
   Real Re              = earthRadius;             // do these need to be the hard-coded
   Real mass_moon_earth = moonMass / earthMass;    // values from JPD code, in order to
   Real mass_sun_earth  = sunMass / earthMass;     // match the math here?

   // zero the coefficient delta matrices
   for (Integer n=0;  n<5;  ++n)   // wcs < instead of <=
      for (Integer m=0;  m<=n;  ++m)  // should this be m <= 4????
      {
         CTide[n][m] = 0.0;
         STide[n][m] = 0.0;
      }

   //------------------------------------------------------------------------------
   Real SunPolar[3];    // R,Latitude,Longitude (Radians)
   Real MoonPolar[3];   // R,Latitude,Longitude (Radians)
   for (Integer ii = 0; ii < 3; ii++)
   {
      SunPolar[ii]  = 0.0;
      MoonPolar[ii] = 0.0;
   }
   CartesianToPolar (sunpos,SunPolar);
   CartesianToPolar (moonpos,MoonPolar);

   Real PMoon[5][5];
   Real PSun[5][5];
   // Initialize these arrays
   for (Integer ii = 0; ii < 5; ii++)
   {
      for (Integer jj = 0; jj < 5; jj++)
      {
         PMoon[ii][jj] = 0.0;
         PSun[ii][jj]  = 0.0;
      }
   }
   PolarToLegendre (MoonPolar,PMoon);
   PolarToLegendre (SunPolar,PSun);

   //------------------------------------------------------------------------------
   // eqn 1, p.59
   // eqn 4, p.60 (4th degree coefficients)
   for (Integer n=2;  n<=3;  ++n)
      for (Integer m=0;  m<=n;  ++m)  // should this be m <= 2????
      {
         Real FMoon = mass_moon_earth*Pow(Re/MoonPolar[0],n+1)*PMoon[n][m];
         Real FSun  = mass_sun_earth*Pow(Re/SunPolar[0],n+1)*PSun[n][m];

         CTide[n][m] += k[n][m]/(2*n+1) * (FMoon*Cos(m*MoonPolar[2]) + FSun*Cos(m*SunPolar[2]));
         STide[n][m] += k[n][m]/(2*n+1) * (FMoon*Sin(m*MoonPolar[2]) + FSun*Sin(m*SunPolar[2]));
         if (n==2)
         {
            CTide[4][m] += k_plus[m]/(2*n+1) * (FMoon*Cos(m*MoonPolar[2]) + FSun*Cos(m*SunPolar[2]));
            STide[4][m] += k_plus[m]/(2*n+1) * (FMoon*Sin(m*MoonPolar[2]) + FSun*Sin(m*SunPolar[2]));
         }
      }

   //------------------------------------------------------------------------------
   // TechNote32 Step 2, p.60
   // compute GMST (or add it as a function input)
//   Real JD = jday + 2300000.0;  // what is this number and why is it added on???  wcs
   // jday is A1 JD; we want UT1 (approximated by UTC) JD
   Real a1mjd  = jday - GmatTimeConstants::JD_JAN_5_1941;
   Real JD     = TimeConverterUtil::Convert(a1mjd, TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD,
                 GmatTimeConstants::JD_JAN_5_1941) + GmatTimeConstants::JD_JAN_5_1941;
   Real t  = (JD-GmatTimeConstants::JD_OF_J2000)/GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;  // (ignore difference between TDB and TDT)
   Real t2 = t*t;
   Real t3 = t2*t;
   Real t4 = t3*t;
   Real GMST = 67310.54841 + 3164400184.812866*t + 0.093104*t2 - 6.2E-06*t3; // seconds of time
   GMST /= 240.0; // degrees

   // Fundamental arguments of nutation theory (Delaunay variables)
   // Tech Note 32 p.48
   Real F[5];
   F[0] = 134.96340251*3600 + 1717915923.2178*t + 31.8792*t2 + 0.051635*t3 - 0.00024470*t4;
   F[0] /= 3600;  // deg

   F[1] = 357.52910918*3600 + 129596581.0481*t - 0.5532*t2 + 0.000136*t3 - 0.00001149*t4;
   F[1] /= 3600;  // deg

   F[2] = 93.27209062*3600 + 1739527262.8478*t - 12.7512*t2 - 0.001037*t3 + 0.00000417*t4;
   F[2] /= 3600;  // deg

   F[3] = 297.85019547*3600 + 1602961601.2090*t - 6.3706*t2 + 0.006593*t3 - 0.00003169*t4;
   F[3] /= 3600;  // deg

   F[4] = 125.04455501*3600 - 6962890.5431*t + 7.4722*t2 + 0.007702*t3 - 0.00005939*t4;
   F[4] /= 3600;  // deg

   // compute (2,0) freq dependent terms, IERS eqn 5a, p.60, (n=2,m=0)
   Real freq_dep_C20 = 0;
   
   // This overflows the array:
   //for (Integer f=0;  f<=Table63bDim1;  ++f)
   for (Integer f=0;  f<Table63bDim1;  ++f)
   {
      Real theta_f = 0;
      for (Integer j=0;  j<=4;  ++j)
         theta_f += Table63b[f][j]*F[j];  // p.61
      theta_f = -theta_f * GmatMathConstants::RAD_PER_DEG; // radians
      freq_dep_C20 += (Table63b[f][5]*Cos(theta_f)-Table63b[f][6]*Sin(theta_f)); // eqn 5a
   }
   CTide[2][0] += freq_dep_C20 * 1e-12;

   // compute (2,1) freq dependent terms, IERS eqn 5b, p.60, (n=2,m=1)
   Real freq_dep_C21 = 0;
   Real freq_dep_S21 = 0;
   Integer m = 1;

   // This overflows the array:
   //for (Integer f=0;  f<=Table63aDim1;  ++f)
   for (Integer f=0;  f<Table63aDim1;  ++f)
   {
      Real theta_f = 0;
      for (Integer j=0;  j<=4;  ++j)
         theta_f += Table63a[f][j]*F[j];
      theta_f = (m*(GMST+GmatMathConstants::PI_DEG)-theta_f) * GmatMathConstants::RAD_PER_DEG; // radians
      freq_dep_C21 += Table63a[f][5]*Sin(theta_f)+Table63a[f][6]*Cos(theta_f); // eqn 5b
      freq_dep_S21 += Table63a[f][5]*Cos(theta_f)-Table63a[f][6]*Sin(theta_f); // eqn 5b
   }
   CTide[2][1] += freq_dep_C21 * 1e-12;
   STide[2][1] += freq_dep_S21 * 1e-12;

   // compute (2,2) freq dependent terms, IERS eqn 5b, p.60, (n=2,m=2)
   Real freq_dep_C22 = 0;
   Real freq_dep_S22 = 0;
   m = 2;

   // This overflows the array:
   //for (Integer f=0;  f<=Table63cDim1;  ++f)
   for (Integer f=0;  f<Table63cDim1;  ++f)
   {
      Real theta_f = 0;
      for (Integer j=0;  j<=4;  ++j)
         theta_f += Table63c[f][j]*F[j];
      theta_f = (m*(GMST+GmatMathConstants::PI_DEG)-theta_f) * GmatMathConstants::RAD_PER_DEG; // radians
      freq_dep_C22 += ( Table63c[f][5]*Cos(theta_f));
      freq_dep_S22 += (-Table63c[f][5]*Sin(theta_f));
   }

   CTide[2][2] += freq_dep_C22 * 1e-12;
   STide[2][2] += freq_dep_S22 * 1e-12;

   // solid earth pole tide, IERS p.65
   // Commented out unless we have xp and yp data

   Real ym2000 = (JD-GmatTimeConstants::JD_OF_J2000)/GmatTimeConstants::DAYS_PER_YEAR;  // years past 2000
   Real xp_bar = 0.054 + ym2000*0.00083;  // IERS p. 84
   Real yp_bar = 0.357 + ym2000*0.00395;  // IERS p. 84

   Real m1 =   xp-xp_bar;
   Real m2 = -(yp-yp_bar);

   CTide[2][1] -= 1.333E-09*(m1+0.0115*m2);
   STide[2][1] -= 1.333E-09*(m2-0.0115*m1);

   // ocean pole tide (TechNote 32 working version, section 6.3, p.10)
   CTide[2][1] -= 2.2344E-10*(m1-0.01737*m2);
   STide[2][1] -= 1.7680E-10*(m2-0.03351*m1);

   //------------------------------------------------------------------------------
   // IERS Step 3 (correct for permanent tide if needed, IERS p.66)
   if (removepermtide)  // if input C20 includes permanent tide then remove it
      CTide[2][0] -= (4.4228E-08)*(-0.31460)*k[2][0];
   
   #ifdef DEBUG_TIDE
      MessageInterface::ShowMessage("Tide coefficients:\n");
      for (Integer n=0;  n<5;  ++n)   // wcs < instead of <=
         for (Integer m=0;  m<=n;  ++m)
         {
            MessageInterface::ShowMessage("   C[%d][%d] = %le, S[%d][%d] = %le\n", n, m,
               CTide[n][m], n, m, STide[n][m]);
         }
   #endif      
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
void HarmonicGravity::CartesianToPolar(const Real pos[3], Real out[3])
{
   // R,Theta,Phi == R,Latitude,Longitude
   Real xy = Sqrt(pos[0]*pos[0] + pos[1]*pos[1]);
   out[0]  = Sqrt(xy*xy + pos[2]*pos[2]);
   out[1]  = ATan2(pos[2],xy);
   out[2]  = ATan2(pos[1],pos[0]);
}
//------------------------------------------------------------------------------
void HarmonicGravity::PolarToLegendre(const Real polar[3], Real P[5][5])
{
   Real s           = Sin(polar[1]);
   Real c           = Cos(polar[1]);
   try
   {
      Real sqrt5       = sqrt(5.0);
      Real sqrt5over3  = sqrt(5.0/3.0);
      Real sqrt7       = sqrt(7.0);
      Real sqrt7over6  = sqrt(7.0/6.0);
      Real sqrt7over15 = sqrt(7.0/15.0);
      Real sqrtPoint7  = sqrt(0.7);
      // P(2,0)
      P[2][0] = sqrt5*(1.5*s*s-0.5);
      P[2][1] = 3.0*sqrt5over3*c*s;
      P[2][2] = 1.5*sqrt5over3*c*c;
      P[3][0] = sqrt7*(2.5*s*s*s-1.5*s);
      P[3][1] = sqrt7over6*c*(7.5*s*s-1.5);
      P[3][2] = 7.5*sqrt7over15*c*c*s;
      P[3][3] = 2.5*sqrtPoint7*c*c*c;
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage("error computing sqrt.  Message is:\n");
      MessageInterface::ShowMessage("%s\n", (be.GetFullMessage()).c_str());
   }
}

