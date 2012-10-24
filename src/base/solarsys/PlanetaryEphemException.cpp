//$Id$
//------------------------------------------------------------------------------
//                              PlanetaryEphemException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2004/02/17
//
/**
 * Implementation code for the PlanetaryEphemException class.
 */
//------------------------------------------------------------------------------
#include "PlanetaryEphemException.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  PlanetaryEphemException(std::string details)
//------------------------------------------------------------------------------
/**
* Constructs an PlanetaryEphemException object (default constructor).
 */
//------------------------------------------------------------------------------
PlanetaryEphemException::PlanetaryEphemException(const std::string &details) :
BaseException  ("PlanetaryEphem (sub)class exception: ", details)
{

}
