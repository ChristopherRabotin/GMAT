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


#ifndef TDRSSTrackingSystem_hpp
#define TDRSSTrackingSystem_hpp

/**
 * The TrackingSystem used for the united space network
 */
#include "estimation_defs.hpp"
#include "TrackingSystem.hpp"

class ESTIMATION_API TDRSSTrackingSystem : public TrackingSystem
{
public:
   TDRSSTrackingSystem(const std::string &name);
   virtual ~TDRSSTrackingSystem();
   TDRSSTrackingSystem(const TDRSSTrackingSystem& usn);
   TDRSSTrackingSystem& operator=(const TDRSSTrackingSystem& usn);

   virtual GmatBase* Clone() const;
};

#endif /* TDRSSTrackingSystem_hpp */
