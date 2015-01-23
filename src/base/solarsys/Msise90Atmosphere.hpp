//$Id$
//------------------------------------------------------------------------------
//                              Msise90Atmosphere
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
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
   Integer                 mass;
};

#endif // Msise90Atmosphere_hpp
