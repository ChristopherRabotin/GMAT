//$Header$ 
//------------------------------------------------------------------------------
//                             SphericalRADEC
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus 
// Created: 2003/12/16 Joey Gurganus
//
/**
 * Implements the SphericalRADEC class for spherical elements including
 * Right Ascension of Velocity and Declination of Velocity.
 *
 * @note:   This code is revived from the original argosy and swingby 
 */
//------------------------------------------------------------------------------

#include "SphericalRADEC.hpp"
#include "AngleUtil.hpp"         // for PutAngleInRadRange()
#include "UtilityException.hpp"

//---------------------------------
//  static data
//---------------------------------
const std::string SphericalRADEC::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Radial Magnitude",
   "Right Ascension",
   "Declination",
   "Velocity Magnitude",
   "Right Ascension of Velocity",
   "Declination of Velocity"
};

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  SphericalRADEC::SphericalRADEC() 
//------------------------------------------------------------------------------
/**
 * Constructs base Spherical structures 
 */
SphericalRADEC::SphericalRADEC() :
   Spherical       (),
   raVelocity      (0.0),
   decVelocity     (0.0)
{
}

//------------------------------------------------------------------------------
//  SphericalRADEC::SphericalAZFPA(const Rvector6& state)
//------------------------------------------------------------------------------
SphericalRADEC::SphericalRADEC(const Rvector6& state) :
   Spherical    (state[0], state[1], state[2], state[3]),
   raVelocity   (state[4]),
   decVelocity  (state[5])
{
}

//------------------------------------------------------------------------------
//  SphericalRADEC::SphericalRADEC(Real rMag,  Real ra, Real dec, Real vMag, 
//                             Real vRA, Real vDec)
//------------------------------------------------------------------------------
SphericalRADEC::SphericalRADEC(Real rMag,  Real ra, Real dec, Real vMag,
                           Real vRA, Real vDec) :
   Spherical        (rMag, ra, dec, vMag),
   raVelocity       (vRA),
   decVelocity      (vDec)
{
}

//------------------------------------------------------------------------------
//   SphericalRADEC::SphericalRADEC(const SphericalRADEC &spherical)
//------------------------------------------------------------------------------
SphericalRADEC::SphericalRADEC(const SphericalRADEC &spherical) :
   Spherical        (spherical.positionMagnitude, spherical.rightAscension,
                     spherical.declination, spherical.velocityMagnitude),
   raVelocity       (spherical.raVelocity),
   decVelocity      (spherical.decVelocity)
{
}

//------------------------------------------------------------------------------
//  SphericalRADEC& SphericalRADEC::operator=(const SphericalRADEC &spherical)
//------------------------------------------------------------------------------
SphericalRADEC& SphericalRADEC::operator=(const SphericalRADEC &spherical)
{
   Spherical::operator=(spherical);
   if (this != &spherical)
   {
      SetPositionMagnitude( spherical.GetPositionMagnitude() );
      SetRightAscension( spherical.GetRightAscension() );
      SetDeclination( spherical.GetDeclination() );
      SetVelocityMagnitude( spherical.GetVelocityMagnitude() );
      SetVelocityRA( spherical.GetVelocityRA() );
      SetVelocityDeclination( spherical.GetVelocityDeclination() );
   }
   return *this;
}

//------------------------------------------------------------------------------
//  SphericalRADEC::~SphericalRADEC() 
//------------------------------------------------------------------------------
SphericalRADEC::~SphericalRADEC() 
{
}

//------------------------------------------------------------------------------
// Rvector6 SphericalRADEC::GetState()
//------------------------------------------------------------------------------
Rvector6 SphericalRADEC::GetState()
{
   return Rvector6(positionMagnitude, rightAscension, declination,
                  velocityMagnitude, raVelocity, decVelocity);
}

//------------------------------------------------------------------------------
// void SphericalRADEC::SetState(const Rvector6& state)
//------------------------------------------------------------------------------
void SphericalRADEC::SetState(const Rvector6& state)
{
   SetPositionMagnitude(state[0]);
   SetRightAscension(state[1]);
   SetDeclination(state[2]);
   SetVelocityMagnitude(state[3]);
   SetVelocityRA(state[4]);
   SetVelocityDeclination(state[5]);
}

