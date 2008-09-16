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
#include "BodyInertialAxes.hpp"
#include "EquatorAxes.hpp"
#include "ObjectReferencedAxes.hpp"
#include "TOEEqAxes.hpp"
#include "MOEEqAxes.hpp"
#include "TODEqAxes.hpp"
#include "MODEqAxes.hpp"
#include "TOEEcAxes.hpp"
#include "MOEEcAxes.hpp"
#include "TODEcAxes.hpp"
#include "MODEcAxes.hpp"
#include "GeocentricSolarEclipticAxes.hpp"
#include "GeocentricSolarMagneticAxes.hpp"

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
      withAxes = new TOEEqAxes(withName);
   }
   else if (ofType == "TOEEc")
   {
      withAxes = new TOEEcAxes(withName);
   }
   else if (ofType == "MOEEq")
   {
      withAxes = new MOEEqAxes(withName);
   }
   else if (ofType == "MOEEc")
   {
      withAxes = new MOEEcAxes(withName);
   }
   else if (ofType == "TODEq")
   {
      withAxes = new TODEqAxes(withName);
   }
   else if (ofType == "TODEc")
   {
      withAxes = new TODEcAxes(withName);
   }
   else if (ofType == "MODEq")
   {
      withAxes = new MODEqAxes(withName);
   }
   else if (ofType == "MODEc")
   {
      withAxes = new MODEcAxes(withName);
   }
   else if (ofType == "ObjectReferenced")  
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
   else if (ofType == "BodyInertial")
   {
      withAxes = new BodyInertialAxes(withName);
   }
   else if ((ofType == "GSE") || (ofType == "GeocentricSolarEcliptic"))
   {
      withAxes = new GeocentricSolarEclipticAxes(withName);
   }
   else if ((ofType == "GSM") || (ofType == "GeocentricSolarMagnetic"))
   {
      withAxes = new GeocentricSolarMagneticAxes(withName);
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
      creatables.push_back("BodyInertial");
      creatables.push_back("GSE");
      creatables.push_back("GSM");
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





