//------------------------------------------------------------------------------
//                              Subtract
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/03/31
//
/**
 * Declares Subtract class.
 */
//------------------------------------------------------------------------------
#ifndef Subtract_hpp
#define Subtract_hpp

#include "MathFunction.hpp"

class GMAT_API Subtract : public MathFunction
{
public:
   Subtract(const std::string &nomme);
   virtual ~Subtract();
   Subtract(const Subtract &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();  
};

#endif // Subtract_hpp
