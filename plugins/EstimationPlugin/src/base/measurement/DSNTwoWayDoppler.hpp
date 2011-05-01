//$Id: DSNTwoWayDoppler.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         DSNTwoWayDoppler
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS
// Task 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 27, 2010
//
/**
 * Measurement model class for Doppler measurements made by the deep space
 * network
 */
//------------------------------------------------------------------------------


#ifndef DSNTwoWayDoppler_hpp
#define DSNTwoWayDoppler_hpp

#include "estimation_defs.hpp"
#include "AveragedDoppler.hpp"

/**
 * Class defining 2-way Doppler measurements from the Deep Space Network.
 */
class ESTIMATION_API DSNTwoWayDoppler: public AveragedDoppler
{
public:
   DSNTwoWayDoppler(const std::string &withName = "");
   virtual ~DSNTwoWayDoppler();
   DSNTwoWayDoppler(const DSNTwoWayDoppler& dd);
   DSNTwoWayDoppler& operator=(const DSNTwoWayDoppler& dd);

   virtual GmatBase* Clone() const;

   virtual const std::vector<RealArray>& CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id);

protected:
   /// Uplink range rate for the 2 paths
   Real uplinkRangeRate[2];
   /// Downlink range rate for the 2 paths
   Real downlinkRangeRate[2];

   virtual bool Evaluate(bool);
   virtual void SetHardwareDelays(bool loadEvents = true);
};

#endif /* DSNTwoWayDoppler_hpp */
