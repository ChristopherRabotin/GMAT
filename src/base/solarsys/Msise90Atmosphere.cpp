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
#include <math.h>


Msise90Atmosphere::Msise90Atmosphere() :
    AtmosphereModel     ("MSISE90")
{
}


Msise90Atmosphere::~Msise90Atmosphere()
{
}


bool Msise90Atmosphere::Density(Real *pos, Real *density, Real epoch, 
                                Integer count)
{
    Integer i, i6;
    Real    alt;
  
    // For now, hard code the MSISE90 parameters.
    Real    f107  =   150.0;
    Real    f107a =   150.0;
    Integer doy   =   172;
    Real    sod   = 29000.0;
    Integer year  =  2003;
    Real    lat   =    60.0;
    Real    lon   =   -70.0; 
    Real    lst   =    16.0;
    
    Integer yd = year * 1000 + doy;
    
    Real    ap[7], den[8], temp[2];
    for (i = 0; i < 7; i++)
        ap[i] = 4.0;
    
    for (i = 0; i < count; ++i) {
        i6 = i*6;
        alt = sqrt(pos[ i6 ]*pos[ i6 ] + 
                   pos[i6+1]*pos[i6+1] + 
                   pos[i6+2]*pos[i6+2]) - 6378.14;
        msise90.GTD6(yd,sod,alt,lat,lon,lst,f107a,f107,ap,48,den,temp);
        density[i] = den[5];
    }
    
    return true;
}

