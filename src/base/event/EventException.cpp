//$Id$
//------------------------------------------------------------------------------
//                               EventException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2011/09/01
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
/**
 * Exception class used by the Located Event hierarchy.
 */
//------------------------------------------------------------------------------


#include "EventException.hpp"


//------------------------------------------------------------------------------
//  EventException(const std::string &details)
//------------------------------------------------------------------------------
/**
 * Constructs EventException instance (default constructor).
 * 
 * @param <details> A message providing the details of the exception.
 */
//------------------------------------------------------------------------------
EventException::EventException(const std::string &details) :
   BaseException           ("Event Exception Thrown: ", details)
{
}


//------------------------------------------------------------------------------
//  ~EventException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
EventException::~EventException()
{
}


//------------------------------------------------------------------------------
//  EventException(const EventException &be)
//------------------------------------------------------------------------------
/**
 * Constructs EventException instance (copy constructor).
 *
 * @param be The instance that is copied.
 */
//------------------------------------------------------------------------------
EventException::EventException(const EventException &be) :
    BaseException       (be)
{
}
