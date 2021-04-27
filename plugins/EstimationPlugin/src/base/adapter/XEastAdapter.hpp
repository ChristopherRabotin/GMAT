//$Id$
//------------------------------------------------------------------------------
//                           XEastAdapter
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
* A measurement adapter for XEast measurements
*/
//------------------------------------------------------------------------------

#ifndef XEastAdapter_hpp
#define XEastAdapter_hpp

#include "AngleAdapterDeg.hpp"


/**
* A measurement adapter for XEast measurements
*/
class ESTIMATION_API XEastAdapter : public AngleAdapterDeg
{
public:
   XEastAdapter(const std::string& name);
   virtual ~XEastAdapter();
   XEastAdapter(const XEastAdapter& rak);
   XEastAdapter&      operator=(const XEastAdapter& rak);

   virtual GmatBase*    Clone() const;

   virtual bool         Initialize();

   //virtual const MeasurementData&
   //   CalculateMeasurement(bool withEvents = false,
   //      ObservationData* forObservation = NULL,
   //      std::vector<RampTableData>* rampTB = NULL,
   //      bool forSimulation = false);

   virtual Real         CalcMeasValue();
   virtual Rvector6     CalcDerivValue();
};

#endif /* XEastAdapter_hpp */
