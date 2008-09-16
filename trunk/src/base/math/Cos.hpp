//------------------------------------------------------------------------------
//                              Cos
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
 * Declares Cos class.
 */
//------------------------------------------------------------------------------
#ifndef Cos_hpp
#define Cos_hpp

#include "MathFunction.hpp"
#include "RealUtilities.hpp"

class GMAT_API Cos : public MathFunction
{
public:
   Cos(const std::string &nomme);
   virtual ~Cos();
   Cos(const Cos &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
   virtual Real Evaluate();
};

#endif // Cos_hpp
