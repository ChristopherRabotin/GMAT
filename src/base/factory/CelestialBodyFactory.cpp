//$Header$
//------------------------------------------------------------------------------
//                            CelestialBodyFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2004/01/29
//
/**
 *  Implementation code for the CelestialBodyFactory class, responsible for
 *  creating CelestialBody objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "CelestialBodyFactory.hpp"
#include "Star.hpp"
#include "Planet.hpp"
#include "Moon.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateCelestialBody(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested CelestialBody class
 *
 * @param <ofType> the CelestialBody object to create and return.
 * @param <withName> the name to give the newly-created CelestialBody object.
 *
 */
//------------------------------------------------------------------------------
CelestialBody* CelestialBodyFactory::CreateCelestialBody(std::string ofType,
                                                         std::string withName)
{
   if (ofType == "Star")
      return new Star(withName);
   else if (ofType == "Planet")
      return new Planet(withName);
   else if (ofType == "Moon")
      return new Moon(withName);
   // add more here .......
   else {
      return NULL;   // doesn't match any known type of command
   }
}

//------------------------------------------------------------------------------
//  CelestialBodyFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CelestialBodyFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
CelestialBodyFactory::CelestialBodyFactory() :
Factory(Gmat::CELESTIAL_BODY)
{
   if (creatables.empty())
   {
      creatables.push_back("Star");
      creatables.push_back("Planet");
      creatables.push_back("Moon");
   }
}

//------------------------------------------------------------------------------
//  CelestialBodyFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CelestialBodyFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
CelestialBodyFactory::CelestialBodyFactory(StringArray createList) :
Factory(createList,Gmat::CELESTIAL_BODY)
{
   if (creatables.empty())
   {
      creatables.push_back("Star");
      creatables.push_back("Planet");
      creatables.push_back("Moon");
   }
}

//------------------------------------------------------------------------------
//  CelestialBodyFactory(const CelestialBodyFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CelestialBodyFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
CelestialBodyFactory::CelestialBodyFactory(const CelestialBodyFactory& fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("Star");
      creatables.push_back("Planet");
      creatables.push_back("Moon");
   }
}

//------------------------------------------------------------------------------
//  CelestialBodyFactory& operator= (const CelestialBodyFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the CelestialBodyFactory class.
 *
 * @param <fact> the CelestialBodyFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" CelestialBodyFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
CelestialBodyFactory& CelestialBodyFactory::operator= (
                                             const CelestialBodyFactory& fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("Star");
      creatables.push_back("Planet");
      creatables.push_back("Moon");
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~CelestialBodyFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the CelestialBodyFactory base class.
 *
 */
//------------------------------------------------------------------------------
CelestialBodyFactory::~CelestialBodyFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------





