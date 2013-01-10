//$Id$
//------------------------------------------------------------------------------
//                         StationFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: January 10, 2013
//
/**
 * Implementation of a factory used to create (ground)stations
 */
//------------------------------------------------------------------------------


#include "StationFactory.hpp"

// Here you add include statements for each class that can be instantiated in
// this factory.  The example here is a SampleClass object.
#include "GroundStation.hpp"

//------------------------------------------------------------------------------
// StationFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
StationFactory::StationFactory() :
   Factory           (Gmat::SPACE_POINT)
{
   if (creatables.empty())
   {
      creatables.push_back("GroundStation");
   }
}


//------------------------------------------------------------------------------
// ~StationFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
StationFactory::~StationFactory()
{
}


//------------------------------------------------------------------------------
// StationFactory(const StationFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
StationFactory::StationFactory(const StationFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("GroundStation");
   }
}


//------------------------------------------------------------------------------
// StationFactory& operator=(const StationFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
StationFactory& StationFactory::operator=(
      const StationFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("GroundStation");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for GMAT commands
 *
 * @param ofType The subtype of the command
 * @param withName The new command's name
 *
 * @return A newly created GmatCommand (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
SpacePoint* StationFactory::CreateSpacePoint(const std::string &ofType,
                                            const std::string &withName)
{
   if (ofType == "GroundStation")
      return new GroundStation(withName);
   // add more here .......

   return NULL;   // doesn't match any type of Command known by this factory
}
