//$Id: EventException.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                              EventException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2009/12/07
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Exception class used by the Event hierarchy.
 */
//------------------------------------------------------------------------------


#include "EventException.hpp" // class's header file

//------------------------------------------------------------------------------
//  EventException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs EventException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
EventException::EventException(const std::string &details) :
    BaseException           ("Event Exception: ", details)
{
}


//------------------------------------------------------------------------------
//  ~EstimatorException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
EventException::~EventException()
{
}


//------------------------------------------------------------------------------
//  EventException(const EventException &ce)
//------------------------------------------------------------------------------
/**
 * Constructs EventException instance (copy constructor).
 *
 * @param ce Instance that gets copied.
 */
//------------------------------------------------------------------------------
EventException::EventException(const EventException &ce) :
    BaseException       (ce)
{}

