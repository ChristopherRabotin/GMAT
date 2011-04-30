//------------------------------------------------------------------------------
//                              Norm
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
 * Declares Norm class.
 */
//------------------------------------------------------------------------------
#ifndef Norm_hpp
#define Norm_hpp

#include "MathFunction.hpp"

class GMAT_API Norm : public MathFunction
{
public:
   Norm(const std::string &nomme);
   virtual ~Norm();
   Norm(const Norm &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
//    virtual Rmatrix MatrixEvaluate();

protected:

  
private:
   
};

#endif // Norm_hpp
