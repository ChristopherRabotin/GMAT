//$Header$
//------------------------------------------------------------------------------
//                                  ExpressionParser
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/09/15
//
/**
 * Implements ExpressionParser class which parses and evaluates the
 * expression. The valid expressions are composed of the following items:
 *    1. Numbers
 *    2. Operators(+,-,/,*,^)
 *    3. Parentheses
 *    4. Varialbes
 *
 * Assume this precendence of each operator:
 *    highest   + - (unary)
 *              ^ (exponentiation)
 *              * /
 *              + -
 *
 * Parser Code Reference: "The Complete Reference C++" by Herbert Schildt,
 *    4th Edition, P963-997
 */
//------------------------------------------------------------------------------

#include "ExpressionParser.hpp"
#include "ParameterException.hpp"
#include "MessageInterface.hpp"
#include <iostream>
using namespace std;

//#define DEBUG_EXP_PARSER 1

//---------------------------------
// static varible
//---------------------------------
ExpressionParser* ExpressionParser::theExpressionParser = NULL;

//---------------------------------
// public methods
//---------------------------------


//------------------------------------------------------------------------------
// ExpressionParser()
//------------------------------------------------------------------------------
ExpressionParser::ExpressionParser()
{
   mExp = NULL;
   mParamDb = NULL;
   
   for (int i=0; i<NUM_VARS; i++)
      mVars[i] = 0.0;
}

//------------------------------------------------------------------------------
// ~ExpressionParser()
//------------------------------------------------------------------------------
ExpressionParser::~ExpressionParser()
{
}
   
//------------------------------------------------------------------------------
// Real EvalExp(const char *exp)
//------------------------------------------------------------------------------
/*
 * Evaluates given expression and return results.
 *
 * @param <exp> null terminated expression
 *
 * @return expression evaluated results
 */
//------------------------------------------------------------------------------
Real ExpressionParser::EvalExp(const char *exp)
{
   Real result;
   mExp = exp;
   *mToken = '\0';
   
#if DEBUG_EXP_PARSER
   MessageInterface::ShowMessage
      ("ExpressionParser::EvalExp() exp=%s\n", exp);
#endif
   
   GetToken();

   if (!*mToken)
   {
      HandleSyntaxError(NO_EXPRESSION_PRESENT);
      return 0.0;
   }

   EvalTwoTerms(result);

   if (*mToken)
      HandleSyntaxError(SYNTAX_ERROR); // last token must be null

   
   return result;
}

//------------------------------------------------------------------------------
// void SetParameterDatabase(ParameterDatabase *pdb)
//------------------------------------------------------------------------------
/*
 * Sets ParameterDatabase pointer.
 *
 * @param <pdb> ParameterDatabase pointer to set to
 *
 */
//------------------------------------------------------------------------------
void ExpressionParser::SetParameterDatabase(ParameterDatabase *pdb)
{
   mParamDb = pdb;
}

//---------------------------------
// private methods
//---------------------------------
 
//------------------------------------------------------------------------------
// void EvalTwoTerms(Real &result)
//------------------------------------------------------------------------------
/*
 * Add or subtract two terms.
 */
//------------------------------------------------------------------------------
void ExpressionParser::EvalTwoTerms(Real &result)
{
   register char op; // it's heavily used
   Real temp;

   EvalTwoFactors(result);

   while ((op = *mToken) == '+' || op == '-')
   {
      GetToken();
      EvalTwoFactors(temp);

      switch (op)
      {
      case '-':
         result = result - temp;
         break;
      case '+':
         result = result + temp;
         break;
      default:
         break;
      }
   }
}

//------------------------------------------------------------------------------
// void EvalTwoFactors(Real &result)
//------------------------------------------------------------------------------
/*
 * Multiply or divide two factors.
 */
//------------------------------------------------------------------------------
void ExpressionParser::EvalTwoFactors(Real &result)
{
   register char op; // it's heavily used
   Real temp;

   EvalExponent(result);

   while ((op = *mToken) == '*' || op == '/' || op == '%')
   {
      GetToken();
      EvalExponent(temp);

      switch (op)
      {
      case '*':
         result = result * temp;
         break;
      case '/':
         result = result / temp;
         break;
      default:
         break;
      }
   }
}

//------------------------------------------------------------------------------
// void EvalExponent(Real &result)
//------------------------------------------------------------------------------
/*
 * Process exponent.
 */
//------------------------------------------------------------------------------
void ExpressionParser::EvalExponent(Real &result)
{
   register int t;
   Real temp, ex;

   EvalUnary(result);

   if (*mToken == '^')
   {
      GetToken();
      EvalExponent(temp);
      ex = result;

      if (temp == 0.0)
      {
         result = 1.0;
         return;
      }

      for (t=(int)temp-1; t>0; --t)
         result = result * (Real)ex;
   }
}

//------------------------------------------------------------------------------
// void EvalUnary(Real &result)
//------------------------------------------------------------------------------
/*
 * Evaluates a unary + or -.
 */
