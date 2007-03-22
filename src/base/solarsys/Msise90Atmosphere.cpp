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
#include <stdio.h>
#include <math.h>

#include "Msise90Atmosphere.hpp"
#include "MessageInterface.hpp"
#include "CelestialBody.hpp"

#ifndef __SKIP_MSISE90__
extern "C" 
{ 
   void gtd6_(int*,float*,float*,float*,float*,float*,float*,float*,float*,int*,float*,float*);
}
#endif

//#define DEBUG_MSISE90_ATMOSPHERE

#ifdef DEBUG_MSISE90_ATMOSPHERE
static FILE *logFile;  // Temp log file
#endif

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
    #ifdef DEBUG_MSISE90_ATMOSPHERE
    logFile = NULL;
    #endif
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
fluxfilename        (msise.fluxfilename),
sod                 (msise.sod),
yd                  (msise.yd),
f107                (msise.f107),
f107a               (msise.f107a),
mass                (msise.mass)
{
        for (Integer i = 0; i < 7; i++)
      ap[i] = msise.ap[i];
}

//------------------------------------------------------------------------------
//  Msise90Atmosphere& operator= (const Msise90Atmosphere& m)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Msise90Atmosphere class.
 *
 * @param <m> the Msise90Atmosphere object whose data to assign to "this"
 *            AtmosphereModel.
 *
 * @return "this" Msise90Atmosphere with data of input Msise90Atmosphere st.
 */
