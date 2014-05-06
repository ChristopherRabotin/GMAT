//$Id$
//------------------------------------------------------------------------------
//                                  GeocentricSolarMagneticAxes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2014 United States Government as represented by the
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
 * Definition of the GeocentricSolarMagneticAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef GeocentricSolarMagneticAxes_hpp
#define GeocentricSolarMagneticAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "DynamicAxes.hpp"
#include "ObjectReferencedAxes.hpp"

class GMAT_API GeocentricSolarMagneticAxes : public ObjectReferencedAxes
{
public:

   // default constructor
   GeocentricSolarMagneticAxes(const std::string &itsName = "");
   // copy constructor
   GeocentricSolarMagneticAxes(const GeocentricSolarMagneticAxes &gse);
   // operator = for assignment
   const GeocentricSolarMagneticAxes& operator=(const GeocentricSolarMagneticAxes &gse);
   // destructor
   virtual ~GeocentricSolarMagneticAxes();

   virtual bool                           IsParameterReadOnly(const Integer id) const;

   virtual GmatCoordinate::ParameterUsage UsesPrimary() const;
   virtual GmatCoordinate::ParameterUsage UsesSecondary() const;
   virtual GmatCoordinate::ParameterUsage UsesXAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesYAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesZAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "FK5") const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;

   // method to initialize the data
   virtual bool Initialize();

   // all classes derived from GmatBase must supply this Clone method;
   virtual GmatBase*       Clone() const;

    
protected:
      
   enum
   {
      GeocentricSolarMagneticAxesParamCount = ObjectReferencedAxesParamCount
   };

   static const Real lambdaD;  // degrees West
   static const Real phiD;     // degrees North

   Rvector3         dipoleEF;
   const Real       *dipEF;
   Rvector6         rvSunVec;
   const Real       *rvSun;

   void         ComputeDipoleEarthFixed();
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);
   
};
#endif // GeocentricSolarEclipticAxes_hpp
