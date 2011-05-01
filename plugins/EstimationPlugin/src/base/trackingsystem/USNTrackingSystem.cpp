//$Id$
//------------------------------------------------------------------------------
//                             USNTrackingSystem
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/02/12 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the USNTrackingSystem class
 */
//------------------------------------------------------------------------------


#include "USNTrackingSystem.hpp"

//------------------------------------------------------------------------------
// USNTrackingSystem(const std::string & name) :
//------------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param name The name of the new instance
 */
//------------------------------------------------------------------------------
USNTrackingSystem::USNTrackingSystem(const std::string & name) :
   TrackingSystem          ("USNTrackingSystem", name)
{
   objectTypeNames.push_back("USNTrackingSystem");

   for (Integer i = Gmat::USN_FIRST_MEASUREMENT; i < Gmat::USN_MAX_MEASUREMENT;
         ++i)
   {
      allowedMeasurements.push_back(i);
   }
}


//------------------------------------------------------------------------------
// ~USNTrackingSystem()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
USNTrackingSystem::~USNTrackingSystem()
{
}


//------------------------------------------------------------------------------
// USNTrackingSystem(const USNTrackingSystem& usn)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param usn The instance that is copied into the new one
 */
//------------------------------------------------------------------------------
USNTrackingSystem::USNTrackingSystem(const USNTrackingSystem& usn) :
   TrackingSystem          (usn)
{
}


//------------------------------------------------------------------------------
// USNTrackingSystem& operator=(const USNTrackingSystem & usn)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param usn The instance that supplies parameters for this one
 *
 * @return This instance configured to match usn
 */
//------------------------------------------------------------------------------
USNTrackingSystem& USNTrackingSystem::operator=(const USNTrackingSystem & usn)
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
GmatBase* USNTrackingSystem::Clone() const
{
   return new USNTrackingSystem(*this);
}
