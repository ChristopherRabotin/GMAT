//$Id: GeometricAzEl.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         GeometricAzEl
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc./ Wendy Shoan, GSFC/GSSB
// Created: 2009.08.11
//
/**
 * Definition of the geometric azimuth/elevation measurement.
 */
//------------------------------------------------------------------------------


#ifndef GeometricAzEl_hpp
#define GeometricAzEl_hpp

#include "geometricmeasurement_defs.hpp"
#include "CoreMeasurement.hpp"

/**
 * Class for instantaneous Azimuth/Elevation measurements
 */
class GEOMETRICMEAS_API GeometricAzEl: public CoreMeasurement
{
public:
   GeometricAzEl(const std::string &name = "");
   virtual ~GeometricAzEl();
   GeometricAzEl(const GeometricAzEl &aem);
   GeometricAzEl& operator=(const GeometricAzEl &aem);

   virtual GmatBase*       Clone() const;
   virtual bool            Initialize();
   virtual const std::vector<RealArray>&  
                           CalculateMeasurementDerivatives(GmatBase *obj, Integer id);

protected:
   bool                    Evaluate(bool withEvents = false);
};

#endif /* GeometricAzEl_hpp */
