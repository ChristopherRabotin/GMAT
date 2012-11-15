//$Id:  $
//------------------------------------------------------------------------------
//                                  ICRFAxes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Wendy C. Shoan/GSFC/GSSB
//         Tuan Dang Nguyen/GSFC
// Created: 2012.02.23
//
/**
 * Definition of the ICRFAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef ICRFAxes_hpp
#define ICRFAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "InertialAxes.hpp"

class GMAT_API ICRFAxes : public InertialAxes
{
public:

   // default constructor
   ICRFAxes(const std::string &itsName = "");
   // copy constructor
   ICRFAxes(const ICRFAxes &icrf);
   // operator = for assignment
   const ICRFAxes& operator=(const ICRFAxes &icrf);
   // destructor
   virtual ~ICRFAxes();

   // method to initialize the data
   virtual bool Initialize();

   virtual GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "FK5") const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;

   Rmatrix33  GetRotationMatrix(const A1Mjd &atEpoch, bool forceComputation = false);

protected:

   enum
   {
      ICRFAxesParamCount = InertialAxesParamCount,
   };

   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

};
#endif // ICRFAxes_hpp
