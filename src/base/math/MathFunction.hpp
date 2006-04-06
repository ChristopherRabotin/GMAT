//$Header$
//------------------------------------------------------------------------------
//                                   MathFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Waka Waktola
// Created: 2006/04/04
//
/**
 * Defines the Math functions class for Math in scripts.
 */
//------------------------------------------------------------------------------

#ifndef MathFunction_hpp
#define MathFunction_hpp

#include "GmatBase.hpp"
#include "MathNode.hpp"
#include "MathException.hpp"

class MathFunction : public MathNode
{
public:
   MathFunction(const std::string &typeStr, const std::string &nomme);
   virtual ~MathFunction();
   MathFunction(const MathFunction &mf);
   MathFunction& operator=(const MathFunction &mf);
   
   // Inherited (GmatBase) methods
   virtual GmatBase* Clone(void) const; 
                                        
   // Inherited (MathNode) methods                                            
   virtual Real Evaluate() const;
   virtual bool EvaluateInputs() const; 
   
   Rmatrix *MatrixEvaluate();
   bool SetChildren(MathNode *leftChild, MathNode *rightChild);
   MathNode* GetLeft();
   MathNode* GetRight();
   
protected:

   MathNode *leftNode;
   MathNode *rightNode;

    enum
    {
      LEFT_NODE = MathNodeParamCount,
      RIGHT_NODE,
      MathFunctionParamCount
    };
    
    static const std::string PARAMETER_TEXT[MathFunctionParamCount - MathNodeParamCount];
    static const Gmat::ParameterType PARAMETER_TYPE[MathFunctionParamCount - MathNodeParamCount];

};

#endif //MathFunction_hpp
