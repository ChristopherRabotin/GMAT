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
#include "PhysicalConstants.hpp"
#include "MessageInterface.hpp"

// initialize static default values
// default values for CelesitalBody data
const Gmat::BodyType        Star::BODY_TYPE           = Gmat::STAR;
//const Real                  Star::MASS                = 1.989E30;    // kg
const Real                  Star::EQUATORIAL_RADIUS   = 6.97E5;      // km
const Real                  Star::FLATTENING          = 0.0; 
//const Real                  Star::POLAR_RADIUS        = 6.97E5;      // km
const Real                  Star::MU                  = 1.32712438e11; //km^3/s^2
const Gmat::PosVelSource    Star::POS_VEL_SOURCE      = Gmat::SLP;   // for Build 2, at least
const Gmat::AnalyticMethod  Star::ANALYTIC_METHOD     = Gmat::TWO_BODY; // ??
const Integer               Star::BODY_NUMBER         = 3;  
const Integer               Star::REF_BODY_NUMBER     = 3;    
const Integer               Star::ORDER               = 4;      
const Integer               Star::DEGREE              = 4;
const Rmatrix               Star::SIJ                 = Rmatrix(5,5); //zeroes
const Rmatrix               Star::CIJ                 = Rmatrix(5,5,
      1.0, 0.0,             0.0,             0.0,             0.0,
      0.0, 0.0,             0.0,             0.0,             0.0,
      0.0, 0.0,             0.0,             0.0,             0.0,
      0.0, 0.0,             0.0,             0.0,             0.0,
      0.0, 0.0,             0.0,             0.0,             0.0);
//const Integer               Star::COEFFICIENT_SIZE    = 4;


const Real                  Star::STAR_RADIANT_POWER       = 1358.0;       // W / m^2
const Real                  Star::STAR_REFERENCE_DISTANCE  = 1.49597870e8; // km (1 AU)
const Real                  Star::STAR_PHOTOSPHERE_RADIUS  = 695990000.0;  // m
// add other ones as needed

//---------------------------------
// static data
//---------------------------------
const std::string
Star::PARAMETER_TEXT[StarParamCount - CelestialBodyParamCount] =
{
   "RadiantPower",
   "ReferenceDistance",
   "PhotosphereRadius"
};

const Gmat::ParameterType
Star::PARAMETER_TYPE[StarParamCount - CelestialBodyParamCount] =
{
Gmat::REAL_TYPE,
Gmat::REAL_TYPE,
Gmat::REAL_TYPE
};


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
CelestialBody       ("Star",name)
{
   parameterCount = StarParamCount;
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
CelestialBody       (st),
radiantPower        (st.radiantPower),
referenceDistance   (st.referenceDistance),
photosphereRadius   (st.photosphereRadius)  
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

   CelestialBody::operator=(st);
   radiantPower        = st.radiantPower;
   referenceDistance   = st.referenceDistance;
   photosphereRadius   = st.photosphereRadius;

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
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Star.
 *
 * @return clone of the star.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Star::Clone(void) const
{
   return (new Star(*this));
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Star::GetParameterText(const Integer id) const
{
   if ((id >= CelestialBodyParamCount) && (id < StarParamCount))
      return PARAMETER_TEXT[id - CelestialBodyParamCount];
   return CelestialBody::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer     Star::GetParameterID(const std::string &str) const
{
   for (Integer i = CelestialBodyParamCount; i < StarParamCount; i++)
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
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Star::GetParameterType(const Integer id) const
{
   if ((id >= CelestialBodyParamCount) && (id < StarParamCount))
      return PARAMETER_TYPE[id - CelestialBodyParamCount];
      
   return CelestialBody::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string Star::GetParameterTypeString(const Integer id) const
{
   return CelestialBody::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real        Star::GetRealParameter(const Integer id) const
{
   if (id == RADIANT_POWER)              return radiantPower;
   if (id == REFERENCE_DISTANCE)         return referenceDistance;
   if (id == PHOTOSPHERE_RADIUS)         return photosphereRadius;

   return CelestialBody::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return  Real value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Real        Star::SetRealParameter(const Integer id, const Real value)
{
   if (id == RADIANT_POWER)              return (radiantPower        = value);
   if (id == REFERENCE_DISTANCE)         return (referenceDistance   = value);
   if (id == PHOTOSPHERE_RADIUS)         return (photosphereRadius   = value);
   
   return CelestialBody::SetRealParameter(id, value);
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
   mu                  = Star::MU;
   //mass                = Star::MASS;
   mass                = mu /
                         GmatPhysicalConst::UNIVERSAL_GRAVITATIONAL_CONSTANT;
   equatorialRadius    = Star::EQUATORIAL_RADIUS;
   flattening          = Star::FLATTENING;
  // polarRadius         = Star::POLAR_RADIUS;
   polarRadius         = (1.0 - flattening) * equatorialRadius;
   posVelSrc           = Star::POS_VEL_SOURCE;
   analyticMethod      = Star::ANALYTIC_METHOD;
   centralBody         = "";
   bodyNumber          = Star::BODY_NUMBER;
   referenceBodyNumber = Star::REF_BODY_NUMBER;

   atmManager          = NULL;
   
   // fill in default values for Star-specific stuff
   radiantPower        = Star::STAR_RADIANT_POWER;
   referenceDistance   = Star::STAR_REFERENCE_DISTANCE;
   photosphereRadius   = Star::STAR_PHOTOSPHERE_RADIUS;

   order               = Star::ORDER;
   degree              = Star::DEGREE;
   sij                 = Star::SIJ;
   cij                 = Star::CIJ;
   defaultMu           = Star::MU;
   defaultEqRadius     = Star::EQUATORIAL_RADIUS;
   //coefficientSize     = Star::COEFFICIENT_SIZE;
   //defaultSij          = Star::SIJ;
   //defaultCij          = Star::CIJ;
   //defaultCoefSize     = Star::COEFFICIENT_SIZE;
   
   if (instanceName != SolarSystem::SUN_NAME)
      MessageInterface::ShowMessage(
       "Unknown star created - please supply physical parameter values");
}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

