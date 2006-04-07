//------------------------------------------------------------------------------
//                              Divide
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: LaMont Ruley
// Created: 2006/04/07
//
/**
 * Declares Divide class.
 */
//------------------------------------------------------------------------------
#ifndef Divide_hpp
#define Divide_hpp

#include "MathFunction.hpp"

class GMAT_API Divide : public MathFunction
{
public:
   Divide(const std::string &nomme);
   virtual ~Divide();
   Divide(const Divide &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();

protected:

  
private:
   
};

#endif // Divide_hpp
