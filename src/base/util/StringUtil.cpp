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

#include "StringUtil.hpp"
#include "MessageInterface.hpp"
#include "UtilityException.hpp"
#include "GmatGlobal.hpp"
#include "Linear.hpp"            // for ToString()
#include "StringTokenizer.hpp"   // for StringTokenizer()
#include <map>
#include <algorithm>
#include <stdio.h>               // for sprintf()
#include <cstdlib>               // Required for GCC 4.3
#include <algorithm>             // Required for GCC 4.3

//#define DEBUG_STRING_UTIL 1
//#define DEBUG_ARRAY_INDEX 2
//#define DEBUG_STRING_UTIL_ARRAY 1
//#define DEBUG_STRING_UTIL_SEP 2
//#define DEBUG_NO_BRACKETS
//#define DEBUG_BALANCED_BRACKETS
//#define DEBUG_MATH_EQ 1
//#define DEBUG_STRING_UTIL_SEP_COMMA


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
// std::string RemoveLastNumber(const std::string &str, Integer &lastNumber)
//------------------------------------------------------------------------------
/* This method returns string without numbers appended to string.
 * It will set lastNumber to 0 if there is no number appended.
 *
 * @param str input string
 * @param lastNumber the number appended to string to output
 *
 * @return string without last number if number found
 *
 * For example,
 *    justString will return justString and set lastNumber to 0
 *    someString123 will return someString and set lastNumber to 123
 *    some1String2 will return soma1String and set lastNumber to 2
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveLastNumber(const std::string &str,
                                             Integer &lastNumber)
{
   lastNumber = 0;
   UnsignedInt index = str.find_last_not_of("0123456789");
   if (index == str.size())
      return str;

   std::string str1 = str.substr(index+1);

   if (!ToInteger(str1, lastNumber))
      lastNumber = 0;

   return str.substr(0, index+1);
}


//------------------------------------------------------------------------------
// std::string RemoveLastString(const std::string &str, const std::string &lastStr)
//------------------------------------------------------------------------------
/* This method returns last string stripped off from the given input string if
 * last string found
 *
 * @param str input string
 * @param lastStr string to strip off
 * @param removeAll set to true if lastStr is recursively removed
 *
 * @return string without last string if last string exist
 *
 * For example, if lastStr is XXX and removeAll is false
 *    inputStringXXX will return inputString
 *    inputStringXXXXXX will return inputStringXXX
 *    inputStringXXX will return inputStringXXX
 * if lastStr is XXX and removeAll is true
 *    inputStringXXX will return inputString
 *    inputStringXXXXXX will return inputString
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveLastString(const std::string &str,
                                             const std::string &lastStr,
                                             bool removeAll)
{
   std::string str1 = str;

   if (EndsWith(str1, lastStr))
      str1 = str1.substr(0, str1.size() - lastStr.size());

   if (removeAll)
   {
      if (EndsWith(str1, lastStr))
         return RemoveLastString(str1, lastStr, removeAll);
   }

   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveSpaceInBrackets(const std::string &str,
//                                   const std::string &bracketPair)
//------------------------------------------------------------------------------
/*
 * Removes spaces in array indexing.
 *   "A( 3, 3)  B(1  ,1)" gives "A(3,3)  B(1,1)"
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveSpaceInBrackets(const std::string &str,
                                                  const std::string &bracketPair)
{
   // Removed extra space in the array indexing, between bracketPair (), [], {}
   std::string str1;
   std::string::size_type index1 = 0;
   std::string::size_type closeParen;
   std::string::size_type length = str.size();
   std::string closeStr = bracketPair.substr(1,1);

   while (index1 < length)
   {
      if (str[index1] == bracketPair[0])
      {
         str1.push_back(str[index1]);
         closeParen = str.find(bracketPair[1], index1);

         if (closeParen == str.npos)
            throw UtilityException
               ("Closing bracket \"" + closeStr + "\" not found");

         // find close paren and copy non-blank char
         for (UnsignedInt j=index1+1; j<=closeParen; j++)
            if (str[j] != ' ')
               str1.push_back(str[j]);

         index1 = closeParen+1;
      }
      else
      {
         str1.push_back(str[index1++]);
      }
   }

   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveTrailingZeros(Real val, const std::string &valStr)
//------------------------------------------------------------------------------
/**
 * Removes trailing zeros from real number string. It goes through another
 * iteration using less precisoin format for numbers ending .999999.
 *
 * @param val  Real number
 * @param valStr String of real number to remove trailing zeros
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveTrailingZeros(Real val, const std::string &valStr,
                                                Integer iterCount)
{
   #ifdef DEBUG_TRAILING_ZEROS
   MessageInterface::ShowMessage
      ("RemoveTrailingZeros() entered, val = %f, valStr = %s, iterCount = %d\n",
       val, valStr.c_str(), iterCount);
   #endif
   
   if (iterCount > 1)
      return valStr;
   
   std::string str = valStr;
   std::string str1;
   std::string::size_type point = str.find_last_of(".");
   std::string::size_type lastNonZero = str.find_last_not_of("0");
   
   if (point == lastNonZero)
   {
      str1 = str.substr(0, lastNonZero + 2);
   }
   else if (lastNonZero == str.size() - 1)
   {
      // use less precision format for case ending with .9999999
      char buffer[50];
      sprintf(buffer, "%.*f", 12 - iterCount, val);
      str1 = buffer;
      
      #ifdef DEBUG_TRAILING_ZEROS
      MessageInterface::ShowMessage
         ("   Used .%df format, str1 = %s\n", 12-iterCount, str1.c_str());
      #endif
      
      if (iterCount < 2)
         str1 = RemoveTrailingZeros(val, str1, ++iterCount);
   }
   else
   {
      str1 = str.substr(0, lastNonZero + 1);
   }
   
   return str1;
}


//------------------------------------------------------------------------------
// std::string Trim(const std::string &str, StripType stype = BOTH,
//                  bool removeSemicolon = false, bool removeEol = false)
//------------------------------------------------------------------------------
/*
 * Removes leading or/and trailing blanks.  On option it removes end-of-line
 * first before removing semicolon.
 *
 * @param  str  input string to be trimmed
 * @param  stype  option of trimming front, end, or both (default is BOTH)
 * @param  removeSemicolon  option of removing trailing semicolon
 * @param  removeEol  option of removing end-of-line character first before
 *                    removing semicolon
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::Trim(const std::string &str, StripType stype,
                                 bool removeSemicolon, bool removeEol)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("GmatStringUtil::Trim() str=<%s>, stripType=%d, removeSemicolon=%d, "
       "removeEol=%d\n", str.c_str(), stype, removeSemicolon, removeEol);
   #endif

   if (str == "")
      return str;

   std::string str2;
   std::string whiteSpace = " \t";

   std::string::size_type index1 = str.find_first_not_of(whiteSpace);
   std::string::size_type index2 = str.find_last_not_of(whiteSpace);

   if (index1 == str.npos)
      index1 = 0;

   if (stype == LEADING)
      str2 = str.substr(index1);
   else if (stype == TRAILING)
      str2 = str.substr(0, index2 + 1);
   else if (stype == BOTH)
      str2.assign(str.substr(index1, index2-index1+1));

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("   index1=%d, index2=%d str=<%s>, str2=<%s>\n", index1, index2,
       str.c_str(), str2.c_str());
   #endif

   // remove trailing end-of-lines (\r or \n) or blanks or tabs (\t)
   // before removing semicolns
   if (removeSemicolon)
   {
      if (str2.size() > 0)
      {
         // if option is to remove eol
         if (removeEol)
         {
            // replace all occurance of tab with a space
            str2 = Replace(str2, "\t", " ");

            // remove trailing \r and \n
            while (str2[str2.size()-1] == '\n' || str2[str2.size()-1] == '\r')
               str2.erase(str2.size()-1, 1);

            // remove trailing blanks
            while (str2[str2.size()-1] == ' ')
               str2.erase(str2.size()-1, 1);
         }

         // remove trailing semicolns
         while (str2.size() > 0 && str2[str2.size()-1] == ';')
            str2.erase(str2.size()-1, 1);
      }
   }

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("GmatStringUtil::Trim() returning <%s>\n", str2.c_str());
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
// std::string ToUpper(const std::string &str, bool firstLetterOnly = false)
//------------------------------------------------------------------------------
/**
 * Makes whole string or first letter upper case.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToUpper(const std::string &str, bool firstLetterOnly)
{
   Integer len = str.length();
   
   if (len == 0)
      return str;
   
   std::string upper = str;
   
   if (firstLetterOnly)
      upper[0] = toupper(str[0]);
   else
      for (int i=0; i<len; i++)
         upper[i] = toupper(str[i]);
   
   return upper;
}


//------------------------------------------------------------------------------
// std::string ToLower(const std::string &str, bool firstLetterOnly = false)
//------------------------------------------------------------------------------
/**
 * Makes whole string or first letter lower case.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToLower(const std::string &str, bool firstLetterOnly)
{
   Integer len = str.length();
   
   if (len == 0)
      return str;
   
   std::string lower = str;
   
   if (firstLetterOnly)
      lower[0] = tolower(str[0]);
   else
      for (int i=0; i<len; i++)
         lower[i] = tolower(str[i]);
   
   return lower;
}


//------------------------------------------------------------------------------
// std::string Capitalize(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Capitialize the first letter of the string
 */
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
   #ifdef DEBUG_REPLACE
   MessageInterface::ShowMessage
      ("GmatStringUtil::Replace()> str=\"%s\", from=\"%s\", to=\"%s\"\n", str.c_str(),
       from.c_str(), to.c_str());
   #endif

   std::string str1 = str;
   std::string::size_type pos = str1.find(from);

   // if string not found, just return same string
   if (pos == str1.npos)
      return str1;

   // if input string is the same as string to replace, just return <to> string
   if (str == from)
      return to;

   bool done = false;
   std::string::size_type start = 0;

   while (!done)
   {
      pos = str1.find(from, start);

      #ifdef DEBUG_REPLACE
      MessageInterface::ShowMessage("===> start=%u, pos=%u\n", start, pos);
      #endif

      if (pos != str1.npos)
      {
         str1.replace(pos, from.size(), to);
         start = pos + to.size();

         #ifdef DEBUG_REPLACE
         MessageInterface::ShowMessage("===> start=%u, str1=<%s>\n", start, str1.c_str());
         #endif
      }
      else
      {
         break;
      }
   }

   return str1;
}


