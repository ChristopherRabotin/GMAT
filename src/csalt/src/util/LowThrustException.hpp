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


#ifndef LowThrustException_hpp
#define LowThrustException_hpp

#include "BaseException.hpp"
#include "csaltdefs.hpp"          // For GMAT_API

/**
 * Exception class used to report issues with event location.
 */
class CSALT_API LowThrustException : public BaseException
{
public:
   LowThrustException(const std::string &details = "");
   //   virtual ~LowThrustException();
   LowThrustException(const LowThrustException &be);
};

#endif // LowThrustException_hpp
