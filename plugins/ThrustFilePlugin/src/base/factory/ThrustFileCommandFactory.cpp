//------------------------------------------------------------------------------
//                        ThrustFileCommandFactory
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
/**
 *
 */
//------------------------------------------------------------------------------


#include "ThrustFileCommandFactory.hpp"
#include "BeginFileThrust.hpp"
#include "EndFileThrust.hpp"


//------------------------------------------------------------------------------
// ThrustFileCommandFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
ThrustFileCommandFactory::ThrustFileCommandFactory() :
   Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("BeginFileThrust");
      creatables.push_back("EndFileThrust");
   }
}


//------------------------------------------------------------------------------
// ~ThrustFileCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ThrustFileCommandFactory::~ThrustFileCommandFactory()
{
}


//------------------------------------------------------------------------------
// ThrustFileCommandFactory(const ThrustFileCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
ThrustFileCommandFactory::ThrustFileCommandFactory(const ThrustFileCommandFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("BeginFileThrust");
      creatables.push_back("EndFileThrust");
   }
}


//------------------------------------------------------------------------------
// ThrustFileCommandFactory& operator=(const ThrustFileCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
ThrustFileCommandFactory& ThrustFileCommandFactory::operator=(
      const ThrustFileCommandFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("BeginFileThrust");
         creatables.push_back("EndFileThrust");
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
GmatCommand* ThrustFileCommandFactory::CreateCommand(const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "BeginFileThrust")
      return new BeginFileThrust(withName);
   if (ofType == "EndFileThrust")
      return new EndFileThrust(withName);

   return NULL;
}
