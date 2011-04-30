//$Id$
//------------------------------------------------------------------------------
//                            SolarSystemFactory
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
// Created: 2004/01/29
//
/**
 *  Implementation code for the SolarSystemFactory class, responsible for
 *  creating SolarSystem objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "SolarSystemFactory.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateSolarSystem(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested SolarSystem class
 *
 * @param <ofType> the SolarSystem object to create and return.
 * @param <withName> the name to give the newly-created SolarSystem object.
 *
 * @note As of 2003/10/14, we are ignoring the ofType parameter.
 *       Use of this parameter may be added later.
 */
//------------------------------------------------------------------------------
SolarSystem* SolarSystemFactory::CreateSolarSystem(const std::string &ofType,
                                                   const std::string &withName)
{
   return new SolarSystem(withName);
}


//------------------------------------------------------------------------------
//  SolarSystemFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SolarSystemFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
SolarSystemFactory::SolarSystemFactory() :
Factory(Gmat::SOLAR_SYSTEM)
{
   if (creatables.empty())
      creatables.push_back("SolarSystem");
}

//------------------------------------------------------------------------------
//  SolarSystemFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SolarSystemFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
SolarSystemFactory::SolarSystemFactory(StringArray createList) :
Factory(createList,Gmat::SOLAR_SYSTEM)
{
   if (creatables.empty())
      creatables.push_back("SolarSystem");
}

//------------------------------------------------------------------------------
//  SolarSystemFactory(const SolarSystemFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SolarSystemFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
SolarSystemFactory::SolarSystemFactory(const SolarSystemFactory& fact) :
Factory(fact)
{
   if (creatables.empty())
      creatables.push_back("SolarSystem");
}

//------------------------------------------------------------------------------
//  SolarSystemFactory& operator= (const SolarSystemFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the SolarSystemFactory class.
 *
 * @param <fact> the SolarSystemFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" SolarSystemFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
SolarSystemFactory& SolarSystemFactory::operator= (
                                             const SolarSystemFactory& fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
      creatables.push_back("SolarSystem");
   return *this;
}

//------------------------------------------------------------------------------
// ~SolarSystemFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the SolarSystemFactory base class.
 *
 */
//------------------------------------------------------------------------------
SolarSystemFactory::~SolarSystemFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------





