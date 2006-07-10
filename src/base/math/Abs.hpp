//$Header$
//------------------------------------------------------------------------------
//                              Abs
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2006/06/27
//
/**
 * Declares Abs class.
 */
//------------------------------------------------------------------------------
#ifndef Abs_hpp
#define Abs_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API Abs : public MathFunction
{
public:
   Abs(const std::string &nomme);
   virtual ~Abs();
   Abs(const Abs &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
};

#endif // Abs_hpp
