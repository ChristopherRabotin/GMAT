//------------------------------------------------------------------------------
//                              Sin
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/04/10
//
/**
 * Declares Sin class.
 */
//------------------------------------------------------------------------------
#ifndef Sin_hpp
#define Sin_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API Sin : public MathFunction
{
public:
   Sin(const std::string &nomme);
   virtual ~Sin();
   Sin(const Sin &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
};

#endif // Sin_hpp
