//$Id: Ionosphere.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         Ionosphere Model
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Author: Tuan Dang Nguyen
// Created: 2010/06/21
//
/**
 * IRI 2007 ionosphere media correction model.
 */
//------------------------------------------------------------------------------

#include "Ionosphere.hpp"
#include "GmatConstants.hpp"
#include "TimeSystemConverter.hpp"
#include "CalculationUtilities.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include "StringUtil.hpp"
#include <fstream>
#include <sstream>

//#define DEBUG_IONOSPHERE_ELECT_DENSITY
//#define DEBUG_IONOSPHERE_TEC
//#define DEBUG_IONOSPHERE_CORRECTION
//#define DEBUG_IONOSPHERE_CONSTRUCTION
//#define DEBUG_IONOSPHERE_INITIALIZE

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
IonosphereCorrectionModel* IonosphereCorrectionModel::instance = NULL;

const Real Ionosphere::NUM_OF_INTERVALS = 200;
const Real Ionosphere::IONOSPHERE_MAX_ALTITUDE = 2000.0;

//// These arrays are used for Guasian Quadratic algorithm
//const Real Ionosphere::QUAD_WEIGHTS[20] = { 0.008807003569575835, 0.02030071490019353, 0.03133602416705452, 0.04163837078835238, 0.05096505990862025, 0.05909726598075916, 0.06584431922458829, 0.07104805465919108, 0.07458649323630191, 0.076376693565363, 0.076376693565363, 0.07458649323630191, 0.07104805465919108, 0.06584431922458829, 0.05909726598075916, 0.05096505990862025, 0.04163837078835238, 0.03133602416705452, 0.02030071490019353, 0.008807003569575835 };
//const Real Ionosphere::QUAD_POINTS[20] = { 0.003435700407452558, 0.0180140363610431, 0.04388278587433703, 0.08044151408889055, 0.1268340467699246, 0.1819731596367425, 0.2445664990245864, 0.3131469556422902, 0.3861070744291775, 0.4617367394332513, 0.5382632605667487, 0.6138929255708225, 0.6868530443577098, 0.7554335009754136, 0.8180268403632576, 0.8731659532300754, 0.9195584859111095, 0.956117214125663, 0.981985963638957, 0.9965642995925474 };


IonosphereCorrectionModel* IonosphereCorrectionModel::Instance() 
{
   if (instance == NULL)
      instance = new IonosphereCorrectionModel;
   return instance;
}


Ionosphere* IonosphereCorrectionModel::GetIonosphereInstance()
{
   if (ionosphereObj == NULL)
      ionosphereObj = new Ionosphere("IRI2007");

   return ionosphereObj;
}


IonosphereCorrectionModel::IonosphereCorrectionModel()
{
   ionosphereObj = NULL;
}


IonosphereCorrectionModel::~IonosphereCorrectionModel() 
{
   if (ionosphereObj)
   {
      delete ionosphereObj;
      ionosphereObj = NULL;
   }

   if (instance)
   {
      delete instance;
      instance = NULL;
   }
}


//------------------------------------------------------------------------------
// Ionosphere(const std::string& nomme)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
//------------------------------------------------------------------------------
Ionosphere::Ionosphere(const std::string &nomme):
   MediaCorrection("Ionosphere", nomme),
   yyyymmddMin      (20000101),          // year 2000, month 01, day 01
   yyyymmddMax      (20000101)           // year 2000, month 01, day 01
{
#ifdef DEBUG_IONOSPHERE_CONSTRUCTION
   MessageInterface::ShowMessage("Ionosphere default construction\n");
#endif

   objectTypeNames.push_back("Ionosphere");
   model = 2;                 // 2 for IRI2007 ionosphere model
   
   waveLength = 0.0;          // wave length of the signal
   epoch = 0.0;               // time
   yyyy = 0;                  // year
   mmdd = 0;                  // month and day
   hours = 0.0;               // hours
}


//------------------------------------------------------------------------------
// ~Ionosphere()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Ionosphere::~Ionosphere()
{
}


