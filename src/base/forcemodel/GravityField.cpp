//$Header$
//------------------------------------------------------------------------------
//                              GravityField
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: D. Conway
// Created: 2003/03/17
// Modified: 2004/04/15 W. Shoan GSFC Code 583
//           Updated for GMAT style, standards; changed Real to Real, int
//           to Integer, etc.
//
//           2004/11/02 D. Conway Thinking Systems, Inc.
//           Changed internal arrays to static arrays to fix memory error.
//
/**
 * This is the GravityField class.
 *
 * @note original prolog information included at end of file prolog.
 */
//
// *** File Name : shgravityfield.cpp
// *** Created   : March 17, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C/C++ Source
// Development Environment   : Borland C++ 5.02
// Modification History      : 3/17/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery.  Happy St. Patrick's Day!
// **************************************************************************

#include <iostream>
#include <fstream>
//#include <strstream>
#include <sstream>
#include <iomanip>
#include "GravityField.hpp"
#include "ForceModelException.hpp"
#include "CelestialBody.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "Rvector.hpp"
#include "TimeTypes.hpp"
//#include "SolarSystemException.hpp"


// #define DEBUG_GRAVITY_FIELD
// #define DEBUG_GRAVITY_FIELD_DETAILS


using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------
const std::string
GravityField::PARAMETER_TEXT[GravityFieldParamCount - HarmonicFieldParamCount] =
{
   "Mu",
   "A"   // be more descriptive here?
};

const Gmat::ParameterType
GravityField::PARAMETER_TYPE[GravityFieldParamCount - HarmonicFieldParamCount] =
{
Gmat::REAL_TYPE,
Gmat::REAL_TYPE
};



//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GravityField(const std::string &name, const std::string &bodyName,
//               Integer deg, Integer ord)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the GravityField class
 * (default constructor).
 *
 * @param <name> parameter indicating the name of the object.
 * @param <deg>  maximum degree of he polynomials.
 * @param <ord>  maximum order of he polynomials.
 */
//------------------------------------------------------------------------------
GravityField::GravityField(const std::string &name, const std::string &forBodyName,
                           Integer maxDeg, Integer maxOrd) :
    HarmonicField   (name, "GravityField", maxDeg, maxOrd),
    mu              (398600.4415),
    a               (6378.1363),
    defaultMu       (398600.4415),
    defaultA        (6378.1363),
//    Cbar            (NULL),
//    Sbar            (NULL),
//    dCbar           (NULL),
//    dSbar           (NULL),
    gfInitialized   (false)
{
   bodyName = forBodyName;
   parameterCount = GravityFieldParamCount;
}


//------------------------------------------------------------------------------
//  ~GravityField()
//------------------------------------------------------------------------------
/**
 * This method destroys the GravityField object
 * (destructor).
 *
 */
//------------------------------------------------------------------------------
GravityField::~GravityField(void)
{
//   int cc;
//
//   if (Cbar) {
//      for (cc = 0;cc <= maxDegree+1; ++cc)
//         delete [] Cbar[cc];
//      delete Cbar;
//   }
//
//   if (Sbar) {
//      for (cc = 0;cc <= maxDegree+1; ++cc)
//         delete [] Sbar[cc];
//      delete Sbar;
//   }
//
//   if (dCbar) {
//      for (cc = 0;cc <= GRAV_MAX_DRIFT_DEGREE; ++cc)
//         delete [] dCbar[cc];
//      delete dCbar;
//   }
//
//   if (dSbar) {
//      for (cc = 0;cc <= GRAV_MAX_DRIFT_DEGREE; ++cc)
//         delete [] dSbar[cc];
//      delete dSbar;
//   }
}


//------------------------------------------------------------------------------
//  GravityField(const GravityField & gf)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the GravityField class from the input
 * object.
 * (copy constructor).
 *
 * @param <gf> object to copy to create this new object.
 */
//------------------------------------------------------------------------------
GravityField::GravityField(const GravityField &gf) :
    HarmonicField   (gf),
    mu              (gf.mu),
    a               (gf.a),
    defaultMu       (gf.defaultMu),
    defaultA        (gf.defaultA),
//    Cbar            (NULL),  // or (gf.Cbar),  
//    Sbar            (NULL),  // or (gf.Sbar),
//    dCbar           (NULL),  // or (gf.dCbar),
//    dSbar           (NULL),  // or (gf.dSbar)
    gfInitialized   (false)
{
}


//------------------------------------------------------------------------------
//  GravityField& operator=(const GravityField & gf)
//------------------------------------------------------------------------------
/**
 * This method sets the 'this' object to the value(s) of the input
 * object.
 *
 * @param <gf> object whose values to copy to this object.
 */
