//$Id: DSNTwoWayRange.hpp 65 2010-03-04 00:10:28Z djconway@NDC $
//------------------------------------------------------------------------------
//                         DSNTwoWayRange
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2010/03/08
//
/**
 * The DSN 2-way range core measurement model.
 */
//------------------------------------------------------------------------------


#ifndef DSNTwoWayRange_hpp
#define DSNTwoWayRange_hpp

#include "estimation_defs.hpp"
#include "TwoWayRange.hpp"


/**
 * Deep Space Network 2-Way Range Measurement Model
 */
class ESTIMATION_API DSNTwoWayRange: public TwoWayRange
{
public:
   DSNTwoWayRange(const std::string nomme = "");
   virtual ~DSNTwoWayRange();
   DSNTwoWayRange(const DSNTwoWayRange& usn);
   DSNTwoWayRange& operator=(const DSNTwoWayRange& usn);

   virtual GmatBase* Clone() const;
   virtual bool Initialize();

   virtual const std::vector<RealArray>& CalculateMeasurementDerivatives(
         GmatBase *obj, Integer id);

protected:
   /// Mapping between station ID and associated frequency
   std::map<std::string,Real>      freqMap;

   /// Inertial Range rate of the target spacecraft
   Real                 targetRangeRate;
   /// Range rate of the uplink
   Real                 uplinkRangeRate;
   /// Range rate of the downlink
   Real                 downlinkRangeRate;

   virtual bool                  Evaluate(bool withEvents = false);
   Real                          GetFrequencyFactor(Real frequency = 0.0);
};

#endif /* DSNTwoWayRange_hpp */
