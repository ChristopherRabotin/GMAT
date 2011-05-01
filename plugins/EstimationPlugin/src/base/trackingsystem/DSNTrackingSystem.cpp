//$Id: DSNTrackingSystem.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             DSNTrackingSystem
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/02/22 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the DSNTrackingSystem class
 */
//------------------------------------------------------------------------------


#include "DSNTrackingSystem.hpp"
#include "EstimationDefs.hpp"


//------------------------------------------------------------------------------
// DSNTrackingSystem(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The tracking system name
 */
//------------------------------------------------------------------------------
DSNTrackingSystem::DSNTrackingSystem(const std::string &name) :
   TrackingSystem          ("DSNTrackingSystem", name)
{
   objectTypeNames.push_back("DSNTrackingSystem");

   for (Integer i = Gmat::DSN_FIRST_MEASUREMENT; i < Gmat::DSN_MAX_MEASUREMENT;
         ++i)
   {
      allowedMeasurements.push_back(i);
   }
}


//------------------------------------------------------------------------------
// ~DSNTrackingSystem()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DSNTrackingSystem::~DSNTrackingSystem()
{
}


//------------------------------------------------------------------------------
// DSNTrackingSystem(const DSNTrackingSystem & dts)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param dts The instance providing configuration data for the new one
 */
//------------------------------------------------------------------------------
DSNTrackingSystem::DSNTrackingSystem(const DSNTrackingSystem & dts) :
   TrackingSystem          (dts)
{
}


//------------------------------------------------------------------------------
// DSNTrackingSystem& operator=(const USNTrackingSystem & dts)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param dts The instance that supplies parameters for this one
 *
 * @return This instance configured to match usn
 */
//------------------------------------------------------------------------------
DSNTrackingSystem& DSNTrackingSystem::operator=(const DSNTrackingSystem & dts)
{
   if (this != &dts)
   {
      TrackingSystem::operator=(dts);
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
GmatBase* DSNTrackingSystem::Clone() const
{
   return new DSNTrackingSystem(*this);
}
