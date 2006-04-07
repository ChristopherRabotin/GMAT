//$Header$
//------------------------------------------------------------------------------
//                                   MathNode
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2006/03/27
//
/**
 * Defines the MathNode base class for Math in scripts.
 */
//------------------------------------------------------------------------------


#ifndef MathNode_hpp
#define MathNode_hpp


#include "GmatBase.hpp"
#include "MathException.hpp"

/**
 * All function classes are derived from this base class.
 */
class GMAT_API MathNode : public GmatBase
{
public:
   MathNode(const std::string &typeStr, const std::string &nomme);
   virtual ~MathNode();
   MathNode(const MathNode &mn);
   MathNode&                   operator=(const MathNode &mn);
   
   // Inherited (GmatBase) methods
   virtual GmatBase*    Clone(void) const;
                                    
   virtual Real Evaluate();
   virtual bool EvaluateInputs();
   virtual void ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount);
   
   Rmatrix MatrixEvaluate();
   
protected:
   Real realValue;
   Rmatrix matrix;
};


#endif //MathNode_hpp


