//$Id$
//------------------------------------------------------------------------------
//                           DerivativeModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Created: 2008/03/28
//
/**
 * Base class for derivatives used by the Solvers.
 */
//------------------------------------------------------------------------------

#ifndef DerivativeModel_hpp
#define DerivativeModel_hpp

#include "gmatdefs.hpp"


/**
 * Base class for gradients, Jacobians, Hessians, and so forth.
 */
class GMAT_API DerivativeModel
{
public:
   /// Enumerated mode options used when calculating derivatives 
   enum derivativeMode {
      FORWARD_DIFFERENCE,
      CENTRAL_DIFFERENCE,
      BACKWARD_DIFFERENCE,
      USER_SUPPLIED
   };

public:
   DerivativeModel();
   virtual ~DerivativeModel();
   DerivativeModel(const DerivativeModel& dm);
   DerivativeModel&     operator=(const DerivativeModel& dm);
	
   void                 SetDifferenceMode(derivativeMode mode);
   virtual bool         Initialize(UnsignedInt varCount, 
                                   UnsignedInt componentCount = 1);
   virtual void         Achieved(Integer pertNumber, Integer componentId, 
                                 Real dx, Real value, bool plusEffect = true);
   virtual bool         Calculate(std::vector<Real> &) = 0;

protected:
   /// Specifier for the calculation mode
   derivativeMode       calcMode;
   /// The total number of variables in the model
   Integer              variableCount;

   /** 
    * The perturbation step taken for finite differencing.  The current model 
    * assumes forward and backward steps have the same magnitude when using 
    * central differencing.
    */
   std::vector<Real>    pert;
   /// The results of a forward perturbation
   std::vector<Real>    plusPertEffect;
   /// The results of a backward perturbation
   std::vector<Real>    minusPertEffect;
};

#endif /*DerivativeModel_hpp*/
