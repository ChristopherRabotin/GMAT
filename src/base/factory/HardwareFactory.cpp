//$Id$
//------------------------------------------------------------------------------
//                         HardwareFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// order 124
//
// Author: Darrel Conway (Based on HardwareFactory)
// Created: 2004/11/10
//
/**
 *  This class is the factory class for hardware.  
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "HardwareFactory.hpp"
#include "FuelTank.hpp"
#include "Thruster.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Hardware* CreateHardware(const std::string &ofType,
//                           const std::string &withName)
//------------------------------------------------------------------------------
/**
 * Creates tanks, thrusters, and other hardware attached to the GMAT spacecraft.
 * 
 * Must be implemented by derived classes that create Hardware objects -
 * in that case, it returns a new Hardware object.  Otherwise, it
 * throws an exception indicating that the class does not create objects of
 * type Hardware.
 *
 * @param <ofType>   specific type of Hardware object to create.
 * @param <withName> name to give to the newly created Hardware object.
 *
 * @return pointer to a new Hardware object.
 *
 * @exception <FactoryException> thrown if the factory does not create
 *                               objects of type Hardware.
 */
//------------------------------------------------------------------------------
Hardware* HardwareFactory::CreateHardware(const std::string &ofType,
                                  const std::string &withName)
{
   if (ofType == "FuelTank")
      return new FuelTank(withName);
   if (ofType == "Thruster")
      return new Thruster(withName);
   return NULL;
}



//------------------------------------------------------------------------------
//  HardwareFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class HardwareFactory
 *  (default constructor)
 */
//------------------------------------------------------------------------------
HardwareFactory::HardwareFactory() :
Factory(Gmat::HARDWARE)
{
   if (creatables.empty())
   {
      creatables.push_back("FuelTank");
      creatables.push_back("Thruster");
   }
}

//------------------------------------------------------------------------------
//  HardwareFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class HardwareFactory
 *  (constructor)
 *
 * @param <createList> the initial list of createble objects for this class
 */
//------------------------------------------------------------------------------
HardwareFactory::HardwareFactory(StringArray createList) :
Factory(createList, Gmat::HARDWARE)
{
   if (creatables.empty())
   {
      creatables.push_back("FuelTank");
      creatables.push_back("Thruster");
   }
}

//------------------------------------------------------------------------------
//  HardwareFactory(const HardwareFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the (base) class HardwareFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
HardwareFactory::HardwareFactory(const HardwareFactory& fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("FuelTank");
      creatables.push_back("Thruster");
   }
}

//------------------------------------------------------------------------------
//  HardwareFactory& operator= (const HardwareFactory& fact)
//------------------------------------------------------------------------------
/**
   * Assignment operator for the HardwareFactory base class.
   *
   * @param <fact> the HardwareFactory object whose data to assign
   *                 to "this" factory.
   *
   * @return "this" HardwareFactory with data of input factory fact.
   */
//------------------------------------------------------------------------------
HardwareFactory& HardwareFactory::operator= (const HardwareFactory& fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("FuelTank");
      creatables.push_back("Thruster");
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~HardwareFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the HardwareFactory base class.
   *
   */
//------------------------------------------------------------------------------
HardwareFactory::~HardwareFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
