//$Id: ObTypeFactory.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                             ObTypeFactory
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/06/19
//
/**
 * Factory used to create ObType objects
 */
//------------------------------------------------------------------------------


#include "ObTypeFactory.hpp"

// Supported ObTypes
#include "GmatObType.hpp"


//-----------------------------------------------------------------------------
// ObTypeFactory()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
ObTypeFactory::ObTypeFactory() :
   Factory     (Gmat::OBTYPE)
{
   if (creatables.empty())
   {
      creatables.push_back("GMATInternal");
   }
}


//-----------------------------------------------------------------------------
// ~ObTypeFactory()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
ObTypeFactory::~ObTypeFactory()
{
}


//-----------------------------------------------------------------------------
// ObTypeFactory(StringArray createList)
//-----------------------------------------------------------------------------
/**
 * Secondary constructor designed to override the names of the creatables
 *
 * @param createList The new list of creatable objects
 */
//-----------------------------------------------------------------------------
ObTypeFactory::ObTypeFactory(StringArray createList) :
   Factory        (createList, Gmat::OBTYPE)
{
   if (creatables.empty())
   {
      creatables.push_back("GMATInternal");
   }
}


//-----------------------------------------------------------------------------
// ObTypeFactory(const ObTypeFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fact The factory that gets copied here
 */
//-----------------------------------------------------------------------------
ObTypeFactory::ObTypeFactory(const ObTypeFactory& fact) :
   Factory        (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("GMATInternal");
   }
}


//-----------------------------------------------------------------------------
// ObTypeFactory& operator= (const ObTypeFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fact The factory that gets copied here
 *
 * @return This factory, configured to match fact
 */
//-----------------------------------------------------------------------------
ObTypeFactory& ObTypeFactory::operator= (const ObTypeFactory& fact)
{
   if (&fact != this)
   {
      Factory::operator=(fact);

      if (creatables.empty())
      {
         creatables.push_back("GMATInternal");
      }
   }

   return *this;
}


//-----------------------------------------------------------------------------
// ObType* CreateObType(const std::string &ofType, const std::string &withName)
//-----------------------------------------------------------------------------
/**
 * Creates an ObType object with the specified name
 *
 * @param ofType The scripted type of the object
 * @param withName The name of the new object
 *
 * @return A pointer to the new object, or NULL if no new object was created
 */
//-----------------------------------------------------------------------------
ObType* ObTypeFactory::CreateObType(const std::string &ofType,
      const std::string &withName)
{
   ObType *retval = NULL;

   if (ofType == "GMATInternal")
      retval = new GmatObType(withName);

   return retval;
}
