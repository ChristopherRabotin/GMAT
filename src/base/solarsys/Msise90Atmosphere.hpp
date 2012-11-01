//$Id$
//------------------------------------------------------------------------------
//                              Msise90Atmosphere
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/21
//
/**
 * The MSISE90 atmosphere
 *
 * Wraps the MSISE90 atmosphere code, ported by a.i. Solutions from the FORTRAN,
 * into the GMAT atmosphere model format.
 */
//------------------------------------------------------------------------------

#ifndef Msise90Atmosphere_hpp
#define Msise90Atmosphere_hpp

#include "AtmosphereModel.hpp"
#include "TimeTypes.hpp"
 
class GMAT_API Msise90Atmosphere : public AtmosphereModel
{
public:
   Msise90Atmosphere(const std::string &name = "");
   virtual ~Msise90Atmosphere();
   Msise90Atmosphere(const Msise90Atmosphere& msise);
   Msise90Atmosphere&      operator=(const Msise90Atmosphere& msise);
    
   bool                    Density(Real *position, Real *density,
                                   Real epoch = GmatTimeConstants::MJD_OF_J2000,
                                   Integer count = 1);

   virtual GmatBase*       Clone() const; // inherited from GmatBase
protected:
   /// Flag to indicate if data comes from a file
   bool                    fileData;
   /// Name of the file
   std::string             fluxfilename;
   /// Second of day
   Real                    sod;
   /// Year + Day of year, in the form YYYYDDD
   Integer                 yd;
   /// Value of F10.7 to use
   Real                    f107;
   /// 3 month average of the F10.7 data
   Real                    f107a;
   /// Geomagnetic index (Ap, not Kp)
   Real                    ap[7];
   
   Integer                 mass;

   void                    GetInputs(Real epoch);
};

#endif // Msise90Atmosphere_hpp
