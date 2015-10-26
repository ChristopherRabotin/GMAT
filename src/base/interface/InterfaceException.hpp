//$Id$
//------------------------------------------------------------------------------
//                                  InterfaceException
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
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
// Author: Wendy Shoan/GSFC
// Created: 2006.09.15
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Exception class used by the Interface classes.
 */
//------------------------------------------------------------------------------


#ifndef InterfaceException_hpp
#define InterfaceException_hpp

#include "BaseException.hpp" // inheriting class's header file

/**
 * Exceptions thrown from the interface subsystem
 */
class GMAT_API InterfaceException : public BaseException
{
   public:
      // class constructor
      InterfaceException(const std::string &details = "");
      // class destructor
      ~InterfaceException();
      // Copy constructor
      InterfaceException(const InterfaceException &ce);
};

#endif // InterfaceException_hpp

