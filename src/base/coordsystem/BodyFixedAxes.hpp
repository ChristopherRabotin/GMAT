//$Id$
//------------------------------------------------------------------------------
//                                  BodyFixedAxes
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
// Created: 2005/01/25
//
/**
 * Definition of the BodyFixedAxes class.
 *
 * @note There are three data files currently needed:
 *    EOP file containing polar motion (x,y) and UT1-UTC offset
 *    coefficient files containing nutation and planetary coefficients
 */
//------------------------------------------------------------------------------

#ifndef BodyFixedAxes_hpp
#define BodyFixedAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "DynamicAxes.hpp"
#include "EopFile.hpp"
#include "DeFile.hpp"
#include "ItrfCoefficientsFile.hpp"
#include "CelestialBody.hpp"

class GMAT_API BodyFixedAxes : public DynamicAxes
{
public:

   // default constructor
   BodyFixedAxes(const std::string &itsName = "");
   // copy constructor
   BodyFixedAxes(const BodyFixedAxes &bfAxes);
   // operator = for assignment
   const BodyFixedAxes& operator=(const BodyFixedAxes &bfAxes);
   // destructor
   virtual ~BodyFixedAxes();
   
   // methods to set files for reading
   // 3 are needed:
   //    leap second file (NOTE - this should be done in the utilities!!)
   //    EOP file containing polar motion (x,y) and UT1-UTC offset
   //    coefficient files containing nutation and planetary coefficients
   
   virtual GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "FK5") const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;
   
   // method to initialize the data
   virtual bool Initialize();
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;

//   // We need to override this method from
//   // to check for a CelestialBody origin only
//   virtual bool            SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                                        const std::string &name = "");

protected:

   enum
   {
      BodyFixedAxesParamCount = DynamicAxesParamCount,
   };
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

   DeFile                   *de;
   Real                     prevEpoch;
   Real                     prevUpdateInterval;
   Real                     prevOriginUpdateInterval;
   Gmat::RotationDataSource prevLunaSrc;

};
#endif // BodyFixedAxes_hpp