//------------------------------------------------------------------------------
// Real SphericalRADEC::GetVelocityRA() const
//------------------------------------------------------------------------------
Real SphericalRADEC::GetVelocityRA() const
{
   return raVelocity;
}

//------------------------------------------------------------------------------
// void SphericalRADEC::SetVelocityRA(const Real vRA) 
//------------------------------------------------------------------------------
void SphericalRADEC::SetVelocityRA(const Real vRA)
{
   raVelocity = vRA;
}

//------------------------------------------------------------------------------
// Real SphericalRADEC::GetVelocityDeclination() const
//------------------------------------------------------------------------------
Real SphericalRADEC::GetVelocityDeclination() const
{
   return decVelocity;
}

//------------------------------------------------------------------------------
// void SphericalRADEC::SetVelocityDeclination(const Real vDec) 
//------------------------------------------------------------------------------
void SphericalRADEC::SetVelocityDeclination(const Real vDec)
{
   decVelocity = vDec;
}

//------------------------------------------------------------------------------
// Rvector6 SphericalRADEC::GetCartesian()
//------------------------------------------------------------------------------
Rvector6 SphericalRADEC::GetCartesian()
{
   // Check if input of radial magnitude is not valid then return false
   if (GmatMathUtil::Abs(GetPositionMagnitude()) <= ORBIT_TOLERANCE)
   {
      throw UtilityException("SphericalRADEC::GetCartesian(): "
                           "position magnitude is less than orbit tolerance");
   }

   // Get the position after converting to part of cartesian 
   Rvector3 position = GetPosition();

   // Convert right ascension of velocity and declination of velocity from
   // units of degrees to units of radians
   Real raV  = GmatMathUtil::Rad( GetVelocityRA() );
   Real decV = GmatMathUtil::Rad( GetVelocityDeclination() );
    
   // start to calculate the conversion to cartesian's velocity
   Real vX = GetVelocityMagnitude() * GmatMathUtil::Cos(decV) * 
      GmatMathUtil::Cos(raV);

   Real vY = vX * GmatMathUtil::Tan(raV);

   Real vZ = GetVelocityMagnitude() * GmatMathUtil::Sin(decV);

   // Set up the Cartesian's new velocity
   Rvector3 velocity; 
   velocity.Set(vX,vY,vZ);

   // Return the cartesian conversion
   return Rvector6(position,velocity);
}

//------------------------------------------------------------------------------
// Integer SphericalRADEC::GetNumData() const
//------------------------------------------------------------------------------
Integer SphericalRADEC::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* SphericalRADEC::GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* SphericalRADEC::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
//  std::string* SphericalRADEC::ToValueStrings(void)
//------------------------------------------------------------------------------
std::string* SphericalRADEC::ToValueStrings(void)
{
   std::stringstream ss("");

   ss << GetPositionMagnitude();
   stringValues[0] = ss.str();
   
   ss.str("");
   ss << GetRightAscension();
   stringValues[1] = ss.str();
   
   ss.str("");
   ss << GetDeclination();
   stringValues[2] = ss.str();
   
   ss.str("");
   ss << GetVelocityMagnitude();
   stringValues[3] = ss.str();

   ss.str("");
   ss << GetVelocityRA();
   stringValues[4] = ss.str();

   ss.str("");
   ss << GetVelocityDeclination();
   stringValues[5] = ss.str();
   
   return stringValues;
}

//------------------------------------------------------------------------------
//  std::string ToString()
//------------------------------------------------------------------------------
/**
 * @return data value string
 */
//------------------------------------------------------------------------------
std::string SphericalRADEC::ToString()
{
   std::string s;
   std::string *val;

   val = ToValueStrings();
   
   for (int i=0; i<NUM_DATA; i++)
      s = s + val[i] + " ";
   
   return s;
}

//---------------------------------
// friend functions
//---------------------------------

