//$Header$
//------------------------------------------------------------------------------
//                              ManeuverFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the base class for maneuver reference frames. 
 */
//------------------------------------------------------------------------------


#include "ManeuverFrame.hpp"


ManeuverFrame::ManeuverFrame()
{
    xBasis[0] = yBasis[1] = zBasis[2] = 1.0;
    xBasis[1] = yBasis[2] = zBasis[0] = 
    xBasis[2] = yBasis[0] = zBasis[1] = 0.0;
}


ManeuverFrame::~ManeuverFrame()
{
}
