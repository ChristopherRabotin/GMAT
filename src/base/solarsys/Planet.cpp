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
//#include "MessageInterface.hpp"

// initialize static default values
// default values for CelesitalBody data
const Gmat::BodyType        Planet::DEFAULT_BODY_TYPE         = Gmat::PLANET;
const Gmat::PosVelSource    Planet::DEFAULT_POS_VEL_SOURCE    = Gmat::SLP; 
const Gmat::AnalyticMethod  Planet::DEFAULT_ANALYTIC_METHOD   = Gmat::TWO_BODY;
const Integer               Planet::DEFAULT_BODY_NUMBER       = 1;
const Integer               Planet::DEFAULT_REF_BODY_NUMBER   = 3;

// default values for physical constants for each planet
const Real                  Planet::MASS[NumberOfPlanets]                      =         // kg
{
   0.330217033464048e+24,
   0.486898629521689e+25,
   5.976e24,    // Swingby says 0.597422733923030e+25
   0.641910770215361e+24,
   0.189916962053049e+28,
   0.568637885324189e+27,
   0.866330715467283e+26,
   0.102987226790760e+21,
   0.153007332240150e+17
};
const Real                  Planet::EQUATORIAL_RADIUS[NumberOfPlanets]         =         // km
{
   2439.7, 
   6051.8,
   6378.1363,
   3396.200,
   71492.00,
   60368.0,
   25559.0,
   24764.0,
   1195.0
};
const Real                  Planet::POLAR_RADIUS[NumberOfPlanets]              =         // km 
{     
   2439.7,
   6051.8, 
   6356.755,
   3376.189,  // North polar radius, per http://ltpwww.gsfc.nasa.gov/tharsis/geodesy.html
   66854.0,   // per http://www.angelfire.com/sc/jstroberg/astrodata.html
   54362.0,   // per http://members.shaw.ca/evildrganymede/art/planets/saturn.htm
   24973.0,   // per http://www.nasm.si.edu/research/ceps/rpif/uranus/rpifuranus.html
   24340.0,   // per http://www.geocities.com/vickyparalkar/nepstats.html
   1195.0     // per http://nssdc.gsfc.nasa.gov/planetary/factsheet/plutofact.html
};
const Real                  Planet::MU[NumberOfPlanets]                        =         // km^3/s^2
{
   0.220320804727213035e+05,
   0.324858765616871577e+06,
   398600.4415,    // Swingby says 0.398600448073446198e+06
   0.428282865887688895e+05,
   0.126712597081794508e+09,
   0.379395197088299505e+08,
   0.578015853359771776e+07,
   0.687130777147952165e+07,
   0.102086492070628526e+04
};
const Integer               Planet::ORDER[NumberOfPlanets]               =
{4, 4, 4, 4, 4, 4, 4, 4, 4};

const Integer               Planet::DEGREE[NumberOfPlanets]              =
{4, 4, 4, 4, 4, 4, 4, 4, 4};
const Integer               Planet::COEFFICIENT_SIZE[NumberOfPlanets]    =
{4, 4, 4, 4, 4, 4, 4, 4, 4};  // do I need this?
const Rmatrix               Planet::SIJ[NumberOfPlanets]                 =
{
   Rmatrix(5,5,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.0,             0.0,             0.0,             0.0,
           0.0, 0.119528010e-08,-0.140010930e-05, 0.0,             0.0,
           0.0, 0.248806640e-06,-0.619230590e-06, 0.141403690e-05, 0.0,
           0.0,-0.473422660e-06, 0.662868900e-06,-0.201009910e-06, 0.308845320e-06),
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
   1.0,                0.0,                0.0,                0.0,                 0.0,
   0.0,                0.0,                0.0,                0.0,                 0.0,
   -0.484165390000e-03,-0.186987640000e-09, 0.243908370000e-05, 0.0,                 0.0,
   0.957122390000e-06, 0.202839970000e-05, 0.904408620000e-06, 0.721153880000e-06,  0.0,
   0.540143330000e-06,-0.536367950000e-06, 0.350349320000e-06, 0.990258120000e-06, -0.188488500000e-06),
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

   Integer bodyIndex;
   
   // fill in with default values, use Earth values for Earth and unheard-of
   // planets
   centralBody         = cBody;
   bodyType            = Planet::DEFAULT_BODY_TYPE;
   posVelSrc           = Planet::DEFAULT_POS_VEL_SOURCE;
   analyticMethod      = Planet::DEFAULT_ANALYTIC_METHOD;
   bodyNumber          = Planet::DEFAULT_BODY_NUMBER;
   referenceBodyNumber = Planet::DEFAULT_REF_BODY_NUMBER;

   if (instanceName == SolarSystem::MERCURY_NAME)      bodyIndex = MERCURY;
   else if (instanceName == SolarSystem::VENUS_NAME)   bodyIndex = VENUS;
   else if (instanceName == SolarSystem::EARTH_NAME)   bodyIndex = EARTH;
   else if (instanceName == SolarSystem::MARS_NAME)    bodyIndex = MARS;
   else if (instanceName == SolarSystem::JUPITER_NAME) bodyIndex = JUPITER;
   else if (instanceName == SolarSystem::SATURN_NAME)  bodyIndex = SATURN;
   else if (instanceName == SolarSystem::URANUS_NAME)  bodyIndex = URANUS;
   else if (instanceName == SolarSystem::NEPTUNE_NAME) bodyIndex = NEPTUNE;
   else if (instanceName == SolarSystem::PLUTO_NAME)   bodyIndex = PLUTO;
   // for Earth and other(?) planets, use Earth defaults
   else
   {
      bodyIndex = EARTH;
      //MessageInterface::ShowMessage(
      // "Unknown planet created - please supply physical parameter values");
   }
   
   mass                = Planet::MASS[bodyIndex];
   equatorialRadius    = Planet::EQUATORIAL_RADIUS[bodyIndex];
   polarRadius         = Planet::POLAR_RADIUS[bodyIndex];
   mu                  = Planet::MU[bodyIndex];
   order               = Planet::ORDER[bodyIndex];
   degree              = Planet::DEGREE[bodyIndex];
   coefficientSize     = Planet::COEFFICIENT_SIZE[bodyIndex];

   sij                 = Planet::SIJ[bodyIndex];
   cij                 = Planet::CIJ[bodyIndex];
   defaultSij          = Planet::SIJ[bodyIndex];
   defaultCij          = Planet::CIJ[bodyIndex];

   atmManager          = new AtmosphereManager(instanceName);


   // set defaults in case use chooses to go back to not using potential file
   defaultMu           = Planet::MU[bodyIndex];
   defaultEqRadius     = Planet::EQUATORIAL_RADIUS[bodyIndex];
   defaultCoefSize     = Planet::COEFFICIENT_SIZE[bodyIndex];

}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

