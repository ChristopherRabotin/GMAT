//$Id: EventLocatorFactory.hpp 1914 2011-11-16 19:06:27Z djconway@NDC $
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
 * Definition of the factory used to create event locators in this plugin
 */
//------------------------------------------------------------------------------


#ifndef EventLocatorFactory_hpp
#define EventLocatorFactory_hpp

#include "Factory.hpp"
#include "EventLocatorDefs.hpp"


/**
 * Factory class that creates EventLocator objects
 */
class LOCATOR_API EventLocatorFactory: public Factory
{
public:
   EventLocatorFactory();
   virtual ~EventLocatorFactory();
   EventLocatorFactory(const EventLocatorFactory& elf);
   EventLocatorFactory& operator=(const EventLocatorFactory& elf);

   virtual EventLocator* CreateEventLocator(const std::string &ofType,
                                        const std::string &withName);
};

#endif /* EventLocatorFactory_hpp */
