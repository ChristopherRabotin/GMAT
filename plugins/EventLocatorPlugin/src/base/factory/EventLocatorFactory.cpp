//$Id: EventLocatorFactory.cpp 1914 2011-11-16 19:06:27Z djconway@NDC $
//------------------------------------------------------------------------------
//                           EventLocatorFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 6, 2011
//
/**
 * Implementation of the factory used to create event locators in this plugin
 */
//------------------------------------------------------------------------------


#include "EventLocatorFactory.hpp"
#include "ContactLocator.hpp"
#include "EclipseLocator.hpp"

//#define INCLUDE_CONTACT

//------------------------------------------------------------------------------
// EventLocatorFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
EventLocatorFactory::EventLocatorFactory() :
   Factory           (Gmat::EVENT_LOCATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("EclipseLocator");
#ifdef INCLUDE_CONTACT
      creatables.push_back("ContactLocator");
#endif
   }
}


//------------------------------------------------------------------------------
// ~EventLocatorFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EventLocatorFactory::~EventLocatorFactory()
{
}


//------------------------------------------------------------------------------
// EventLocatorFactory(const EventLocatorFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
EventLocatorFactory::EventLocatorFactory(const EventLocatorFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("EclipseLocator");
#ifdef INCLUDE_CONTACT
      creatables.push_back("ContactLocator");
#endif
   }
}


//------------------------------------------------------------------------------
// EventLocatorFactory& operator=(const EventLocatorFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
EventLocatorFactory& EventLocatorFactory::operator=(
      const EventLocatorFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("EclipseLocator");
#ifdef INCLUDE_CONTACT
         creatables.push_back("ContactLocator");
#endif
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// EventLocator* CreateEventLocator(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for EventLocators
 *
 * @param ofType The subtype of the event locator
 * @param withName The new locator's name
 *
 * @return A newly created EventLocator (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
EventLocator* EventLocatorFactory::CreateEventLocator(const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "EclipseLocator")
      return new EclipseLocator(withName);
#ifdef INCLUDE_CONTACT
   else if (ofType == "ContactLocator")
      return new ContactLocator(withName);
#endif
   // add more here .......

   return NULL;   // doesn't match any known type of EventLocator
}
