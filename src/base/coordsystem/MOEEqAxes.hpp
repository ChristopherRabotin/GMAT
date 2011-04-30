//$Id$
//------------------------------------------------------------------------------
//                                  MOEEqAxes
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
// Created: 2005/04/28
//
/**
 * Definition of the MOEEqAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef MOEEqAxes_hpp
#define MOEEqAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "InertialAxes.hpp"

class GMAT_API MOEEqAxes : public InertialAxes
{
public:

   // default constructor
   MOEEqAxes(const std::string &itsName = "");
   // copy constructor
   MOEEqAxes(const MOEEqAxes &moe);
   // operator = for assignment
   const MOEEqAxes& operator=(const MOEEqAxes &moe);
   // destructor
   virtual ~MOEEqAxes();
   
   // method to initialize the data
   virtual bool Initialize();

   virtual GmatCoordinate::ParameterUsage UsesEpoch() const;
   //virtual GmatCoordinate::ParameterUsage UsesEopFile() const;
   //virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   
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
      MOEEqAxesParamCount = InertialAxesParamCount,
   };
   
   //static const std::string PARAMETER_TEXT[MOEEqAxesParamCount - 
   //                                        InertialAxesParamCount];
   
   //static const Gmat::ParameterType PARAMETER_TYPE[MOEEqAxesParamCount - 
   //                                                InertialAxesParamCount];
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

   // no additional data at this time
};
#endif // MOEEqAxes_hpp
