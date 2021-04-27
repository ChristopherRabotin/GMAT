//$Id$
//------------------------------------------------------------------------------
//                               LowThrustException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2015.06.22
//
/**
 * Exception class used by the Low Thrust subsystem.
 */
//------------------------------------------------------------------------------

#include "LowThrustException.hpp"

//------------------------------------------------------------------------------
//  LowThrustException(const std::string &details)
//------------------------------------------------------------------------------
/**
 * Constructs LowThrustException instance (default constructor).
 *
 * @param <details> A message providing the details of the exception.
 */
//------------------------------------------------------------------------------
LowThrustException::LowThrustException(const std::string &details) :
   BaseException("LowThrust Exception Thrown: ", details)
{
}

//------------------------------------------------------------------------------
//  LowThrustException(const LowThrustException &be)
//------------------------------------------------------------------------------
/**
 * Constructs LowThrustException instance (copy constructor).
 *
 * @param be The instance that is copied.
 */
//------------------------------------------------------------------------------
LowThrustException::LowThrustException(const LowThrustException &be) :
   BaseException(be)
{
}
