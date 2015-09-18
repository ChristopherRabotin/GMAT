//$Id$
//------------------------------------------------------------------------------
//                                 PythonCommandFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// FDSS II .
//
// Author: Farideh Farahnak
// Created: 2015/02/23
//
/**
 * Factory used to create scripted Python commands
 */
//------------------------------------------------------------------------------

#include "PythonCommandFactory.hpp"
#include "CallPythonFunction.hpp"

//------------------------------------------------------------------------------
// PythonCommandFactory()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
PythonCommandFactory::PythonCommandFactory() :
   Factory           (Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("CallPythonFunction");
   }
}


//------------------------------------------------------------------------------
// ~PythonCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PythonCommandFactory::~PythonCommandFactory()
{
}


//------------------------------------------------------------------------------
// PythonCommandFactory(const PythonCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param elf The factory copied here
 */
//------------------------------------------------------------------------------
PythonCommandFactory::PythonCommandFactory(const PythonCommandFactory& elf) :
   Factory           (elf)
{
   if (creatables.empty())
   {
      creatables.push_back("CallPythonFunction");
   }
}


//------------------------------------------------------------------------------
// PythonCommandFactory& operator=(const PythonCommandFactory& elf)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param elf The factory copied to this one
 *
 * @return this instance, set to match elf
 */
//------------------------------------------------------------------------------
PythonCommandFactory& PythonCommandFactory::operator=(
      const PythonCommandFactory& elf)
{
   if (this != &elf)
   {
      Factory::operator=(elf);

      if (creatables.empty())
      {
         creatables.push_back("CallPythonFunction");
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
GmatCommand* PythonCommandFactory::CreateCommand(const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "CallPythonFunction")
      return new CallPythonFunction();

   return NULL;   // doesn't match any type of Command known by this factory
}
