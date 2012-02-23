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

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone(void) const;

   // Parameter access methods - overridden from GmatBase
   /* placeholder - may be needed later
   virtual std::string     GetParameterText(const Integer id) const;
   virtual Integer         GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                           GetParameterType(const Integer id) const;
   virtual std::string     GetParameterTypeString(const Integer id) const;
    */

protected:

   enum
   {
      ICRFAxesParamCount = InertialAxesParamCount,
   };

   //static const std::string PARAMETER_TEXT[ICRFAxesParamCount -
   //                                        InertialAxesParamCount];

   //static const Gmat::ParameterType PARAMETER_TYPE[ICRFAxesParamCount -
   //                                                InertialAxesParamCount];

   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

   // no additional data at this time
};
#endif // ICRFAxes_hpp
