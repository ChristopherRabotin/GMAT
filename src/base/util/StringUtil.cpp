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

#include "StringUtil.hpp"

//#define DEBUG_STRING_UTIL 1
#if DEBUG_STRING_UTIL
#include "MessageInterface.hpp"
#endif

using namespace std;

//------------------------------------------------------------------------------
// std::string Trim(const std::string &str, StripType stype = TRAILING)
//------------------------------------------------------------------------------
std::string GmatStringUtil::Trim(const std::string &str, StripType stype)
{
   std::string str2;

   UnsignedInt index1 = str.find_first_not_of(' ');
   UnsignedInt index2 = str.find_last_not_of(' ');

   if (index1 == str.npos)
      index1 = 0;

   if (stype == LEADING)
      str2 = str.substr(index1);
   else if (stype == TRAILING)
      str2 = str.substr(0, index2);
   else if (stype == BOTH)
      str2.assign(str.substr(index1, index2-index1+1));

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("==> index1=%d, index2=%d str=<%s>, str2=<%s>\n", index1, index2,
       str.c_str(), str2.c_str());
   #endif
   
   return str2;
}


//------------------------------------------------------------------------------
// std::string Strip(const std::string &str, StripType stype = TRAILING)
//------------------------------------------------------------------------------
std::string GmatStringUtil::Strip(const std::string &str, StripType stype)
{
   return Trim(str, stype);
}


//------------------------------------------------------------------------------
// bool ToDouble(const std::string &str, Real *value)
//------------------------------------------------------------------------------
// Note: atof() returns 100.00 for 100.00ABC, but we want it be an error
//------------------------------------------------------------------------------
bool GmatStringUtil::ToDouble(const std::string &str, Real *value)
{
   std::string str2 = Trim(str, BOTH);
   Integer numDot = 0;
   
   if (str2.length() == 0)
      return false;

   if (str2[0] != '-' && !isdigit(str2[0]))
      return false;

   for (unsigned int i=0; i<str2.length(); i++)
   {
      if (i == 0 && str2[0] == '-')
         continue;
      
      if (str2[i] == '.')
      {
         numDot++;

         if (numDot > 1)
            return false;
         
         continue;
      }
      
      if (!isdigit(str2[i]))
         return false;
   }
   
   *value = atof(str2.c_str());
   return true;
}


//------------------------------------------------------------------------------
// std::string ToUpper(const std::string &str)
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToUpper(const std::string &str)
{
   Integer len = str.length();
   
   if (len == 0)
      return str;

   std::string upper = str;
   for (int i=0; i<len; i++)
      upper[i] = toupper(str[i]);

   return upper;
}
