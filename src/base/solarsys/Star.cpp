//$Header$
//------------------------------------------------------------------------------
//                                  Star
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan
// Created: 2004/01/29
//
/**
 * Implementation of the Star class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Star.hpp"

// initialize static default values
// default values for CelesitalBody data
const Gmat::BodyType        Star::BODY_TYPE           = Gmat::STAR;
const Real                  Star::MASS                = 1.989E30;    // kg
const Real                  Star::EQUATORIAL_RADIUS   = 6.97E5;      // km
const Real                  Star::POLAR_RADIUS        = 6.97E5;      // km
const Real                  Star::MU                  = 2.74e4;      // cm/s**2  - need diff units????
const Gmat::PosVelSource    Star::POS_VEL_SOURCE      = Gmat::SLP;   // for Build 2, at least
const Gmat::AnalyticMethod  Star::ANALYTIC_METHOD     = Gmat::TWO_BODY; // ??
const CelestialBody*        Star::CENTRAL_BODY        = NULL;
const Integer               Star::BODY_NUMBER         = 1;           // don't know this!!!!!!
const Integer               Star::REF_BODY_NUMBER     = 1;           // don't know this!!!!!!
const Integer               Star::ORDER               = 4;           // don't know this!!!!!!
const Integer               Star::DEGREE              = 4;           // don't know this!!!!!!
// add other ones as needed


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Star(std::string name)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Star class
 * (default constructor).
 *
 * @param <name> optional parameter indicating the name of the celestial
 *               body (default is "Sun").
 */
//------------------------------------------------------------------------------
Star::Star(std::string name) :
CelestialBody     (name)
{
   //parameterCount += 30; -- ???
   InitializeStar();  // should this be the default?
}

//------------------------------------------------------------------------------
//  Star(const Star &st)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Star class as a copy of the
 * specified Star class (copy constructor).
 *
 * @param <st> Star object to copy.
 */
//------------------------------------------------------------------------------
Star::Star(const Star &st) :
CelestialBody (st)
{
}

//------------------------------------------------------------------------------
//  Star& operator= (const Star& st)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Star class.
 *
 * @param <st> the Star object whose data to assign to "this"
 *            solar system.
 *
 * @return "this" Star with data of input Star st.
 */
//------------------------------------------------------------------------------
Star& Star::operator=(const Star &st)
{
   if (&st == this)
      return *this;

   GmatBase::operator=(st);
   return *this;
}

//------------------------------------------------------------------------------
//  ~Star()
//------------------------------------------------------------------------------
/**
 * Destructor for the Star class.
 */
//------------------------------------------------------------------------------
Star::~Star()
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
RealArray  Star::GetState(A1Mjd atTime)
{
   return state; // put in the real stuff based on the PosVelSource flag, etc.****************
}

//------------------------------------------------------------------------------
//  Star* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Star.
 *
 * @return clone of the star.
 *
 */
//------------------------------------------------------------------------------
Star* Star::Clone(void) const
{
   // TBD
    return NULL;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  void  InitializeStar()
//------------------------------------------------------------------------------
/**
 * This method initializes the data values for the body.
 *
 */
//------------------------------------------------------------------------------
void Star::InitializeStar()
{
   CelestialBody::Initialize();
   // fill in with default values, for the Sun
   bodyType            = Star::BODY_TYPE;
   mass                = Star::MASS;
   equatorialRadius    = Star::EQUATORIAL_RADIUS;
   polarRadius         = Star::POLAR_RADIUS;
   mu                  = Star::MU;
   posVelSrc           = Star::POS_VEL_SOURCE;
   analyticMethod      = Star::ANALYTIC_METHOD;
   //centralBody         = Star::CENTRAL_BODY;
   centralBody         = NULL;
   bodyNumber          = Star::BODY_NUMBER;
   referenceBodyNumber = Star::REF_BODY_NUMBER;
   order               = Star::ORDER;
   degree              = Star::DEGREE;
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