//------------------------------------------------------------------------------
void ExpressionParser::EvalUnary(Real &result)
{
   register char op;

   op = 0;

   if ((mTokenType == DELIMITER) && *mToken == '+' || *mToken == '-')
   {
      op = *mToken;
      GetToken();
   }

   EvalParenExp(result);

   if (op == '-')
      result = - result;
}

//------------------------------------------------------------------------------
//void EvalParenExp(Real &result)
//------------------------------------------------------------------------------
/*
 * Evaluates a parenthesized expression.
 */
//------------------------------------------------------------------------------
void ExpressionParser::EvalParenExp(Real &result)
{
   if (*mToken == '(')
   {
      GetToken();

      EvalTwoTerms(result);

      if (*mToken != ')')
         HandleSyntaxError(UNBALANCED_PARENTHESES);

      GetToken();
   }
   else
   {
      GetValue(result);
   }
}

//------------------------------------------------------------------------------
// void GetValue(Real &result)
//------------------------------------------------------------------------------
/*
 * Get the value of a number or variable.
 */
//------------------------------------------------------------------------------
void ExpressionParser::GetValue(Real &result)
{
#if DEBUG_EXP_PARSER
   MessageInterface::ShowMessage
      ("ExpressionParser::GetValue() mToken=%s\n", mToken);
#endif
   
   switch (mTokenType)
   {
   case VARIABLE:
      result = EvalVariable(mToken);
      GetToken();
      return;
   case NUMBER:
      result = atof(mToken);
      GetToken();
      return;
   default:
      HandleSyntaxError(SYNTAX_ERROR);
   }
}

//------------------------------------------------------------------------------
// Real EvalVariable(char *var)
//------------------------------------------------------------------------------
/*
 * Get the value of a variable
 */
//------------------------------------------------------------------------------
Real ExpressionParser::EvalVariable(char *var)
{
   if (mParamDb == NULL)
   {
      throw ParameterException
         ("ExpressionParser::EvalVariable() Associated ParameterDatabase is NULL.\n"
          "Make sure to call SetParameterDatabase() in EvaluateReal() of variable.\n");
   }

   std::string varName = std::string(var);
   Parameter *param = mParamDb->GetParameter(varName);

#if DEBUG_EXP_PARSER
   MessageInterface::ShowMessage
      ("ExpressionParser::EvalVariable() varName:%s\n", varName.c_str());
   //StringArray paramNames = mParamDb->GetNamesOfParameters();
   //for (int i = 0; i<mParamDb->GetNumParameters(); i++)
   //   MessageInterface::ShowMessage
   //      ("ExpressionParser::EvalVariable() In mParamDb: %s\n", paramNames[i].c_str());
#endif
   
   if (param != NULL)
   {
      return param->EvaluateReal();
   }
   else
   {
      throw ParameterException
         ("ExpressionParser::EvalVariable() Requested parameter: " + varName +
          " has NULL pointer. Make sure to call SetRefObject() of variable.\n");
   }
   
}

//------------------------------------------------------------------------------
// void GetToken()
//------------------------------------------------------------------------------
/*
 * Obtain the next token.
 */
//------------------------------------------------------------------------------
void ExpressionParser::GetToken()
{
   register char *temp;

   mTokenType = NUMBER;
   temp = mToken;
   *temp = '\0';

   if (!*mExp)
      return; // end of expression

   while (isspace(*mExp))
      ++mExp; // skip over white spaces

   if (strchr("+-*/^=()", *mExp))
   {
      mTokenType = DELIMITER;
      *temp++ = *mExp++; // advance to next char
   }
   else if (isalpha(*mExp))
   {
      while (!IsDelimiter(*mExp))
         *temp++ = *mExp++;
      
      mTokenType = VARIABLE;
   }
   //else if (isdigit(*mExp))
   else if (isdigit(*mExp) || *mExp == '.') //loj: 7/15/05 Added check for .
   {
      while (!IsDelimiter(*mExp))
         *temp++ = *mExp++;
      
      mTokenType = NUMBER;
   }

   *temp = '\0';
}

//------------------------------------------------------------------------------
// bool IsDelimiter(char c)
//------------------------------------------------------------------------------
/*
 * Return true if c is a delimiter.
 */
//------------------------------------------------------------------------------
bool ExpressionParser::IsDelimiter(char c)
{
   if (strchr(" +-/*^=()", c) || c==9 || c=='\r' || c==0)
      return true;
   else
      return false;
}

//------------------------------------------------------------------------------
// void HandleSyntaxError(int error)
//------------------------------------------------------------------------------
/*
 * Display a syntax error.
 */
//------------------------------------------------------------------------------
void ExpressionParser::HandleSyntaxError(int error)
{
   static char *errMsg[] =
   {
      "Syntax Error\n",
      "Unbalanced Parentheses\n",
      "No expression Present\n",
   };

   throw ParameterException(std::string(errMsg[error]));
}
