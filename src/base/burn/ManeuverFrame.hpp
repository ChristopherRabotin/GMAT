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


#ifndef ManeuverFrame_hpp
#define ManeuverFrame_hpp


#include "gmatdefs.hpp"


class ManeuverFrame 
{
public:
	ManeuverFrame();
	virtual ~ManeuverFrame();
 
protected:
    /// Basis vector in "x"-direction
    Real            xBasis[3];
    /// Basis vector in "y"-direction
    Real            yBasis[3];
    /// Basis vector in "z"-direction
    Real            zBasis[3];
};


#endif // ManeuverFrame_hpp
