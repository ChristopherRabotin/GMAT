//$Id$
//------------------------------------------------------------------------------
//                                  Moon
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan
// Created: 2004/01/29
//
/**
 * Implementation of the Moon class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "Moon.hpp"
#include "PhysicalConstants.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"
#include "FileManager.hpp"
#include "StringUtil.hpp"

//#define DEBUG_MOON 1

using namespace GmatMathUtil;


// initialize static default values
// default values for CelesitalBody data
const Gmat::BodyType        Moon::BODY_TYPE           = Gmat::MOON;
const Gmat::PosVelSource    Moon::POS_VEL_SOURCE      = Gmat::DE_405; 
const Gmat::AnalyticMethod  Moon::ANALYTIC_METHOD     = Gmat::LOW_FIDELITY; 
const Integer               Moon::ORDER               = 0; 
const Integer               Moon::DEGREE              = 0;  
 // 2006.01.31 Equatorial radius - to match STK; was 1738.1; 
const Real                  Moon::LUNA_EQUATORIAL_RADIUS   = 1738.2000;// km
const Real                  Moon::LUNA_FLATTENING          = 0.0;
// Units for mu are km^3 / s^2
const Real                  Moon::LUNA_MU                  = 4902.8005821478;
const Integer               Moon::LUNA_BODY_NUMBER         = 2; 
const Integer               Moon::LUNA_REF_BODY_NUMBER     = 3; 

const Rmatrix               Moon::LUNA_SIJ                 = Rmatrix(5,5,
   0.0,                  0.0,                  0.0,                  0.0,
   0.0,
   0.0,                  0.0,                  0.0,                  0.0,
   0.0,
   0.0, 4.78976286742000E-09, 1.19043314469000E-08,                  0.0,
   0.0,
   0.0, 5.46564929895000E-06, 4.88875341590000E-06,-1.76416063010000E-06,
   0.0,
   0.0, 1.63304293851000E-06,-6.76012176494000E-06,-1.34287028168000E-05,
   3.94334642990000E-06);
const Rmatrix               Moon::LUNA_CIJ                 = Rmatrix(5,5,
                     1.0,                 0.0,                  0.0,
                     0.0,                 0.0,
                     0.0,                 0.0,                  0.0,
                     0.0,                 0.0,
   -9.09314486280000E-05, 9.88441569067000E-09, 3.47139237760000E-05,
                     0.0,                 0.0,
   -3.17765981183000E-06, 2.63497832935000E-05, 1.42005317544000E-05,
    1.22860504604000E-05,                 0.0,
    3.21502582986000E-06,-6.01154071094000E-06,-7.10667037450000E-06,
   -1.37041711834000E-06,-6.03652719918000E-06);

const Real                  Moon::ANALYTIC_EPOCH      = 21544.500370768266;
const Rvector6              Moon::ANALYTIC_ELEMENTS   = Rvector6(
      385494.90434829952,  0.055908943292024992,   20.940245433093748,
      12.233244412716252, 68.004298803147648,     137.94325682926458);


/// @todo add other ones as needed

//---------------------------------
// static data
//---------------------------------
//const std::string
//Moon::PARAMETER_TEXT[MoonParamCount - CelestialBodyParamCount] =
//{
//  
//};
//
//const Gmat::ParameterType
//Moon::PARAMETER_TYPE[MoonParamCount - CelestialBodyParamCount] =
//{
//   
//};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Moon(std::string name)
//------------------------------------------------------------------------------
/**
* This method creates an object of the Moon class
 * (default constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body (default is "Earth").
 */
//------------------------------------------------------------------------------
Moon::Moon(std::string name) :
CelestialBody     ("Moon",name)
{
   objectTypeNames.push_back("Moon");
   InitializeMoon(SolarSystem::EARTH_NAME);  
   parameterCount = MoonParamCount;
}

//------------------------------------------------------------------------------
//  Moon(std::string name, const std::string &cBody)
//------------------------------------------------------------------------------
/**
* This method creates an object of the Moon class
 * (constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body.
 * @param <cBody> pointer to a central body.
 */
//------------------------------------------------------------------------------
Moon::Moon(std::string name, const std::string &cBody) :
CelestialBody     ("Moon",name)
{
   objectTypeNames.push_back("Moon");
   InitializeMoon(cBody); 
   parameterCount = MoonParamCount;
}

//------------------------------------------------------------------------------
//  Moon(const Moon &m)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Moon class as a copy of the
 * specified Moon class (copy constructor).
 *
 * @param <m> Moon object to copy.
 */
//------------------------------------------------------------------------------
Moon::Moon(const Moon &m) :
CelestialBody (m)

{
}

//------------------------------------------------------------------------------
//  Moon& operator= (const Moon& m)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Moon class.
 *
 * @param <m> the Moon object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" Moon with data of input Moon st.
 */
//------------------------------------------------------------------------------
Moon& Moon::operator=(const Moon &m)
{
   if (&m == this)
      return *this;

   CelestialBody::operator=(m);
   return *this;
}

