//$Header$
//------------------------------------------------------------------------------
//                                  GeocentricSolarMagneticAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
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

   virtual GmatCoordinate::ParameterUsage UsesXAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesYAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesZAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesEopFile() const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;

   // method to initialize the data
   virtual bool Initialize();

   // all classes derived from GmatBase must supply this Clone method;
   virtual GmatBase*       Clone() const;

    
protected:
      
   enum
   {
      GeocentricSolarMagneticAxesParamCount = ObjectReferencedAxesParamCount
   };

   const static Real lambdaD = 70.1;  // degrees West
   const static Real phiD    = 78.6;  // degrees North

   Rvector3          dipoleEF;

   void         ComputeDipoleEarthFixed();
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch);
   
};
#endif // GeocentricSolarEclipticAxes_hpp
