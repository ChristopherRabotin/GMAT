//$Id: MeasurementFactory.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         MeasurementFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/07/09
//
/**
 * Factory class used to create core measurement objects
 */
//------------------------------------------------------------------------------


#include "MeasurementFactory.hpp"
#include "MessageInterface.hpp"

// Specific types supported
#include "GeometricRange.hpp"
#include "GeometricRangeRate.hpp"
#include "GeometricAzEl.hpp"
#include "GeometricRADec.hpp"


//-----------------------------------------------------------------------------
// CoreMeasurement *MeasurementFactory::CreateMeasurement(
//       const std::string & ofType, const std::string & withName)
//-----------------------------------------------------------------------------
/**
 * Creates a CoreMeasurement object with the specified name
 *
 * @param ofType The scripted type of the object
 * @param withName The name of the new object
 *
 * @return A pointer to the new object, or NULL if no new object was created
 */
//-----------------------------------------------------------------------------
CoreMeasurement *MeasurementFactory::CreateMeasurement(
      const std::string & ofType, const std::string & withName)
{
   if (ofType == "GeometricRange")
      return new GeometricRange(withName);
   if (ofType == "GeometricRangeRate")
      return new GeometricRangeRate(withName);
   if (ofType == "GeometricAzEl")
      return new GeometricAzEl(withName);
   if (ofType == "GeometricRADec")
      return new GeometricRADec(withName);

   return NULL;
}


//-----------------------------------------------------------------------------
// MeasurementFactory()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
MeasurementFactory::MeasurementFactory() :
   Factory     (Gmat::CORE_MEASUREMENT)
{
   if (creatables.empty())
   {
      creatables.push_back("GeometricRange");
      creatables.push_back("GeometricRangeRate");
      creatables.push_back("GeometricAzEl");
      creatables.push_back("GeometricRADec");
   }
}


//-----------------------------------------------------------------------------
// ~MeasurementFactory()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
MeasurementFactory::~MeasurementFactory()
{
}


//-----------------------------------------------------------------------------
// MeasurementFactory& operator=(const MeasurementFactory & fact)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fact The factory that gets copied here
 *
 * @return This factory, configured to match fact
 */
//-----------------------------------------------------------------------------
MeasurementFactory& MeasurementFactory::operator=(
      const MeasurementFactory & fact)
{
   Factory::operator=(fact);
   if (creatables.empty())

   {
      creatables.push_back("GeometricRange");
      creatables.push_back("GeometricRangeRate");
      creatables.push_back("GeometricAzEl");
      creatables.push_back("GeometricRADec");
   }

   return *this;
}


//-----------------------------------------------------------------------------
// MeasurementFactory(StringArray createList)
//-----------------------------------------------------------------------------
/**
 * Secondary constructor designed to override the names of the creatables
 *
 * @param createList The new list of creatable objects
 */
//-----------------------------------------------------------------------------
MeasurementFactory::MeasurementFactory(StringArray createList) :
   Factory        (createList, Gmat::CORE_MEASUREMENT)
{
   if (creatables.empty())
   {
      creatables.push_back("GeometricRange");
      creatables.push_back("GeometricRangeRate");
      creatables.push_back("GeometricAzEl");
      creatables.push_back("GeometricRADec");
   }
}



//-----------------------------------------------------------------------------
// MeasurementFactory(const MeasurementFactory& fact)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fact The factory that gets copied here
 */
//-----------------------------------------------------------------------------
MeasurementFactory::MeasurementFactory(const MeasurementFactory& fact) :
   Factory        (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("GeometricRange");
      creatables.push_back("GeometricRangeRate");
      creatables.push_back("GeometricAzEl");
      creatables.push_back("GeometricRADec");
   }
}
