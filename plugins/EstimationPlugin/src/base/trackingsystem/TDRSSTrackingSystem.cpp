//$Id$
//------------------------------------------------------------------------------
//                             TDRSSTrackingSystem
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/05/10 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the TDRSSTrackingSystem class
 */
//------------------------------------------------------------------------------


#include "TDRSSTrackingSystem.hpp"

//------------------------------------------------------------------------------
// TDRSSTrackingSystem(const std::string & name) :
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the new instance
 */
//------------------------------------------------------------------------------
TDRSSTrackingSystem::TDRSSTrackingSystem(const std::string & name) :
   TrackingSystem          ("TDRSSTrackingSystem", name)
{
   objectTypeNames.push_back("TDRSSTrackingSystem");

   for (Integer i = Gmat::TDRSS_FIRST_MEASUREMENT; i < Gmat::TDRSS_MAX_MEASUREMENT;
         ++i)
   {
      allowedMeasurements.push_back(i);
   }
}


//------------------------------------------------------------------------------
// ~TDRSSTrackingSystem()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TDRSSTrackingSystem::~TDRSSTrackingSystem()
{
}


//------------------------------------------------------------------------------
// TDRSSTrackingSystem(const TDRSSTrackingSystem& usn)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param usn The instance that is copied into the new one
 */
//------------------------------------------------------------------------------
TDRSSTrackingSystem::TDRSSTrackingSystem(const TDRSSTrackingSystem& usn) :
   TrackingSystem          (usn)
{
}


//------------------------------------------------------------------------------
// TDRSSTrackingSystem& operator=(const TDRSSTrackingSystem & usn)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param usn The instance that supplies parameters for this one
 *
 * @return This instance configured to match usn
 */
//------------------------------------------------------------------------------
TDRSSTrackingSystem& TDRSSTrackingSystem::operator=(const TDRSSTrackingSystem & usn)
{
   if (this != &usn)
   {
      TrackingSystem::operator=(usn);
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
GmatBase* TDRSSTrackingSystem::Clone() const
{
   return new TDRSSTrackingSystem(*this);
}
