//$Header$
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
#include "Planet.hpp"

// initialize static default values
// default values for CelesitalBody data
const Gmat::BodyType        Planet::BODY_TYPE           = Gmat::PLANET;
const Real                  Planet::MASS                = 5.976e24;     // kg
const Real                  Planet::EQUATORIAL_RADIUS   = 6378.164;     // km
const Real                  Planet::POLAR_RADIUS        = 6356.8;       // km - need more precision
const Real                  Planet::MU                  = 9.80;      // m/s^2???
const Gmat::PosVelSource    Planet::POS_VEL_SOURCE      = Gmat::SLP;   // for Build 2, at least
const Gmat::AnalyticMethod  Planet::ANALYTIC_METHOD     = Gmat::TWO_BODY; // ??
const CelestialBody*        Planet::CENTRAL_BODY        = NULL;        // doesn't make sense?
const Integer               Planet::BODY_NUMBER         = 2;           // don't know this!!!!!!
const Integer               Planet::REF_BODY_NUMBER     = 2;           // don't know this!!!!!!
const Integer               Planet::ORDER               = 4;           // don't know this!!!!!!
const Integer               Planet::DEGREE              = 4;           // don't know this!!!!!!
// add other ones as needed


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
CelestialBody     (name)
{
   //parameterCount += 30; -- ???
   InitializePlanet(NULL);  
}

//------------------------------------------------------------------------------
//  Planet(std::string name, CelestialBody* cBody)
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
Planet::Planet(std::string name, CelestialBody* cBody) :
CelestialBody     (name)
{
   //parameterCount += 30; -- ???
   InitializePlanet(cBody); 
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
CelestialBody (pl)
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

   GmatBase::operator=(pl);
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
//  RealArray GetState(A1Mjd atTime)
//------------------------------------------------------------------------------
/**
 * This method returns the state (position and velocity) of the body at the
 * requested time.
 *
 * @param <atTime>  time for which state of the body is requested.
 *
 * @return state of the body at the requested time.
 *
 */
//------------------------------------------------------------------------------
RealArray  Planet::GetState(A1Mjd atTime)
{
   return state; // put in the real stuff based on the PosVelSource flag, etc.****************
}

//------------------------------------------------------------------------------
//  Planet* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Planet.
 *
 * @return clone of the star.
 *
 */
//------------------------------------------------------------------------------
Planet* Planet::Clone(void) const
{
   return NULL;  // TBD
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  void  InitializePlanet(CelestialBody* cBody)
//------------------------------------------------------------------------------
/**
 * This method initializes the data values for the body.
 *
 * @param <cBody> central body (default is NULL).
 *
 */
//------------------------------------------------------------------------------
void Planet::InitializePlanet(CelestialBody* cBody)
{
   CelestialBody::Initialize();
   // fill in with default values, for the Sun
   bodyType            = Planet::BODY_TYPE;
   mass                = Planet::MASS;
   equatorialRadius    = Planet::EQUATORIAL_RADIUS;
   polarRadius         = Planet::POLAR_RADIUS;
   mu                  = Planet::MU;
   posVelSrc           = Planet::POS_VEL_SOURCE;
   analyticMethod      = Planet::ANALYTIC_METHOD;
   //centralBody         = Planet::CENTRAL_BODY;
   centralBody         = cBody;
   bodyNumber          = Planet::BODY_NUMBER;
   referenceBodyNumber = Planet::REF_BODY_NUMBER;
   order               = Planet::ORDER;
   degree              = Planet::DEGREE;
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

