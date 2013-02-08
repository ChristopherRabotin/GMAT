//$Id$
//------------------------------------------------------------------------------
//                                 StringUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2006/1/6
//
/**
 * This file provides string utility.
 */
//------------------------------------------------------------------------------
#ifndef StringUtil_hpp
#define StringUtil_hpp

#include "gmatdefs.hpp"
#include "GmatGlobal.hpp"

namespace GmatStringUtil
{
   enum StripType
   {
      LEADING = 1,
      TRAILING = 2,
      BOTH = 3,
   };
   
   GMAT_API std::string RemoveAll(const std::string &str, char ch, Integer start = 0);
   GMAT_API std::string RemoveAll(const std::string &str, const std::string &removeStr,
                           Integer start = 0);
   GMAT_API std::string RemoveAllBlanks(const std::string &str, bool ignoreSingleQuotes = false);
   GMAT_API std::string RemoveLastNumber(const std::string &str, Integer &lastNumber);
   GMAT_API std::string RemoveLastString(const std::string &str, const std::string &lastStr,
                           bool removeAll = false);
   GMAT_API std::string RemoveSpaceInBrackets(const std::string &str,
                           const std::string &bracketPair);
   GMAT_API std::string RemoveMultipleSpaces(const std::string &str);
   GMAT_API std::string RemoveTrailingZeros(Real val, const std::string &valStr,
                           Integer iterCount = 0);
   GMAT_API std::string RemoveScientificNotation(const std::string &str);
   GMAT_API std::string RemoveMathSymbols(const std::string &str, bool removeMathOperator = false);
   
   GMAT_API std::string Trim(const std::string &str, StripType stype = BOTH,
                           bool removeSemicolon = false, bool removeEol = false);
   GMAT_API std::string Strip(const std::string &str, StripType stype = BOTH);
   GMAT_API std::string ToUpper(const std::string &str, bool firstLetterOnly = false);
   GMAT_API std::string ToLower(const std::string &str, bool firstLetterOnly = false);
   GMAT_API std::string Capitalize(const std::string &str);
   GMAT_API std::string Replace(const std::string &str, const std::string &from,
                           const std::string &to);
   GMAT_API std::string ReplaceName(const std::string &str, const std::string &from,
                           const std::string &to);
   GMAT_API std::string ReplaceNumber(const std::string &str, const std::string &from,
                           const std::string &to);
   GMAT_API std::string ReplaceChainedUnaryOperators(const std::string &str);
   
   GMAT_API std::string ToString(const bool &val);
   GMAT_API std::string ToString(const Real &val, Integer precision, bool showPoint = false,
                           Integer width = 1);
   GMAT_API std::string ToString(const Integer &val, Integer width);
   
   GMAT_API std::string ToString(const Real &val, bool useCurrentFormat = true,
                           bool scientific = false, bool showPoint = true, 
                           Integer precision = GmatGlobal::DATA_PRECISION,
                           Integer width = GmatGlobal::DATA_WIDTH);
   GMAT_API std::string ToString(const Integer &val, bool useCurrentFormat = true,
                           Integer width = GmatGlobal::INTEGER_WIDTH);
   GMAT_API std::string ToStringNoZeros(const Real &val);
   
   GMAT_API std::string RemoveExtraParen(const std::string &str, bool ignoreComma = false,
                                         bool ignoreSingleQuotes = false);
   GMAT_API std::string RemoveOuterParen(const std::string &str);
   GMAT_API std::string RemoveOuterString(const std::string &str, const std::string &start,
                           const std::string &end);
   GMAT_API std::string RemoveEnclosingString(const std::string &str, const std::string &enStr);
   GMAT_API std::string RemoveInlineComment(const std::string &str, const std::string &cmStr);
   GMAT_API std::string MakeCommentLines(const std::string &str, bool breakAtCr = false);
   GMAT_API std::string ParseFunctionName(const std::string &str);
   GMAT_API std::string AddEnclosingString(const std::string &str, const std::string &enStr);
   GMAT_API std::string GetInvalidNameMessageFormat();
   
   GMAT_API char GetClosingBracket(const char &openBracket);
   
   GMAT_API StringArray SeparateBy(const std::string &str, const std::string &delim,
                           bool putBracketsTogether = false, bool insertDelim = false,
                           bool insertComma = true);
   GMAT_API StringArray SeparateByComma(const std::string &str, bool checkSingleQuote = true);
   GMAT_API StringArray SeparateDots(const std::string &str);
   GMAT_API StringArray DecomposeBy(const std::string &str, const std::string &delim);
   
   GMAT_API bool IsNumber(const std::string &str);
   GMAT_API bool IsValidReal(const std::string &str, Real &value, Integer &errorCode,
                             bool trimParens = false, bool allowOverflow = true);
   GMAT_API bool ToReal(const std::string &str, Real *value, bool trimParens = false, bool allowOverflow = true);
   GMAT_API bool ToReal(const std::string &str, Real &value, bool trimParens = false, bool allowOverflow = true);
   GMAT_API bool ToInteger(const std::string &str, Integer *value, bool trimParens = false, bool allowOverflow = true);
   GMAT_API bool ToInteger(const std::string &str, Integer &value, bool trimParens = false, bool allowOverflow = true);
   GMAT_API bool ToUnsignedInt(const std::string &str, UnsignedInt *value, bool trimParens = false, bool allowOverflow = true);
   GMAT_API bool ToUnsignedInt(const std::string &str, UnsignedInt &value, bool trimParens = false, bool allowOverflow = true);
   GMAT_API bool ToBoolean(const std::string &str, bool *value, bool trimParens = false);
   GMAT_API bool ToBoolean(const std::string &str, bool &value, bool trimParens = false);
   GMAT_API bool ToOnOff(const std::string &str, std::string &value, bool trimParens = false);
   
