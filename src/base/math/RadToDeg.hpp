//$Id$
//------------------------------------------------------------------------------
//                              RadToDeg
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
 * Declares RadToDeg class.
 */
//------------------------------------------------------------------------------
#ifndef RadToDeg_hpp
#define RadToDeg_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API RadToDeg : public MathFunction
{
public:
   RadToDeg(const std::string &nomme);
   virtual ~RadToDeg();
   RadToDeg(const RadToDeg &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
   //virtual Rmatrix MatrixEvaluate();
protected:

private:
   
};

#endif // RadToDeg_hpp
