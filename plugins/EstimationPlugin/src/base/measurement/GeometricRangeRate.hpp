//$Id: GeometricRangeRate.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         GeometricRangeRate
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc./Wendy Shoan, GSFC/GSSB
// Created: 2009.08.11
//
/**
 * Definition of the geometric range rate measurement.
 */
//------------------------------------------------------------------------------


#ifndef GeometricRangeRate_hpp
#define GeometricRangeRate_hpp

#include "estimation_defs.hpp"
#include "GeometricMeasurement.hpp"

class ESTIMATION_API GeometricRangeRate: public GeometricMeasurement
{
public:
   GeometricRangeRate(const std::string &name = "");
   virtual ~GeometricRangeRate();
   GeometricRangeRate(const GeometricRangeRate &rrm);
   GeometricRangeRate& operator=(const GeometricRangeRate &rrm);

   virtual GmatBase*       Clone() const;
   virtual bool            Initialize();
   virtual const std::vector<RealArray>&  
                           CalculateMeasurementDerivatives(GmatBase *obj, Integer id);

protected:
   bool                    Evaluate(bool withEvents = false);
};

#endif /* GeometricRangeRate_hpp */
