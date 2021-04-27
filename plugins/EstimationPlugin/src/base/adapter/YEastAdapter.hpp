//$Id$
//------------------------------------------------------------------------------
//                           YEastAdapter
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
* A measurement adapter for YEast measurements
*/
//------------------------------------------------------------------------------

#ifndef YEastAdapter_hpp
#define YEastAdapter_hpp

#include "AngleAdapterDeg.hpp"


/**
* A measurement adapter for YEast measurements
*/
class ESTIMATION_API YEastAdapter : public AngleAdapterDeg
{
public:
   YEastAdapter(const std::string& name);
   virtual ~YEastAdapter();
   YEastAdapter(const YEastAdapter& rak);
   YEastAdapter&        operator=(const YEastAdapter& rak);

   virtual GmatBase*    Clone() const;

   //virtual const MeasurementData&
   //   CalculateMeasurement(bool withEvents = false,
   //      ObservationData* forObservation = NULL,
   //      std::vector<RampTableData>* rampTB = NULL,
   //      bool forSimulation = false);

   virtual Real         CalcMeasValue();
   virtual Rvector6     CalcDerivValue();
};

#endif /* YEastAdapter_hpp */
