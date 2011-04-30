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
 * Class implementation for the MathParser.
 */
//------------------------------------------------------------------------------

#include "MathParser.hpp"
#include "MathFunction.hpp"
#include "FunctionRunner.hpp"
#include "StringUtil.hpp"       // for ParseParameter()
#include "FileManager.hpp"      // for GetGmatFunctionPath()
#include "MessageInterface.hpp"

#ifdef __UNIT_TEST__
#include "MathFactory.hpp"
#else
#include "Moderator.hpp"
#endif

//#define DEBUG_PARSE 1
//#define DEBUG_PARSE_EQUATION 1
//#define DEBUG_MATH_PARSER 2
//#define DEBUG_DECOMPOSE 1
//#define DEBUG_PARENTHESIS 1
//#define DEBUG_FIND_OPERATOR 1
//#define DEBUG_ADD_SUBTRACT 1
//#define DEBUG_MULT_DIVIDE 1
//#define DEBUG_MATRIX_OPS 1
//#define DEBUG_FUNCTION 2
//#define DEBUG_ATAN2
//#define DEBUG_MATH_PARSER_PARAM 1
//#define DEBUG_INVERSE_OP 1
//#define DEBUG_POWER 1
//#define DEBUG_UNARY 1
//#define DEBUG_CREATE_NODE 1
//#define DEBUG_GMAT_FUNCTION 1

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------

//------------------------------------------------------------------------------
//  MathParser(std::string typeStr, std::string nomme)
//------------------------------------------------------------------------------
/**
 * Constructs the MathParser object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <nomme>   Name for the object
 */
//------------------------------------------------------------------------------
MathParser::MathParser()
{
   theGmatFuncCount = 0;
   BuildAllFunctionList();
}


//------------------------------------------------------------------------------
//  MathParser(const MathParser &copy)
//------------------------------------------------------------------------------
/**
 * Constructs the MathParser object (copy constructor).
 * 
 * @param <copy> Object that is copied
 */
//------------------------------------------------------------------------------
MathParser::MathParser(const MathParser &copy)
{
   theGmatFuncCount = 0;
   BuildAllFunctionList();
}


//------------------------------------------------------------------------------
//  MathParser& operator=(const MathParser &right)
//------------------------------------------------------------------------------
/**
 * Sets one MathParser object to match another (assignment operator).
 * 
 * @param <right> The object that is copied.
 * 
 * @return this object
 */
//------------------------------------------------------------------------------
MathParser& MathParser::operator=(const MathParser &right)
{
   if (this == &right)
      return *this;
   
   theGmatFuncCount = 0;
   BuildAllFunctionList();
   return *this;
}


//------------------------------------------------------------------------------
//  ~MathParser(void)
//------------------------------------------------------------------------------
/**
 * Destroys the MathParser object (destructor).
 */
//------------------------------------------------------------------------------
MathParser::~MathParser()
{
}


//------------------------------------------------------------------------------
// bool IsEquation(const std::string &str, bool checkMinusSign)
//------------------------------------------------------------------------------
/*
 * Examines if given string is a math equation.
 * Call this method with RHS of assignement.
 *
 * @param str The string to be examined for an equation
 * @param checkMinusSign Check for leading minus sign.
 *        Set this flag to true if minus sign is a part of string and is not considered
 *        math unary operator
 * @return  true if string is an equation, means has math operators and/or functions
 *
 */
//------------------------------------------------------------------------------
bool MathParser::IsEquation(const std::string &str, bool checkMinusSign)
{
   theEquation = str;
   
   #if DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("=================================================================\n");
   MessageInterface::ShowMessage
      ("MathParser::IsEquation() str=%s\n", str.c_str());
   MessageInterface::ShowMessage
      ("=================================================================\n");
   #endif
   
   bool isEq = false;
   std::string left, right;
   Real rval;
   std::string::size_type opIndex;
   
   // Check if string is enclosed with quotes
   if (GmatStringUtil::IsEnclosedWith(str, "'"))
   {
      isEq = false;
   }
   // Check if it is just a number
   else if (GmatStringUtil::ToReal(str, &rval))
   {
      isEq = false;
   }
   else
   {
      // build GmatFunction list first
      BuildGmatFunctionList(str);
      
      if (GetFunctionName(MATH_FUNCTION, str, left) != "" ||
          GetFunctionName(MATRIX_FUNCTION, str, left) != "" ||
          GetFunctionName(UNIT_CONVERSION, str, left) != "" ||
          FindOperatorFrom(str, 0, left, right, opIndex) != "" ||
          GetFunctionName(GMAT_FUNCTION, str, left) != "")
      {
         isEq = true;

         if (checkMinusSign)
         {
            // Check for - sign used as string
            if (GmatStringUtil::NumberOfOccurrences(str, '-') == 1 &&
                GmatStringUtil::StartsWith(str, "-") &&
                GmatStringUtil::IsSingleItem(str))
            {
               isEq = false;
            }
         }
      }
      else
      {
         // Check ' for matrix transpose and ^(-1) for inverse
         if (str.find("'") != str.npos || str.find("^(-1)") != str.npos)
            isEq = true;
      }
   }
   
   #if DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("MathParser::IsEquation(%s) returning %u\n", str.c_str(), isEq);
   #endif
   
   return isEq;
}


//------------------------------------------------------------------------------
// std::string FindLowestOperator(const std::string &str, Integer &opIndex,
//                                Integer start = 0)
//------------------------------------------------------------------------------
/*
 * Finds lowest operator from the input string.
 * Single operators are +, -, *, /, ^, '.
 *
 * Precedence of operators is (highest to lowest)
 *    Parentheses ()
 *    matrix Transpose('), power (^),  matrix power(^)
 *    Unary plus (+), unary minus (-)
 *    Multiplication (*), right division (/), matrix multiplication (*), matrix right division (/)
 *    Addition (+), subtraction (-)
 *
 * @param  str  Input string
 * @param  opIndex  Index of operator
 * @param  start  Index to start
 *
 * @return  Single operator,
 *          "", if operator not found
 */
