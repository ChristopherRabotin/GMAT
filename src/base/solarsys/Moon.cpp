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

// initialize static default values
// default values for CelesitalBody data
const Gmat::BodyType        Moon::BODY_TYPE           = Gmat::MOON;
const Real                  Moon::MASS                = 7.3483e22;     // kg
const Real                  Moon::EQUATORIAL_RADIUS   = 1738.1;     // km
const Real                  Moon::POLAR_RADIUS        = 1736.0;       // km - need more precision
const Real                  Moon::MU                  = 4.902794e12;      // m^3 / s^2
const Gmat::PosVelSource    Moon::POS_VEL_SOURCE      = Gmat::SLP;   // for Build 2, at least
const Gmat::AnalyticMethod  Moon::ANALYTIC_METHOD     = Gmat::TWO_BODY; // ??
const Integer               Moon::BODY_NUMBER         = 2; 
const Integer               Moon::REF_BODY_NUMBER     = 3; 
const Integer               Moon::ORDER               = 4; 
const Integer               Moon::DEGREE              = 4;  

const Integer               Moon::COEFFICIENT_SIZE    = 4;
const Rmatrix               Moon::SIJ                 = Rmatrix(4,4,
                                                                0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
const Rmatrix               Moon::CIJ                 = Rmatrix(4,4,
                                                                0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);

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
CelestialBody     (name)
{
   InitializeMoon(NULL);  
}

//------------------------------------------------------------------------------
//  Moon(std::string name, CelestialBody* cBody)
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
Moon::Moon(std::string name, CelestialBody* cBody) :
CelestialBody     (name)
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

   GmatBase::operator=(m);
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
//  Moon* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Moon.
 *
 * @return clone of the star.
 *
 */
//------------------------------------------------------------------------------
Moon* Moon::Clone(void) const
{
   Moon* theClone = new Moon(*this);
   return theClone;   // huh??????????????????????????????
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  void  InitializeMoon(CelestialBody* cBody)
//------------------------------------------------------------------------------
/**
 * This method initializes the data values for the body.
 *
 * @param <cBody> central body (default is NULL).
 *
 */
//------------------------------------------------------------------------------
void Moon::InitializeMoon(CelestialBody* cBody)
{
   CelestialBody::Initialize();
   // fill in with default values, for the Sun
   bodyType            = Moon::BODY_TYPE;
   mass                = Moon::MASS;
   equatorialRadius    = Moon::EQUATORIAL_RADIUS;
   polarRadius         = Moon::POLAR_RADIUS;
   mu                  = Moon::MU;
   posVelSrc           = Moon::POS_VEL_SOURCE;
   analyticMethod      = Moon::ANALYTIC_METHOD;
   centralBody         = cBody;
   bodyNumber          = Moon::BODY_NUMBER;
   referenceBodyNumber = Moon::REF_BODY_NUMBER;
   order               = Moon::ORDER;
   degree              = Moon::DEGREE;

   atmManager          = new AtmosphereManager(instanceName);
   
   coefficientSize     = Moon::COEFFICIENT_SIZE;
   sij                 = Moon::SIJ;
   cij                 = Moon::CIJ;
   defaultSij          = Moon::SIJ;
   defaultCij          = Moon::CIJ;
   defaultMu           = Moon::MU;
   defaultEqRadius     = Moon::EQUATORIAL_RADIUS;
   defaultCoefSize     = Moon::COEFFICIENT_SIZE;
   
   //if (instanceName != SolarSystem::MOON_NAME)
   //MessageInterface::ShowMessage(
   // "Unknown moon created - please supply physical parameter values");
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

