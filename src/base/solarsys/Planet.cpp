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
#include "Rmatrix.hpp"
#include "Planet.hpp"

// initialize static default values
// default values for CelesitalBody data
const Gmat::BodyType        Planet::BODY_TYPE           = Gmat::PLANET;
const Real                  Planet::MASS                = 5.976e24;     // kg
const Real                  Planet::EQUATORIAL_RADIUS   = 6378.140;     // km
const Real                  Planet::POLAR_RADIUS        = 6356.755;       // km - need more precision?
const Real                  Planet::MU                  = 3.986005e14;      // m^3/s^2
const Gmat::PosVelSource    Planet::POS_VEL_SOURCE      = Gmat::SLP;   // for Build 2, at least
const Gmat::AnalyticMethod  Planet::ANALYTIC_METHOD     = Gmat::TWO_BODY; // ??
const Integer               Planet::BODY_NUMBER         = 1;  
const Integer               Planet::REF_BODY_NUMBER     = 3; 
const Integer               Planet::ORDER               = 4;  
const Integer               Planet::DEGREE              = 4;
const Integer               Planet::COEFFICIENT_SIZE    = 4;
const Rmatrix               Planet::SIJ                 = Rmatrix(4,4,
        0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
const Rmatrix               Planet::CIJ                 = Rmatrix(4,4,
       0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);

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
   Planet* theClone = new Planet(*this);
   return theClone;   // huh??????????????????????????????
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
   centralBody         = cBody;
   bodyNumber          = Planet::BODY_NUMBER;
   referenceBodyNumber = Planet::REF_BODY_NUMBER;
   order               = Planet::ORDER;
   degree              = Planet::DEGREE;
   coefficientSize     = Planet::COEFFICIENT_SIZE;

   sij                 = Planet::SIJ;
   cij                 = Planet::CIJ;
   defaultSij          = Planet::SIJ;
   defaultCij          = Planet::CIJ;

   atmManager          = new AtmosphereManager(instanceName);


   // set defaults in case use chooses to go back to not using potential file
   defaultMu           = Planet::MU;
   defaultEqRadius     = Planet::EQUATORIAL_RADIUS;
   defaultCoefSize     = Planet::COEFFICIENT_SIZE;

}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

