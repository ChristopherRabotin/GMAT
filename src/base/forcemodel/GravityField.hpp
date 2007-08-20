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
//           Updated for GMAT style, standards; changed quad-t to Real, int
//           to Integer, etc.
//
/**
 * This is the base class for celestial bodies.
 *
 * @note This is an abstract class.
 * @note original prolog information included at end of file prolog.
 */
//
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
//           Updated for GMAT style, standards; changed quad-t to Real, int
//           to Integer, etc.
//
/**
 * This is the GravityField class.
 *
 * @note original prolog information included at end of file prolog.
 */
//
// *** File Name : val_gravityfield.h
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
//------------------------------------------------------------------------------

#ifndef GravityField_hpp   /* protect against multiple includes */
#define GravityField_hpp

#include "HarmonicField.hpp"
#include "CelestialBody.hpp"


class GMAT_API GravityField : public HarmonicField
{
public:
   GravityField(const std::string &name, const std::string &forBodyName,
                Integer maxDeg = HarmonicField::HF_MAX_DEGREE,
                Integer maxOrd = HarmonicField::HF_MAX_ORDER);
   virtual ~GravityField(void);
   GravityField(const GravityField & gf);
   GravityField&   operator=(const GravityField & gf);

   virtual bool    Initialize(void);
   bool            gravity_rtq(Real jday, Real F[]);
   
   virtual bool    GetDerivatives(Real * state, Real dt = 0.0, 
                                                Integer order = 1);

   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

   // override acces methods derived from GmatBase
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual bool        IsParameterReadOnly(const Integer id) const;

   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
   virtual Real        GetRealParameter(const std::string &label) const;
   virtual Real        SetRealParameter(const std::string &label,
                                        const Real value);


protected:

   enum   // do we need the Abar, etc. to be in this list?
   {
      MU = HarmonicFieldParamCount,
      A,
      GravityFieldParamCount
   };

   static const std::string PARAMETER_TEXT[
      GravityFieldParamCount - HarmonicFieldParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[
      GravityFieldParamCount - HarmonicFieldParamCount];

   /// gravity parameter of central body
   Real               mu;
   /// radius of central body ( mean equatorial )
   Real               a;
   /// default mu
   Real               defaultMu;
   /// default equatorial radius
   Real               defaultA;
   /// normalized harmonic coefficients
   Real               Cbar[361][361];
   /// normalized harmonic coefficients
   Real               Sbar[361][361];
   /// coefficient drifts per year
   Real               dCbar[17][17];
   /// coefficient drifts per year
   Real               dSbar[17][17];
   ///
   bool               gfInitialized;
   /// Flag used to keep from scrolling the "truncating to order" message
   bool               orderTruncateReported;
   /// Flag used to keep from scrolling the "truncating to degree" message
   bool               degreeTruncateReported;
   
   bool          gravity_init(void);

   bool          ReadFile();
   
   void          PrepareArrays();

   bool          IsBlank(char* aLine);

   static const Integer GRAV_MAX_DRIFT_DEGREE = 2;
   static const Integer BUFSIZE               = 256;
   static const Integer stateSize             = 6;
   
   // stuff moved in here for performance
   Rvector6 frv;
   Rvector6 trv;
   A1Mjd    now;
   Integer satcount;
   
   Real     *sum2Diag;
   Real     *sum3Diag;
   Real     **sum2OffDiag;
   Real     **sum3OffDiag;
   
   CoordinateConverter cc;
   Rmatrix33           rotMatrix;
   Rvector6            outState;
   Rvector6            theState;
};


#endif // GravityField_hpp
