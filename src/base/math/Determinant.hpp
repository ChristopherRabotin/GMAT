//------------------------------------------------------------------------------
//                              Determinant
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
// Author: Allison Greene
// Created: 2006/04/20
//
/**
 * Declares Determinant class.
 */
//------------------------------------------------------------------------------
#ifndef Determinant_hpp
#define Determinant_hpp

#include "MathFunction.hpp"

class GMAT_API Determinant : public MathFunction
{
public:
   Determinant(const std::string &nomme);
   virtual ~Determinant();
   Determinant(const Determinant &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual bool ValidateInputs(); 
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual Real Evaluate();
//    virtual Rmatrix MatrixEvaluate();

protected:

  
private:
   
};

#endif // Determinant_hpp
