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
#include "CoordinateConverter.hpp"
#include "StringUtil.hpp"
#include "GravityFile.hpp"
#include "FileManager.hpp"
//#include "SolarSystemException.hpp"


//#define DEBUG_GRAVITY_FIELD
//#define DEBUG_GRAVITY_FIELD_DETAILS
//#define DEBUG_FIRST_CALL
//#define DEBUG_GRAV_COORD_SYSTEM
//#define DEBUG_BODY_STATE
//#define DEBUG_GRAVITY_FILE_READ

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

#ifdef DEBUG_FIRST_CALL
static bool firstCallFired = false;
#endif

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
 * @param <deg>  maximum degree of the polynomials.
 * @param <ord>  maximum order of the polynomials.
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
    gfInitialized   (false),
    sum2Diag        (NULL),
    sum3Diag        (NULL),
    sum2OffDiag     (NULL),
    sum3OffDiag     (NULL)
    
{
   objectTypeNames.push_back("GravityField");
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
GravityField::~GravityField()
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

   if (sum2Diag)
      delete [] sum2Diag; 

   if (sum3Diag)
      delete [] sum3Diag; 

   if (sum2OffDiag)
   {
      for (Integer i = 0; i <= degree; ++i)  
         delete [] sum2OffDiag[i];
      delete [] sum2OffDiag;
   }
    
   if (sum3OffDiag)
   {
      for (Integer i = 0; i <= degree; ++i)  
         delete [] sum3OffDiag[i];
      delete [] sum3OffDiag;
   }
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
    HarmonicField          (gf),
    mu                     (gf.mu),
    a                      (gf.a),
    defaultMu              (gf.defaultMu),
    defaultA               (gf.defaultA),
    gfInitialized          (false),
    orderTruncateReported  (gf.orderTruncateReported),
    degreeTruncateReported (gf.degreeTruncateReported),
    frv                    (gf.frv),
    trv                    (gf.trv),
    now                    (gf.now),
    satcount               (gf.satcount),
    sum2Diag               (NULL),
    sum3Diag               (NULL),
    sum2OffDiag            (NULL),
    sum3OffDiag            (NULL),
    cc                     (gf.cc),
    rotMatrix              (gf.rotMatrix),
    outState               (gf.outState),
    theState               (gf.theState)
{
   objectTypeNames.push_back("GravityField");
   bodyName = gf.bodyName;
   
   for (Integer i = 0; i < 361; i++)
   {
      for (Integer j = 0; j < 361; j++)
      {
         Cbar[i][j] = gf.Cbar[i][j];
         Sbar[i][j] = gf.Sbar[i][j];
      }
   }
   
   for (Integer i = 0; i < 17; i++)
   {
      for (Integer j = 0; j < 17; j++)
      {
         dCbar[i][j] = gf.dCbar[i][j];
         dSbar[i][j] = gf.dSbar[i][j];
      }
   }
  
   parameterCount = GravityFieldParamCount;
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
   mu                     = gf.mu;
   a                      = gf.a;
   defaultMu              = gf.defaultMu;
   defaultA               = gf.defaultA;
   bodyName               = gf.bodyName;
   gfInitialized          = false;  // is that what I want to do?
   orderTruncateReported  = gf.orderTruncateReported;
   degreeTruncateReported = gf.degreeTruncateReported;
   frv                    = gf.frv;
   trv                    = gf.trv;
   now                    = gf.now;
   satcount               = gf.satcount;
   sum2Diag               = NULL;
   sum3Diag               = NULL;
   sum2OffDiag            = NULL;
   sum3OffDiag            = NULL;
   cc                     = gf.cc;
   rotMatrix              = gf.rotMatrix;
   outState               = gf.outState;
   theState               = gf.theState;
   
   for (Integer i = 0; i < 361; i++)
   {
      for (Integer j = 0; j < 361; j++)
      {
         Cbar[i][j] = gf.Cbar[i][j];
         Sbar[i][j] = gf.Sbar[i][j];
      }
   }
   
   for (Integer i = 0; i < 17; i++)
   {
      for (Integer j = 0; j < 17; j++)
      {
         dCbar[i][j] = gf.dCbar[i][j];
         dSbar[i][j] = gf.dSbar[i][j];
      }
   }
   
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
   //if (gfInitialized && hMinitialized) return true;
   if (!HarmonicField::Initialize())
   {
      throw ForceModelException(
            "GravityField: Legendre Polynomial initialization failed!");
   }
    
   degreeTruncateReported = false;
   orderTruncateReported  = false;

   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif

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
   Real               p, summ[4] = {0.0, 0.0, 0.0, 0.0}, D, dT, Cbar_nm, Sbar_nm;

   Real               arr[4] = {0.0, 0.0, 0.0, 0.0};
   static const Real  sqrt2 = Sqrt(2.0);

   dT = (jday - 2.4464305e+06)/365.25; /* years since Jan 1, 1986 */

   if (r == 0.0)  // (should I) check for hFinitialized here?
   {
      throw ForceModelException("gravity_rtq: Spherical representation not initialized!");
   }

   Real aOverR = a/r;
   p = (aOverR)*mu/r; /* Ref.[2], Eq.(26), n = 1 */
   arr[3] = -mu/r/r;
   p *= aOverR;

   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("  Calculated p = %le\n", p);
      MessageInterface::ShowMessage("  Iterating over degree = %d, order = %d\n",
         degree, order);
   #endif

   Integer np1;
   Real pOverA, aBarMsqrt2;
//   for (n=2;n<=min(G->degree,LP->degree);n++) {
   for (n = 2; n <= degree; ++n)
   {
      np1 = n+1;
      p *= aOverR; /* Ref.[2], Eq.(26), rho_n+1 */

//      for (m = 0;( m <= n && m <= min(G->order,LP->order) );m++) {
      for (m = 0;(m <= n && m <= order); ++m)
      {
         #ifdef DEBUG_GRAVITY_FIELD_DETAILS
            //dT = (jday - (Real)2.4464305e+06)/(Real)365.25; /* years since Jan 1, 1986 */
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
            //dT = (jday - (Real)2.4464305e+06)/(Real)365.25; /* years since Jan 1, 1986 */
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
            summ[2] = sum2Diag[n]*Abar[n][1]*Cbar_nm;
            summ[3] = sum3Diag[n]*Abar[np1][1]*Cbar_nm;
//            summ[2] = Sqrt( (Real)( n*(np1) ) )*Abar[n][1]*Cbar_nm;
//            summ[3] = Sqrt( (Real)( (2*n+1)*(n+2)*(np1) )/
//                            (Real)( 2*n+3 ) )*Abar[np1][1]*Cbar_nm;
         }
         else
         {
            aBarMsqrt2 = Abar[n][m]*m*sqrt2;
            summ[0] += aBarMsqrt2*( Cbar_nm*re[m-1] + Sbar_nm*im[m-1] );
            summ[1] += aBarMsqrt2*( Sbar_nm*re[m-1] - Cbar_nm*im[m-1] );
            D = Cbar_nm*re[m] + Sbar_nm*im[m];
            summ[2] += sum2OffDiag[n][m]*Abar[n][m+1]*D;
            summ[3] += sum3OffDiag[n][m]*Abar[np1][m+1]*D;
//            D = (Cbar_nm*re[m] + Sbar_nm*im[m])*sqrt2;
//            summ[2] += Sqrt( (Real)((n-m)*(np1+m)) )*Abar[n][m+1]*D;
//            summ[3] += Sqrt( (Real)( (2*n+1)*(n+m+2)*(np1+m) )/
//                             (Real)( 2*n+3 ) )*Abar[np1][m+1]*D;
         }
      } /* end m summation */
      
      #ifdef DEBUG_GRAVITY_FIELD
         MessageInterface::ShowMessage("  Calculated summ = %le  %le  %le  %le\n",
            summ[0], summ[1], summ[2], summ[3]);
      #endif

      pOverA = p/a;
      arr[0] += pOverA*summ[0];
      arr[1] += pOverA*summ[1];
      arr[2] += pOverA*summ[2];
      arr[3] -= pOverA*summ[3];

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
   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
         MessageInterface::ShowMessage(
            "GravityField(%s) inputs:\n"
            "   state = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n"
            "   dt = %.10lf\n   dvorder = %d\n",
            instanceName.c_str(), state[0], state[1], state[2], state[3], 
            state[4], state[5], dt, dvorder);
      }
   #endif

   if ((dvorder > 2) || (dvorder < 1))
      return false;
      
   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("%s %d %s %le %s  %le %le %le %le %le %le\n",
          "Entered GravityField::GetDerivatives with order", dvorder, "dt = ",
          dt, "and state\n",
          state[0], state[1], state[2], state[3], state[4], state[5]);
   #endif
      
/// @todo Optimize this code -- May be possible to make GravityField calcs more efficient

   //Integer stateSize = 6;
   satcount = dimension / stateSize; 
   
   if (stateSize * satcount != dimension)
      throw ForceModelException(
         "GravityField state dimension and state size do not match!");

   // Currently hardcoded for one spacecraft.  - remove this!!!!!!!!!!!!!!
   if (satcount < 1)
      throw ForceModelException(
         "GravityField requires at least one spacecraft.");
      
   //Real* satState;
   Real satState[6];
   Real f[3]; //, rbb3, mu_rbb;
   Real aIndirect[3] = {0.0,0.0,0.0};
   Integer nOffset;
   bool sameCS = false;
   
   now = epoch + dt/GmatTimeUtil::SECS_PER_DAY;
   //const Rvector6 *rv = &(body->GetState(now));
   //if (body->GetName() != fixedCS->GetOriginName())
   //   throw ForceModelException(
   //         "Incorrect central body for Body Fixed coordinate system in "
   //         + instanceName);
   if (targetCS == inputCS)   sameCS = true;

   #ifdef DEBUG_GRAV_COORD_SYSTEM
       MessageInterface::ShowMessage(
         "------ body = %s\n------ inputCS = %s\n------ targetCS = %s"
         "\n------ fixedCS = %s\n", 
         body->GetName().c_str(),     inputCS->GetName().c_str(), 
         targetCS->GetName().c_str(), fixedCS->GetName().c_str());
   #endif
   
   
//   CelestialBody *targetBody = (CelestialBody*) targetCS->GetOrigin();
//   CelestialBody *fixedBody  = (CelestialBody*) fixedCS->GetOrigin();
   //SpacePoint *targetBody = targetCS->GetOrigin();
   //SpacePoint *fixedBody = fixedCS->GetOrigin();
   //const Rvector6 rv = fixedBody->GetMJ2000State(now) - 
   //   targetBody->GetMJ2000State(now);
   //const Rvector6 rv = fixedBody->GetState(now) - 
   //   targetBody->GetState(now);
   //Rvector frv = ((CelestialBody*) fixedBody)->GetState(now);
   //Rvector trv = ((CelestialBody*) targetBody)->GetState(now);

   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
    	 CelestialBody *targetBody = (CelestialBody*) targetCS->GetOrigin();
    	 CelestialBody *fixedBody  = (CelestialBody*) fixedCS->GetOrigin();
         MessageInterface::ShowMessage(
            "   Epoch = %.12lf\n   targetBody = %s\n   fixedBody = %s\n", 
            now.Get(), targetBody->GetName().c_str(), 
            fixedBody->GetName().c_str());
         MessageInterface::ShowMessage(
            "------ body = %s\n------ inputCS = %s\n------ targetCS = %s\n"
            "------ fixedCS = %s\n", 
            body->GetName().c_str(),     inputCS->GetName().c_str(), 
            targetCS->GetName().c_str(), fixedCS->GetName().c_str());
      }
   #endif

