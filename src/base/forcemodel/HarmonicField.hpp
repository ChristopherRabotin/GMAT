//$Id$
//------------------------------------------------------------------------------
//                              HarmonicField
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
// Created: 2003/03/14
// Modified: 2004/04/15 W. Shoan GSFC Code 583
//           Updated for GMAT style, standards; changed quad-t to Real, int
//           to Integer, add use of GMAT util classes, etc.
///
/**
 * This is the HarmonicField class.
 *
 * @note original prolog information included at end of file prolog.
 */
//
// *** File Name : shfieldmodel.h  -> HarmonicField.hpp  (wcs 2004.04.14)
// *** Created   : March 14, 2003
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
// Modification History      : 3/14/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
// **************************************************************************

#ifndef HarmonicField_hpp
#define HarmonicField_hpp

#include "ODEModelException.hpp"
#include "GravityBase.hpp"
#include "CelestialBody.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "CoordinateSystem.hpp"
#include "EopFile.hpp"


/**
 * Common header file definitions for execution of gravity, legendreP, and redux routines
 *
 * Steven Queen
 * Goddard Space Flight Center
 * Flight Dynamics Analysis Branch
 * Steven.Z.Queen@nasa.gov
 * Feburary 28, 2003
 *
 * Note that in GMAT, this code is derived from GravityBase rather than
 * PhysicalModel.  That means that is we add other HarmonicField models that are
 * not gravity fields, we'll need to do some code rearrangement to have the
 * common harmonic elements separate but usable.
 *
 * The GravityBase class was added in Oct 2012 to handle issues with different
 * types of sophisticated gravity models -- specifically, Polyhedral models and
 * Spherical Harmonic models -- in the ODEModel collection.
 */


class GMAT_API HarmonicField : public GravityBase
{
public:
    HarmonicField(const std::string &name,
          const std::string &typeName = "HarmonicField",
          Integer maxDeg = HF_MAX_DEGREE, Integer maxOrd = HF_MAX_ORDER);
    virtual ~HarmonicField(void);

    HarmonicField(const HarmonicField& hf);
    HarmonicField&          operator=(const HarmonicField& hf);

    virtual bool            Initialize();
    virtual bool            CheckQualifier(const std::string &qualifier,
                                           const std::string &forType = "");
    virtual bool            SetDegreeOrder(Integer degree, Integer order);
    virtual bool            SetFilename(const std::string &fn);
    virtual void            SetEopFile(EopFile *eopF);

   // inherited from GmatBase
    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer     GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType
       GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;

    virtual Integer     GetIntegerParameter(const Integer id) const;
    virtual Integer     SetIntegerParameter(const Integer id,
                                            const Integer value);
    virtual Integer     GetIntegerParameter(const std::string &label) const;
    virtual Integer     SetIntegerParameter(const std::string &label,
                                            const Integer value);
    virtual std::string GetStringParameter(const Integer id) const;
    virtual bool        SetStringParameter(const Integer id,
                                           const std::string &value);
    virtual std::string GetStringParameter(const std::string &label) const;
    virtual bool        SetStringParameter(const std::string &label,
                                           const std::string &value);
    virtual GmatBase*   GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
    const StringArray&  GetRefObjectNameArray(const Gmat::ObjectType type);
    virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
    virtual void        SetForceOrigin(CelestialBody* toBody);
                                     
    bool                IsParameterReadOnly(const Integer id) const;
    
    // constants defining maximum degree and order
    static const Integer HF_MAX_DEGREE = 360;
    static const Integer HF_MAX_ORDER  = 360;

protected:

   enum   // do we need the Abar, etc. to be in this list?
   {
      MAX_DEGREE = PhysicalModelParamCount,
      MAX_ORDER,
      DEGREE,
      ORDER,
      FILENAME,
      INPUT_COORD_SYSTEM,
      FIXED_COORD_SYSTEM,
      TARGET_COORD_SYSTEM,
      HarmonicFieldParamCount
   };

   static const std::string PARAMETER_TEXT[
      HarmonicFieldParamCount - PhysicalModelParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[
      HarmonicFieldParamCount - PhysicalModelParamCount];

   /// Flag indicating if the system has been initialized
   bool                    hMinitialized;  // inherit from PhysicalModel?
   /// Maximum degree for the field
   Integer                 maxDegree;
   /// Maximum order for the field
   Integer                 maxOrder;
   /// Current degree for the field
   Integer                 degree;
   /// Current order for the field
   Integer                 order;
   /// transformed spherical coordinates
   Real                    r,s,t,u;
   /// The name of the potential file
   std::string             filename;

   /// has the file been read?
   bool                    fileRead;
   bool                    usingDefaultFile;
   
   bool                    isFirstTimeDefault;


   std::string             inputCSName;
   std::string             fixedCSName;
   std::string             targetCSName;
   std::string             potPath;
   
   CoordinateSystem        *inputCS;
   CoordinateSystem        *fixedCS;
   CoordinateSystem        *targetCS;
   EopFile                 *eop;
   
};

#endif // HarmonicField_hpp
