//$Id$
//------------------------------------------------------------------------------
//                                  MODEqAxes
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
// Created: 2005/05/03
//
/**
 * Definition of the MODEqAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef MODEqAxes_hpp
#define MODEqAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "MeanOfDateAxes.hpp"

class GMAT_API MODEqAxes : public MeanOfDateAxes
{
public:

   // default constructor
   MODEqAxes(const std::string &itsName = "");
   // copy constructor
   MODEqAxes(const MODEqAxes &mod);
   // operator = for assignment
   const MODEqAxes& operator=(const MODEqAxes &mod);
   // destructor
   virtual ~MODEqAxes();
   
   // method to initialize the data
   virtual bool Initialize();
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;
   
protected:

   enum
   {
      MODEqAxesParamCount = MeanOfDateAxesParamCount,
   };
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);
};
#endif // MODEqAxes_hpp
