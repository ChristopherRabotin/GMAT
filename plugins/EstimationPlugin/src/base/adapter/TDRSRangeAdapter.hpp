//$Id$
//------------------------------------------------------------------------------
//                           TDRSRangeAdapter
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

#ifndef TDRSRangeAdapter_hpp
#define TDRSRangeAdapter_hpp

#include "RangeAdapterKm.hpp"


/**
 * A measurement adapter for GN range measurement
 */
class ESTIMATION_API TDRSRangeAdapter: public RangeAdapterKm
{
public:
   TDRSRangeAdapter(const std::string& name);
   virtual ~TDRSRangeAdapter();
   TDRSRangeAdapter(const TDRSRangeAdapter& rak);
   TDRSRangeAdapter&      operator=(const TDRSRangeAdapter& rak);

   virtual GmatBase*    Clone() const;

   virtual const MeasurementData&
      CalculateMeasurement(bool withEvents = false,
      ObservationData* forObservation = NULL,
      std::vector<RampTableData>* rampTB = NULL,
      bool forSimulation = false);

   virtual const std::vector<RealArray>&
      CalculateMeasurementDerivatives(GmatBase *obj,
      Integer id);

};

#endif /* TDRSRangeAdapter_hpp */
