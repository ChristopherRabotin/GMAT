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


#ifndef TrackingDataFactory_hpp
#define TrackingDataFactory_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"

/**
 * The factory used to make TrackingData objects.
 */
class ESTIMATION_API TrackingDataFactory : public Factory
{
public:
   TrackingDataFactory();
   virtual ~TrackingDataFactory();
   TrackingDataFactory(StringArray createList);
   TrackingDataFactory(const TrackingDataFactory& tsf);
   TrackingDataFactory& operator=(const TrackingDataFactory& tsf);

   virtual TrackingData* CreateTrackingData(const std::string &ofType,
            const std::string &withName = "");
};

#endif /* TrackingSystemFactory_hpp */
