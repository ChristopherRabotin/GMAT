//$Id$
//------------------------------------------------------------------------------
//                            AssetFactory
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
// Author: Darrel Conway
// Created: 2008/08/12
//
/**
 *  This class is the factory class for assets.  
 */
//------------------------------------------------------------------------------

#include "AssetFactory.hpp"

// Creatable assets
#include "GroundStation.hpp"

#include "MessageInterface.hpp"  // temporary


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  AssetFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AssetFactory. 
 * (default constructor)
 */
//------------------------------------------------------------------------------
AssetFactory::AssetFactory() :
   Factory     (Gmat::SPACE_POINT)
{
   if (creatables.empty())
   {
      creatables.push_back("GroundStation");
   }
}

//------------------------------------------------------------------------------
// ~VF13Factory()
//------------------------------------------------------------------------------
/**
 * Destructor for the AssetFactory class.
 */
//------------------------------------------------------------------------------
AssetFactory::~AssetFactory()
{
}


//------------------------------------------------------------------------------
//  AssetFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AssetFactory.
 *
 * @param createList List of creatable asset objects
 */
//------------------------------------------------------------------------------
AssetFactory::AssetFactory(StringArray createList) :
   Factory(createList, Gmat::SPACE_POINT)
{
}


//------------------------------------------------------------------------------
//  AssetFactory(const AssetFactory& af)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AssetFactory.  (copy constructor)
 *
 * @param af The factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
AssetFactory::AssetFactory(const AssetFactory& af) :
   Factory  (af)
{
   if (creatables.empty())
   {
      creatables.push_back("GroundStation");
   }
}

//------------------------------------------------------------------------------
//  AssetFactory& operator= (const AssetFactory& af)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the AssetFactory class.
 *
 * @param <af> the VF13Factory object that is copied.
 *
 * @return "this" AssetFactory with data set to match the input factory (af).
 */
//------------------------------------------------------------------------------
AssetFactory& AssetFactory::operator=(const AssetFactory& af)
{
   Factory::operator=(af);
   return *this;
}

//------------------------------------------------------------------------------
// SpacePoint* AssetFactory::CreateSpacePoint(const std::string &ofType,
//                                            const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested SpacePoint class. 
 *
 * @param <ofType> type of SpacePoint object to create and return.
 * @param <withName> the name for the newly-created SpacePoint object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
SpacePoint* AssetFactory::CreateSpacePoint(const std::string &ofType,
                                           const std::string &withName)
{
   if (ofType == "GroundStation")
      return new GroundStation(withName);
    
   return NULL;
}