//------------------------------------------------------------------------------
GravityField& GravityField::operator=(const GravityField &gf)
{
   if (&gf == this)
      return *this;
   
//   int cc;
   HarmonicField::operator=(gf);
   mu               = gf.mu;
   a                = gf.a;
   defaultMu        = gf.defaultMu;
   defaultA         = gf.defaultA;
//   if (Cbar) {
//      for (cc = 0;cc <= maxDegree+1; ++cc)
//         delete [] Cbar[cc];
//      delete [] Cbar;
//      Cbar  = NULL;
//   }
//
//   if (Sbar) {
//      for (cc = 0;cc <= maxDegree+1; ++cc)
//         delete [] Sbar[cc];
//      delete [] Sbar;
//      Sbar  = NULL;
//   }
//
//   if (dCbar) {
//      for (cc = 0;cc <= GRAV_MAX_DRIFT_DEGREE; ++cc)
//         delete [] dCbar[cc];
//      delete [] dCbar;
//      dCbar = NULL;
//   }
//
//   if (dSbar) {
//      for (cc = 0;cc <= GRAV_MAX_DRIFT_DEGREE; ++cc)
//         delete [] dSbar[cc];
//      delete [] dSbar;
//      dSbar = NULL;
//   }
   gfInitialized = false;  // is that what I want to do?
   
   return *this;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the object.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::Initialize(void)
{
   if (!HarmonicField::Initialize())
   {
      throw ForceModelException(
            "GravityField: Legendre Polynomial initialization failed!");
   }
    
   degreeTruncateReported = false;
   orderTruncateReported  = false;

   return gravity_init();
}



/**
 * Gravitational model based on degree n and order m spherical harmonics.
 * Uses normalized "derived" associated legendre functions to prevent
 * numerical overflow at order and degree above 90 per Ref.[1].
 * Also, a set of transformed coordinates per Ref. [2] is used to avoid
 * the singularity at the poles.
 *
 * INPUTS:  G     = pointer to gravity structure (see gravity_init)
 *          LP    = pointer to Legendre Polynomial structure (see legendreP)
 *          jday  = julian day (date) for coefficient drift
 *
 * OUTPUTS: F     = acceleration vector in ITRF (terrestrial) coordinates
 *                  (distance/sec^2 units determined by G.mu, G.a and LP.r)
 *
 * REFERENCES:
 * [1]   Lundberg, J.B., and Schutz, B.E., "Recursion Formulas of Legendre
 *       Functions for Use with Nonsingular Geopotential Models", Journal
 *       of Guidance, Dynamics, and Control, Vol. 11, No.1, Jan.-Feb. 1988.
 *
 * [2]   Pines, S., "Uniform Representation of the Gravitational Potential
 *       and its Derivatives", AIAA Journal, Vol. 11, No. 11, 1973.
 *
 * Steven Queen
 * Goddard Space Flight Center
 * Flight Dynamics Analysis Branch
 * Steven.Z.Queen@nasa.gov
 * February 28, 2003
 */
//------------------------------------------------------------------------------
//  bool gravity_rtq(Real jday, Real F[])
//------------------------------------------------------------------------------
/**
 * This method computed the gfravitational force.
 *
 * @param <jday> julian day
 * @param <F>    force (output)
 *
 * @return success flag.
 *
 * @note - reused prolog information looks to be out-of-date
 */
//------------------------------------------------------------------------------
bool GravityField::gravity_rtq(Real jday, Real F[] )
{
   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("Entered gravity_rtq with Julian epoch %le\n", jday);
   #endif

   if (!gfInitialized)
      if (!Initialize())
         return false;

   // if file has been changed, after initialization, read the new file
   if (!fileRead)
      if (!gravity_init())
         return false;

   Integer            n, m;
   Real               p, summ[4], D, dT, Cbar_nm, Sbar_nm;

   Real               arr[4] = {0.0, 0.0, 0.0, 0.0};
   Real               sqrt2 = Sqrt(2.0);

   if (r == 0.0)  // (should I) check for hFinitialized here?
   {
      throw ForceModelException("gravity_rtq: Spherical representation not initialized!");
   }

   p = (a/r)*mu/r; /* Ref.[2], Eq.(26), n = 1 */
   arr[3] = -mu/r/r;
   p *= a/r;

   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("  Calculated p = %le\n", p);
      MessageInterface::ShowMessage("  Iterating over degree = %d, order = %d\n",
         degree, order);
   #endif

//   for (n=2;n<=min(G->degree,LP->degree);n++) {
   for (n = 2; n <= degree; ++n)
   {

      p *= a/r; /* Ref.[2], Eq.(26), rho_n+1 */

//      for (m = 0;( m <= n && m <= min(G->order,LP->order) );m++) {
      for (m = 0;(m <= n && m <= order); ++m)
      {
         #ifdef DEBUG_GRAVITY_FIELD_DETAILS
            dT = (jday - (Real)2.4464305e+06)/(Real)365.25; /* years since Jan 1, 1986 */
            MessageInterface::ShowMessage("  m = %d  n = %d\n   Cbar[n][m] = %le   Sbar[n][m] = %le\n",
               m, n, Cbar[n][m], Sbar[n][m]);
            if ( n <= GRAV_MAX_DRIFT_DEGREE )
            {
               MessageInterface::ShowMessage("  dCbar[n][m] = %le  dSbar[n][m] = %le  dt = %le\n",
                  dCbar[n][m], dSbar[n][m], dT);
            }
         #endif

         /* time rate of change of coefficients (drift/year)*/
         if ( n <= GRAV_MAX_DRIFT_DEGREE )
         {
            dT = (jday - (Real)2.4464305e+06)/(Real)365.25; /* years since Jan 1, 1986 */
            Cbar_nm = Cbar[n][m] + dCbar[n][m] * dT;
            Sbar_nm = Sbar[n][m] + dSbar[n][m] * dT;
         }
         else
         {
            Cbar_nm = Cbar[n][m];
            Sbar_nm = Sbar[n][m];
         }
         
         /* acceleration vector coefficients, Ref.[2], Eqs.(27), (30) */
         if ( m == 0 )
         {
            summ[0] = (Real)0.0;
            summ[1] = (Real)0.0;
            summ[2] = Sqrt( (Real)( n*(n+1) ) )*Abar[n][1]*Cbar_nm;
            summ[3] = Sqrt( (Real)( (2*n+1)*(n+2)*(n+1) )/
                            (Real)( 2*n+3 ) )*Abar[n+1][1]*Cbar_nm;
         }
         else
         {
            summ[0] += Abar[n][m]*m*( Cbar_nm*re[m-1] + Sbar_nm*im[m-1] )*sqrt2;
            summ[1] += Abar[n][m]*m*( Sbar_nm*re[m-1] - Cbar_nm*im[m-1] )*sqrt2;
            D = (Cbar_nm*re[m] + Sbar_nm*im[m])*sqrt2;
            summ[2] += Sqrt( (Real)((n-m)*(n+m+1)) )*Abar[n][m+1]*D;
            summ[3] += Sqrt( (Real)( (2*n+1)*(n+m+2)*(n+m+1) )/
                             (Real)( 2*n+3 ) )*Abar[n+1][m+1]*D;
         }
      } /* end m summation */
      
      #ifdef DEBUG_GRAVITY_FIELD
         MessageInterface::ShowMessage("  Calculated summ = %le  %le  %le  %le\n",
            summ[0], summ[1], summ[2], summ[3]);
      #endif

      arr[0] += p/a*summ[0];
      arr[1] += p/a*summ[1];
      arr[2] += p/a*summ[2];
      arr[3] -= p/a*summ[3];

   } /* end n summation */

   /* acceleration "force" vector, Ref.[2], Eq.(31) */
   F[0] = arr[0] + arr[3]*s;
   F[1] = arr[1] + arr[3]*t;
   F[2] = arr[2] + arr[3]*u;

   return true;
}

//------------------------------------------------------------------------------
//  bool GetDerivatives(Real * state, Real dt, int dvorder)
//------------------------------------------------------------------------------
/**
* This method initializes the gravity parameters.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::GetDerivatives(Real * state, Real dt, Integer dvorder)
{
   if ((dvorder > 2) || (dvorder < 1))
      return false;
      
   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("%s %d %s %le %s  %le %le %le %le %le %le\n",
          "Entered GravityField::GetDerivatives with order", dvorder, "dt = ",
          dt, "and state\n",
          state[0], state[1], state[2], state[3], state[4], state[5]);
   #endif
      
/// @todo Optimize this code -- May be possible to make GravityField calcs more efficient

   Integer stateSize = 6;
   Integer satcount = dimension / stateSize; 
   
   if (stateSize * satcount != dimension)
      throw ForceModelException("GravityField state dimension and state size do not match!");

   // Currently hardcoded for one spacecraft.  - remove this!!!!!!!!!!!!!!
   if (satcount < 1)
      throw ForceModelException("GravityField requires at least one spacecraft.");
      
   Real* satState;
   Real f[3], rbb3, mu_rbb, aIndirect[3], now;
   Integer nOffset;
   
   now = epoch + dt/GmatTimeUtil::SECS_PER_DAY;
   const Rvector6 *rv = &(body->GetState(now));

   // Precalculations for the indirect effect term
   rbb3 = (*rv)[0] * (*rv)[0] + (*rv)[1] * (*rv)[1] + (*rv)[2] * (*rv)[2];
   if (rbb3 != 0.0) {
      rbb3 = rbb3 * sqrt(rbb3);
      mu_rbb = mu / rbb3;
      aIndirect[0] = mu_rbb * (*rv)[0];
      aIndirect[1] = mu_rbb * (*rv)[1];
      aIndirect[2] = mu_rbb * (*rv)[2];
   }
   else
      aIndirect[0] = aIndirect[1] = aIndirect[2] = 0.0;

   for (Integer n = 0; n < satcount; ++n) {
      nOffset = n * stateSize;
      satState = state + nOffset;
    
      if (!legendreP_rtq(satState))
         throw ForceModelException("GravityField::legendreP_rtq failed");

      #ifdef DEBUG_GRAVITY_FIELD
         MessageInterface::ShowMessage("%s%le  s = %le  t = %le  u = %le\n", 
            "legendreP_rtq r = ", r, s, t, u);
         MessageInterface::ShowMessage("%s%le\n", 
            "Calling gravity_rtq for Julian epoch = ", epoch + 2430000.0 + dt/86400.0);
      #endif

      if (!gravity_rtq(epoch + 2430000.0 + dt/86400.0, f))
         throw ForceModelException("GravityField::gravity_rtq failed");

      #ifdef DEBUG_GRAVITY_FIELD
         MessageInterface::ShowMessage("%s %le  %le  %le\n", 
            "gravity_rtq returned force = ", f[0], f[1], f[2]);
         MessageInterface::ShowMessage("%s %le  %le  %le\n", 
            "Indirect term is", aIndirect[0], aIndirect[1], aIndirect[2]);
      #endif

   
      switch (dvorder)
      {
         case 1:
            deriv[0+nOffset] = satState[3];
            deriv[1+nOffset] = satState[4];
            deriv[2+nOffset] = satState[5];
            deriv[3+nOffset] = f[0] - aIndirect[0];
            deriv[4+nOffset] = f[1] - aIndirect[1];
            deriv[5+nOffset] = f[2] - aIndirect[2];
            break;
   
         case 2:
            deriv[0+nOffset] = f[0] - aIndirect[0];
            deriv[1+nOffset] = f[1] - aIndirect[1];
            deriv[2+nOffset] = f[2] - aIndirect[2];
            deriv[3+nOffset] =
               deriv[4+nOffset] =
               deriv[5+nOffset] = 0.0;
            break;
   
         default:
            throw ForceModelException("GravityField::GetDerivatives requires order = 1 or 2");
      }
   }

   return true;
}

//---------------------------------
// inherited methods from GmatBase
//---------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the GravityField.
 *
 * @return clone of the GravityField.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GravityField::Clone(void) const
{
   return (new GravityField(*this));
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string GravityField::GetParameterText(const Integer id) const
{
   if ((id >= HarmonicFieldParamCount) && (id < GravityFieldParamCount))
      return PARAMETER_TEXT[id - HarmonicFieldParamCount];
   return HarmonicField::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Integer     GravityField::GetParameterID(const std::string &str) const
{
   for (Integer i = HarmonicFieldParamCount; i < GravityFieldParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - HarmonicFieldParamCount])
         return i;
   }
   return HarmonicField::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType
GravityField::GetParameterType(const Integer id) const
{
   if ((id >= HarmonicFieldParamCount) && (id < GravityFieldParamCount))
      return PARAMETER_TYPE[id - HarmonicFieldParamCount];
   return HarmonicField::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string GravityField::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];   
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param id    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real        GravityField::GetRealParameter(const Integer id) const
{
   if (id == MU)       return mu;
   if (id == A)        return a;
 
   return HarmonicField::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
//------------------------------------------------------------------------------
Real        GravityField::SetRealParameter(const Integer id,
                                           const Real value)
{
   if (id == MU)       return (mu     = value);
   if (id == A)        return (a      = value);

   return HarmonicField::SetRealParameter(id, value);
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
Real        GravityField::GetRealParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id);
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
Real        GravityField::SetRealParameter(const std::string &label,
                                           const Real value)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value);
}
                                     

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  bool gravity_init()
//------------------------------------------------------------------------------
/**
* This method initializes the gravity parameters.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::gravity_init(void)
{
//   Integer      cc, dd, size=0;
   Integer      defDegree, defOrder;
   //Integer      n=0, m=0, iscomment, rtn;

   //if (body == NULL) throw ForceModelException("Body undefined for GravityField.");
   if (body == NULL)
   {
      if (solarSystem == NULL)
         throw ForceModelException("Solar System undefined in GravityField.");
      body = solarSystem->GetBody(bodyName);
      if (body == NULL) throw ForceModelException("Body \"" + bodyName + 
                                           "\" undefined for GravityField.");
   }

//   // delete arrays, if they have already been created (useful when reading
//   // a new file)
//   if (Cbar) {
//      for (cc = 0;cc <= maxDegree+1; ++cc) {
//         delete [] Cbar[cc];
//      }
//      delete [] Cbar;
//      Cbar = NULL;
//   }
//
//   if (Sbar) {
//      for (cc = 0;cc <= maxDegree+1; ++cc) {
//         delete [] Sbar[cc];
//      }
//      delete [] Sbar;
//      Sbar = NULL;
//   }
//
//   if (dCbar) {
//      for (cc = 0;cc <= GRAV_MAX_DRIFT_DEGREE; ++cc)
//         delete [] dCbar[cc];
//      delete [] dCbar;
//      dCbar = NULL;
//   }
//
//   if (dSbar) {
//      for (cc = 0;cc <= GRAV_MAX_DRIFT_DEGREE; ++cc)
//         delete [] dSbar[cc];
//      delete [] dSbar;
//      dSbar = NULL;
//   }
//   
//   for (cc = 2;cc <= maxDegree; ++cc)
//   {
//      for (dd = 0; dd <= cc; ++dd)
//      {
//         size++;
//      }
//   }
//
//   Cbar  = new Real*[maxDegree+3];
//   Sbar  = new Real*[maxDegree+3];
//   dCbar = new Real*[GRAV_MAX_DRIFT_DEGREE+1];
//   dSbar = new Real*[GRAV_MAX_DRIFT_DEGREE+1];
//
//   if ( !Cbar || !Sbar || !dCbar || !dSbar )
//   {
//      throw ForceModelException("In GravityField, gravity_init: "
//                                "memory allocation failed!");
//      return false;
//   }
//   for (cc = 0;cc <= maxDegree+1; ++cc)
//   {
//      if ( maxOrder >= cc )
//      {
//         Cbar[cc] = new Real[cc+3];
//         Sbar[cc] = new Real[cc+3];
//      }
//      else
//      {
//         Cbar[cc] = new Real[maxOrder+3];
//         Sbar[cc] = new Real[maxOrder+3];
//      }
//      if ( !Cbar[cc] || !Sbar[cc] )
//      {
//         throw ForceModelException("GravityField::gravity_init: Cannot allocate Cbar or Sbar");
//         return false;
//      }
//
//      if ( cc <= GRAV_MAX_DRIFT_DEGREE )
//      {
//         dCbar[cc] = new Real[GRAV_MAX_DRIFT_DEGREE+1];
//         dSbar[cc] = new Real[GRAV_MAX_DRIFT_DEGREE+1];
//         if ( !dCbar[cc] || !dSbar[cc] )
//         {
//            throw ForceModelException("GravityField::gravity_init: Cannot allocate dCbar or dSbar");
//            return false;
//         }
//      }
//   }

   if (!fileRead) {        // Only read the file if the name has changed or it 
                           // was not yet read
      if (!ReadFile())
      {
         // try to get default coefficients from the body
         MessageInterface::ShowMessage("Using default coefficients from the body.\n");
         mu     = body->GetGravitationalConstant();
         a      = body->GetEquatorialRadius();
         Rmatrix sij = body->GetHarmonicCoefficientsSij();
         Rmatrix cij = body->GetHarmonicCoefficientsCij();
         Integer idx,jdx;
         Integer srows, scolumns, crows, ccolumns;
         sij.GetSize(srows, scolumns);
         cij.GetSize(crows, ccolumns);
         for (idx=0; idx<srows; idx++)
            for (jdx=0; jdx<scolumns; jdx++)
               Sbar[idx][jdx] = sij.GetElement(idx,jdx);
         for (idx=0; idx<crows; idx++)
            for (jdx=0; jdx<ccolumns; jdx++)
               Cbar[idx][jdx] = cij.GetElement(idx,jdx);
   
         // zero out the drift values, as there are no default values in the body - should there be?
         for (idx=0; idx<GRAV_MAX_DRIFT_DEGREE+1;idx++)
         {
            for (jdx=0; jdx<GRAV_MAX_DRIFT_DEGREE+1;jdx++)
            {
               dSbar[idx][jdx] = 0.0;
               dCbar[idx][jdx] = 0.0;
            }
         }
   
         defDegree = body->GetDegree();
         defOrder  = body->GetOrder();
         // truncate the degree and/or order, if necessary
   //      if ((defDegree > degree) && !degreeTruncateReported) {
   //      {
   //         MessageInterface::ShowMessage(
   //            "In GravityField, truncating to degree = %d\n",degree);
   //         degreeTruncateReported = true;
   //
   //      }
   //      else 
         if (defDegree < degree)
         {
            MessageInterface::ShowMessage(
                           "In GravityField, truncating to degree = %d\n",degree);
         }
         else if (defDegree < degree)
         {
            degree = defDegree;
            MessageInterface::ShowMessage(
                           "In GravityField, truncating to degree = %d\n", degree);
         }
   //      if ((defOrder > order) && !orderTruncateReported)
   //      {
   //         MessageInterface::ShowMessage(
   //                        "In GravityField, truncating to order = %d\n",order);
   //      }
   //      else 
         if ((defOrder < order) && !orderTruncateReported)
         {
            MessageInterface::ShowMessage(
                           "In GravityField, truncating to order = %d\n",order);
         }
         else if (defOrder < order)
         {
            order = defOrder;
            MessageInterface::ShowMessage(
                           "In GravityField, truncating to order = %d\n", order);
         }
         if (order > degree)
         {
            order = degree;
            MessageInterface::ShowMessage(
                           "In GravityField, truncating to order = %d\n", order);
         }
         //return false;
      }
   }
   
   /* transform from tide free to zero tide system */ /* <<<<-- took out to match STK (SQ)*/
   /* G.Cbar[2][0] += (Real)3.11080e-8 * 0.3 / Sqrt((Real)5.0); */

   gfInitialized = true;

   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("%s%lf%s%lf\n",
         "GravityField::gravity_init() succeeded, mu = ", mu, " r = ", a);
      MessageInterface::ShowMessage("%s%d%s%d\n", 
         "   maxDegree = ", maxDegree, "  maxOrder = ", maxOrder);
      MessageInterface::ShowMessage("   C20 = %le S20 = %le\n",
         Cbar[2][0], Sbar[2][0]);
      MessageInterface::ShowMessage("   C21 = %le S21 = %le\n",
         Cbar[2][1], Sbar[2][1]);
      MessageInterface::ShowMessage("   C22 = %le S22 = %le\n",
         Cbar[2][2], Sbar[2][2]);
      MessageInterface::ShowMessage("   C44 = %le S44 = %le\n",
         Cbar[4][4], Sbar[4][4]);
      MessageInterface::ShowMessage("   dC20 = %le dS20 = %le\n",
         dCbar[2][0], dSbar[2][0]);
      MessageInterface::ShowMessage("   dC21 = %le dS21 = %le\n",
         dCbar[2][1], dSbar[2][1]);
   #endif
   
   return true;
}

