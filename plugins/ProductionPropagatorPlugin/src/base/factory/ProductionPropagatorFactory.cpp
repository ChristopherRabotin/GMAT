//$Id: ProductionPropagatorFactory.cpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            ProductionPropagatorFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Author: Darrel Conway
// Created: Nov 6, 2013
//
/**
 * Implementation code for the ProductionPropagatorFactory class, which creates objects that
 * extend the SRP model for solar sailing.
 *
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ProductionPropagatorFactory.hpp"
#include "PrinceDormand853.hpp"

#include "MessageInterface.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  PhysicalModel* ProductionPropagatorFactory::(const std::string &ofType,
//                               const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns a PhysicalModel object.
 *
 * @param <ofType> type of PhysicalModel object to create and return.
 * @param <withName> the name for the newly-created PhysicalModel object.
 *
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Propagator* ProductionPropagatorFactory::CreatePropagator(const std::string &ofType,
                                    const std::string &withName)
{
   if (ofType == "PrinceDormand853")
      return new PrinceDormand853(withName);
   return NULL;
}


//------------------------------------------------------------------------------
//  ProductionPropagatorFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProductionPropagatorFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
ProductionPropagatorFactory::ProductionPropagatorFactory() :
   Factory     (Gmat::PROPAGATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("PrinceDormand853");
   }
}

//------------------------------------------------------------------------------
//  ProductionPropagatorFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProductionPropagatorFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
ProductionPropagatorFactory::ProductionPropagatorFactory(StringArray createList) :
   Factory(createList, Gmat::PROPAGATOR)
{
}


//------------------------------------------------------------------------------
//  ProductionPropagatorFactory(const ProductionPropagatorFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ProductionPropagatorFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
ProductionPropagatorFactory::ProductionPropagatorFactory(const ProductionPropagatorFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("PrinceDormand853");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * ProductionPropagatorFactory operator for the ProductionPropagatorFactory base class.
 *
 * @param <fact> the ProductionPropagatorFactory object that is copied.
 *
 * @return "this" ProductionPropagatorFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
ProductionPropagatorFactory& ProductionPropagatorFactory::operator=(const ProductionPropagatorFactory& fact)
{
   if (this != &fact)
   {
      Factory::operator=(fact);
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~ProductionPropagatorFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the ProductionPropagatorFactory base class.
 */
//------------------------------------------------------------------------------
ProductionPropagatorFactory::~ProductionPropagatorFactory()
{
}
