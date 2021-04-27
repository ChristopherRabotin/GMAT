//$Id$
//------------------------------------------------------------------------------
//                           RangeSkinAdapter
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
// Created: May, 16 2019
/**
* A measurement adapter for c-band measurements
*/
//------------------------------------------------------------------------------

#ifndef RangeSkinAdapter_hpp
#define RangeSkinAdapter_hpp

#include "GNRangeAdapter.hpp"

/**
* A measurement adapter for c-band measurements
*/
class ESTIMATION_API RangeSkinAdapter : public GNRangeAdapter
{
public:
   RangeSkinAdapter(const std::string& name);
   virtual ~RangeSkinAdapter();
   RangeSkinAdapter(const RangeSkinAdapter& rsa);
   RangeSkinAdapter&      operator=(const RangeSkinAdapter& rsa);

   virtual GmatBase*    Clone() const;

   virtual void ComputeBiasAndNoise(const std::string& measType, 
                                    const Integer numTrip);
   
   virtual void ApplyBiasAndNoise(const std::string& measType,
      const RealArray& corrections, const RealArray& values);
   
   virtual Real ApplyMultiplier(const std::string& useMeasType,
      const Real factor, const GmatBase* obj);

   virtual const std::vector<RealArray>&
      CalculateMeasurementDerivatives(GmatBase *obj,
         Integer id);

   virtual void ComputeMeasurementBias(const std::string biasName,
      const std::string measType,
      Integer numTrip);

   virtual void ComputeMeasurementNoiseSigma(const std::string noiseSigmaName,
      const std::string measType,
      Integer numTrip);
};

#endif /* RangeSkinAdapter_hpp */