//$Id$
//------------------------------------------------------------------------------
//                                   MathFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

class GMAT_API MathFunction : public MathNode
{
public:
   MathFunction(const std::string &typeStr, const std::string &nomme);
   virtual ~MathFunction();
   MathFunction(const MathFunction &mf);
   MathFunction& operator=(const MathFunction &mf);
   
   virtual Real Evaluate();
   virtual Rmatrix MatrixEvaluate();
   
   virtual bool SetChildren(MathNode *leftChild, MathNode *rightChild);
   virtual MathNode* GetLeft();
   virtual MathNode* GetRight();
   
protected:

   MathNode *leftNode;
   MathNode *rightNode;
   
   void GetScalarOutputInfo(Integer &type, Integer &rowCount,
                            Integer &colCount);
   void GetMatrixOutputInfo(Integer &type, Integer &rowCount,
                            Integer &colCount, bool allowScalarInput);
   bool ValidateScalarInputs();
   bool ValidateMatrixInputs(bool allowScalarInput);
   
//    /// Parameter IDs
//    enum
//    {
//       MathFunctionParamCount = MathNodeParamCount,
//    };
};

#endif //MathFunction_hpp