//------------------------------------------------------------------------------
// std::string ReplaceName(const std::string &str, const std::string &from,
//                         const std::string &to)
//------------------------------------------------------------------------------
/*
 * Replaces all occurenece of <from> string to <to> string if <from> is not
 * part of word. It replaces the string if character before and after <from> are
 * not alphanumeric except underscores "_".
 *
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ReplaceName(const std::string &str, const std::string &from,
                                        const std::string &to)
{
   #ifdef DEBUG_REPLACE_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::ReplaceName() str=\"%s\", from=\"%s\", to=\"%s\"\n", str.c_str(),
       from.c_str(), to.c_str());
   #endif
   
   std::string str1 = str;
   std::string::size_type pos = str1.find(from);
   
   // if string not found, just return same string
   if (pos == str1.npos)
      return str1;
   
   // if input string is the same as string to replace, just return <to> string
   if (str == from)
      return to;
   
   bool done = false;
   std::string::size_type start = 0;
   std::string::size_type strSize = str1.size();
   std::string::size_type fromSize = from.size();
   bool replace = false;
   
   while (!done)
   {
      #ifdef DEBUG_REPLACE_NAME
      MessageInterface::ShowMessage("===> str1='%s'\n", str1.c_str());
      #endif
      
      strSize = str1.size();
      pos = str1.find(from, start);
      
      if (pos != str1.npos)
      {
         replace = false;
         
         #ifdef DEBUG_REPLACE_NAME
         MessageInterface::ShowMessage("===> start=%u, pos=%u\n", start, pos);
         #endif
         
         if (pos == 0 && fromSize < strSize)
         {
            if (!isalnum(str1[fromSize]) && str1[fromSize] != '_')
               replace = true;
         }
         else if (pos > 0 && (pos + fromSize) < strSize)
         {
            if (!isalnum(str1[pos + fromSize]) && str1[pos + fromSize] != '_')
               replace = true;
         }
         else if (pos == strSize-fromSize) // replace last name
         {
            replace = true;
         }
         
         #ifdef DEBUG_REPLACE_NAME
         MessageInterface::ShowMessage("===> replace=%d\n", replace);
         #endif
         
         if (replace)
         {
            // Check for the system Parameter name which should not not be replace,
            // such as SMA in sat.SMA Parameter or sat.EarthEqCS.X
            if (pos == 0 || ((pos > 0 && str1[pos-1] == '.') &&
                             (pos-1 != str1.find_last_of('.'))))
               str1.replace(pos, fromSize, to);
         }
         
         start = pos + to.size();
         
         #ifdef DEBUG_REPLACE_NAME
         MessageInterface::ShowMessage("===> start=%d, str1=<%s>\n", start, str1.c_str());
         #endif
      }
      else
      {
         break;
      }
   }
   
   #ifdef DEBUG_REPLACE_NAME
   MessageInterface::ShowMessage("GmatStringUtil::ReplaceName() returning <%s>\n", str1.c_str());
   #endif
   
   return str1;
}


//------------------------------------------------------------------------------
// std::string ReplaceNumber(const std::string &str, const std::string &from,
//                         const std::string &to)
//------------------------------------------------------------------------------
/*
 * Replaces all occurenece of <from> string to <to> string if <from> is a part
 * of number. It will replace if input string is for example 3e-, 0E+, 34234
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ReplaceNumber(const std::string &str, const std::string &from,
                                          const std::string &to)
{
   #ifdef DEBUG_REPLACE_NUMBER
   MessageInterface::ShowMessage
      ("GmatStringUtil::ReplaceNumber() str=\"%s\", from=\"%s\", to=\"%s\"\n", str.c_str(),
       from.c_str(), to.c_str());
   #endif
   
   std::string str1 = str;
   std::string::size_type pos = str1.find(from);

   // if string not found, just return same string
   if (pos == str1.npos)
      return str1;

   // if input string is the same as string to replace, just return <to> string
   if (str == from)
      return to;

   bool done = false;
   std::string::size_type start = 0;
   std::string::size_type fromSize = from.size();
   
   while (!done)
   {
      pos = str1.find(from, start);
      if (pos != str1.npos)
      {
         #ifdef DEBUG_REPLACE_NUMBER
         MessageInterface::ShowMessage("===> start=%u, pos=%u\n", start, pos);
         #endif
         
         if (pos > 0 && isdigit(str1[pos-1]))
            str1.replace(pos, fromSize, to);
         
         start = pos + to.size();
         
         #ifdef DEBUG_REPLACE_NUMBER
         MessageInterface::ShowMessage("===> start=%d, str1=<%s>\n", start, str1.c_str());
         #endif
      }
      else
      {
         break;
      }
   }
   
   #ifdef DEBUG_REPLACE_NUMBER
   MessageInterface::ShowMessage("GmatStringUtil::ReplaceNumber() returning <%s>\n", str1.c_str());
   #endif

   return str1;
}


//------------------------------------------------------------------------------
// std::string ToString(const bool &val)
//------------------------------------------------------------------------------
/*
 * Formats bool value to String. If value is not true, it returns "false".
 *
 * @param  val  boolean value
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const bool &val)
{
   if (val == true)
      return "true";
   else
      return "false";
}


//------------------------------------------------------------------------------
// std::string ToString(const Real &val, Integer precision, bool showPoint,
//                      Integer width)
//------------------------------------------------------------------------------
/*
 * Formats Real value to String.
 *
 * @param  val        Real value
 * @param  precision  Precision to be used in formatting
 * @param  showPoint  Formats using ios::showpoint if true (false)
 * @param  width      Width to be used in formatting (1)
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const Real &val, Integer precision,
                                     bool showPoint, Integer width)
{
   return GmatRealUtil::ToString(val, false, false, showPoint, precision, width);
}


//------------------------------------------------------------------------------
// std::string ToString(const Integer &val, Integer width)
//------------------------------------------------------------------------------
/*
 * Formats Integer value to String.
 *
 * @param  val  Integer value
 * @param  width  Width to be used in formatting
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const Integer &val, Integer width)
{
   return GmatRealUtil::ToString(val, false, width);
}


//------------------------------------------------------------------------------
// std::string ToString(const Real &val, bool useCurrentFormat, bool scientific,
//                      bool showPoint, Integer precision, Integer width)
//------------------------------------------------------------------------------
/*
 * Formats Real value to String.
 *
 * @param  val  Real value
 * @param  useCurrentFormat  Uses precision and width from GmatGlobal (true)
 * @param  scientific  Formats using scientific notation if true (true)
 * @param  showPoint if true, shows decimal point and trailing zeros (false)
 * @param  precision  Precision to be used in formatting (GmatGlobal::DATA_PRECISION)
 * @param  width  Width to be used in formatting (GmatGlobal::INTEGER_WIDTH)
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const Real &val, bool useCurrentFormat,
                                     bool scientific, bool showPoint,
                                     Integer precision, Integer width)
{
   return GmatRealUtil::ToString(val, useCurrentFormat, scientific, showPoint,
                                 precision, width);
}


//------------------------------------------------------------------------------
// std::string ToString(const Integer &val, bool useCurrentFormat, Integer width)
//------------------------------------------------------------------------------
/*
 * Formats Integer value to String.
 *
 * @param  val  Integer value
 * @param  useCurrentFormat  Uses width from GmatGlobal if true (true)
 * @param  width  Width to be used in formatting (GmatGlobal::INTEGER_WIDTH)
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToString(const Integer &val, bool useCurrentFormat,
                                     Integer width)
{
   return GmatRealUtil::ToString(val, useCurrentFormat, width);
}


//------------------------------------------------------------------------------
// std::string ToStringNoZeros(const Real &val)
//------------------------------------------------------------------------------
/**
 * Formats real number to string without trailing zeros.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ToStringNoZeros(const Real &val)
{
   std::string str, str1;
   char buffer[50];
   sprintf(buffer, "%.14f", val);
   str = buffer;
   
   return RemoveTrailingZeros(val, str);
}


//-------------------------------------------------------------------------------
// char GetClosingBracket(const char &openBracket)
//-------------------------------------------------------------------------------
char GmatStringUtil::GetClosingBracket(const char &openBracket)
{
   switch (openBracket)
   {
   case '(':
      return ')';
   case '[':
      return ']';
   case '{':
      return '}';
   case '<':
      return '>';

   default:
      UtilityException ex;
      ex.SetDetails("Found unknown open bracket: %c", openBracket);
      throw ex;
   }
}


//------------------------------------------------------------------------------
// StringArray SeparateBy(const std::string &str, const std::string &delim,
//                        bool putBracketsTogether, bool insertDelim)
//------------------------------------------------------------------------------
/*
 * Separates string by input delimiter.
 *
 * @param  str  input string
 * @param  delim  input delimiter
 * @param  putBracketsTogether  true if putting brackets together (false)
 * @param  insertDelim  true if inserting delimiter back if only 1 delimiter (false)
 *
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::SeparateBy(const std::string &str,
                                       const std::string &delim,
                                       bool putBracketsTogether,
                                       bool insertDelim, bool insertComma)
{
   #if DEBUG_STRING_UTIL_SEP
   MessageInterface::ShowMessage
      ("GmatStringUtil::SeparateBy() str='%s', delim='%s', putBracketsTogether=%d, "
       "insertDelim=%d\n", str.c_str(), delim.c_str(), putBracketsTogether, insertDelim);
   #endif
   
   StringTokenizer st;
   if (insertDelim)
      st.Set(str, delim, insertDelim);
   else
      st.Set(str, delim);
   
   //StringTokenizer st(str, delim);
   StringArray tempParts = st.GetAllTokens();
   
   if (!putBracketsTogether)
      return tempParts;
   
   //-----------------------------------------------------------------
   // now go through each part and put brackets together
   //-----------------------------------------------------------------
   StringArray parts;
   std::string openBrackets = "([{";
   std::string::size_type index1;
   int count = tempParts.size();
   
   #if DEBUG_STRING_UTIL_SEP
   MessageInterface::ShowMessage("There are %d parts to go through\n", count);
   for (int i=0; i<count; i++)
      MessageInterface::ShowMessage
         ("   tempParts[%d]='%s'\n", i, tempParts[i].c_str());
   #endif
   
   Integer size = -1;
   bool append = false;
   
   // go through each part and put brackets together, insert back delimiter if only one
   for (int i=0; i<count; i++)
   {      
      #if DEBUG_STRING_UTIL_SEP
      MessageInterface::ShowMessage("   ==================== count = %d\n", i);
      #endif
      
      index1 = tempParts[i].find_first_of(openBrackets);
      
      if (index1 != str.npos)
      {
         #if DEBUG_STRING_UTIL_SEP
         MessageInterface::ShowMessage("   one of open brackets ([{ found\n");
         #endif
         if (append)
         {
            // if only one delimiter, insert it back in (loj: 2008.03.24)
            if (delim.size() == 1 && !insertDelim)
               parts[size] = parts[size] + delim + tempParts[i];
            else
               parts[size] = parts[size] + " " + tempParts[i];
         }
         else
         {
            #if DEBUG_STRING_UTIL_SEP > 1
            MessageInterface::ShowMessage
               ("   adding1 '%s'\n", tempParts[i].c_str());
            #endif
            
            parts.push_back(tempParts[i]);
            size++;
         }
         
         // if any bracket is not balanced, append
         if (!IsBracketBalanced(parts[size], "()") ||
             !IsBracketBalanced(parts[size], "[]") ||
             !IsBracketBalanced(parts[size], "{}"))
            append = true;
         else
            append = false;
         
         #if DEBUG_STRING_UTIL_SEP > 1
         MessageInterface::ShowMessage
            ("   parts[%d]='%s'\n", size, parts[size].c_str());
         MessageInterface::ShowMessage("   append=%d\n", append);
         #endif
      }
      else
      {
         if (append)
         {
            #if DEBUG_STRING_UTIL_SEP > 1
            MessageInterface::ShowMessage
               ("   appending '%s'\n", tempParts[i].c_str());
            #endif
            
            // if only one delimitter, insert it back in (loj: 2008.03.24)
            if (delim.size() == 1 && !insertDelim)
               parts[size] = parts[size] + delim + tempParts[i];
            else
            {
               if (insertComma)
                  parts[size] = parts[size] + "," + tempParts[i];
               else
                  parts[size] = parts[size] + tempParts[i];
            }
         }
         else
         {
            #if DEBUG_STRING_UTIL_SEP > 1
            MessageInterface::ShowMessage
               ("   adding2 '%s'\n", tempParts[i].c_str());
            #endif
            
            parts.push_back(tempParts[i]);
            size++;
         }

         // if any bracket is not balanced, append
         if (!IsBracketBalanced(parts[size], "()") ||
             !IsBracketBalanced(parts[size], "[]") ||
             !IsBracketBalanced(parts[size], "{}"))
            append = true;
         else
            append = false;

         #if DEBUG_STRING_UTIL_SEP > 1
         MessageInterface::ShowMessage
            ("   parts2[%d]='%s'\n", size, parts[size].c_str());
         MessageInterface::ShowMessage("   append=%d\n", append);
         #endif
      }
   }
   
   StringArray parts1;
   
   // add non-blank items
   for (unsigned int i=0; i<parts.size(); i++)
   {
      parts[i] = Strip(parts[i]);
      if (parts[i] != "")
         parts1.push_back(parts[i]);
   }
   
   #if DEBUG_STRING_UTIL_SEP
   MessageInterface::ShowMessage
      ("GmatStringUtil::SeparateBy() str='%s' returning:\n", str.c_str());
   for (unsigned int i=0; i<parts1.size(); i++)
      MessageInterface::ShowMessage
         ("   parts1[%d] = '%s'\n", i, parts1[i].c_str());
   #endif
   
   return parts1;
}


//------------------------------------------------------------------------------
// StringArray SeparateByComma(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Separates string by comma leaving all parentheses and single quotes intact.
 * If parentheses are not balanced, it will just return input string.
 *
 * @param  str  input string
 * @return  StringArray of separated parts
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::SeparateByComma(const std::string &str)
{
   #ifdef DEBUG_STRING_UTIL_SEP_COMMA
   MessageInterface::ShowMessage
      ("GmatStringUtil::SeparateByComma() str=\"%s\"n", str.c_str());
   #endif

   StringArray parts;
   parts.push_back(str);

   // if no comma is found, just return input string
   std::string::size_type index1 = str.find(",");
   if (index1 == str.npos)
      return parts;

   // Make sure that all parentheses are balanced
   if (!IsParenBalanced(str))
      return parts;

   // Now go through each character in the string
   parts.clear();
   Integer count = str.size();
   std::string str1 = str;
   bool insideQuote = false;
   Integer openCount = 0;
   std::string part;

   for (int i=0; i<count; i++)
   {
      if (str1[i] == ',')
      {
         if (insideQuote || openCount > 0)
         {
            part = part + str1[i];
         }
         else
         {
            parts.push_back(part);
            part = "";
         }
      }
      else if (str1[i] == '\'')
      {
         part = part + str1[i];
         if (insideQuote)
            insideQuote = false;
         else
            insideQuote = true;
      }
      else if (str1[i] == '(')
      {
         part = part + str1[i];
         openCount++;
      }
      else if (str1[i] == ')')
      {
         part = part + str1[i];
         openCount--;
      }
      else
      {
         part = part + str1[i];
      }
   }

   parts.push_back(part);

   // Let's stip off leadin and trailing blanks before returning
   for (UnsignedInt i=0; i<parts.size(); i++)
   {
      parts[i] = Trim(parts[i], BOTH);
   }

   return parts;
}


//-------------------------------------------------------------------------------
// StringArray SeparateDots(const std::string &chunk)
//-------------------------------------------------------------------------------
/*
 * Breaks string by dots, but keep decimal point number together.
 *
 * @param <chunk> input string to be break apart
 *
 * @return string array of parts
 */
