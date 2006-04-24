//------------------------------------------------------------------------------
//                              Norm
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
 * Declares Norm class.
 */
//------------------------------------------------------------------------------
#ifndef Norm_hpp
#define Norm_hpp

#include "MathFunction.hpp"

class GMAT_API Norm : public MathFunction
{
public:
   Norm(const std::string &nomme);
   virtual ~Norm();
   Norm(const Norm &copy);
                 
   // inherited from GmatBase
   virtual GmatBase* Clone() const;
   virtual Real Evaluate();
   virtual bool EvaluateInputs(); 
   virtual void ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount);
   virtual Rmatrix MatrixEvaluate();

protected:

  
private:
   
};

#endif // Norm_hpp
