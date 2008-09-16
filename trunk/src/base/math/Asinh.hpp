//$Header$
//------------------------------------------------------------------------------
//                              Asinh
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
 * Declares Asinh class.
 */
//------------------------------------------------------------------------------
#ifndef Asinh_hpp
#define Asinh_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API Asinh : public MathFunction
{
public:
   Asinh(const std::string &nomme);
   virtual ~Asinh();
   Asinh(const Asinh &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
};

#endif // Asinh_hpp
