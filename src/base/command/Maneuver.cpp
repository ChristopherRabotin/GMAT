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


#include "Maneuver.hpp"


Maneuver::Maneuver(void) :
    Command     ("Maneuver")
{}


Maneuver::~Maneuver(void)
{}

    
Maneuver::Maneuver(const Maneuver& m) :
    Command     ("Maneuver")
{}


Maneuver& Maneuver::operator=(const Maneuver& m)
{
    if (this == &m)
        return *this;
    
    return *this;
}
