//$Id: MeasurementModelFactory.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         MeasurementModelFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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


#include "MeasurementModelFactory.hpp"
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
MeasurementModel* MeasurementModelFactory::CreateMeasurementModel(
      const std::string & ofType, const std::string & withName)
{
   return new MeasurementModel(withName);
}


//------------------------------------------------------------------------------
//  MeasurementModelFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MeasurementModelFactory
 * (default constructor).
 */
//------------------------------------------------------------------------------
MeasurementModelFactory::MeasurementModelFactory() :
   Factory(Gmat::MEASUREMENT_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("MeasurementModel");
   }
}

//------------------------------------------------------------------------------
// ~MeasurementModelFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the MeasurementModelFactory base class.
 */
//------------------------------------------------------------------------------
MeasurementModelFactory::~MeasurementModelFactory()
{
}


//------------------------------------------------------------------------------
// MeasurementModelFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class MeasurementModelFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects for this factory.
 */
//------------------------------------------------------------------------------
MeasurementModelFactory::MeasurementModelFactory(StringArray createList) :
   Factory(createList,Gmat::MEASUREMENT_MODEL)
{
   if (creatables.empty())
   {
      creatables.push_back("MeasurementModel");
   }
}


//------------------------------------------------------------------------------
// MeasurementModelFactory(const MeasurementModelFactory& fact)
//------------------------------------------------------------------------------
/**
   * This method creates an object of the class MeasurementModelFactory
   * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
MeasurementModelFactory::MeasurementModelFactory(const MeasurementModelFactory & fact) :
   Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("MeasurementModel");
   }
}


//------------------------------------------------------------------------------
//  MeasurementModelFactory& operator= (const MeasurementModelFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the MeasurementModelFactory class.
 *
 * @param <fact> the MeasurementModelFactory object whose data to assign to "this"
 *               factory.
 *
 * @return "this" MeasurementModelFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
MeasurementModelFactory&
      MeasurementModelFactory::operator=(const MeasurementModelFactory & fact)
{
   Factory::operator=(fact);
   if (creatables.empty())
   {
      creatables.push_back("MeasurementModel");
   }
   return *this;
}
