//$Id: MatlabInterfaceFactory.cpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                            MatlabInterfaceFactory
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
// Author: Linda Jun
// Created: 2010/03/30
//
/**
 *  Implementation code for the MatlabInterfaceFactory class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "MatlabInterfaceFactory.hpp"
#include "MatlabInterface.hpp"
#include "MessageInterface.hpp"

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
Interface* MatlabInterfaceFactory::CreateInterface(const std::string &ofType,
                                                    const std::string &withName)
{
   if (ofType == "MatlabInterface")
      return MatlabInterface::Instance();
   
   return NULL;
}


//------------------------------------------------------------------------------
//  MatlabInterfaceFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MatlabInterfaceFactory. 
 * (default constructor)
 */
//------------------------------------------------------------------------------
MatlabInterfaceFactory::MatlabInterfaceFactory() :
   Factory (Gmat::INTERFACE)
{
   if (creatables.empty())
   {
      creatables.push_back("MatlabInterface");
   }
}


//------------------------------------------------------------------------------
//  MatlabInterfaceFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MatlabInterfaceFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
MatlabInterfaceFactory::MatlabInterfaceFactory(StringArray createList) :
   Factory(createList, Gmat::INTERFACE)
{
}


//------------------------------------------------------------------------------
//  MatlabInterfaceFactory(const MatlabInterfaceFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MatlabInterfaceFactory.
 * (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
MatlabInterfaceFactory::MatlabInterfaceFactory(const MatlabInterfaceFactory& fact) :
   Factory (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("MatlabInterface");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * MatlabInterfaceFactory operator for the MatlabInterfaceFactory base class.
 *
 * @param <fact> the MatlabInterfaceFactory object that is copied.
 *
 * @return "this" MatlabInterfaceFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
MatlabInterfaceFactory& MatlabInterfaceFactory::operator=(const MatlabInterfaceFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    

//------------------------------------------------------------------------------
// ~MatlabInterfaceFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the MatlabInterfaceFactory base class.
 */
//------------------------------------------------------------------------------
MatlabInterfaceFactory::~MatlabInterfaceFactory()
{
}

