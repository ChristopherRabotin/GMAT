//$Header$
//------------------------------------------------------------------------------
//                            AxisSystemFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy Shoan
// Created: 2004/12/23
//
/**
 *  Implementation code for the AxisSystemFactory class, responsible for
 *  creating AxisSystem objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "AxisSystemFactory.hpp"
#include "AxisSystem.hpp"
#include "MessageInterface.hpp"
#include "MJ2000EqAxes.hpp"
#include "MJ2000EcAxes.hpp"
#include "BodyFixedAxes.hpp"
#include "EquatorAxes.hpp"
#include "ObjectReferencedAxes.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateAxisSystem(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an AxisSystem object of the requested 
 * type.
 *
 * @param <ofType>   the AxisSystem object to create and return.
 * @param <withName> the name to give the newly-created AxisSystem object.
 *
 */
//------------------------------------------------------------------------------
AxisSystem*
AxisSystemFactory::CreateAxisSystem(const std::string &ofType,
                                    const std::string &withName)
{
   AxisSystem *withAxes = NULL;
   if (ofType == "MJ2000Eq")
   {
      withAxes = new MJ2000EqAxes(withName);
   }
   else if (ofType == "MJ2000Ec")
   {
      withAxes = new MJ2000EcAxes(withName);
   }
   else if (ofType == "TOEEq")
   {
      MessageInterface::ShowMessage(                // *** temporary
         "TOEEqAxes not yet imnplemented.\n");
      //withAxes = new TOEEqAxes(withName);
   }
   else if (ofType == "TOEEc")
   {
      MessageInterface::ShowMessage(                // *** temporary
         "TOEEcAxes not yet imnplemented.\n");
      //withAxes = new TOEEcAxes(withName);
   }
   else if (ofType == "MOEEq")
   {
      MessageInterface::ShowMessage(                // *** temporary
          "MOEEqAxes not yet imnplemented.\n");
      //withAxes = new MOEEqAxes(withName);
   }
   else if (ofType == "MOEEc")
   {
      MessageInterface::ShowMessage(                // *** temporary
         "MOEEcAxes not yet imnplemented.\n");
      //withAxes = new MOEEcAxes(withName);
   }
   else if (ofType == "TODEq")
   {
      MessageInterface::ShowMessage(                // *** temporary
         "TODEqAxes not yet imnplemented.\n");
      //withAxes = new TODEqAxes(withName);
   }
   else if (ofType == "TODEc")
   {
      MessageInterface::ShowMessage(                // *** temporary
         "TODEcAxes not yet imnplemented.\n");
      //withAxes = new TODEcAxes(withName);
   }
   else if (ofType == "MODEq")
   {
      MessageInterface::ShowMessage(                // *** temporary
         "MODEqAxes not yet imnplemented.\n");
      //withAxes = new MODEqAxes(withName);
   }
   else if (ofType == "MODEc")
   {
      MessageInterface::ShowMessage(                // *** temporary
         "MODEcAxes not yet imnplemented.\n");
      //withAxes = new MODEcAxes(withName;
   }
   else if (ofType == "ObjectReferenced")  // need to do more here????
   {
      withAxes = new ObjectReferencedAxes(withName);
   }
   else if (ofType == "Equator")
   {
      withAxes = new EquatorAxes(withName);
   }
   else if (ofType == "BodyFixed")
   {
      withAxes = new BodyFixedAxes(withName);
   }
   return withAxes;
}


//------------------------------------------------------------------------------
//  AxisSystemFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AxisSystemFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
AxisSystemFactory::AxisSystemFactory() :
Factory(Gmat::AXIS_SYSTEM) //loj: 1/19/05 Changed from ATMOSPHERE
{
   if (creatables.empty())
   {
      creatables.push_back("MJ2000Eq");
      creatables.push_back("MJ2000Ec");
      creatables.push_back("TOEEq");
      creatables.push_back("TOEEc");
      creatables.push_back("MOEEq");
      creatables.push_back("MOEEc");
      creatables.push_back("TODEq");
      creatables.push_back("TODEc");
      creatables.push_back("MODEq");
      creatables.push_back("MODEc");
      creatables.push_back("ObjectReferenced");
      creatables.push_back("Equator");
      creatables.push_back("BodyFixed");
   }
}

//------------------------------------------------------------------------------
//  AxisSystemFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AxisSystemFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
AxisSystemFactory::AxisSystemFactory(StringArray createList) :
Factory(createList,Gmat::AXIS_SYSTEM)
{
}

//------------------------------------------------------------------------------
//  AxisSystemFactory(const AxisSystemFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AxisSystemFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
AxisSystemFactory::AxisSystemFactory(const AxisSystemFactory& fact) :
Factory(fact)
{
}

//------------------------------------------------------------------------------
//  AxisSystemFactory& operator= (const AxisSystemFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the AxisSystemFactory class.
 *
 * @param <fact> the AxisSystemFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" AxisSystemFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
AxisSystemFactory& AxisSystemFactory::operator= (
                                             const AxisSystemFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~AxisSystemFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the AxisSystemFactory base class.
 *
 */
//------------------------------------------------------------------------------
AxisSystemFactory::~AxisSystemFactory()
{
   // deletes handled by Factory base class
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------





