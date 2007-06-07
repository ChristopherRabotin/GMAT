//$Header$
//------------------------------------------------------------------------------
//                            AttitudeFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
// Author: Wendy Shoan
// Created: 2006.03.16
//
/**
 *  Implementation code for the AttitudeFactory class, responsible for
 *  creating Attitude objects.
 */
//------------------------------------------------------------------------------
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "AttitudeFactory.hpp"
#include "Attitude.hpp"   // for Attitude class
#include "CSFixed.hpp"
#include "Spinner.hpp"
//#include "ThreeAxis.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateAttitude(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Attitude class 
 *
 * @param <ofType> the Attitude object to create and return.
 * @param <withName> the name to give the newly-created Attitude object.
 *
 */
//------------------------------------------------------------------------------
Attitude* AttitudeFactory::CreateAttitude(const std::string &ofType,
                                          const std::string &withName)
{
   if ((ofType == "CSFixed") || (ofType == "CoordinateSystemFixed"))
      return new CSFixed(withName);
   else if (ofType == "Spinner")
      return new Spinner(withName);
//   //else if (ofType == "ThreeAxis")   // TBD
//   //   return new ThreeAxis(withName);
   else
   {
      return NULL;   // doesn't match any known type of Attitude Model
   }
}


//------------------------------------------------------------------------------
//  AttitudeFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AttitudeFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
AttitudeFactory::AttitudeFactory() :
Factory(Gmat::ATTITUDE)
{
   if (creatables.empty())
   {
      //creatables.push_back("CSFixed");
      creatables.push_back("CoordinateSystemFixed");
      creatables.push_back("Spinner");
      //creatables.push_back("ThreeAxis");
      // others TBD
   }
}

//------------------------------------------------------------------------------
//  AttitudeFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AttitudeFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 *
 */
//------------------------------------------------------------------------------
AttitudeFactory::AttitudeFactory(StringArray createList) :
Factory(createList,Gmat::ATTITUDE)
{
   if (creatables.empty())
   {
      //creatables.push_back("CSFixed");
      creatables.push_back("CoordinateSystemFixed");
      creatables.push_back("Spinner");
      //creatables.push_back("ThreeAxis");
      // others TBD
   }
}

//------------------------------------------------------------------------------
//  AttitudeFactory(const AttitudeFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class AttitudeFactory 
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
AttitudeFactory::AttitudeFactory(const AttitudeFactory& fact) :
Factory(fact)
{
   if (creatables.empty())
   {
      //creatables.push_back("CSFixed");
      creatables.push_back("CoordinateSystemFixed");
      creatables.push_back("Spinner");
      //creatables.push_back("ThreeAxis");
      // others TBD
   }
}

//------------------------------------------------------------------------------
//  AttitudeFactory& operator= (const AttitudeFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the AttitudeFactory class.
 *
 * @param <fact> the AttitudeFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" AttitudeFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
AttitudeFactory& AttitudeFactory::operator= (const AttitudeFactory& fact)
{
   if (this == &fact) return *this;
   Factory::operator=(fact);
   if (creatables.empty())
   {
      //creatables.push_back("CSFixed");
      creatables.push_back("CoordinateSystemFixed");
      creatables.push_back("Spinner");
      //creatables.push_back("ThreeAxis");
      // others TBD
   }
   return *this;
}

//------------------------------------------------------------------------------
// ~AttitudeFactory()
//------------------------------------------------------------------------------
/**
   * Destructor for the AttitudeFactory base class.
   *
   */
//------------------------------------------------------------------------------
AttitudeFactory::~AttitudeFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
