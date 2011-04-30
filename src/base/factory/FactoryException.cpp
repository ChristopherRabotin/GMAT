//$Id$
//------------------------------------------------------------------------------
//                              FactoryException
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
// Created: 2003/09/30
//
/**
 * Implementation code for the FactoryException class.
 */
//------------------------------------------------------------------------------
#include "FactoryException.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  FactoryException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs an FactoryException object (default constructor).
 */
//------------------------------------------------------------------------------

FactoryException::FactoryException(std::string details) :
BaseException  ("Factory (sub)class exception: ", details)
{
   
}

