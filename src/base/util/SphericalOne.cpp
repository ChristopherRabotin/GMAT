//$Header$ 
//------------------------------------------------------------------------------
//                             SphericalOne
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
 * Implements SphericalOne class for spherical elements including Azimuth and 
 * Flight Path Angle.
 *
 * @note:   This code is revived from the original argosy and swingby 
 */
//------------------------------------------------------------------------------

#include "SphericalOne.hpp"

//---------------------------------
//  static data
//---------------------------------
const std::string SphericalOne::DATA_DESCRIPTIONS[NUM_DATA] =
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
//  SphericalOne::SphericalOne() 
//------------------------------------------------------------------------------
/**
 * Constructs base Spherical structures 
 */
SphericalOne::SphericalOne() :
    Spherical       (),
    azimuth         (0.0),
    flightPathAngle (0.0)
{
}

//------------------------------------------------------------------------------
//  SphericalOne::SphericalOne(Real rMag,  Real ra, Real dec, Real vMag, 
//                             Real az, Real fltPathAngle)
//------------------------------------------------------------------------------
SphericalOne::SphericalOne(Real rMag,  Real ra, Real dec, Real vMag,
                           Real az, Real fltPathAngle) :
    Spherical        (rMag,ra,dec,vMag),
    azimuth          (az),
    flightPathAngle  (fltPathAngle)
{
}

//------------------------------------------------------------------------------
//   SphericalOne::SphericalOne(const SphericalOne &spherical)
//------------------------------------------------------------------------------
SphericalOne::SphericalOne(const SphericalOne &spherical) :
    Spherical        (spherical.positionMagnitude, spherical.rightAscension,
                      spherical.declination, spherical.velocityMagnitude),
    azimuth          (spherical.azimuth),
    flightPathAngle  (spherical.flightPathAngle)
{
}

//------------------------------------------------------------------------------
//  SphericalOne& SphericalOne::operator=(const SphericalOne &spherical)
//------------------------------------------------------------------------------
SphericalOne& SphericalOne::operator=(const SphericalOne &spherical)
{
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
//  SphericalOne::~SphericalOne() 
//------------------------------------------------------------------------------
SphericalOne::~SphericalOne() 
{
}

//------------------------------------------------------------------------------
//  std::ostream& operator<<(std::ostream& output, SphericalOne &s)
//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& output, SphericalOne &s)
{
    Rvector v(6, s.positionMagnitude, s.rightAscension, s.declination,
              s.velocityMagnitude, s.azimuth, s.flightPathAngle);

    output << v << std::endl;

    return output;
}

//------------------------------------------------------------------------------
//  <friend>
//  std::istream& operator>>(std::istream& input, SphericalOne &s)
//------------------------------------------------------------------------------
std::istream& operator>>(std::istream& input, SphericalOne &s )
{
    input >> s.positionMagnitude >> s.rightAscension 
          >> s.declination >> s.velocityMagnitude
          >> s.azimuth >> s.flightPathAngle;

    return input;
}

//------------------------------------------------------------------------------
// Real SphericalOne::GetAzimuth() const
//------------------------------------------------------------------------------
Real SphericalOne::GetAzimuth() const
{
        return azimuth;
}

//------------------------------------------------------------------------------
// void SphericalOne::SetAzimuth(const Real az) 
//------------------------------------------------------------------------------
void SphericalOne::SetAzimuth(const Real az)
{
        azimuth = az;
}

//------------------------------------------------------------------------------
// Real SphericalOne::GetFlightPathAngle() const
//------------------------------------------------------------------------------
Real SphericalOne::GetFlightPathAngle() const
{
        return flightPathAngle;
}

//------------------------------------------------------------------------------
// void SphericalOne::SetFlightPathAngle(const Real fPA) 
//------------------------------------------------------------------------------
void SphericalOne::SetFlightPathAngle(const Real fPA) 
{
        flightPathAngle = fPA;
}

//------------------------------------------------------------------------------
// bool SphericalOne::ToSphericalOne(const Cartesian &cartesian)
//------------------------------------------------------------------------------
bool SphericalOne::ToSphericalOne(const Cartesian &cartesian)
{
    if (!Spherical::ToSpherical(cartesian,true))
    {
        return false;
    }

    // Get position and velocity vectors from Cartesian  
    Rvector3 position = cartesian.GetPosition();
    Rvector3 velocity = cartesian.GetVelocity();
    
    // Calculate the local plane coordinate system of the spacecraft
    Rvector3 x_hat_lp, y_hat_lp, z_hat_lp;
    CalculateLocalPlaneCoord(position,x_hat_lp,y_hat_lp,z_hat_lp);

    // Calculate azimuth angle measured with clockwise from the z_hat_lp to 
    // the projection of the velocity vector onto the y_hat_lp - z_hat_lp plane
    Real mState = GmatMathUtil::ATan(velocity*y_hat_lp , velocity*z_hat_lp);
    mState = Spherical::GetDegree(mState,0.0,GmatMathUtil::TWO_PI);
    SetAzimuth(mState);

    // Set Flight Path Angle measured from the x_hat_lp axis to the velocity
    // vector
    if (velocity.GetMagnitude() <= ORBIT_TOLERANCE)
    {
       mState = 0.0;
    } 
    else
    {
       mState = GmatMathUtil::ACos((velocity*x_hat_lp)/velocity.GetMagnitude());
       mState = Spherical::GetDegree(mState,-GmatMathUtil::PI,GmatMathUtil::PI);
    }
    SetFlightPathAngle(mState);

    return true;
} 

//------------------------------------------------------------------------------
// Cartesian SphericalOne::GetCartesian() 
//------------------------------------------------------------------------------
Cartesian SphericalOne::GetCartesian()
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
    return Cartesian(position,velocity);
}

//------------------------------------------------------------------------------
// void SphericalOne::CalculateLocalPlaneCoord(const Rvector3 position,
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
void SphericalOne::CalculateLocalPlaneCoord(const Rvector3 position,
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
// Integer SphericalOne::GetNumData() const
//------------------------------------------------------------------------------
Integer SphericalOne::GetNumData() const
{
   return NUM_DATA;
}

//------------------------------------------------------------------------------
// const std::string* SphericalOne::GetDataDescriptions() const
//------------------------------------------------------------------------------
const std::string* SphericalOne::GetDataDescriptions() const
{
   return DATA_DESCRIPTIONS;
}

//------------------------------------------------------------------------------
//  std::string* SphericalOne::ToValueStrings(void)
//------------------------------------------------------------------------------
std::string* SphericalOne::ToValueStrings(void)
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
