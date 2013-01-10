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
   Factory           (Gmat::SPACECRAFT)
{
   if (creatables.empty())
   {
      // Replace the FormationClass string here with your class name.  For multiple 
      // classes of the same core type, push back multiple names here
      creatables.push_back("Formation");
   }
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
      // Replace the FormationClass string here with your class name.  For multiple 
      // classes of the same type, push back multiple names here
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
         // Replace the FormationClass string here with your class name.  For multiple 
         // classes of the same type, push back multiple names here
         creatables.push_back("Formation");
      }
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatCommand* CreateCommand(const std::string &ofType,
//       const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creation method for GMAT SpaceObjects
 *
 * @param ofType The subtype of the command
 * @param withName The new command's name
 *
 * @return A newly created GmatCommand (or NULL if this factory doesn't create
 *         the requested type)
 */
//------------------------------------------------------------------------------
SpaceObject* FormationFactory::CreateSpacecraft(const std::string &ofType,
                                             const std::string &withName)
{
   // This is how you'd implement creation of a ConsolePrint command 
   if (ofType == "Formation")
      return new Formation(Gmat::FORMATION, ofType, withName);
   // add more here .......

   return NULL;   // doesn't match any type of Command known by this factory
}
