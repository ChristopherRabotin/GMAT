//$Id$
//------------------------------------------------------------------------------
//                           AzimuthAdapter
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
* A measurement adapter for azimuth measurements
*/
//------------------------------------------------------------------------------

#ifndef AzimuthAdapter_hpp
#define AzimuthAdapter_hpp

#include "AngleAdapterDeg.hpp"


/**
* A measurement adapter for azimuth measurements
*/
class ESTIMATION_API AzimuthAdapter : public AngleAdapterDeg
{
public:
   AzimuthAdapter(const std::string& name);
   virtual ~AzimuthAdapter();
   AzimuthAdapter(const AzimuthAdapter& rak);
   AzimuthAdapter&      operator=(const AzimuthAdapter& rak);

   virtual GmatBase*    Clone() const;

   virtual bool         Initialize();

   //virtual const MeasurementData&
   //   CalculateMeasurement(bool withEvents = false,
   //   ObservationData* forObservation = NULL,
   //   std::vector<RampTableData>* rampTB = NULL,
   //   bool forSimulation = false);

   virtual Real         CalcMeasValue();
   virtual Rvector6     CalcDerivValue();
};

#endif /* AzimuthAdapter_hpp */