//------------------------------------------------------------------------------
// Ionosphere(const Ionosphere& ions)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
Ionosphere::Ionosphere(const Ionosphere& ions):
   MediaCorrection(ions),
   yyyymmddMin  (ions.yyyymmddMin),
   yyyymmddMax  (ions.yyyymmddMax),
   waveLength   (ions.waveLength),
   epoch        (ions.epoch),
   yyyy         (ions.yyyy),
   mmdd         (ions.mmdd),
   hours        (ions.hours)
{
#ifdef DEBUG_IONOSPHERE_CONSTRUCTION
   MessageInterface::ShowMessage("Ionosphere copy construction\n");
#endif

   stationLoc    = ions.stationLoc;
   spacecraftLoc = ions.spacecraftLoc;
}


//-----------------------------------------------------------------------------
// Ionosphere& operator=(const Ionosphere& ions)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param tps The Troposphere that is provides parameters for this one
 *
 * @return This Troposphere, configured to match tps
 */
//-----------------------------------------------------------------------------
Ionosphere& Ionosphere::operator=(const Ionosphere& ions)
{
   if (this != &ions)
   {
      MediaCorrection::operator=(ions);
      
      yyyymmddMin     = ions.yyyymmddMin;
      yyyymmddMax     = ions.yyyymmddMax;

      waveLength      = ions.waveLength;
      epoch           = ions.epoch;
      yyyy            = ions.yyyy;
      mmdd            = ions.mmdd;
      hours           = ions.hours;
      stationLoc      = ions.stationLoc;
      spacecraftLoc   = ions.spacecraftLoc;
   }
   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone()
//------------------------------------------------------------------------------
/**
 * Clone a Ionosphere object
 */
//------------------------------------------------------------------------------
GmatBase* Ionosphere::Clone() const
{
   return new Ionosphere(*this);
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Performs any pre-run initialization that the object needs.
 *
 * @return true unless initialization fails.
 */
//------------------------------------------------------------------------------
extern "C" int load_all_files__(integer *ierror, char *errmsg, ftnlen errmsg_len);
bool Ionosphere::Initialize()
{
   if (IsInitialized())
      return true;

#ifdef DEBUG_IONOSPHERE_INITIALIZE
   MessageInterface::ShowMessage("Ionosphere::Initialize()\n");
#endif

   if (MediaCorrectionInterface::Initialize())
   {
      // Get time range from ap.dat file
      GetTimeRange();

      // Read all data files and store data to memory
      integer errNo;
      ftnlen len = 0;
      char errmsg[256]; 
      load_all_files__(&errNo, &errmsg[0], len);
      if (errNo >= 1000)
         throw MeasurementException("Error: can't open Ionosphere data file.\n");
      else if ((1000 > errNo)&&(errNo > 0))
      {
         std::string str(errmsg);
         throw MeasurementException(str + "\n");
      }

      isInitialized = true;
   }

   return true;
}


//------------------------------------------------------------------------------
//  void GetTimeRange()
//------------------------------------------------------------------------------
/**
 * This function is used to specify time range for Ionosphere model. 
 * The range is set to yyyymmdMin and yyyymmddMax variables.
 *
 */
//------------------------------------------------------------------------------
void Ionosphere::GetTimeRange()
{
   Integer year, month, day;
   std::fstream fs;
   std::stringstream ss;
   std::string theLine, oldLine;
   
   // 1. Open ap.data file
   std::string filename = dataPath + "/IonosphereData/ap.dat";
   try
   {
      fs.open(filename.c_str(), std::fstream::in);
   }
   catch(...)
   {
      throw MeasurementException("Error: " + filename + " file does not exist or cannot open.\n");
   }
      
   // 2. Get time lower bound (It is shown in the first line of ap.dat file)
   // 2.1. Get the first line in ap.dat file
   std::getline(fs,theLine);
   // 2.2. Extract year, month, and day from this line
   ss << theLine;
   ss >> year >> month >> day;
   if (year >= 58)
      year = 1900 + year;
   else
      year = 2000 + year;
   yyyymmddMin = year*10000 + month*100 + day;

   // 3. Get time upper bound (It is shown in the last line of ap.dat file)
   // 3.1. Get the last line in ap.dat file
   while(!fs.eof())
   {
      oldLine = theLine;
      std::getline(fs,theLine);
   }
   if (theLine == "")
      theLine = oldLine;
   //MessageInterface::ShowMessage("last line = %s\n", theLine.c_str());

   // 3.2. Extract year, month, and day from this line
   ss.str("");
   ss << theLine;
   ss >> year >> month >> day;
   if (year >= 58)
      year = 1900 + year;
   else
      year = 2000 + year;
   yyyymmddMax = year*10000 + month*100 + day;

   // 4. Close ap.data file
   fs.close();

   // 5. Verify the range:
   if (yyyymmddMax <= yyyymmddMin)
      throw MeasurementException("Error: time range specified from " + filename + " file is invalid.\n");
}


//------------------------------------------------------------------------------
// bool SetWaveLength(Real lambda)
//------------------------------------------------------------------------------
/**
 * Set wave length
 * @param lambda  The wave length
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetWaveLength(Real lambda)
{
   waveLength = lambda;
   return true;
}


//------------------------------------------------------------------------------
// bool SetTime(GmatEpoch ep)
//------------------------------------------------------------------------------
/**
 * Set time
 * @param ep  The time
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetTime(GmatEpoch ep)
{
   epoch = ep;
   Real temp;
   std::string time;
   TimeSystemConverter::Instance()->Convert("A1ModJulian", epoch, "", "UTCGregorian", temp, time, 2);
   yyyy = atoi(time.substr(0,4).c_str());
   mmdd = atoi(time.substr(5,2).c_str())*100 + atoi(time.substr(8,2).c_str());
   hours = atof(time.substr(11,2).c_str()) + atof(time.substr(14,2).c_str())/60 +
      atof(time.substr(17,2).c_str())/3600 + atof(time.substr(20,3).c_str())/3600000.0;
   
   return true;
}


//------------------------------------------------------------------------------
// bool SetStationPosition(Rvector3 p)
//------------------------------------------------------------------------------
/**
 * Set station position
 * @param p  Position of station. (unit: km)
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetStationPosition(Rvector3 p)
{
   stationLoc = p;
   return true;
}


//------------------------------------------------------------------------------
// bool SetSpacecraftPosition(Rvector3 p)
//------------------------------------------------------------------------------
/**
 * Set spacecraft position
 * @param p  Position of spacecraft. (unit: km)
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetSpacecraftPosition(Rvector3 p)
{
   spacecraftLoc = p;
   return true;
}


//------------------------------------------------------------------------------
// bool SetEarthRadius(Real r)
//------------------------------------------------------------------------------
/**
 * Set earth radius
 * @param r  radius of earth. (unit: km)
 */
//------------------------------------------------------------------------------
bool Ionosphere::SetEarthRadius(Real r)
{
   earthRadius= r;
   return true;
}


//---------------------------------------------------------------------------
// float ElectronDensity(Rvector3 pos2, Rvector3 pos1)
//---------------------------------------------------------------------------
/**
 * This function is used to calculate average electron density along
 * 2 positions.
 *
 * @ param pos1 the first position in Earth fixed coordinate system (unit: km)
 * @ param pos2 the second position in Earth fixed coordinate system (unit: km)
 *
 * return value is electron density (unit: number electrons per m3)
 *
 */
//---------------------------------------------------------------------------
//extern "C" int iri_web__(integer *jmag, logical *jf, real *alati, real *
//	along, integer *iyyyy, integer *mmdd, integer *iut, real *dhour, real 
//	*height, real *h_tec_max__, integer *ivar, real *vbeg, real *vend, 
//	real *vstp, real *a, real *b, integer *ier);

extern "C" int iri_sub__(logical *jf, integer *jmag, real *alati, real *
   along, integer *iyyyy, integer *mmdd, real *dhour, real *heibeg, real
   *heiend, real *heistp, real *outf, real *oarr, integer *ier);

float Ionosphere::ElectronDensity(Rvector3 pos1)
{
   CelestialBody* earth = solarSystem->GetBody("Earth");
   Real radius = earth->GetRealParameter(earth->GetParameterID("EquatorialRadius"));
   Real flattening = earth->GetRealParameter(earth->GetParameterID("Flattening"));

   Rvector6 state;
   state[0] = pos1[0]; state[1] = pos1[1]; state[2] = pos1[2];

   // the fisrt position's latitude and longitude (unit: degree):
   real latitude  = (real)(GmatCalcUtil::CalculatePlanetData("Latitude", state, radius, flattening, 0.0));
   real longitude = (real)(GmatCalcUtil::CalculatePlanetData("Longitude", state, radius, flattening, 0.0));
   real hbeg      = (real)(GmatCalcUtil::CalculatePlanetData("Altitude", state, radius, flattening, 0.0));

   // mmag  = 0 geographic   =1 geomagnetic coordinates
   integer jmag = 0;   // 1;
   
   // jf(1:30)     =.true./.false. flags; explained in IRISUB.FOR
   logical jf[31];
   for (int i=1; i <= 30; ++i)
      jf[i] = TRUE_;
   
   //jf[1] = FALSE_;
   jf[2] = FALSE_;           // FALSE_ for Te, Ti not computed
   jf[3] = FALSE_;           // FALSE_ for Ni not computed

   jf[5] = FALSE_;           // FALSE_ for foF2 - URSI
   jf[6] = FALSE_;           // FALSE_ for Ni - DS-95 & TTS-03
   jf[23] = FALSE_;          // FALSE_ for Te_topside (Intercosmos)
   jf[29] = FALSE_;          // FALSE_ for new options as def. by JF(30)
   jf[30] = FALSE_;          // FALSE_ for NeQuick topside model
   
   jf[12] = FALSE_;          // FALSE_ for no messages to unit 6
   jf[21] = FALSE_;          // FALSE_ for ion drift not computed
   jf[28] = FALSE_;          // FALSE_ for spread-F probability not computed
   
   // iy,md        date as yyyy and mmdd (or -ddd)
   // hour         decimal hours LT (or UT+25)
   integer iy = (integer)yyyy;
   integer md = (integer)mmdd;
   real hour = (real)hours;
   
   // Upper and lower integration limits
   //real hbeg = (real)(pos1.GetMagnitude() - earthRadius); // 0

//   if (hbeg < 1.0)
//      hbeg = 1.0;         // If height is less than 1.0km then set it to 1.0km
   // Accept height less than 1.0 Km and below sea level (0.0)
   real hend = hbeg;
   real hstp = 1.0;
   
   integer error = 0;

   real outf[20*501+1];
   real oarr[51];


   integer ivar = 1;        // get attitude result
   integer iut = 1;         // 1 for universal time; 0 for local time

# ifdef DEBUG_IONOSPHERE_ELECT_DENSITY
   MessageInterface::ShowMessage("           .At time = %lf A1Mjd:",epoch);
   MessageInterface::ShowMessage("         year = %d   md = %d   hour = %lf h,   time type = %s,\n", iy, md, hour, (iut?"Universal":"Local"));
   MessageInterface::ShowMessage("              At position (x,y,z) = (%lf,  %lf,  %lf)km in Earth fixed coordinate system: ", pos1[0], pos1[1], pos1[2]);
   MessageInterface::ShowMessage("(latitude = %lf degree,  longitude = %lf degree,  attitude = %lf km,  ", latitude, longitude, hbeg);
   MessageInterface::ShowMessage("coordinate system type = %s)\n",(jmag?"Geomagetic":"Geographic"));
#endif

//   iri_web__(&jmag, &jf[1], &latitude, &longitude, &iy, &md, &iut, &hour, &hbeg, &hbeg, 
//      &ivar, &hbeg, &hend, &hstp, &outf[21], &oarr[1], &error);
   hour = hour + iut*25.0;
   iri_sub__(&jf[1], &jmag, &latitude, &longitude, &iy, &md, &hour, &hbeg, &hend, &hstp, &outf[21], &oarr[1], &error);
   if (error != 0)
      throw MeasurementException("Ionosphere data files not found\n");

   real density = outf[20+1];
   if (density < 0.0)
      density = 0.0;

#ifdef DEBUG_IONOSPHERE_ELECT_DENSITY
   MessageInterface::ShowMessage("              Electron density at that time and location = %le electrons per m3.\n", density);
#endif
   
   return density;         //*(pos2-pos1).GetMagnitude();
}


//---------------------------------------------------------------------------
// Real Ionosphere::TEC()
// This function is used to calculate number of electron inside a 1 meter 
// square cross sectioncylinder with its bases on spacecraft and on ground 
// station.
//
//  return value: tec  (unit: number of electrons per 1 meter square)
//---------------------------------------------------------------------------
Real Ionosphere::TEC()
{
#ifdef DEBUG_IONOSPHERE_TEC
   MessageInterface::ShowMessage("         It performs calculation electron density along the path\n");
   MessageInterface::ShowMessage("            from ground station location: (%lf,  %lf,  %lf)km\n", stationLoc[0], stationLoc[1], stationLoc[2]);
   MessageInterface::ShowMessage("            to spacecraft location:       (%lf,  %lf,  %lf)km\n", spacecraftLoc[0], spacecraftLoc[1], spacecraftLoc[2]);
   MessageInterface::ShowMessage("         Earth radius : %lf\n", earthRadius);
#endif
//   Rvector3 sR;
//   if (spacecraftLoc.GetMagnitude() - earthRadius > IONOSPHERE_MAX_ATTITUDE)
//      sR = spacecraftLoc.GetUnitVector() * (IONOSPHERE_MAX_ATTITUDE + earthRadius); 
//   else
//      sR = spacecraftLoc;

//   //Rvector3 dR = (spacecraftLoc - stationLoc) / NUM_OF_INTERVALS;
//   Rvector3 dR = (sR - stationLoc) / NUM_OF_INTERVALS;
//   Rvector3 p1 = stationLoc;

   // Fix bug to calculate end point
   // Solution to where a line intersects a sphere is a quadratic equation
   Real a, b, c, discriminant;
   Rvector3 s = spacecraftLoc - stationLoc;
   // Solve for intersection of signal with sphere of IONOSPHERE_MAX_ALTITUDE
   a = s*s;
   b = 2.0 * stationLoc * s;
   c = stationLoc*stationLoc - GmatMathUtil::Pow(earthRadius + IONOSPHERE_MAX_ALTITUDE, 2);

   discriminant = b*b - 4.0 * a*c;
   if (discriminant <= 0)
   {
       return 0; // Path does not travel through ionosphere
   }

   Real d1, d2; // Roots of quadratic equation

   d1 = (-b - GmatMathUtil::Sqrt(b*b - 4.0*a*c)) / (2.0*a);
   d2 = (-b + GmatMathUtil::Sqrt(b*b - 4.0*a*c)) / (2.0*a);

   if ((d1 > 1 && d2 > 1) || (d1 < 0 && d2 < 0))
   {
       return 0; // Segment between start and end does not travel through ionosphere
   }

   d1 = GmatMathUtil::Max(d1, 0); // Truncate segment before start point of signal
   d2 = GmatMathUtil::Min(d2, 1); // Truncate segment after end point of signal

   Rvector3 start, end;
   start = stationLoc + d1*s;
   end   = stationLoc + d2*s;

   // This is our old algorithm for integration
   //Rvector3 dR = (spacecraftLoc - stationLoc) / NUM_OF_INTERVALS;

   // Evenly spaced integration points
   Rvector3 dR = (end - start) / NUM_OF_INTERVALS;
   Rvector3 p1 = start;
   Rvector3 p2;
   Real electdensity, ds;
   Real tec = 0.0;
   for(int i = 0; i < NUM_OF_INTERVALS; ++i)
   {
      p2 = p1 + dR;
      electdensity = ElectronDensity((p1+p2)/2);                // unit: electron / m^3
      ds = (p2-p1).GetMagnitude()*GmatMathConstants::KM_TO_M;   // unit: m
      tec += electdensity*ds;                                   // unit: electron / m^2
      p1 = p2;
   }

   //// Gaussian Quadrature:
   //Rvector3 dR = (end - start);
   //Rvector3 p1 = start;
   //Rvector3 p2;
   //Real electdensity, ds;
   //Real tec = 0.0;
   //for(int i = 0; i < NUM_OF_INTERVALS; ++i)
   //{
   //   p2 = start + dR*Ionosphere::QUAD_POINTS[i];
   //   electdensity = ElectronDensity(p2)*Ionosphere::QUAD_WEIGHTS[i];                   // unit: electron / m^3
   //   tec += electdensity*dR.GetMagnitude()*GmatMathConstants::KM_TO_M;                                   // unit: electron / m^2
   //}
   
   return tec;
}


//---------------------------------------------------------------------------
// Real Ionosphere::BendingAngle()
//---------------------------------------------------------------------------
Real Ionosphere::BendingAngle()
{
   // 1. Calculate end points which speccify path inside ionosphere
   // Solution to where a line intersects a sphere is a quadratic equation
   Real a, b, c, discriminant;
   Rvector3 s = spacecraftLoc - stationLoc;
   // Solve for intersection of signal with sphere of IONOSPHERE_MAX_ALTITUDE
   a = s*s;
   b = 2.0 * stationLoc * s;
   c = stationLoc*stationLoc - GmatMathUtil::Pow(earthRadius + IONOSPHERE_MAX_ALTITUDE, 2);

   discriminant = b*b - 4.0 * a*c;
   if (discriminant <= 0)
   {
      return 0; // Path does not travel through ionosphere
   }

   Real d1, d2; // Roots of quadratic equation

   d1 = (-b - GmatMathUtil::Sqrt(b*b - 4.0*a*c)) / (2.0*a);
   d2 = (-b + GmatMathUtil::Sqrt(b*b - 4.0*a*c)) / (2.0*a);

   if ((d1 > 1 && d2 > 1) || (d1 < 0 && d2 < 0))
   {
      return 0; // Segment between start and end does not travel through ionosphere
   }

   d1 = GmatMathUtil::Max(d1, 0); // Truncate segment before start point of signal
   d2 = GmatMathUtil::Min(d2, 1); // Truncate segment after end point of signal

   Rvector3 start, end;
   start = stationLoc + d1*s;
   end = stationLoc + d2*s;

   // 2. Calculate angle correction
   //Rvector3 rangeVec = spacecraftLoc - stationLoc;    // made changes by TUAN NGUYEN
   Rvector3 rangeVec = end - start;                     // made changes by TUAN NGUYEN
   Rvector3 dR = rangeVec / NUM_OF_INTERVALS;
   //Rvector3 r_i1 = spacecraftLoc;                     // made changes by TUAN NGUYEN
   Rvector3 r_i1 = end;                                 // made changes by TUAN NGUYEN
   Rvector3 r_i;
   Real n_i, n_i1, density_i, density_i1;
   
   // Frequency of signal
   Real freq = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / waveLength;

   // Angle of incidence at position r_i1
   Real theta_i1 = GmatMathUtil::ACos(rangeVec.GetUnitVector()*r_i1.GetUnitVector());             // unit: radian
   
   // Elevetion angle at position r_i1
   Real beta_i1 = GmatMathConstants::PI_OVER_TWO - theta_i1;                                      // unit: radian
   //MessageInterface::ShowMessage("Elevation angle = %.12lf degree\n", beta_i1*GmatMathConstants::DEG_PER_RAD);

   // Electron density at position r_i1
   density_i1 = ElectronDensity(r_i1);

   // Index of refaction at position ri1
   n_i1 = 1 - 40.3*density_i1 / (freq*freq);

   // Refaction correction 
   Real dtheta_i1 = 0.0;
   for (int i = NUM_OF_INTERVALS; i > 0; --i)
   {
      // the previous position of r_i
      r_i = r_i1 - dR;
      
      // density at position r_i 
      density_i = ElectronDensity(r_i);
      
      // index of refaction at position r_i 
      n_i = 1 - 40.3*density_i/(freq*freq);

      Real dtheta = ((n_i1 - n_i)/ n_i) * GmatMathUtil::Tan(theta_i1);
      //MessageInterface::ShowMessage("dtheta = %.12lf rad\n", dtheta);
      dtheta_i1 += dtheta;

      // Reset position
      r_i1 = r_i;
      // Recalculate angle of incidence
      theta_i1 = GmatMathUtil::ACos(rangeVec.GetUnitVector()*r_i1.GetUnitVector()) - dtheta_i1;             // unit: radian
      // Reset desity, index of refaction for the new position
      density_i1 = density_i;
      n_i1 = n_i;
   }
   
   Real dbeta = -dtheta_i1;             // elevation angle's correction equals negative of incidence angle's correction 
   //MessageInterface::ShowMessage("Elevation angle correction = %.12lf x e-3 degree\n", dbeta*GmatMathConstants::DEG_PER_RAD*1000.0);
   return dbeta;
}


//---------------------------------------------------------------------------
// RealArray Ionosphere::Correction()
// This function is used to calculate Ionosphere correction
// Return values:
//    . Range correction (unit: m)
//    . Angle correction (unit: radian)
//    . Time correction  (unit: s)
//---------------------------------------------------------------------------
RealArray Ionosphere::Correction()
{
#ifdef DEBUG_IONOSPHERE_CORRECTION
   MessageInterface::ShowMessage("Ionosphere::Correction() start\n");
#endif
   // Initialize before doing calculation
   if (!IsInitialized())
      Initialize();

   // Verify time having a valid value
   Integer mjdate = yyyy*10000 + mmdd;
   if ((yyyymmddMin > mjdate)||(mjdate >= yyyymmddMax))
   {
      Integer year, month, day, md;

      year = yyyymmddMin/10000;
      md = yyyymmddMin - year*10000;
      month = md/100;
      day = md - month*100;
      std::string dateMin = GmatStringUtil::ToString(month) +
         "/" + GmatStringUtil::ToString(day) +
         "/" + GmatStringUtil::ToString(year);

      year = yyyymmddMax/10000;
      md = yyyymmddMax - year*10000;
      month = md/100;
      day = md - month*100;
      std::string dateMax = GmatStringUtil::ToString(month) +
         "/" + GmatStringUtil::ToString(day) +
         "/" + GmatStringUtil::ToString(year);

      throw MeasurementException("Error: Epoch is out of range. Time range for Ionosphere calculation is from "+ dateMin + " to " + dateMax + ".\n");
   }

   Real freq = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM / waveLength;
   Real tec = TEC();                  // Equation 6.70 of MONTENBRUCK and GILL      // unit: number of electrons/ m^2
   Real drho = 40.3*tec/(freq*freq);  // Equation 6.69 of MONTENBRUCK and GILL      // unit: meter

   // Unit of dphi has to be radian because in all caller functions use correction in radian unit.             // made changes by TUAN NGUYEN
   Real dphi = BendingAngle();                                                      // unit: radian            // made changes by TUAN NGUYEN
   Real dtime = drho/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM;                  // unit: s

#ifdef DEBUG_IONOSPHERE_CORRECTION
   MessageInterface::ShowMessage
      ("Ionosphere::Correction: freq = %.12lf MHz,  tec = %.12lfe16,  "
       "drho = %.12lf m, dphi = %.12lf degree, dtime = %.12lf s\n", freq/1.0e6,
       tec/1.0e16, drho, dphi*GmatMathConstants::DEG_PER_RAD, dtime);
#endif

   RealArray ra;
   ra.push_back(drho);
   ra.push_back(dphi);
   ra.push_back(dtime);
   
   return ra;
}

