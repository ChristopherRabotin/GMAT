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
 * Class implementation for the MathParser.
 */
//------------------------------------------------------------------------------

#include "MathParser.hpp"
#include "MathFunction.hpp"
#include "StringUtil.hpp"
#include "MessageInterface.hpp"

#ifdef __USE_SIMPLE_NODE__
#include "SimpleMathNode.hpp"
#endif

#ifdef __UNIT_TEST__
#include "MathFactory.hpp"
#else
#include "Moderator.hpp"
#endif

//#define DEBUG_MATH_PARSER 1
//#define DEBUG_DECOMPOSE 1
//#define DEBUG_PARENTHESIS 1
//#define DEBUG_ADD_SUBTRACT 1
//#define DEBUG_PARSE 1
//#define DEBUG_FUNCTION 1


//---------------------------------
// static data
//---------------------------------
const std::string
MathParser::MATH_FUNC_LIST[MathFunctionCount] =
{
   "sin",  "cos",   "tan",  "asin",  "acos",  "atan2",  "atan",
   "log",  "log10", "exp",  "sqrt",
}; 


const std::string
MathParser::MATRIX_OPS_LIST[MatrixOpsCount] =
{
   "transpose",  "det",   "inv",  "norm", 
}; 


const std::string
MathParser::UNIT_CONV_LIST[UnitConvCount] =
{
   "degToRad",  "radToDeg",
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
// MathNode*  Parse(const std::string &theEquation)
//------------------------------------------------------------------------------
/**
 * Breaks apart the text representation of an equation and uses the compoment
 * pieces to construct the MathTree.
 *
 * @return constructed MathTree pointer
 */
//------------------------------------------------------------------------------
#ifdef __USE_SIMPLE_NODE__
SimpleMathNode* MathParser::Parse(const std::string &theEquation)
#else
MathNode* MathParser::Parse(const std::string &theEquation)
#endif
{
   #if DEBUG_PARSE
   MessageInterface::ShowMessage
      ("=================================================================\n");
   MessageInterface::ShowMessage
      ("MathParser::Parse() theEquation=%s\n", theEquation.c_str());
   MessageInterface::ShowMessage
      ("=================================================================\n");
   #endif
   
   // first remove all blank spaces
   std::string newEq = GmatStringUtil::RemoveAll(theEquation, ' ');
   
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::Parse() newEq=%s\n", newEq.c_str());
   #endif
   
   //Is this done in Assignment command?
   //MathTree *mathTree = new MathTree();

   //MathTree->SetTopNode(ParseNode(newEq));

   // for testing
   #ifdef __USE_SIMPLE_NODE__
   SimpleMathNode *topNode = ParseNode(newEq);
   #else
   MathNode *topNode = ParseNode(newEq);
   #endif
   
   #if DEBUG_PARSE
   WriteNode(topNode, 0);
   #endif

   return topNode;
}


//------------------------------------------------------------------------------
// MathNode* ParseNode(const std::string &str)
//------------------------------------------------------------------------------
#ifdef __USE_SIMPLE_NODE__
SimpleMathNode* MathParser::ParseNode(const std::string &str)
#else
MathNode* MathParser::ParseNode(const std::string &str)
#endif
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage("MathParser::ParseNode() str=%s\n", str.c_str());
   #endif

   StringArray items = Decompose(str);
   std::string op = items[0];
   std::string left = items[1];
   std::string right = items[2];

   #if DEBUG_MATH_PARSER > 1
   WriteItems("MathParser::ParseNode() After Decompose()", items);
   #endif
   
   #ifdef __USE_SIMPLE_NODE__
   SimpleMathNode *mathNode;
   #else
   MathNode *mathNode;
   #endif
   
   // If operator is empty, create MathElement, create MathFunction otherwise
   if (op == "")
   {
      #if DEBUG_MATH_PARSER > 1
      MessageInterface::ShowMessage
         ("=====> Should create MathElement: %s\n", str.c_str());
      #endif
      
      mathNode = CreateNode("MathElement", str);
   }
   else
   {
      #if DEBUG_MATH_PARSER > 1
      MessageInterface::ShowMessage
         ("=====> Should create MathNode: %s\n", op.c_str());
      #endif
      
      std::string operands = "( " + left + ", " + right + " )";
      if (right == "")
         operands = "( " + left + " )";

      mathNode = CreateNode(op, operands);
      
      #ifdef __USE_SIMPLE_NODE__
      SimpleMathNode *leftNode = NULL;
      SimpleMathNode *rightNode = NULL;
      #else
      MathNode *leftNode = NULL;
      MathNode *rightNode = NULL;
      #endif
      
      #if DEBUG_MATH_PARSER > 1
      MessageInterface::ShowMessage
         ("===============> Create left node: %s\n", left.c_str());
      #endif
      
      if (left != "")
         leftNode = ParseNode(left);

      #if DEBUG_MATH_PARSER > 1
      MessageInterface::ShowMessage
         ("===============> Create right node: %s\n", right.c_str());
      #endif
      
      if (right != "")
         rightNode = ParseNode(right);

      #ifdef __USE_SIMPLE_NODE__
         mathNode->SetChildren(leftNode, rightNode);
      #else
         ((MathFunction*)mathNode)->SetChildren(leftNode, rightNode);
      #endif
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
#ifdef __USE_SIMPLE_NODE__
SimpleMathNode* MathParser::CreateNode(const std::string &type, const std::string &exp)
#else
MathNode* MathParser::CreateNode(const std::string &type, const std::string &exp)
#endif
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::CreateNode() type=%s, exp=%s\n", type.c_str(),
       exp.c_str());
   #endif

   
   #ifndef __USE_SIMPLE_NODE__

   #ifdef __UNIT_TEST__
   static MathFactory mf;
   MathNode *node = mf.CreateMathNode(type, exp);
   #else
   Moderator* theModerator = Moderator::Instance();
   MathNode *node = theModerator->CreateMathNode(type, exp);
   #endif

   
   #else
   
   SimpleMathNode *node = new SimpleMathNode(type, exp);
   
   if (type == "MathElement")
   {
      // just for testing
      if (exp.find("matA") != exp.npos)
      {
         Rmatrix matA(3, 3, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 80.0, 90.0);
         node->SetMatrixValue(matA);
      }
      else if (exp.find("matB") != exp.npos)
      {
         Rmatrix matB(3, 3, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
         node->SetMatrixValue(matB);
      }
      else
      {
         Real rval;
         if (GmatStringUtil::ToDouble(exp, &rval))
            ((SimpleMathNode*)node)->SetRealValue(rval);
      }
   }
   
   #endif
   
   if (node == NULL)
      throw MathException("*** ERROR *** Cannot create MathNode" + type);

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
      str1 = str.substr(1, str.size()-2);
   
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
   WriteItems("==> MathParser::Decompose(): returning ", items);
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
   
   // find opening parenthesis '('
   UnsignedInt index1 = str.find('(');
   
   if (index1 == str.npos)
   {
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("==> MathParser::ParseParenthesis(): open parenthesis not found."
                 " returning ", items);
      #endif
      
      return items;
   }

   // if ( is part of fuction, just return
   if (IsParenPartOfFunction(str[index1-1]))
   {
      // find match closing parenthesis ')'
      UnsignedInt index2 = FindMatchingParen(str, index1);

      // if char is ')'
      if (index2 == str.size()-1)
      {
         // find first math function
         op = GetFunction(MATH_FUNCTION, str, left);
      }
      
      //left = str.substr(index1+1, index2-index1-1);
      
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("==> MathParser::ParseParenthesis() - parenthesis is part "
                 "of function. returning ", items);
      #endif
      
      return items;
   }
   
   // find match closing parenthesis ')'
   UnsignedInt index2 = FindMatchingParen(str, index1);
      
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::ParseParenthesis() index1=%d, index2=%d, str.size()=%d\n",
       index1, index2, str.size());
   #endif

   if (index1 == 0 && index2 == str.size()-1)
   {
      left = str.substr(1, str.size()-2);
      FillItems(items, op, left, right);
      
      #if DEBUG_PARENTHESIS
      WriteItems("==> MathParser::ParseParenthesis(): complete parenthesis found."
                 " returning ", items);
      #endif
      
      return items;
   }
   
   // find opening parenthesis '('
   UnsignedInt index3 = str.find('(', index2);
   if (index3 != str.npos)
   {
      op = str.substr(index3-1, 1);
      op = GetOperatorName(op);
      left = str.substr(0, index2+1);
      right = str.substr(index3, str.size()-index3+1);
   }
   else
   {
      op = FindOperator(str, index2, left, right);
   }
   
   FillItems(items, op, left, right);
   
   #if DEBUG_PARENTHESIS
   WriteItems("==> MathParser::ParseParenthesis() returning ", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string MathParser::FindOperator(const std::string &str, UnsignedInt start,
                                     std::string &left, std::string &right)
{
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::FindOperator() str=%s, start=%d\n", str.c_str(), start);
   #endif
   
   StringArray items;
   std::string op;
   UnsignedInt index;
   UnsignedInt index1 = str.find("+", start);
   UnsignedInt index2 = str.find("-", start);
   
   if (index1 == str.npos && index2 == str.npos)
   {
      index1 = str.find("*", start);
      index2 = str.find("/", start);
      
      if (index1 == str.npos && index2 == str.npos)
      {
         index1 = str.find("^", start);
      }
   }
   
   index = index1 == str.npos ? index2 : index1;

   if (index != str.npos)
   {
      op = str.substr(index, 1);
      op = GetOperatorName(op);
      left = str.substr(0, index);
      right = str.substr(index+1, str.size()-index);
   }
   
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::FindOperator() returning op=%s, left=%s, right=%s\n",
       op.c_str(), left.c_str(), right.c_str());
   #endif
   
   return op;
   
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
std::string MathParser::GetOperatorName(const std::string &op)
{
   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::GetOperatorName() op=%s\n", op.c_str());
   #endif
   
   std::string opName = "--Unknown Operator--";
   
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

   #if DEBUG_PARENTHESIS
   MessageInterface::ShowMessage
      ("MathParser::GetOperatorName() opName=%s\n", opName.c_str());
   #endif
   
   return opName;
}


//------------------------------------------------------------------------------
// StringArray ParseAddSubtract(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseAddSubtract(const std::string &str)
{
   #if DEBUG_ADD_SUBTRACT
   MessageInterface::ShowMessage
      ("MathParser::ParseAddSubtract() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";
   std::string left;
   std::string right;
      
   // find first + or -
   UnsignedInt index1 = str.find('+');
   UnsignedInt index2 = str.find('-');

   if (index1 == str.npos && index2 == str.npos)
   {
      FillItems(items, "", "", "");
      #if DEBUG_ADD_SUBTRACT
      WriteItems("==> After ParseAddSubtract(): '+' or '-' not found", items);
      #endif
      return items;
   }

   
   UnsignedInt index;
   if (index1 != str.npos)
   {
      op = "Add";
      index = index1;
   }
   else if (index2 != str.npos)
   {
      op = "Subtract";
      index = index2;
   }
   
   left = str.substr(0, index);
   right = str.substr(index+1, str.npos);
   
   if (right == "")
      throw MathException("*** ERROR *** Need right side in: " + str + "\n");
   
   if (left == "") // unary operation
      FillItems(items, "", "", "");      
   else
      FillItems(items, op, left, right);

   #if DEBUG_ADD_SUBTRACT
   WriteItems("==> After ParseAddSubtract()", items);
   #endif
   
   return items;
   
}


//------------------------------------------------------------------------------
// StringArray ParseMultDivide(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseMultDivide(const std::string &str)
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::ParseMultDivide() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";
   
   // find first * or /
   UnsignedInt index1 = str.find('*');
   UnsignedInt index2 = str.find('/');

   if (index1 == str.npos && index2 == str.npos)
   {
      FillItems(items, "", "", "");
      return items;
   }

   UnsignedInt index;
   if (index1 != str.npos)
   {
      op = "Multiply";
      index = index1;
   }
   else if (index2 != str.npos)
   {
      op = "Divide";
      index = index2;
   }
   
   std::string left = str.substr(0, index);
   std::string right = str.substr(index+1, str.npos);

   if (left == "")
      throw MathException("*** ERROR *** Need left side in: " + str + "\n");
   
   if (right == "")
      throw MathException("*** ERROR *** Need right side in: " + str + "\n");
   
   FillItems(items, op, left, right);

   #if DEBUG_MATH_PARSER > 1
   WriteItems("==> After ParseMultDivide()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParsePower(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParsePower(const std::string &str)
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::ParsePower() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";
   
   // find first ^
   UnsignedInt index1 = str.find('^');
   
   if (index1 == str.npos)
   {
      FillItems(items, "", "", "");
      return items;
   }
   
   UnsignedInt index;
   if (index1 != str.npos)
   {
      op = "Power";
      index = index1;
   }
   
   std::string left = str.substr(0, index);
   std::string right = str.substr(index+1, str.npos);

   if (left == "")
      throw MathException("*** ERROR *** Need left side in: " + str + "\n");
   
   if (right == "")
      throw MathException("*** ERROR *** Need right side in: " + str + "\n");
   
   FillItems(items, op, left, right);
   
   #if DEBUG_MATH_PARSER > 1
   WriteItems("==> After ParsePower()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseUnary(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseUnary(const std::string &str)
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::ParseUnary() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string op = "";
   
   // find first -
   UnsignedInt index1 = str.find('-');
   
   if (index1 == str.npos)
   {
      FillItems(items, "", "", "");
      return items;
   }
   
   op = "Negate";
   
   //std::string left = str.substr(0, index1);
   std::string left = str.substr(index1+1, str.npos);

   FillItems(items, op, left, "");
   
   #if DEBUG_MATH_PARSER > 1
   WriteItems("==> After ParseUnary()", items);
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
   std::string fnName = GetFunction(MATH_FUNCTION, str, left);

   if (fnName == "")
   {
      FillItems(items, "", "", "");
      return items;
   }
   
   if (left == "")
      throw MathException("*** ERROR *** Need an argument in: " + str + "\n");

   FillItems(items, fnName, left, "");
   
   #if DEBUG_FUNCTION > 1
   WriteItems("==> After ParseMathFunction()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// StringArray ParseMatrixOps(const std::string &str)
//------------------------------------------------------------------------------
StringArray MathParser::ParseMatrixOps(const std::string &str)
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::ParseMatrixOps() str=%s\n", str.c_str());
   #endif
   
   StringArray items;
   std::string left;
   
   // find first math function
   std::string fnName = GetFunction(MATRIX_OPS, str, left);

   if (fnName == "")
      FillItems(items, "", "", "");
   else
      FillItems(items, fnName, left, "");
   
   #if DEBUG_MATH_PARSER > 1
   WriteItems("==> After ParseMatrixOps()", items);
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
   std::string fnName = GetFunction(UNIT_CONVERSION, str, left);

   if (fnName == "")
      FillItems(items, "", "", "");
   else
      FillItems(items, fnName, left, "");
   
   #if DEBUG_MATH_PARSER > 1
   WriteItems("==> After ParseUnitConversion()", items);
   #endif
   
   return items;
}


//------------------------------------------------------------------------------
// bool IsThisLastCharOfFunction(char ch, const std::string list[],
//                              Integer count)
//------------------------------------------------------------------------------
bool MathParser::IsThisLastCharOfFunction(char ch, const std::string list[],
                                          Integer count)
{
   for (int i=0; i<count; i++)
   {
      int last = list[i].size()-1;
      if (ch == list[i][last])
         return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// bool IsParenPartOfFunction(char lastChar)
//------------------------------------------------------------------------------
bool MathParser::IsParenPartOfFunction(char lastChar)
{
   if (IsThisLastCharOfFunction(lastChar, MATH_FUNC_LIST, MathFunctionCount) ||
       IsThisLastCharOfFunction(lastChar, MATRIX_OPS_LIST, MatrixOpsCount) ||
       IsThisLastCharOfFunction(lastChar, UNIT_CONV_LIST, UnitConvCount))
      return true;
   
   return false;
}


//------------------------------------------------------------------------------
// std::string GetFunction(Integer functionType, const std::string &str,
//                         std::string &left)
//------------------------------------------------------------------------------
std::string MathParser::GetFunction(Integer functionType, const std::string &str,
                                    std::string &left)
{
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::GetFunction() functionType=%d, str=%s\n", functionType,
       str.c_str());
   #endif
   
   std::string fnName = "";
   UnsignedInt index1 = str.npos;

   switch (functionType)
   {
   case MATH_FUNCTION:
      {
         BuildFunction(str, MATH_FUNC_LIST, MathFunctionCount, fnName, left);
         break;
      }
   case MATRIX_OPS:
      {
         BuildFunction(str, MATRIX_OPS_LIST, MatrixOpsCount, fnName, left);
         break;
      }
   case UNIT_CONVERSION:
      {
         BuildFunction(str, UNIT_CONV_LIST, UnitConvCount, fnName, left);
         break;
      }
   default:
      break;
   }
   
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::GetFunction() fnName=%s, left=%s\n", fnName.c_str(),
       left.c_str());
   #endif
   
   return fnName;
}


//------------------------------------------------------------------------------
// void BuildFunction(const std::string &str, std::string list[],
//                     Integer count, std::string &fnName, std::string &left)
//------------------------------------------------------------------------------
void MathParser::BuildFunction(const std::string &str, const std::string list[],
                               Integer count, std::string &fnName,
                               std::string &left)
{
   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage("MathParser::BuildFunction() str=%s\n", str.c_str());
   #endif
   
   UnsignedInt functionIndex = str.npos;
   
   for (int i=0; i<count; i++)
   {
      functionIndex = str.find(list[i]);
      
      // Try Capitalized function name
      if (functionIndex == str.npos)
         functionIndex = str.find(GmatStringUtil::Capitalize(list[i]));
      
      if (functionIndex != str.npos)
      {
         fnName = list[i];
         break;
      }
   }
   
   if (fnName != "")
   {
      //UnsignedInt index2 = str.find(")", index1);
      //if (index2 == str.npos)
      //   throw MathException("Need ) on " + fnName);
      
      UnsignedInt index1 = str.find("(", functionIndex);
      UnsignedInt index2 = FindMatchingParen(str, index1);

      #if DEBUG_FUNCTION
      MessageInterface::ShowMessage
         ("MathParser::BuildFunction() index1=%d, index2=%d\n", index1, index2);
      #endif
      
      left = str.substr(index1+1, index2-index1-1);
   }

   #if DEBUG_FUNCTION
   MessageInterface::ShowMessage
      ("MathParser::BuildFunction() fnName=%s, left=%s\n", fnName.c_str(), left.c_str());
   #endif
}


//------------------------------------------------------------------------------
// Integer FindMatchingParen(const std::string &str, UnsignedInt start)
//------------------------------------------------------------------------------
Integer MathParser::FindMatchingParen(const std::string &str, UnsignedInt start)
{
   #if DEBUG_MATH_PARSER > 1
   MessageInterface::ShowMessage
      ("MathParser::FindMatchingParen() str=%s, start=%d\n", str.c_str(), start);
   #endif
   
   int leftCounter = 0;
   int rightCounter = 0;
   UnsignedInt index;
   
   for (int i=start; i<str.size(); i++)
   {
      if (str[i] == '(')
         leftCounter++;

      if (str[i] == ')')
         rightCounter++;

      if (leftCounter == rightCounter)
         return i;
   }
   
   throw MathException("*** ERROR *** Unmatching parenthesis: ')' in " + str + "\n");
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
// void WriteNode(MathNode *node, Integer level)
//------------------------------------------------------------------------------
#ifdef __USE_SIMPLE_NODE__
void MathParser::WriteNode(SimpleMathNode *node, Integer level)
#else
void MathParser::WriteNode(MathNode *node, Integer level)
#endif
{
#if DEBUG_PARSE
   
   if (node == NULL)
      return;

   level++;

   if (node->GetTypeName() != "MathElement")
   {
      for (int i=0; i<level; i++)
         MessageInterface::ShowMessage("....");
      
      MessageInterface::ShowMessage
         ("node=%s: %s\n", node->GetTypeName().c_str(), node->GetName().c_str());
   }

   // Left node
   if (node->GetTypeName() != "MathElement")
   {
      #ifdef __USE_SIMPLE_NODE__
      SimpleMathNode *funcNode = node;
      #else
      MathFunction *funcNode = (MathFunction*)node;
      #endif

      if (funcNode->GetLeft())
      {
         for (int i=0; i<level; i++)
            MessageInterface::ShowMessage("....");
      
         MessageInterface::ShowMessage
            ("left=%s: %s\n", funcNode->GetLeft()->GetTypeName().c_str(),
             funcNode->GetLeft()->GetName().c_str());

         WriteNode(funcNode->GetLeft(), level);
      }
   }

   // Right node
   if (node->GetTypeName() != "MathElement")
   {
      #ifdef __USE_SIMPLE_NODE__
      SimpleMathNode *funcNode = node;
      #else
      MathFunction *funcNode = (MathFunction*)node;
      #endif
      
      if (funcNode->GetRight())
      {
         for (int i=0; i<level; i++)
            MessageInterface::ShowMessage("....");
      
         MessageInterface::ShowMessage
            ("right=%s: %s\n", funcNode->GetRight()->GetTypeName().c_str(),
             funcNode->GetRight()->GetName().c_str());

         WriteNode(funcNode->GetRight(), level);
      }
   }
   
#endif
}
   


