//------------------------------------------------------------------------------
//                              Multiply
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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

   virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();
};

#endif // Multiply_hpp
