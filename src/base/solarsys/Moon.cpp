//$Header$
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
#include "CelestialBody.hpp"
#include "Moon.hpp"
#include "PhysicalConstants.hpp"
#include "MessageInterface.hpp"


// initialize static default values
// default values for CelesitalBody data
const Gmat::BodyType        Moon::BODY_TYPE           = Gmat::MOON;
//const Real                  Moon::MASS                = 7.3483e22;     // kg
const Real                  Moon::EQUATORIAL_RADIUS   = 1738.1;     // km
const Real                  Moon::FLATTENING          = 0.0;
//const Real                  Moon::POLAR_RADIUS        = 1736.0;       // km - need more precision
const Real                  Moon::MU                  = 4902.799;      // km^3 / s^2 
const Gmat::PosVelSource    Moon::POS_VEL_SOURCE      = Gmat::SLP;   // for Build 2, at least
const Gmat::AnalyticMethod  Moon::ANALYTIC_METHOD     = Gmat::TWO_BODY; // ??
const Integer               Moon::BODY_NUMBER         = 2; 
const Integer               Moon::REF_BODY_NUMBER     = 3; 
const Integer               Moon::ORDER               = 4; 
const Integer               Moon::DEGREE              = 4;  

//const Integer               Moon::COEFFICIENT_SIZE    = 4;

const Rmatrix               Moon::SIJ                 = Rmatrix(5,5,
   0.0,                  0.0,                  0.0,                  0.0,                 0.0,
   0.0,                  0.0,                  0.0,                  0.0,                 0.0,
   0.0, 4.78976286742000E-09, 1.19043314469000E-08,                  0.0,                 0.0,
   0.0, 5.46564929895000E-06, 4.88875341590000E-06,-1.76416063010000E-06,                 0.0,
   0.0, 1.63304293851000E-06,-6.76012176494000E-06,-1.34287028168000E-05, 3.94334642990000E-06);
const Rmatrix               Moon::CIJ                 = Rmatrix(5,5,
                     1.0,                 0.0,                  0.0,                  0.0,                   0.0,
                     0.0,                 0.0,                  0.0,                  0.0,                   0.0,
   -9.09314486280000E-05, 9.88441569067000E-09, 3.47139237760000E-05,                  0.0,                  0.0,
   -3.17765981183000E-06, 2.63497832935000E-05, 1.42005317544000E-05, 1.22860504604000E-05,                  0.0,
    3.21502582986000E-06,-6.01154071094000E-06,-7.10667037450000E-06,-1.37041711834000E-06,-6.03652719918000E-06);


// add other ones as needed


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
   InitializeMoon(NULL);  
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
   InitializeMoon(cBody); 
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
 * @param <pl> the Moon object whose data to assign to "this"
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
   CelestialBody::Initialize();
   // fill in with default values, for the Sun
   bodyType            = Moon::BODY_TYPE;
   mu                  = Moon::MU;
   mass                = mu /
                         GmatPhysicalConst::UNIVERSAL_GRAVITATIONAL_CONSTANT;
  // mass                = Moon::MASS;
   equatorialRadius    = Moon::EQUATORIAL_RADIUS;
   flattening          = Moon::FLATTENING;
   //polarRadius         = Moon::POLAR_RADIUS;
   polarRadius         = (1.0 - flattening) * equatorialRadius;
   posVelSrc           = Moon::POS_VEL_SOURCE;
   analyticMethod      = Moon::ANALYTIC_METHOD;
   centralBody         = cBody;
   bodyNumber          = Moon::BODY_NUMBER;
   referenceBodyNumber = Moon::REF_BODY_NUMBER;

   atmManager          = new AtmosphereManager(instanceName);

   //coefficientSize     = Moon::COEFFICIENT_SIZE;
   order               = Moon::ORDER;
   degree              = Moon::DEGREE;
   sij                 = Moon::SIJ;
   cij                 = Moon::CIJ;

   //defaultSij          = sij;
   //defaultCij          = cij;
   defaultMu           = Moon::MU;
   defaultEqRadius     = Moon::EQUATORIAL_RADIUS;
   //defaultCoefSize     = Moon::COEFFICIENT_SIZE;
   
   if (instanceName != SolarSystem::MOON_NAME)
   MessageInterface::ShowMessage(
    "Unknown moon created - please supply physical parameter values");
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

