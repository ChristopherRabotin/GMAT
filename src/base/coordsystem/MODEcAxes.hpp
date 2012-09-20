//$Id$
//------------------------------------------------------------------------------
//                                  MODEcAxes
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
// Created: 2005/05/11
//
/**
 * Definition of the MODEcAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef MODEcAxes_hpp
#define MODEcAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "MeanOfDateAxes.hpp"

class GMAT_API MODEcAxes : public MeanOfDateAxes
{
public:

   // default constructor
   MODEcAxes(const std::string &itsName = "");
   // copy constructor
   MODEcAxes(const MODEcAxes &mod);
   // operator = for assignment
   const MODEcAxes& operator=(const MODEcAxes &mod);
   // destructor
   virtual ~MODEcAxes();
   
   // method to initialize the data
   virtual bool Initialize();
   
   // all classes derived from GmatBase must supply this Clone method;
   // this must be implemented in the 'leaf' classes
   virtual GmatBase*       Clone() const;
   
protected:

   enum
   {
      MODEcAxesParamCount = MeanOfDateAxesParamCount,
   };
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);
};
#endif // MODEcAxes_hpp
