//$Id$
//------------------------------------------------------------------------------
//                                  MJ2000EcAxes
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
// Created: 2005/01/06
//
/**
 * Definition of the MJ2000EcAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef MJ2000EcAxes_hpp
#define MJ2000EcAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "InertialAxes.hpp"

class GMAT_API MJ2000EcAxes : public InertialAxes
{
public:

   // default constructor
   MJ2000EcAxes(const std::string &itsName = "");
   // copy constructor
   MJ2000EcAxes(const MJ2000EcAxes &MJ2000);
   // operator = for assignment
   const MJ2000EcAxes& operator=(const MJ2000EcAxes &MJ2000);
   // destructor
   virtual ~MJ2000EcAxes();
   
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
      MJ2000EcAxesParamCount = InertialAxesParamCount,
   };
   
   //static const std::string PARAMETER_TEXT[MJ2000EcAxesParamCount - 
   //                                        InertialAxesParamCount];
   
   //static const Gmat::ParameterType PARAMETER_TYPE[MJ2000EcAxesParamCount - 
   //                                                InertialAxesParamCount];
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

   // no additional data at this time
};
#endif // MJ2000EcAxes_hpp
