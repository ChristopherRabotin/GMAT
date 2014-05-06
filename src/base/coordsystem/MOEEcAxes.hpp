//$Id$
//------------------------------------------------------------------------------
//                                  MOEEcAxes
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
// Created: 2005/05/11
//
/**
 * Definition of the MOEEcAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef MOEEcAxes_hpp
#define MOEEcAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "InertialAxes.hpp"

class GMAT_API MOEEcAxes : public InertialAxes
{
public:

   // default constructor
   MOEEcAxes(const std::string &itsName = "");
   // copy constructor
   MOEEcAxes(const MOEEcAxes &moe);
   // operator = for assignment
   const MOEEcAxes& operator=(const MOEEcAxes &moe);
   // destructor
   virtual ~MOEEcAxes();
   
   // method to initialize the data
   virtual bool Initialize();

   virtual GmatCoordinate::ParameterUsage UsesEpoch() const;
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone(void) const;
   
protected:

   enum
   {
      MOEEcAxesParamCount = InertialAxesParamCount,
   };
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);

};
#endif // MOEEcAxes_hpp
