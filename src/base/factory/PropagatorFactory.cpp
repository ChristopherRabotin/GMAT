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
#include "RungeKutta89.hpp"             // for RungeKutta89 class
#include "DormandElMikkawyPrince68.hpp" // for DormandElMikkawyPrince68 class
#include "RungeKuttaFehlberg56.hpp"     // for RungeKuttaFehlberg56 class
#include "PrinceDormand45.hpp"          // for PrinceDormand45 class
#include "PrinceDormand78.hpp"          // for PrinceDormand78 class
#include "AdamsBashfordMoulton.hpp"     // for AdamsBashfordMoulton class
#include "BulirschStoer.hpp"            // for BulirschStoer class
// add others here for future builds

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreatePropagator(const std::string &ofType, const std::string &withName)
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
Propagator* PropagatorFactory::CreatePropagator(const std::string &ofType,
                                                const std::string &withName)
{
   if (ofType == "RungeKutta89")
      return new RungeKutta89(withName);
   if (ofType == "PrinceDormand45")
      return new PrinceDormand45(withName);
   if (ofType == "PrinceDormand78")
      return new PrinceDormand78(withName);
   if (ofType == "BulirschStoer")
      return new BulirschStoer(withName);
   if (ofType == "AdamsBashfordMoulton")
      return new AdamsBashfordMoulton(withName);
   if (ofType == "DormandElMikkawyPrince68")
      return new DormandElMikkawyPrince68(withName);
   if (ofType == "RungeKuttaFehlberg56")
      return new RungeKuttaFehlberg56(withName);
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
   if (creatables.empty())
   {
      creatables.push_back("RungeKutta89");
      creatables.push_back("PrinceDormand45");
      creatables.push_back("PrinceDormand78");
      creatables.push_back("BulirschStoer");
      creatables.push_back("AdamsBashfordMoulton");
      creatables.push_back("DormandElMikkawyPrince68");
      creatables.push_back("RungeKuttaFehlberg56");
   }
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