//------------------------------------------------------------------------------
//  bool ReadFile()
//------------------------------------------------------------------------------
/**
 * This method reads the potential file.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::ReadFile()
{
   Integer      fileDegree, fileOrder;
   std::string  errMsg;
   bool         isOk = true;
   
   #ifdef DEBUG_GRAVITY_FIELD
      char str[1024];
      strcpy(str, filename.c_str());
      MessageInterface::ShowMessage("%s \"%s\"\n",
         "GravityField::ReadFile() called for file", str);
   #endif
   

   // Determine the type of file  --> add switch later!!!!!!!!!!
   if ((filename.find(".dat",0) != std::string::npos) ||
       (filename.find(".DAT",0) != std::string::npos) )
   {
      if (!ReadDatFile(fileDegree, fileOrder))
      {
         errMsg = "Error reading coefficients, mu, and equatorial radius from "
                  + filename;
         //throw ForceModelException(errMsg);
         isOk = false;
      }
   }
   else if ((filename.find(".grv",0) != std::string::npos) ||
            (filename.find(".GRV",0) != std::string::npos) )
   {
      if (!ReadGrvFile(fileDegree, fileOrder))
      {
         errMsg = "Error reading coefficients, mu, and equatorial radius from "
                  + filename;
         //throw ForceModelException(errMsg);
         isOk = false;
      }
   }
   else if ((filename.find(".cof",0) != std::string::npos) ||
            (filename.find(".COF",0) != std::string::npos) )
   {
      if (!ReadCofFile(fileDegree, fileOrder))
      {
         errMsg = "Error reading coefficients, mu, and equatorial radius from "
                  + filename;
         //throw ForceModelException(errMsg);
         isOk = false;
      }
   }
   else
   {
      errMsg = "Gravity file \"" + filename + "\" is of unknown format.";
      //throw ForceModelException(errMsg);
      isOk = false;
   }
   if (!isOk)
   {
      MessageInterface::ShowMessage(errMsg);
      fileRead = true;  // will be reset if/when new filename is set
      return false;
   }

   // truncate the degree and/or order, if necessary
//   if (fileDegree > degree)
//   {
//     MessageInterface::ShowMessage(
//                        "In GravityField, truncating to degree = %d\n",degree);
//   }
//   else 
   if (fileDegree < degree)
   {
      degree = fileDegree;
      MessageInterface::ShowMessage(
                        "In GravityField, truncating to degree = %d\n", degree);
   }
//   if (fileOrder > order)
//   {
//      MessageInterface::ShowMessage(
//                        "In GravityField, truncating to order = %d\n",order);
//   }
//   else 
   if (fileOrder < order)
   {
      order = fileOrder;
      MessageInterface::ShowMessage(
                        "In GravityField, truncating to order = %d\n", order);
   }
   if (order > degree)
   {
      order = degree;
      MessageInterface::ShowMessage(
                        "In GravityField, truncating to order = %d\n", order);
   }

   if (degree < 0)
      throw ForceModelException("Invalid degree in GravityField: Degree < 0");
   if (order < 0)
      throw ForceModelException("Invalid degree in GravityField: Degree < 0");

   fileRead = true;
   return true;
}

//------------------------------------------------------------------------------
//  bool ReadCofFile(Integer& fileDeg, Integer& fileOrd)
//------------------------------------------------------------------------------
/**
 * This method reads a ???????? file to get the gravity parameters.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::ReadCofFile(Integer& fileDeg, Integer& fileOrd)
{
   Integer       n, m;
   Integer       fileOrder, fileDegree;
   Real          Cnm=0.0, Snm=0.0; // , dCnm=0.0, dSnm=0.0;
   Integer       noIdea;
   Real          noClue;
   Real          tmpMu;
   Real          tmpA;

   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("Entered GravityField::ReadCofFile\n");
   #endif

   std::ifstream inFile;
   inFile.open(filename.c_str());
   if (!inFile)
   {
      //throw ForceModelException("Cannot open file " + filename);
      //MessageInterface::ShowMessage("Cannot open file %s \n", filename);
      return false;
   }

   PrepareArrays();

   std::string s;
   std::string firstStr;
   while (!inFile.eof())
   {
      getline(inFile,s);
      std::istringstream lineStr;
      lineStr.str(s);
      // ignore comment lines
      if (s[0] != 'C')
      {
         lineStr >> firstStr;
         if (firstStr == "END") break;
         if (firstStr == "POTFIELD")
         {
            lineStr >> fileDegree >> fileOrder >> noIdea >> tmpMu >> tmpA >> noClue;
            if (tmpMu == 0.0)
               mu = defaultMu;
            else
               mu = tmpMu / 1.0e09;  // -> Km^3/sec^2
            if (tmpA  == 0.0)
               a  = defaultA;
            else
               a = tmpA / 1000.0;  // -> km
         }
         else if (firstStr == "RECOEF")
         {
            lineStr >> n >> m >> Cnm >> Snm; 
               if ( n <= HF_MAX_DEGREE && m <= HF_MAX_ORDER )
               {
                  Cbar[n][m] = (Real)Cnm;
                  Sbar[n][m] = (Real)Snm;
               }
         }
      }
   }

   fileDeg = fileDegree;
   fileOrd = fileOrder;
   // make sure mu and a are in KM and Km^3/sec^2 (they are in meters on the files)
   return true;  
}

//------------------------------------------------------------------------------
//  bool ReadGrvFile(Integer& fileDeg, Integer& fileOrd)
//------------------------------------------------------------------------------
/**
 * This method reads a ???????? file to get the gravity parameters.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::ReadGrvFile(Integer& fileDeg, Integer& fileOrd)
{
   Integer       n, m;
   Integer       fileOrder, fileDegree;
   Real          Cnm=0.0, Snm=0.0; // , dCnm=0.0, dSnm=0.0;
   Real          tmpMu = 0.0;
   Real          tmpA  = 0.0;
   std::string   isNormalized;

   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("Entered GravityField::ReadGrvFile\n");
   #endif

   std::ifstream inFile;
   inFile.open(filename.c_str());
   if (!inFile)
   {
      //throw ForceModelException("Cannot open file " + filename);
      //MessageInterface::ShowMessage("Cannot open file %s \n", filename);
      return false;
   }

   PrepareArrays();

   std::string s;
   std::string firstStr;
   while (!inFile.eof())
   {
      getline(inFile,s);
      std::istringstream lineStr;
      lineStr.str(s);
      // ignore comment lines
      if (s[0] != '#')
      {
         lineStr >> firstStr;
         if (firstStr == "END") break;

         // ignore the stk version and blank lines
         if ((strcasecmp(firstStr.c_str(),"Model") == 0) ||
               (strcasecmp(firstStr.c_str(),"BEGIN") == 0))
         {
            // do nothing - we don't need to know this
         }
         else if (strcasecmp(firstStr.c_str(),"Degree") == 0)
         {
            lineStr >> fileDegree;
         }
         else if (strcasecmp(firstStr.c_str(),"Order") == 0)
         {
            lineStr >> fileOrder;
         }
         else if (strcasecmp(firstStr.c_str(),"Gm") == 0)
         {
            lineStr >> tmpMu;
            if (tmpMu == 0.0)
               mu = defaultMu;
            else
               mu = tmpMu / 1.0e09;     // -> Km^3/sec^2
         }
         else if (strcasecmp(firstStr.c_str(),"RefDistance") == 0)
         {
            lineStr >> tmpA;
            if (tmpA == 0.0)
               a = defaultA;
            else
               a = tmpA / 1000.0;  // -> Km
         }
         else if (strcasecmp(firstStr.c_str(),"Normalized") == 0)
         {
            lineStr >> isNormalized;
            if (isNormalized == "No")
               throw ForceModelException(
                     "File " + filename + " is not normalized.");
            }
         else
         {
            // DJC changes, 11/9/04
            // Ensure that m and n fall in the allowed ranges
            n = (Integer) atoi(firstStr.c_str());
            if ((n > 0) && (n < HF_MAX_DEGREE)) {
               lineStr >> m;
               if ((m > 0) && (m < n)) {            
                  lineStr >> Cnm >> Snm;
//                  if ( n <= HF_MAX_DEGREE && m <= HF_MAX_ORDER )
//                  {
                  Cbar[n][m] = (Real)Cnm;
                  Sbar[n][m] = (Real)Snm;
//                  }
               }
            }
         }
      }
   }

   fileDeg = fileDegree;
   fileOrd = fileOrder;

   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("Leaving GravityField::ReadGrvFile\n");
      MessageInterface::ShowMessage("   dCbar[2][0] = %le   dSbar[2][0] = %le   \n",
                                    dCbar[2][0], dSbar[2][0]);
   #endif

   return true;  
}

//------------------------------------------------------------------------------
//  bool ReadDatFile(Integer& fileDeg, Integer& fileOrd)
//------------------------------------------------------------------------------
/**
 * This method reads a ???????? file to get the gravity parameters.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::ReadDatFile(Integer& fileDeg, Integer& fileOrd)
{
   Integer      cc, dd, sz=0;
   Integer      iscomment, rtn;
   Integer      n=0, m=0;
   Integer      fileDegree, fileOrder;
   Real         Cnm=0.0, Snm=0.0, dCnm=0.0, dSnm=0.0;
   char         buf[CelestialBody::BUFSIZE];
   FILE        *fp;

   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("Entered GravityField::ReadDatFile\n");
   #endif

   for (cc = 2;cc <= HF_MAX_DEGREE; ++cc)
   {
      for (dd = 0; dd <= cc; ++dd)
      {
         sz++;
      }
   }
   
   /* read coefficients from file */
   fp = fopen( filename.c_str(), "r");
   if (!fp){
      //MessageInterface::ShowMessage("Cannot open file %s \n", filename);
//      gfInitialized = false;  // ???????????????????????????????????
      return false;
   }

   PrepareArrays();

   iscomment = 1;
   while ( iscomment )
   {
      rtn = fgetc( fp );
      if ( (char)rtn == '#' )
      {
         fgets( buf, CelestialBody::BUFSIZE, fp );
      }
      else
      {
         ungetc( rtn, fp );
         iscomment = 0;
      }
   }

   fscanf(fp, "%lg\n", &mu ); mu = (Real)mu / 1.0e09;      // -> Km^3/sec^2
   fscanf(fp, "%lg\n", &a ); a = (Real)a / 1000.0;         // -> Km
   fgets( buf, CelestialBody::BUFSIZE, fp );
   while ( ( (char)(rtn=fgetc(fp)) != '#' ) && (rtn != EOF) )
   {
      ungetc( rtn, fp );
      fscanf( fp, "%i %i %le %le\n", &n, &m, &dCnm, &dSnm );
      if ( n <= GRAV_MAX_DRIFT_DEGREE  && m <= n )
      {
         dCbar[n][m] = (Real)dCnm;
         dSbar[n][m] = (Real)dSnm;
      }
   }

   fgets( buf, CelestialBody::BUFSIZE, fp );

   fileDegree = 0;
   fileOrder  = 0;
   cc=0;n=0;m=0;
   do
   {
      if ( n <= HF_MAX_DEGREE && m <= HF_MAX_ORDER )
      {
         Cbar[n][m] = (Real)Cnm;
         Sbar[n][m] = (Real)Snm;
      }
      if (n > fileDegree) fileDegree = n;
      if (n > fileOrder)  fileOrder  = n;
      
      cc++;
   } while ( ( cc<=sz ) && ( fscanf( fp, "%i %i %le %le\n", &n, &m, &Cnm, &Snm ) > 0 ));

   fileDeg = fileDegree;
   fileOrd = fileOrder;
   return true;
}


