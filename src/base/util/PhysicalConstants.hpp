//$Header$
//------------------------------------------------------------------------------
//                           PhysicalConstants
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: D. McGarry
// Created: 1995/07/20 for GSS project
// Modified: 2003/09/16 Linda Jun - Modified to use namespace instead of struct
//
/**
 * Provides declarations for commonly used physical constants.
 */
//------------------------------------------------------------------------------
#ifndef PhysicalConstants_hpp
#define PhysicalConstants_hpp

namespace GmatPhysicalConst
{
    //Speed Of Light Constant
    const Real SPEED_OF_LIGHT_VACUUM = 299792458.0;  // m/s
    const Real c = 299792458.0;  // m/s

    //Astronomical Constants
    const Real MOON_GRAVITATION_CONSTANT = 4.902794e12;  // m^3/s^2
    const Real INCLINATION_OF_LUNAR_ORBIT = 0.089804108;  // radians
    const Real MOON_MEAN_DISTANCE_FROM_EARTH = 384401000.0;  // m (+/- 1000)
    const Real MOON_ECCENTRICITY_OF_ORBIT = 0.0549;
    const Real INCLINATION_OF_LUNAR_ORBIT_TO_ECLIPTIC = 8.98041144343826;  // radians
    const Real MEAN_MOON_RADIUS = 1738200.0;  // m
    const Real MOON_MASS = 7.3483e22;  // kg 
        
    const Real SUN_GRAVITATION_CONSTANT = 1.32712438e20;  // m^3/s^2
    const Real SUN_PHOTOSPHERE_RADIUS = 695990000.0;  // m
        
    const Real ASTRONOMICAL_UNIT = 1.4959787e11;  // m
       
    const Real SOLAR_PARALLAX_IN_DEG = 0.002442819;  // degrees
    const Real SOLAR_PARALLAX_IN_RAD = 0.002442819 * 0.0174532925199433;  //radians 

    //Physical Properties of the Earth
    const Real EARTH_EQUATORIAL_RADIUS = 6378140.0;  // m
    const Real EARTH_POLAR_RADIUS = 6356755.0;  // m
    const Real EARTH_MEAN_RADIUS = 6371000.0;  // m
        
    const Real EARTH_FLATTENING_FACTOR = 0.00335281;
    const Real f = 0.00335281;
        
    const Real EARTH_GRAVITATION_CONSTANT = 3.986005e14;  // m^3/s^2
    const Real mu = 3.986005e14;  // m^3/s^2
        
    const Real EARTH_ORBITAL_SPEED = 29790.0;  // m/s
    
    const Real ECCENTRICITY_TERRESTIAL_ORBIT = 0.016751;
    
    const Real ROTATIONAL_ANGULAR_RATE_OF_EARTH_DEG = 0.004178074622;  // deg/s
    const Real ROTATIONAL_ANGULAR_RATE_OF_EARTH_RAD = 0.004178074622 * 0.0174532925199433;  // rad/s

    const Real OBLIQUITY_OF_THE_ECLIPTIC = 0.4090928;  // radians

    //values of terms in the expansion of the Earth's gravitational
    //potential in a series of a spherical harmonics
    const Real EARTH_J2 = 0.00108263;
    const Real EARTH_J3 = -0.00000254;
    const Real EARTH_J4 = -0.00000161;
}

#endif // PhysicalConstants_hpp
