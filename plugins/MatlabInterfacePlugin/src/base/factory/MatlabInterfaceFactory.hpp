//$Id: MatlabInterfaceFactory.hpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                            MatlabInterfaceFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Linda Jun
// Created: 2010/03/30
//
/**
 *  Declaration code for the MatlabInterfaceFactory class.
 */
//------------------------------------------------------------------------------
#ifndef MatlabInterfaceFactory_hpp
#define MatlabInterfaceFactory_hpp

#include "matlabinterface_defs.hpp"
#include "Factory.hpp"
#include "Interface.hpp"

class MATLAB_API MatlabInterfaceFactory : public Factory
{
public:
   virtual Interface* CreateInterface(const std::string &ofType,
                                      const std::string &withName);
   
   /// default constructor
   MatlabInterfaceFactory();
   /// constructor
   MatlabInterfaceFactory(StringArray createList);
   /// copy constructor
   MatlabInterfaceFactory(const MatlabInterfaceFactory& fact);
   /// assignment operator
   MatlabInterfaceFactory& operator=(const MatlabInterfaceFactory& fact);
   
   virtual ~MatlabInterfaceFactory();
   
};

#endif // MatlabInterfaceFactory_hpp
