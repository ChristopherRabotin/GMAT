//------------------------------------------------------------------------------
//                              Sqrt
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
 * Declares Sqrt class.
 */
//------------------------------------------------------------------------------
#ifndef Sqrt_hpp
#define Sqrt_hpp

#include "MathFunction.hpp"

class GMAT_API Sqrt : public MathFunction
{
public:
   Sqrt(const std::string &nomme);
   virtual ~Sqrt();
   Sqrt(const Sqrt &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual Real Evaluate();
};

#endif // Sqrt_hpp
