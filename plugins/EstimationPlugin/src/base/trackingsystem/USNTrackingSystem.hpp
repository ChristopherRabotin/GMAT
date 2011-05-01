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


#ifndef USNTrackingSystem_hpp
#define USNTrackingSystem_hpp

#include "estimation_defs.hpp"
#include "TrackingSystem.hpp"

/**
 * The TrackingSystem used for the united space network
 */
class ESTIMATION_API USNTrackingSystem : public TrackingSystem
{
public:
   USNTrackingSystem(const std::string &name);
   virtual ~USNTrackingSystem();
   USNTrackingSystem(const USNTrackingSystem& usn);
   USNTrackingSystem& operator=(const USNTrackingSystem& usn);

   virtual GmatBase* Clone() const;
};

#endif /* USNTrackingSystem_hpp */
