//$Id$
//------------------------------------------------------------------------------
//                               InterpolatorException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number #####
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2009/09/28
//
//------------------------------------------------------------------------------

#include "InterpolatorException.hpp"

// class constructor
InterpolatorException::InterpolatorException(std::string details) :
   BaseException("Interpolator Exception: ", details)
{
}

// class destructor
InterpolatorException::~InterpolatorException()
{
}

