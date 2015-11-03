//$Id: EventFactory.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         EventFactory
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
// Created: 2009/12/09
//
/**
 * Factory used to create Event objects
 */
//------------------------------------------------------------------------------


#include "EventFactory.hpp"
#include "LightTimeCorrection.hpp"


//------------------------------------------------------------------------------
// EventFactory()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EventFactory::EventFactory() :
   Factory        (Gmat::EVENT)
{
   if (creatables.empty())
   {
      creatables.push_back("LightTimeCorrection");
   }
}


//------------------------------------------------------------------------------
// ~EventFactory()
//------------------------------------------------------------------------------
/**
 * Factory destructor
 */
//------------------------------------------------------------------------------
EventFactory::~EventFactory()
{
}


//------------------------------------------------------------------------------
// EventFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * Constructor that configures based on an object creation list
 *
 * @param createList The list of creatable objects
 *
 * @note This seems to me to be impractical to use --  how can the resulting
 *       constructor calls be set up without some additional information?  Once
 *       that information is available, why not just use the default constructor
 *       rather than pass in a list and somehow map it to object construction
 *       by hand?
 */
//------------------------------------------------------------------------------
EventFactory::EventFactory(StringArray createList) :
   Factory     (createList, Gmat::EVENT)
{
   if (creatables.empty())
   {
      creatables.push_back("LightTimeCorrection");
   }
}


//------------------------------------------------------------------------------
// EventFactory(const EventFactory& fact)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fact The factory providing configuration data for this one
 */
//------------------------------------------------------------------------------
EventFactory::EventFactory(const EventFactory& fact) :
   Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("LightTimeCorrection");
   }
}


//------------------------------------------------------------------------------
// EventFactory& operator=(const EventFactory & fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fact The Factory that is setting properties for this one
 *
 * @return This Factory, configured to match fact
 */
//------------------------------------------------------------------------------
EventFactory & EventFactory::operator=(const EventFactory& fact)
{
   if (&fact != this)
   {
      Factory::operator=(fact);

      if (creatables.empty())
      {
         creatables.push_back("LightTimeCorrection");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// Event* CreateEvent(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates the Event objects for the Estimation plugin.
 *
 * @param ofType   specific type of Event object to create.
 * @param withName name to give to the newly created Event object.
 *
 * @return pointer to a new Event object.
 *
 * @exception FactoryException thrown if the factory does not create
 *                             objects of type Event.
 */
//------------------------------------------------------------------------------
Event* EventFactory::CreateEvent(const std::string &ofType,
      const std::string &withName)
{
   Event *retval = NULL;

   if (ofType == "LightTimeCorrection")
      retval = new LightTimeCorrection(withName);

   return retval;
}
