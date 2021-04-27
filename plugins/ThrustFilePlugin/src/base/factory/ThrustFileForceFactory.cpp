//------------------------------------------------------------------------------
//                         ThrustFileForceFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
//
/**
 * Implementation of a factory used in a GMAT plugin
 */
//------------------------------------------------------------------------------


#include "ThrustFileForceFactory.hpp"

//------------------------------------------------------------------------------
// ThrustFileForceFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
ThrustFileForceFactory::ThrustFileForceFactory() :
   Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
//      creatables.push_back("Save");
   }
}


//------------------------------------------------------------------------------
// ~ThrustFileForceFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThrustFileForceFactory::~ThrustFileForceFactory()
{
}


//------------------------------------------------------------------------------
// ThrustFileForceFactory(const ThrustFileForceFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
ThrustFileForceFactory::ThrustFileForceFactory(const ThrustFileForceFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
//      creatables.push_back("Save");
   }
}


//------------------------------------------------------------------------------
// ThrustFileForceFactory& operator=(const ThrustFileForceFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
ThrustFileForceFactory& ThrustFileForceFactory::operator=(
      const ThrustFileForceFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
//         creatables.push_back("Save");
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
GmatCommand* ThrustFileForceFactory::CreateCommand(const std::string &ofType,
                                     const std::string &withName)
{
//   if (ofType == "Save")
//      return new Save();
   // add more here .......

   return NULL;   // doesn't match any type of Command known by this factory
}
