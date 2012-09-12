//$Id$
//------------------------------------------------------------------------------
//                                   MathParser
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS task
// 124.
//
// Author: Darrel J. Conway
// Created: 2006/03/16
// Modified: 2006/04/10 Linda Jun, NASA/GSFC
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

#include "gmatdefs.hpp"
#include "MathNode.hpp"
#include "MathException.hpp"

class GMAT_API MathParser
{
public:
   
   MathParser();
   MathParser(const MathParser &copy);
   MathParser& operator=(const MathParser &right);
   virtual ~MathParser();
   
   bool         IsEquation(const std::string &str, bool checkMinusSign);
   std::string  FindLowestOperator(const std::string &str, Integer &opIndex,
                                   Integer start = 0);
   MathNode*    Parse(const std::string &str);
   StringArray  GetGmatFunctionNames();
   
protected:
   
   MathNode*    ParseNode(const std::string &str);
   MathNode*    CreateNode(const std::string &type, const std::string &exp);
   StringArray  Decompose(const std::string &str);
   
private:
   
   std::string  originalEquation;
   std::string  theEquation;
   std::string  inverseOpStr;
   std::string  transposeOpStr;
   char         inverseOp;
   char         transposeOp;
   Integer      theGmatFuncCount;
   
   StringArray  ParseParenthesis(const std::string &str);
   StringArray  ParseAddSubtract(const std::string &str);
   StringArray  ParseMultDivide(const std::string &str);
   StringArray  ParseMatrixOps(const std::string &str);
   StringArray  ParsePower(const std::string &str);
   StringArray  ParseUnary(const std::string &str);
   StringArray  ParseMathFunctions(const std::string &str);
   StringArray  ParseUnitConversion(const std::string &str);
   
   bool         IsMathFunction(const std::string &str);
   bool         HasFunctionName(const std::string &str, const StringArray &fnList);
   bool         IsParenPartOfFunction(const std::string &str);
   bool         IsGmatFunction(const std::string &name);
   bool         IsValidOperator(const std::string &str);
   
   std::string  GetFunctionName(UnsignedInt functionType, const std::string &str,
                                std::string &leftStr);
   std::string::size_type
                FindSubtract(const std::string &str, std::string::size_type start);
   std::string::size_type
                FindMatchingParen(const std::string &str,
                                  std::string::size_type start);
   std::string::size_type
                FindOperatorIndex(std::string::size_type index1,
                                  std::string::size_type index2,
                                  std::string::size_type index3 = std::string::npos);
   std::string  FindOperatorFrom(const std::string &str,
                                 std::string::size_type start,
                                 std::string &left, std::string &right,
                                 std::string::size_type &opIndex);
   std::string  GetOperator(const IntegerMap::iterator &pos1,
                            const IntegerMap::iterator &pos2,
                            const IntegerMap::iterator &pos3,
                            const IntegerMap &opIndexMap,
                            Integer &opIndex);
   std::string  FindOperator(const std::string &str, Integer &opIndex,
                             bool isAfterCloseParen = false);
   std::string  GetOperatorName(const std::string &op, bool &opFound);
   void         BuildAllFunctionList();
   void         BuildGmatFunctionList(const std::string &str);
   void         BuildFunction(const std::string &str, const StringArray &fnList,
                              std::string &fnName, std::string &leftStr);
   void         FillItems(StringArray &items, const std::string &op,
                          const std::string &left, const std::string &right);
   void         WriteItems(const std::string &msg, StringArray &items, bool addEol = false);
   void         WriteNode(MathNode *node, UnsignedInt level);
   
   enum
   {
      MATH_FUNCTION,
      MATRIX_FUNCTION,
      MATRIX_OP,
      UNIT_CONVERSION,
      GMAT_FUNCTION,
   };
   
   StringArray  realFuncList;
   StringArray  matrixFuncList;
   StringArray  matrixOpList;
   StringArray  unitConvList;
   StringArray  gmatFuncList;
};


#endif //MathParser_hpp


