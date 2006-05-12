//------------------------------------------------------------------------------
//                              Transpose
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
 * Declares Transpose class.
 */
//------------------------------------------------------------------------------
#ifndef Transpose_hpp
#define Transpose_hpp

#include "MathFunction.hpp"

class GMAT_API Transpose : public MathFunction
{
public:
   Transpose(const std::string &nomme);
   virtual ~Transpose();
   Transpose(const Transpose &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   
   virtual void GetOutputInfo(Integer &type, Integer &rowCount, Integer &colCount);
   virtual bool ValidateInputs(); 
//    virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();

protected:

  
private:
   
};

#endif // Transpose_hpp
