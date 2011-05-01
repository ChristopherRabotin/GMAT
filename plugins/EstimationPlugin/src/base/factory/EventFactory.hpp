//$Id: EventFactory.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         EventFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/12/09
//
/**
 * Factory used to build events
 */
//------------------------------------------------------------------------------


#ifndef EventFactory_hpp
#define EventFactory_hpp

/// Descriptor here
#include "estimation_defs.hpp"
#include "Factory.hpp"


class ESTIMATION_API EventFactory: public Factory
{
public:
   EventFactory();
   virtual ~EventFactory();
   EventFactory(StringArray createList);
   EventFactory(const EventFactory& fact);
   EventFactory& operator= (const EventFactory& fact);

   virtual Event* CreateEvent(const std::string &ofType,
         const std::string &withName = "");

};

#endif /* EventFactory_hpp */