//------------------------------------------------------------------------------
std::string MathParser::FindLowestOperator(const std::string &str,
                                           Integer &opIndex, Integer start)
{
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("=================================================================\n");
   MessageInterface::ShowMessage
      ("FindLowestOperator() entered, str=%s, start=%u, length=%u\n", str.c_str(),
       start, str.size());
   MessageInterface::ShowMessage
      ("=================================================================\n");
   #endif
   
   Integer firstOpen = str.find_first_of("(", start);
   Integer start1 = start;
   Integer open1 = -1, close1 = -1;
   Integer length = str.size();
   Integer index = -1;
   bool isOuterParen;
   bool done = false;
   std::string opStr;
   IntegerMap opIndexMap;
   std::string substr;
   IntegerMap::iterator pos;
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage("   firstOpen=%u\n", firstOpen);
   #endif
   
   if (firstOpen == (Integer)str.npos)
      firstOpen = -1;
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage("   firstOpen=%u\n", firstOpen);
   #endif
   
   if (firstOpen > 0)
   {
      std::string::size_type inverseOp = str.find("^(-1)");
      // Check for ^(-1) which goes toghether as inverse operator
      if (inverseOp != str.npos)
      {
         if (str.substr(firstOpen-1, 5) == "^(-1)")
         {
            #if DEBUG_FIND_OPERATOR
            MessageInterface::ShowMessage("   found ^(-1)\n");
            #endif
            
            firstOpen = str.find_first_of("(", firstOpen + 3);
            if (firstOpen == (Integer)str.npos)
               firstOpen = inverseOp;
         }
      }
   }
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindLowestOperator() Find operator before first open parenthesis, "
       "firstOpen=%d\n", firstOpen);
   #endif
   
   //-----------------------------------------------------------------
   // find a lowest operator before first open paren
   //-----------------------------------------------------------------
   if (firstOpen > 0)
   {
      substr = str.substr(0, firstOpen);
      opStr = FindOperator(substr, index);
      if (opStr != "")
         opIndexMap[opStr] = index;
   }
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindLowestOperator() Find lowest operator before last close parenthesis");
   #endif
   //-----------------------------------------------------------------
   // find a lowest operator before last close paren
   //-----------------------------------------------------------------
   while (!done)
   {
      GmatStringUtil::FindMatchingParen(str, open1, close1, isOuterParen, start1);
      
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage("   open1=%u, close1=%u\n", open1, close1);
      #endif
      
      // find next open parenthesis '('
      start1 = str.find('(', close1);
      
      if (start1 == -1)
      {
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage
            ("   ===> There is no ( found after %d, so exiting while loop\n", close1);
         #endif
         break;
      }
      
      substr = str.substr(close1+1, start1-close1-1);
      
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage("   substr=%s\n", substr.c_str());
      #endif
      
      opStr = FindOperator(substr, index);
      
      if (opStr != "")
         opIndexMap[opStr] = close1 + index + 1;
   }
   
   
   //-----------------------------------------------------------------
   // find a lowest operator after last close paren
   //-----------------------------------------------------------------
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindLowestOperator() Find lowest operator after last close parenthesis");
   #endif
   if (close1 != length-1)
   {
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage("   after last close parenthesis\n");
      #endif
      
      substr = str.substr(close1+1);
      opStr = FindOperator(substr, index);
      
      if (opStr != "")
         opIndexMap[opStr] = close1 + index + 1;
   }
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage("   There are %d operators\n", opIndexMap.size());
   for (pos = opIndexMap.begin(); pos != opIndexMap.end(); ++pos)
      MessageInterface::ShowMessage
         ("      op=%s, index=%d\n", pos->first.c_str(), pos->second);
   #endif
   
   IntegerMap::iterator pos1;
   IntegerMap::iterator pos2;
   Integer index1 = -1;
   Integer index2 = -1;
   std::string lastOp;
   bool opFound = false;
   bool unaryMinusFound = false;
   
   // find + or - first
   pos1 = opIndexMap.find("+");
   pos2 = opIndexMap.find("-");
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("   +op index=%d, -op index=%d\n",
       pos1 == opIndexMap.end() ? -1 : pos1->second,
       pos2 == opIndexMap.end() ? -1 : pos2->second);
   #endif
   
   if (pos1 != opIndexMap.end() || pos2 != opIndexMap.end())
   {
      if (pos1 != opIndexMap.end())
         index1 = pos1->second;
      if (pos2 != opIndexMap.end())
         index2 = pos2->second;
      
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage("   index1=%d, index2=%d\n", index1, index2);
      #endif
      
      // Check for unary - operator
      if (index2 == 0)
      {
         // Check for function
         if (firstOpen > 0 && IsParenPartOfFunction(str.substr(1, firstOpen-1)))
            unaryMinusFound = false;
         else
            unaryMinusFound = true;
      }
      
      if (!unaryMinusFound)
      {
         opStr = GetOperator(pos1, pos2, opIndexMap, index);
         opFound = true;
      }
      
//       // Check for unary - operator
//       if (index2 == 0)
//       {
//          // Check for function
//          if (firstOpen > 0 && IsParenPartOfFunction(str.substr(1, firstOpen-1)))
//             unaryMinusFound = false;
//          else
//             unaryMinusFound = true;
//       }
//       else
//       {
//          opStr = GetOperator(pos1, pos2, opIndexMap, index);
//          opFound = true;
//       }
   }
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("   unaryMinusFound=%d, opFound=%d\n", unaryMinusFound, opFound);
   #endif
   
   if (!opFound)
   {
      // find * or /
      pos1 = opIndexMap.find("*");
      pos2 = opIndexMap.find("/");
      if (pos1 != opIndexMap.end() || pos2 != opIndexMap.end())
      {
         opStr = GetOperator(pos1, pos2, opIndexMap, index);
      }
      else
      {
         if (unaryMinusFound)
         {
            index = 0;
            opStr = "-";
         }
         else
         {
            // find ^ and not ^(-1) which is inverse of matrix
            // find ' which is transpose of matrix (LOJ: 2010.07.29)
            pos1 = opIndexMap.find("^");
            pos2 = opIndexMap.find("'");
            if (pos1 != opIndexMap.end() || pos2 != opIndexMap.end())
            {
               opStr = GetOperator(pos1, pos2, opIndexMap, index);
            }
         }
      }
   }
   
   opIndex = index;
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindLowestOperator() returning opStr=%s, opIndex=%d\n",
       opStr.c_str(), opIndex);
   #endif
   
   return opStr;
}


//------------------------------------------------------------------------------
// MathNode*  Parse(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Breaks apart the text representation of an equation and uses the compoment
 * pieces to construct the MathTree.
 *
 * @param  str  Input equation to be parsed
 * @return constructed MathTree pointer
 */
//------------------------------------------------------------------------------
MathNode* MathParser::Parse(const std::string &str)
{
   originalEquation = str;
   theEquation = str;
   
   #if DEBUG_PARSE
   MessageInterface::ShowMessage
      ("=================================================================\n");
   MessageInterface::ShowMessage
      ("MathParser::Parse() theEquation=%s\n", theEquation.c_str());
   MessageInterface::ShowMessage
      ("=================================================================\n");
   #endif
   
   // first remove all blank spaces and semicoln
   std::string newEq = GmatStringUtil::RemoveAll(theEquation, ' ');
   std::string::size_type index = newEq.find_first_of(';');
   if (index != newEq.npos)
      newEq.erase(index);
   
   // second remove extra parenthesis (This need more testing - so commented out)
   //newEq = GmatStringUtil::RemoveExtraParen(newEq);
   
   // check if parenthesis are balanced
   if (!GmatStringUtil::IsParenBalanced(newEq))
      throw MathException("Found unbalanced parenthesis");
   
   #if DEBUG_PARSE
   MessageInterface::ShowMessage
      ("MathParser::Parse() newEq=%s\n", newEq.c_str());
   #endif
   
   // build GmatFunction list first
   BuildGmatFunctionList(newEq);
   
   MathNode *topNode = ParseNode(newEq);
   
   #if DEBUG_PARSE
   WriteNode(topNode, 0);
   MessageInterface::ShowMessage
      ("MathParser::Parse() returning topNode=<%p><%s>\n", topNode,
       topNode->GetTypeName().c_str());
   MessageInterface::ShowMessage
      ("=================================================================\n");
   #endif
   
   return topNode;
}


//------------------------------------------------------------------------------
// StringArray GetGmatFunctionNames()
//------------------------------------------------------------------------------
StringArray MathParser::GetGmatFunctionNames()
{
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::GetGmatFunctionNames() returning %d names\n", theGmatFuncCount);
   for (Integer i=0; i<theGmatFuncCount; i++)
      MessageInterface::ShowMessage("   <%s>\n", gmatFuncList[i].c_str());
   #endif
   
   return gmatFuncList;
}


//------------------------------------------------------------------------------
// MathNode* ParseNode(const std::string &str)
//------------------------------------------------------------------------------
MathNode* MathParser::ParseNode(const std::string &str)
{
   #if DEBUG_CREATE_NODE
   MessageInterface::ShowMessage("MathParser::ParseNode() str='%s'\n", str.c_str());
   #endif
   
   StringArray items = Decompose(str);
   std::string op = items[0];
   std::string left = items[1];
   std::string right = items[2];
   
   #if DEBUG_CREATE_NODE
   WriteItems("MathParser::ParseNode() After Decompose()", items);
   #endif
   
   MathNode *mathNode;
   
   // If operator is empty, create MathElement, create MathFunction otherwise
   if (op == "")
   {
      #if DEBUG_CREATE_NODE
      MessageInterface::ShowMessage
         ("=====> Should create MathElement: '%s'\n", str.c_str());
      #endif
      
      // Remove extra parenthesis before creating a node (LOJ: 2010.07.29)
      std::string str1 = GmatStringUtil::RemoveExtraParen(str);
      
      #if DEBUG_CREATE_NODE
      MessageInterface::ShowMessage
         ("       Creating MathElement with '%s'\n", str1.c_str());
      #endif
      
      if (str1 == "")
         throw MathException("Missing input arguments");
      
      mathNode = CreateNode("MathElement", str1);
   }
   else
   {
      #if DEBUG_CREATE_NODE
      MessageInterface::ShowMessage
         ("=====> Should create MathNode: %s\n", op.c_str());
      #endif
      
      std::string operands = "( " + left + ", " + right + " )";
      if (right == "")
         operands = "( " + left + " )";
      
      mathNode = CreateNode(op, operands);
      
      MathNode *leftNode = NULL;
      MathNode *rightNode = NULL;
      
      // if node is FunctionRunner, just create left node as MathElement(loj: 2008.08.25)
      // input nodes are created when FunctionRunner is created
      if (mathNode->IsOfType("FunctionRunner"))
      {
         leftNode = CreateNode("MathElement", left);
         mathNode->SetChildren(leftNode, rightNode);
         
         leftNode->SetFunctionInputFlag(true);
      }
      else
      {
         #if DEBUG_CREATE_NODE
         MessageInterface::ShowMessage
            ("   left='%s', right='%s'\n", left.c_str(), right.c_str());
         #endif
         
         // check for empty argument for function
         if (left == "")
         {
            if (IsMathFunction(op))
               throw MathException(op + "() - Missing input arguments");
         }
         else
         {
            #if DEBUG_CREATE_NODE
            MessageInterface::ShowMessage
               ("===============> Parse left node: '%s'\n", left.c_str());
            #endif
            leftNode = ParseNode(left);
         }
         
         // check if two operands are needed
         if (right == "")
         {
            if (op == "Add" || op == "Subtract" || op == "Multiply" || op == "Divide")
               throw MathException(op + "() - Not enough input arguments");
         }
         else
         {
            #if DEBUG_CREATE_NODE
            MessageInterface::ShowMessage
               ("===============> Parse right node: '%s'\n", right.c_str());
            #endif
            rightNode = ParseNode(right);
         }
         
         mathNode->SetChildren(leftNode, rightNode);
      }
   }
   
   #if DEBUG_CREATE_NODE
   MessageInterface::ShowMessage
      ("MathParser::ParseNode() returning <%p><%s>\n",
       mathNode, mathNode->GetTypeName().c_str());
   #endif
   
   return mathNode;
}


