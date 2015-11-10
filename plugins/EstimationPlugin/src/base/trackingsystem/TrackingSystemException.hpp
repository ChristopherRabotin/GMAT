//$Id: TrackingSystemException.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         TrackingSystemException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
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
// Created: 2004/07/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Exception class used by the TrackingSystem hierarchy.
 */
//------------------------------------------------------------------------------


#ifndef TrackingSystemException_hpp
#define TrackingSystemException_hpp

#include "estimation_defs.hpp"
#include "BaseException.hpp"


class ESTIMATION_API TrackingSystemException : public BaseException
{
public:
   //---------------------------------------------------------------------------
   // TrackingSystemException(const std::string &details)
   //---------------------------------------------------------------------------
   /**
    * Default constructor.
    *
    * @param <details> Message explaining why the exception was thrown.
    */
   //---------------------------------------------------------------------------
   TrackingSystemException(const std::string &details = "") :
      BaseException("Tracking System Exception Thrown: ", details)
   {
   }

   //---------------------------------------------------------------------------
   // ~TrackingSystemException()
   //---------------------------------------------------------------------------
   /**
    * Destructor.
    */
   //---------------------------------------------------------------------------
   ~TrackingSystemException()
   {
   }

   //---------------------------------------------------------------------------
   // TrackingSystemException(const TrackingSystemException &soe)
   //---------------------------------------------------------------------------
   /**
    * Copy constructor.
    *
    * @param <soe> Exception used to generate this one.
    */
   //---------------------------------------------------------------------------
   TrackingSystemException(const TrackingSystemException &soe) :
      BaseException(soe)
   {
   }
};

#endif // TrackingSystemException_hpp
