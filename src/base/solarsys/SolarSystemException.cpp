//$Id$
//------------------------------------------------------------------------------
//                              SolarSystemException
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
// Created: 2004/03/02
//
/**
 * Implementation code for the SolarSystemException class.
 */
//------------------------------------------------------------------------------
#include "SolarSystemException.hpp"

//------------------------------------------------------------------------------
//  public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  SolarSystemException(std::string details)
//------------------------------------------------------------------------------
/**
* Constructs an SolarSystemException object (default constructor).
 */
//------------------------------------------------------------------------------

SolarSystemException::SolarSystemException(std::string details) :
BaseException  ("SolarSystem exception: ", details)
{

}