//------------------------------------------------------------------------------
// MathNode* CreateNode(const std::string &type, const std::string &exp)
//------------------------------------------------------------------------------
/**
 * Creates MathNode through the Moderator.
 *
 * @return StringArray of elements
 */
//------------------------------------------------------------------------------
MathNode* MathParser::CreateNode(const std::string &type, const std::string &exp)
{
   #if DEBUG_CREATE_NODE
   MessageInterface::ShowMessage
      ("MathParser::CreateNode() type=%s, exp=%s\n", type.c_str(),
       exp.c_str());
   #endif
   
   // check if type is GmatFunction
   std::string actualType = type;
   std::string nodeName = exp;
   
   // If node is FunctionRunner, add function name to node name (loj: 2008.08.21)
   if (IsGmatFunction(type))
   {
      actualType = "FunctionRunner";
      nodeName = type + exp;
   }
   
   #if DEBUG_CREATE_NODE
   MessageInterface::ShowMessage("   actualType=%s\n", actualType.c_str());
   #endif
   
   MathNode *node = NULL;
   
   #ifdef __UNIT_TEST__
   static MathFactory mf;
   node = mf.CreateMathNode(actualType, nodeName);
   #else
   Moderator* theModerator = Moderator::Instance();
   node = theModerator->CreateMathNode(actualType, nodeName);
   #endif
   
   if (node == NULL)
      throw MathException("Cannot create MathNode of \"" + actualType + "\"");
   
   #ifdef DEBUG_MORE_MEMORY
   MessageInterface::ShowMessage
      ("MathParser::CreateNode() node <%p><%s> '%s' created\n", node, actualType.c_str());
   #endif
   
   if (actualType == "FunctionRunner")
   {
      FunctionRunner *fRunner = (FunctionRunner*)node;
      fRunner->SetFunctionName(type);
      
      // add function input arguments
      std::string exp1 = exp;
      exp1 = GmatStringUtil::RemoveOuterString(exp1, "(", ")");
      
      #if DEBUG_CREATE_NODE > 1
      MessageInterface::ShowMessage("   exp1='%s'\n", exp1.c_str());
      #endif
      
      StringArray inputs = GmatStringUtil::SeparateBy(exp1, ",", true);
      for (UnsignedInt i=0; i<inputs.size(); i++)
      {
         //================================================================
         #ifdef __ALLOW_MATH_EXP_NODE__
         //================================================================
         
         // Create all input nodes (loj: 2008.08.25)
         MathNode *node = ParseNode(inputs[i]);
         
         #if DEBUG_CREATE_NODE > 1
         MessageInterface::ShowMessage("   inputs[%d] = '%s'\n", i, inputs[i].c_str());
         MessageInterface::ShowMessage
            ("------------------------------------------ Create input nodes\n");
         #endif
         #if DEBUG_CREATE_NODE > 1
         WriteNode(node, 0);
         MessageInterface::ShowMessage
            ("-------------------------------------------------------------\n");
         #endif
         
         fRunner->AddInputNode(node);
         
         // If input is GmatFunction or math equation, set "" (loj: 2008.08.22)
         // Should we do this here? Just hold off for now
         if (IsGmatFunction(inputs[i]) || IsEquation(inputs[i], false))
         {
            Integer type1, row1, col1;
            node->GetOutputInfo(type1, row1, col1);
            if (type1 == Gmat::REAL_TYPE)
               fRunner->AddFunctionInput(inputs[i]);
            else
               fRunner->AddFunctionInput(inputs[i]);
            
            #if DEBUG_CREATE_NODE
            MessageInterface::ShowMessage
               ("   Setting \"\" to FunctionRunner, input has Function or Equation\n");
            #endif
         }
         else
         {
            fRunner->AddFunctionInput(inputs[i]);
         }
         
         //================================================================
         #else
         //================================================================
         
         // check if input is math expression
         if (IsGmatFunction(inputs[i]) || IsEquation(inputs[i], false))
            throw MathException
               ("*** WARNING *** Currently passing math expression to a "
                "function is not allowed in \"" + originalEquation + "\"");
         
         fRunner->AddFunctionInput(inputs[i]);
         
         //================================================================
         #endif
         //================================================================
         
      }
      
      // add function output arguments
      fRunner->AddFunctionOutput("");
      
      // set function inputs and outputs to FunctionManager through FunctionRunner
      fRunner->SetFunctionInputs();
      fRunner->SetFunctionOutputs();
   }
   
   #if DEBUG_CREATE_NODE
   MessageInterface::ShowMessage
      ("MathParser::CreateNode() '%s' returning node <%p><%s>\n", exp.c_str(), node,
       node->GetTypeName().c_str());
   #endif
   
   return node;
}


//------------------------------------------------------------------------------
// StringArray Decompose(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Breaks string into operator, left and right elements.
 *
 * The order of parsing is as follows:
 *    ParseParenthesis()
 *    ParseAddSubtract()
 *    ParseMultDivide()
 *    ParsePower()
 *    ParseUnary()
 *    ParseMathFunctions()
 *    ParseMatrixOps()
 *    ParseUnitConversion()
 *
 * @return StringArray of elements
 */
