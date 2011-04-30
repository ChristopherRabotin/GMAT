//$Id$
//------------------------------------------------------------------------------
//                            CoordinateSystemFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/01/18
//
/**
 *  Implementation code for the CoordinateSystemFactory class, responsible for
 *  creating CoordinateSystem objects.
 */
//------------------------------------------------------------------------------

#include "CoordinateSystemFactory.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CoordinateSystem* CreateCoordinateSystem(const std::string &ofType, 
//                                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested CoordinateSystem class 
 *
 * @param <ofType>   type of CoordinateSystem object to create and return.
 * @param <withName> the name to give the newly-created CoordinateSystem object.
 *
 * @return new CoordinateSystem object
 *
 */
//------------------------------------------------------------------------------
CoordinateSystem*
CoordinateSystemFactory::CreateCoordinateSystem(const std::string &ofType,
                                                const std::string &withName)
{
   return new CoordinateSystem(ofType, withName);
}


//------------------------------------------------------------------------------
//  CoordinateSystemFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CoordinateSystemFactory
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
CoordinateSystemFactory::CoordinateSystemFactory() :
Factory(Gmat::COORDINATE_SYSTEM)
{
   if (creatables.empty())
   {
      creatables.push_back("CoordinateSystem");  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  CoordinateSystemFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CoordinateSystemFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
CoordinateSystemFactory::CoordinateSystemFactory(StringArray createList) :
Factory(createList,Gmat::COORDINATE_SYSTEM)
{
   if (creatables.empty())
   {
      creatables.push_back("CoordinateSystem");  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  CoordinateSystemFactory(const CoordinateSystemFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class CoordinateSystemFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
CoordinateSystemFactory::CoordinateSystemFactory(const CoordinateSystemFactory& fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("CoordinateSystem");  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  CoordinateSystemFactory& operator= (const CoordinateSystemFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the CoordinateSystemFactory class.
 *
 * @param <fact> the CoordinateSystemFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" CoordinateSystemFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
CoordinateSystemFactory&
CoordinateSystemFactory::operator= (const CoordinateSystemFactory& fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("CoordinateSystem");  // default type for this factory
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~CoordinateSystemFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the CoordinateSystemFactory base class.
 */
//------------------------------------------------------------------------------
CoordinateSystemFactory::~CoordinateSystemFactory()
{
   // deletes handled by Factory destructor
}