//-------------------------------------------------------------------------------
StringArray GmatStringUtil::SeparateDots(const std::string &chunk)
{
   Real rval;
   StringArray parts;

   // Separate by dots if not a number
   if (ToReal(chunk, rval))
   {
      parts.push_back(chunk);
   }
   else
   {
      StringTokenizer st(chunk, ".");
      parts = st.GetAllTokens();
   }

   #ifdef DEBUG_SEP_DOTS
   for (UnsignedInt i=0; i<parts.size(); i++)
      MessageInterface::ShowMessage
         ("   parts[%d]=%s\n", i, parts[i].c_str());
   #endif

   return parts;
}


//------------------------------------------------------------------------------
// bool IsNumber(const std::string &str)
//------------------------------------------------------------------------------
/*
 * @return  true if input string is a number.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsNumber(const std::string &str)
{
   Real rval;
   Integer ival;
   
   if (ToReal(str, rval) || ToInteger(str, ival))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool ToReal(const std::string &str, Real *value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @see ToReal(const std::string &str, Real &value)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToReal(const std::string &str, Real *value, bool trimParens)
{
   return ToReal(str, *value, trimParens);
}


//------------------------------------------------------------------------------
// bool ToReal(const std::string &str, Real &value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * This method converts string to Real using atof() after validation.
 *
 * @param  str  input string to be converted to Real
 * @param  value  output Real value
 *
 * @return true if input string represents valid Real number, false otherwise
 *
 * @note  atof() returns 100.00 for 100.00ABC, but we want it be an error.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToReal(const std::string &str, Real &value, bool trimParens)
{
   if (str == "")
      return false;

   std::string str2 = Trim(str, BOTH);
   if (trimParens)
   {
      str2 = RemoveExtraParen(str2);
      str2 = Trim(str2, BOTH);
   }
   Integer numDot = 0;
   std::string::size_type index1;

   if (str2.length() == 0)
      return false;

   // If first character is not '+', '-', '.' and digit, it's false
   if (str2[0] != '+' && str2[0] != '-' && !isdigit(str2[0]) && str2[0] != '.')
      return false;

   // Check for multiple E or e
   index1 = str2.find_first_of("Ee");
   if (index1 != str2.npos)
      if (str2.find_first_of("Ee", index1 + 1) != str2.npos)
         return false;

   // Check for multiple + or -
   index1 = str2.find_first_of("+-");
   if (index1 != str2.npos)
   {
      index1 = str2.find_first_of("+-", index1 + 1);
      if (index1 != str2.npos)
      {
         std::string::size_type firstE = str2.find_first_of("Ee");
         if (firstE != str2.npos && index1 < firstE)
            return false;
      }
   }

   for (unsigned int i=0; i<str2.length(); i++)
   {
      if (i == 0 && (str2[0] == '-' || str2[0] == '+'))
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
             (str2[i+1] == '+' || str2[i+1] == '-' || isdigit(str2[i+1])))
            continue;

         if ((str2[i] == '+' || str2[i] == '-' || isdigit(str2[i])) &&
             (str2[i-1] == 'e' || str2[i-1] == 'E'))
            continue;

         return false;
      }
   }

   value = atof(str2.c_str());
   return true;
}


//------------------------------------------------------------------------------
// bool ToInteger(const std::string &str, Integer *value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @see ToInteger(const std::string &str, Integer &value)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToInteger(const std::string &str, Integer *value, bool trimParens)
{
   return ToInteger(str, *value, trimParens);
}


//------------------------------------------------------------------------------
// bool ToInteger(const std::string &str, Integer &value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * This method converts string to Integer (signed integer) using atoi() after
 * validation.
 *
 * @param  str  input string to be converted to Integer
 * @param  value  output Integer value
 *
 * @return true if input string represents valid Integer number, false otherwise
 *
 * @note  atoi() returns 0 for X or 100 for 100ABC, but we want it be an error.
 *        This method returns signed interger value.
 *        The minimum value of Integer is -2147483647.
 *        The maximum value of Integer is +2147483647.
 *        The value out of this range will return complementary number
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToInteger(const std::string &str, Integer &value, bool trimParens)
{
   std::string str2 = Trim(str, BOTH);
   if (trimParens)
   {
      str2 = RemoveExtraParen(str2);
      str2 = Trim(str2, BOTH);
   }

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
// bool ToBoolean(const std::string &str, bool *value, bool trimParens = false)
//------------------------------------------------------------------------------
bool GmatStringUtil::ToBoolean(const std::string &str, bool *value, bool trimParens)
{
   return ToBoolean(str, *value, trimParens);
}


//------------------------------------------------------------------------------
// bool ToBoolean(const std::string &str, bool &value, bool trimParens = false)
//------------------------------------------------------------------------------
/*
 * @return true if string value is boolean (TRUE, FALSE, true, false, True, False).
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::ToBoolean(const std::string &str, bool &value, bool trimParens)
{
   std::string str2 = Trim(str, BOTH);
   if (trimParens)
   {
      str2 = RemoveExtraParen(str2);
      str2 = Trim(str2, BOTH);
   }

   if (str2.length() == 0)
      return false;

   if (ToLower(str2) == "true")
   {
      value = true;
      return true;
   }

   if (ToLower(str2) == "false")
   {
      value = false;
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// RealArray ToRealArray(const std::string &str)
//------------------------------------------------------------------------------
RealArray GmatStringUtil::ToRealArray(const std::string &str)
{
//   MessageInterface::ShowMessage("ToRealArray() str='%s'\n", str.c_str());

   RealArray realArray;

   if (!IsBracketBalanced(str, "[]"))
      return realArray;

   std::string str1 = RemoveOuterString(str, "[", "]");
   str1 = Trim(str1);

   if (str1 == "")
      return realArray;

   StringArray vals = SeparateBy(str1, " ,");
   Real rval;

//   MessageInterface::ShowMessage("   vals.size()=%d\n", vals.size());

   for (UnsignedInt i=0; i<vals.size(); i++)
   {
      if (ToReal(vals[i], rval))
          realArray.push_back(rval);
      else
         throw UtilityException
            ("Invalid Real value \"" + vals[i] + "\" found in \"" + str + "\"");
   }

   return realArray;
}


//------------------------------------------------------------------------------
// IntegerArray ToIntegerArray(const std::string &str)
//------------------------------------------------------------------------------
IntegerArray GmatStringUtil::ToIntegerArray(const std::string &str)
{
   IntegerArray intArray;

   if (!IsBracketBalanced(str, "[]"))
      return intArray;

   std::string str1 = RemoveOuterString(str, "[", "]");
   str1 = Trim(str1);

   if (str1 == "")
      return intArray;

   StringArray vals = SeparateBy(str1, " ,");
   Integer ival;

   for (UnsignedInt i=0; i<vals.size(); i++)
   {
      if (ToInteger(vals[i], ival))
          intArray.push_back(ival);
      else
         throw UtilityException
            ("Invalid Integer value \"" + vals[i] + "\" found in \"" + str + "\"");
   }

   return intArray;
}


//------------------------------------------------------------------------------
// UnsignedIntArray ToUnsignedIntArray(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Parse string to unsigned int array.
 * [0 127 255] or [0, 127, 255] to array of 0, 127, 255.
 */
