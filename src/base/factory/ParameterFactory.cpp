//$Header$
//------------------------------------------------------------------------------
//                            ParameterFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel Conway
// Created: 2003/10/28
//
/**
 *  Implementation code for the ParameterFactory class, responsible
 *  for creating Parameter objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "ParameterFactory.hpp"
#include "ElapsedTimeParam.hpp"       // for ElapsedTimeParam class
#include "CurrentTimeParam.hpp"       // for CurrentTimeParam class
#include "SpacecraftStateParam.hpp"   // for SpacecraftStateParam class

// add others here for future builds

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateParameter(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Parameter class
 *
 * @param <ofType> the Parameter object to create and return.
 * @param <withName> the name to give the newly-created Parameter object.
 */
//------------------------------------------------------------------------------
Parameter* ParameterFactory::CreateParameter(std::string ofType,
                                             std::string withName)
{
   if (ofType == "ElapsedTimeParam")
      return new ElapsedTimeParam(withName, "", 86400.0);
   if (ofType == "CurrentTimeParam")
      return new CurrentTimeParam(withName, "", 0.0);
   if (ofType == "SpacecraftStateParam")
      return new SpacecraftStateParam(withName, "");
   // add others here
   else
      return NULL;
}


//------------------------------------------------------------------------------
//  ParameterFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ParameterFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
ParameterFactory::ParameterFactory() :
Factory(Gmat::PARAMETER)
{
   if (creatables.empty()) {
      creatables.push_back("ElapsedTimeParam");
      creatables.push_back("CurrentTimeParam");
//      creatables.push_back("SpacecraftStateParam");
   }
}

//------------------------------------------------------------------------------
//  ParameterFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ParameterFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects
 *
 */
//------------------------------------------------------------------------------
ParameterFactory::ParameterFactory(StringArray createList) :
Factory(createList, Gmat::PARAMETER)
{
}

//------------------------------------------------------------------------------
//  PropagatorFactory(const PropagatorFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class ParameterFactory
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
ParameterFactory::ParameterFactory(const ParameterFactory& fact) :
Factory(fact)
{

}

//------------------------------------------------------------------------------
//  ParameterFactory& operator= (const ParameterFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ParameterFactory base class.
 *
 * @param <fact> the ParameterFactory object whose data to assign to "this"
 *  factory.
 *
 * @return "this" ParameterFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
ParameterFactory& ParameterFactory::operator= (const ParameterFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~ParameterFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the ParameterFactory base class.
   *
   */
//------------------------------------------------------------------------------
ParameterFactory::~ParameterFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
