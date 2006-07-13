//$Header$
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

namespace GmatStringUtil
{
   enum StripType
   {
      LEADING = 1,
      TRAILING = 2,
      BOTH = 3,
   };
   
   std::string RemoveAll(const std::string &str, char ch, Integer start = 0);
   std::string Trim(const std::string &str, StripType stype = TRAILING,
                    bool removeSemicolon = false);
   std::string Strip(const std::string &str, StripType stype = TRAILING);
   std::string ToUpper(const std::string &str);
   std::string ToLower(const std::string &str);
   std::string Capitalize(const std::string &str);
   std::string ToString(const Real &val, bool scientific=false,
                        Integer width=10, Integer precision=9);
   std::string ToString(const Integer &val, Integer width=4);
   bool ToDouble(const std::string &str, Real *value);
   bool ToDouble(const std::string &str, Real &value);
   bool ToInteger(const std::string &str, Integer *value);
   bool ToInteger(const std::string &str, Integer &value);
   void ParseParameter(const std::string &str, std::string &type,
                       std::string &owner, std::string &depObj);
   void GetArrayIndex(const std::string &str, Integer &row, Integer &col,
                      std::string &name);
   void GetArrayIndex(const std::string &str, std::string &rowStr,
                      std::string &colStr, Integer &row, Integer &col,
                      std::string &name);
   void GmatStringUtil::GetArrayIndexVar(const std::string &str, std::string &rowStr,
                                         std::string &colStr, std::string &name);
   void FindFirstAndLast(const std::string &str, char ch, Integer &first,
                         Integer &last);
   void FindParenMatch(const std::string &str, Integer &open, Integer &close,
                       bool &isOuterParen);
}

#endif // StringUtil_hpp