//------------------------------------------------------------------------------
//  friend Rvector6 CartesianToSphericalRADEC(const Rvector6& cartesian)
//------------------------------------------------------------------------------
Rvector6 CartesianToSphericalRADEC(const Rvector6& cartesian)
{
   SphericalRADEC newSpherical;

   if (!newSpherical.CartesianToSpherical(cartesian,false))
   {
      throw UtilityException("SphericalAZFPA::CartesianToSphericalAZFPA(): "
                              "failure of converting to Spherical elements\n");

   }

   // Get velocity vectors from Cartesian
   Real  vX = cartesian[3];
   Real  vY = cartesian[4];
   Real  vZ = cartesian[5];

   // Calculate right ascension of velocity which is measured east of vernal
   // equinox using atan2() which returns an angle between 0.0 and TWO_PI
   Real raV = GmatMathUtil::ATan(vY, vX);
   
   // Do we want to put angle between 0.0 and TWO_PI?
   // raV = AngleUtil::PutAngleInRadRange(raV, 0.0, GmatMathUtil::TWO_PI);
   
   // convert to degree
   raV = raV * GmatMathUtil::DEG_PER_RAD;
   
   newSpherical.SetVelocityRA(raV);
   
   //loj: 1/23/06 replaced by above
   //newSpherical.SetVelocityRA( newSpherical.GetDegree(
   //                            GmatMathUtil::ATan(vY,vX), 0.0, 
   //                            GmatMathUtil::TWO_PI ));
   
   // Calculate declination of Velocity which is measured north from the 
   // equator using atan2() which will return an angle between -PI/2..PI/2 
   // radians since the second argument will be greater than or equal to zero.
   newSpherical.SetVelocityDeclination(GmatMathUtil::Deg
                          (GmatMathUtil::ATan(vZ, 
                           GmatMathUtil::Sqrt(vX*vX + vY*vY))) );
   
   return newSpherical.GetState();
}

//------------------------------------------------------------------------------
//  friend Rvector6 SphericalRADECToCartesian(const Rvector6& spherical)
//------------------------------------------------------------------------------
Rvector6 SphericalRADECToCartesian(const Rvector6& spherical)
{
   SphericalRADEC newSph(spherical);
   
   return (newSph.GetCartesian()); 
}

//------------------------------------------------------------------------------
//  friend Rvector6 KeplerianToSphericalRADEC(const Rvector6& keplerian,
//                                            const Real mu, Anomaly anomaly)
//------------------------------------------------------------------------------
Rvector6 KeplerianToSphericalRADEC(const Rvector6& keplerian, 
                                   const Real mu, Anomaly anomaly)
{
   Rvector6 cartesian = CoordUtil::KeplerianToCartesian(keplerian,mu,anomaly);

   return CartesianToSphericalRADEC(cartesian);
}

//------------------------------------------------------------------------------
//  friend Rvector6 SphericalRADECToKeplerian(const Rvector6& spherical,
//                                            const Real mu, Anomaly &anomaly)
//------------------------------------------------------------------------------
Rvector6 SphericalRADECToKeplerian(const Rvector6& spherical,
                                   const Real mu, Anomaly &anomaly)
{
   Rvector6 cartesian = SphericalRADECToCartesian(spherical);

   return (CoordUtil::CartesianToKeplerian(cartesian,mu,anomaly));
}

//------------------------------------------------------------------------------
//  friend Rvector6 AZFPA_To_RADEC(const Rvector6& spherical)
//------------------------------------------------------------------------------
Rvector6 AZFPA_To_RADECV(const Rvector6& spherical)
{
   SphericalAZFPA azFPA(spherical);

   Rvector6 cartesian = SphericalAZFPAToCartesian(azFPA.GetState());

   return CartesianToSphericalRADEC(cartesian);
}

//------------------------------------------------------------------------------
//  friend Rvector6 RADECV_To_AZFPA(const Rvector6& spherical)
//------------------------------------------------------------------------------
Rvector6 RADECV_To_AZFPA(const Rvector6& spherical)
{
   Rvector6 cartesian = SphericalRADECToCartesian(spherical);

   return CartesianToSphericalAZFPA(cartesian);
}

//------------------------------------------------------------------------------
//  friend std::ostream& operator<<(std::ostream& output, SphericalRADEC &s)
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& output, SphericalRADEC &s)
{
   Rvector v(6, s.positionMagnitude, s.rightAscension, s.declination,
             s.velocityMagnitude, s.raVelocity, s.decVelocity);

   output << v << std::endl;

   return output;
}

//------------------------------------------------------------------------------
//  friend std::istream& operator>>(std::istream& input, SphericalRADEC &s)
//------------------------------------------------------------------------------
std::istream& operator>>(std::istream& input, SphericalRADEC &s )
{
   input >> s.positionMagnitude >> s.rightAscension
         >> s.declination >> s.velocityMagnitude
         >> s.raVelocity >> s.decVelocity;

   return input;
}
