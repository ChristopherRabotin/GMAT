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
//           2004/06/09 Wendy Shoan - Removed constants that are specific to 
//                                    celestial bodies.
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
    const Real SPEED_OF_LIGHT_VACUUM            = 299792458.0;  // m/s
    const Real c                                = 299792458.0;  // m/s
    
    // gravitational constant (units: km^3/(kg s^2))
    const Real UNIVERSAL_GRAVITATIONAL_CONSTANT = 6.673e-20; 

    //Astronomical Constants        
    const Real ASTRONOMICAL_UNIT                = 1.4959787e11;  // m
}

#endif // PhysicalConstants_hpp
