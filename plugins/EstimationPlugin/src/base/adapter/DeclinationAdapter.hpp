//$Id$
//------------------------------------------------------------------------------
//                           DeclinationAdapter
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
* A measurement adapter for declination measurements
*/
//------------------------------------------------------------------------------

#ifndef DeclinationAdapter_hpp
#define DeclinationAdapter_hpp

#include "AngleAdapterDeg.hpp"


/**
* A measurement adapter for declination measurements
*/
class ESTIMATION_API DeclinationAdapter : public AngleAdapterDeg
{
public:
   DeclinationAdapter(const std::string& name);
   virtual ~DeclinationAdapter();
   DeclinationAdapter(const DeclinationAdapter& rak);
   DeclinationAdapter&  operator=(const DeclinationAdapter& rak);

   virtual GmatBase*    Clone() const;

   //virtual const MeasurementData&
   //   CalculateMeasurement(bool withEvents = false,
   //      ObservationData* forObservation = NULL,
   //      std::vector<RampTableData>* rampTB = NULL,
   //      bool forSimulation = false);

   virtual Real         CalcMeasValue();
   virtual Rvector6     CalcDerivValue();
};

#endif /* DeclinationAdapter_hpp */
