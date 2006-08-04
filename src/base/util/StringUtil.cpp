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
#include "MessageInterface.hpp"
#include "GmatBaseException.hpp"
#include "Linear.hpp"            // for ToString()
#include <map>

//#define DEBUG_STRING_UTIL 1

using namespace std;


//------------------------------------------------------------------------------
// std::string RemoveAll(const std::string &str, char ch, Integer start = 0)
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveAll(const std::string &str, char ch,
                                      Integer start)
{
   std::string str2 = str;

   std::string::iterator iter = str2.begin();
   for (int i=0; i<start; i++)
      iter++;
   
   while (iter != str2.end())
   {
      if (*iter == ch)
         str2.erase(iter);
      else
         ++iter;
   }

   return str2;
}


//------------------------------------------------------------------------------
// std::string Trim(const std::string &str, StripType stype = TRAILING,
//                  bool removeSemicolon = false)
//------------------------------------------------------------------------------
std::string GmatStringUtil::Trim(const std::string &str, StripType stype,
                                 bool removeSemicolon)
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

   if (removeSemicolon)
      if (str2[str2.size()-1] == ';')
         str2.erase(str2.size()-1, 1);
   
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


//------------------------------------------------------------------------------
// std::string ToLower(const std::string &str)
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToLower(const std::string &str)
{
   Integer len = str.length();
   
   if (len == 0)
      return str;

   std::string lower = str;
   for (int i=0; i<len; i++)
      lower[i] = tolower(str[i]);

   return lower;
}


//------------------------------------------------------------------------------
// std::string Capitalize(const std::string &str)
//------------------------------------------------------------------------------
std::string GmatStringUtil::Capitalize(const std::string &str)
{
   Integer len = str.length();
   
   if (len == 0)
      return str;

   std::string newstr = str;
   if (newstr.length() == 0)
      return str;
   
   newstr = Trim(newstr, BOTH);
   newstr[0] = toupper(newstr[0]);

   return newstr;
}


