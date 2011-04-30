//$Id$
//------------------------------------------------------------------------------
//                              StringTokenizer
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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

#ifndef StringTokenizer_hpp
#define StringTokenizer_hpp

#include "gmatdefs.hpp"

class GMAT_API StringTokenizer
{
public:
   StringTokenizer();
   StringTokenizer(const std::string &str, const std::string &delim);
   StringTokenizer(const std::string &str, const std::string &delim,
                   bool insertDelim);
   ~StringTokenizer();
   
   // inline methods
   void SetDelimiters(const std::string &delim) { delimiters = delim; }
   std::string GetDelimiters() { return delimiters; }
   
   Integer CountTokens() const; 
   std::string GetToken(const Integer loc) const;
   const StringArray& GetAllTokens() const;
   
   void Set(const std::string &str, const std::string &delim);
   void Set(const std::string &str, const std::string &delim, bool insertDelim);
   
private:
   
   StringArray  stringTokens;
   std::string  delimiters;
   Integer      countTokens;
   
   void Parse(const std::string &str);
   void Parse(const std::string &str, bool insertDelim);
   
};

#endif // StringTokenizer_hpp
