//$Header$
//------------------------------------------------------------------------------
//                                 Maneuver
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Definition for the Maneuver command class
 */
//------------------------------------------------------------------------------


#ifndef Maneuver_hpp
#define Maneuver_hpp

#include "Command.hpp"
#include "Burn.hpp"
#include "Spacecraft.hpp"


/**
 *
 */
class Maneuver : public Command
{
public:
    Maneuver(void);
    virtual ~Maneuver(void);
    
    Maneuver(const Maneuver& m);
    Maneuver&           operator=(const Maneuver& m);
    
protected:
    /// The burn object used for the maneuver
    Burn                *burn;
    /// The name of the spacecraft that gets maneuvered
    std::string         satName;
    /// The spacecraft
    Spacecraft          *sat;
};

#endif // Maneuver_hpp
