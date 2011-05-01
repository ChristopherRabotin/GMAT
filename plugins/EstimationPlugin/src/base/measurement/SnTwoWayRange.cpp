//$Id: SnTwoWayRange.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         SnTwoWayRange
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
// Created: 2009/12/18
//
/**
 * The space network 2-way range measurement model
 */
//------------------------------------------------------------------------------


#include "SnTwoWayRange.hpp"
#include "MeasurementException.hpp"


//-----------------------------------------------------------------------------
// SnTwoWayRange(const std::string &nomme)
//-----------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param nomme Name of the new object
 */
//-----------------------------------------------------------------------------
SnTwoWayRange::SnTwoWayRange(const std::string &nomme) :
   TwoWayRange          ("SnTwoWayRange", nomme)
{
   objectTypeNames.push_back("SnTwoWayRange");
}


//-----------------------------------------------------------------------------
// ~SnTwoWayRange()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
SnTwoWayRange::~SnTwoWayRange()
{
   // TODO Auto-generated destructor stub
}


//-----------------------------------------------------------------------------
// SnTwoWayRange(const SnTwoWayRange& sn) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param sn The model that sets parameters for this new instance
 */
//-----------------------------------------------------------------------------
SnTwoWayRange::SnTwoWayRange(const SnTwoWayRange& sn) :
   TwoWayRange       (sn)
{

}


//-----------------------------------------------------------------------------
// SnTwoWayRange& operator=(const SnTwoWayRange& sn)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param sn The model that sets parameters for this instance
 *
 * @return This object, configured to match sn
 */
//-----------------------------------------------------------------------------
SnTwoWayRange& SnTwoWayRange::operator=(const SnTwoWayRange& sn)
{
   if (this != &sn)
   {
      TwoWayRange::operator=(sn);
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a new model that matches this one, and returns it as a GmatBase
 * pointer
 *
 * @return A new SN 2-way range model configured to match this one
 */
//------------------------------------------------------------------------------
GmatBase* SnTwoWayRange::Clone() const
{
   return new SnTwoWayRange(*this);
}


//------------------------------------------------------------------------------
// const std::vector<RealArray>& CalculateMeasurementDerivatives(
//       GmatBase *obj, Integer id)
//------------------------------------------------------------------------------
/**
 * Calculates the measurement derivatives for the model
 *
 * @param obj The object supplying the "with respect to" parameter
 * @param id The ID of the parameter
 *
 * @return A matrix of the derivative data, contained in a vector of Real
 *         vectors
 */
//------------------------------------------------------------------------------
const std::vector<RealArray>& SnTwoWayRange::CalculateMeasurementDerivatives(
      GmatBase *obj, Integer id)
{
   throw MeasurementException(
         "Measurement derivatives not implemented for SnTwoWayRange");
}


//------------------------------------------------------------------------------
// bool Evaluate(bool withEvents)
//------------------------------------------------------------------------------
/**
 * Calculates measurement values based on the current state of the participants.
 *
 * This method can perform the calculations either with or without event
 * corrections.  When calculating without events, the purpose of the calculation
 * is to determine feasibility of the measurement.
 *
 * @param withEvents Flag used to toggle event inclusion
 *
 * @return true if the measurement was calculated, false if not
 *
 * @note The SnTwoWayRange class is not yet implemented, so this method always
 *       returns false.
 */
//------------------------------------------------------------------------------
bool SnTwoWayRange::Evaluate(bool withEvents)
{
   bool retval = false;

   return retval;
}
