//$Header$
//------------------------------------------------------------------------------
//                              AtmosphereException
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
//  AtmosphereException(const std::string &details)
//------------------------------------------------------------------------------
/**
 * Constructs an AtmosphereException object (default constructor).
 *
 * @param <details> Detailed message about why the exception was thrown.
 */
//------------------------------------------------------------------------------

AtmosphereException::AtmosphereException(const std::string &details) :
BaseException  ("Atmosphere model exception: ", details)
{
}