//------------------------------------------------------------------------------
// std::string Replace(const std::string &str, const std::string &from,
//                     const std::string &to)
//------------------------------------------------------------------------------
/*
 * Replaces all occurenece of <from> string to <to> string.
 *
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::Replace(const std::string &str, const std::string &from,
                                    const std::string &to)
{
   std::string str1 = str;
   std::string::size_type pos = str1.find(from);
   bool done = false;
   
   while (!done)
   {
      pos = str1.find(from);
      if (pos != str1.npos)
         str1.replace(pos, from.size(), to);
      else
         break;
   }
   
   return str1;
}


//------------------------------------------------------------------------------
// std::string ToString(const Integer &val, Integer width=3)
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const Integer &val, Integer width)
{
   return GmatRealUtil::ToString(val, width);
//    std::stringstream ss("");
//    ss.width(width);
//    ss << val;
//    return std::string(ss.str());
}


//------------------------------------------------------------------------------
// std::string ToString(const Real &val, bool scientific=false,
//                      Integer width=10, Integer precision=9)
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const Real &val, bool scientific,
                                     Integer width, Integer precision)
{
   return GmatRealUtil::ToString(val, scientific, width, precision);
//    std::stringstream ss("");
//    ss.width(width);
//    ss.precision(precision);
//    if (scientific)
//       ss.setf(std::ios::scientific);
   
//    ss << val;
//    return std::string(ss.str());
}


//------------------------------------------------------------------------------
// bool ToDouble(const std::string &str, Real *value)
//------------------------------------------------------------------------------
bool GmatStringUtil::ToDouble(const std::string &str, Real *value)
{
   return ToDouble(str, *value);
}


//------------------------------------------------------------------------------
// bool ToDouble(const std::string &str, Real &value)
//------------------------------------------------------------------------------
// Note: atof() returns 100.00 for 100.00ABC, but we want it be an error
//------------------------------------------------------------------------------
bool GmatStringUtil::ToDouble(const std::string &str, Real &value)
{
   std::string str2 = Trim(str, BOTH);
   Integer numDot = 0;
   
   if (str2.length() == 0)
      return false;

   if (str2[0] != '-' && !isdigit(str2[0]) && str2[0] != '.')
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
      {
         // Handle scientific notation
         if ((str2[i] == 'e' || str2[i] == 'E') &&
             (str2[i+1] == '+' || str2[i+1] == '-'))
            continue;
         
         if ((str2[i] == '+' || str2[i] == '-') &&
             (str2[i-1] == 'e' || str2[i-1] == 'E'))
            continue;

         return false;
      }
   }
   
   value = atof(str2.c_str());
   return true;
}


//------------------------------------------------------------------------------
// bool GmatStringUtil::ToInteger(const std::string &str, Integer *value)
//------------------------------------------------------------------------------
bool GmatStringUtil::ToInteger(const std::string &str, Integer *value)
{
   return ToInteger(str, *value);
}

//------------------------------------------------------------------------------
// bool ToInteger(const std::string &str, Real *value)
//------------------------------------------------------------------------------
// Note: atoi() returns 0 for X or 100 for 100ABC, but we want it be an error
//------------------------------------------------------------------------------
bool GmatStringUtil::ToInteger(const std::string &str, Integer &value)
{
   std::string str2 = Trim(str, BOTH);
   
   if (str2.length() == 0)
      return false;

   if (str2[0] != '-' && !isdigit(str2[0]))
      return false;

   for (unsigned int i=0; i<str2.length(); i++)
   {
      if (i == 0 && str2[0] == '-')
         continue;
      
      if (!isdigit(str2[i]))
         return false;
   }
   
   value = atoi(str2.c_str());
   return true;
}


//------------------------------------------------------------------------------
// void ParseParameter(const std::string &str, std::string &type,
//                     std::string &owner, std::string &depobj)
//------------------------------------------------------------------------------
void GmatStringUtil::ParseParameter(const std::string &str, std::string &type,
                                    std::string &owner, std::string &depObj)
{
   //find owner.depObj.type
   std::string str1 = str;
   std::string::size_type pos1 = str1.find(".");
   std::string::size_type pos2 = str1.find_last_of(".");
   
   if (pos1 != str1.npos && pos2 != str1.npos)
   {
      owner = str1.substr(0, pos1);
      type = str1.substr(pos2+1, str1.npos-pos2);

      depObj = "";
      if (pos2 > pos1)
         depObj = str1.substr(pos1+1, pos2-pos1-1);
   }

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("GmatStringUtil::ParseParameter() str=%s, type=%s, owner=%s, depObj=%s\n",
       str.c_str(), type.c_str(), owner.c_str(), depObj.c_str());
   #endif
   
}


//------------------------------------------------------------------------------
// void GetArrayIndex(const std::string &str, Integer &row, Integer &col,
//                    std::string &name)
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayIndex(const std::string &str, Integer &row,
                                   Integer &col, std::string &name)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("Report::GetArrayIndex() str=%s\n", str.c_str());
   #endif

   std::string rowStr;
   std::string colStr;
   row = -1;
   col = -1;
   Integer intVal;
   
   GetArrayIndexVar(str, rowStr, colStr, name);
   
   // array index starts at 0
   if (rowStr != "-1")
      if (ToInteger(rowStr, intVal))
         row = intVal - 1;
   
   if (colStr != "-1")
      if (ToInteger(colStr, intVal))
         col = intVal - 1;
   
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("Report::GetArrayIndex() row=%d, col=%d, name=%s\n", row, col, name.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void GetArrayIndex(const std::string &str, std::string &rowStr,
//                    std::string &colStr, Integer &row, Integer &col,
//                    std::string &name)
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayIndex(const std::string &str, std::string &rowStr,
                                   std::string &colStr, Integer &row, Integer &col,
                                   std::string &name)
{
   row = -1;
   col = -1;
   Integer intVal;
   
   GetArrayIndexVar(str, rowStr, colStr, name);
   
   // array index starts at 0
   if (rowStr != "-1")
      if (ToInteger(rowStr, intVal))
         row = intVal - 1;
   
   if (colStr != "-1")
      if (ToInteger(colStr, intVal))
         col = intVal - 1;
}


//------------------------------------------------------------------------------
// void GetArrayIndexVar(const std::string &str, std::string &rowStr,
//                       std::string &colStr, std::string &name)
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayIndexVar(const std::string &str, std::string &rowStr,
                                      std::string &colStr, std::string &name)
{
   std::string str1;
   str1 = RemoveAll(str, ' ');
   
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("GmatStringUtil::GetArrayIndexVar() str=%s, str1=%s\n", str.c_str(), str1.c_str());
   #endif

   rowStr = "-1";
   colStr = "-1";
   
   // Handle Array indexing
   UnsignedInt openParen = str1.find('(');
   if (openParen != str1.npos)
   {
      UnsignedInt comma = str1.find(',');
      if (comma == str1.npos)
         throw GmatBaseException("Expecting \",\" for Array " + str);
      
      UnsignedInt closeParen = str1.find(')');
      if (closeParen == str1.npos)
         throw GmatBaseException("Expecting \")\" for Array " + str);
      
      rowStr = str1.substr(openParen+1, comma-openParen-1);
      colStr = str1.substr(comma+1, closeParen-comma-1);
   }

   name = str1.substr(0, openParen);
   
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("StringUtil::GetArrayIndexVar() rowStr=%s, colStr=%s, name=%s\n",
       rowStr.c_str(), colStr.c_str(), name.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void FindFirstAndLast(const std::string &str, char ch, Integer &first,
//                       Integer &last)
//------------------------------------------------------------------------------
/*
 * Finds first and last index of given chracter from the string.
 * if given chracter is not found it sets to -1
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindFirstAndLast(const std::string &str, char ch,
                                      Integer &first, Integer &last)
{
   first = str.find_first_of(ch);
   if (first == (Integer)str.npos)
      first = -1;

   last = str.find_last_of(ch);
   if (last == (Integer)str.npos)
      last = -1;
}


//------------------------------------------------------------------------------
// void FindParenMatch(const std::string &str, Integer &openParen,
//                     Integer &closeParen, bool &isOuterParen)
//------------------------------------------------------------------------------
/*
 * Finds open and close parenthesis index from the string.
 * if parhenthesis is not found it sets to -1
 *
 * @param <str> input string
 * @param <openParen> set to index of first open parenthesis
 * @param <openParen> set to index of matching close parenthesis of openParen
 * @param <isOuterParen> set to true if item is enclosed with parenthesis
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindParenMatch(const std::string &str, Integer &openParen,
                                    Integer &closeParen, bool &isOuterParen)
{
   //MessageInterface::ShowMessage("FindParenMatch() str=%s\n", str.c_str());
   
   openParen = str.find_first_of('(');
   if (openParen == (Integer)str.npos)
      openParen = -1;
   
   closeParen = str.find_last_of(')');
   if (closeParen == (Integer)str.npos)
      closeParen = -1;
   
   std::string str1 = str;
   if (openParen != -1)
      str1 = str.substr(openParen);
   
   isOuterParen = IsOuterParen(str1);
   
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("FindParenMatch() str=%s, openParen=%d, closeParen=%d, isOuterParen=%d\n",
       str.c_str(), openParen, closeParen, isOuterParen);
   #endif
}


//------------------------------------------------------------------------------
// void FindMatchingParen(const std::string &str, Integer &openParen,
//                        Integer &closeParen, bool &isOuterParen,
//                        Integer start = 0)
//------------------------------------------------------------------------------
/*
 * Finds matching close parenthesis of the first open parenthesis.
 * if parhenthesis is not found it sets to -1
 *
 * @param <str> input string
 * @param <openParen> set to index of first open parenthesis
 * @param <closeParen> set to index of matching close parenthesis of openParen
 * @param <isOuterParen> set to true if item is enclosed with parenthesis
 * @param <start> input starting index
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindMatchingParen(const std::string &str, Integer &openParen,
                                       Integer &closeParen, bool &isOuterParen,
                                       Integer start)
{
   //MessageInterface::ShowMessage
   //   ("FindMatchingParen() start=%d, str=%s\n", start, str.c_str());

   // initialize output parameters
   openParen = -1;
   closeParen = -1;
   isOuterParen = false;
   
   int length = str.size();
   Integer openCounter = 0;
   
   for (int i=start; i<length; i++)
   {
      if (str[i] == '(')
      {
         openCounter++;
         if (openCounter == 1)
            openParen = i;
      }
      else if (str[i] == ')')
      {
         openCounter--;
         closeParen = i;
         if (openCounter == 0)
            break;
      }
   }
   
   if (openParen == 0 && closeParen == length-1)
      isOuterParen = true;
   
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("FindMatchingParen() str=%s, openParen=%d, closeParen=%d, isOuterParen=%d\n",
       str.c_str(), openParen, closeParen, isOuterParen);
   #endif
}


//------------------------------------------------------------------------------
// void FindLastParenMatch(const std::string &str, Integer &openParen,
//                         Integer &closeParen, Integer start = 0)
//------------------------------------------------------------------------------
/*
 * Finds last matching close parenthesis.
 *
 * @param <str> input string
 * @param <openParen> set to index of last first open parenthesis
 * @param <closeParen> set to index of matching close parenthesis of last openParen
 * @param <start> input starting index
 *
 * @return last matching parenthesis index, -1 if close parenthesis not found
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindLastParenMatch(const std::string &str, Integer &openParen,
                                        Integer &closeParen, Integer start)
{
   //MessageInterface::ShowMessage
   //   ("FindLastParenMatch() start=%d, str=%s\n", start, str.c_str());
   
   Integer open1, close1;
   bool isOuterParen;
   Integer start1 = start;
   bool done = false;
   
   while (!done)
   {
      FindMatchingParen(str, open1, close1, isOuterParen, start1);
      //MessageInterface::ShowMessage
      //   ("   ===> start=%d, open1=%d, close1=%d\n", start1, open1, close1);
      
      // find next open parenthesis '('
      start1 = str.find('(', close1);
      
      if (start1 == -1)
         done = true;
   }
   
   openParen = open1;
   closeParen = close1;
}


//------------------------------------------------------------------------------
// bool IsEnclosedWithExtraParen(const std::string &str, bool checkOps = true)
//------------------------------------------------------------------------------
/*
 * return true if item is enclosed with extra parenthesis
 * It will return true: ((a+b)), (a(1,1)), 
 * It wiill return false: (123.456), (1,2), (a*b(1,1)), ((3+5)*2)
 * 
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsEnclosedWithExtraParen(const std::string &str, bool checkOps)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage("IsEnclosedWithExtraParen() str=%s\n", str.c_str());
   #endif
   
   int length = str.size();
   bool isEnclosed = false;
   Integer openCounter = 0;
   
   Integer openParen = str.find_first_of('(');
   if (openParen == (Integer)str.npos)
      openParen = -1;

   Integer closeParen = str.find_last_of(')');
   if (closeParen == (Integer)str.npos)
      closeParen = -1;

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("IsEnclosedWithExtraParen() openParen=%d, closeParen=%d\n", openParen, closeParen);
   #endif
   
   if (openParen == -1 || closeParen == -1)
      return false;
   
   if (openParen != 0 || closeParen != length-1)
      return false;
   
   for (int i=0; i<length; i++)
   {
      if (str[i] == '(')
      {
         openCounter++;
         if (openCounter == 1)
            openParen = i;
      }
      else if (str[i] == ')')
      {
         openCounter--;
         closeParen = i;
         if (openCounter == 0)
            break;
      }
   }
   
   // if enclosed with open and close parenthesis
   if (openParen == 0 && closeParen == length-1)
   {
      // check for double parenthesis
      if (str[1] == '(' && str[length-2] == ')')
      {
         Integer open2, close2;
         Integer start = 1;
         bool isOuter;
         
         FindMatchingParen(str, open2, close2, isOuter, start);
         if (close2 == length-2)
            isEnclosed = true;
      }
      
      if (!checkOps)
         isEnclosed = true;
      
      if (!isEnclosed)
      {
         // check if there is any operator
         std::string substr = str.substr(1, length-2);
         Real rval;
         
         #if DEBUG_STRING_UTIL
         MessageInterface::ShowMessage
            ("IsEnclosedWithExtraParen() substr=%s\n", substr.c_str());
         #endif
         
         if (IsParenPartOfArray(substr))
            isEnclosed = true;
         else if (ToDouble(substr, rval))
            isEnclosed = true;
         else
         {
            if (substr.find('+') == substr.npos && substr.find('-') == substr.npos &&
                substr.find('*') == substr.npos && substr.find('/') == substr.npos)
               isEnclosed = true;
         }
      }
   }   
   
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("IsEnclosedWithExtraParen() isEnclosed=%d\n", isEnclosed);
   #endif
   
   return isEnclosed;
}


//------------------------------------------------------------------------------
// bool IsOuterParen(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if outer parenthesis is not part of ^(#) or array.
 * It ill return true for (a*b-c), (vec(3,2))
 * It ill return false for (A+b^(-1), (2,2), (abc,def)
 * 
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsOuterParen(const std::string &str)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage("IsOuterParen() str=%s\n", str.c_str());
   #endif
   
   int length = str.size();
   bool isOuterParen = true;
   
   Integer openParen = str.find_first_of('(');
   if (openParen == (Integer)str.npos)
      openParen = -1;
   
   Integer closeParen = str.find_last_of(')');
   if (closeParen == (Integer)str.npos)
      closeParen = -1;
   
   if (openParen != 0 && closeParen != length-1)
      return false;

   if (!IsEnclosedWithExtraParen(str, false))
      return false;
   
//    if (IsEnclosedWithExtraParen(str) && IsSingleItem(str))
//       return true;
   
   if (openParen == 0 && closeParen == length-1)
   {
      // make sure ending ) is not part of ^(-1)
      UnsignedInt lastOpenParen = str.find_last_of('(');
      UnsignedInt lastCloseParen = str.find_last_of(')', closeParen-1);

      #if DEBUG_STRING_UTIL > 1
      MessageInterface::ShowMessage
         ("IsOuterParen() lastOpenParen=%d, lastCloseParen=%d\n",
          lastOpenParen, lastCloseParen);
      #endif
      
      std::string substr = str.substr(lastOpenParen);
      if (lastOpenParen > 0)
         if (str.find("^(", lastOpenParen-1) != str.npos &&
             (lastCloseParen == str.npos || lastCloseParen < lastOpenParen))
             isOuterParen = false;
      
      // make sure ending ) is not part of array
      if (isOuterParen)
         if (IsParenPartOfArray(substr))
            isOuterParen = false;
   }
   else
   {
      isOuterParen = false;
   }
   
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("IsOuterParen() openParen=%d, closeParen=%d, length=%d, "
       "isOuterParen=%d\n", openParen, closeParen, length, isOuterParen);
   #endif
   
   return isOuterParen;
}


//------------------------------------------------------------------------------
// bool IsParenPartOfArray(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if parenthesis is part of an array.
 * For example: (2,2) or (abc,def)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsParenPartOfArray(const std::string &str)
{
   bool ret = true;
   
   int length = str.size();
   int comma = str.find(',');

   for (int i=1; i<comma; i++)
      if (!isalnum(str[i]))
      {
         ret = false;
         break;
      }

   if (ret)
      for (int i=comma+1; i<length-1; i++)
         if (!isalnum(str[i]))
         {
            ret = false;
            break;
         }
   
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("IsParenPartOfArray() str=%s, length=%d, comma=%d, ret=%d\n",
       str.c_str(), length, comma, ret);
   #endif
   
   return ret;
}


//------------------------------------------------------------------------------
// bool IsSingleItem(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if string does not have arithmetic operators or '.' or
 * enclosed with double parenthesis
 * For example -123.0, abc, (a), (sat.X), (-123.0).
 * It will return false for (1,1)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsSingleItem(const std::string &str)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage("IsSingleItem() str=%s\n", str.c_str());
   #endif
   
   Integer length = str.size();
   bool singleItem = true;
   Integer minusSignCounter = 0;
   Real rval;
   
   // first check for number
   if (ToDouble(str, rval))
      return true;
   
   for (int i=0; i<length; i++)
   {
      if (isalnum(str[i]) || str[i] == '.')
         continue;
      
      if (str[i] == '-')
      {
         minusSignCounter++;
         continue;
      }

      singleItem = false;
      break;
   }
   
   if (singleItem)
   {
      if (minusSignCounter > 0)
         if (str[0] != '-' || minusSignCounter != 1)
            singleItem = false;
   }
   
   return singleItem;
}


//------------------------------------------------------------------------------
// std::string RemoveExtraParen(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Removs extra pair of parenthesis.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveExtraParen(const std::string &str)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("RemoveExtraParen() entering str=%s\n", str.c_str());
   #endif
   
   std::string str1 = str;
   std::string substr;
   Integer length = str.size();
   Integer openCounter = 0;
   Integer openParen = 0, closeParen = 0;
   Integer counter = 0;
   std::map<Integer, Integer> openParenMap;
   std::map<Integer, Integer> closeParenMap;
   
   // remove outer parenthesis
   while (IsEnclosedWithExtraParen(str1))
   {
      counter++;
      str1 = str.substr(counter, length-counter-counter);
   }
   
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage("RemoveExtraParen() str1=%s\n", str1.c_str());
   #endif
   
   std::string str2 = str1;
   length = str1.size();

   // go through each char and remove extra parenthesis
   for (int i=0; i<length; i++)
   {
      if (str1[i] == '(')
      {
         openParen = i;
         openCounter++;
         openParenMap[openCounter] = i;
         
         #if DEBUG_STRING_UTIL
         MessageInterface::ShowMessage
            ("===> openParen:  i=%d, openCounter=%d\n", i, openCounter);
         #endif
      }
      else if (str1[i] == ')')
      {
         closeParen = i;
         closeParenMap[openCounter] = i;
         
         #if DEBUG_STRING_UTIL
         MessageInterface::ShowMessage
            ("===> closeParen: i=%d, openCounter=%d\n", i, openCounter);
         #endif

         //-----------------------------------------------------------
         // check one more outer parenthesis
         //-----------------------------------------------------------
         
         openParen = openParenMap[openCounter];
         closeParen = closeParenMap[openCounter];
         
         substr = str1.substr(openParen, closeParen-openParen+1);
               
         #if DEBUG_STRING_UTIL
         MessageInterface::ShowMessage
            ("===> substr=%s\n   openParen=%d, closeParen=%d, "
             "closeCounter=%d, openCounter=%d\n", substr.c_str(), openParen,
             closeParen, closeCounter, openCounter);
         #endif
         
         // if ( is not part of function or array
         if ((openParen == 0) ||
             (str1[openParen-1] == '+' || str1[openParen-1] == '-' ||
              str1[openParen-1] == '*' || str1[openParen-1] == '/' ||
              str1[openParen-1] == '(' || str1[openParen-1] == ' '))
         {
            if (str1[closeParen+1] != '^')
            {
               if (IsEnclosedWithExtraParen(substr))
               {
                  str2[openParen] = '?';
                  str2[closeParen] = '?';
               }
            }
         }
         
         openCounter--;
         
      }
   }
   
   str2 = RemoveAll(str2, '?', 0);
   
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage("RemoveExtraParen() exiting str2=%s\n", str2.c_str());
   #endif
   
   return str2;
}
