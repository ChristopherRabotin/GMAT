//$Header$
//------------------------------------------------------------------------------
//                              FunctionFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Allison Greene
// Created: 2004/9/22
//
/**
 * Implementation code for the FunctionFactory class, responsible for creating
 * Function objects.
 */
//------------------------------------------------------------------------------


#ifndef FunctionFactory_hpp
#define FunctionFactory_hpp

#include "Factory.hpp"
#include "Function.hpp"

class FunctionFactory : public Factory
{
public:
   Function* CreateFunction(const std::string &ofType, const std::string &withName = "");

   FunctionFactory();
   virtual ~FunctionFactory();
   FunctionFactory(StringArray createList);
   FunctionFactory(const FunctionFactory& fact);
   FunctionFactory& operator= (const FunctionFactory& fact);

};


#endif // FunctionFactory_hpp
