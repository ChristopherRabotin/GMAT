//$Header$
//------------------------------------------------------------------------------
//                            CoordinateSystemFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
//  CreateCoordinateSystem(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested CoordinateSystem class 
 *
 * @param <ofType> the CoordinateSystem object to create and return.
 * @param <withName> the name to give the newly-created CoordinateSystem object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 * @note As of 2003/10/14, we are ignoring the ofType parameter.  Use of this
 *       parameter may be added later.
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

