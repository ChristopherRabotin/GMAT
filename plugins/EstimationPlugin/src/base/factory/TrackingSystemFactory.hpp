//$Id$
//------------------------------------------------------------------------------
//                             TrackingSystemFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/02/22 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the TrackingSystemFactory class
 */
//------------------------------------------------------------------------------


#ifndef TrackingSystemFactory_hpp
#define TrackingSystemFactory_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"

/**
 * The factory used to make TrackingSystem objects.
 */
class ESTIMATION_API TrackingSystemFactory : public Factory
{
public:
   TrackingSystemFactory();
   virtual ~TrackingSystemFactory();
   TrackingSystemFactory(StringArray createList);
   TrackingSystemFactory(const TrackingSystemFactory& tsf);
   TrackingSystemFactory& operator=(const TrackingSystemFactory& tsf);

   virtual TrackingSystem* CreateTrackingSystem(const std::string &ofType,
            const std::string &withName = "");
};

#endif /* TrackingSystemFactory_hpp */
