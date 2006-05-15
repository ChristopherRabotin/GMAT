//------------------------------------------------------------------------------
//                              Negate
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun, NASA/GSFC
// Created: 2006/05/04
//
/**
 * Declares Negate class.
 */
//------------------------------------------------------------------------------
#ifndef Negate_hpp
#define Negate_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API Negate : public MathFunction
{
public:
   Negate(const std::string &nomme);
   virtual ~Negate();
   Negate(const Negate &copy);
   
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();
};

#endif // Negate_hpp
