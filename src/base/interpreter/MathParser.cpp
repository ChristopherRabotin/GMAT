//$Id$
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
//#define DEBUG_FUNCTION 1
//#define DEBUG_MATH_PARSER_PARAM 1
//#define DEBUG_INVERSE_OP 1
//#define DEBUG_POWER 1
//#define DEBUG_UNARY 1
//#define DEBUG_CREATE_NODE 1
//#define DEBUG_GMAT_FUNCTION 1

//---------------------------------
// static data
//---------------------------------
const std::string
MathParser::MATH_FUNC_LIST[MathFuncCount] =
{
   "asin",  "sin",  "acos",  "cos",  "atan2",  "atan",  "tan",  
   "log10", "log",  "exp",  "sqrt",  "abs",
}; 


const std::string
MathParser::MATRIX_FUNC_LIST[MatrixFuncCount] =
{
   "transpose",  "det",   "inv",  "norm",
}; 


const std::string
MathParser::MATRIX_OP_LIST[MatrixOpCount] =
{
   "'",  "^(-1)",
}; 


const std::string
MathParser::UNIT_CONV_LIST[UnitConvCount] =
{
   "degToRad",  "radToDeg",
}; 


std::string
MathParser::GMAT_FUNC_LIST[MAX_GMAT_FUNCTIONS] =
{
   "", "", "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "", "", "",
   "", "", "", "", "", "", "", "", "", "",
}; 


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
   theGmatFuncCount = 0;
   
   if (this == &right)
      return *this;
   
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
// bool IsEquation(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Examines if given string is an equation.
 * Call this method with RHS of assignement.
 *
 * @param str The string to be examined for an equation
 * @return  true if string is an equation, means has math operators and/or functions
 *
 */
//------------------------------------------------------------------------------
bool MathParser::IsEquation(const std::string &str)
{
   theEquation = str;
   
   #if DEBUG_PARSE_EQUATION
   MessageInterface::ShowMessage
      ("MathParser::IsEquation() str=%s\n", str.c_str());
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
 * Single operators are +, -, *, /, ^.
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
      ("FindLowestOperator() entered str=%s, start=%u, length=%u\n", str.c_str(),
       start, str.size());
   #endif
   
   Integer firstOpen = str.find_first_of("(", start);
   Integer start1 = start;
   Integer open1, close1;
   Integer length = str.size();
   Integer index;
   bool isOuterParen;
   bool done = false;
   std::string opStr;
   std::map<std::string, Integer> opIndexMap;
   std::string substr;
   std::map<std::string, Integer>::iterator pos;
   
   //-----------------------------------------------------------------
   // find a lowest operator before first open paren
   //-----------------------------------------------------------------
   if (firstOpen != 0)
   {
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage
         ("FindLowestOperator() Find operator before first open paren, "
          "firstOpen=%u\n", firstOpen);
      #endif
      
      substr = str.substr(0, firstOpen);
      opStr = FindOperator(substr, index);
      if (opStr != "")
         opIndexMap[opStr] = index;
   }
   
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindLowestOperator() Find lowest operator before last close parenn");
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
         break;
      
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
   if (close1 != length-1)
   {
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage("   after last close paren\n");
      #endif
      
      substr = str.substr(close1+1);
      opStr = FindOperator(substr, index);
      
      if (opStr != "")
         opIndexMap[opStr] = close1 + index + 1;
   }
   
   #if DEBUG_FIND_OPERATOR
   for (pos = opIndexMap.begin(); pos != opIndexMap.end(); ++pos)
      MessageInterface::ShowMessage
         ("      op=%s, index=%u\n", pos->first.c_str(), pos->second);
   #endif
   
   std::map<std::string, Integer>::iterator pos1;
   std::map<std::string, Integer>::iterator pos2;
   std::string lastOp;
   
   // find + or - first
   pos1 = opIndexMap.find("+");
   pos2 = opIndexMap.find("-");
   if (pos1 != opIndexMap.end() || pos2 != opIndexMap.end())
   {
      opStr = GetOperator(pos1, pos2, opIndexMap, index);
   }
   else
   {
      // find * or /
      pos1 = opIndexMap.find("*");
      pos2 = opIndexMap.find("/");
      if (pos1 != opIndexMap.end() || pos2 != opIndexMap.end())
      {
         opStr = GetOperator(pos1, pos2, opIndexMap, index);
         
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage
            ("   found * or / pos1=%u, pos2=%u\n", pos1, pos2);
         #endif
      }
      else
      {
         // find ^
         pos1 = opIndexMap.find("^");
         if (pos1 != opIndexMap.end())
         {
            opStr = pos1->first;
            index = pos1->second;
         }
      }
   }
   
   
   opIndex = index;
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindLowestOperator() returning opStr=%s, opIndex=%u\n",
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
   
   // Should parameters be created here?
   if (topNode)
      CreateParameter(topNode, 0);
   
   #if DEBUG_PARSE
   WriteNode(topNode, 0);
   #endif
   
   return topNode;
}