//------------------------------------------------------------------------------
UnsignedIntArray GmatStringUtil::ToUnsignedIntArray(const std::string &str)
{
   UnsignedIntArray intArray;

   if (!IsBracketBalanced(str, "[]"))
      return intArray;

   std::string str1 = RemoveOuterString(str, "[", "]");
   str1 = Trim(str1);

   if (str1 == "")
      return intArray;

   StringArray vals = SeparateBy(str1, " ,");
   Integer ival;

   for (UnsignedInt i=0; i<vals.size(); i++)
   {
      if (ToInteger(vals[i], ival))
          intArray.push_back((UnsignedInt)ival);
      else
         throw UtilityException
            ("Invalid Integer value \"" + vals[i] + "\" found in \"" + str + "\"");
   }

   return intArray;
}

//------------------------------------------------------------------------------
// StringArray ToStringArray(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Parse a brace-enclosed string to a string array.
 * {'str1', 'str2', 'str3']  to an array of 'str1', 'str2', 'str3'.
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::ToStringArray(const std::string &str)
{
   StringArray strArray;

   if (!IsBracketBalanced(str, "{}"))
   {
      std::string errmsg = "String array \"";
      errmsg += str + "\" does not have matching braces.\n";
      throw UtilityException(errmsg);
   }

   std::string str1 = RemoveOuterString(str, "{", "}");
   str1 = Trim(str1);

   if (str1 == "")
      return strArray;

   StringArray strVals = SeparateByComma(str1); // <<<<<<<<<<<<<<
   for (UnsignedInt i=0; i<strVals.size(); i++)
   {
      std::string str2 = Trim(strVals.at(i));
      strArray.push_back(str2);
   }

   return strArray;
}


//------------------------------------------------------------------------------
// BooleanArray ToBooleanArray(const std::string &str)
//------------------------------------------------------------------------------
BooleanArray GmatStringUtil::ToBooleanArray(const std::string &str)
{
   BooleanArray boolArray;

   if (!IsBracketBalanced(str, "[]"))
      return boolArray;

   std::string str1 = RemoveOuterString(str, "[", "]");
   str1 = Trim(str1);
   
   if (str1 == "")
      return boolArray;

   StringArray vals = SeparateBy(str1, " ,");
   bool bval;

   for (UnsignedInt i=0; i<vals.size(); i++)
   {
      if (ToBoolean(vals[i], bval))
          boolArray.push_back(bval);
      else
         throw UtilityException
            ("Invalid Boolean value \"" + vals[i] + "\" found in \"" + str + "\"");
   }

   return boolArray;
}


//------------------------------------------------------------------------------
// void ParseParameter(const std::string &str, std::string &type,
//                     std::string &owner, std::string &dep)
//------------------------------------------------------------------------------
/*
 * Parses input string as Parameter elements.
 * Parameter is in the form of: Owner.Dependency.Type
 * Owner is the owener object of the Type and Dependency is defined by the
 * Parameter class of Type.  Dependency can be omitted.
 *    such as Sat.MJ2000Eq.X, Sat.Y, Sat.Earth.ECC, ImpBurn.B
 *
 * @see Parameter
 *
 * @param  str  input parameter string
 * @param  type  output type of parameter
 * @param  owner  ouput owner of parameter
 * @param  dep  output dependency of parameter
 */
//------------------------------------------------------------------------------
void GmatStringUtil::ParseParameter(const std::string &str, std::string &type,
                                    std::string &owner, std::string &dep)
{
   type  = "";
   owner = "";
   dep   = "";
   //find owner.dep.type
   std::string str1 = str;
   std::string::size_type pos1 = str1.find(".");
   std::string::size_type pos2 = str1.find_last_of(".");

   if (pos1 != str1.npos && pos2 != str1.npos)
   {
      owner = str1.substr(0, pos1);
      type = str1.substr(pos2+1, str1.npos-pos2);

      dep = "";
      if (pos2 > pos1)
         dep = str1.substr(pos1+1, pos2-pos1-1);
   }

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("GmatStringUtil::ParseParameter() str=%s, type=%s, owner=%s, dep=%s\n",
       str.c_str(), type.c_str(), owner.c_str(), dep.c_str());
   #endif

}


//------------------------------------------------------------------------------
// void GetArrayCommaIndex(const std::string &str, Integer &comma,
//                         const std::string &bracketPair = "()")
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayCommaIndex(const std::string &str, Integer &comma,
                                        const std::string &bracketPair)
{
   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("GmatStringUtil::GetArrayCommaIndex() str=%s\n", str.c_str());
   #endif

   comma = -1;
   std::string openStr = bracketPair.substr(0,1);
   std::string::size_type openBracket = str.find(openStr);
   std::string::size_type firstComma;

   Integer length = str.size();
   std::string str1 = str.substr(openBracket+1, length-openBracket-2);

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("   openBracket=%u, str1=%s\n", openBracket, str1.c_str());
   #endif

   // if array index is empty, Arr()
   if (str1 == "")
      return;


   // if there is no opening bracket
   if (openBracket == str.npos)
   {
      comma = -1;  // wcs - 2007.11.16 - I think we just want to return -1 here ...
      //firstComma = str.find(',');
      //if (firstComma != str.npos)
      //   comma = firstComma;

      #if DEBUG_ARRAY_INDEX
      MessageInterface::ShowMessage
         ("GmatStringUtil::GetArrayCommaIndex() comma=%d\n", comma);
      #endif

      return;
   }

   Integer open, close;
   bool isOuterBracket;
   length = str1.size();

   // find matching closing parenthesis
   FindMatchingBracket(str1, open, close, isOuterBracket, bracketPair, 0);

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("   length=%d, open=%d, close=%d, isOuterBracket=%d\n",
       length, open, close, isOuterBracket);
   #endif


   firstComma = str1.find(',');


   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage("   firstComma=%u\n", firstComma);
   #endif

   // if closing paren found
   if (close != -1)
   {
      std::string::size_type commaAfterClose = str1.find(',', close);

      // if row is missing
      if (commaAfterClose == str1.npos && firstComma > UnsignedInt(open))
      {
         firstComma = str1.npos;
      }
      // if row has parenthesis
      else if (commaAfterClose != str1.npos)
      {
         firstComma = close + 1;
      }
   }

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage("   firstComma=%u\n", firstComma);
   #endif

   if (firstComma != str1.npos)
      comma = firstComma + openBracket + 1;

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("GmatStringUtil::GetArrayCommaIndex() comma=%d\n", comma);
   #endif
}


