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
#include "MessageInterface.hpp"
#include "CelestialBody.hpp"

#define DEBUG_MSISE90_ATMOSPHERE

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
    fluxfilename        ("")
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
  
   Real    lon;     // longitude
   Real    lst;     // Local apparent solar time (Hrs)
   Real    radlat, lat, geolat, rad, geoRad, arg;
   Real    den[8], temp[2], rad2deg = 180.0 / M_PI, ra;
   Real    flatteningFactor = 1.0 / 298.257223563;  // WGS-84
   Real    geodesicFactor = 6378.137 * (1.0 - flatteningFactor);

////// Lifted from PlanetData
   if (mCentralBody == NULL)
      throw AtmosphereException(
         "Central body pointer not set in MSISE90 model.");

   Real gmst, gha = mCentralBody->GetHourAngle(epoch);

   GetInputs(epoch);
   for (i = 0; i < count; ++i) {
      i6 = i*6;

      //--------------------------------------------------
      // Longitude
      //--------------------------------------------------
      // Get spacecraft RightAscension
      ra = atan2(pos[i6+1], pos[i6]) * rad2deg;

      // Compute east longitude
      lon = ra - gha;

      lat = atan2(pos[i6+2], sqrt(pos[i6]*pos[i6] + pos[i6+1]*pos[i6+1]))
               * rad2deg;

      // compute Local Sidereal Time (LST = GMST + Longitude)
      // according to Vallado Eq. 3-41
      gmst = -gha;
      lst = gmst + lon;

      // convert it to hours (1h = 15 deg according to Vallado 3.5)
      lst /= 15.0;
      rad = sqrt(pos[ i6 ]*pos[ i6 ] +
                 pos[i6+1]*pos[i6+1] +
                 pos[i6+2]*pos[i6+2]);
      // Now geodetic latitude
      arg = pos[i6+2] / rad;
      arg = ((fabs(arg) <= 1.0) ? arg : arg / fabs(arg));
      radlat = M_PI / 2.0 - acos(arg);
      // Convert to geodetic latitude, in degrees
      radlat += flatteningFactor * sin(2.0 * radlat);
      geolat = radlat * 180.0 / M_PI;

      // Geodetic altitude
      geoRad = geodesicFactor / sqrt(1.0 - (2.0 - flatteningFactor) *
                                flatteningFactor * cos(radlat) * cos(radlat));
      alt = rad - geoRad;

      #ifdef DEBUG_MSISE90_ATMOSPHERE
         MessageInterface::ShowMessage(
            "Calculating MSISE90 Density from parameters:\n   "
            "yd = %d\n   sod = %lf   alt = %lf\n   lat = %lf\n   lon = %lf\n"
            "   lst = %lf\n   f107a = %lf\n   f107 = %lf\n   ap = "
            "[%lf %lf %lf %lf %lf %lf %lf]\n", yd, sod, alt, lat, lon, lst,
            f107a, f107, ap[0], ap[1], ap[2], ap[3], ap[4], ap[5], ap[6]);
         MessageInterface::ShowMessage(
            "   Radius = %15.9lf\n   GeodesicRad = %15.9lf\n  "
            "GeodesicLat = %lf\n", rad, geoRad, geolat);
      #endif

      msise90.GTD6(yd,sod,alt,lat,lon,lst,f107a,f107,ap,48,den,temp);
      density[i] = den[5];

      #ifdef DEBUG_MSISE90_ATMOSPHERE
         MessageInterface::ShowMessage(
            "   Density = %15.9le\n", density[i]);
      #endif
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

//------------------------------------------------------------------------------
// GmatBase* Clone() cnst
//------------------------------------------------------------------------------
/**
 * Clone the object (inherited from GmatBase).
 *
 * @return a clone of "this" object.
 */
//------------------------------------------------------------------------------
GmatBase* Msise90Atmosphere::Clone() const
{
   return new Msise90Atmosphere(*this);
}


//------------------------------------------------------------------------------
//  Msise90Atmosphere(const Msise90Atmosphere& msise)
//------------------------------------------------------------------------------
/**
 *  Copy constructor.
 *
 * @param <msise> MSISE90 object to copy increating the new one.
 */
//------------------------------------------------------------------------------
Msise90Atmosphere::Msise90Atmosphere(const Msise90Atmosphere& msise) :
AtmosphereModel     (msise),
fileData            (false),  // is this correct?
fluxfilename        (msise.fluxfilename)
{
}

