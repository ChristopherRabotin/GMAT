//$Header$
//------------------------------------------------------------------------------
//                           ExponentialAtmosphere
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
 * An exponentially modeled atmosphere
 */
//------------------------------------------------------------------------------


#include "ExponentialAtmosphere.hpp"
#include <math.h>

ExponentialAtmosphere::ExponentialAtmosphere() :
    AtmosphereModel     ("Exponential")
{
    SetConstants();
}


ExponentialAtmosphere::~ExponentialAtmosphere()
{
}


//ExponentialAtmosphere::ExponentialAtmosphere(const ExponentialAtmosphere& atm) :
//    AtmosphereModel(atm)
//{
//    SetConstants();
//}
//
//
//ExponentialAtmosphere& ExponentialAtmosphere::operator=(const ExponentialAtmosphere& atm)
//{
//    if (this == &atm)
//        return *this;
//        
//    return *this;
//}
  
    
bool ExponentialAtmosphere::Density(Real *position, Real *density, Integer count)
{
    if (sunVector == NULL)
        throw AtmosphereException("Sun vector was not initialized");
        
    if (centralBodyLocation == NULL)
        throw AtmosphereException("Central body vector was not initialized");
    
    Real psi, ra, dec, sunRa, sunDec, mag, sv[3], loc[3], height, sz;
    Integer i, index;
    
    sv[0] = sunVector[0] - centralBodyLocation[0];
    sv[1] = sunVector[1] - centralBodyLocation[1];
    sv[2] = sunVector[2] - centralBodyLocation[2];
    
    // Update the sun vector
    mag = sqrt(sv[0]*sv[0] + sv[1]*sv[1] + sv[2]*sv[2]);
               
    if (mag == 0.0)
        throw AtmosphereException("Sun vector has zero magnitude");
    
    sunRa  = atan2(sv[1], sv[0]);
    sunDec = asin(sv[2] / mag);
    
    
    for (i = 0; i < count; ++i) {
        loc[0] = position[ i*6 ] - centralBodyLocation[0];
        loc[1] = position[i*6+1] - centralBodyLocation[1];
        loc[2] = position[i*6+2] - centralBodyLocation[2];
        
        mag = sqrt(loc[0]*loc[0] + loc[1]*loc[1] + loc[2]*loc[2]);
        height = mag - cbRadius;
        if (height <= 0.0)
            throw AtmosphereException("Position vector is inside central body");
            
        ra  = atan2(loc[1], loc[0]);
        dec = asin(loc[2] / mag);
        mag = sin(dec)*sin(sunDec)+cos(dec)*cos(sunDec)*cos(ra-sunRa-lagAngle);
        if (fabs(mag) > 1.0 + 1.0e-13)
            throw AtmosphereException("Arg for bulge calculation not physical");
        if (fabs(mag) > 1.0)
            mag = (mag > 0.0 ? 1.0 : -1.0);
        psi = acos(mag);
        
        index = (Integer)(height / 100.0);
        if (index >= 10) {
            index = 10;
            sz = height * z[10];
        }
        else {
            sz = (height - scale[index]) / 100.0;
            sz = height * (z[index] * sz + (1.0 - sz) * z[index+1]);
        }
        // Calculate bulge factor
        mag = 1.0 + pow(psi/2.0, 6.0);
        density[i] = rho0 * exp(sz) * mag;
    }
    
    return false;
}


void ExponentialAtmosphere::SetConstants(void)
{
    rho0 = 1.0;
    lagAngle = 0.523599;        // radians
    
    scale[ 0] =    0.0;             z[ 0]     = -0.160;
    scale[ 1] =  100.0;             z[ 1]     = -0.160;
    scale[ 2] =  200.0;             z[ 2]     = -0.110;
    scale[ 3] =  300.0;             z[ 3]     = -0.085;
    scale[ 4] =  400.0;             z[ 4]     = -0.067;
    scale[ 5] =  500.0;             z[ 5]     = -0.057;
    scale[ 6] =  600.0;             z[ 6]     = -0.050;
    scale[ 7] =  700.0;             z[ 7]     = -0.045;
    scale[ 8] =  800.0;             z[ 8]     = -0.040;
    scale[ 9] =  900.0;             z[ 9]     = -0.037;
    scale[10] = 1000.0;             z[10]     = -0.035;
}
