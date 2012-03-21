//$Id: VernerFactory.cpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            VernerFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Author: Darrel Conway
// Created: May 3 2010
//
/**
 * Implementation code for the VernerFactory class, which creates objects that
 * extend the SRP model for solar sailing.
 *
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "EphemPropFactory.hpp"
#include "SPKPropagator.hpp"

#include "MessageInterface.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  PhysicalModel* VernerFactory::(const std::string &ofType,
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
Propagator* EphemPropFactory::CreatePropagator(const std::string &ofType,
                                    const std::string &withName)
{
   if (ofType == "SPK")
      return new SPKPropagator(withName);

   return NULL;
}


//------------------------------------------------------------------------------
//  VernerFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class VernerFactory.
 * (default constructor)
 */
//------------------------------------------------------------------------------
EphemPropFactory::EphemPropFactory() :
   Factory     (Gmat::PROPAGATOR)
{
   if (creatables.empty())
   {
      creatables.push_back("SPK");
   }
}

//------------------------------------------------------------------------------
//  VernerFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class VernerFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
EphemPropFactory::EphemPropFactory(StringArray createList) :
   Factory(createList, Gmat::PROPAGATOR)
{
}


//------------------------------------------------------------------------------
//  VernerFactory(const VernerFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class VernerFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
EphemPropFactory::EphemPropFactory(const EphemPropFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("SPK");
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * VernerFactory operator for the VernerFactory base class.
 *
 * @param <fact> the VernerFactory object that is copied.
 *
 * @return "this" VernerFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
EphemPropFactory& EphemPropFactory::operator=(const EphemPropFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}


//------------------------------------------------------------------------------
// ~VernerFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the VernerFactory base class.
 */
//------------------------------------------------------------------------------
EphemPropFactory::~EphemPropFactory()
{
}
