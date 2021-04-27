//$Id$
//------------------------------------------------------------------------------
//                           RightAscAdapter
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
* A measurement adapter for right ascension measurements
*/
//------------------------------------------------------------------------------

#ifndef RightAscAdapter_hpp
#define RightAscAdapter_hpp

#include "AngleAdapterDeg.hpp"


/**
* A measurement adapter for right ascension measurements
*/
class ESTIMATION_API RightAscAdapter : public AngleAdapterDeg
{
public:
   RightAscAdapter(const std::string& name);
   virtual ~RightAscAdapter();
   RightAscAdapter(const RightAscAdapter& rak);
   RightAscAdapter&     operator=(const RightAscAdapter& rak);

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

#endif /* RightAscAdapter_hpp */
