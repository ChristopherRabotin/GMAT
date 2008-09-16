//$Header$
//------------------------------------------------------------------------------
//                            ForceModelFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/14
//
/**
 *  Implementation code for the ForceModelFactory class, responsible for
 *  creating ForceModel objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "ForceModelFactory.hpp"
#include "ForceModel.hpp"   // for ForceModel class


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateForceModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested ForceModel class 
 *
 * @param <ofType> the ForceModel object to create and return.
 * @param <withName> the name to give the newly-created ForceModel object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
ForceModel* ForceModelFactory::CreateForceModel(const std::string &ofType,
                                                const std::string &withName)
{
   return new ForceModel(withName);
}


//------------------------------------------------------------------------------
//  ForceModelFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ForceModelFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
ForceModelFactory::ForceModelFactory() :
Factory(Gmat::FORCE_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("ForceModel");  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  ForceModelFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ForceModelFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
ForceModelFactory::ForceModelFactory(StringArray createList) :
Factory(createList,Gmat::FORCE_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("ForceModel");  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  ForceModelFactory(const ForceModelFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class ForceModelFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
ForceModelFactory::ForceModelFactory(const ForceModelFactory& fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("ForceModel");  // default type for this factory
   }
}

//------------------------------------------------------------------------------
//  ForceModelFactory& operator= (const ForceModelFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ForceModelFactory class.
 *
 * @param <fact> the ForceModelFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" ForceModelFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
ForceModelFactory& ForceModelFactory::operator= (const ForceModelFactory& fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("ForceModel");  // default type for this factory
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~ForceModelFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the ForceModelFactory base class.
   *
   */
//------------------------------------------------------------------------------
ForceModelFactory::~ForceModelFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
