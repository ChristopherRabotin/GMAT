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

#ifndef StringTokenizer_hpp
#define StringTokenizer_hpp

#include <string>
#include <vector>
#include <iterator>
#include "gmatdefs.hpp"

class StringTokenizer
{
public:
    StringTokenizer();
    StringTokenizer(const std::string &str);
    StringTokenizer(const std::string &str, const std::string &delim);
    ~StringTokenizer();

    Integer                  CountTokens() const; 
    std::string              GetToken(const Integer loc) const;
    std::vector<std::string> GetAllTokens() const;
    void                     Set(const std::string &str);
    void                     Set(const std::string &str,
                                 const std::string &delim);

//  @todo - may add this later ->  std::string GetDelimiter() const;
    
private:
    std::vector<std::string> stringTokens;
    std::string              delimiters;
    Integer                  countTokens;

    void Parse(const std::string &str,const std::string &delim);
                                
};

#endif // StringTokenizer_hpp
