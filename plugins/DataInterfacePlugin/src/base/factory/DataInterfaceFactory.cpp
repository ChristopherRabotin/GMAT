//$Id$
//------------------------------------------------------------------------------
//                         DataInterfaceFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: September 20, 2012
//
/**
 * Implementation of a factory used in a GMAT plugin
 */
//------------------------------------------------------------------------------


#include "DataInterfaceFactory.hpp"

#include "FileInterface.hpp"

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
   Factory           (Gmat::INTERFACE)
{
   if (creatables.empty())
   {
      creatables.push_back("FileInterface");
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
      creatables.push_back("FileInterface");
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
         creatables.push_back("FileInterface");
      }
   }

   return *this;
}

Interface* DataInterfaceFactory::CreateInterface(const std::string& ofType,
      const std::string& withName)
{
   Interface *retval = NULL;

   if (ofType == "FileInterface")
      return new FileInterface(withName);

   return retval;
}

//------------------------------------------------------------------------------
//GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Returns a GmatBase object pointer to a new object.
 *
 * @param <ofType>   specific type of Spacecraft object to create.
 * @param <withName> name to give to the newly created Spacecraft object.
 *
 * @return pointer to a new Spacecraft object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 * objects of the requested type.
 */
//------------------------------------------------------------------------------
GmatBase* DataInterfaceFactory::CreateObject(const std::string &ofType,
                                const std::string &withName)
{
   return CreateInterface(ofType, withName);
}

