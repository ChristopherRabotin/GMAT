//------------------------------------------------------------------------------
//                              Inverse
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2006/04/20
//
/**
 * Declares Inverse class.
 */
//------------------------------------------------------------------------------
#ifndef Inverse_hpp
#define Inverse_hpp

#include "MathFunction.hpp"

class GMAT_API Inverse : public MathFunction
{
public:
   Inverse(const std::string &nomme);
   virtual ~Inverse();
   Inverse(const Inverse &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   virtual Rmatrix MatrixEvaluate();

protected:

  
private:
   
};

#endif // Inverse_hpp
