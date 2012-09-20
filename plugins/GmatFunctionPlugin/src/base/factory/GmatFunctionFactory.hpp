//$Id$
//------------------------------------------------------------------------------
//                              GmatFunctionFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, task order 28.
//
// Author: Darrel Conway (Thinking Systems)
// Created: 2012/09/18
//
/**
 * Definition code for the GmatFunctionFactory class, responsible for creating
 * GmatFunction objects.
 */
//------------------------------------------------------------------------------


#ifndef GmatFunctionFactory_hpp
#define GmatFunctionFactory_hpp

#include "GmatFunction_defs.hpp"
#include "Factory.hpp"
#include "Function.hpp"

/**
 * The GmatFunctionFactory provides the interfaces used to create GmatFunctions.
 */
class GMATFUNCTION_API GmatFunctionFactory : public Factory
{
public:
   Function* CreateFunction(const std::string &ofType, const std::string &withName = "");

   GmatFunctionFactory();
   virtual ~GmatFunctionFactory();
   GmatFunctionFactory(StringArray createList);
   GmatFunctionFactory(const GmatFunctionFactory& fact);
   GmatFunctionFactory& operator= (const GmatFunctionFactory& fact);
};


#endif // GmatFunctionFactory_hpp
