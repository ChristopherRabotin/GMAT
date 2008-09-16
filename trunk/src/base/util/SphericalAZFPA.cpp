//$Header$ 
//------------------------------------------------------------------------------
//                             SphericalAZFPA
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
 * Implements SphericalAZFPA class for spherical elements including Azimuth and 
 * Flight Path Angle.
 *
 * @note:   This code is revived from the original argosy and swingby 
 */
//------------------------------------------------------------------------------

#include "SphericalAZFPA.hpp"
#include "UtilityException.hpp"

//---------------------------------
//  static data
//---------------------------------
const std::string SphericalAZFPA::DATA_DESCRIPTIONS[NUM_DATA] =
{
   "Radical Magnitude",
   "Right Ascension",
   "Declination",
   "Velocity Magnitude",
   "Azimuth",
   "Flight Path Angle"
};

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  SphericalAZFPA::SphericalAZFPA() 
//------------------------------------------------------------------------------
/**
 * Constructs base Spherical structures 
 */
SphericalAZFPA::SphericalAZFPA() :
    Spherical       (),
    azimuth         (0.0),
    flightPathAngle (0.0)
{
}

//------------------------------------------------------------------------------
//  SphericalAZFPA::SphericalAZFPA(const Rvector6& state)
//------------------------------------------------------------------------------
SphericalAZFPA::SphericalAZFPA(const Rvector6& state) :
   Spherical       (state[0], state[1], state[2], state[3]),
   azimuth         (state[4]),
   flightPathAngle (state[5])
{
}

//------------------------------------------------------------------------------
//  SphericalAZFPA::SphericalAZFPA(Real rMag,  Real ra, Real dec, Real vMag, 
//                             Real az, Real fltPathAngle)
//------------------------------------------------------------------------------
SphericalAZFPA::SphericalAZFPA(Real rMag,  Real ra, Real dec, Real vMag,
                           Real az, Real fltPathAngle) :
    Spherical        (rMag,ra,dec,vMag),
    azimuth          (az),
    flightPathAngle  (fltPathAngle)
{
}

//------------------------------------------------------------------------------
//   SphericalAZFPA::SphericalAZFPA(const SphericalAZFPA &spherical)
//------------------------------------------------------------------------------
SphericalAZFPA::SphericalAZFPA(const SphericalAZFPA &spherical) :
    Spherical        (spherical.positionMagnitude, spherical.rightAscension,
                      spherical.declination, spherical.velocityMagnitude),
    azimuth          (spherical.azimuth),
    flightPathAngle  (spherical.flightPathAngle)
{
}

//------------------------------------------------------------------------------
//  SphericalAZFPA& SphericalAZFPA::operator=(const SphericalAZFPA &spherical)
//------------------------------------------------------------------------------
SphericalAZFPA& SphericalAZFPA::operator=(const SphericalAZFPA &spherical)
{
   Spherical::operator=(spherical);
   if (this != &spherical)
   {
      SetPositionMagnitude( spherical.GetPositionMagnitude() );
      SetRightAscension( spherical.GetRightAscension() );
      SetDeclination( spherical.GetDeclination() );
      SetVelocityMagnitude( spherical.GetVelocityMagnitude() );
      SetAzimuth( spherical.GetAzimuth() );
      SetFlightPathAngle( spherical.GetFlightPathAngle() );
   }
   return *this;
}

//------------------------------------------------------------------------------
//  SphericalAZFPA::~SphericalAZFPA() 
//------------------------------------------------------------------------------
SphericalAZFPA::~SphericalAZFPA() 
{
}

//------------------------------------------------------------------------------
//     friend functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  friend std::ostream& operator<<(std::ostream& output, SphericalAZFPA &s)
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& output, SphericalAZFPA &s)
{
    Rvector v(6, s.positionMagnitude, s.rightAscension, s.declination,
              s.velocityMagnitude, s.azimuth, s.flightPathAngle);

    output << v << std::endl;

    return output;
}

//------------------------------------------------------------------------------
//  friend std::istream& operator>>(std::istream& input, SphericalAZFPA &s)
//------------------------------------------------------------------------------
std::istream& operator>>(std::istream& input, SphericalAZFPA &s )
{
    input >> s.positionMagnitude >> s.rightAscension 
          >> s.declination >> s.velocityMagnitude
          >> s.azimuth >> s.flightPathAngle;

    return input;
}

