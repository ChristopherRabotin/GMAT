//$Id$
//------------------------------------------------------------------------------
//                         FormationFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2012 Thinking Systems, Inc.
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: January 10, 2013
//
/**
 * Implementation of a factory used to create Formations
 */
//------------------------------------------------------------------------------


#include "FormationFactory.hpp"
#include "Formation.hpp"

//------------------------------------------------------------------------------
// FormationFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * Replace the type Gmat::COMMAND in the base class call with the factory type 
 * you need.
 */
//------------------------------------------------------------------------------
FormationFactory::FormationFactory() :
   Factory           (Gmat::FORMATION)
{
   if (creatables.empty())
   {
      creatables.push_back("Formation");
   }

   // Register supported types
   GmatType::RegisterType(Gmat::FORMATION, "Formation");
}


//------------------------------------------------------------------------------
// ~FormationFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
FormationFactory::~FormationFactory()
{
}


//------------------------------------------------------------------------------
// FormationFactory(const FormationFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
FormationFactory::FormationFactory(const FormationFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("Formation");
   }
}


//------------------------------------------------------------------------------
// FormationFactory& operator=(const FormationFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
FormationFactory& FormationFactory::operator=(
      const FormationFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("Formation");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for GMAT SpaceObjects
 *
 * @param ofType The subtype of the space object
 * @param withName The new command's name
 *
 * @return A newly created GmatCommand (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
GmatBase* FormationFactory::CreateObject(const std::string &ofType,
                                         const std::string &withName)
{
   return CreateFormation(ofType, withName);
}

//------------------------------------------------------------------------------
// SpaceObject* CreateFormation(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for GMAT Formation
 *
 * @param ofType The type name of the Formation
 * @param withName The new Formation name
 *
 * @return A newly created Formation (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
SpaceObject* FormationFactory::CreateFormation(const std::string &ofType,
                                               const std::string &withName)
{
   if (ofType == "Formation")
      return new Formation(Gmat::FORMATION, ofType, withName);
   
   return NULL;   // doesn't match any type of Command known by this factory
}