//------------------------------------------------------------------------------
Msise90Atmosphere& Msise90Atmosphere::operator=(const Msise90Atmosphere& msise)
{
   if (&msise == this)
      return *this;

   AtmosphereModel::operator=(msise);
   
   fileData     = false;  // is this correct?
   fluxfilename = msise.fluxfilename;
   sod          = msise.sod;
   yd           = msise.yd;
   f107         = msise.f107;
   f107a        = msise.f107a;
   mass         = msise.mass;
   
        for (Integer i = 0; i < 7; i++)
      ap[i] = msise.ap[i];
      
   return *this;
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
   
   #ifdef DEBUG_MSISE90_ATMOSPHERE
   logFile = fopen("GMAT-MSISE90.txt", "w");
   #endif
   
   if (mCentralBody == NULL)
      throw AtmosphereException(
         "Central body pointer not set in MSISE90 model.");

   Real gha = mCentralBody->GetHourAngle(epoch);

   GetInputs(epoch);

   for (i = 0; i < count; ++i) 
   {
      i6 = i*6;
      mass = 48;
      
      //--------------------------------------------------
      // Longitude
      //--------------------------------------------------
      // Get spacecraft RightAscension
      ra = atan2(pos[i6+1], pos[i6]) * rad2deg;

      // Compute east longitude
      lon = ra - gha;

      // Range check because the lst calculation needs 0 < lon < 360
      if (lon < 0.0) lon += 360.0;
      if (lon > 360.0) lon -= 360.0;

      lat = atan2(pos[i6+2], sqrt(pos[i6]*pos[i6] + pos[i6+1]*pos[i6+1]))
               * rad2deg;

      // // compute Local Sidereal Time (LST = GMST + Longitude)
      // // according to Vallado Eq. 3-41
      // Real gmst = -gha;
      // lst = gmst + lon;
      //
      // // convert it to hours (1h = 15 deg according to Vallado 3.5)
      // lst /= 15.0;

      // Calculate lst as documented in the FORTRAN code
      lst = sod/3600.0 + lon/15.0;

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
            "   Radius = %15.9lf\n   GeodesicRad = %15.9lf\n  "
            "GeodesicLat = %lf\n", rad, geoRad, geolat);      
      #endif
      
      #ifdef DEBUG_MSISE90_ATMOSPHERE
         MessageInterface::ShowMessage(
            "Calculating MSISE90 Density from parameters:\n   "
            "yd = %d\n   sod = %lf   alt = %lf\n   lat = %lf\n   lon = %lf\n"
            "   lst = %lf\n   f107a = %lf\n   f107 = %lf\n   ap = "
            "[%lf %lf %lf %lf %lf %lf %lf]\n", yd, sod, alt, geolat, lon, lst,
            f107a, f107, ap[0], ap[1], ap[2], ap[3], ap[4], ap[5], ap[6]);
      #endif
      
      int xyd = yd;
      float xsod = sod;
      float xalt = alt;
      float xlat = geolat; // Geodetic Latitude
      float xlon = lon;
      float xlst = lst;
      float xf107a = f107a;
      float xf107 = f107;
      float xap[7];
      int xmass = mass;
      float xden[8];
      float xtemp[2];

      Integer j;
      for (j = 0; j < 7; j++)  xap[j] = ap[j];
      for (j = 0; j < 8; j++)  xden[j] = den[j];
      for (j = 0; j < 2; j++)   xtemp[j] = temp[j];

      #ifdef DEBUG_MSISE90_ATMOSPHERE
      fprintf(logFile, "Pre-GTDS6() \n");
      fprintf(logFile, "=========== \n");
      fprintf(logFile, "Epoch                  = %le \n", epoch);
      fprintf(logFile, "Year & Days            = %d \n", xyd);
      fprintf(logFile, "Seconds                = %le \n", xsod);
      fprintf(logFile, "Altitude               = %le \n", xalt);
      fprintf(logFile, "Latitude               = %le \n", xlat);
      fprintf(logFile, "Longitude              = %le \n", xlon);
      fprintf(logFile, "Solar Time             = %le \n", xlst);
      fprintf(logFile, "F107 Average           = %le \n", xf107a);
      fprintf(logFile, "F107                   = %le \n", xf107);
      fprintf(logFile, "Geomagnetic index[0]   = %le \n", xap[0]);
      fprintf(logFile, "Geomagnetic index[1]   = %le \n", xap[1]);
      fprintf(logFile, "Geomagnetic index[2]   = %le \n", xap[2]);
      fprintf(logFile, "Geomagnetic index[3]   = %le \n", xap[3]);
      fprintf(logFile, "Geomagnetic index[4]   = %le \n", xap[4]);
      fprintf(logFile, "Geomagnetic index[5]   = %le \n", xap[5]);
      fprintf(logFile, "Geomagnetic index[6]   = %le \n", xap[6]);
      fprintf(logFile, "Mass                   = %le \n", xmass);
      fprintf(logFile, "HE Number Density      = %le \n", xden[0]);
      fprintf(logFile, "O Number Density       = %le \n", xden[1]);
      fprintf(logFile, "N2 Number Density      = %le \n", xden[2]);
      fprintf(logFile, "O2 Number Density      = %le \n", xden[3]);
      fprintf(logFile, "AR Number Density      = %le \n", xden[4]);
      fprintf(logFile, "Total Mass Density     = %le \n", xden[5]);
      fprintf(logFile, "H Number Density       = %le \n", xden[7]);
      fprintf(logFile, "EXOSPHERIC Temperature = %le \n", xtemp[0]);
      fprintf(logFile, "Temperature at Alt     = %le \n", xtemp[1]);
      fprintf(logFile, "\n");
      fprintf(logFile, "\n");
      #endif
      
#ifndef __SKIP_MSISE90__
      gtd6_(&xyd,&xsod,&xalt,&xlat,&xlon,&xlst,&xf107a,&xf107,&xap[0],&xmass,&xden[0],&xtemp[0]);
#endif
      
      #ifdef DEBUG_MSISE90_ATMOSPHERE
      fprintf(logFile, "Post-GTDS6() \n");
      fprintf(logFile, "=========== \n");
      fprintf(logFile, "Epoch                  = %le \n", epoch);
      fprintf(logFile, "Year & Days            = %d \n", xyd);
      fprintf(logFile, "Seconds                = %le \n", xsod);
      fprintf(logFile, "Altitude               = %le \n", xalt);
      fprintf(logFile, "Latitude               = %le \n", xlat);
      fprintf(logFile, "Longitude              = %le \n", xlon);
      fprintf(logFile, "Solar Time             = %le \n", xlst);
      fprintf(logFile, "F107 Average           = %le \n", xf107a);
      fprintf(logFile, "F107                   = %le \n", xf107);
      fprintf(logFile, "Geomagnetic index[0]   = %le \n", xap[0]);
      fprintf(logFile, "Geomagnetic index[1]   = %le \n", xap[1]);
      fprintf(logFile, "Geomagnetic index[2]   = %le \n", xap[2]);
      fprintf(logFile, "Geomagnetic index[3]   = %le \n", xap[3]);
      fprintf(logFile, "Geomagnetic index[4]   = %le \n", xap[4]);
      fprintf(logFile, "Geomagnetic index[5]   = %le \n", xap[5]);
      fprintf(logFile, "Geomagnetic index[6]   = %le \n", xap[6]);
      fprintf(logFile, "Mass                   = %le \n", xmass);
      fprintf(logFile, "HE Number Density      = %le \n", xden[0]);
      fprintf(logFile, "O Number Density       = %le \n", xden[1]);
      fprintf(logFile, "N2 Number Density      = %le \n", xden[2]);
      fprintf(logFile, "O2 Number Density      = %le \n", xden[3]);
      fprintf(logFile, "AR Number Density      = %le \n", xden[4]);
      fprintf(logFile, "Total Mass Density     = %le \n", xden[5]);
      fprintf(logFile, "H Number Density       = %le \n", xden[6]);
      fprintf(logFile, "N Number Density       = %le \n", xden[7]);
      fprintf(logFile, "EXOSPHERIC Temperature = %le \n", xtemp[0]);
      fprintf(logFile, "Temperature at Alt     = %le \n", xtemp[1]);
      fprintf(logFile, "\n");
      fprintf(logFile, "\n");
      #endif
      
      yd = xyd;
      sod = xsod;
      alt = xalt;
      lat = xlat;
      lon = xlon;
      lst = xlst;
      f107a = xf107a;
      f107 = xf107;
      mass = xmass;
      
      for (j = 0; j < 7; j++)  ap[j] = xap[j];
      for (j = 0; j < 8; j++)  den[j] = xden[j];
      for (j = 0; j < 2; j++)  temp[j] = xtemp[j];

      density[i] = den[5] * 1000.0;

      #ifdef DEBUG_MSISE90_ATMOSPHERE
         MessageInterface::ShowMessage(
            "   Density = %15.9le\n", density[i]);
      #endif
   }
   
   #ifdef DEBUG_MSISE90_ATMOSPHERE
   fflush(logFile);
   fclose(logFile);
   #endif
   
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

    sod  = 86400.0 * (epoch - iEpoch + 0.5);  // Includes noon/midnight adjustment
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
   return (new Msise90Atmosphere(*this));
}
