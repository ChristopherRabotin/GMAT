//$Header$
//------------------------------------------------------------------------------
//                              StringTokenizer
//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------

#include "StringTokenizer.hpp"

//-------------------------------------
// public methods
//-------------------------------------

//---------------------------------------------------------------------------
//  StringTokenizer()
//---------------------------------------------------------------------------
/**
 * Creates default constructor.
 *
 * 
 */
StringTokenizer::StringTokenizer()
{
   delimiters = " ";
   countTokens = 0;
}

//---------------------------------------------------------------------------
//  StringTokenizer(const std::string &str)
//---------------------------------------------------------------------------
/**
 * Creates constructor with parameter.
 *
 * @param <str>   Given String
 * 
 */
StringTokenizer::StringTokenizer(const std::string &str)
{
   delimiters = " ";
   Parse(str,delimiters);
}

//---------------------------------------------------------------------------
//  StringTokenizer(const std::string &str, const std::string &delim)
//---------------------------------------------------------------------------
/**
 * Creates constructor with parameters.
 *
 * @param <str>   Given String
 * @param <delim> Given delimiters
 * 
 */
StringTokenizer::StringTokenizer(const std::string &str,
                                 const std::string &delim)
{
   delimiters = delim;
   Parse(str,delimiters);
}

//---------------------------------------------------------------------------
//  ~StringTokenizer()
//---------------------------------------------------------------------------
/**
 * Destructor.
 * 
 */
StringTokenizer::~StringTokenizer()
{
}

//---------------------------------------------------------------------------
//  Integer CountTokens() const
//---------------------------------------------------------------------------
/**
 * Get the number of tokens.
 *
 * @return number of tokens. 
 * 
 */
Integer StringTokenizer::CountTokens() const
{
   return countTokens;
}

//---------------------------------------------------------------------------
//  std::string GetToken(const Integer loc) 
//---------------------------------------------------------------------------
/**
 * Get the string from specifiying the token number.
 *
 * @param <loc>   Specified token number
 * @return        return string in the specified token number 
 * 
 */
std::string StringTokenizer::GetToken(const Integer loc)  const
{
   if (loc <= countTokens)
      return stringTokens.at(loc);

   return std::string("");
}

//---------------------------------------------------------------------------
//  std::vector<string> GetAllTokens() const;
//---------------------------------------------------------------------------
/**
 * Get all token string. 
 *
 * @return       all token strings 
 * 
 */
std::vector<std::string> StringTokenizer::GetAllTokens() const 
{
   return stringTokens; 
}

    
//---------------------------------------------------------------------------
//  void Set(const std::string &str) 
//---------------------------------------------------------------------------
/**
 * Set the string token.
 *
 * @param <str>   given string
 * 
 */
void StringTokenizer::Set(const std::string &str)
{
   Set(str,delimiters);
}

//---------------------------------------------------------------------------
//  void Set(const std::string &str, const std::string &delim) 
//---------------------------------------------------------------------------
/**
 * Set the string token.
 *
 * @param <str>   given string
 * @param <delim> given delimiters 
 * 
 */
void StringTokenizer::Set(const std::string &str, const std::string &delim)
{
   stringTokens.clear();
   delimiters = delim;
   Parse(str,delimiters);
}

//-------------------------------------
// private methods
//-------------------------------------

//---------------------------------------------------------------------------
//  void Parse(const std::string &str,const std::string &delim)
//---------------------------------------------------------------------------
/**
 * Parse out the string for token
 *
 * @param <str>   Given string
 * @param <delim> delimiters
 *
 */
void StringTokenizer::Parse(const std::string &str,const std::string &delim)
{
    std::string::size_type pos, lastPos;

    // Skip delimiters at beginning.
    lastPos = str.find_first_not_of(delim, 0);

    // Find first "non-delimiter".
    pos = str.find_first_of(delim, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        stringTokens.push_back(str.substr(lastPos, pos - lastPos));

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delim, pos);

        // Find next "non-delimiter"
        pos = str.find_first_of(delim, lastPos);
    }

    // Set the number of token found
    countTokens = stringTokens.size();
}
