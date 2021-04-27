//$Id: EstimatorException.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                              EstimatorException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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

