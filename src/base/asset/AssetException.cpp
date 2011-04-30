//$Id$
//------------------------------------------------------------------------------
//                              AssetException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Wendy Shoan
// Created: 2008.09.22
//
/**
 * Implementation code for the AssetException class.
 */
//------------------------------------------------------------------------------
#include "AssetException.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  AssetException(std::string details)
//------------------------------------------------------------------------------
/**
* Constructs an AssetException object (default constructor).
 */
//------------------------------------------------------------------------------

AssetException::AssetException(std::string details) :
BaseException  ("Asset exception: ", details)
{

}

