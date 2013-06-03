//$Id$
//------------------------------------------------------------------------------
//                              GravityField
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: D. Conway
// Created: 2003/03/17
// Modified: 2004/04/15 W. Shoan GSFC Code 583
//           Updated for GMAT style, standards; changed quad-t to Real, int
//           to Integer, etc.
//           2011.05.20 W. Shoan - updates based on code from John P. Downing to add Earth tides
//
/**
 * This is the base class for celestial bodies.
 *
 * @note This is an abstract class.
 * @note original prolog information included at end of file prolog.
 */
//
// GMAT: General Mission Analysis Tool.
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
#include "HarmonicGravity.hpp"
#include "ODEModelException.hpp"


class GMAT_API GravityField : public HarmonicField
{
public:
   GravityField(const std::string &name, const std::string &forBodyName,
                Integer maxDeg = HarmonicField::HF_MAX_DEGREE,
                Integer maxOrd = HarmonicField::HF_MAX_ORDER);
   virtual ~GravityField();
   GravityField(const GravityField & gf);
   GravityField&   operator=(const GravityField & gf);

   virtual bool    Initialize();
   
   virtual bool    GetDerivatives(Real *state, Real dt = 0.0, 
                                  Integer order = 1, 
                                  const Integer id = -1);
   virtual Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc);

   virtual bool    GetBodyAndMu(std::string &itsName, Real &itsMu);

   // inherited from GmatBase
   virtual GmatBase* Clone(void) const;

   // override access methods derived from GmatBase
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
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id,
                                          const std::string &value);
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool        SetStringParameter(const std::string &label,
                                          const std::string &value);
   
   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index, 
                         Integer quantity);

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS

protected:

   enum
   {
      MU = HarmonicFieldParamCount,
      A,
      EARTH_TIDE_MODEL,
      GravityFieldParamCount
   };

   static std::vector<HarmonicGravity*> cache;

   static const std::string PARAMETER_TEXT[
      GravityFieldParamCount - HarmonicFieldParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[
      GravityFieldParamCount - HarmonicFieldParamCount];

   /// gravity parameter of central body
   Real               mu;
   /// radius of central body ( mean equatorial )
   Real               a;
   /// string for tide model
   std::string        earthTideModel;
   /// default mu
   Real               defaultMu;
   /// default equatorial radius
   Real               defaultA;
//   /// The minimum distance required from body origin for computation
//   Real               minimumDistance;
   ///
   bool               gfInitialized;
   /// Flag used to keep from scrolling the "truncating to order" message
   bool               orderTruncateReported;   // no longer needed - only truncated in Initialize
   /// Flag used to keep from scrolling the "truncating to degree" message
   bool               degreeTruncateReported;  // no longer needed - only truncated in Initialize
   
   HarmonicGravity    *gravityModel;    // JPD
   

   bool          IsBlank(char* aLine);  // leaving this one in for now
   bool          IsBlank(const std::string &aLine);  // leaving this one in for now

   static const Integer stateSize             = 6;
   
   // stuff moved in here for performance
   Rvector6 frv;
   Rvector6 trv;
   A1Mjd    now;
   
   CoordinateConverter cc;

   //  JPD added these ...............
   void Calculate (Real dt, Real state[6],
                   Real force[3], Rmatrix33& grad);
   void InverseRotate(Rmatrix33& rot, const Real in[3], Real out[3]);
   
   HarmonicGravity* GetGravityFile(const std::string &filename, const Real &radius, const Real &mukm);
};


#endif // GravityField_hpp
