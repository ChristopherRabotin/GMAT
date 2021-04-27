//$Id$
//------------------------------------------------------------------------------
//                            SmootherFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Author: John McGreevy, Emergent Space Technologies, Inc.
// Created: 2019/07/12
//
/**
 *  This class is the factory class for Smoother.
 */
//------------------------------------------------------------------------------
#ifndef SmootherFactory_hpp
#define SmootherFactory_hpp

#include "kalman_defs.hpp"
#include "Factory.hpp"
#include "Smoother.hpp"

class KALMAN_API SmootherFactory : public Factory
{
public:
   GmatBase*    CreateObject(const std::string &ofType,
                             const std::string &withName = "");
   Smoother*    CreateSmoother(const std::string &ofType,
                               const std::string &withName = "");

   // default constructor
   SmootherFactory();
   // constructor
   SmootherFactory(StringArray createList);
   // copy constructor
   SmootherFactory(const SmootherFactory &fact);
   // assignment operator
   SmootherFactory& operator= (const SmootherFactory &fact);

   // destructor
   ~SmootherFactory();

protected:
   // protected data

private:
   // private data


};

#endif // SmootherFactory_hpp