//------------------------------------------------------------------------------
//  friend Rvector6 CartesianToSphericalAZFPA(const Rvector6& cartesian)
//------------------------------------------------------------------------------
Rvector6 CartesianToSphericalAZFPA(const Rvector6& cartesian)
{
    SphericalAZFPA newSph;

//    if (!Spherical::CartesianToSpherical(cartesian))

    if (!newSph.CartesianToSpherical(cartesian,true))
    {
        throw UtilityException("SphericalAZFPA::CartesianToSphericalAZFPA(): "
                               "failure of converting to Spherical elements\n");
    }

    // Get position and velocity vectors from Cartesian  
    Rvector3 position = cartesian.GetR();
    Rvector3 velocity = cartesian.GetV();
    
    // Calculate the local plane coordinate system of the spacecraft
    Rvector3 x_hat_lp, y_hat_lp, z_hat_lp;
    newSph.CalculateLocalPlaneCoord(position,x_hat_lp,y_hat_lp,z_hat_lp);

    // Calculate azimuth angle measured with clockwise from the z_hat_lp to 
    // the projection of the velocity vector onto the y_hat_lp - z_hat_lp plane
    Real mState = GmatMathUtil::ATan(velocity*y_hat_lp , velocity*z_hat_lp);
    mState = newSph.GetDegree(mState,0.0,GmatMathUtil::TWO_PI);

    newSph.SetAzimuth(mState);

    // Set Flight Path Angle measured from the x_hat_lp axis to the velocity
    // vector
    if (velocity.GetMagnitude() <= Spherical::ORBIT_TOLERANCE)
    {
       mState = 0.0;
    } 
    else
    {
       mState = GmatMathUtil::ACos((velocity*x_hat_lp)/velocity.GetMagnitude());
       mState = newSph.GetDegree(mState,-GmatMathUtil::PI,GmatMathUtil::PI);
    }

    newSph.SetFlightPathAngle(mState);

    return newSph.GetState();
}

//------------------------------------------------------------------------------
//  friend Rvector6 SphericalAZFPAToCartesian(const Rvector6& sphVector)
//------------------------------------------------------------------------------
Rvector6 SphericalAZFPAToCartesian(const Rvector6& sphVector)
{
   SphericalAZFPA newSph(sphVector);

   return(newSph.GetCartesian());
}

//------------------------------------------------------------------------------
//  friend Rvector6 KeplerianToSphericalAZFPA(const Rvector6& keplerian, 
//                                            const Real mu, Anomaly anomaly)
//------------------------------------------------------------------------------
Rvector6 KeplerianToSphericalAZFPA(const Rvector6& keplerian, const Real mu, 
                                   Anomaly anomaly)
{
   Rvector6 cartesian;

   cartesian = CoordUtil::KeplerianToCartesian(keplerian,mu,anomaly);
   return (CartesianToSphericalAZFPA(cartesian)); 
}

//------------------------------------------------------------------------------
//  friend Rvector6 SphericalAZFPAToKeplerian(const Rvector6& spherical, 
//                                            const Real mu, Anomaly &anomaly)
//------------------------------------------------------------------------------
Rvector6 SphericalAZFPAToKeplerian(const Rvector6& spherical, const Real mu,
                                   Anomaly &anomaly)
{
   Rvector6 cartesian;

   cartesian = SphericalAZFPAToCartesian(spherical);
   return (CoordUtil::CartesianToKeplerian(cartesian,mu,anomaly));
}


//------------------------------------------------------------------------------
//     public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Rvector6 SphericalAZFPA::GetState() 
//------------------------------------------------------------------------------
Rvector6 SphericalAZFPA::GetState()
{
   return Rvector6(positionMagnitude, rightAscension, declination,
                  velocityMagnitude, azimuth, flightPathAngle);
}

//------------------------------------------------------------------------------
// void SphericalAZFPA::SetState(const Rvector6& state) 
//------------------------------------------------------------------------------
void SphericalAZFPA::SetState(const Rvector6& state) 
{
   SetPositionMagnitude(state[0]);
   SetRightAscension(state[1]);
   SetDeclination(state[2]);
   SetVelocityMagnitude(state[3]);
   SetAzimuth(state[4]);
   SetFlightPathAngle(state[5]);
}

//------------------------------------------------------------------------------
// Real SphericalAZFPA::GetAzimuth() const
//------------------------------------------------------------------------------
Real SphericalAZFPA::GetAzimuth() const
{
        return azimuth;
}

//------------------------------------------------------------------------------
// void SphericalAZFPA::SetAzimuth(const Real az) 
//------------------------------------------------------------------------------
void SphericalAZFPA::SetAzimuth(const Real az)
{
        azimuth = az;
}

//------------------------------------------------------------------------------
// Real SphericalAZFPA::GetFlightPathAngle() const
//------------------------------------------------------------------------------
Real SphericalAZFPA::GetFlightPathAngle() const
{
        return flightPathAngle;
}

