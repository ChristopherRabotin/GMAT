//$Id$
//------------------------------------------------------------------------------
//                           FormationInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2013/01/09
//
/**
 * Proxy code for Formation objects, used to define interfaces implemented in 
 * the Formation plugin. 
 */
//------------------------------------------------------------------------------


#include "FormationInterface.hpp"


//------------------------------------------------------------------------------
// FormationInterface(Gmat::ObjectType typeId, const std::string &typeStr,
//       const std::string &instName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param typeId The object's ObjectType
 * @param typeStr The text type of the object
 * @param instName The object's name
 */
//------------------------------------------------------------------------------
FormationInterface::FormationInterface(Gmat::ObjectType typeId, 
	  const std::string &typeStr, const std::string &instName) :
   SpaceObject    (typeId, typeStr, instName)
{
   objectTypes.push_back(Gmat::FORMATION);
   objectTypeNames.push_back("FormationInterface");
}


//------------------------------------------------------------------------------
// ~FormationInterface()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
FormationInterface::~FormationInterface()
{
}


//------------------------------------------------------------------------------
// FormationInterface(const FormationInterface& fi)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fi The interface copied here
 */
//------------------------------------------------------------------------------
FormationInterface::FormationInterface(const FormationInterface& fi) :
   SpaceObject       (fi)
{
}


//------------------------------------------------------------------------------
// FormationInterface& operator=(const FormationInterface& fi)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fi The interface copied here
 *
 * @return this object set to look like fi.
 */
//------------------------------------------------------------------------------
FormationInterface& FormationInterface::operator=(const FormationInterface& fi)
{
   if (this != &fi)
   {
      SpaceObject::operator=(fi);
   }

   return *this;
}
