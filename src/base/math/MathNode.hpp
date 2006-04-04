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
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
                                               
   virtual Real Evaluate() const;
   virtual bool EvaluateInputs() const;
   Rmatrix *MatrixEvaluate();
   void ReportOutputs(Integer &type, Integer &rowCount, Integer &colCount);
   

protected:

    Real realValue;
    Rmatrix *matrix;

    enum
    {
      REAL_VAR = GmatBaseParamCount,
      MATRIX,
      MathNodeParamCount  /// Count of the parameters for this class
    };

    static const std::string PARAMETER_TEXT[MathNodeParamCount - GmatBaseParamCount];
    static const Gmat::ParameterType PARAMETER_TYPE[MathNodeParamCount - GmatBaseParamCount];

};


#endif //MathNode_hpp
