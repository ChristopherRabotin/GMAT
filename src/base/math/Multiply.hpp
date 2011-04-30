//------------------------------------------------------------------------------
//                              Multiply
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/04/04
//
/**
 * Declares Multiply class.
 */
//------------------------------------------------------------------------------
#ifndef Multiply_hpp
#define Multiply_hpp

#include "MathFunction.hpp"

class GMAT_API Multiply : public MathFunction
{
public:
   Multiply(const std::string &nomme);
   virtual ~Multiply();
   Multiply(const Multiply &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();
};

#endif // Multiply_hpp
