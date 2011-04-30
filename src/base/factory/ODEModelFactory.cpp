//$Id$
//------------------------------------------------------------------------------
//                            ODEModelFactory
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
// Author: Wendy Shoan
// Created: 2003/10/14
//
/**
 *  Implementation code for the ODEModelFactory class, responsible for
 *  creating ODEModel objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "ODEModelFactory.hpp"
#include "ODEModel.hpp"   // for ODEModel class


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateObject(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested ODEModel class
 * in generic way.
 *
 * @param <ofType> the ODEModel object to create and return.
 * @param <withName> the name to give the newly-created ODEModel object.
 *
 */
//------------------------------------------------------------------------------
ODEModel* ODEModelFactory::CreateObject(const std::string &ofType,
                                        const std::string &withName)
{
   return CreateODEModel(ofType, withName);
}


//------------------------------------------------------------------------------
//  CreateODEModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested ODEModel class 
 *
 * @param <ofType> the ODEModel object to create and return.
 * @param <withName> the name to give the newly-created ODEModel object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
ODEModel* ODEModelFactory::CreateODEModel(const std::string &ofType,
                                          const std::string &withName)
{
   return new ODEModel(withName, ofType);
}


//------------------------------------------------------------------------------
//  ODEModelFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ODEModelFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
ODEModelFactory::ODEModelFactory() :
Factory(Gmat::ODE_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("ForceModel");  // default type for this factory
      creatables.push_back("ODEModel");  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  ODEModelFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ODEModelFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
ODEModelFactory::ODEModelFactory(StringArray createList) :
Factory(createList,Gmat::ODE_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("ForceModel");  // default type for this factory
      creatables.push_back("ODEModel");  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  ODEModelFactory(const ODEModelFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class ODEModelFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
ODEModelFactory::ODEModelFactory(const ODEModelFactory& fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("ForceModel");  // default type for this factory
      creatables.push_back("ODEModel");  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  ODEModelFactory& operator= (const ODEModelFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ODEModelFactory class.
 *
 * @param <fact> the ODEModelFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" ODEModelFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
ODEModelFactory& ODEModelFactory::operator= (const ODEModelFactory& fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("ForceModel");  // default type for this factory
      creatables.push_back("ODEModel");  // default type for this factory
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~ODEModelFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the ODEModelFactory base class.
   *
   */
//------------------------------------------------------------------------------
ODEModelFactory::~ODEModelFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
