//$Id$
//------------------------------------------------------------------------------
//                              StationFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: January 10, 2013
//
//
/**
 * Definition of the factory used to create message writing commands in this
 * plugin
 */
//------------------------------------------------------------------------------


#ifndef StationFactory_hpp
#define StationFactory_hpp

#include "StationDefs.hpp"
#include "Factory.hpp"


/**
 * Factory class that creates station objects
 */
class STATION_API StationFactory : public Factory
{
public:
   StationFactory();
   virtual ~StationFactory();
   StationFactory(const StationFactory& elf);
   StationFactory& operator=(const StationFactory& elf);

   virtual GmatBase*        CreateObject(const std::string &ofType,
                                         const std::string &withName = "");        
   virtual SpacePoint*      CreateSpacePoint(const std::string &ofType,
                                             const std::string &withName = "");        
};

#endif /* StationFactory_hpp */
