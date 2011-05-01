//$Id: EstimatorException.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             EstimatorException
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
 * Exception class used by the Estimator hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef EstimatorException_hpp
#define EstimatorException_hpp

#include "estimation_defs.hpp"
#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown from the estimation subsystem
 */
class ESTIMATION_API EstimatorException : public BaseException
{
public:
   // class constructor
   EstimatorException(const std::string &details = "");
   // class destructor
   ~EstimatorException();
   // Copy constructor
   EstimatorException(const EstimatorException &ce);
};

#endif // EstimatorException_hpp

