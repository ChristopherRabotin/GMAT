//$Header$
//------------------------------------------------------------------------------
//                              PlanetaryEphemException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2004/02/29
//
/**
 * Implementation code for the AtmosphereException class.
 */
//------------------------------------------------------------------------------
#include "AtmosphereException.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  AtmosphereException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs an AtmosphereException object (default constructor).
 */
//------------------------------------------------------------------------------

AtmosphereException::AtmosphereException(std::string details) :
BaseException  ("Atmosphere model exception: ", details)
{

}