//------------------------------------------------------------------------------
//  ~Moon()
//------------------------------------------------------------------------------
/**
 * Destructor for the Moon class.
 */
//------------------------------------------------------------------------------
Moon::~Moon()
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
 * @note currently only implemented for the moons (listed in IAU doc) of our
 *       Solar System.  See "Report of the IAU/IAG Working Group on
 *       Cartographic Coordinates and Rotational Elements of the Planets
 *       and Satellites: 2000"
 */
//------------------------------------------------------------------------------
Rvector Moon::GetBodyCartographicCoordinates(const A1Mjd &forTime) const
{
   Real alpha = 0;
   Real delta = 0;
   Real W     = 0;
   Real Wdot  = 0.0;
   // intermediate angle computations, in radians (for trig functions)
   Real p1  = 0.0, p2  = 0.0, p3  = 0.0, p4  = 0.0;
   Real p5  = 0.0, p6  = 0.0, p7  = 0.0, p8  = 0.0;
   Real p9  = 0.0, p10 = 0.0, p11 = 0.0, p12 = 0.0;
   Real p13 = 0.0;
   // Real p14 = 0.0, p15 = 0.0, p16 = 0.0;
   Real d = GetJulianDaysFromTCBEpoch(forTime); // interval in Julian days
   Real T = d / 36525;                          // interval in Julian centuries
   // Compute for Eath's Moon
   if (theCentralBodyName == SolarSystem::EARTH_NAME) 
   {
      p1  = Rad(125.045 -  0.0529921 * d);
      p2  = Rad(250.089 -  0.1059842 * d);
      p3  = Rad(260.008 + 13.0120009 * d);
      p4  = Rad(176.625 + 13.3407154 * d);
      p5  = Rad(357.529 +  0.9856003 * d);
      p6  = Rad(311.589 + 26.4057084 * d);
      p7  = Rad(134.963 + 13.0649930 * d);
      p8  = Rad(276.617 +  0.3287146 * d);
      p9  = Rad( 34.226 +  1.7484877 * d);
      p10 = Rad( 15.134 -  0.1589763 * d);
      p11 = Rad(119.743 +  0.0036096 * d);
      p12 = Rad(239.961 +  0.1643573 * d);
      p13 = Rad( 25.053 + 12.9590088 * d);
      
      alpha = 269.9949  
             +  0.0031 * T        -  3.8787 * Sin(p1)  -  0.1204 * Sin(p2)
             +  0.0700 * Sin(p3)  -  0.0172 * Sin(p4)  +  0.0072 * Sin(p6)
             -  0.0052 * Sin(p10) +  0.0043 * Sin(p13);
      delta =  66.5392   
             +  0.0130 * T        +  1.5419 * Cos(p1)  +  0.0239 * Cos(p2)
             -  0.0278 * Cos(p3)  +  0.0068 * Cos(p4)  -  0.0029 * Cos(p6)
             +  0.0009 * Cos(p7)  +  0.0008 * Cos(p10) -  0.0009 * Cos(p13);
      W     =  38.3213 
             + 13.17635815 * d    -  1.4E-12 * d * d   +  3.5610 * Sin(p1)
             +  0.1208 * Sin(p2)  -  0.0642 * Sin(p3)  +  0.0158 * Sin(p4)
             -  0.0252 * Sin(p5)  -  0.0066 * Sin(p6)  -  0.0047 * Sin(p7)
             -  0.0046 * Sin(p8)  +  0.0028 * Sin(p9)  +  0.0052 * Sin(p10)
             +  0.0040 * Sin(p11) +  0.0019 * Sin(p12) -  0.0044 * Sin(p13);
      Wdot  =  13.17635815
             -  2.8e-12 *    d     -  0.18870 * Cos(p1)
             -  0.01280 * Cos(p2)  -  0.835   * Cos(p3) +  0.211   * Cos(p4)
             +  0.0248  * Cos(p5)  -  0.17    * Cos(p6) -  0.061   * Cos(p7)
             -  0.0015  * Cos(p8)  +  0.0049  * Cos(p9) -  0.00083 * Cos(p10)
             +  0.00001 * Cos(p11) +  0.00031 * Cos(p12) - 0.057 * Cos(p13);
   }
   // Compute for Mars' moons
   else if (theCentralBodyName == SolarSystem::MARS_NAME)
   {
      p1  = Rad(169.51  -    0.4357640 * d);
      p2  = Rad(192.93  + 1128.4096700 * d +  8.864 * T * T);
      p3  = Rad( 53.47  -    0.0181510 * d);
      if (instanceName == SolarSystem::PHOBOS_NAME)
      {
         alpha = 317.68           -    0.108 * T     +  1.79 * Sin(p1);
         delta =  52.90           -    0.061 * T     -  1.08 * Cos(p1);
         W     =  35.06           + 1128.8445850 * d +  8.864 * T * T  
                 - 1.42 * Sin(p1) -    0.78 * Sin(p2);
         Wdot  = 0.0;
         MessageInterface::ShowMessage("Wdot not yet computed for Phobos\n");
      }
      else  // Deimos
      {
         alpha = 316.65           -    0.108 * T     +  2.98 * Sin(p3);
         delta =  53.52           -    0.061 * T     -  1.78 * Cos(p3);
         W     =  79.41           +  285.1618970 * d -  0.520 * T * T  
                 - 2.58 * Sin(p3) +    0.19 * Cos(p3);
         Wdot  = 0.0;
         MessageInterface::ShowMessage("Wdot not yet computed for Deimos\n");
      }
   }
   /// @todo add others when needed 
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
Real  Moon::GetHourAngle(A1Mjd atTime) 
{
   try
   {
      Rvector cart = GetBodyCartographicCoordinates(atTime);
      hourAngle = cart[2];  
      // reduce to a quantity within one day (86400 seconds, 360.0 degrees)
      hourAngle = AngleUtil::PutAngleInDegRange(hourAngle,0.0,360.0);
      return hourAngle;
   }
   catch (SolarSystemException &sse)
   {
      return CelestialBody::GetHourAngle(atTime);
   }
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Moon.
 *
 * @return clone of the Moon.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Moon::Clone(void) const
{
   return (new Moon(*this));
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  void  InitializeMoon(const std::string &cBody)
//------------------------------------------------------------------------------
/**
 * This method initializes the data values for the body.
 *
 * @param <cBody> central body (default is NULL).
 *
 */
//------------------------------------------------------------------------------
void Moon::InitializeMoon(const std::string &cBody)
{
   CelestialBody::InitializeBody();
   
   // fill in with default values, for the Sun
   bodyType            = Moon::BODY_TYPE;
   posVelSrc           = Moon::POS_VEL_SOURCE;
   analyticMethod      = Moon::ANALYTIC_METHOD;
   theCentralBodyName  = cBody;

   order               = Moon::ORDER;
   degree              = Moon::DEGREE;
   
   analyticEpoch       = Moon::ANALYTIC_EPOCH;
   analyticKepler      = Moon::ANALYTIC_ELEMENTS;
   rotationSrc         = Gmat::IAU_DATA;

   if (instanceName == SolarSystem::MOON_NAME)
   {
      mu                  = Moon::LUNA_MU;
      mass                = mu /
         GmatPhysicalConst::UNIVERSAL_GRAVITATIONAL_CONSTANT;
      // mass                = Moon::MASS;
      equatorialRadius    = Moon::LUNA_EQUATORIAL_RADIUS;
      flattening          = Moon::LUNA_FLATTENING;
      polarRadius         = (1.0 - flattening) * equatorialRadius;
      bodyNumber          = Moon::LUNA_BODY_NUMBER;
      referenceBodyNumber = Moon::LUNA_REF_BODY_NUMBER;
      sij                 = Moon::LUNA_SIJ;
      cij                 = Moon::LUNA_CIJ;
      
      defaultMu           = Moon::LUNA_MU;
      defaultEqRadius     = Moon::LUNA_EQUATORIAL_RADIUS;
      models[Gmat::GRAVITY_FIELD].push_back("LP100");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
      rotationSrc         = Gmat::DE_FILE;
   }
   else if (instanceName == SolarSystem::AMALTHEA_NAME)
   {
      models[Gmat::GRAVITY_FIELD].push_back("amalthea6x6");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (instanceName == SolarSystem::CALLISTO_NAME)
   {
      models[Gmat::GRAVITY_FIELD].push_back("callisto2x2");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (instanceName == SolarSystem::EUROPA_NAME)
   {
      models[Gmat::GRAVITY_FIELD].push_back("europa2x2");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (instanceName == SolarSystem::GANYMEDE_NAME)
   {
      models[Gmat::GRAVITY_FIELD].push_back("ganymede2x2");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   else if (instanceName == SolarSystem::IO_NAME)
   {
      models[Gmat::GRAVITY_FIELD].push_back("io2x2");
      models[Gmat::GRAVITY_FIELD].push_back("Other");
   }
   
   // NEED values for other moons in here!!!!
   // And how do I get position and velocity for the other moons?
   
   //loj: 3/23/06 set default potential file name from the startup file.
   try
   {
      FileManager *fm = FileManager::Instance();
      std::string potfile = GmatStringUtil::ToUpper(instanceName) + "_POT_FILE";
      std::string filename = fm->GetFullPathname(potfile);

      #if DEBUG_MOON
      MessageInterface::ShowMessage
         ("Moon::InitializeMoon() body=%s, potfilename=%s\n", instanceName.c_str(),
          filename.c_str());
      #endif
      
      potentialFileName = filename;
   }
   catch (GmatBaseException &e)
   {
      MessageInterface::ShowMessage(e.GetFullMessage());
   }

   // write message for now
   if (instanceName != SolarSystem::MOON_NAME)
   MessageInterface::ShowMessage(
         "Unknown moon created - please supply physical parameter values");
}


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

