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


//------------------------------------------------------------------------------
//  Msise90Atmosphere()
//------------------------------------------------------------------------------
/**
 *  Constructor.
 */
//------------------------------------------------------------------------------
Msise90Atmosphere::Msise90Atmosphere() :
    AtmosphereModel     ("MSISE90"),
    fileData            (false),
    fluxfilename        (""),
    nominalF107         (125.0),
    nominalF107a        (172.0),
    nominalAp           (40.0)
{
}


//------------------------------------------------------------------------------
//  ~Msise90Atmosphere()
//------------------------------------------------------------------------------
/**
 *   Destructor.
 */
//------------------------------------------------------------------------------
Msise90Atmosphere::~Msise90Atmosphere()
{
}


//------------------------------------------------------------------------------
//  bool Density(Real *pos, Real *density, Real epoch, Integer count)
//------------------------------------------------------------------------------
/**
 *  Calculates the density at each of the states in the input vector, using the 
 *  MSISE90 atmosphere model.
 * 
 *  @param pos       The input vector of spacecraft states
 *  @param density   The array of output densities
 *  @param epoch     The current TAIJulian epoch
 *  @param count     The number of spacecraft contained in pos 
 */
//------------------------------------------------------------------------------
bool Msise90Atmosphere::Density(Real *pos, Real *density, Real epoch, 
                                Integer count)
{
    Integer i, i6;
    Real    alt;
  
    // For now, hard code the MSISE90 parameters.
    Real    lon   =   -70.0;     /// @todo: longitude calculations
    Real    lst   =    16.0;     /// @todo: Code local apparent solar time (Hrs)
    Real    lat, rad, arg;
    Real    den[8], temp[2];
    
    GetInputs(epoch);
    for (i = 0; i < count; ++i) {
        i6 = i*6;
        rad = sqrt(pos[ i6 ]*pos[ i6 ] + 
                   pos[i6+1]*pos[i6+1] + 
                   pos[i6+2]*pos[i6+2]);
        alt = rad - 6378.14;
        // For now, geocentric latitude
        arg = pos[i6+2] / rad;
        arg = ((fabs(arg) <= 1.0) ? arg : arg / fabs(arg));
        lat = acos(arg) * 180.0 / M_PI; 
        msise90.GTD6(yd,sod,alt,lat,lon,lst,f107a,f107,ap,48,den,temp);
        density[i] = den[5];
    }
    
    return true;
}


//------------------------------------------------------------------------------
//  void GetInputs(Real epoch)
//------------------------------------------------------------------------------
/**
 *  Sets the input global data for the model, either from a file or from user
 *  input constants.
 *
 *  @param epoch The current TAIJulian epoch
 */
//------------------------------------------------------------------------------
void Msise90Atmosphere::GetInputs(Real epoch)
{
    /// @todo Get the MSISE90 epoch wired up; this is a crude approximation
    Integer iEpoch = (Integer)(epoch);  // Truncate the epoch
    Integer yearOffset = (Integer)((epoch + 5.5) / 365.25);
    Integer year   = 1941 + yearOffset;
    Integer doy = iEpoch - (Integer)(yearOffset * 365.25) + 5;

    sod  = 86400.0 * (epoch - iEpoch);  // Includes noon/midnight adjustment
    sod = (sod < 0.0 ? sod + 86400.0 : sod);
    yd   = year * 1000 + doy;

    if (fileData) {
       /// @todo Implement the file reader
    }
    else {
       f107 = nominalF107;
       f107a = nominalF107a;
       for (Integer i = 0; i < 7; i++)
           ap[i] = nominalAp;
    }
}
