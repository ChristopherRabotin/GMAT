//$Id$
//------------------------------------------------------------------------------
//                         DataInterfaceFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2012 Thinking Systems, Inc.
// Free usage granted to all users; this is shell code.  Adapt and enjoy.  
// Attribution is appreciated.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: September 20, 2012
//
/**
 * Implementation of a factory used in a GMAT plugin
 *
 * Replace code in the indicated places for your Factory
 */
//------------------------------------------------------------------------------


#include "DataInterfaceFactory.hpp"

//------------------------------------------------------------------------------
// DataInterfaceFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
DataInterfaceFactory::DataInterfaceFactory() :
   Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
//      creatables.push_back("Save");
   }
}


//------------------------------------------------------------------------------
// ~DataInterfaceFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DataInterfaceFactory::~DataInterfaceFactory()
{
}


//------------------------------------------------------------------------------
// DataInterfaceFactory(const DataInterfaceFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
DataInterfaceFactory::DataInterfaceFactory(const DataInterfaceFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
//      creatables.push_back("Save");
   }
}


//------------------------------------------------------------------------------
// DataInterfaceFactory& operator=(const DataInterfaceFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
DataInterfaceFactory& DataInterfaceFactory::operator=(
      const DataInterfaceFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         // Replace the SampleClass string here with your class name.  For multiple 
         // classes of the same type, push back multiple names here
//         creatables.push_back("Save");
      }
   }

   return *this;
}

Interface* DataInterfaceFactory::CreateInterface(const std::string& ofType,
      const std::string& withName)
{
   Interface *retval = NULL;
   return retval;
}
