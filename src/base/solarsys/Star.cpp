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
const Real                  Star::MU                  = 1.32712438e20;      // m^3 / s^2
const Gmat::PosVelSource    Star::POS_VEL_SOURCE      = Gmat::SLP;   // for Build 2, at least
const Gmat::AnalyticMethod  Star::ANALYTIC_METHOD     = Gmat::TWO_BODY; // ??
const Integer               Star::BODY_NUMBER         = 3;  
const Integer               Star::REF_BODY_NUMBER     = 3;    
const Integer               Star::ORDER               = 4;      
const Integer               Star::DEGREE              = 4;
const Integer               Star::COEFFICIENT_SIZE    = 4;
const Rmatrix               Star::SIJ                 = Rmatrix(4,4,
            0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
const Rmatrix               Star::CIJ                 = Rmatrix(4,4,
           0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);


const Real                  Star::RADIANT_POWER       = 1358.0;       // W / m^2
const Real                  Star::REFERENCE_DISTANCE  = 1.49597870e8; // km (1 AU)
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
CelestialBody     (st),
radiantPower      (st.radiantPower),
referenceDistance (st.referenceDistance)
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
   radiantPower      = st.radiantPower;
   referenceDistance = st.referenceDistance;
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
//  Real GetRadiantPower() const
//------------------------------------------------------------------------------
/**
 * This method returns the radiant power of the star.
 *
 * @return radiant power of the star.
 *
 */
//------------------------------------------------------------------------------
Real Star::GetRadiantPower() const
{
   return radiantPower;
}

//------------------------------------------------------------------------------
//  Real GetReferenceDifference() const
//------------------------------------------------------------------------------
/**
 * This method returns the reference distance associated with the radiant power
 * of the star.
 *
 * @return reference distance of the star.
 *
 */
//------------------------------------------------------------------------------
Real Star::GetReferenceDistance() const
{
   return referenceDistance;
}

//------------------------------------------------------------------------------
//  bool SetRadiantPower(Real radPower, Real refDistance)
//------------------------------------------------------------------------------
 /**
 * This method sets the radiant power and reference distance for the star.
 *
 * @return flag indicating success of the operation.
 *
 */
//------------------------------------------------------------------------------
bool Star::SetRadiantPower(Real radPower, Real refDistance)
{
   radiantPower      = radPower;
   referenceDistance = refDistance;
   return true;
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
   Star* theClone = new Star(*this);
   return theClone;   // huh??????????????????????????????
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
   // fill in with default values, for the Sun (all from CelestialBody)
   bodyType            = Star::BODY_TYPE;
   mass                = Star::MASS;
   equatorialRadius    = Star::EQUATORIAL_RADIUS;
   polarRadius         = Star::POLAR_RADIUS;
   mu                  = Star::MU;
   posVelSrc           = Star::POS_VEL_SOURCE;
   analyticMethod      = Star::ANALYTIC_METHOD;
   centralBody         = NULL;
   bodyNumber          = Star::BODY_NUMBER;
   referenceBodyNumber = Star::REF_BODY_NUMBER;
   order               = Star::ORDER;
   degree              = Star::DEGREE;

   atmManager          = NULL;
   
   // fill in default values for Star-specific stuff
   radiantPower        = Star::RADIANT_POWER;
   referenceDistance   = Star::REFERENCE_DISTANCE;

   coefficientSize     = Star::COEFFICIENT_SIZE;
   sij                 = Star::SIJ;
   cij                 = Star::CIJ;  
   defaultSij          = Star::SIJ;
   defaultCij          = Star::CIJ;
   defaultMu           = Star::MU;
   defaultEqRadius     = Star::EQUATORIAL_RADIUS;
   defaultCoefSize     = Star::COEFFICIENT_SIZE;
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