// The following code is a start on full field modeling at non-central bodies.  
// It is not currently correct, and therefore disabled for the May 2007 beta.
//   if (body->GetName() != targetCS->GetOriginName())  // or fixed?????
//   //if (rbb3 != 0.0) 
//   {
//      frv = fixedBody->GetState(now);
//      trv = targetBody->GetState(now);
//      
//      const Real *rvf = frv.GetDataVector();
//      const Real *rvt = trv.GetDataVector();
//      
//      //Real rvf[6];
//      //Real rvt[6];
//      
//      #ifdef DEBUG_BODY_STATE
//         MessageInterface::ShowMessage("*** About to call GetState ....\n");
//      #endif
//      //fixedBody->GetState(now, rvf);
//      //targetBody->GetState(now, rvt);
//      #ifdef DEBUG_BODY_STATE
//         MessageInterface::ShowMessage("*** DID call GetState ....\n");
//      #endif
//      
//      Real rv[3];
//      rv[0] = rvf[0] - rvt[0];
//      rv[1] = rvf[1] - rvt[1];
//      rv[2] = rvf[2] - rvt[2];
//      
//      // Precalculations for the indirect effect term
//      //rbb3 = (*rv)[0] * (*rv)[0] + (*rv)[1] * (*rv)[1] + (*rv)[2] * (*rv)[2];
//      rbb3 = (rv[0] * rv[0]) + (rv[1] * rv[1]) + (rv[2] * rv[2]);
//      if (GmatMathUtil::Abs(rbb3) < 1.0e-14)
//         aIndirect[0] = aIndirect[1] = aIndirect[2] = 0.0;
//      else
//      {
//         rbb3 = rbb3 * sqrt(rbb3);
//         mu_rbb = mu / rbb3;
//         //aIndirect[0] = mu_rbb * (*rv)[0];
//         //aIndirect[1] = mu_rbb * (*rv)[1];
//         //aIndirect[2] = mu_rbb * (*rv)[2];
//         aIndirect[0] = mu_rbb * rv[0];
//         aIndirect[1] = mu_rbb * rv[1];
//         aIndirect[2] = mu_rbb * rv[2];
//      }
//      #ifdef DEBUG_GRAVITY_FIELD
//         MessageInterface::ShowMessage(
//            "Indirect calcs for body %s, target %s, and fixed  %s\n"
//            "   rv = [%lf %lf %lf]\n",
//            body->GetName().c_str(), targetCS->GetOriginName().c_str(), 
//            fixedCS->GetOriginName().c_str(), 
//            rv[0], rv[1], rv[2]);
//      #endif
//   }
//   else
      aIndirect[0] = aIndirect[1] = aIndirect[2] = 0.0;

   for (Integer n = 0; n < satcount; ++n) 
   {
      nOffset = n * stateSize;
      //satState = state + nOffset;
      for (Integer i=0;i<6;i++) satState[i] = state[i+nOffset];
      
      // convert to body fixed coordinate system
      //A1Mjd               a1Now(now);
      Real                tmpState[6];
      //theState.Set(satState);
      //cc.Convert(a1Now, theState, inputCS, outState, fixedCS);
      //cc.Convert(now, theState, inputCS, outState, fixedCS);
      cc.Convert(now, satState, inputCS, tmpState, fixedCS);

      #ifdef DEBUG_FIRST_CALL
         if (firstCallFired == false)
         {
            MessageInterface::ShowMessage(
               "   State conversion check:\n      %s --> %s\n", 
               inputCS->GetName().c_str(), fixedCS->GetName().c_str());
            MessageInterface::ShowMessage(
               "      Epoch: %.12lf\n", now.Get());
            MessageInterface::ShowMessage(
               "      input State  = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
               theState[0], theState[1], theState[2], theState[3], theState[4], 
               theState[5]);
            MessageInterface::ShowMessage(
               "      output State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
               outState[0], outState[1], outState[2], outState[3], outState[4], 
               outState[5]);
         }
      #endif

      if (sameCS) rotMatrix = cc.GetLastRotationMatrix();
      //outState.GetR(tmpState);
      //outState.GetV(tmpState + 3);

      #ifdef DEBUG_FIRST_CALL
         if (firstCallFired == false)
         {
            MessageInterface::ShowMessage(
               "   tmpState = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
               tmpState[0], tmpState[1], tmpState[2], tmpState[3], tmpState[4], 
               tmpState[5]);
         }
      #endif

      //if (!legendreP_rtq(satState))
      if (!legendreP_rtq(tmpState))
         throw ForceModelException("GravityField::legendreP_rtq failed");

      #ifdef DEBUG_GRAVITY_FIELD
         MessageInterface::ShowMessage("%s%le  s = %le  t = %le  u = %le\n", 
            "legendreP_rtq r = ", r, s, t, u);
         MessageInterface::ShowMessage("%s%le\n", 
                           "Calling gravity_rtq for Julian epoch = ", 
                           epoch + GmatTimeUtil::JD_JAN_5_1941 
                           + dt/GmatTimeUtil::SECS_PER_DAY);
      #endif

      if (!gravity_rtq(epoch + GmatTimeUtil::JD_JAN_5_1941 + 
                       dt/GmatTimeUtil::SECS_PER_DAY, f))
         throw ForceModelException("GravityField::gravity_rtq failed");

      #ifdef DEBUG_GRAVITY_FIELD
         MessageInterface::ShowMessage("%s %le  %le  %le\n", 
            "gravity_rtq returned force = ", f[0], f[1], f[2]);
         MessageInterface::ShowMessage("%s %le  %le  %le\n", 
            "Indirect term is", aIndirect[0], aIndirect[1], aIndirect[2]);
      #endif

      // Convert back to target CS
      Real fNew[3];
      if (sameCS)
      {
         
         const Real *rm = rotMatrix.GetDataVector();
         const Real  rmt[9] = {rm[0], rm[3], rm[6],
                               rm[1], rm[4], rm[7],
                               rm[2], rm[5], rm[8]};
         Integer p3 = 0;
         for (Integer p = 0; p < 3; ++p)
         {
            p3 = 3*p;
            fNew[p] = rmt[p3]   * f[0] + 
                      rmt[p3+1] * f[1] + 
                      rmt[p3+2] * f[2];
        }  
        
          /*
         Rvector3 fAccel(f[0], f[1], f[2]);
         Rvector3 fNewVector = rotMatrix.Transpose() * fAccel;
         fNew[0] = fNewVector[0];
         fNew[1] = fNewVector[1];
         fNew[2] = fNewVector[2];
         */
      }
      else
      {
         Rvector6 fState(f[0], f[1], f[2], 0.0, 0.0, 0.0);
         Rvector6 fConv;
         //cc.Convert(a1Now, fState, fixedCS, fConv, targetCS, true); 
         cc.Convert(now, fState, fixedCS, fConv, targetCS, true); 
         fNew[0] = fConv[0];
         fNew[1] = fConv[1];
         fNew[2] = fConv[2];
      }
      
      #ifdef DEBUG_GRAVITY_FIELD
         MessageInterface::ShowMessage("%s %le  %le  %le\n", 
                 "converted force = ", fNew[0], fNew[1], fNew[2]);
      #endif
   
      switch (dvorder)
      {
         case 1:
            deriv[0+nOffset] = satState[3];
            deriv[1+nOffset] = satState[4];
            deriv[2+nOffset] = satState[5];
            deriv[3+nOffset] = fNew[0] - aIndirect[0];
            deriv[4+nOffset] = fNew[1] - aIndirect[1];
            deriv[5+nOffset] = fNew[2] - aIndirect[2];
            break;
   
         case 2:
            deriv[0+nOffset] = fNew[0] - aIndirect[0];
            deriv[1+nOffset] = fNew[1] - aIndirect[1];
            deriv[2+nOffset] = fNew[2] - aIndirect[2];
            deriv[3+nOffset] =
               deriv[4+nOffset] =
               deriv[5+nOffset] = 0.0;
            break;
   
         default:
            throw ForceModelException("GravityField::GetDerivatives requires order = 1 or 2");
      } // end switch
   }  // end for

   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
         MessageInterface::ShowMessage(
            "   GravityField[%s <> %s] --> mu = %lf, origin = %s, [%.10lf %.10lf "
            "%.10lf %.16lf %.16lf %.16lf]\n",
            instanceName.c_str(), body->GetName().c_str(), mu, 
            targetCS->GetOriginName().c_str(), 
            deriv[0], deriv[1], deriv[2], deriv[3], deriv[4], deriv[5]);
         firstCallFired = true;
      }
   #endif

   return true;
}

