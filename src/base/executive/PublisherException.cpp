//$Id$
//------------------------------------------------------------------------------
//                              PublisherException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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

