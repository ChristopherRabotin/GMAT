//$Id$
//------------------------------------------------------------------------------
//                             TrackingDataFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
