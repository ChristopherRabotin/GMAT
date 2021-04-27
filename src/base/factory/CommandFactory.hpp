//$Id$
//------------------------------------------------------------------------------
//                         CommandFactory
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/09
//
/**
 *  This class is the factory class for commands.  
 */
//------------------------------------------------------------------------------
#ifndef CommandFactory_hpp
#define CommandFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "GmatCommand.hpp"

class GMAT_API CommandFactory : public Factory
{
public:
   GmatBase*     CreateObject(const std::string &ofType,
                              const std::string &withName = "");
   GmatCommand*  CreateCommand(const std::string &ofType,
                               const std::string &withName = "");

   // default constructor
   CommandFactory();
   // constructor
   CommandFactory(StringArray createList);
   // copy constructor
   CommandFactory(const CommandFactory& fact);
   // assignment operator
   CommandFactory& operator= (const CommandFactory& fact);

   // destructor
   ~CommandFactory();

   // method to return list of types of objects that this factory can create
   virtual StringArray      GetListOfCreatableObjects(
                                  const std::string &qualifier = "");

protected:
   // protected data
   /// List of commands that toggle from object mode to command mode
   StringArray              sequenceStarters;

private:

   // private data


};

#endif // CommandFactory_hpp

