//$Header$
//------------------------------------------------------------------------------
//                              CoordinateSystemException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy Shoan
// Created: 2004/12/22
//
/**
 * Implementation code for the CoordinateSystemException class.
 */
//------------------------------------------------------------------------------
#include "CoordinateSystemException.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CoordinateSystemException(std::string details)
//------------------------------------------------------------------------------
/**
* Constructs an CoordinateSystemException object (default constructor).
 */
//------------------------------------------------------------------------------

CoordinateSystemException::CoordinateSystemException(std::string details) :
BaseException  ("CoordinateSystem exception: ", details)
{

}

