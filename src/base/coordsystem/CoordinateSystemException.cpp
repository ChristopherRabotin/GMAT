//$Id$
//------------------------------------------------------------------------------
//                              CoordinateSystemException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
*
* @param details details to add to the exception message (defaults to "").
 */
//------------------------------------------------------------------------------

CoordinateSystemException::CoordinateSystemException(std::string details) :
BaseException  ("CoordinateSystem exception: ", details)
{

}

