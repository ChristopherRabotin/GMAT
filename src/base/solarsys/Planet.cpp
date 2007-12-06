//$Id$
//------------------------------------------------------------------------------
//                                  Planet
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan
// Created: 2004/01/29
//
/**
 * Implementation of the Planet class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Rmatrix.hpp"
#include "Planet.hpp"
#include "CoordUtil.hpp"
#include "MessageInterface.hpp"
#include "PhysicalConstants.hpp"
#include "RealUtilities.hpp"
#include "SolarSystemException.hpp"
#include "TimeTypes.hpp"
#include "TimeSystemConverter.hpp"
#include "AngleUtil.hpp"
#include "FileManager.hpp"
#include "StringUtil.hpp"

//#define DEBUG_PLANET 1
//#define DEBUG_PLANET_ANALYTIC

using namespace GmatMathUtil;

// initialize static default values
// default values for Planet data
const Gmat::BodyType        Planet::DEFAULT_BODY_TYPE         = Gmat::PLANET;
const Gmat::PosVelSource    Planet::DEFAULT_POS_VEL_SOURCE    = Gmat::DE_405; 
const Gmat::AnalyticMethod  Planet::DEFAULT_ANALYTIC_METHOD   = Gmat::LOW_FIDELITY;
const Integer               Planet::DEFAULT_BODY_NUMBER       = 1;
const Integer               Planet::DEFAULT_REF_BODY_NUMBER   = 3;

// Units for Equatorial radius are km
const Real                  Planet::EQUATORIAL_RADIUS[NumberOfPlanets]         =
{
   2.43970000000000e+003, // to match STK 2006.01.31 - was 2439.7,
   6.05190000000000e+003, // match to STK 2006.01.31 - was 6051.8,
   6.3781363E3, // to match STK 2006.01.31 - was 6378.1363,
   3.39700000000000e+003, // to match STK 2006.01.31 - was 3396.200,
   7.14920000000000e+004, // to match STK 2006.01.31 - was 71492.00,
   6.02680000000000e+004, // to match STK 2006.01.31 - was 60368.0,
   2.55590000000000e+004, // to match STK 2006.01.31 - was 25559.0,
   2.52690000000000e+004, // to match STK 2006.01.31 - was 24764.0,
   1162.0 // changed to match with STK. old:1195.0
};
const Real                  Planet::FLATTENING[NumberOfPlanets]         =   
{
   0.0,
   0.0,
   0.00335270, // match to STK 2006.01.31 - was 0.0033528,
   0.00647630, // match to STK 2006.01.31 - was 0.0064763,
   0.06487439, // match to STK 2006.01.31 - was 0.0648744,
   0.09796243, // match to STK 2006.01.31 - was 0.0979624,
   0.02292734, // match to STK 2006.01.31 - was 0.0229273,
   0.01856029, // match to STK 2006.01.31 - was 0.0171,
   0.0
};
// Units for Mu are km^3/s^2
const Real                  Planet::MU[NumberOfPlanets]                        =
{
   22032.080486418, 
   324858.59882646, 
   398600.4415,  
   42828.314258067, 
   126712767.85780, 
   37940626.061137,
   5794549.0070719, 
   6836534.0638793, 
   981.60088770700  
};
const Integer               Planet::ORDER[NumberOfPlanets]               =
                            {0, 0, 4, 0, 0, 0, 0, 0, 0};

const Integer               Planet::DEGREE[NumberOfPlanets]              =
                            {0, 0, 4, 0, 0, 0, 0, 0, 0};
const Rmatrix               Planet::SIJ[NumberOfPlanets]                 =
{
   Rmatrix(5,5,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0),
   Rmatrix(5,5,
           0.0,                  0.0,                  0.0,                  
           0.0,                  0.0,
           0.0,                  0.0,                  0.0,                  
           0.0,                  0.0,
           0.0, 1.47467423600000E-08,-9.53141845209000E-08,                  
           0.0,                  0.0,
           0.0, 5.40176936891000E-07, 8.11618282044000E-07, 
           2.11451354723000E-07, 0.0,
           0.0, 4.91465604098000E-07, 4.83653955909000E-07,
          -1.18564194898000E-07,      1.37586364127000E-06),
   Rmatrix(5,5,
           0.0,                  0.0,                  0.0,                  
           0.0,                  0.0,
           0.0,                  0.0,                  0.0,                  
           0.0,                  0.0,
           0.0, 1.19528010000000E-09,-1.40026639758800E-06,                  
           0.0,                  0.0,
           0.0, 2.48130798255610E-07,-6.18922846478490E-07, 
           1.41420398473540E-06, 0.0,
           0.0,-4.73772370615970E-07, 6.62571345942680E-07,
          -2.00987354847310E-07,      3.08848036903550E-07),
   Rmatrix(5,5,
           0.0,                  0.0,                  0.0,                  
           0.0,                  0.0,
           0.0,                  0.0,                  0.0,                  
           0.0,                  0.0,
           0.0, 6.54655690000000E-09, 4.90611750000000E-05,                  
           0.0,                  0.0,
           0.0, 2.52926620000000E-05, 8.31603630000000E-06, 
           2.55554990000000E-05, 0.0,
           0.0, 3.70906170000000E-06,-8.97764090000000E-06,
          -1.72832060000000E-07,     -1.28554120000000E-05),
   Rmatrix(5,5,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0),
   Rmatrix(5,5,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0),
   Rmatrix(5,5,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0),
   Rmatrix(5,5,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0),
   Rmatrix(5,5,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0),
};
const Rmatrix               Planet::CIJ[NumberOfPlanets]                 =
{
   Rmatrix(5,5,
                             1.0, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0,
           -2.68328157300000E-05, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0),
   Rmatrix(5,5,
                             1.0,                  0.0,                  0.0,
                             0.0,                  0.0,
                             0.0,                  0.0,                  0.0,
                             0.0,                  0.0,
           -1.95847963769000E-06, 2.87322988834000E-08, 8.52182522007000E-07,
                             0.0,                  0.0,
            7.98507258430000E-07, 2.34759127059000E-06,-9.45132723095000E-09,
           -1.84756674598000E-07,                  0.0,
            7.15385582249000E-07,-4.58736811774000E-07, 1.26875441402000E-07,
           -1.74034531883000E-07, 1.78438307106000E-07),
   Rmatrix(5,5,
                             1.0,                  0.0,                  0.0,
                             0.0,                   0.0,
                             0.0,                  0.0,                  0.0,
                             0.0,                   0.0,
           -4.84165374886470E-04,-1.86987640000000E-10, 2.43926074865630E-06,
                             0.0,                   0.0,
            9.57170590888000E-07, 2.03013720555300E-06, 9.04706341272910E-07, 
            7.21144939823090E-07,                   0.0,
            5.39777068357300E-07,-5.36243554298510E-07, 3.50670156459380E-07,
            9.90868905774410E-07,-1.88481367425270E-07),
   Rmatrix(5,5,
                             1.0,                  0.0,                  0.0,
                             0.0,                   0.0,
                             0.0,                  0.0,                  0.0,
                             0.0,                   0.0,
           -8.75977040000000E-04, 3.69395770000000E-09,-8.46829230000000E-05,
                             0.0,                   0.0,
           -1.19062310000000E-05, 3.74737410000000E-06,-1.59844090000000E-05, 
            3.51325710000000E-05,                   0.0,
            5.14919500000000E-06, 4.26122630000000E-06,-1.05467200000000E-06, 
            6.47421510000000E-06, 2.97350070000000E-07),
   Rmatrix(5,5,
                             1.0, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0,
           -6.59640053360000E-03, 0.0,             0.0,             0.0,
                             0.0,
            2.19219394350000E-04, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0),
   Rmatrix(5,5,
                             1.0, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0,
           -7.35666364600000E-03, 0.0,             0.0,             0.0,
                             0.0,
           -3.77964473010000E-04, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0),
   Rmatrix(5,5,
                             1.0, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0,
           -5.36656314600000E-03, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0),
   Rmatrix(5,5,
                             1.0, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0,
           -1.78885438200000E-03, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0,
                             0.0, 0.0,             0.0,             0.0,
                             0.0),
   Rmatrix(5,5,
           1.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0),
};
const Real                  Planet::ANALYTIC_EPOCH[NumberOfPlanets]      =
{
   21544.500370768266, 21544.500370768266, 21544.500370768266,
   21544.500370768266, 21544.500370768266, 21544.500370768266,
   21544.500370768266, 21544.500370768266, 21544.500370768266
};
const Rvector6              Planet::ANALYTIC_ELEMENTS[NumberOfPlanets]   =
{
   Rvector6(57909212.938567216, 0.20562729774965544, 28.551674963293556,
            10.99100758149257, 67.548689584103984,  175.10396761800456),
   Rvector6(108208423.76486244, 0.0067572911404369688, 24.433051334216176,
            8.007373221205856, 124.55871063212626,     49.889845117140576),
   Rvector6(149653978.9783766,        0.01704556707314489, 23.439034090426388,
            0.00018646554487906264, 101.7416388084352,    358.12708491129), 
   Rvector6(227939100.16983532,   0.093314935483163344, 24.677089965042784,
            3.3736838414054472, 333.01849018562076,     23.020633424007744),
   Rvector6(779362950.5867208, 0.049715759324379896, 23.235170252934984,
            3.253166212922,   12.959463238924978,    20.296667207322848),
   Rvector6(1433895241.1645338,  0.055944006117351672, 22.551333377462712, 
            5.9451029086964872, 83.977808941927856,   316.23400767222348),
   Rvector6(2876804054.239868,   0.044369079419761096, 23.663364175915172,
            1.850441916938424, 168.86875273062818,    145.8502865552013),
   Rvector6(4503691751.2342816, 0.011211871260687014, 22.29780590076114,
            3.47555654789392,  33.957145210261132,   266.76236610390636),
   Rvector6(5909627293.567856,    0.24928777871911536, 23.4740184346088,
            43.998303104440304, 183.03164997859696,    25.513664216653164)
};

//---------------------------------
// static data
//---------------------------------
const std::string
Planet::PARAMETER_TEXT[PlanetParamCount - CelestialBodyParamCount] =
{
   "NutationUpdateInterval",
};

const Gmat::ParameterType
Planet::PARAMETER_TYPE[PlanetParamCount - CelestialBodyParamCount] =
{
   Gmat::REAL_TYPE,
};



//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Planet(std::string name)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Planet class
 * (default constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body (default is "Sun").
 */
