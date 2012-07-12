//$Id$
//------------------------------------------------------------------------------
//                            CCommandFactory
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
// Created: 2011/06/14
//
/**
 *  Implementation code for the CCommandFactory class, responsible for
 *  creating Command objects associated with the C Interface.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "CCommandFactory.hpp"

#include "PrepareMissionSequence.hpp"   // for PrepareMissionSequence command


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatCommand* CreateCommand(const std::string &ofType,
//                             const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested command class
 *
 * @param <ofType>   type of command object to create and return.
 * @param <withName> name of the command (currently not used).
 *
 * @return command object
 */
//------------------------------------------------------------------------------
GmatCommand* CCommandFactory::CreateCommand(const std::string &ofType,
                                           const std::string &withName)
{
   if (ofType == "PrepareMissionSequence")
       return new PrepareMissionSequence;
   // add more here .......
   else
   {
      return NULL;   // doesn't match any known type of command
   }

}


//------------------------------------------------------------------------------
//  CCommandFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CCommandFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
CCommandFactory::CCommandFactory() :
    Factory(Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("PrepareMissionSequence");
      qualifiedCreatables.push_back("PrepareMissionSequence");
   }
}

//------------------------------------------------------------------------------
//  CCommandFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CCommandFactory.
 *
 * @param <createList> list of creatable command objects
 */
//------------------------------------------------------------------------------
CCommandFactory::CCommandFactory(StringArray createList) :
    Factory(createList,Gmat::COMMAND)
{
}

//------------------------------------------------------------------------------
//  CCommandFactory(const CCommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the (base) class CCommandFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
CCommandFactory::CCommandFactory(const CCommandFactory& fact) :
    Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("PrepareMissionSequence");
      qualifiedCreatables.push_back("PrepareMissionSequence");
   }
}

//------------------------------------------------------------------------------
//  CCommandFactory& operator= (const CCommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the CCommandFactory base class.
 *
 * @param <fact> the CCommandFactory object whose data to assign to "this" factory.
 *
 * @return "this" CCommandFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
CCommandFactory& CCommandFactory::operator= (const CCommandFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~CCommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the CCommandFactory base class.
 */
//------------------------------------------------------------------------------
CCommandFactory::~CCommandFactory()
{
   // deletes handled by Factory destructor
}


StringArray CCommandFactory::GetListOfCreatableObjects(
                                  const std::string &qualifier)
{
   if (qualifier != "")
   {
      if (qualifier == "SequenceStarters")
         return qualifiedCreatables;
   }

   return Factory::GetListOfCreatableObjects(qualifier);
}
