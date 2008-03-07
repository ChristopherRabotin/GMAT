//$Id$
//------------------------------------------------------------------------------
//                                 StringUtil
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
   
   std::string RemoveAll(const std::string &str, char ch, Integer start = 0);
   std::string RemoveLastNumber(const std::string &str, Integer &lastNumber);
   
   std::string RemoveSpaceInBrackets(const std::string &str,
                                     const std::string &bracketPair);
   std::string Trim(const std::string &str, StripType stype = BOTH,
                    bool removeSemicolon = false, bool removeEol = false);
   std::string Strip(const std::string &str, StripType stype = BOTH);
   std::string ToUpper(const std::string &str);
   std::string ToLower(const std::string &str);
   std::string Capitalize(const std::string &str);
   std::string Replace(const std::string &str, const std::string &from,
                       const std::string &to);
   std::string ReplaceName(const std::string &str, const std::string &from,
                           const std::string &to);
   
   std::string ToString(const Real &val, Integer precision);
   std::string ToString(const Integer &val, Integer width);
   
   std::string ToString(const Real &val, bool useCurrentFormat = true,
                        bool scientific = false, bool showPoint = true, 
                        Integer precision = GmatGlobal::DATA_PRECISION,
                        Integer width = GmatGlobal::DATA_WIDTH);
   std::string ToString(const Integer &val, bool useCurrentFormat = true,
                        Integer width = GmatGlobal::INTEGER_WIDTH);
   
   std::string RemoveExtraParen(const std::string &str);
   
   char GetClosingBracket(const char &openBracket);
   
   StringArray SeparateBy(const std::string &str, const std::string &delim,
                          bool putBracketsTogether = false);
   
   bool ToReal(const std::string &str, Real *value, bool trimParens = false);
   bool ToReal(const std::string &str, Real &value, bool trimParens = false);
   bool ToInteger(const std::string &str, Integer *value, bool trimParens = false);
   bool ToInteger(const std::string &str, Integer &value, bool trimParens = false);
   bool ToBoolean(const std::string &str, bool *value, bool trimParens = false);
   bool ToBoolean(const std::string &str, bool &value, bool trimParens = false);
   
   void ParseParameter(const std::string &str, std::string &type,
                       std::string &owner, std::string &dep);
   void GetArrayCommaIndex(const std::string &str, Integer &comma,
                           const std::string &bracketPair = "()");
   void GetArrayIndex(const std::string &str, Integer &row, Integer &col,
                      std::string &name, const std::string &bracketPair = "()");
   void GetArrayIndex(const std::string &str, std::string &rowStr,
                      std::string &colStr, Integer &row, Integer &col,
                      std::string &name, const std::string &bracketPair = "()");
   void GetArrayIndexVar(const std::string &str, std::string &rowStr,
                         std::string &colStr, std::string &name,
                         const std::string &bracketPair = "()");
   void FindFirstAndLast(const std::string &str, char ch, Integer &first,
                         Integer &last);
   void FindParenMatch(const std::string &str, Integer &open, Integer &close,
                       bool &isOuterParen);
   void FindMatchingParen(const std::string &str, Integer &openParen,
                          Integer &closeParen, bool &isOuterParen,
                          Integer start = 0);
   void FindMatchingBracket(const std::string &str, Integer &openBracket,
                            Integer &closeBracket, bool &isOuterBracket,
                            const std::string &bracket, Integer start = 0);
   void FindLastParenMatch(const std::string &str, Integer &openParen,
                           Integer &closeParen, Integer start = 0);
   
   bool IsEnclosedWithExtraParen(const std::string &str, bool checkOps = true);
   bool IsBracketBalanced(const std::string &str, const std::string &bracketPair);
   bool IsParenBalanced(const std::string &str);
   bool AreAllBracketsBalanced(const std::string &str, const std::string &allPairs);
   bool IsOuterParen(const std::string &str);
   bool IsCommaPartOfArray(const std::string &str, Integer start = 0);
   bool IsBracketPartOfArray(const std::string &str, const std::string &bracketPairs,
                             bool checkOnlyFirst);
   bool IsParenPartOfArray(const std::string &str);
   bool HasNoBrackets(const std::string &str, bool parensForArraysAllowed = true);
   bool IsSingleItem(const std::string &str);
   bool StartsWith(const std::string &str, const std::string &value);
   bool EndsWith(const std::string &str, const std::string &value);
   bool IsValidName(const std::string &str, bool isObject = false);
   bool IsBlank(const std::string &str, bool ignoreEol = false);
   
   Integer NumberOfOccurrences(const std::string str, const char c);
}

#endif // StringUtil_hpp

