//$Id: EstimatorException.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                              EstimatorException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2009/08/11
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Exception class used by the Estimation hierarchy.
 */
//------------------------------------------------------------------------------


#include "EstimatorException.hpp" // class's header file

//------------------------------------------------------------------------------
//  EstimatorException(std::string details)
//------------------------------------------------------------------------------
/**
 * Constructs EstimatorException instance (default constructor).
 * 
 * @param details A message providing the details of the exception. 
 */
//------------------------------------------------------------------------------
EstimatorException::EstimatorException(const std::string &details) :
    BaseException           ("Estimator Exception: ", details)
{
}


//------------------------------------------------------------------------------
//  ~EstimatorException()
//------------------------------------------------------------------------------
/**
 * Class destructor.
 */
//------------------------------------------------------------------------------
EstimatorException::~EstimatorException()
{
}


//------------------------------------------------------------------------------
//  EstimatorException(const EstimatorException &ce)
//------------------------------------------------------------------------------
/**
 * Constructs EstimatorException instance (copy constructor).
 *
 * @param ce Instance that gets copied.
 */
//------------------------------------------------------------------------------
EstimatorException::EstimatorException(const EstimatorException &ce) :
    BaseException       (ce)
{}

