//$Id: ExtraPropagatorFactory.cpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            ExtraPropagatorFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Author: Darrel Conway
// Created: May 3 2010
//
/**
 * Implementation code for the ExtraPropagatorFactory class, which creates objects that
 * extend the SRP model for solar sailing.
 *
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "ExtraPropagatorFactory.hpp"
#include "BulirschStoer.hpp"

#include "MessageInterface.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  PhysicalModel* ExtraPropagatorFactory::(const std::string &ofType,
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
Propagator* ExtraPropagatorFactory::CreatePropagator(const std::string &ofType,
                                    const std::string &withName)
{
   if (ofType == "BulirschStoer")
      return new BulirschStoer(withName);

   return NULL;
}


//------------------------------------------------------------------------------
//  ExtraPropagatorFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExtraPropagatorFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
ExtraPropagatorFactory::ExtraPropagatorFactory() :
   Factory     (Gmat::PROPAGATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("BulirschStoer");
   }
}

//------------------------------------------------------------------------------
//  ExtraPropagatorFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExtraPropagatorFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
ExtraPropagatorFactory::ExtraPropagatorFactory(StringArray createList) :
   Factory(createList, Gmat::PROPAGATOR)
{
}


//------------------------------------------------------------------------------
//  ExtraPropagatorFactory(const ExtraPropagatorFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ExtraPropagatorFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
ExtraPropagatorFactory::ExtraPropagatorFactory(const ExtraPropagatorFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("BulirschStoer");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * ExtraPropagatorFactory operator for the ExtraPropagatorFactory base class.
 *
 * @param <fact> the ExtraPropagatorFactory object that is copied.
 *
 * @return "this" ExtraPropagatorFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
ExtraPropagatorFactory& ExtraPropagatorFactory::operator=(const ExtraPropagatorFactory& fact)
{
   if (this != &fact)
   {
      Factory::operator=(fact);
   }

   return *this;
}


//------------------------------------------------------------------------------
// ~ExtraPropagatorFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the ExtraPropagatorFactory base class.
 */
//------------------------------------------------------------------------------
ExtraPropagatorFactory::~ExtraPropagatorFactory()
{
}
