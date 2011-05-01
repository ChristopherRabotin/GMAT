//$Id: OpticalTrackingSystem.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
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


#ifndef OpticalTrackingSystem_hpp
#define OpticalTrackingSystem_hpp

/**
 * Base class for tracking systems that use optical (typically angle)
 * measurements that include light time iteration.
 */
#include "estimation_defs.hpp"
#include "TrackingSystem.hpp"

class ESTIMATION_API OpticalTrackingSystem : public TrackingSystem
{
public:
   OpticalTrackingSystem(const std::string &name);
   virtual ~OpticalTrackingSystem();
   OpticalTrackingSystem(const OpticalTrackingSystem& usn);
   OpticalTrackingSystem& operator=(const OpticalTrackingSystem& usn);

   virtual GmatBase* Clone() const;
};

#endif /* OpticalTrackingSystem_hpp */