//------------------------------------------------------------------------------
// void SphericalAZFPA::SetFlightPathAngle(const Real fPA) 
//------------------------------------------------------------------------------
void SphericalAZFPA::SetFlightPathAngle(const Real fPA) 
{
        flightPathAngle = fPA;
}


//------------------------------------------------------------------------------
// Rvector6 SphericalAZFPA::GetCartesian() 
//------------------------------------------------------------------------------
Rvector6 SphericalAZFPA::GetCartesian()
{
    // Get the position after converting to part of cartesian 
    Rvector3 position = GetPosition();
    Rvector3 velocity; 


    // Calculate the local plane coordinate system of the spacecraft
    Rvector3 x_hat_lp, y_hat_lp, z_hat_lp;
    CalculateLocalPlaneCoord(position,x_hat_lp,y_hat_lp,z_hat_lp);

    Real v_mag = GetVelocityMagnitude();

    Real cosA = GmatMathUtil::Cos( GmatMathUtil::Rad(GetAzimuth()) ); 
    Real sinA = GmatMathUtil::Sin( GmatMathUtil::Rad(GetAzimuth()) ); 

    Real cosBeta = GmatMathUtil::Cos( GmatMathUtil::Rad(GetFlightPathAngle()) );
    Real sinBeta = GmatMathUtil::Sin( GmatMathUtil::Rad(GetFlightPathAngle()) );

    // Calculate Vx component
    Real vX = v_mag * ( cosBeta * x_hat_lp.Get(0) +
                         sinBeta * sinA * y_hat_lp.Get(0) +
                         sinBeta * cosA * z_hat_lp.Get(0));
    // Calculate Vy component
    Real vY = v_mag * ( cosBeta * x_hat_lp.Get(1) +
                        sinBeta * sinA * y_hat_lp.Get(1) +
                        sinBeta * cosA * z_hat_lp.Get(1));
    // Calculate Vz component
    Real vZ = v_mag * ( cosBeta * x_hat_lp.Get(2) +
                        sinBeta * sinA * y_hat_lp.Get(2) +
                        sinBeta * cosA * z_hat_lp.Get(2));
    
    velocity.Set(vX,vY,vZ);
    return Rvector6(position,velocity);
}

//------------------------------------------------------------------------------
// void SphericalAZFPA::CalculateLocalPlaneCoord(const Rvector3 position,
//                    Rvector3 &x_hat_lp,Rvector3 &y_hat_lp,Rvector3 &z_hat_lp)
//------------------------------------------------------------------------------
/**
 * Calculate the local plane coordinate system of the spacecraft.
 *
 * @param <position>   input of cartesian's position
 * @param <x_hat_lp>   output of unit vector for x-axis
 * @param <y_hat_lp>   output of unit vector for y-axis
 * @param <z_hat_lp>   output of unit vector for z-axis
 *
 * @note:   this method is reused from swingby and argosy

 *
 */
void SphericalAZFPA::CalculateLocalPlaneCoord(const Rvector3 position,
                   Rvector3 &x_hat_lp,Rvector3 &y_hat_lp,Rvector3 &z_hat_lp)
{
    // Get the position vector magnitude
    Real r_mag = position.GetMagnitude();

        // Get x, y, and z from the position
    Real posX = position.Get(0);
    Real posY = position.Get(1);
    Real posZ = position.Get(2);

    // Calculate right ascention measured east of vernal equinox + 90 degree
    Real alpha2 = GmatMathUtil::ATan(posY,posX) + GmatMathUtil::PI/2.0;
    
    // Directed along the geocentric position vector
    x_hat_lp.Set(posX/r_mag, posY/r_mag, posZ/r_mag);

    // Axis displaced from the inertial y axis by the origin's
    // right ascension and lying in the equatorial plane 
    y_hat_lp.Set(GmatMathUtil::Cos(alpha2), GmatMathUtil::Sin(alpha2), 0.0);

    z_hat_lp = Cross(x_hat_lp, y_hat_lp);
}


//------------------------------------------------------------------------------
// Integer SphericalAZFPA::GetNumData() const
//------------------------------------------------------------------------------
Integer SphericalAZFPA::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* SphericalAZFPA::GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* SphericalAZFPA::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
//  std::string* SphericalAZFPA::ToValueStrings(void)
//------------------------------------------------------------------------------
std::string* SphericalAZFPA::ToValueStrings(void)
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
   ss << GetAzimuth();
   stringValues[4] = ss.str();

   ss.str("");
   ss << GetFlightPathAngle();
   stringValues[5] = ss.str();
   
   return stringValues;
}
