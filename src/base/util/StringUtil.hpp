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
   
   std::string RemoveAll(const std::string &str, char ch);
   std::string Trim(const std::string &str, StripType stype = TRAILING);
   std::string Strip(const std::string &str, StripType stype = TRAILING);
   bool ToDouble(const std::string &str, Real *value);
   std::string ToUpper(const std::string &str);
   std::string ToLower(const std::string &str);
   std::string Capitalize(const std::string &str);
}

#endif // StringUtil_hpp
