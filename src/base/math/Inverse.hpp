//$Id$
//------------------------------------------------------------------------------
//                              Inverse
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
 * Declares Inverse class.
 */
//------------------------------------------------------------------------------
#ifndef Inverse_hpp
#define Inverse_hpp

#include "MathFunction.hpp"

class GMAT_API Inverse : public MathFunction
{
public:
   Inverse(const std::string &nomme);
   virtual ~Inverse();
   Inverse(const Inverse &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual Real Evaluate();
   virtual bool ValidateInputs(); 
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual Rmatrix MatrixEvaluate();

protected:

  
private:
   
};

#endif // Inverse_hpp
