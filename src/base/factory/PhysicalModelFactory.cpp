//$Id$
//------------------------------------------------------------------------------
//                            PhysicalModelFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/22
//
/**
 *  Implementation code for the PhysicalModelFactory class, responsible for
 *  creating PhysicalModel objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "PhysicalModelFactory.hpp"
#include "PointMassForce.hpp"
#include "SolarRadiationPressure.hpp"
#include "DragForce.hpp" 
#include "GravityField.hpp" 
#include "RelativisticCorrection.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreatePhysicalModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested PhysicalModel 
 * class.
 *
 * @param <ofType> the PhysicalModel object to create and return.
 * @param <withName> the name to give the newly-created PhysicalModel object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
PhysicalModel* PhysicalModelFactory::CreateObject(const std::string &ofType,
                                                  const std::string &withName)
{
   return CreatePhysicalModel(ofType, withName);
}


//------------------------------------------------------------------------------
//  CreatePhysicalModel(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested PhysicalModel 
 * class.
 *
 * @param <ofType> the PhysicalModel object to create and return.
 * @param <withName> the name to give the newly-created PhysicalModel object.
 *
 * @note As of 2003/10/14, we are ignoring the withName parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
PhysicalModel* PhysicalModelFactory::CreatePhysicalModel(
                                     const std::string &ofType,
                                     const std::string &withName)
{
   if (ofType == "PointMassForce")
       return new PointMassForce(withName);
   else if (ofType == "SolarRadiationPressure")
       return new SolarRadiationPressure(withName);
   else if (ofType == "DragForce")
       return new DragForce(withName);
   else if (ofType == "GravityField")
       return new GravityField(withName, "Earth");
   else if (ofType == "RelativisticCorrection")
       return new RelativisticCorrection(withName, "Earth");
   return NULL;
}


//------------------------------------------------------------------------------
//  PhysicalModelFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PhysicalModelFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
PhysicalModelFactory::PhysicalModelFactory() 
   :
   Factory(Gmat::PHYSICAL_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("PointMassForce");
      creatables.push_back("GravityField");
      creatables.push_back("SolarRadiationPressure");
      creatables.push_back("DragForce");
      creatables.push_back("RelativisticCorrection");
   }
}

//------------------------------------------------------------------------------
//  PhysicalModelFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PhysicalModelFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
PhysicalModelFactory::PhysicalModelFactory(StringArray createList) 
   :
   Factory(createList,Gmat::PHYSICAL_MODEL)
{
}

//------------------------------------------------------------------------------
//  PhysicalModelFactory(const PhysicalModelFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class PhysicalModelFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
PhysicalModelFactory::PhysicalModelFactory(const PhysicalModelFactory &fact) 
   :
   Factory(fact)
{
}

//------------------------------------------------------------------------------
//  PhysicalModelFactory& operator= (const PhysicalModelFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the PhysicalModelFactory class.
 *
 * @param <fact> the PhysicalModelFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" PhysicalModelFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
PhysicalModelFactory& PhysicalModelFactory::operator= (
                                             const PhysicalModelFactory &fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~PhysicalModelFactory()
//------------------------------------------------------------------------------
/**
* Destructor for the PhysicalModelFactory base class.
 *
 */
//------------------------------------------------------------------------------
PhysicalModelFactory::~PhysicalModelFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

