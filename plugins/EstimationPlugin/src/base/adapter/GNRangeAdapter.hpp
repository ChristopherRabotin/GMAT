//$Id$
//------------------------------------------------------------------------------
//                           GNRangeAdapter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Feb 12, 2014
/**
 * A measurement adapter for GN range measurement
 */
//------------------------------------------------------------------------------

#ifndef GNRangeAdapter_hpp
#define GNRangeAdapter_hpp

#include "RangeAdapterKm.hpp"


/**
 * A measurement adapter for GN range measurement
 */
class ESTIMATION_API GNRangeAdapter: public RangeAdapterKm
{
public:
   GNRangeAdapter(const std::string& name);
   virtual ~GNRangeAdapter();
   GNRangeAdapter(const GNRangeAdapter& rak);
   GNRangeAdapter&      operator=(const GNRangeAdapter& rak);

   virtual GmatBase*    Clone() const;

   virtual void ComputeBiasAndNoise(const std::string& useMeasType,
      const Integer numTrip);

   virtual void ApplyBiasAndNoise(const std::string& useMeasType,
      const RealArray& corrections, 
      const RealArray& values);

   virtual const MeasurementData&
      CalculateMeasurement(bool withEvents = false,
      ObservationData* forObservation = NULL,
      std::vector<RampTableData>* rampTB = NULL,
      bool forSimulation = false);

};

#endif /* GNRangeAdapter_hpp */
