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
//  #include "ElapsedSecParam.hpp" 
//  #include "CurrentA1MjdParam.hpp"
//  #include "CartXParam.hpp"
//  #include "CartYParam.hpp"
//  #include "CartZParam.hpp"
//  #include "CartVxParam.hpp"
//  #include "CartVyParam.hpp"
//  #include "CartVzParam.hpp"
//  #include "KepSmaParam.hpp"
//  #include "KepEccParam.hpp"
//  #include "KepIncParam.hpp"
//  #include "KepRaanParam.hpp"
//  #include "KepAopParam.hpp"
//  #include "KepTaParam.hpp"

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
//     if (ofType == "ElapsedSecParam")
//        return new ElapsedSecParam(withName);
//     if (ofType == "CurrentA1MjdParam")
//        return new CurrentA1MjdParam(withName);
//     if (ofType == "CartXParam")
//        return new CartXParam(withName);
//     if (ofType == "CartYParam")
//        return new CartYParam(withName);
//     if (ofType == "CartZParam")
//        return new CartZParam(withName);
//     if (ofType == "CartVxParam")
//        return new CartVxParam(withName);
//     if (ofType == "CartVyParam")
//        return new CartVyParam(withName);
//      if (ofType == "CartVzParam")
//        return new CartVzParam(withName);
//      if (ofType == "KepSmaParam")
//        return new KepSmaParam(withName);
//      if (ofType == "KepEccParam")
//        return new KepEccParam(withName);
//      if (ofType == "KepIncParam")
//        return new KepIncParam(withName);
//      if (ofType == "KepRaanParam")
//        return new KepRaanParam(withName);
//      if (ofType == "KepAopParam")
//        return new KepAopParam(withName);
//      if (ofType == "KepTaParam")
//        return new KepTaParam(withName);
//      // add others here
//      else
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
   if (creatables.empty())
   {
//        creatables.push_back("ElapsedSecParam");
//        creatables.push_back("CurrentA1MjdParam");
//        creatables.push_back("CartXParam");
//        creatables.push_back("CartYParam");
//        creatables.push_back("CartZParam");
//        creatables.push_back("CartVxParam");
//        creatables.push_back("CartVyParam");
//        creatables.push_back("CartVzParam");
//        creatables.push_back("KepSmaParam");
//        creatables.push_back("KepEccParam");
//        creatables.push_back("KepIncParam");
//        creatables.push_back("KepRaanParam");
//        creatables.push_back("KepAopParam");
//        creatables.push_back("KepTaParam");
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