//------------------------------------------------------------------------------
// void GetArrayIndexVar(const std::string &str, std::string &rowStr,
//                       std::string &colStr, std::string &name,
//                       const std::string &bracketPair)
//------------------------------------------------------------------------------
/*
 * Returns array row and column index as string. If open bracket is not found,
 * it will set "-1" as row and column string index and input string as name.
 * If row or column is missing it will "-1" to index string.
 *    Arr     will set rowStr to "-1", colStr to "-1", name to "Arr"
 *    Arr(1,) will set rowStr to  "1", colStr to "-1", name to "Arr"
 *    Arr(,1) will set rowStr to "-1", colStr to  "1", name to "Arr"
 *    Arr(,)  will set rowStr to "-1", colStr to "-1", name to "Arr"
 *    Arr()   will set rowStr to "-1", colStr to "-1", name to "Arr"
 *
 * @note It will remove all white spaces between array index
 *
 * The input array string can be a form of:
 *    array[3,3], table(1,b), array(arr1(1,1), arr2(2,2)), etc
 *
 * If input string is array1(1,1) the output will be:
 *    rowStr = "1", colStr = "1", name = "array1"
 * If input string is array2(var1, arr1(1,1)) the output will be:
 *    rowStr = "var1", colStr = "arr1(1,1)", name = "array2"
 * If input string is array2(var1, arr1(1,  c(a, b))) the output will be:
 *    rowStr = "var1", colStr = "arr1(1,c(a,b))", name = "array2"
 *
 * @param  str  input array string
 * @param  rowStr  output row string index
 * @param  colStr  output column string index
 * @param  name  output array name
 * @param  bracketPair  bracket pair used in the input array, such as "[]", "()"
 *
 * @exception UtilityException  if parentheses are not balanced, eg) Arr(a, b(3,2
 */
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayIndexVar(const std::string &str, std::string &rowStr,
                                      std::string &colStr, std::string &name,
                                      const std::string &bracketPair)
{
   std::string str1;
   str1 = Trim(str, BOTH, true, true);
   str1 = RemoveAll(str1, ' ');
   std::string openStr = bracketPair.substr(0,1);
   std::string closeStr = bracketPair.substr(1,1);
   
   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("GmatStringUtil::GetArrayIndexVar() str=%s\n   str1=%s\n",
       str.c_str(), str1.c_str());
   #endif

   // Check if bracket is balanced
   if (!IsBracketBalanced(str1, bracketPair))
   {
      UtilityException ex;
      ex.SetDetails("Text has unbalanced brackets: \"%s\"\n", str.c_str());
      throw ex;
   }

   rowStr = "-1";
   colStr = "-1";
   name = str1;

   std::string::size_type openBracket = str1.find(openStr);

   // if there is no opening bracket
   if (openBracket == str1.npos)
      return;

   name = str1.substr(0, openBracket);

   Integer length = str1.size();
   std::string str2 = str1.substr(openBracket+1, length-openBracket-2);

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage("   str2=%s\n", str2.c_str());
   #endif

   // if array index is empty, Arr()
   if (str2 == "")
      return;

   Integer comma;

   // get array comma index
   GetArrayCommaIndex(str1, comma, bracketPair);
      
   UnsignedInt closeBracket = str1.size() - 1;
   
   // if single array, such as a(5), b(a(5,5)), set row string as "1"
   if (comma == -1)
   {
      rowStr = "1";
      colStr = str1.substr(openBracket+1, closeBracket-openBracket-1);
   }
   else
   {
      rowStr = str1.substr(openBracket+1, comma-openBracket-1);
      colStr = str1.substr(comma+1, closeBracket-comma-1);
   }

   name = str1.substr(0, openBracket);

   if (rowStr == "")
      rowStr = "-1";

   if (colStr == "")
      colStr = "-1";

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("StringUtil::GetArrayIndexVar() rowStr=%s, colStr=%s, name=%s\n",
       rowStr.c_str(), colStr.c_str(), name.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void GetArrayIndex(const std::string &str, Integer &row, Integer &col,
//                    std::string &name, const std::string &bracketPair)
//------------------------------------------------------------------------------
/*
 * Returns array integer row and column index.  This method calls
 * GetArrayIndexVar() and if row and column string index string is not valid
 * integer, it returns -1 as array index.
 *
 * @see GetArrayIndexVar()
 *
 * @param  str  input array string
 * @param  row  output row integer index, -1 if invalid index
 * @param  col  output column integer index, -1 if invalid index
 * @param  name  output array name
 * @param  bracketPair  bracket pair used in the input array, such as "[]", "()"
 */
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayIndex(const std::string &str, Integer &row,
                                   Integer &col, std::string &name,
                                   const std::string &bracketPair)
{
   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("StringUtil::GetArrayIndex() str=%s\n", str.c_str());
   #endif

   std::string rowStr;
   std::string colStr;
   row = -1;
   col = -1;
   Integer intVal;

   GetArrayIndexVar(str, rowStr, colStr, name, bracketPair);

   if (rowStr != "-1")
   {
      if (ToInteger(rowStr, intVal))
      {
         if (bracketPair == "()")
            row = intVal - 1; // array index start at 0
         else
            row = intVal;
      }
   }

   if (colStr != "-1")
   {
      if (ToInteger(colStr, intVal))
      {
         if (bracketPair == "()")
            col = intVal - 1; // array index start at 0
         else
            col = intVal;
      }
   }

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("StringUtil::GetArrayIndex() row=%d, col=%d, name=%s\n", row, col, name.c_str());
   #endif
}


//------------------------------------------------------------------------------
// void GetArrayIndex(const std::string &str, std::string &rowStr,
//                    std::string &colStr, Integer &row, Integer &col,
//                    std::string &name, const std::string &bracketPair)
//------------------------------------------------------------------------------
/*
 * Returns array row and column index as string and integer. This method calls
 * GetArrayIndexVar() and if row and column string index is "-1", it returns
 * -1 as array index.
 *
 * @see GetArrayIndexVar()
 *
 * @param  str  input array string
 * @param  rowStr  output row string index
 * @param  colStr  output column string index
 * @param  row  output row integer index
 * @param  col  output column integer index
 * @param  name  output array name
 * @param  bracketPair  bracket pair used in the input array, such as "[]", "()"
 */
//------------------------------------------------------------------------------
void GmatStringUtil::GetArrayIndex(const std::string &str, std::string &rowStr,
                                   std::string &colStr, Integer &row, Integer &col,
                                   std::string &name, const std::string &bracketPair)
{
   row = -1;
   col = -1;
   Integer intVal;

   GetArrayIndexVar(str, rowStr, colStr, name, bracketPair);

   // array index starts at 0
   if (rowStr != "-1")
      if (ToInteger(rowStr, intVal))
         row = intVal - 1;

   if (colStr != "-1")
      if (ToInteger(colStr, intVal))
         col = intVal - 1;
}


//------------------------------------------------------------------------------
// void FindFirstAndLast(const std::string &str, char ch, Integer &first,
//                       Integer &last)
//------------------------------------------------------------------------------
/*
 * Finds first and last index of given chracter from the string.
 * if given chracter is not found it sets to -1
 *
 * @param  str  input string
 * @param  ch  input character to find
 * @param  first  output index of first input character found
 * @param  last  output index of last input character found
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
 * Finds open and close parentheses index from the string.
 * if parhenthesis is not found it sets to -1
 *
 * @param <str> input string
 * @param <openParen> set to index of first open parenthesis
 * @param <openParen> set to index of matching close parenthesis of openParen
 * @param <isOuterParen> set to true if item is enclosed with parentheses
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
      ("GmatStringUtil::FindParenMatch() str=%s, openParen=%d, closeParen=%d, "
       "isOuterParen=%d\n", str.c_str(), openParen, closeParen, isOuterParen);
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
 * @param <isOuterParen> set to true if item is enclosed with parentheses
 * @param <start> input starting index
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindMatchingParen(const std::string &str, Integer &openParen,
                                       Integer &closeParen, bool &isOuterParen,
                                       Integer start)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("FindMatchingParen() start=%d, str=%s\n", start, str.c_str());
   #endif

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
// void FindMatchingBracket(const std::string &str, Integer &openBracket,
//                          Integer &closeBracket, bool &isOuterBracket,
//                          const std::string &bracket, Integer start = 0)
//------------------------------------------------------------------------------
/*
 * Finds matching closing bracket of the first open bracket.
 * if parhenthesis is not found it sets to -1
 *
 * @param <str> input string
 * @param <openBracket> set to index of first open bracket
 * @param <closeBracket> set to index of matching close bracket of openBracket
 * @param <isOuterBracket> set to true if item is enclosed with bracket
 * @param <bracket> input open/close bracket (eg: "()", "{}", "[]", "<>")
 * @param <start> input starting index
 */
//------------------------------------------------------------------------------
void GmatStringUtil::FindMatchingBracket(const std::string &str, Integer &openBracket,
                                         Integer &closeBracket, bool &isOuterBracket,
                                         const std::string &bracket, Integer start)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("FindMatchingBracket() start=%d, str=%s\n", start, str.c_str());
   #endif

   // initialize output parameters
   openBracket = -1;
   closeBracket = -1;
   isOuterBracket = false;

   int length = str.size();
   Integer openCounter = 0;

   for (int i=start; i<length; i++)
   {
      if (str[i] == bracket[0])
      {
         openCounter++;
         if (openCounter == 1)
            openBracket = i;
      }
      else if (str[i] == bracket[1])
      {
         openCounter--;
         closeBracket = i;
         if (openCounter == 0)
            break;
      }
   }

   if (openBracket == 0 && closeBracket == length-1)
      isOuterBracket = true;

   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("FindMatchingBracket() str=%s, openBracket=%d, closeBracket=%d, "
       "isOuterBracket=%d\n", str.c_str(), openBracket, closeBracket,
       isOuterBracket);
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
// bool IsEnclosedWith(const std::string &str, const string &enclosingStr)
//------------------------------------------------------------------------------
/*
 * Returns true if item is enclosed with given enclosing string.
 *
 * @param  str  Input string to check for enclosed with
 * @param  enclosingStr  Input enclosing string to look for enclosed with
 *
 * @return true if string is enclosed with given enclosing string
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsEnclosedWith(const std::string &str,
                                    const std::string &enclosingStr)
{
   if (str == "")
      return false;
   
   return (StartsWith(str, enclosingStr) && EndsWith(str, enclosingStr));
}


//------------------------------------------------------------------------------
// bool IsEnclosedWithExtraParen(const std::string &str, bool checkOps = true)
//------------------------------------------------------------------------------
/*
 * Returns true if item is enclosed with extra parentheses
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

   // if enclosed with open and close parentheses
   if (openParen == 0 && closeParen == length-1)
   {
      // check for double parentheses
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
         else if (ToReal(substr, rval))
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
// bool IsEnclosedWithBraces(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if entire string is enclosed with braces
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsEnclosedWithBraces(const std::string &str)
{
   return (StartsWith(str, "{") && EndsWith(str, "}"));
}

//------------------------------------------------------------------------------
// bool IsEnclosedWithBrackets(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if entire string is enclosed with brackets (in cases of
 * arrays of Real numbers, e.g.)
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsEnclosedWithBrackets(const std::string &str)
{
   return (StartsWith(str, "[") && EndsWith(str, "]"));
}

//------------------------------------------------------------------------------
// bool IsParenBalanced(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if parentheses are balanced (no mismatching parentheses)
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsParenBalanced(const std::string &str)
{
   int length = str.size();
   Integer openCounter = 0;
   bool retval = false;

   for (int i=0; i<length; i++)
   {
      if (str[i] == '(')
         openCounter++;
      else if (str[i] == ')')
         openCounter--;
   }

   if (openCounter == 0)
      retval = true;

   return retval;
}


//------------------------------------------------------------------------------
// bool IsBracketBalanced(const std::string &str, const std::string &bracketPair)
//------------------------------------------------------------------------------
/*
 * @return true if brackets are balanced (no mismatching brackets) or
 *         no brackets found
 *
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsBracketBalanced(const std::string &str,
                                       const std::string &bracketPair)
{
   char open = bracketPair[0];
   char close = bracketPair[1];
   int length = str.size();
   Integer openCounter = 0;
   bool retval = true;

   for (int i=0; i<length; i++)
   {
      if (str[i] == open)
         openCounter++;
      else if (str[i] == close)
         openCounter--;
   }

   if (openCounter != 0)
      retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// bool AreAllBracketsBalanced(const std::string &str,
//                             const std::string &allPairs)
//------------------------------------------------------------------------------
bool GmatStringUtil::AreAllBracketsBalanced(const std::string &str,
                                            const std::string &allPairs)
{
   #ifdef DEBUG_BALANCED_BRACKETS
      MessageInterface::ShowMessage(
          "Entering AreAllBracketsBalanced with str = %s and allPairs = %s\n",
          str.c_str(), allPairs.c_str());
   #endif
   Integer count = allPairs.size();
   if (count%2 == 1)
      throw UtilityException("Invalid number of Bracket pairs\n");
   Integer numPairs = count / 2;
   std::string openBrackets  = allPairs.substr(0, numPairs);
   std::string closeBrackets = allPairs.substr(numPairs);
   #ifdef DEBUG_BALANCED_BRACKETS
      MessageInterface::ShowMessage(
          "   openBrackets = %s and closeBrackets = %s\n",
          openBrackets.c_str(), closeBrackets.c_str());
   #endif

   Integer openCounter = 0;
   IntegerArray bracketsFound;

   int length = str.size();
   bool retval = true;

   for (int i=0; i<length; i++)
   {
      #ifdef DEBUG_BALANCED_BRACKETS
         //MessageInterface::ShowMessage(
         //    "   Now checking character %c at position %d\n", str[i], i);
      #endif
      for (Integer jj = 0; jj < numPairs; jj++)
      {
         if (str[i] == openBrackets[jj])
         {
            #ifdef DEBUG_BALANCED_BRACKETS
               MessageInterface::ShowMessage(
                   "   found an open bracket and it = %c\n", str[i]);
            #endif
            bracketsFound.push_back(jj);
            openCounter++;
         }
         else if (str[i] == closeBrackets[jj])
         {
            #ifdef DEBUG_BALANCED_BRACKETS
               MessageInterface::ShowMessage(
                   "   found a close bracket and it = %c\n", str[i]);
            #endif
            if ((openCounter > 0) && (bracketsFound.at(openCounter-1) == jj))
            {
               bracketsFound.pop_back();
               openCounter--;
            }
            else
               return false;
         }
      }
   }

   if (openCounter != 0)
      retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// bool IsOuterParen(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if outer parentheses is not part of ^(#) or array.
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

   if (openParen == 0 && closeParen == length-1)
   {
      // make sure ending ) is not part of ^(-1)
      std::string::size_type lastOpenParen = str.find_last_of('(');
      std::string::size_type lastCloseParen = str.find_last_of(')', closeParen-1);

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

      #if DEBUG_STRING_UTIL
      MessageInterface::ShowMessage
         ("===> IsOuterParen() isOuterParen=%d\n", isOuterParen);
      #endif

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
// bool IsCommaPartOfArray(const std::string &str, Integer start)
//------------------------------------------------------------------------------
/*
 * Finds if first comma after start position is part of an array.
 * It first removes outer parentheses and check for comma.
 *
 * It will true for A(arr2(2,2)), A( b(x,y), I)
 * It will false for A(1,2), A(1, b(c,d))
 *
 * @param  str  input string
 * @param  start  input starting position for searching
 *
 * @return  true if comma is part of an arry, false else
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsCommaPartOfArray(const std::string &str, Integer start)
{
   // First remove parentheses around array
   std::string::size_type open = str.find_first_of("(");
   std::string::size_type close = str.find_last_of(")");
   std::string str1 = str.substr(open+1, close-open-1);

   close = str1.find(")");

   // if close paren not found, return false
   if (close == str.npos)
      return false;

   // if comma is after open paren, comma is part of array
   open = str1.find("(");
   UnsignedInt comma = str1.find(",");

   bool retval = false;

   if (comma > open)
      retval = true;

   #if DEBUG_ARRAY_INDEX
   MessageInterface::ShowMessage
      ("IsCommaPartOfArray() str1=<%s>, comma=%u, open=%u, retval=%d\n",
       str1.c_str(), comma, open, retval);
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool IsBracketPartOfArray(const std::string &str,
//                           const std::string &bracketPairs,
//                           bool checkOnlyFirst)
//------------------------------------------------------------------------------
/*
 * Check if string is part of array.
 *
 * @param str  input string
 * @param bracketPairs  bracket pairs used in checking ("()", "([)]")
 * @param checkOnlyFirst  true if checking for first bracket pair
 *
 * return true if parentheses or square bracket is part of an array.
 *    For example: (2,2) or (abc,def) or [2,2], [i,j]
 *
 * return false if no parentheses found or non-alphanumeric char found inside
 * parentheses.
 *    For example: abc, abc((2,2)), (1,), (3,2]
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsBracketPartOfArray(const std::string &str,
                                          const std::string &bracketPairs,
                                          bool checkOnlyFirst)
{
   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsBracketPartOfArray() str=%s, bracketPairs=%s, "
       "checkOnlyFirst=%d\n", str.c_str(), bracketPairs.c_str(), checkOnlyFirst);
   #endif

   std::string str1 = RemoveAll(str, ' ');
   bool ret = true;
   std::string::size_type index1, index2, comma;
   std::string substr;
   Integer count = bracketPairs.size();
   char openChar = 'x';
   char closeChar = 'x';

   if (count%2 == 1)
      throw UtilityException("Invalid number of Bracket pair\n");

   std::string openBrackets = bracketPairs.substr(0, count/2);
   std::string closeBrackets = bracketPairs.substr(count/2);

   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage
      ("   openBrackets=%s, closeBrackets=%s\n", openBrackets.c_str(),
       closeBrackets.c_str());
   #endif

   index1 = str1.find_first_of(openBrackets);
   //MessageInterface::ShowMessage("   index1=%u\n", index1);

   if (index1 == str1.npos)
   {
      #if DEBUG_STRING_UTIL
      MessageInterface::ShowMessage("   No open bracket found.\n");
      #endif

      return false;
   }


   openChar = str[index1];
   //MessageInterface::ShowMessage("   openChar=%c\n", openChar);

   if (checkOnlyFirst)
      index2 = str1.find_first_of(closeBrackets, index1);
   else
      index2 = str1.find_last_of(closeBrackets);

   //MessageInterface::ShowMessage("   index2=%u\n", index2);

   if (index2 == str1.npos)
   {
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage("   No close bracket found\n");
      #endif

      return false;
   }

   closeChar = str[index2];
   //MessageInterface::ShowMessage("   closeChar=%c\n", closeChar);

   if ((openChar == '(' && closeChar == ']') ||
       (openChar == '[' && closeChar == ')'))
   {
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage("   open and close bracket don't match.\n");
      #endif

      return false;
   }


   //-----------------------------------------------------------------
   // str1 does not include open and close bracket
   //-----------------------------------------------------------------
   std::string str2 = str1.substr(index1+1, index2-index1-1);
   //MessageInterface::ShowMessage("   str1=<%s>/n", str1.c_str());

   if (str2 == "")
   {
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage("   It is empty sub-string.\n");
      #endif

      return false;
   }


   Integer length = str2.size();
   comma = str2.find(',');
   //MessageInterface::ShowMessage("   comma=%u\n", comma);

   //-----------------------------------------------------------------
   // if single dimension array
   //-----------------------------------------------------------------
   if (comma == str2.npos)
   {
      substr = str2.substr(0, length-1);
      //MessageInterface::ShowMessage("   1st=%s\n", substr.c_str());

      if (IsSingleItem(substr))
         ret = true;
      else
         ret = false;

      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage
         ("   It is single dimenstion array. returning ret=%d\n", ret);
      #endif

      return ret;
   }

   //-----------------------------------------------------------------
   // It's double dimension array
   //-----------------------------------------------------------------
   substr = str2.substr(0, comma-1);

   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage("   1st=%s\n", substr.c_str());
   #endif

   if (!IsSingleItem(substr))
   {
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage
         ("   It is double dimenstion array. 1st index is not a single item\n");
      #endif

      //return IsBracketPartOfArray(substr, bracketPairs, checkOnlyFirst);
      return false;
   }

   substr = str2.substr(comma+1, length-comma-1);
   //MessageInterface::ShowMessage("   2nd=%s\n", substr.c_str());

   if (!IsSingleItem(substr))
   {
      #if DEBUG_STRING_UTIL_ARRAY
      MessageInterface::ShowMessage
         ("   It is double dimenstion array. 2nd index is not a single item\n");
      #endif

      return false;
   }

   #if DEBUG_STRING_UTIL_ARRAY
   MessageInterface::ShowMessage
      ("   It is double dimension array. returning ret=%d\n", ret);
   #endif

   return ret;
}


//------------------------------------------------------------------------------
// bool IsParenPartOfArray(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if parentheses is part of an array.
 *    For example: (2,2) or (abc,def)
 *
 * return false if no parentheses found or non-alphanumeric char found inside
 * parentheses.
 *    For example: abc, abc((2,2))
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsParenPartOfArray(const std::string &str)
{
   //MessageInterface::ShowMessage
   //   ("===> GmatStringUtil::IsParenPartOfArray() str=%s\n", str.c_str());

   return IsBracketPartOfArray(str, "()", false);

}


//------------------------------------------------------------------------------
// bool IsThereEqualSign(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Checks if there is equal sign (=) not enclosed with single quotes.
 * It will return when it finds first equal sign not in quotes.
 *
 * @param  return true if it finds equal sign not in quotes, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsThereEqualSign(const std::string &str)
{
   Integer size = str.size();
   bool inQuotes = false;

   for (Integer i=0; i<size; i++)
   {
      if (str[i] == '\'')
      {
         if (inQuotes)
            inQuotes = false;
         else
            inQuotes = true;
      }
      else if (str[i] == '=')
      {
         if (!inQuotes)
            return true;
      }
   }

   return false;
}


//------------------------------------------------------------------------------
// bool IsThereMathSymbol(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Checks if there is any math symbos, "-,+,*,/,^,=,<,>", not enclosed with
 * single quotes. It will return when it finds first math symbol not in quotes.
 *
 * @param  return true if it finds any math symbol not in qotes, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsThereMathSymbol(const std::string &str)
{
   Integer size = str.size();
   bool inQuotes = false;

   for (Integer i=0; i<size; i++)
   {
      if (str[i] == '\'')
      {
         if (inQuotes)
            inQuotes = false;
         else
            inQuotes = true;
      }
      else if (str[i] == '+' || str[i] == '-' || str[i] == '*' || str[i] == '/' ||
               str[i] == '^' || str[i] == '=' || str[i] == '<' || str[i] == '>')
      {
         if (!inQuotes)
            return true;
      }
   }

   return false;
}


//------------------------------------------------------------------------------
// bool HasNoBrackets(const std::string &str,
//                    bool parensForArraysAllowed = true)
//------------------------------------------------------------------------------
/*
 * return true if string does not contain any brackets, braces, or parentheses,
 * or only contains parentheses as part of an arrray element, if
 * parensForArraysAllowed is true.
 *
 * return false if there are brackets, braces, or parentheses, unless
 * existing parentheses are allowed for array elements.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::HasNoBrackets(const std::string &str,
                                   bool parensForArraysAllowed)
{
   #ifdef DEBUG_NO_BRACKETS
      MessageInterface::ShowMessage("Entering HasNoBrackets with str = %s\n",
         str.c_str());
   #endif
   std::string str1 = str;
   std::string str2 = str;
   std::string left, right, arrName;
   bool hasNone = true;
   if ((str1.find('(') != str1.npos) || (str1.find(')') != str1.npos))
   {
      if (parensForArraysAllowed)
      {
         Integer open = 0, close = 0;
         bool    isOuter, done = false;
         while (!done)
         {
            GmatStringUtil::FindMatchingParen(str1, open, close, isOuter);
            if (((open == -1) && (close != -1)) ||
                ((open != -1) && (close == -1)) )
                return false;
            else if ((open == -1) && (close == -1))
               done = true;
            else
            {
               str2 = str1.substr(0, close + 1);
               //if (!IsParenPartOfArray(str2))  return false;
               GmatStringUtil::GetArrayIndexVar(str2, left, right, arrName, "()");
               #ifdef DEBUG_NO_BRACKETS
                  MessageInterface::ShowMessage("   left = %s, right = %s, arrName = %s\n",
                     left.c_str(), right.c_str(), arrName.c_str());
               #endif
               if ((arrName == "") || (left == "-1") || (right == "-1"))
               {
                  #ifdef DEBUG_NO_BRACKETS
                     MessageInterface::ShowMessage
                        ("   NOT a proper array ... returning false\n");
                  #endif
                  return false;
               }
               // To remove compiler warning:
               // forcing value to bool 'true' or 'false' (performance warning)
               //hasNone = hasNone + HasNoBrackets(left, parensForArraysAllowed)
               //          + HasNoBrackets(right, parensForArraysAllowed);
               hasNone = HasNoBrackets(left, parensForArraysAllowed) ||
                  HasNoBrackets(right, parensForArraysAllowed) || hasNone;
               if (!hasNone)
               {
                  #ifdef DEBUG_NO_BRACKETS
                     MessageInterface::ShowMessage
                        ("   left or right contains non-array parens ... returning false\n");
                  #endif
                  return false;
               }
               str1 = str1.substr(close+1);
            }
         }
      }
      else
         return false;
   }
   if ((str.find('[') != str.npos) || (str.find(']') != str.npos)) return false;
   if ((str.find('{') != str.npos) || (str.find('}') != str.npos)) return false;

   return true;
}


//------------------------------------------------------------------------------
// bool IsSingleItem(const std::string &str)
//------------------------------------------------------------------------------
/*
 * return true if string does not have arithmetic operators or '.' or
 * enclosed with double parentheses. For example -123.0, abc, (a), (sat.X), (-123.0).
 *
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
   if (ToReal(str, rval))
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
 * This method removs extra pair of parentheses.
 * If input string is "(a(1,1) + 10.0)" it return a(1,1) + 10.0.
 *
 * *** NOTES ***
 * This method is not complete and needs more testing.
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

   // remove outer parentheses
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

   // go through each char and remove extra parentheses
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
         // check one more outer parentheses
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


//------------------------------------------------------------------------------
// std::string RemoveOuterString(const std::string &str, const std::string &start,
//                               const std::string end)
//------------------------------------------------------------------------------
/*
 * This method removes outer pair of bracket if it has one.
 * If input string is "(a(1,1) + 10.0)" it return a(1,1) + 10.0.
 *
 * @param  str  Input string
 * @param  start  Starting string to be checked and removed
 * @param  end  Ending string to be checked and removed
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveOuterString(const std::string &str,
                                               const std::string &start,
                                               const std::string &end)
{
   #if DEBUG_STRING_UTIL
   MessageInterface::ShowMessage
      ("RemoveOuterString() entering str=\"%s\", start='%s', end='%s'\n", str.c_str(),
       start.c_str(), end.c_str());
   #endif

   std::string str1 = str;

   if (StartsWith(str, start) && EndsWith(str, end))
      str1 = str.substr(1, str.size() - 2);

   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveEnclosingString(const std::string &str, const std::string &enStr)
//------------------------------------------------------------------------------
/*
 * Removes enclosing string if actually enclosed with the string.
 *
 * @param  str  Input string to remove given enclosnig string
 * @param  enStr The enclosing string to check and remove
 *
 * @return string with enclosing string removed if found, original string otherwise
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveEnclosingString(const std::string &str,
                                                  const std::string &enStr)
{
   std::string str1 = str;

   // First check if string is enclosed with given string
   if (IsEnclosedWith(str, enStr))
   {
      str1 = str.substr(enStr.size());
      str1 = str1.substr(0, str1.size() - enStr.size());
   }

   #ifdef DEBUG_REMOVE_ENCLOSING_STRING
   MessageInterface::ShowMessage
      ("GmatStringUtil::RemoveEnclosingString() returning <%s>\n", str1.c_str());
   #endif

   return str1;
}


//------------------------------------------------------------------------------
// std::string RemoveInlineComment(const std::string &str, const std::string &comStr)
//------------------------------------------------------------------------------
/*
 * Removes inline comments from the input string.
 * ex) "Create String srt1; % str2" will return "Create String str1;"
 *
 * @param  str  Input string to remove given enclosnig string
 * @param  cmStr The inline comment string to use for removing
 *
 * @return string with inline comments removed
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::RemoveInlineComment(const std::string &str,
                                                const std::string &cmStr)
{
   std::string str1 = str;
   std::string::size_type index = str.find(cmStr);
   if (index == str1.npos)
      return str1;
   else
      return str1.substr(0, index);
}

//------------------------------------------------------------------------------
// std::string ParseFunctionName(const std::string &str)
//------------------------------------------------------------------------------
/**
 * Parses function name from the following syntax:
 *    [out] = Function1(in);
 *    Function2(in);
 *    Function3;
 *
 * @param  str  Input string
 * @return  Function name or blank if name not found
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::ParseFunctionName(const std::string &str)
{
   if (str == "")
      return "";
   
   // Remove all spaces and semicolons
   std::string str1 = RemoveAll(str, ' ');
   while (str1[str1.size()-1] == ';')
      str1.erase(str1.size()-1, 1);
   
   std::string funcName;
   if (str1.find("[") != str1.npos)
   {
      std::string::size_type index1 = str1.find("=");
      std::string::size_type index2 = str1.find("(", index1 + 1);
      if (index2 == str1.npos)
         funcName = str1.substr(index1+1);
      else
         funcName = str1.substr(index1+1, index2-index1-1);
   }
   else
   {
      std::string::size_type index2 = str1.find("(");
      if (index2 == str1.npos)
         funcName = str1.substr(0);
      else
         funcName = str1.substr(0, index2);
   }
   
   return funcName;
}


//------------------------------------------------------------------------------
// std::string AddEnclosingString(const std::string &str, const std::string &enStr)
//------------------------------------------------------------------------------
/**
 * Put string in enclosing string
 *
 * @param  str  Input string
 * @param  enStr Enclosing string
 * @return  String with enclosing string
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::AddEnclosingString(const std::string &str,
                                               const std::string &enStr)
{
   if (str == "")
      return "";
   
   std::string str1 = str;
   str1 = enStr + str + enStr;
   return str1;
}


//------------------------------------------------------------------------------
// std::string GetInvalidNameMessageFormat()
//------------------------------------------------------------------------------
/**
 * Returns invalid object name message.
 */
//------------------------------------------------------------------------------
std::string GmatStringUtil::GetInvalidNameMessageFormat()
{
   return ("\"%s\" is not a valid name. Please reenter a valid name.\n\n"
           "[Name cannot be a GMAT keyword or command type name, such as \"GMAT\", "
           "\"Create\", \"Propagate\", \"function\" and must begin with a letter, "
           "which may be followed by any combination of letters, digits, and underscores.]");
}


//------------------------------------------------------------------------------
// StringArray DecomposeBy(const std::string &str, const std::string &delim)
//------------------------------------------------------------------------------
/**
 * Returns the first token and the rest
 * e.g) "cd c:/my test directory" returns if delimiter is " ".
 *   cd
 *   c:/my test directory
 *
 * @param  str  Input string
 * @param  delim Delimiter
 * @return  First token and the rest or empty string array if token not found
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::DecomposeBy(const std::string &str,
                                        const std::string &delim)
{
   StringArray parts;
   if (str == "")
      return parts;
   
   std::string::size_type index1 = str.find_first_of(delim);
   if (index1 != str.npos)
   {
      parts.push_back(str.substr(0, index1));
      parts.push_back(str.substr(index1 + 1));
      return parts;
   }
   else
   {
      return parts;
   }
}


//------------------------------------------------------------------------------
// bool StartsWith(const std::string &str, const std::string &value)
//------------------------------------------------------------------------------
/*
 * Returns true if string starts with value and false if it does not.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::StartsWith(const std::string &str, const std::string &value)
{
   return (str.size() >= value.size()) &&
      (str.substr(0, value.size()) == value);
}


//------------------------------------------------------------------------------
// bool EndsWith(const std::string &str, const std::string &value)
//------------------------------------------------------------------------------
/*
 * Returns true if string ends with value and false if it does not.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::EndsWith(const std::string &str, const std::string &value)
{
   return (str.size() >= value.size()) &&
          (str.substr(str.size() - value.size(), value.size()) == value);
}


//------------------------------------------------------------------------------
// bool EndsWithPathSeparator(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Returns true if string ends with path separator (/ or \\) and false if it does not.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::EndsWithPathSeparator(const std::string &str)
{
   if (EndsWith(str, "/") || EndsWith(str, "\\"))
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool IsValidName(const std::string &str, bool ignoreBracket)
//------------------------------------------------------------------------------
/*
 * Checks for valid name.
 *
 * Returns true if string is:
 *    1. keyword (not "GMAT", "Create", or "function") and
 *    2. does not start with a number and
 *    3. contains only alphanumeric (underscore is allowed)
 *
 * @param  str  The input string to check for valid name
 * @param  ignoreBracket  true if input string has bracket and to ignore it
 *                      when checking (false)
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidName(const std::string &str, bool ignoreBracket)
{
   if (str == "")
      return false;

   // strip blanks first (loj: 2008.08.27)
   std::string str1 = Strip(str);

   if (str1 == "GMAT" || str1 == "Create" || str1 == "function")
   {
      #ifdef DEBUG_VALID_NAME
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsValidName(%s) returning false\n", str1.c_str());
      #endif
      return false;
   }

   // First letter must start with alphabet
   if (!isalpha(str1[0]))
   {
      #ifdef DEBUG_VALID_NAME
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsValidName(%s) returning false\n", str1.c_str());
      #endif
      return false;
   }

   // if ignoring open parenthesis, remove it first
   if (ignoreBracket)
   {
      std::string::size_type openParen = str1.find_first_of("([");
      if (openParen != str1.npos)
      {
         str1 = str1.substr(0, openParen);
         str1 = Trim(str1);
      }
   }

   for (UnsignedInt i=1; i<str1.size(); i++)
   {
      if (!isalnum(str1[i]) && str1[i] != '_')
      {
         #ifdef DEBUG_VALID_NAME
         MessageInterface::ShowMessage
            ("GmatStringUtil::IsValidName(%s) returning false\n", str1.c_str());
         #endif
         return false;
      }
   }

   #ifdef DEBUG_VALID_NAME
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidName(%s) returning true\n", str1.c_str());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool IsValidNumber(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Checks if string is valid integer or real number.
 *
 * @param  str  The input string to check for valid number
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsValidNumber(const std::string &str)
{
   std::string str1 = Strip(str);
   if (str1 == "")
      return false;

   Real rval;
   Integer ival;

   if (!ToInteger(str1, ival, true) && !ToReal(str1, rval, true))
   {
      #ifdef DEBUG_VALID_NUMBER
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsValidNumber(%s) returning false\n", str1.c_str());
      #endif
      return false;
   }

   #ifdef DEBUG_VALID_NUMBER
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsValidNumber(%s) returning true\n", str1.c_str());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// bool IsBlank(const std::string &text, bool ignoreEol = false)
//------------------------------------------------------------------------------
/*
 * Checks if text has only blank spaces.
 *
 * @param  text  input text
 * @param  ignoreEol  Set this to true if end-of-line char to be ignored
 *
 * @return true if text has only blank spaces
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsBlank(const std::string &text, bool ignoreEol)
{
   std::string str = Trim(text, GmatStringUtil::BOTH);

   if (ignoreEol)
   {
      str = RemoveAll(str, '\n');
      str = RemoveAll(str, '\r');
   }

   if (str == "")
      return true;
   else
      return false;
}


//------------------------------------------------------------------------------
// bool HasMissingQuote(const std::string &str, const std::string &quote)
//------------------------------------------------------------------------------
/*
 * Checks if string has missing starting or ending quote.
 *
 * @param  str    input text
 * @param  quote  quote to be used for checking
 *
 * @return true if string has missing quote, false otherwise
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::HasMissingQuote(const std::string &str,
                                     const std::string &quote)
{
   if ((StartsWith(str, quote) && !EndsWith(str, quote)) ||
       (!StartsWith(str, quote) && EndsWith(str, quote)))
      return true;

   return false;
}


//------------------------------------------------------------------------------
// bool IsMathEquation(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Checks if string is a math equation with numbers and variables only.
 * Math operators are "/+-*^".
 * If string is enclosed with single quotes, it just returns false.
 * If string has function or array, it returns false.
 */
//------------------------------------------------------------------------------
bool GmatStringUtil::IsMathEquation(const std::string &str)
{
   #if DEBUG_MATH_EQ > 1
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsMathEquation() entered, str=<%s>\n", str.c_str());
   #endif

   if (IsEnclosedWith(str, "'"))
   {
      #if DEBUG_MATH_EQ
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsMathEquation(%s) returning false\n", str.c_str());
      #endif
      return false;
   }

   StringArray parts = SeparateBy(str, "+-*/^");
   Integer numParts = parts.size();

   #if DEBUG_MATH_EQ > 1
   MessageInterface::ShowMessage("..... has %d parts\n", numParts);
   MessageInterface::ShowMessage("..... check if it has more than 1 part\n");
   #endif

   // check if it has more than 1 part
   if (numParts == 1)
   {
      #if DEBUG_MATH_EQ
      MessageInterface::ShowMessage
         ("GmatStringUtil::IsMathEquation(%s) returning false\n", str.c_str());
      #endif
      return false;
   }

   // check if it is array or function
   #if DEBUG_MATH_EQ > 1
   MessageInterface::ShowMessage("..... check if parentheses part of array\n");
   #endif

   std::string::size_type index = str.find_first_of("(");
   if (index != str.npos)
   {
      // check if parentheses balanced
      #if DEBUG_MATH_EQ > 1
      MessageInterface::ShowMessage("..... check if parentheses is balanced\n");
      #endif
      if (!IsParenBalanced(str))
      {
         #if DEBUG_MATH_EQ
         MessageInterface::ShowMessage
            ("GmatStringUtil::IsMathEquation(%s) returning false\n", str.c_str());
         #endif
         return false;
      }
   }

   // go through each part, ignoring parentheses
   for (Integer i=0; i<numParts; i++)
   {
      std::string str1 = parts[i];
      #if DEBUG_MATH_EQ > 1
      MessageInterface::ShowMessage("   '%s'\n", str1.c_str());
      #endif

      // remove ()
      str1 = RemoveAll(str1, '(');
      str1 = RemoveAll(str1, ')');

      #if DEBUG_MATH_EQ > 1
      MessageInterface::ShowMessage("   after () removed '%s'\n", str1.c_str());
      #endif

      // check if it is valid name
      #if DEBUG_MATH_EQ > 1
      MessageInterface::ShowMessage("..... check if is valid name\n");
      #endif

      if (!IsValidName(str1) && !IsValidNumber(str1))
      {
         #if DEBUG_MATH_EQ
         MessageInterface::ShowMessage
            ("GmatStringUtil::IsMathEquation(%s) returning false\n", str.c_str());
         #endif
         return false;
      }
   }

   #if DEBUG_MATH_EQ
   MessageInterface::ShowMessage
      ("GmatStringUtil::IsMathEquation(%s) returning true\n", str.c_str());
   #endif

   return true;
}


//------------------------------------------------------------------------------
// Integer NumberOfOccurrences(const std::string &str, const char c)
//------------------------------------------------------------------------------
/*
 * Counts the number of occurrences of the character c in the string str.
 *
 * @param  str  input string
 * @param  c    character
 *
 * @return number of occurrences of c in str
 */
//------------------------------------------------------------------------------
Integer GmatStringUtil::NumberOfOccurrences(const std::string &str, const char c)
{
   Integer count = 0;
   Integer strSz = (Integer) str.size();
   for (Integer ii = 0; ii < strSz; ii++)
      if (str[ii] == c)  count++;

   return count;
}


//------------------------------------------------------------------------------
// StringArray GetVarNames(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Searchs all variable names and returns in a std::vector array.
 *
 * @see IsValidName()
 * @param  str  input string to search for names
 * @return   item names in array
 */
//------------------------------------------------------------------------------
StringArray GmatStringUtil::GetVarNames(const std::string &str)
{
   std::string str1 = str;
   std::string name;
   StringArray itemNames;
   Real rval;

   if (str == "")
      return itemNames;

   for (UnsignedInt i=0; i<str1.size(); i++)
   {
      if (isalnum(str1[i]) || str1[i] == '_')
      {
         name = name + str1[i];
      }
      else
      {
         if (name != "" && !ToReal(name, rval))
         {
            // just add new names
            if (find(itemNames.begin(), itemNames.end(), name) == itemNames.end())
               itemNames.push_back(name);
         }
         name = "";
      }
   }

   // add last item
   if (name != "" && find(itemNames.begin(), itemNames.end(), name) == itemNames.end())
      itemNames.push_back(name);

   return itemNames;
}


//------------------------------------------------------------------------------
// void WriteStringArray(const StringArray &strArray, const std::string &desc = "",
//                       const std::string &prefix = "")
//------------------------------------------------------------------------------
void GmatStringUtil::WriteStringArray(const StringArray &strArray,
                                      const std::string &desc,
                                      const std::string &prefix)
{
   Integer arrSize = strArray.size();
   MessageInterface::ShowMessage("%s\n", desc.c_str());
   MessageInterface::ShowMessage("%sThere are %d strings:\n", prefix.c_str(), arrSize);
   for (Integer i = 0; i < arrSize; i++)
   {
      MessageInterface::ShowMessage("%s'%s'\n", prefix.c_str(), strArray[i].c_str());
   }
}

