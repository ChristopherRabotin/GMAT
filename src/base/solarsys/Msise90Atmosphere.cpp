//$Header$
//------------------------------------------------------------------------------
//                              Msise90Atmosphere
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/21
//
/**
 * The MSISE90 atmosphere
 */
//------------------------------------------------------------------------------


#include "Msise90Atmosphere.hpp"


Msise90Atmosphere::Msise90Atmosphere() :
    AtmosphereModel     ("MSISE90")
{
}


Msise90Atmosphere::~Msise90Atmosphere()
{
}


//Msise90Atmosphere::Msise90Atmosphere(const Msise90Atmosphere& msise) :
//    AtmosphereModel     (msise)
//{
//}
//
//
//Msise90Atmosphere& Msise90Atmosphere::operator=(const Msise90Atmosphere& msise)
//{
//    if (this == &msise)
//        return *this;
//        
//    return *this;
//}
    

bool Msise90Atmosphere::Density(Real *position, Real *density, Integer count)
{
    return false;
}

