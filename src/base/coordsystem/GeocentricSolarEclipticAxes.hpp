//$Id$
//------------------------------------------------------------------------------
//                                  GeocentricSolarEclipticAxes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/06/02
//
/**
 * Definition of the GeocentricSolarEclipticAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef GeocentricSolarEclipticAxes_hpp
#define GeocentricSolarEclipticAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "DynamicAxes.hpp"
#include "ObjectReferencedAxes.hpp"

class GMAT_API GeocentricSolarEclipticAxes : public ObjectReferencedAxes
{
public:

   // default constructor
   GeocentricSolarEclipticAxes(const std::string &itsName = "");
   // copy constructor
   GeocentricSolarEclipticAxes(const GeocentricSolarEclipticAxes &gse);
   // operator = for assignment
   const GeocentricSolarEclipticAxes& operator=(const GeocentricSolarEclipticAxes &gse);
   // destructor
   virtual ~GeocentricSolarEclipticAxes();

   virtual bool                           IsParameterReadOnly(const Integer id) const;

   virtual GmatCoordinate::ParameterUsage UsesPrimary() const;
   virtual GmatCoordinate::ParameterUsage UsesSecondary() const;
   virtual GmatCoordinate::ParameterUsage UsesXAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesYAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesZAxis() const;
   
   // all classes derived from GmatBase must supply this Clone method;
   virtual GmatBase*       Clone() const;

    
protected:
      
   enum
   {
      GeocentricSolarEclipticAxesParamCount = ObjectReferencedAxesParamCount
   };
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);
   
};
#endif // GeocentricSolarEclipticAxes_hpp