//------------------------------------------------------------------------------
Planet::Planet(std::string name) :
   CelestialBody     ("Planet",name),
   nutationUpdateInterval    (60.0)
//   updateInterval    (60.0)
{   
   objectTypeNames.push_back("Planet");
   InitializePlanet(SolarSystem::SUN_NAME);  
   parameterCount = PlanetParamCount;
}

//------------------------------------------------------------------------------
//  Planet(std::string name, const std::string &cBody)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Planet class
 * (constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body.
 * @param <cBody> pointer to a central body.
 */
//------------------------------------------------------------------------------
Planet::Planet(std::string name, const std::string &cBody) :
   CelestialBody     ("Planet",name),
   nutationUpdateInterval    (60.0)
//   updateInterval    (60.0)
{
   objectTypeNames.push_back("Planet");
   InitializePlanet(cBody); 
   parameterCount = PlanetParamCount;
}

//------------------------------------------------------------------------------
//  Planet(const Planet &pl)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Planet class as a copy of the
 * specified Planet class (copy constructor).
 *
 * @param <pl> Planet object to copy.
 */
//------------------------------------------------------------------------------
Planet::Planet(const Planet &pl) :
   CelestialBody  (pl),
   nutationUpdateInterval (pl.nutationUpdateInterval)
//   updateInterval (pl.updateInterval)
{
}

//------------------------------------------------------------------------------
//  Planet& operator= (const Planet& pl)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Planet class.
 *
 * @param <pl> the Planet object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" Planet with data of input Planet st.
 */
//------------------------------------------------------------------------------
Planet& Planet::operator=(const Planet &pl)
{
   if (&pl == this)
      return *this;

   CelestialBody::operator=(pl);
   nutationUpdateInterval = pl.nutationUpdateInterval;
//   updateInterval = pl.updateInterval;
   return *this;
}

//------------------------------------------------------------------------------
//  ~Planet()
//------------------------------------------------------------------------------
/**
 * Destructor for the Planet class.
 */
//------------------------------------------------------------------------------
Planet::~Planet()
{
}

//------------------------------------------------------------------------------
//  Rvector GetBodyCartographicCoordinates(const A1Mjd &forTime) const
//------------------------------------------------------------------------------
/**
 * This method returns the cartographic coordinates for the planet.
 *
 * @param <forTime> time for which to compute the cartographic coordinates.
 *
 * @return vector containing alpha, delta, W, Wdot.
 *
 * @note currently only implemented for the (currently known) planets of our
 *       Solar System.  See "Report of the IAU/IAG Working Group on
 *       Cartographic Coordinates and Rotational Elements of the Planets
 *       and Satellites: 2000"
 */
//------------------------------------------------------------------------------
Rvector Planet::GetBodyCartographicCoordinates(const A1Mjd &forTime) const
{
   Real alpha = 0;
   Real delta = 0;
   Real W     = 0;
   Real Wdot  = 0.0; 
   Real d = GetJulianDaysFromTCBEpoch(forTime); // interval in Julian days
   Real T = d / 36525;                        // interval in Julian centuries
   if (instanceName == SolarSystem::MERCURY_NAME)
   {
      alpha = 281.01  - 0.033 * T;
      delta = 61.45   - 0.005 * T;
      W     = 329.548 + 6.1385025 * d;
      Wdot  = 6.1385025 * CelestialBody::dDot;
   }
   else if (instanceName == SolarSystem::VENUS_NAME)
   {
      alpha = 272.76;
      delta = 67.16;
      W     = 160.20 - 1.4813688 * d;
      Wdot = - 1.4813688 * CelestialBody::dDot;
   }
   else if (instanceName == SolarSystem::EARTH_NAME)
   {
      alpha = 0.00    - 0.641 * T;
      delta = 90.00   - 0.557 * T;
      W     = 190.147 + 360.9856235 * d;
      Wdot  = 360.9856235 * CelestialBody::dDot;
   }
   else if (instanceName == SolarSystem::MARS_NAME)
   {
      alpha = 317.68143 - 0.1061 * T;
      delta = 52.88650  - 0.0609 * T;
      W     = 176.630   + 350.89198226 * d;
      Wdot = 350.89198226 * CelestialBody::dDot;
   }
   else if (instanceName == SolarSystem::JUPITER_NAME)
   {
      alpha = 268.05 - 0.009 * T;
      delta = 64.49  + 0.003 * T;
      W     = 284.95 + 870.5366420 * d;
      Wdot  = 870.5366420 * CelestialBody::dDot;
   }
   else if (instanceName == SolarSystem::SATURN_NAME)
   {
      alpha = 40.589 - 0.036 * T;
      delta = 83.537 - 0.004 * T;
      W     = 38.90  + 810.7939024 * d;
      Wdot  = 810.7939024 * CelestialBody::dDot;
   }
   else if (instanceName == SolarSystem::URANUS_NAME)
   {
      alpha = 257.311;
      delta = -15.175;
      W     = 203.81 - 501.1600928 * d;
      Wdot  = - 501.1600928 * CelestialBody::dDot;
   }
   else if (instanceName == SolarSystem::NEPTUNE_NAME)
   {
      Real N    = 357.85 + 52.316 * T;
      //Real NDot = 52.316 * CelestialBody::TDot;
      Real NDot = 6.0551e-04;   // per new specs 2004.02.22
      alpha     = 299.36 + 0.70 * Sin(Rad(N));
      delta     = 43.46  - 0.51 * Cos(Rad(N));
      W         = 253.18 + 536.3128492 * d - 0.48 * Sin(Rad(N));
      Wdot      = 536.3128492 * CelestialBody::dDot - 0.48 * NDot * Cos(Rad(N));
   }
   else if (instanceName == SolarSystem::PLUTO_NAME)
   {
      alpha = 313.02;
      delta = 9.09;
      W     = 236.77 - 56.3623195 * d;
      Wdot  = - 56.3623195 * CelestialBody::dDot;
   }
   else
   {
      return CelestialBody::GetBodyCartographicCoordinates(forTime);
   }
   
   return Rvector(4, alpha, delta, W, Wdot);
}

//------------------------------------------------------------------------------
//  Real GetHourAngle(A1Mjd atTime)
//------------------------------------------------------------------------------
/**
 * This method returns the hour angle for the body, referenced from the
 * Prime Meridian, measured westward
 *
 * @param <atTime> time for which to compute the hour angle
 *
 * @return hour angle for the body, in degrees, from the Prime Meridian
 *
 * @note algorithm 15, Vallado p. 192
 * @todo move this to Planet?  Add generic calculation here.
 *
 */
//------------------------------------------------------------------------------
Real  Planet::GetHourAngle(A1Mjd atTime) 
{
   if (instanceName == SolarSystem::EARTH_NAME)
   {
      // Convert the time to a UT1 MJD
      // 20.02.06 - arg: changed to use enum types instead of strings
//      Real mjdUT1 = TimeConverterUtil::Convert(atTime.Get(),
//                                               "A1Mjd", "Ut1Mjd", GmatTimeUtil::JD_JAN_5_1941);
      Real mjdUT1 = TimeConverterUtil::Convert(atTime.Get(),
                                 TimeConverterUtil::A1MJD, TimeConverterUtil::UT1MJD,
                                 GmatTimeUtil::JD_JAN_5_1941);
      Real jdUT1    = mjdUT1 + GmatTimeUtil::JD_JAN_5_1941; // right?
                                                            // Compute elapsed Julian centuries (UT1)
      Real tUT1     = (jdUT1 - 2451545.0) / 36525.0;
      //Real timeUt1  = (ut1Jd - 2451545.0) / 36525;
      
      // compute mean sidereal time, in degrees
      // according to Vallado Eq. 3-45, converted to degrees, where
      // 1 hour = 15 degrees and 1 second = 1/240 of a second
      
      Real mst        = (67310.54841 / 240) +
         (((876600 * 15) + (8640184.812866 / 240)) * tUT1) +
         ((0.093104 / 240) * tUT1 * tUT1) -
         ((6.2e-06 / 240) * tUT1 * tUT1 * tUT1);
      
      // reduce to a quantity within one day (86400 seconds, 360.0 degrees)
      hourAngle = AngleUtil::PutAngleInDegRange(mst,0.0,360.0);
   }
   else
   {
      Real d = GetJulianDaysFromTCBEpoch(atTime); // interval in Julian days
      Real T = d / 36525;                        // interval in Julian centuries
      if (instanceName == SolarSystem::MERCURY_NAME)
      {
         hourAngle     = 329.548 + 6.1385025 * d;
      }
      else if (instanceName == SolarSystem::VENUS_NAME)
      {
         hourAngle     = 160.20 - 1.4813688 * d;
      }
      //else if (instanceName == SolarSystem::EARTH_NAME)
      //{
      //   hourAngle     = 190.147 + 360.9856235 * d;
      //}
      else if (instanceName == SolarSystem::MARS_NAME)
      {
         hourAngle     = 176.630   + 350.89198226 * d;
      }
      else if (instanceName == SolarSystem::JUPITER_NAME)
      {
         hourAngle     = 284.95 + 870.5366420 * d;
      }
      else if (instanceName == SolarSystem::SATURN_NAME)
      {
         hourAngle     = 38.90  + 810.7939024 * d;
      }
      else if (instanceName == SolarSystem::URANUS_NAME)
      {
         hourAngle     = 203.81 - 501.1600928 * d;
      }
      else if (instanceName == SolarSystem::NEPTUNE_NAME)
      {
         Real N        = 357.85 + 52.316 * T;
         hourAngle     = 253.18 + 536.3128492 * d - 0.48 * Sin(Rad(N));
      }
      else if (instanceName == SolarSystem::PLUTO_NAME)
      {
         hourAngle     = 236.77 - 56.3623195 * d;
      }
      else
      {
         return CelestialBody::GetHourAngle(atTime);
      }
      // reduce to a quantity within one day (86400 seconds, 360.0 degrees)
      hourAngle = AngleUtil::PutAngleInDegRange(hourAngle,0.0,360.0);
   }
   
   return hourAngle; 
}


//------------------------------------------------------------------------------
//  bool SetAnalyticEpoch(const A1Mjd &toTime)
//------------------------------------------------------------------------------
/**
 * This method sets the epoch to be used for Low Fidelity analytic modeling.
 *
 * @param <toTime> epoch
 *
 * @return success of the operation.
 *
 */
//------------------------------------------------------------------------------
bool Planet::SetAnalyticEpoch(const A1Mjd &toTime)
{
   #ifdef DEBUG_PLANET_ANALYTIC
      MessageInterface::ShowMessage(
      "In Planet::SetAnalyticEpoch with time = %.12f\n", toTime.Get());
   #endif
   if (!CelestialBody::SetAnalyticEpoch(toTime)) return false;
   bool OK = true;
   // For the Earth, send the information to the Sun
   if (instanceName == SolarSystem::EARTH_NAME)
   {
      if (!theCentralBody) 
         throw SolarSystemException("Central body must be set for " 
                                    + instanceName);
      #ifdef DEBUG_PLANET_ANALYTIC
         MessageInterface::ShowMessage(
         "-------- and setting central body's epoch time to %.12f\n", 
         toTime.Get());
      #endif
      OK = theCentralBody->SetAnalyticEpoch(toTime);
   }
   return OK;
}

//------------------------------------------------------------------------------
//  bool SetAnalyticElements(const Rvector6 &kepl)
//------------------------------------------------------------------------------
/**
 * This method sets the elements to be used for Low Fidelity analytic modeling.
 *
 * @param <kepl> initial keplerian elements.
 *
 * @return success of the operation.
 *
 */
//------------------------------------------------------------------------------
bool Planet::SetAnalyticElements(const Rvector6 &kepl)
{
   #ifdef DEBUG_PLANET_ANALYTIC
      MessageInterface::ShowMessage(
      "In Planet::SetAnalyticElements, kepl = \n%.12f %.12f %.12f %.12f %.12f %.12f\n", 
      kepl[0], kepl[1], kepl[2], kepl[3], kepl[4], kepl[5]);
   #endif
   if (!CelestialBody::SetAnalyticElements(kepl)) return false;
   bool OK = true;
   // For the Earth, send the information to the Sun
   if (instanceName == SolarSystem::EARTH_NAME)
   {
      if (!theCentralBody) 
         throw SolarSystemException("Central body must be set for " 
                                    + instanceName);
      Real     ma;
      Real     totalMu = mu + theCentralBody->GetGravitationalConstant();
      Rvector6 cart    = - (CoordUtil::KeplerianToCartesian(kepl,
                            totalMu, CoordUtil::TA)); 
      Rvector6 sunKepl = CoordUtil::CartesianToKeplerian(cart, totalMu, &ma);

      #ifdef DEBUG_PLANET_ANALYTIC
         MessageInterface::ShowMessage(
         "-------- and setting central body's elements to \n %.12f %.12f %.12f %.12f %.12f %.12f\n", 
         sunKepl[0], sunKepl[1], sunKepl[2], 
         sunKepl[3], sunKepl[4], sunKepl[5]);
      #endif
      OK = theCentralBody->SetAnalyticElements(sunKepl);
   }
   return OK;
}

Real Planet::GetNutationUpdateInterval() const 
{
   return nutationUpdateInterval;
//   return updateInterval;
}

bool Planet::SetNutationUpdateInterval(Real val)
{
   if (val < 0.0)
   {
      SolarSystemException sse;
      sse.SetDetails(errorMessageFormat.c_str(),
                     GmatStringUtil::ToString(val, GetDataPrecision()).c_str(),
                     "NutationUpdateInterval", "Real Number >= 0.0");
      throw sse;
   }
   
   nutationUpdateInterval = val;
   return true;
//   if (val < 0.0) return false;
//   updateInterval = val;
//   return true;
}



//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Planet.
 *
 * @return clone of the Planet.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Planet::Clone(void) const
{
   return (new Planet(*this));
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Planet::GetParameterText(const Integer id) const
{
   if (id >= CelestialBodyParamCount && id < PlanetParamCount)
      return PARAMETER_TEXT[id - CelestialBodyParamCount];
   return CelestialBody::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
* This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer Planet::GetParameterID(const std::string &str) const
{
   for (Integer i = CelestialBodyParamCount; i < PlanetParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - CelestialBodyParamCount])
         return i;
   }
   
   return CelestialBody::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Planet::GetParameterType(const Integer id) const
{
   if (id >= CelestialBodyParamCount && id < PlanetParamCount)
      return PARAMETER_TYPE[id - CelestialBodyParamCount];
   
   return CelestialBody::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Planet::GetParameterTypeString(const Integer id) const
{
   return CelestialBody::PARAM_TYPE_STRING[GetParameterType(id)];
}


bool Planet::IsParameterReadOnly(const Integer id) const
{
//   if (id == UPDATE_INTERVAL)
   if (id == NUTATION_UPDATE_INTERVAL)
      return false;
   return CelestialBody::IsParameterReadOnly(id);
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the real value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Planet::GetRealParameter(const Integer id) const
{
//   if (id == UPDATE_INTERVAL) return updateInterval;
   if (id == NUTATION_UPDATE_INTERVAL) return nutationUpdateInterval;
   return CelestialBody::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value) 
//------------------------------------------------------------------------------
/**
* This method sets the real value, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 * @param value to use to set the parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Planet::SetRealParameter(const Integer id, const Real value)
{
//   if (id == UPDATE_INTERVAL)
//   {
//      updateInterval = value;
//      return true;
//   }
   if (id == NUTATION_UPDATE_INTERVAL)
   {
        SetNutationUpdateInterval(value);
      return true;
   }
   return CelestialBody::SetRealParameter(id,value);
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* This method returns the real value, given the input parameter label.
 *
 * @param label label for the requested parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Planet::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const std::string &label, const Real value) 
//------------------------------------------------------------------------------
/**
* This method sets the real value, given the input parameter label.
 *
 * @param label label for the requested parameter.
 * @param value to use to set the parameter.
 *
 * @return real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real Planet::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  void  InitializePlanet(const std::string &cBody)
//------------------------------------------------------------------------------
/**
 * This method initializes the data values for the body.
 *
 * @param <cBody> central body (default is NULL).
 *
 */
//------------------------------------------------------------------------------
void Planet::InitializePlanet(const std::string &cBody)
{
   CelestialBody::InitializeBody();

   Integer bodyIndex = -1;
   
   // fill in with default values, use Earth values for Earth and unheard-of
   // planets
   theCentralBodyName  = cBody;
   bodyType            = Planet::DEFAULT_BODY_TYPE;
   posVelSrc           = Planet::DEFAULT_POS_VEL_SOURCE;
   analyticMethod      = Planet::DEFAULT_ANALYTIC_METHOD;
   bodyNumber          = Planet::DEFAULT_BODY_NUMBER;
   referenceBodyNumber = Planet::DEFAULT_REF_BODY_NUMBER;
   rotationSrc         = Gmat::IAU_DATA;

   if (instanceName == SolarSystem::MERCURY_NAME)      bodyIndex = MERCURY;
   else if (instanceName == SolarSystem::VENUS_NAME)   bodyIndex = VENUS;
   else if (instanceName == SolarSystem::EARTH_NAME)   bodyIndex = EARTH;
   else if (instanceName == SolarSystem::MARS_NAME)    bodyIndex = MARS;
   else if (instanceName == SolarSystem::JUPITER_NAME) bodyIndex = JUPITER;
   else if (instanceName == SolarSystem::SATURN_NAME)  bodyIndex = SATURN;
   else if (instanceName == SolarSystem::URANUS_NAME)  bodyIndex = URANUS;
   else if (instanceName == SolarSystem::NEPTUNE_NAME) bodyIndex = NEPTUNE;
   else if (instanceName == SolarSystem::PLUTO_NAME)   bodyIndex = PLUTO;
   else    // for Earth and other(?) planets, use Earth defaults
   {
      bodyIndex = EARTH;
      std::string errMsg =  "Unknown planet created - please supply ";
      errMsg            +=  "potential file or physical parameter values\n";
      MessageInterface::ShowMessage(errMsg);
   }
   if (bodyIndex == MERCURY)
   {
      models[Gmat::GRAVITY_FIELD].push_back("mercury2x0");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (bodyIndex == VENUS)
   {
      models[Gmat::GRAVITY_FIELD].push_back("mgn75hsaap");
      models[Gmat::GRAVITY_FIELD].push_back("shg120");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (bodyIndex == EARTH)
   {
      models[Gmat::ATMOSPHERE_MODEL].push_back("Exponential");
      models[Gmat::ATMOSPHERE_MODEL].push_back("MSISE90");
      models[Gmat::ATMOSPHERE_MODEL].push_back("Jacchia-Roberts");
      
      models[Gmat::GRAVITY_FIELD].push_back("JGM2");
      models[Gmat::GRAVITY_FIELD].push_back("JGM3");
      models[Gmat::GRAVITY_FIELD].push_back("EGM96");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
      rotationSrc   = Gmat::NOT_APPLICABLE;
      
      // for now, magnetic field remains empty
   }
   else if (bodyIndex == MARS)
   {
      models[Gmat::GRAVITY_FIELD].push_back("GMM-1");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (bodyIndex == JUPITER)
   {
      models[Gmat::GRAVITY_FIELD].push_back("jupiter6x2");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (bodyIndex == SATURN)
   {
      models[Gmat::GRAVITY_FIELD].push_back("saturn8x0");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (bodyIndex == URANUS)
   {
      models[Gmat::GRAVITY_FIELD].push_back("uranus4x0");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (bodyIndex == NEPTUNE)
   {
      models[Gmat::GRAVITY_FIELD].push_back("neptune4x0");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (bodyIndex == PLUTO)
   {
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }

   //loj: 3/23/06 set default potential file name from the startup file.
   try
   {
      FileManager *fm = FileManager::Instance();
      std::string potfile = GmatStringUtil::ToUpper(instanceName) + "_POT_FILE";
      std::string filename = fm->GetFullPathname(potfile);
      
      #if DEBUG_PLANET
      MessageInterface::ShowMessage
         ("Planet::InitializePlanet() body=%s, potfilename=%s\n", instanceName.c_str(),
          filename.c_str());
      #endif
      
      potentialFileName = filename;
   }
   catch (GmatBaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage());
   }

   // Set use potential file to true if Earth
   //if (bodyIndex == EARTH)
      usePotentialFile = false;  // wcs - 2007.05.11  changed to false per S. Hughes
   //else
    //  usePotentialFile = false;
      
   mu                  = Planet::MU[bodyIndex];
   mass                = mu / 
                         GmatPhysicalConst::UNIVERSAL_GRAVITATIONAL_CONSTANT;
   equatorialRadius    = Planet::EQUATORIAL_RADIUS[bodyIndex];
   flattening          = Planet::FLATTENING[bodyIndex];
   polarRadius         = (1.0 - flattening) * equatorialRadius;

   order               = Planet::ORDER[bodyIndex];
   degree              = Planet::DEGREE[bodyIndex];
   sij                 = Planet::SIJ[bodyIndex];
   cij                 = Planet::CIJ[bodyIndex];
   
   analyticEpoch       = Planet::ANALYTIC_EPOCH[bodyIndex];
   analyticKepler      = Planet::ANALYTIC_ELEMENTS[bodyIndex];


   // set defaults in case use chooses to go back to not using potential file
   defaultMu           = mu;
   defaultEqRadius     = equatorialRadius;
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