//------------------------------------------------------------------------------
//  void PrepareArrays()
//------------------------------------------------------------------------------
/**
 * This method zeros out the gravity field arrays prior to reading a new file.
 * 
 * @note: If Cbar, Sbar and their derivative arrays are made dynamic, array
 *        allocation and deallocation should be added to this method rather than
 *        scattered in the code above.
 */
//------------------------------------------------------------------------------
void GravityField::PrepareArrays()
{
   Integer m, n;
   
   for (n=0; n <= HF_MAX_DEGREE; ++n)
      for ( m=0; m <= HF_MAX_ORDER; ++m)
      {
         Cbar[n][m] = 0.0;
         Sbar[n][m] = 0.0;
      }
      
   for (n = 0; n <= GRAV_MAX_DRIFT_DEGREE; ++n) {
      for (m = 0; m <= GRAV_MAX_DRIFT_DEGREE; ++m) {
         dCbar[n][m] = 0.0;
         dSbar[n][m] = 0.0;
      }
   }   
}


//------------------------------------------------------------------------------
//  bool IsBlank(char* aLine)
//------------------------------------------------------------------------------
/**
 * This method returns true if the string is empty or is all white space.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::IsBlank(char* aLine)
{
   Integer i;
   for (i=0;i<(int)strlen(aLine);i++)
   {
      //loj: 5/18/04 if (!isblank(aLine[i])) return false;
      if (!isspace(aLine[i])) return false;
   }
   return true;
}


