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


#include "TrackingSystemFactory.hpp"

// Types created by this factory
#include "DSNTrackingSystem.hpp"
#include "USNTrackingSystem.hpp"
#include "OpticalTrackingSystem.hpp"
#include "TDRSSTrackingSystem.hpp"

//------------------------------------------------------------------------------
// TrackingSystemFactory()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
TrackingSystemFactory::TrackingSystemFactory() :
   Factory           (Gmat::TRACKING_SYSTEM)
{
   if (creatables.empty())
   {
      creatables.push_back("DSNTrackingSystem");
      creatables.push_back("USNTrackingSystem");
      creatables.push_back("OpticalTrackingSystem");
      creatables.push_back("TDRSSTrackingSystem");
   }
}


//------------------------------------------------------------------------------
// ~TrackingSystemFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TrackingSystemFactory::~TrackingSystemFactory()
{
}


TrackingSystemFactory& TrackingSystemFactory::operator=(
         const TrackingSystemFactory & tsf)
{
   if (this != &tsf)
   {
      Factory::operator=(tsf);

      if (creatables.empty())
      {
         creatables.push_back("DSNTrackingSystem");
         creatables.push_back("USNTrackingSystem");
         creatables.push_back("OpticalTrackingSystem");
         creatables.push_back("TDRSSTrackingSystem");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// TrackingSystemFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * Alternatice constructor
 *
 * @param createList List of creatable objects
 */
//------------------------------------------------------------------------------
TrackingSystemFactory::TrackingSystemFactory(StringArray createList) :
   Factory           (createList)
{
   if (creatables.empty())
   {
      creatables.push_back("DSNTrackingSystem");
      creatables.push_back("USNTrackingSystem");
      creatables.push_back("OpticalTrackingSystem");
      creatables.push_back("TDRSSTrackingSystem");
   }
}



//------------------------------------------------------------------------------
// TrackingSystemFactory(const TrackingSystemFactory & tsf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param tsf The TrackingSystemFactory that is used to build this one
 */
//------------------------------------------------------------------------------
TrackingSystemFactory::TrackingSystemFactory(const TrackingSystemFactory & tsf):
   Factory           (tsf)
{
   if (creatables.empty())
   {
      creatables.push_back("DSNTrackingSystem");
      creatables.push_back("USNTrackingSystem");
      creatables.push_back("OpticalTrackingSystem");
      creatables.push_back("TDRSSTrackingSystem");
   }
}


//------------------------------------------------------------------------------
// TrackingSystem* CreateTrackingSystem(const std::string &ofType,
//                                      const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates a TrackingSystem object.
 *
 * @param ofType   specific type of TrackingSystem object to create.
 * @param withName name to give to the newly created TrackingSystem object.
 *
 * @return pointer to a new TrackingSystem object.
 *
 * @exception FactoryException Thrown if the factory does not create
 *                             objects of type TrackingSystem.
 */
//------------------------------------------------------------------------------
TrackingSystem* TrackingSystemFactory::CreateTrackingSystem(
         const std::string &ofType, const std::string &withName)
{
   if (ofType == "DSNTrackingSystem")
      return new DSNTrackingSystem(withName);
   if (ofType == "USNTrackingSystem")
      return new USNTrackingSystem(withName);
   if (ofType == "OpticalTrackingSystem")
      return new OpticalTrackingSystem(withName);
   if (ofType == "TDRSSTrackingSystem")
      return new TDRSSTrackingSystem(withName);

   return Factory::CreateTrackingSystem(ofType, withName);
}
