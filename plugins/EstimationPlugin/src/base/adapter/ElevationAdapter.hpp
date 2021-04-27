//$Id$
//------------------------------------------------------------------------------
//                           ElevationAdapter
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
// Author: 
// Created: Aug 31, 2018
/**
* A measurement adapter for elevation measurements
*/
//------------------------------------------------------------------------------

#ifndef ElevationAdapter_hpp
#define ElevationAdapter_hpp

#include "AngleAdapterDeg.hpp"


/**
* A measurement adapter for elevation measurements
*/
class ESTIMATION_API ElevationAdapter : public AngleAdapterDeg
{
public:
   ElevationAdapter(const std::string& name);
   virtual ~ElevationAdapter();
   ElevationAdapter(const ElevationAdapter& rak);
   ElevationAdapter&      operator=(const ElevationAdapter& rak);

   virtual GmatBase*    Clone() const;

   //virtual const MeasurementData&
   //   CalculateMeasurement(bool withEvents = false,
   //   ObservationData* forObservation = NULL,
   //   std::vector<RampTableData>* rampTB = NULL,
   //   bool forSimulation = false);

   virtual Real         CalcMeasValue();
   virtual Rvector6     CalcDerivValue();
};

#endif /* ElevationAdapter_hpp */
