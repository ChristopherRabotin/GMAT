//$Id$
//------------------------------------------------------------------------------
//                              FunctionFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
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
// number NNG04CC06P.
//
// Author: Linda Jun, NASA/GSFC
// Created: 2006/04/05
//
/**
 * Implementation code for the FunctionFactory class, responsible for creating 
 * Function objects.
 */
//------------------------------------------------------------------------------

#include "FunctionFactory.hpp"
#include "StringUtil.hpp"          // for Capitalize()
#include "MessageInterface.hpp"

// include list of Function classes here
#include "GetLastState.hpp"
#include "GetEphemStates.hpp"
#include "SetSeed.hpp"

//#define DEBUG_MATH_FACTORY 1

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Function class 
 *
 * @param <ofType>   the Function object to create and return.
 * @param <withName> the name of the new object.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
Function* FunctionFactory::CreateObject(const std::string &ofType,
                                        const std::string &withName)
{
   return CreateFunction(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateFunction(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Function class 
 *
 * @param <ofType>   the Function object to create and return.
 * @param <withName> the name of the new object.
 *
 * @return The new object.
 */
//------------------------------------------------------------------------------
Function* FunctionFactory::CreateFunction(const std::string &ofType,
                                      const std::string &withName)
{
   Function *builtinFunction = NULL;
   
   #if DEBUG_MATH_FACTORY
   MessageInterface::ShowMessage
      ("FunctionFactory::CreateFunction() ofType=%s, withName=%s\n",
       ofType.c_str(), withName.c_str());
   #endif
   
   // Create builtin function
   if (ofType == "GetLastState")
      builtinFunction = new GetLastState(ofType, withName);
   else if (ofType == "GetEphemStates")
      builtinFunction = new GetEphemStates(ofType, withName);
   else if (ofType == "SetSeed")
      builtinFunction = new SetSeed(ofType, withName);
   
   return builtinFunction;
}


//------------------------------------------------------------------------------
//  FunctionFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FunctionFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
FunctionFactory::FunctionFactory()
   : Factory(Gmat::FUNCTION)
{
   isCaseSensitive = true;
   
   if (creatables.empty())
      BuildCreatables();

   GmatType::RegisterType(Gmat::FUNCTION, "Function");
}


//------------------------------------------------------------------------------
//  FunctionFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FunctionFactory.
 *
 * @param <createList> list of creatable Function objects
 */
//------------------------------------------------------------------------------
FunctionFactory::FunctionFactory(StringArray createList)
   : Factory(createList, Gmat::FUNCTION)
{
   isCaseSensitive = true;
}


//------------------------------------------------------------------------------
//  FunctionFactory(const FunctionFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class FunctionFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
FunctionFactory::FunctionFactory(const FunctionFactory &fact)
   : Factory(fact)
{
   isCaseSensitive = true;
   
   if (creatables.empty())
      BuildCreatables();
}


//------------------------------------------------------------------------------
//  FunctionFactory& operator= (const FunctionFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the FunctionFactory base class.
 *
 * @param <fact> the FunctionFactory object whose data to assign to "this" factory.
 *
 * @return "this" FunctionFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
FunctionFactory& FunctionFactory::operator=(const FunctionFactory &fact)
{
   Factory::operator=(fact);
   isCaseSensitive = true;
   return *this;
}


//------------------------------------------------------------------------------
// ~FunctionFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the FunctionFactory class.
 */
//------------------------------------------------------------------------------
FunctionFactory::~FunctionFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
/**
 * Fills in creatable objects
 */
//------------------------------------------------------------------------------
void FunctionFactory::BuildCreatables()
{
   // The FIRST letter of function name can be either lower or upper case.
   
   creatables.push_back("GetLastState");
   creatables.push_back("GetEphemStates");
   creatables.push_back("SetSeed");
}

