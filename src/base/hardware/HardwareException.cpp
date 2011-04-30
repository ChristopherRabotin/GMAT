//$Id$
//------------------------------------------------------------------------------
//                                  HardwareException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2003/10/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Exception class used by the Hardware hierarchy.
 */
//------------------------------------------------------------------------------


#include "HardwareException.hpp"

HardwareException::HardwareException(std::string details) :
    BaseException           ("Hardware Exception Thrown: ", details)
{}


HardwareException::~HardwareException()
{}


HardwareException::HardwareException(const HardwareException &he) :
    BaseException       (he)
{}