//------------------------------------------------------------------------------
// StringArray GetGmatFunctionNames()
//------------------------------------------------------------------------------
StringArray MathParser::GetGmatFunctionNames()
{
   static StringArray gmatFuncList;
   gmatFuncList.clear();
   
   for (int i=0; i<theGmatFuncCount; i++)
      gmatFuncList.push_back(GMAT_FUNC_LIST[i]);
   
   return gmatFuncList;
}


//------------------------------------------------------------------------------
// MathNode* ParseNode(const std::string &str)
//------------------------------------------------------------------------------
MathNode* MathParser::ParseNode(const std::string &str)
{
   #if DEBUG_CREATE_NODE
   MessageInterface::ShowMessage("MathParser::ParseNode() str=%s\n", str.c_str());
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
         ("=====> Should create MathElement: %s\n", str.c_str());
      #endif
      
      // check for surrounding parenthesis
      Integer open, close;
      bool isOuterParen;
      std::string str1 = str;
      GmatStringUtil::FindMatchingParen(str1, open, close, isOuterParen);
      if (isOuterParen)
         str1 = str.substr(open+1, close-open-1);
      
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
      
      #if DEBUG_CREATE_NODE
      MessageInterface::ShowMessage
         ("===============> Create left node: %s\n", left.c_str());
      #endif
      
      if (left != "")
         leftNode = ParseNode(left);
      
      #if DEBUG_CREATE_NODE
      MessageInterface::ShowMessage
         ("===============> Create right node: %s\n", right.c_str());
      #endif
      
      if (right != "")
         rightNode = ParseNode(right);
      
      // check for empty argument for function
      if (left == "")
      {
         if (HasFunctionName(op, MATH_FUNC_LIST, MathFuncCount))
            throw MathException("Need an argument of the function \"" + op + "\"");
      }
      
      // check if two operands are needed
      if (right == "")
         if (op == "Add" || op == "Subtract" || op == "Multiply" ||
             op == "Divide")
         {
            throw MathException("Need right side of the operator \"" + op + "\"");
         }
      
      mathNode->SetChildren(leftNode, rightNode);
   }

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
   if (IsGmatFunction(type))
      actualType = "FunctionRunner";
   
   #ifdef __UNIT_TEST__
   static MathFactory mf;
   MathNode *node = mf.CreateMathNode(actualType, exp);
   #else
   Moderator* theModerator = Moderator::Instance();
   MathNode *node = theModerator->CreateMathNode(actualType, exp);
   #endif
   
   if (node == NULL)
      throw MathException("Cannot create MathNode of \"" + actualType + "\"");
   
   if (actualType == "FunctionRunner")
   {
      FunctionRunner *fRunner = (FunctionRunner*)node;
      fRunner->SetFunctionName(type);
      
      // add function input arguments
      std::string exp1 = exp;
      exp1 = GmatStringUtil::RemoveExtraParen(exp1);
      StringArray inputs = GmatStringUtil::SeparateBy(exp1, ",", true);
      for (UnsignedInt i=0; i<inputs.size(); i++)
      {
         #if DEBUG_CREATE_NODE > 1
         MessageInterface::ShowMessage("   inputs[%d] = '%s'\n", i, inputs[i].c_str());
         #endif
         
         if (IsGmatFunction(inputs[i]))
            fRunner->AddFunctionInput("");
         else
            fRunner->AddFunctionInput(inputs[i]);
      }
      
      // add function output arguments
      fRunner->AddFunctionOutput("");
      
      // set function inputs and outputs to FunctionManager through FunctionRunner
      fRunner->SetFunctionInputs();
      fRunner->SetFunctionOutputs();
   }
   
   #if DEBUG_CREATE_NODE
   MessageInterface::ShowMessage
      ("MathParser::CreateNode() returning node <%p>\n", node);
   #endif
   
   return node;
}


//------------------------------------------------------------------------------
// StringArray Decompose(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Breaks string into operator, left and right elements.
 *
 * @return StringArray of elements
 */
