//$Id$
//------------------------------------------------------------------------------
//                             TrackingDataFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/02/23 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the TrackingDataFactory class
 */
//------------------------------------------------------------------------------


#include "TrackingDataFactory.hpp"

// Types created by this factory
#include "TrackingData.hpp"


TrackingDataFactory::TrackingDataFactory() :
   Factory           (Gmat::TRACKING_DATA)
{
   if (creatables.empty())
   {
      creatables.push_back("TrackingData");
   }
}

TrackingDataFactory::~TrackingDataFactory()
{
}


TrackingDataFactory& TrackingDataFactory::operator=(
         const TrackingDataFactory & tsf)
{
   if (this != &tsf)
   {
      Factory::operator=(tsf);

      if (creatables.empty())
      {
         creatables.push_back("TrackingData");
      }
   }

   return *this;
}



TrackingDataFactory::TrackingDataFactory(StringArray createList) :
   Factory           (createList)
{
   if (creatables.empty())
   {
      creatables.push_back("TrackingData");
   }
}



TrackingDataFactory::TrackingDataFactory(const TrackingDataFactory & tsf):
   Factory           (tsf)
{
   if (creatables.empty())
   {
      creatables.push_back("TrackingData");
   }
}


//------------------------------------------------------------------------------
// TrackingData* CreateTrackingData(const std::string &ofType,
//                                         const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates a TrackingData object.
 *
 * Must be implemented by derived classes that create TrackingData objects -
 * in that case, it returns a new TrackingData object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type TrackingData.
 *
 * @param <ofType>   specific type of TrackingData object to create.
 * @param <withName> name to give to the newly created TrackingData object.
 *
 * @return pointer to a new TrackingData object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type TrackingData.
 */
//------------------------------------------------------------------------------
TrackingData* TrackingDataFactory::CreateTrackingData(
         const std::string &ofType, const std::string &withName)
{
   if (ofType == "TrackingData")
      return new TrackingData(withName);

   return Factory::CreateTrackingData(ofType, withName);
}
