//------------------------------------------------------------------------------
//                              Exp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/04/14
//
/**
 * Declares Exp class.
 */
//------------------------------------------------------------------------------
#ifndef Exp_hpp
#define Exp_hpp

#include "MathFunction.hpp"

class GMAT_API Exp : public MathFunction
{
public:
   Exp(const std::string &nomme);
   virtual ~Exp();
   Exp(const Exp &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
};

#endif // Exp_hpp