//------------------------------------------------------------------------------
StringArray MathParser::Decompose(const std::string &str)
{
   #if DEBUG_DECOMPOSE
   MessageInterface::ShowMessage("MathParser::Decompose() str=%s\n", str.c_str());
   #endif

   StringArray items = ParseParenthesis(str);
   
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
            ("MathParser::Decompose() Found outer paren. str1=%s\n", str1.c_str());
         #endif
      }
   }
   
   if (items[0] == "function")
      items[0] = "";
   
   if (items[0] == "")
   {
      items = ParseAddSubtract(str1);
      
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
StringArray MathParser::ParseParenthesis(const std::string &str)
{
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::ParseParenthesis() str=%s\n", str.c_str());
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
      WriteItems("MathParser::ParseParenthesis(): lowest + or - found."
                 " returning ", items);
      #endif
      
      return items;      
   }
   
   //-----------------------------------------------------------------
   // if ( is part of fuction, just return first parenthesis
   //-----------------------------------------------------------------
   std::string substr = str.substr(0, index1);
   //MessageInterface::ShowMessage("===> substr=%s\n", substr.c_str());
   
   if (IsParenPartOfFunction(substr))
   {
      // find matching closing parenthesis
      UnsignedInt index2 = FindMatchingParen(str, index1);
      
      #if DEBUG_PARENTHESIS
      MessageInterface::ShowMessage
         ("Paren is Part of Function. str=%s, size=%u, index1=%u, index2=%u\n",
          str.c_str(), str.size(), index1, index2);
      #endif
      
      // if last char is ')'
      if (index2 == str.size()-1)
      {
         // find math function
         op = GetFunctionName(MATH_FUNCTION, str, left);
         if (op == "")
            op = GetFunctionName(MATRIX_FUNCTION, str, left);
         if (op == "")
            op = GetFunctionName(UNIT_CONVERSION, str, left);
         if (op == "")
            op = GetFunctionName(GMAT_FUNCTION, str, left);
      }
      
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
      
      // handle special atan2(y,x) function
      if (op == "atan2")
      {
         std::string::size_type comma = str.find(',', index1);
         if (comma == str.npos)
            throw MathException("Missing 2nd argument for atan2(y,x)\n");

         left = str.substr(index1+1, comma-index1-1);
         right = str.substr(comma+1, index2-comma-1);
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
         ("MathParser::ParseParenthesis() No operator found\n");
      #endif
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
      ("MathParser::FindOperatorFrom() str=%s, start=%u\n", str.c_str(), start);
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
            ("MathParser::FindOperatorFrom() found ^ str=%s, index1=%u\n",
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
 * Finds operator from the input string.
 * Single operators are +, -, *, /, ^.
 * Double operators are ++, --, +-, -+, *-, /-, ^+, ^-
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
   MessageInterface::ShowMessage("MathParser::FindOperator() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op;
   std::string::size_type index;
   std::string::size_type index1 = str.find_last_of("+");
   std::string::size_type index2 = str.find_last_of("-");
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindOperator() for +,- index1=%u, index2=%u\n", index1, index2);
   #endif
   
   if (index1 == str.npos && index2 == str.npos)
   {
      index1 = str.find_last_of("*");
      index2 = str.find_last_of("/");
      
      #if DEBUG_FIND_OPERATOR
      MessageInterface::ShowMessage
         ("FindOperator() for *,/ index1=%u, index2=%u\n", index1, index2);
      #endif
      
      if (index1 == str.npos && index2 == str.npos)
      {
         index1 = str.find_last_of("^");
         
         #if DEBUG_FIND_OPERATOR
         MessageInterface::ShowMessage
            ("FindOperator() for ^ index1=%u\n", index1);
         #endif
         
         if (index1 != str.npos)
         {
            //MessageInterface::ShowMessage
            //   ("MathParser::FindOperator() found ^ str=%s, index1=%u\n",
            //    str.c_str(), index1);
            
            // try for ^(-1) for inverse
            if (str.substr(index1, 5) == "^(-1)")
            {
               #if DEBUG_FIND_OPERATOR
               MessageInterface::ShowMessage
                  ("MathParser::FindOperator() returning \"\"\n");
               #endif
               return "";
            }
         }
         else
         {
            #if DEBUG_FIND_OPERATOR
            MessageInterface::ShowMessage
               ("MathParser::FindOperator() returning \"\"\n");
            #endif
            return "";
         }
      }
   }
   
   // if both operators found, assign to greator position
   if (index1 != str.npos && index2 != str.npos)
      index = index1 > index2 ? index1 : index2;
   else
      index = index1 == str.npos ? index2 : index1;
   
   
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("FindOperator() index=%u, index1=%u, index2=%u\n", index, index1, index2);
   #endif
   
   // Causing VC++ run time error 
   // check for double operator (*-, /-)
   
   if (index == 0)
   {
      op = str.substr(index, 1);
      opIndex = index;
   }
   else if (str[index-1] == '+' || str[index-1] == '-' ||
            str[index-1] == '*' || str[index-1] == '/' ||
            str[index-1] == '^')
   {
      op = str.substr(index-1, 1);
      opIndex = index-1;
   }
   else if (index != str.npos)
   {
      op = str.substr(index, 1);
      opIndex = index;
   }
   else
   {
      op = "";
      opIndex = -1;
   }
      
   #if DEBUG_FIND_OPERATOR
   MessageInterface::ShowMessage
      ("MathParser::FindOperator() returning op=%s, opIndex=%u\n", op.c_str(), opIndex);
   #endif

   return op;
   
}


//------------------------------------------------------------------------------
// std::string MathParser::GetOperator(const std::map<std::string, Integer>::iterator &pos1,
//                                     const std::map<std::string, Integer>::iterator &pos2,
//                                     const std::map<std::string, Integer> &opIndexMap,
//                                     Integer &opIndex)
//------------------------------------------------------------------------------
std::string MathParser::GetOperator(const std::map<std::string, Integer>::iterator &pos1,
                                        const std::map<std::string, Integer>::iterator &pos2,
                                        const std::map<std::string, Integer> &opIndexMap,
                                        Integer &opIndex)
{
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
   return opStr;
}


//------------------------------------------------------------------------------
// UnsignedInt FindSubtract(const std::string &str, std::string::size_type start)
//------------------------------------------------------------------------------
std::string::size_type MathParser::FindSubtract(const std::string &str, std::string::size_type start)
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
   
   //return index2;
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
         ("combined operator found, %s\n", str.substr(index, 2));
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
   
   // find first ^
   std::string::size_type index1 = str.find('^');
   
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
      //std::string left = str.substr(0, index1);
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

   if (fnName == "")
   {
      // try matrix op ' for transpose
      std::string::size_type index1 = str.find("'");

      #if DEBUG_MATRIX_OPS
      MessageInterface::ShowMessage
         ("MathParser::ParseMatrixOps() find ' index1=%u\n", index1);
      #endif
      
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
            fnName = "inv";
            FillItems(items, fnName, left, "");
         }
      }
      else // ' found
      {
         left = str.substr(0, index1);
         fnName = "transpose";
         FillItems(items, fnName, left, "");
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
// bool HasFunctionName(const std::string &str, const std::string list[],
//                      UnsignedInt count)
//------------------------------------------------------------------------------
bool MathParser::HasFunctionName(const std::string &str, const std::string list[],
                                 UnsignedInt count)
{
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::HasFunctionName() str=%s\n", str.c_str());
   #endif
   
   for (UnsignedInt i=0; i<count; i++)
   {
      if (str == list[i])
         return true;
   }
   
   // Try Capitalized function name
   for (UnsignedInt i=0; i<count; i++)
   {
      if (str == GmatStringUtil::Capitalize(list[i]))
         return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// bool IsParenPartOfFunction(const std::string &str)
//------------------------------------------------------------------------------
bool MathParser::IsParenPartOfFunction(const std::string &str)
{
   if (HasFunctionName(str, MATH_FUNC_LIST, MathFuncCount) ||
       HasFunctionName(str, MATRIX_FUNC_LIST, MatrixFuncCount) ||
       HasFunctionName(str, UNIT_CONV_LIST, UnitConvCount))
      return true;

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
      if (name1 == GMAT_FUNC_LIST[i])
         return true;
   
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
      ("MathParser::GetFunctionName() functionType=%u, str=%s\n", functionType,
       str.c_str());
   #endif
   
   std::string fnName = "";
   
   switch (functionType)
   {
   case MATH_FUNCTION:
      {
         BuildFunction(str, MATH_FUNC_LIST, MathFuncCount, fnName, left);
         break;
      }
   case MATRIX_FUNCTION:
      {
         BuildFunction(str, MATRIX_FUNC_LIST, MatrixFuncCount, fnName, left);
         break;
      }
   case UNIT_CONVERSION:
      {
         BuildFunction(str, UNIT_CONV_LIST, UnitConvCount, fnName, left);
         break;
      }
   case GMAT_FUNCTION:
      {
         BuildFunction(str, GMAT_FUNC_LIST, theGmatFuncCount, fnName, left);
         break;
      }
   default:
      break;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::GetFunctionName() fnName=%s, left=%s\n", fnName.c_str(),
       left.c_str());
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
   StringArray names = GmatStringUtil::GetVarNames(str);
   FileManager *fm = FileManager::Instance();
   theGmatFuncCount = 0;
   
   for (UnsignedInt i=0; i<names.size(); i++)
   {
      if (fm->GetGmatFunctionPath(names[i]) != "")
      {
         GMAT_FUNC_LIST[theGmatFuncCount++] = names[i];
         
         // Check for maximum number of GmatFunction allowed
         if (theGmatFuncCount >= MAX_GMAT_FUNCTIONS)
         {
            MathException ex;
            ex.SetDetails("It reached the maximum number of GmatFunction %d",
                          MAX_GMAT_FUNCTIONS);
            throw ex;
         }
      }
   }
   
   #if DEBUG_GMAT_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildGmatFunctionList() found %d GmatFunctions\n",
       theGmatFuncCount);
   #endif
}


//------------------------------------------------------------------------------
// void BuildFunction(const std::string &str, std::string list[],
//                     UnsignedInt count, std::string &fnName, std::string &left)
//------------------------------------------------------------------------------
void MathParser::BuildFunction(const std::string &str, const std::string list[],
                               UnsignedInt count, std::string &fnName,
                               std::string &left)
{
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildFunction() str='%s', count=%d\n", str.c_str(), count);
   #endif
   
   std::string::size_type functionIndex = str.npos;
   
   for (UnsignedInt i=0; i<count; i++)
   {
      #if DEBUG_FUNCTION > 1
      MessageInterface::ShowMessage("   list[%d]='%s'\n", i, list[i].c_str());
      #endif
      
      functionIndex = str.find(list[i] + "(");
      
      // Try function name with first letter capitalized
      if (functionIndex == str.npos)
         functionIndex = str.find(GmatStringUtil::Capitalize(list[i]) + "(");
      
      if (functionIndex != str.npos)
      {
         fnName = list[i];
         break;
      }
   }
   
   if (fnName != "")
   {
      UnsignedInt index1 = str.find("(", functionIndex);
      UnsignedInt index2 = FindMatchingParen(str, index1);
      
      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::BuildFunction() index1=%u, index2=%u\n", index1, index2);
      #endif
      
      left = str.substr(index1+1, index2-index1-1);
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildFunction() fnName=%s, left=%s\n", fnName.c_str(), left.c_str());
   #endif
}


//------------------------------------------------------------------------------
// UnsignedInt FindMatchingParen(const std::string &str, UnsignedInt start)
//------------------------------------------------------------------------------
std::string::size_type MathParser::FindMatchingParen(const std::string &str, std::string::size_type start)
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::FindMatchingParen() str=%s, start=%u\n", str.c_str(), start);
   #endif
   
   int leftCounter = 0;
   int rightCounter = 0;
   
   for (UnsignedInt i=start; i<str.size(); i++)
   {
      if (str[i] == '(')
         leftCounter++;

      if (str[i] == ')')
         rightCounter++;
      
      if (leftCounter == rightCounter)
         return i;
   }
   
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
// void CreateParameter(MathNode *node, UnsignedInt level))
//------------------------------------------------------------------------------
void MathParser::CreateParameter(MathNode *node, UnsignedInt level)
{
   #ifndef __UNIT_TEST__
   if (node == NULL)
      return;
   
   level++;
   
   if (!node->IsFunction())
   {
      if (!node->IsNumber())
      {
         Moderator* theModerator = Moderator::Instance();
         std::string owner, type, depObj;
         std::string name = node->GetName();
         GmatStringUtil::ParseParameter(name, type, owner, depObj);

         if (name != "" && type != "")
         {
            #if DEBUG_MATH_PARSER_PARAM
            MessageInterface::ShowMessage
               ("MathParser::CreateParameter() name=%s, type=%s, owner=%s, "
                "depObj=%s\n", name.c_str(), type.c_str(), owner.c_str(),
                depObj.c_str());
            #endif

            theModerator->CreateParameter(type, name, owner, depObj);
         }
      }
   }
   else
   {      
      if (node->GetLeft())
         CreateParameter(node->GetLeft(), level);

      if (node->GetRight())
         CreateParameter(node->GetRight(), level);
   }

   #endif
   
}

