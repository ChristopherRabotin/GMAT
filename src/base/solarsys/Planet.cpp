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
#include "MessageInterface.hpp"

// initialize static default values
// default values for Planet data
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
//const Integer               Planet::ORDER[NumberOfPlanets]               =
//{4, 4, 4, 4, 4, 4, 4, 4, 4};

//const Integer               Planet::DEGREE[NumberOfPlanets]              =
//{4, 4, 4, 4, 4, 4, 4, 4, 4};
//const Integer               Planet::COEFFICIENT_SIZE[NumberOfPlanets]    =
//{4, 4, 4, 4, 4, 4, 4, 4, 4};  // do I need this?
//const Rmatrix               Planet::SIJ[NumberOfPlanets]                 =
//{
//   Rmatrix(5,5,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0),
//   Rmatrix(5,5,
//           0.0,                  0.0,                  0.0,                  0.0,                  0.0,
//           0.0,                  0.0,                  0.0,                  0.0,                  0.0,
//           0.0, 1.47467423600000E-08,-9.53141845209000E-08,                  0.0,                  0.0,
//           0.0, 5.40176936891000E-07, 8.11618282044000E-07, 2.11451354723000E-07,                  0.0,
//           0.0, 4.91465604098000E-07, 4.83653955909000E-07,-1.18564194898000E-07, 1.37586364127000E-06),
//   Rmatrix(5,5,
//           0.0,                  0.0,                  0.0,                  0.0,                  0.0,
//           0.0,                  0.0,                  0.0,                  0.0,                  0.0,
//           0.0, 1.19528010000000E-09,-1.40026639758800E-06,                  0.0,                  0.0,
//           0.0, 2.48130798255610E-07,-6.18922846478490E-07, 1.41420398473540E-06,                  0.0,
//           0.0,-4.73772370615970E-07, 6.62571345942680E-07,-2.00987354847310E-07, 3.08848036903550E-07),
//   Rmatrix(5,5,
//           0.0,                  0.0,                  0.0,                  0.0,                  0.0,
//           0.0,                  0.0,                  0.0,                  0.0,                  0.0,
//           0.0, 6.54655690000000E-09, 4.90611750000000E-05,                  0.0,                  0.0,
//           0.0, 2.52926620000000E-05, 8.31603630000000E-06, 2.55554990000000E-05,                  0.0,
//           0.0, 3.70906170000000E-06,-8.97764090000000E-06,-1.72832060000000E-07,-1.28554120000000E-05),
//   Rmatrix(5,5,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0),
//   Rmatrix(5,5,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0),
//   Rmatrix(5,5,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0),
//   Rmatrix(5,5,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0),
//   Rmatrix(5,5,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0),
//};
//const Rmatrix               Planet::CIJ[NumberOfPlanets]                 =
//{
//   Rmatrix(5,5,
//                             1.0, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0,
//           -2.68328157300000E-05, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0),
//   Rmatrix(5,5,
//                             1.0,                  0.0,                  0.0,                  0.0,                  0.0,
//                             0.0,                  0.0,                  0.0,                  0.0,                  0.0,
//           -1.95847963769000E-06, 2.87322988834000E-08, 8.52182522007000E-07,                  0.0,                  0.0,
//            7.98507258430000E-07, 2.34759127059000E-06,-9.45132723095000E-09,-1.84756674598000E-07,                  0.0,
//            7.15385582249000E-07,-4.58736811774000E-07, 1.26875441402000E-07,-1.74034531883000E-07, 1.78438307106000E-07),
//   Rmatrix(5,5,
//                             1.0,                  0.0,                  0.0,                  0.0,                   0.0,
//                             0.0,                  0.0,                  0.0,                  0.0,                   0.0,
//           -4.84165374886470E-04,-1.86987640000000E-10, 2.43926074865630E-06,                  0.0,                   0.0,
//            9.57170590888000E-07, 2.03013720555300E-06, 9.04706341272910E-07, 7.21144939823090E-07,                   0.0,
//            5.39777068357300E-07,-5.36243554298510E-07, 3.50670156459380E-07, 9.90868905774410E-07, -1.88481367425270E-07),
//   Rmatrix(5,5,
//                             1.0,                  0.0,                  0.0,                 0.0,                   0.0,
//                             0.0,                  0.0,                  0.0,                 0.0,                   0.0,
//           -8.75977040000000E-04, 3.69395770000000E-09,-8.46829230000000E-05,                 0.0,                   0.0,
//           -1.19062310000000E-05, 3.74737410000000E-06,-1.59844090000000E-05, 3.51325710000000E-05,                  0.0,
//            5.14919500000000E-06, 4.26122630000000E-06,-1.05467200000000E-06, 6.47421510000000E-06, 2.97350070000000E-07),
//   Rmatrix(5,5,
//                             1.0, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0,
//           -6.59640053360000E-03, 0.0,             0.0,             0.0,             0.0,
//            2.19219394350000E-04, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0),
//   Rmatrix(5,5,
//                             1.0, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0,
//           -7.35666364600000E-03, 0.0,             0.0,             0.0,             0.0,
//           -3.77964473010000E-04, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0),
//   Rmatrix(5,5,
//                             1.0, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0,
//           -5.36656314600000E-03, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0),
//   Rmatrix(5,5,
//                             1.0, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0,
//           -1.78885438200000E-03, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0,
//                             0.0, 0.0,             0.0,             0.0,             0.0),
//   Rmatrix(5,5,
//           1.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0,
//           0.0, 0.0,             0.0,             0.0,             0.0),
//};

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
CelestialBody     ("Planet",name)
{   
   InitializePlanet(NULL);  
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
CelestialBody     ("Planet",name)
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
  // Planet* theClone = new Planet(*this);
   //return theClone;   // huh??????????????????????????????
   return (new Planet(*this));
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
      MessageInterface::ShowMessage(
       "Unknown planet created - please supply potential file or physical parameter values\n");
   }
   
   mass                = Planet::MASS[bodyIndex];
   equatorialRadius    = Planet::EQUATORIAL_RADIUS[bodyIndex];
   polarRadius         = Planet::POLAR_RADIUS[bodyIndex];
   mu                  = Planet::MU[bodyIndex];
   //order               = Planet::ORDER[bodyIndex];
   //degree              = Planet::DEGREE[bodyIndex];
   //coefficientSize     = Planet::COEFFICIENT_SIZE[bodyIndex];

   //sij                 = Planet::SIJ[bodyIndex];
   //cij                 = Planet::CIJ[bodyIndex];
   //defaultSij          = sij;
   //defaultCij          = cij;
   //defaultCoefSize     = Planet::COEFFICIENT_SIZE[bodyIndex];

   atmManager          = new AtmosphereManager(instanceName);

   // set defaults in case use chooses to go back to not using potential file
   defaultMu           = mu;
   defaultEqRadius     = equatorialRadius;

}

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// none at this time

