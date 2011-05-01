//$Id$
//------------------------------------------------------------------------------
//                             OpticalTrackingSystem
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: Mar 23, 2010 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the OpticalTrackingSystem class
 */
//------------------------------------------------------------------------------


#include "OpticalTrackingSystem.hpp"


//------------------------------------------------------------------------------
// OpticalTrackingSystem(const std::string & name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The tracking system name
 */
//------------------------------------------------------------------------------
OpticalTrackingSystem::OpticalTrackingSystem(const std::string & name) :
   TrackingSystem("OpticalTrackingSystem", name)
{
   objectTypeNames.push_back("OpticalTrackingSystem");

   // Eventually may want to do this, but for now leave this TS open to all
//   for (Integer i = Gmat::GENERAL_FIRST_MEASUREMENT;
//         i < Gmat::GENERAL_MAX_MEASUREMENT; ++i)
//   {
//      allowedMeasurements.push_back(i);
//   }
}


//------------------------------------------------------------------------------
// ~OpticalTrackingSystem()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
OpticalTrackingSystem::~OpticalTrackingSystem()
{
}


//------------------------------------------------------------------------------
// OpticalTrackingSystem(const OpticalTrackingSystem &ots)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ots The instance providing configuration data for the new one
 */
//------------------------------------------------------------------------------
OpticalTrackingSystem::OpticalTrackingSystem(const OpticalTrackingSystem &ots) :
   TrackingSystem          (ots)
{
}


//------------------------------------------------------------------------------
// OpticalTrackingSystem& operator=(const OpticalTrackingSystem& ots)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ots The instance providing configuration data for this one
 *
 * @return This instance, configured to match ots
 */
//------------------------------------------------------------------------------
OpticalTrackingSystem& OpticalTrackingSystem::operator=(
      const OpticalTrackingSystem& ots)
{
   if (this != &ots)
   {
      TrackingSystem::operator=(ots);
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Replicates this object through a call made on a GmatBase pointer
 *
 * @return A new instance generated through the copy constructor for this object
 */
//------------------------------------------------------------------------------
GmatBase *OpticalTrackingSystem::Clone() const
{
   return new OpticalTrackingSystem(*this);
}
