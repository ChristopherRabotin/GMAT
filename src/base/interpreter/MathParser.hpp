//$Header$
//------------------------------------------------------------------------------
//                                   MathParser
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS task
// 124.
//
// Author: Darrel J. Conway
// Created: 2006/03/16
// Modified: 2006/04/10 Linda Jun /NASA/GSFC
//   - Added actual code
//
/**
 * Class definition for the MathParser.
 *
 * The MathParser class takes a line of script that evaluates to inline math,
 * and breaks that line apart into its component elements using a recursive 
 * descent algorithm.  The resulting representation is stored in a binary tree 
 * structure, which is calculated, depth first, when the expression needs to be
 * evaluated during execution of a script.
 */
//------------------------------------------------------------------------------

#ifndef MathParser_hpp
#define MathParser_hpp


//#include "MathTree.hpp"

#ifdef __USE_SIMPLE_NODE__
#include "SimpleMathNode.hpp"
#else
#include "MathNode.hpp"
#endif

#include "MathException.hpp"

class GMAT_API MathParser
{
public:
   
   MathParser();
   MathParser(const MathParser &copy);
   MathParser& operator=(const MathParser &right);
   virtual ~MathParser();
   
   //void Parse(MathTree *mathTree, const std::string &theEquation);
   #ifdef __USE_SIMPLE_NODE__
   SimpleMathNode* Parse(const std::string &theEquation);
   #else
   MathNode* Parse(const std::string &theEquation);
   #endif
   
protected:

   #ifdef __USE_SIMPLE_NODE__
   SimpleMathNode* ParseNode(const std::string &str);
   SimpleMathNode* CreateNode(const std::string &type,
                              const std::string &exp);
   #else
   MathNode* ParseNode(const std::string &str);
   MathNode* CreateNode(const std::string &type,
                        const std::string &exp);
   #endif
   
   StringArray Decompose(const std::string &str);
   
private:

   StringArray ParseParenthesis(const std::string &str);
   StringArray ParseAddSubtract(const std::string &str);
   StringArray ParseMultDivide(const std::string &str);
   StringArray ParseMatrixOps(const std::string &str);
   StringArray ParsePower(const std::string &str);
   StringArray ParseUnary(const std::string &str);
   StringArray ParseMathFunctions(const std::string &str);
   StringArray ParseUnitConversion(const std::string &str);

   bool IsThisLastCharOfFunction(char ch, const std::string list[],
                                 Integer count);
   bool IsParenPartOfFunction(char lastChar);
   std::string GetFunction(Integer functionType, const std::string &str,
                           std::string &leftStr);
   std::string FindOperator(const std::string &str, UnsignedInt start,
                             std::string &left, std::string &right);
   std::string GetOperatorName(const std::string &op);
   void BuildFunction(const std::string &str, const std::string list[],
                      Integer count, std::string &fnName, std::string &leftStr);
   Integer FindMatchingParen(const std::string &str, UnsignedInt start);
   void FillItems(StringArray &items, const std::string &op,
                  const std::string &left, const std::string &right);
   
   void WriteItems(const std::string &msg, StringArray &items);

   #ifdef __USE_SIMPLE_NODE__
   void WriteNode(SimpleMathNode *node, Integer level);
   #else
   void WriteNode(MathNode *node, Integer level);
   #endif
   
   enum
   {
      MATH_FUNCTION,
      MATRIX_OPS,
      UNIT_CONVERSION,
   };
   
   enum
   {
      SIN = 0,
      COS, TAN, ASIN, ACOS, ATAN2, ATAN,
      LOG, LOG10, EXP, SQRT,
      MathFunctionCount,
   };
   
   enum
   {
      TRANSPOSE = 0,
      DET, INV, NORM,
      MatrixOpsCount,
   };
   
   enum
   {
      DEG_TO_RAD = 0,
      RAD_TO_DEG,
      UnitConvCount,
   };
   
   static const std::string MATH_FUNC_LIST[MathFunctionCount];
   static const std::string MATRIX_OPS_LIST[MatrixOpsCount];
   static const std::string UNIT_CONV_LIST[UnitConvCount];
};


#endif //MathParser_hpp


