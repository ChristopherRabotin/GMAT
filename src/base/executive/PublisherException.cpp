//$Id$
//------------------------------------------------------------------------------
//                              PublisherException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2004/07/29
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P.
//
/**
 * Exception class used by the Publisher.
 */
//------------------------------------------------------------------------------


#include "PublisherException.hpp" // class's header file

// class constructor
PublisherException::PublisherException(std::string details) :
    BaseException   ("Publisher Exception: ", details)
{
}

// class destructor
PublisherException::~PublisherException()
{
}

