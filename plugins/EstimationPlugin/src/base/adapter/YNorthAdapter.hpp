//$Id$
//------------------------------------------------------------------------------
//                           YNorthAdapter
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
* A measurement adapter for YNorth measurements
*/
//------------------------------------------------------------------------------

#ifndef YNorthAdapter_hpp
#define YNorthAdapter_hpp

#include "AngleAdapterDeg.hpp"


/**
* A measurement adapter for YNorth measurements
*/
class ESTIMATION_API YNorthAdapter : public AngleAdapterDeg
{
public:
   YNorthAdapter(const std::string& name);
   virtual ~YNorthAdapter();
   YNorthAdapter(const YNorthAdapter& rak);
   YNorthAdapter&       operator=(const YNorthAdapter& rak);

   virtual GmatBase*    Clone() const;

   //virtual const MeasurementData&
   //   CalculateMeasurement(bool withEvents = false,
   //      ObservationData* forObservation = NULL,
   //      std::vector<RampTableData>* rampTB = NULL,
   //      bool forSimulation = false);

   virtual Real         CalcMeasValue();
   virtual Rvector6     CalcDerivValue();
};

#endif /* YNorthAdapter_hpp */
