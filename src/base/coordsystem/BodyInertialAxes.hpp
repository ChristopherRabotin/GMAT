//$Id$
//------------------------------------------------------------------------------
//                                  BodyInertialAxes
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
// Created: 2006.08.01
//
/**
 * Definition of the BodyInertialAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef BodyInertialAxes_hpp
#define BodyInertialAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "InertialAxes.hpp"

class GMAT_API BodyInertialAxes : public InertialAxes
{
public:

   // default constructor
   BodyInertialAxes(const std::string &itsName = "");
   // copy constructor
   BodyInertialAxes(const BodyInertialAxes &bodyInertial);
   // operator = for assignment
   const BodyInertialAxes& operator=(const BodyInertialAxes &bodyInertial);
   // destructor
   virtual ~BodyInertialAxes();
   
   // method to initialize the data
   virtual bool                           Initialize();
   virtual GmatCoordinate::ParameterUsage UsesEpoch() const;

   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*                      Clone(void) const;

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
      BodyInertialAxesParamCount = InertialAxesParamCount,
   };
   
   //static const std::string PARAMETER_TEXT[BodyInertialAxesParamCount - 
   //                                        InertialAxesParamCount];
   
   //static const Gmat::ParameterType PARAMETER_TYPE[BodyInertialAxesParamCount - 
   //                                                InertialAxesParamCount];
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

   // no additional data at this time
};
#endif // BodyInertialAxes_hpp
