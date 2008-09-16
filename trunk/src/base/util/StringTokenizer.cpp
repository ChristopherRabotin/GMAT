//$Header$
//-------------------------------------------------------------------------------
//                              StringTokenizer
//-------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus
// Created: 2004/05/14
//
/**
 * Definition of the StringTokenizer class base
 */
//-------------------------------------------------------------------------------

#include "StringTokenizer.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_STRING_TOKENIZER 1

//-------------------------------------
// public methods
//-------------------------------------

//-------------------------------------------------------------------------------
//  StringTokenizer()
//-------------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 * 
 */
//-------------------------------------------------------------------------------
StringTokenizer::StringTokenizer()
{
   delimiters = " ";
   countTokens = 0;
}


//-------------------------------------------------------------------------------
//  StringTokenizer(const std::string &str, const std::string &delim)
//-------------------------------------------------------------------------------
/**
 * Creates constructor with parameters.
 *
 * @param <str>   Given String
 * @param <delim> Given delimiters
 * 
 */
//-------------------------------------------------------------------------------
StringTokenizer::StringTokenizer(const std::string &str, const std::string &delim)
{
   #if DEBUG_STRING_TOKENIZER
   MessageInterface::ShowMessage
      ("StringTokenizer() str=<%s>, delim=<%s>\n", str.c_str(), delim.c_str());
   #endif
   
   delimiters = delim;
   Parse(str);
}


//-------------------------------------------------------------------------------
//  ~StringTokenizer()
//-------------------------------------------------------------------------------
/**
 * Destructor.
 * 
 */
//-------------------------------------------------------------------------------
StringTokenizer::~StringTokenizer()
{
}

//-------------------------------------------------------------------------------
//  Integer CountTokens() const
//-------------------------------------------------------------------------------
/**
 * Get the number of tokens.
 *
 * @return number of tokens. 
 * 
 */
//-------------------------------------------------------------------------------
Integer StringTokenizer::CountTokens() const
{
   return countTokens;
}

//-------------------------------------------------------------------------------
//  std::string GetToken(const Integer loc) 
//-------------------------------------------------------------------------------
/**
 * Get the string from specifiying the token number.
 *
 * @param <loc>   Specified token number
 * @return        return string in the specified token number 
 * 
 */
//-------------------------------------------------------------------------------
std::string StringTokenizer::GetToken(const Integer loc)  const
{
   if (loc <= countTokens)
      return stringTokens.at(loc);

   return std::string("");
}

//-------------------------------------------------------------------------------
//  std::vector<string> GetAllTokens() const;
//-------------------------------------------------------------------------------
/**
 * Get all token string. 
 *
 * @return       all token strings 
 * 
 */
//-------------------------------------------------------------------------------
std::vector<std::string> StringTokenizer::GetAllTokens() const 
{
   return stringTokens; 
}


//-------------------------------------------------------------------------------
//  void Set(const std::string &str, const std::string &delim) 
//-------------------------------------------------------------------------------
/**
 * Set the string token.
 *
 * @param <str>   given string
 * @param <delim> given delimiters 
 * 
 */
//-------------------------------------------------------------------------------
void StringTokenizer::Set(const std::string &str, const std::string &delim)
{
   #if DEBUG_STRING_TOKENIZER
   MessageInterface::ShowMessage
      ("StringTokenizer::SSet() str=<%s>, delim=<%s>\n", str.c_str(), delim.c_str());
   #endif
   
   stringTokens.clear();
   delimiters = delim;
   Parse(str);
}

//-------------------------------------
// private methods
//-------------------------------------

//-------------------------------------------------------------------------------
//  void Parse(const std::string &str)
//-------------------------------------------------------------------------------
/**
 * Parse out the string for token
 *
 * @param <str>   Given string
 *
 */
//-------------------------------------------------------------------------------
void StringTokenizer::Parse(const std::string &str)
{
   #if DEBUG_STRING_TOKENIZER
   MessageInterface::ShowMessage
      ("StringTokenizer::Parse() str=%s, delimiters=<%s>\n", str.c_str(),
       delimiters.c_str());
   #endif
   
   std::string::size_type pos, lastPos;
   std::string substr;
   
   // Skip delimiters at beginning.
   lastPos = str.find_first_not_of(delimiters, 0);
   
   // Find first delimiter.
   pos = str.find_first_of(delimiters, lastPos);
   
   #if DEBUG_STRING_TOKENIZER > 1
   MessageInterface::ShowMessage("   lastPos=%u, pos=%u\n", lastPos, pos);
   #endif
   
   if (pos == str.npos)
   {
      stringTokens.push_back(str);
   }
   else
   {
      while (pos != str.npos || lastPos != str.npos)
      {
         #if DEBUG_STRING_TOKENIZER > 1
         MessageInterface::ShowMessage
            ("   lastPos=%u, pos=%u\n", lastPos, pos);
         #endif
         
         // Found a token, add it to the vector.
         substr = str.substr(lastPos, pos - lastPos);
            
         #if DEBUG_STRING_TOKENIZER > 1
         MessageInterface::ShowMessage("   substr=<%s>\n", substr.c_str());
         #endif
            
         stringTokens.push_back(str.substr(lastPos, pos - lastPos));
         
         // Skip delimiters.  Note the "not_of"
         lastPos = str.find_first_not_of(delimiters, pos);
         
         // Find next "delimiter"
         pos = str.find_first_of(delimiters, lastPos);
      }
   }
   
   // Set the number of token found
   countTokens = stringTokens.size();
   
   #if DEBUG_STRING_TOKENIZER
   MessageInterface::ShowMessage("   countTokens=%d\n", countTokens);
   #endif
   
}