//---------------------------------
// inherited methods from GmatBase
//---------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
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



// All read only for now except degree and order
bool GravityField::IsParameterReadOnly(const Integer id) const
{
   if (id < HarmonicFieldParamCount)
      return HarmonicField::IsParameterReadOnly(id);
      
   return true;
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

   if (body->GetName() != targetCS->GetOriginName())
      throw ForceModelException("Full field gravity is only supported for "
         "the force model origin in current GMAT builds.");


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
         PrepareArrays(); // reset the arrays first
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
   
   if (sum2Diag) 
   {
      delete [] sum2Diag; 
      sum2Diag = NULL;
   }

   if (sum3Diag)
   {
      delete [] sum3Diag; 
      sum3Diag = NULL;
   }

   if (sum2OffDiag)
   {
      for (Integer i = 0; i <= degree; ++i)  
         delete [] sum2OffDiag[i];
      delete [] sum2OffDiag;
      sum2OffDiag = NULL;
   }
    
   if (sum3OffDiag)
   {
      for (Integer i = 0; i <= degree; ++i)  
         delete [] sum3OffDiag[i];
      delete [] sum3OffDiag;
      sum3OffDiag = NULL;
   }
      
   sum2Diag = new Real[degree+1];
   sum3Diag = new Real[degree+1];
   sum2OffDiag = new Real*[degree+1];
   sum3OffDiag = new Real*[degree+1];
   
   for (Integer i = 0; i <= degree; ++i)  
   {
      sum2OffDiag[i] = new Real[order+1];
      sum3OffDiag[i] = new Real[order+1];

      sum2Diag[i] = Sqrt((Real)(i*(i+1)));
      sum3Diag[i] = Sqrt(((Real)((2*i+1)*(i+2)*(i+1)))/((Real)(2*i+3)));
      for (Integer j = 0; j <= order; ++j)
      {
         if (j > i)
            continue;
         sum2OffDiag[i][j] = Sqrt(2.0*(i-j)*(i+1+j));
         sum3OffDiag[i][j] = Sqrt(2.0*(2*i+1)*(i+j+2)*(i+1+j)/(2*i+3));
      }
   }
   

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
   
   #ifdef DEBUG_GRAVITY_FILE_READ
   MessageInterface::ShowMessage
      ("%s \"%s\"\n", "GravityField::ReadFile() called for file", filename.c_str());
   #endif
   
   // Read gravity file
   try
   {
      GravityFile gf;
      if (!gf.ReadFile(filename, fileDegree, fileOrder, mu, a, true,
                       Cbar, Sbar, dCbar, dSbar, HF_MAX_DEGREE, HF_MAX_ORDER,
                       GRAV_MAX_DRIFT_DEGREE))
      {
         errMsg = "Error reading coefficients, mu, and equatorial radius from "
            + filename;
         isOk = false;
      }
   }
   catch (BaseException &e)
   {
      errMsg = e.GetFullMessage();
      isOk = false;
   }
   
   if (!isOk)
   {
      MessageInterface::ShowMessage(errMsg + "\n");
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


