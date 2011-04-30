//$Id$
//------------------------------------------------------------------------------
//                            InterfaceFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Linda Jun
// Created: 2010/04/02
//
/**
 *  Implementation code for the InterfaceFactory class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "InterfaceFactory.hpp"

// Headers for the supported Interfaces


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Interface* CreateInterface(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Interface class. 
 *
 * @param <ofType> type of Interface object to create and return.
 * @param <withName> the name for the newly-created Interface object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Interface* InterfaceFactory::CreateInterface(const std::string &ofType,
                                             const std::string &withName)
{
   // There is no Interface to be created from base code
   // MatlabInterface is created from the MatlabInterfaceFactory
   
   return NULL;
}


//------------------------------------------------------------------------------
//  InterfaceFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class InterfaceFactory. 
 * (default constructor)
 */
//------------------------------------------------------------------------------
InterfaceFactory::InterfaceFactory() :
   Factory (Gmat::INTERFACE)
{
   if (creatables.empty())
   {
      ; // nothing to add
   }
}

//------------------------------------------------------------------------------
//  InterfaceFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class InterfaceFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
InterfaceFactory::InterfaceFactory(StringArray createList) :
   Factory(createList, Gmat::INTERFACE)
{
}


//------------------------------------------------------------------------------
//  InterfaceFactory(const InterfaceFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class InterfaceFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
InterfaceFactory::InterfaceFactory(const InterfaceFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      ; // nothing to add
   }
}


//------------------------------------------------------------------------------
// InterfaceFactory& operator= (const InterfaceFactory& fact)
//------------------------------------------------------------------------------
/**
 * InterfaceFactory operator for the InterfaceFactory base class.
 *
 * @param <fact> the InterfaceFactory object that is copied.
 *
 * @return "this" InterfaceFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
InterfaceFactory& InterfaceFactory::operator=(const InterfaceFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}


//------------------------------------------------------------------------------
// ~InterfaceFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the InterfaceFactory base class.
 */
//------------------------------------------------------------------------------
InterfaceFactory::~InterfaceFactory()
{
}

