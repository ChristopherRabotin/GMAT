//$Header$
//------------------------------------------------------------------------------
//                              AttitudeException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan/GSFC
// Created: 2006.02.16
//
/**
 * Implementation code for the SolarSystemException class.
 */
//------------------------------------------------------------------------------
#include "AttitudeException.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  AttitudeException(std::string details)
//------------------------------------------------------------------------------
/**
* Constructs an AttitudeException object (default constructor).
 */
//------------------------------------------------------------------------------

AttitudeException::AttitudeException(std::string details) :
BaseException  ("Attitude exception: ", details)
{

}

