//$Id: USNTwoWayRange.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         USNTwoWayRange
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
 * The USN 2-way range core measurement model.
 */
//------------------------------------------------------------------------------


#ifndef USNTwoWayRange_hpp
#define USNTwoWayRange_hpp

#include "estimation_defs.hpp"
#include "TwoWayRange.hpp"


/**
 * Universal Space Network 2-Way Range Measurement Model
 */
class ESTIMATION_API USNTwoWayRange: public TwoWayRange
{
public:
   USNTwoWayRange(const std::string nomme = "");
   virtual ~USNTwoWayRange();
   USNTwoWayRange(const USNTwoWayRange& usn);
   USNTwoWayRange& operator=(const USNTwoWayRange& usn);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();

   virtual const std::vector<RealArray>& CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id);

protected:
   /// Inertial Range rate of the target spacecraft
   Real                 targetRangeRate;
   /// Range rate of the uplink
   Real                 uplinkRangeRate;
   /// Range rate of the downlink
   Real                 downlinkRangeRate;

   virtual bool         Evaluate(bool withEvents = false);
};

#endif /* USNTwoWayRange_hpp */