   GMAT_API RealArray ToRealArray(const std::string &str, bool allowOverflow = true);
   GMAT_API IntegerArray ToIntegerArray(const std::string &str, bool allowOverflow = true);
   GMAT_API UnsignedIntArray ToUnsignedIntArray(const std::string &str, bool allowOverflow = true);
   GMAT_API StringArray ToStringArray(const std::string &str);
   GMAT_API BooleanArray ToBooleanArray(const std::string &str);
   
   GMAT_API void ParseParameter(const std::string &str, std::string &type,
                           std::string &owner, std::string &dep);
   GMAT_API void GetArrayCommaIndex(const std::string &str, Integer &comma,
                           const std::string &bracketPair = "()");
   GMAT_API void GetArrayIndexVar(const std::string &str, std::string &rowStr,
                           std::string &colStr, std::string &name,
                           const std::string &bracketPair = "()");
   GMAT_API void GetArrayIndex(const std::string &str, Integer &row, Integer &col,
                           std::string &name, const std::string &bracketPair = "()");
   GMAT_API void GetArrayIndex(const std::string &str, std::string &rowStr,
                           std::string &colStr, Integer &row, Integer &col,
                           std::string &name, const std::string &bracketPair = "()");
   GMAT_API std::string GetArrayName(const std::string &str,
                           const std::string &bracketPair = "()");
   GMAT_API bool IsOneElementArray(const std::string &str);
   GMAT_API bool IsSimpleArrayElement(const std::string &str);
   
   GMAT_API void FindFirstAndLast(const std::string &str, char ch, Integer &first,
                           Integer &last);
   GMAT_API void FindParenMatch(const std::string &str, Integer &open, Integer &close,
                           bool &isOuterParen);
   GMAT_API void FindMatchingParen(const std::string &str, Integer &openParen,
                           Integer &closeParen, bool &isOuterParen,
                           Integer start = 0);
   GMAT_API void FindMatchingBracket(const std::string &str, Integer &openBracket,
                           Integer &closeBracket, bool &isOuterBracket,
                           const std::string &bracket, Integer start = 0);
   GMAT_API void FindLastParenMatch(const std::string &str, Integer &openParen,
                           Integer &closeParen, Integer start = 0);
   
   GMAT_API bool IsEnclosedWith(const std::string &str, const std::string &enclosingStr);
   GMAT_API bool IsEnclosedWithExtraParen(const std::string &str, bool checkOps = true,
                                          bool ignoreComma = false);
   GMAT_API bool IsEnclosedWithBraces(const std::string &str);
   GMAT_API bool IsEnclosedWithBrackets(const std::string &str);
   GMAT_API bool IsBracketBalanced(const std::string &str, const std::string &bracketPair);
   GMAT_API bool IsParenBalanced(const std::string &str);
   GMAT_API bool IsParenEmpty(const std::string &str);
   GMAT_API bool AreAllBracketsBalanced(const std::string &str, const std::string &allPairs);
   GMAT_API bool AreAllNamesValid(const std::string &str, bool blankNameIsOk = false);
   GMAT_API bool IsOuterParen(const std::string &str);
   GMAT_API bool IsCommaPartOfArray(const std::string &str, Integer start = 0);
   GMAT_API bool IsBracketPartOfArray(const std::string &str, const std::string &bracketPairs,
                           bool checkOnlyFirst);
   GMAT_API bool IsParenPartOfArray(const std::string &str);
   GMAT_API bool IsThereEqualSign(const std::string &str);
   GMAT_API bool IsThereMathSymbol(const std::string &str);
   GMAT_API bool IsThereScientificNotation(const std::string &str);
   GMAT_API bool HasNoBrackets(const std::string &str, bool parensForArraysAllowed = true);
   GMAT_API bool IsSingleItem(const std::string &str);
   GMAT_API bool StartsWith(const std::string &str, const std::string &value);
   GMAT_API bool EndsWith(const std::string &str, const std::string &value);
   GMAT_API bool EndsWithPathSeparator(const std::string &str);
   GMAT_API bool IsValidNumber(const std::string &str, bool allowOverflow = true);
   GMAT_API bool IsValidName(const std::string &str, bool ignoreBracket = false,
                             bool blankNameIsOk = false);
   GMAT_API bool IsValidParameterName(const std::string &str);
   GMAT_API bool IsLastNumberPartOfName(const std::string &str);
   GMAT_API bool IsBlank(const std::string &str, bool ignoreEol = false);
   GMAT_API bool HasMissingQuote(const std::string &str, const std::string &quote,
                                 bool ignoreSpaceAfterQuote = true);
   GMAT_API bool IsMathEquation(const std::string &str, bool checkInvalidOpOnly = false,
                                bool blankNameIsOk = false);
   GMAT_API bool IsMathOperator(const char &ch);
   
   GMAT_API Integer NumberOfOccurrences(const std::string &str, const char c);
   
   GMAT_API StringArray GetVarNames(const std::string &str);
   GMAT_API void WriteStringArray(const StringArray &strArray,
                                  const std::string &desc = "",
                                  const std::string &prefix = "");
   
}

#endif // StringUtil_hpp