//------------------------------------------------------------------------------
StringArray MathParser::Decompose(const std::string &str)
{
   #if DEBUG_DECOMPOSE
   MessageInterface::ShowMessage
      ("MathParser::Decompose() entered, str=%s\n", str.c_str());
   #endif
   
   StringArray items = ParseParenthesis(str);
   
   #if DEBUG_DECOMPOSE
   WriteItems("MathParser::Decompose() after ParseParenthesis() ", items);
   #endif
   
   // if no operator found and left is not empty, decompose again
   if (items[0] == "" && items[1] != "")
      items = Decompose(items[1]);
   
   std::string str1;
   str1 = str;
   
   if (items[0] == "" && str[0] == '(' && str[str.size()-1] == ')')
   {
      if (GmatStringUtil::IsOuterParen(str))
      {
         str1 = str.substr(1, str.size()-2);
         #if DEBUG_DECOMPOSE
         MessageInterface::ShowMessage
            ("MathParser::Decompose() Found outer parenthesis. str1=%s\n", str1.c_str());
         #endif
      }
   }
   
   if (items[0] == "function")
      items[0] = "";
   
   if (items[0] == "")
   {
      items = ParseAddSubtract(str1);
      
      if (items[0] == "number")
      {
         #if DEBUG_DECOMPOSE
         WriteItems("MathParser::Decompose(): It is a number, returning ", items);
         #endif
         items[0] = "";
         return items;
      }
      
      if (items[0] == "")
      {
         items = ParseMultDivide(str1);
         
         if (items[0] == "")
         {
            items = ParsePower(str1);
            
            if (items[0] == "")
            {
               items = ParseUnary(str1);
                        
               if (items[0] == "")
               {
                  items = ParseMathFunctions(str1);
               
                  if (items[0] == "")
                  {
                     items = ParseMatrixOps(str1);
                  
                     if (items[0] == "")
                     {
                        items = ParseUnitConversion(str1);
                     
                     }
                  }
               }
            }
         }
      }
   }
   
   #if DEBUG_DECOMPOSE
   WriteItems("MathParser::Decompose(): returning ", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseParenthesis(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Returns string array of operator, left and right
 */
//------------------------------------------------------------------------------
StringArray MathParser::ParseParenthesis(const std::string &str)
{
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::ParseParenthesis() entered, str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";
   std::string left = "";
   std::string right = "";
   std::string::size_type opIndex;
   
   //-----------------------------------------------------------------
   // if no opening parenthesis '(' found, just return
   //-----------------------------------------------------------------
   std::string::size_type index1 = str.find('(');
   
   if (index1 == str.npos)
   {
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("MathParser::ParseParenthesis(): open parenthesis not found."
                 " returning ", items);
      #endif
      
      return items;
   }
   
   //-----------------------------------------------------------------
   // if lowest operator is + or - and not negate, just return
   //-----------------------------------------------------------------
   Integer index;
   std::string opStr1 = FindLowestOperator(str, index);
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage("   lowestOperator found =[%s]\n", opStr1.c_str());
   #endif
   if ((opStr1 == "+" || opStr1 == "-") && index != 0)
   {
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("MathParser::ParseParenthesis(): lowest + or - found."
                 " returning ", items);
      #endif
      
      return items;      
   }
   
   //-----------------------------------------------------------------
   // if lowest operator is * or /, just return with operator
   //-----------------------------------------------------------------
   if (opStr1 == "*" || opStr1 == "/")
   {      
      bool opFound1;      
      op = GetOperatorName(opStr1, opFound1);
      
      left = str.substr(0, index);
      right = str.substr(index+1, str.npos);
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("MathParser::ParseParenthesis(): lowest * or / found."
                 " returning ", items);
      #endif
      
      return items;
   }
   
   //-----------------------------------------------------------------
   // if lowest operator is ^, just return with operator
   //-----------------------------------------------------------------
   if (opStr1 == "^")
   {      
      bool opFound1;      
      op = GetOperatorName(opStr1, opFound1);
      
      left = str.substr(0, index);
      right = str.substr(index+1, str.npos);
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("MathParser::ParseParenthesis(): lowest ^ found."
                 " returning ", items);
      #endif
      
      return items;
   }
   
   //-----------------------------------------------------------------
   // if lowest operator is ', just return with operator
   //-----------------------------------------------------------------
   if (opStr1 == "'")
   {      
      bool opFound1;      
      op = GetOperatorName(opStr1, opFound1);
      
      left = str.substr(0, index);
      right = str.substr(index+1, str.npos);
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("MathParser::ParseParenthesis(): lowest ' found."
                 " returning ", items);
      #endif
      
      return items;
   }
   
   //-----------------------------------------------------------------
   // if ( is part of fuction, just return first parenthesis
   //-----------------------------------------------------------------
   std::string strBeforeParen = str.substr(0, index1);
   
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("   ParseParenthesis() strBeforeParen=%s\n", strBeforeParen.c_str());
   #endif
   
   if (IsParenPartOfFunction(strBeforeParen))
   {
      // find matching closing parenthesis
      UnsignedInt index2 = FindMatchingParen(str, index1);
      
      #if DEBUG_PARENTHESIS
      MessageInterface::ShowMessage
         ("   ParseParenthesis() Parenthesis is part of function. str=%s, size=%u, "
          "index1=%u, index2=%u\n", str.c_str(), str.size(), index1, index2);
      #endif
      
      // if last char is ')'
      if (index2 == str.size()-1)
      {
         #if DEBUG_PARENTHESIS
         MessageInterface::ShowMessage
            ("   ParseParenthesis() last char is ) so find function name\n");
         #endif
         // find math function
         op = GetFunctionName(GMAT_FUNCTION, str, left);
         if (op == "")
            op = GetFunctionName(MATH_FUNCTION, str, left);
         if (op == "")
            op = GetFunctionName(MATRIX_FUNCTION, str, left);
         if (op == "")
            op = GetFunctionName(UNIT_CONVERSION, str, left);
      }
      
      #if DEBUG_PARENTHESIS
      MessageInterface::ShowMessage("   ParseParenthesis() op='%s'\n", op.c_str());
      #endif
      
      // See if there is an operator before this function
      std::string op1, left1, right1;
      op1 = FindOperatorFrom(str, 0, left1, right1, opIndex);
      if (op1 != "" && opIndex != str.npos)
      {
         if (opIndex < index1)
         {
            // return blank for next parse
            #if DEBUG_PARENTHESIS
            MessageInterface::ShowMessage("found operator before function\n");
            #endif
            
            FillItems(items, "", "", "");
            return items;
         }
      }
      
      // Handle special atan2(y,x) function
      if (op == "atan2")
      {
         StringArray parts = GmatStringUtil::SeparateByComma(str);

         #ifdef DEBUG_ATAN2
         for (UnsignedInt i = 0; i < parts.size(); i++)
            MessageInterface::ShowMessage("..... (1) %d '%s'\n", i, parts[i].c_str());
         #endif
         
         bool parsingFailed = true;
         
         if (parts.size() == 1)
         {
            std::string str1 = str.substr(index1+1, index2-index1-1);
            parts = GmatStringUtil::SeparateByComma(str1);

            #ifdef DEBUG_ATAN2
            for (UnsignedInt i = 0; i < parts.size(); i++)
               MessageInterface::ShowMessage("..... (2) %d '%s'\n", i, parts[i].c_str());
            #endif
            
            if (parts.size() == 2)
            {
               left = parts[0];
               right = parts[1];
               if (left != "" && right != "")
                  parsingFailed = false;
            }
         }
         
         if (parsingFailed)
            throw MathException("Atan2() - Missing or invalid input arguments");
      }
      else
      {
         left = str.substr(index1+1, index2-index1-1);
      }
      
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("MathParser::ParseParenthesis() - parenthesis is part "
                 "of function. returning ", items);
      #endif
      
      return items;
   }
   
   
   //-----------------------------------------------------------------
   // If it is ^(-1), handle it later in DecomposeMatrixOps()
   //-----------------------------------------------------------------
   if (str.find("^(-1)") != str.npos)
   {
      #if DEBUG_INVERSE_OP
      MessageInterface::ShowMessage
         ("MathParser::ParseParenthesis() found ^(-1) str=%s\n", str.c_str());
      #endif
      
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("MathParser::ParseParenthesis() found ^(-1)returning ", items);
      #endif
      
      return items;
   }
   
   
   //-----------------------------------------------------------------
   // if enclosed with parenthesis
   //-----------------------------------------------------------------
   if (GmatStringUtil::IsEnclosedWithExtraParen(str, false))
   {   
      left = str.substr(1, str.size()-2);
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("MathParser::ParseParenthesis() complete parenthesis found."
                 " returning ", items);
      #endif
      
      return items;
   }
   
   
   Integer index2;
   //-----------------------------------------------------------------
   // find the lowest operator
   //-----------------------------------------------------------------
   std::string opStr = FindLowestOperator(str, index2);
   if (opStr != "")
   {
      bool opFound;      
      op = GetOperatorName(opStr, opFound);
      
      if (opFound)
      {
         left = str.substr(0, index2);
         right = str.substr(index2+1, str.npos);
         
         if (op == "Subtract" && left == "")
         {
            op = "Negate";
            left = right;
            right = "";
         }
         
         FillItems(items, op, left, right);
         
         #if DEBUG_PARENTHESIS
         WriteItems("MathParser::ParseParenthesis() found lowest operator "
                    "returning ", items);
         #endif
         return items;
      }
   }
   else
   {
      #if DEBUG_PARENTHESIS
      MessageInterface::ShowMessage
         ("   ParseParenthesis() No operator found\n");
      #endif
      
      //--------------------------------------------------------------
      // to be continued - will continue later (loj: 2008.09.03)
      //--------------------------------------------------------------
//       // Check if name before first parenthesis is GmatFunction
//       if (IsGmatFunction(strBeforeParen))
//       {
//          FillItems(items, strBeforeParen, str.substr(index1+1), "");
         
//          #if DEBUG_PARENTHESIS
//          WriteItems("MathParser::ParseParenthesis() found lowest operator "
//                     "returning ", items);
//          #endif
//          return items;
//       }
      //--------------------------------------------------------------
   }
   
   
   FillItems(items, op, left, right);
   
   #if DEBUG_PARENTHESIS
   WriteItems("MathParser::ParseParenthesis() returning ", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// std::string FindOperatorFrom(const std::string &str, std::string::size_type start,
//                              std::string &left, std::string &right,
//                              std::string::size_type &opIndex)
//------------------------------------------------------------------------------
std::string MathParser::FindOperatorFrom(const std::string &str, std::string::size_type start,
                                         std::string &left, std::string &right,
                                         std::string::size_type &opIndex)
{
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("MathParser::FindOperatorFrom() entered, str=%s, start=%u\n", str.c_str(), start);
   #endif
   
   StringArray items;
   std::string op;
   std::string::size_type index;
   std::string::size_type index1 = str.find("+", start);
   std::string::size_type index2 = str.find("-", start);
   
   if (index1 == str.npos && index2 == str.npos)
   {
      index1 = str.find("*", start);
      index2 = str.find("/", start);
      
      if (index1 == str.npos && index2 == str.npos)
         index1 = str.find("^", start);
      
      if (index1 != str.npos)
      {
         #if DEBUG_INVERSE_OP
         MessageInterface::ShowMessage
            ("FindOperatorFrom() found ^ str=%s, index1=%u\n",
             str.c_str(), index1);
         #endif
         
         // try for ^(-1) for inverse
         if (str.substr(index1, 5) == "^(-1)")
            return ""; // handle it later in DecomposeMatrixOps()
      }
   }
   
   index = index1 == str.npos ? index2 : index1;
   
   if (index != str.npos)
   {
      bool opFound;
      op = str.substr(index, 1);
      op = GetOperatorName(op, opFound);
      left = str.substr(0, index);
      right = str.substr(index+1, str.size()-index);
   }
   
   opIndex = index;
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("MathParser::FindOperatorFrom() returning op=%s, left=%s, right=%s, opIndex=%u\n",
       op.c_str(), left.c_str(), right.c_str(), opIndex);
   #endif
   
   return op;
   
}


//------------------------------------------------------------------------------
// std::string GetOperatorName(const std::string &op, bool &opFound)
//------------------------------------------------------------------------------
std::string MathParser::GetOperatorName(const std::string &op, bool &opFound)
{
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::GetOperatorName() op=%s\n", op.c_str());
   #endif
   
   std::string opName = "<" + op + "> :Unknown Operator";
   opFound = true;
   
   if (op == "+")
      opName = "Add";
   else if (op == "-")
      opName = "Subtract";
   else if (op == "*")
      opName = "Multiply";
   else if (op == "/")
      opName = "Divide";
   else if (op == "^")
      opName = "Power";
   else if (op == "'")
      opName = "Transpose";
   else
      opFound = false;
   
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::GetOperatorName() opFound=%u, opName=%s\n", opFound,
       opName.c_str());
   #endif
   
   return opName;
}


//------------------------------------------------------------------------------
// std::string FindOperator(const std::string &str, Integer &opIndex)
//------------------------------------------------------------------------------
/*
 * Finds the right most lowest operator from the input string.
 *
 * Precedence of operators is (lowest to highest)
 *    +, -
 *    *, /
 *    unary -
 *    ', ^
 *    ()
 *
 * Single operators are +, -, *, /, ^, '
 * Double operators are ++, --, +-, -+, *-, /-, ^+, ^-
 *
 * 
 * @param  str  Input string
 * @param  opIndex  Index of operator
 *
 * @return  Single operator,
 *          First operator, if double operator found
 *          "", if operator not found
 */
//------------------------------------------------------------------------------
std::string MathParser::FindOperator(const std::string &str, Integer &opIndex)
{
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("MathParser::FindOperator() entered, str=[ %s ]\n", str.c_str());
   #endif
   
   std::string str1 = str;
   
   // Replace scientific notation e- E- e+ E+
   str1 = GmatStringUtil::ReplaceNumber(str1, "e-", "e#");
   str1 = GmatStringUtil::ReplaceNumber(str1, "e+", "e#");
   str1 = GmatStringUtil::ReplaceNumber(str1, "E-", "e#");
   str1 = GmatStringUtil::ReplaceNumber(str1, "E+", "e#");
   
   StringArray items;
   std::string op;
   std::string::size_type index;
   std::string::size_type index1 = str1.find_last_of("+");
   std::string::size_type index2 = str1.find_last_of("-");
   std::string::size_type index3 = str1.find_first_of("-");
   bool unaryMinusFound = false;
   bool checkNext = true;
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindOperator() for +,- index1=%u, index2=%u, index3=%u\n", index1, index2, index3);
   #endif
   
   if (index1 != str1.npos || index2 != str1.npos)
      checkNext = false;
   
   // Check for unary - operator
   if (index3 != str1.npos)
   {
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage("   Found unary minus operator\n");
      #endif
      unaryMinusFound = true;
      if (index1 != str1.npos || index2 != str1.npos)
      {
         std::string::size_type index4;
         if (index1 != str1.npos && index2 != str1.npos)
            index4 = index1 > index2 ? index1 : index2;
         else
            index4 = index1 == str1.npos ? index2 : index1;
         
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage("   index4=%u\n", index4);
         #endif
         
         if ((index4 > 0) &&
             (str[index4-1] == '*' || str[index4-1] == '/'))
            checkNext = true;
      }
   }
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage("FindOperator() %s *,/\n", checkNext ? "check" : "skip");
   #endif
   
   if (checkNext)
   {
      index1 = str1.find_last_of("*");
      index2 = str1.find_last_of("/");
      
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage
         ("FindOperator() for *,/ index1=%u, index2=%u\n", index1, index2);
      #endif
      
      if (index1 == str1.npos && index2 == str1.npos)
      {
         if (unaryMinusFound)
         {
            op = "-";
            opIndex = 0;
            
            #if DEBUG_FIND_OPERATOR
            MessageInterface::ShowMessage
               ("FindOperator() returning op=%s, opIndex=%u\n", op.c_str(), opIndex);
            #endif
            
            return op;            
         }
         
         index1 = str1.find_last_of("^");
         
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage
            ("FindOperator() for ^ index1=%u\n", index1);
         #endif
         
         if (index1 != str1.npos)
         {
            #if DEBUG_FIND_OPERATOR
            MessageInterface::ShowMessage
               ("FindOperator() found ^ str=%s, index1=%u\n",
                str1.c_str(), index1);
            #endif
            
            // try for ^(-1) for inverse
            if (str1.substr(index1, 5) == "^(-1)")
            {
               #if DEBUG_FIND_OPERATOR
               MessageInterface::ShowMessage
                  ("MathParser::FindOperator() found ^(-1) so returning \"\"");
               #endif
               return "";
            }
         }
         else
         {
            // Find ' for transpose
            index1 = str1.find_last_of("'");
            
            if (index1 != str1.npos)
            {
               #if DEBUG_FIND_OPERATOR
               MessageInterface::ShowMessage
                  ("FindOperator() found ' str=%s, index1=%u\n", str1.c_str(), index1);
               #endif
            }
            else
            {
               #if DEBUG_FIND_OPERATOR
               MessageInterface::ShowMessage
                  ("MathParser::FindOperator() returning \"\"");
               #endif
               return "";
            }
         }
      }
   }
   
   // if both operators found, assign to greater position
   if (index1 != str1.npos && index2 != str1.npos)
      index = index1 > index2 ? index1 : index2;
   else
      index = index1 == str1.npos ? index2 : index1;
   
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindOperator() Checking for double operator, index=%u, index1=%u, "
       "index2=%u\n", index, index1, index2);
   #endif
   
   // check for double operator such as *-, /-
   
   if (index == 0)
   {
      op = str1.substr(index, 1);
      opIndex = index;
   }
   else if (str[index-1] == '+' || str[index-1] == '-' ||
            str[index-1] == '*' || str[index-1] == '/' ||
            str[index-1] == '^')
   {
      op = str1.substr(index-1, 1);
      opIndex = index-1;
   }
   else if (index != str1.npos)
   {
      op = str1.substr(index, 1);
      opIndex = index;
   }
   else
   {
      op = "";
      opIndex = -1;
   }
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindOperator() returning op=%s, opIndex=%u\n", op.c_str(), opIndex);
   #endif
   
   return op;
   
}


//------------------------------------------------------------------------------
// std::string GetOperator(const IntegerMap::iterator &pos1,
//                         const IntegerMap::iterator &pos2,
//                         const IntegerMap &opIndexMap,
//                         Integer &opIndex)
//------------------------------------------------------------------------------
std::string MathParser::GetOperator(const IntegerMap::iterator &pos1,
                                    const IntegerMap::iterator &pos2,
                                    const IntegerMap &opIndexMap,
                                    Integer &opIndex)
{
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage("GetOperator() entered\n");
   if (pos1 != opIndexMap.end())
      MessageInterface::ShowMessage
         ("   pos1=<%s><%d>\n", pos1->first.c_str(), pos1->second);
   else
      MessageInterface::ShowMessage("   pos1 is NULL\n");
   
   if (pos2 != opIndexMap.end())
      MessageInterface::ShowMessage
         ("   pos2=<%s><%d>\n", pos2->first.c_str(), pos2->second);
   else
      MessageInterface::ShowMessage("   pos2 is NULL\n");
   #endif
   
   std::string opStr;
   Integer index = -1;
   
   if (pos1 != opIndexMap.end() || pos2 != opIndexMap.end())
   {
      if (pos2 == opIndexMap.end())
      {
         opStr = pos1->first;
         index = pos1->second;
      }
      else if (pos1 == opIndexMap.end())
      {
         opStr = pos2->first;
         index = pos2->second;
      }
      else
      {
         // if operators on the same level of precedence are evaluated
         // from left to right
         if (pos1->second > pos2->second)
         {
            opStr = pos1->first;
            index = pos1->second;
         }
         else
         {
            opStr = pos2->first;
            index = pos2->second;
         }
      }
   }
   
   opIndex = index;
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("GetOperator() return opStr='%s', opIndex=%d\n", opStr.c_str(), opIndex);
   #endif
   
   return opStr;
}


//------------------------------------------------------------------------------
// UnsignedInt FindSubtract(const std::string &str, std::string::size_type start)
//------------------------------------------------------------------------------
std::string::size_type MathParser::FindSubtract(const std::string &str,
                                                std::string::size_type start)
{
   #if DEBUG_INVERSE_OP
   MessageInterface::ShowMessage
      ("MathParser::FindSubtract() str=%s, start=%u\n", str.c_str(), start);
   #endif
   
   std::string::size_type index2 = str.find('-', start);
   std::string::size_type index3 = str.find("^(-1)", start);
   
   #if DEBUG_INVERSE_OP
   MessageInterface::ShowMessage
      ("MathParser::FindSubtract() index2=%u, index3=%u\n", index2, index3);
   #endif
   
   // found no ^(-1)
   if (index2 != str.npos && index3 == str.npos)
   {
      #if DEBUG_INVERSE_OP
      MessageInterface::ShowMessage
         ("MathParser::FindSubtract() found no ^(-1) returning index2=%u\n",
          index2);
      #endif
      return index2;
   }
   
   // found - inside of ^(-1)
   if (index2 > index3 && index3 + 5 == str.size())
   {
      #if DEBUG_INVERSE_OP
      MessageInterface::ShowMessage
         ("MathParser::FindSubtract() found - inside of ^(-1) "
          "returning str.size()=%u\n", str.size());
      #endif
      return str.size();
   }

   // found - and ^(-1)
   if (index2 < index3)
   {
      #if DEBUG_INVERSE_OP
      MessageInterface::ShowMessage
         ("MathParser::FindSubtract() found - and ^(-1) "
          "returning index2=%u\n", index2);
      #endif
      return index2;
   }

   
   if (index3 != str.npos)
   {
      // If it has only "^(-1)", handle it later in DecomposeMatrixOps()
      if (index3+5 == str.size())
      {
         #if DEBUG_INVERSE_OP
         MessageInterface::ShowMessage
            ("MathParser::FindSubtract() found ^(-1) str=%s\n",
             str.c_str());
         MessageInterface::ShowMessage
            ("MathParser::FindSubtract() returning str.size()=%u\n", str.size());
         #endif
         
         return str.size();
      }
      else
      {
         std::string::size_type index = FindSubtract(str, index3+5);
         
         #if DEBUG_INVERSE_OP
         MessageInterface::ShowMessage
            ("MathParser::FindSubtract() index=%u, after FindSubtract()\n",
             index);
         #endif
         
         // if found first - not in ^(-1)
         if (index != str.npos && index != str.size())
            return index;
      }
   }
   
   #if DEBUG_INVERSE_OP
   MessageInterface::ShowMessage
      ("MathParser::FindSubtract() returning str.size()=%u\n", str.size());
   #endif
   
   return str.size();
}


//------------------------------------------------------------------------------
// StringArray ParseAddSubtract(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseAddSubtract(const std::string &str)
{
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("MathParser::ParseAddSubtract() str=%s, size=%d\n", str.c_str(), str.size());
   #endif
   
   //-----------------------------------------------------------------
   // Operators of equal precedence evaluate from left to right.
   // 10 - 50 + 1 + 30 - 25 should produce
   // (((10 - 50) + 1) + 30) - 25
   //-----------------------------------------------------------------
   
   StringArray items;
   std::string op = "";
   std::string left;
   std::string right;

   // find last - or +
   std::string::size_type index1 = str.find_last_of('+');
   std::string::size_type index2 = str.find_last_of('-');
   
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("ParseAddSubtract() index1=%u, index2=%u\n", index1, index2);
   #endif
   
   //-------------------------------------------------------
   // no + or - found
   //-------------------------------------------------------
   if (index1 == str.npos && index2 == str.npos)
   {
      FillItems(items, "", "", "");
      #if DEBUG_ADD_SUBTRACT
      WriteItems("ParseAddSubtract(): '+' or '-' not found", items);
      #endif
      return items;
   }
   
   //-------------------------------------------------------
   // find lowest operator, expecting + or -
   //-------------------------------------------------------
   Integer index;
   std::string opStr = FindLowestOperator(str, index);
   
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("after FindLowestOperator() opStr=%s, index=%d\n",
       opStr.c_str(), index);
   #endif
   
   //-------------------------------------------------------
   // lowest operator is not + or -
   //-------------------------------------------------------
   if (opStr != "+" && opStr != "-")
   {
      // Check for scientific number
      if (GmatStringUtil::IsNumber(str))
         FillItems(items, "number", str, "");
      else
         FillItems(items, "", "", "");
      
      #if DEBUG_ADD_SUBTRACT
      WriteItems("ParseAddSubtract(): lowest op is not '+' or '-'", items);
      #endif
      return items;
   }
   
   //-------------------------------------------------------
   // If unary operator, handle it later in ParseUnary()
   //-------------------------------------------------------
   if (index2 == 0 && index1 == str.npos)
   {
      #if DEBUG_INVERSE_OP
      MessageInterface::ShowMessage
         ("MathParser::ParseAddSubtract() found unary str=%s\n", str.c_str());
      #endif
      
      FillItems(items, op, left, right);
      return items;
   }
   
   
   UnsignedInt indexLeft = index;
   UnsignedInt indexRight = index + 1;
   
   bool opFound;      
   op = GetOperatorName(opStr, opFound);
   
   // if double operator +- or -+ found
   if (str[index+1] == '+' || str[index+1] == '-')
   {
      #if DEBUG_ADD_SUBTRACT
      MessageInterface::ShowMessage
         ("ParseAddSubtract() double operator found, %s\n",
          str.substr(index, 2).c_str());
      #endif
      
      if (opStr == "+" && str[index+1] == '+')
         op = "Add";
      else if (opStr == "+" && str[index+1] == '-')
         op = "Subtract";
      else if (opStr == "-" && str[index+1] == '-')
         op = "Add";
      else if (opStr == "-" && str[index+1] == '+')
         op = "Subtract";
      
      indexRight = indexRight + 1;
   }
   
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("ParseAddSubtract() indexLeft=%u, indexRight=%u, op=%s\n",
       indexLeft, indexRight, op.c_str());
   #endif
   
   left = str.substr(0, indexLeft);
   right = str.substr(indexRight, str.npos);
   
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("ParseAddSubtract() op=%s, left=%s, right=%s\n", op.c_str(),
       left.c_str(), right.c_str());
   #endif
   
   if (right == "")
      throw MathException("Need right side of \"" + op + "\"");
   
   FillItems(items, op, left, right);
   
   #if DEBUG_ADD_SUBTRACT
   WriteItems("After ParseAddSubtract()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseMultDivide(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseMultDivide(const std::string &str)
{
   #if DEBUG_MULT_DIVIDE
   MessageInterface::ShowMessage
      ("MathParser::ParseMultDivide() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";

   //-----------------------------------------------------------------
   // find last * or /
   // because we want to evaluate * or / in the order it appears
   // a * b / c * d
   //-----------------------------------------------------------------
   
   std::string::size_type index1 = str.find_last_of('*');
   std::string::size_type index2 = str.find_last_of('/');
   
   if (index1 == str.npos && index2 == str.npos)
   {
      FillItems(items, "", "", "");
      #if DEBUG_MULT_DIVIDE
      WriteItems("ParseMultDivide(): No * or / found", items);
      #endif
      return items;
   }
   
   //-------------------------------------------------------
   // find lowest operator, expecting * or /
   //-------------------------------------------------------
   Integer index;
   std::string opStr = FindLowestOperator(str, index);
   
   #if DEBUG_MULT_DIVIDE
   MessageInterface::ShowMessage
      ("after FindLowestOperator() opStr=%s, index=%d\n",
       opStr.c_str(), index);
   #endif
   
   //-------------------------------------------------------
   // lowest operator is not / or *
   //-------------------------------------------------------
   if (opStr != "/" && opStr != "*")
   {
      FillItems(items, "", "", "");
      #if DEBUG_MULT_DIVIDE
      WriteItems("ParseMultDivide(): lowest op is not '/' or '*'", items);
      #endif
      return items;
   }
   
   bool opFound;
   op = GetOperatorName(opStr, opFound);
   std::string left = str.substr(0, index);
   std::string right = str.substr(index+1, str.npos);
   
   //-------------------------------------------------------
   // find double operator *+, *-, /+, /-
   //-------------------------------------------------------
   if (str[index+1] == '+' || str[index+1] == '-')
   {
      std::string right = str.substr(index+2, str.npos);
      
      #if DEBUG_MULT_DIVIDE
      MessageInterface::ShowMessage
         ("combined operator found, %s\n", str.substr(index, 2).c_str());
      #endif
   }
   
   if (left == "")
      throw MathException("Need left side of the operator \"" + op + "\"");
   
   if (right == "")
      throw MathException("Need right side of the operator \"" + op + "\"");
   
   FillItems(items, op, left, right);

   #if DEBUG_MULT_DIVIDE
   WriteItems("After ParseMultDivide()", items);
   #endif
   
   return items;
   
}


//------------------------------------------------------------------------------
// StringArray ParsePower(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParsePower(const std::string &str)
{
   #if DEBUG_POWER
   MessageInterface::ShowMessage
      ("MathParser::ParsePower() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";
   
   // We should find last ^ insted of first ^ to fix bug 2176 (LOJ: 2010.10.29)
   std::string::size_type index1 = str.find_last_of('^');
   
   if (index1 == str.npos)
   {
      FillItems(items, "", "", "");
      return items;
   }
   
   // If it is ^(-1), handle it later in DecomposeMatrixOps()
   if (str.find("^(-1)") != str.npos)
   {
      #if DEBUG_INVERSE_OP
      MessageInterface::ShowMessage
         ("MathParser::ParsePower() found ^(-1) str=%s\n", str.c_str());
      #endif
      
      FillItems(items, "", "", "");
      return items;
   }
   
   
   // If first unary operator found, handle it later in ParseUnary()
   if (str.find("-") != str.npos)
   {
      UnsignedInt index3 = str.find("-");
      if (index3 == 0)
      {
         #if DEBUG_UNARY
         MessageInterface::ShowMessage
            ("MathParser::ParsePower() found - unary str=%s\n", str.c_str());
         #endif
      
         FillItems(items, "", "", "");
         return items;
      }
   }

   
   std::string::size_type index = str.npos;
   if (index1 != str.npos)
   {
      op = "Power";
      index = index1;
   }
   
   std::string left = str.substr(0, index);
   std::string right = str.substr(index+1, str.npos);

   if (left == "")
      throw MathException("Need left side of the operator \"" + op + "\"");
   
   if (right == "")
      throw MathException("Need right side of the operator \"" + op + "\"");
   
   FillItems(items, op, left, right);
   
   #if DEBUG_MATH_PARSER > 1
   WriteItems("After ParsePower()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseUnary(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseUnary(const std::string &str)
{
   #if DEBUG_UNARY
   MessageInterface::ShowMessage
      ("MathParser::ParseUnary() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";
   
   // If it is ^(-1), handle it later in DecomposeMatrixOps()
   if (str.find("^(-1)") != str.npos)
   {
      #if DEBUG_INVERSE_OP
      MessageInterface::ShowMessage
         ("MathParser::ParseUnary() found ^(-1) str=%s\n", str.c_str());
      #endif
      
      FillItems(items, "", "", "");
      return items;
   }
   
   // find  - or -
   std::string::size_type index1 = str.find('-');
   std::string::size_type index2 = str.find('+');
   
   if (index1 == str.npos && index2 == str.npos)
   {
      FillItems(items, "", "", "");
      return items;
   }
   
   if (index1 != str.npos)
      op = "Negate";
   else
      op = "None";
   
   std::string left;
   
   // If power ^ found
   if (str.find("^") != str.npos)
   {
      #if DEBUG_POWER
      MessageInterface::ShowMessage
         ("MathParser::ParseUnary() found ^ str=%s\n", str.c_str());
      #endif
      
      left = str.substr(index1+1);
   }
   else
   {
      left = str.substr(index1+1, str.npos);
   }
   
   FillItems(items, op, left, "");
   
   #if DEBUG_UNARY
   WriteItems("After ParseUnary()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseMathFunctions(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseMathFunctions(const std::string &str)
{
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::ParseMathFunctions() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string left;
   
   // find first math function
   std::string fnName = GetFunctionName(MATH_FUNCTION, str, left);
   
   if (fnName == "")
   {
      // let's try GmatFunction name
      fnName = GetFunctionName(GMAT_FUNCTION, str, left);
      
      if (fnName == "")
      {
         FillItems(items, "", "", "");
         return items;
      }
   }
   
   if (left == "")
      throw MathException("Need an argument of the function \"" + fnName + "\"");
   
   FillItems(items, fnName, left, "");
   
   #if DEBUG_FUNCTION > 1
   WriteItems("After ParseMathFunction()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseMatrixOps(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseMatrixOps(const std::string &str)
{
   #if DEBUG_MATRIX_OPS
   MessageInterface::ShowMessage
      ("MathParser::ParseMatrixOps() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string left;
   
   // find matrix function
   std::string fnName = GetFunctionName(MATRIX_FUNCTION, str, left);
   
   // Check for matrix operator symbol, such as ' for transpose and ^(-1) for inverse
   if (fnName == "")
   {
      // try matrix op ' for transpose
      std::string::size_type index1 = str.find("'");
      
      #if DEBUG_MATRIX_OPS
      MessageInterface::ShowMessage
         ("MathParser::ParseMatrixOps() find ' for transpose, index1=%u\n", index1);
      #endif
      
      // if transpose ' not found
      if (index1 == str.npos)
      {
         // try matrix op ^(-1) for inverse
         index1 = str.find("^(-1)");
         
         if (index1 == str.npos)
         {
            FillItems(items, "", "", "");
         }
         else // ^(-1) found
         {
            left = str.substr(0, index1);
            fnName = "Inv";
            FillItems(items, fnName, left, "");
         }
      }
      else // ' found
      {
         left = str.substr(0, index1);
         fnName = "Transpose";
         FillItems(items, fnName, left, "");
      }
      
      // Check for invalid operators after matrix ops
      if (fnName != "")
      {
         std::string::size_type index2 = index1 + 1;
         
         if (fnName == "Inv")
            index2 = index1 + 4;
         
         if (str.size() > index2)
         {
            std::string nextOp = str.substr(index2+1, 1);
            #if DEBUG_MATRIX_OPS
            MessageInterface::ShowMessage("   nextOp='%s'\n", nextOp.c_str());
            #endif
            if (nextOp != "" && !IsValidOperator(nextOp))
                throw MathException("Invalid math operator \"" + nextOp + "\" found");
         }
      }
   }
   else // matrix function name found
   {
      FillItems(items, fnName, left, "");
   }
   
   #if DEBUG_MATRIX_OPS
   WriteItems("After ParseMatrixOps()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseUnitConversion(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseUnitConversion(const std::string &str)
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::ParseUnitConversion() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string left;
   
   // find first math function
   std::string fnName = GetFunctionName(UNIT_CONVERSION, str, left);

   if (fnName == "")
      FillItems(items, "", "", "");
   else
      FillItems(items, fnName, left, "");
   
   #if DEBUG_MATH_PARSER > 1
   WriteItems("After ParseUnitConversion()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// bool IsMathFunction(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Tests if input string is any of built-in math functions
 */
//------------------------------------------------------------------------------
bool MathParser::IsMathFunction(const std::string &str)
{
   if (HasFunctionName(str, realFuncList) ||
       HasFunctionName(str, matrixFuncList) ||
       HasFunctionName(str, unitConvList))
      return true;
   
   return false;
}


//------------------------------------------------------------------------------
// bool HasFunctionName(const std::string &str, const StringArray &fnList)
//------------------------------------------------------------------------------
bool MathParser::HasFunctionName(const std::string &str, const StringArray &fnList)
{
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::HasFunctionName() str=%s\n", str.c_str());
   #endif
   
   // Find name from the input function list as is
   if (find(fnList.begin(), fnList.end(), str) != fnList.end())
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::HasFunctionName() returning true\n");
      #endif
      return true;
   }
   
   // Try lower the first letter and find
   std::string str1 = GmatStringUtil::ToLower(str, true);
   if (find(fnList.begin(), fnList.end(), str1) != fnList.end())
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::HasFunctionName() returning true\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::HasFunctionName() returning false\n");
   #endif
   
   return false;
}


//------------------------------------------------------------------------------
// bool IsParenPartOfFunction(const std::string &str)
//------------------------------------------------------------------------------
bool MathParser::IsParenPartOfFunction(const std::string &str)
{
   // Check function name in the GmatFunction list first (loj: 2008.08.20)
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::IsParenPartOfFunction() checking for function name '%s'\n",
       str.c_str());
   #endif
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage("   Checking GmatFunction list...\n");
   #endif
   if (HasFunctionName(str, gmatFuncList))
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::IsParenPartOfFunction() returning true, GmatFunction found\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage("   Checking internal MathFunction list...\n");
   #endif
   if (HasFunctionName(str, realFuncList))
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::IsParenPartOfFunction() returning true, MathFunction found\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage("   Checking internal MatrixFunction list...\n");
   #endif
   if (HasFunctionName(str, matrixFuncList))
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::IsParenPartOfFunction() returning true, MatrixFunction found\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage("   Checking internal UnitConversionFunction list...\n");
   #endif
   if (HasFunctionName(str, unitConvList))
   {
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::IsParenPartOfFunction() returning true, UnitConversionFunction found\n");
      #endif
      return true;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::IsParenPartOfFunction() returning false, no function found\n");
   #endif
   return false;
}


//------------------------------------------------------------------------------
// bool IsGmatFunction(const std::string &name)
//------------------------------------------------------------------------------
bool MathParser::IsGmatFunction(const std::string &name)
{
   // if name has open parenthesis, get it up to
   std::string name1 = name;
   std::string::size_type index = name1.find("(");
   if (index != name1.npos)
      name1 = name1.substr(0, index);
   
   for (int i=0; i<theGmatFuncCount; i++)
      if (name1 == gmatFuncList[i])
         return true;
   
   return false;
}


//------------------------------------------------------------------------------
// bool IsValidOperator(const std::string &str)
//------------------------------------------------------------------------------
bool MathParser::IsValidOperator(const std::string &str)
{
   char op = str[0];
   if (op == '+' || op == '-' || op == '*' || op == '/' || op == '^' || op == '\'')
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// std::string GetFunctionName(UnsignedInt functionType, const std::string &str,
//                             std::string &left)
//------------------------------------------------------------------------------
std::string MathParser::GetFunctionName(UnsignedInt functionType,
                                        const std::string &str,
                                        std::string &left)
{
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::GetFunctionName() entered, functionType=%u, str='%s'\n",
       functionType, str.c_str());
   #endif

   left = "";
   std::string fnName = "";
   
   // if string does not start with letter, just return
   if (!isalpha(str[0]))
      return fnName;
   
   switch (functionType)
   {
   case MATH_FUNCTION:
      {
         BuildFunction(str, realFuncList, fnName, left);
         break;
      }
   case MATRIX_FUNCTION:
      {
         BuildFunction(str, matrixFuncList, fnName, left);
         break;
      }
   case UNIT_CONVERSION:
      {
         BuildFunction(str, unitConvList, fnName, left);
         break;
      }
   case GMAT_FUNCTION:
      {
         BuildFunction(str, gmatFuncList, fnName, left);
         break;
      }
   default:
      break;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::GetFunctionName() leaving, fnName='%s', left='%s'\n",
       fnName.c_str(), left.c_str());
   #endif
   
   return fnName;
}


//------------------------------------------------------------------------------
// void BuildGmatFunctionList(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Builds GmatFunction list found in the GmatFunction path.
 */
//------------------------------------------------------------------------------
void MathParser::BuildGmatFunctionList(const std::string &str)
{
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildGmatFunctionList() entered, str='%s'\n", str.c_str());
   #endif
   
   StringArray names = GmatStringUtil::GetVarNames(str);
   FileManager *fm = FileManager::Instance();
   
   for (UnsignedInt i=0; i<names.size(); i++)
   {
      #if DEBUG_GMAT_FUNCTION > 1
      MessageInterface::ShowMessage
         ("   BuildGmatFunctionList() checking '%s'\n", names[i].c_str());
      #endif
      
      if (fm->GetGmatFunctionPath(names[i]) != "")
      {         
         // if GmatFunction not registered, add to the list
         if (find(gmatFuncList.begin(), gmatFuncList.end(), names[i]) == gmatFuncList.end())
            gmatFuncList.push_back(names[i]);
      }
   }
   
   theGmatFuncCount = gmatFuncList.size();
   
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildGmatFunctionList() found total of %d GmatFunction(s)\n",
       theGmatFuncCount);
   for (Integer i=0; i<theGmatFuncCount; i++)
      MessageInterface::ShowMessage("   '%s'\n", gmatFuncList[i].c_str());
   #endif
}


//------------------------------------------------------------------------------
// void BuildFunction(const std::string &str, const StringArray &fnList,
//                    std::string &fnName, std::string &left)
//------------------------------------------------------------------------------
void MathParser::BuildFunction(const std::string &str, const StringArray &fnList,
                               std::string &fnName, std::string &left)
{
   UnsignedInt count = fnList.size();
   fnName = "";
   left = "";
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildFunction() entered, str='%s', function count=%d\n",
       str.c_str(), count);
   #endif
   
   if (count == 0)
      return;
   
   std::string::size_type functionIndex = str.npos;
   
   // Check if function name is in the function list
   std::string fname = GmatStringUtil::ParseFunctionName(str);
   
   #if DEBUG_FUNCTION > 1
   MessageInterface::ShowMessage("==> fname = '%s'\n", fname.c_str());
   #endif
   
   if (find(fnList.begin(), fnList.end(), fname) != fnList.end())
   {
      fnName = fname;
      functionIndex = str.find(fname + "(");
   }
   else
   {
      // Let's try lower case of the first letter
      if (isalpha(fname[0]) && isupper(fname[0]))
      {
         // MSVC++ failes to do: fname[0] = tolower(fname[0])
         std::string fname1 = fname;
         fname1[0] = tolower(fname[0]);
         
         #if DEBUG_FUNCTION > 1
         MessageInterface::ShowMessage
            ("   function name '%s' not found so trying lower case '%s'\n",
             fname.c_str(), fname1.c_str());
         #endif
         
         if (find(fnList.begin(), fnList.end(), fname1) != fnList.end())
         {
            fnName = fname1;
            functionIndex = str.find(fname + "(");
         }
      }
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildFunction() fnName='%s', functionIndex=%u\n",
       fnName.c_str(), functionIndex);
   #endif
   
   if (fnName != "")
   {
      std::string::size_type index1 = str.find("(", functionIndex);
      
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::BuildFunction() calling FindMatchingParen() with start "
          "index=%u\n", index1);
      #endif
      
      UnsignedInt index2 = FindMatchingParen(str, index1);
      
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::BuildFunction() matching ) found at %u\n", index2);
      #endif
      
      left = str.substr(index1+1, index2-index1-1);
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildFunction() leaving, fnName=%s, left=%s\n",
       fnName.c_str(), left.c_str());
   #endif
}


//------------------------------------------------------------------------------
// std::string::size_type FindMatchingParen(const std::string &str, size_type start)
//------------------------------------------------------------------------------
std::string::size_type MathParser::FindMatchingParen(const std::string &str,
                                                     std::string::size_type start)
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::FindMatchingParen() entered, str='%s', str.size()=%u, start=%u\n",
       str.c_str(), str.size(), start);
   #endif
   
   int leftCounter = 0;
   int rightCounter = 0;
   
   for (UnsignedInt i = start; i < str.size(); i++)
   {
      if (str[i] == '(')
         leftCounter++;

      if (str[i] == ')')
         rightCounter++;
      
      if (leftCounter == rightCounter)
         return i;
   }
   
   #if DEBUG_MATH_PARSER
   MessageInterface::ShowMessage
      ("**** ERROR ****  MathParser::FindMatchingParen() Unmatching parenthesis found\n");
   #endif
   
   throw MathException("Unmatching parenthesis found");
}


//------------------------------------------------------------------------------
// void FillItems(StringArray &items, const std::string &op,
//                const std::string &left, const std::string &right)
//------------------------------------------------------------------------------
void MathParser::FillItems(StringArray &items, const std::string &op,
                           const std::string &left, const std::string &right)
{
   items.push_back(op);
   items.push_back(left);
   items.push_back(right);
}


//------------------------------------------------------------------------------
// void WriteItems(const std::string &msg, StringArray &items)
//------------------------------------------------------------------------------
void MathParser::WriteItems(const std::string &msg, StringArray &items)
{
   MessageInterface::ShowMessage
      ("%s items = <%s> <%s> <%s>\n", msg.c_str(),
       items[0].c_str(), items[1].c_str(), items[2].c_str());
}


//------------------------------------------------------------------------------
// void WriteNode(MathNode *node, UnsignedInt level)
//------------------------------------------------------------------------------
void MathParser::WriteNode(MathNode *node, UnsignedInt level)
{
#if DEBUG_PARSE
   
   if (node == NULL)
      return;

   level++;

   if (!node->IsFunction())
   {
      for (UnsignedInt i=0; i<level; i++)
         MessageInterface::ShowMessage("....");
      
      MessageInterface::ShowMessage
         ("node=%s: %s\n", node->GetTypeName().c_str(), node->GetName().c_str());
   }
   else
   {
      for (UnsignedInt i=0; i<level; i++)
         MessageInterface::ShowMessage("....");

      if (node->IsOfType("FunctionRunner"))
         MessageInterface::ShowMessage
            ("node=%s: %s\n", node->GetTypeName().c_str(), node->GetName().c_str());
      else
         MessageInterface::ShowMessage
            ("node=%s: %s\n", node->GetTypeName().c_str(), node->GetName().c_str());
      
      if (node->GetLeft())
      {
         for (UnsignedInt i=0; i<level; i++)
            MessageInterface::ShowMessage("....");
         
         MessageInterface::ShowMessage
            ("left=%s: %s\n", node->GetLeft()->GetTypeName().c_str(),
             node->GetLeft()->GetName().c_str());
         
         WriteNode(node->GetLeft(), level);
      }
      
      
      if (node->GetRight())
      {
         for (UnsignedInt i=0; i<level; i++)
            MessageInterface::ShowMessage("....");
         
         MessageInterface::ShowMessage
            ("right=%s: %s\n", node->GetRight()->GetTypeName().c_str(),
             node->GetRight()->GetName().c_str());
         
         WriteNode(node->GetRight(), level);
      }
   }
#endif
}


//------------------------------------------------------------------------------
// void BuildAllFunctionList()
//------------------------------------------------------------------------------
/*
 * Builds function list.
 *
 * @note
 * We cannot use the list from the Moderator::GetListOfFactoryItems(Gmat::MATH_TREE)
 * since it needs to be grouped into the order of parsing.
 *    ParseParenthesis()
 *    ParseAddSubtract()
 *    ParseMultDivide()
 *    ParsePower()
 *    ParseUnary()
 *    ParseMathFunctions()
 *    ParseMatrixOps()
 *    ParseUnitConversion()
 *
 */
//------------------------------------------------------------------------------
void MathParser::BuildAllFunctionList()
{
   //@todo We should get this list from the MathFactory.
   // Why power (^) is not here? (LOJ: 2010.11.04)
   // Real Function List
   realFuncList.push_back("asin");
   realFuncList.push_back("sin");
   realFuncList.push_back("acos");
   realFuncList.push_back("cos");
   realFuncList.push_back("atan2");
   realFuncList.push_back("atan");
   realFuncList.push_back("tan");
   realFuncList.push_back("log10");
   realFuncList.push_back("log");
   realFuncList.push_back("exp");
   realFuncList.push_back("sqrt");
   realFuncList.push_back("abs");
   
   // Matrix Function List
   matrixFuncList.push_back("transpose");
   matrixFuncList.push_back("det");
   matrixFuncList.push_back("inv");
   matrixFuncList.push_back("norm");
   
   // Unit Conversion List
   unitConvList.push_back("degToRad");
   unitConvList.push_back("radToDeg");
   unitConvList.push_back("deg2Rad");
   unitConvList.push_back("rad2Deg");
   
   // Matrix Operator List
   matrixOpList.push_back("'");
   matrixOpList.push_back("^(-1)");
   
}

