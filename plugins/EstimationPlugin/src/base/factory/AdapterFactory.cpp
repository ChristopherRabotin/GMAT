//$Id: AdapterFactory.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         AdapterFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/06/18
//
/**
 * Factory used to create the MeasurementModel objects
 */
//------------------------------------------------------------------------------


#include "AdapterFactory.hpp"
#include "MeasurementModel.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
// MeasurementModel *CreateMeasurementModel(const std::string & ofType,
//      const std::string & withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the MeasurementModel class
 *
 * @param ofType the MeasurementModel object to create and return.
 * @param withName the name to give the newly-created MeasurementModel object.
 *
 * @return The MeasurementModel
 *
 * @note We are ignoring the ofType parameter.  Use of this parameter may be
 * added later.
 */
//------------------------------------------------------------------------------
MeasurementModel* AdapterFactory::CreateMeasurementModel(
      const std::string & ofType, const std::string & withName)
{
   return new MeasurementModel(withName);
}


//------------------------------------------------------------------------------
//  AdapterFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AdapterFactory
 * (default constructor).
 */
//------------------------------------------------------------------------------
AdapterFactory::AdapterFactory() :
   Factory(Gmat::MEASUREMENT_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("MeasurementModel");
   }
}

//------------------------------------------------------------------------------
// ~AdapterFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the AdapterFactory base class.
 */
//------------------------------------------------------------------------------
AdapterFactory::~AdapterFactory()
{
}


//------------------------------------------------------------------------------
// AdapterFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class AdapterFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 */
//------------------------------------------------------------------------------
AdapterFactory::AdapterFactory(StringArray createList) :
   Factory(createList,Gmat::MEASUREMENT_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("MeasurementModel");
   }
}


//------------------------------------------------------------------------------
// AdapterFactory(const AdapterFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class AdapterFactory
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
AdapterFactory::AdapterFactory(const AdapterFactory & fact) :
   Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("MeasurementModel");
   }
}


//------------------------------------------------------------------------------
//  AdapterFactory& operator= (const AdapterFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the AdapterFactory class.
 *
 * @param <fact> the AdapterFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" AdapterFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
AdapterFactory&
      AdapterFactory::operator=(const AdapterFactory & fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("MeasurementModel");
   }
   return *this;
}
