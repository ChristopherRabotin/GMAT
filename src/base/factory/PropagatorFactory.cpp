//$Header$
//------------------------------------------------------------------------------
//                            PropagatorFactory
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
 *  Implementation code for the PropagatorFactory class, responsible
 *  for creating Propagator objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "PropagatorFactory.hpp"
#include "RungeKutta89.hpp"   // for RungeKutta89 class
// add others here for future builds

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreatePropagator(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Propagator class 
 *
 * @param <ofType> the Propagator object to create and return.
 * @param <withName> the name to give the newly-created Propagator object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
Propagator* PropagatorFactory::CreatePropagator(std::string ofType,
                                                std::string withName)
{
   if (ofType == "RungeKutta89")
      return new RungeKutta89();
   // add others here
   else
      return NULL;
}


//------------------------------------------------------------------------------
//  PropagatorFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PropagatorFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
PropagatorFactory::PropagatorFactory() :
Factory(Gmat::PROPAGATOR)
{
}

//------------------------------------------------------------------------------
//  PropagatorFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PropagatorFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects
 *
 */
//------------------------------------------------------------------------------
PropagatorFactory::PropagatorFactory(StringArray createList) :
Factory(createList, Gmat::PROPAGATOR)
{
}

//------------------------------------------------------------------------------
//  PropagatorFactory(const PropagatorFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class PropagatorFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
PropagatorFactory::PropagatorFactory(const PropagatorFactory& fact) :
Factory(fact)
{

}

//------------------------------------------------------------------------------
//  PropagatorFactory& operator= (const PropagatorFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the PropagatorFactory base class.
 *
 * @param <fact> the PropagatorFactory object whose data to assign to "this"
 *  factory.
 *
 * @return "this" PropagatorFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
PropagatorFactory& PropagatorFactory::operator= (const PropagatorFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~PropagatorFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the PropagatorFactory base class.
   *
   */
//------------------------------------------------------------------------------
PropagatorFactory::~PropagatorFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
