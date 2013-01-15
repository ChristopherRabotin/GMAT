//$Id$
//------------------------------------------------------------------------------
//                        GmatFunctionCommandFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 14, 2013
/**
 * Factory for commands associated with GMAT functions
 */
//------------------------------------------------------------------------------

#ifndef GmatFunctionCommandFactory_hpp
#define GmatFunctionCommandFactory_hpp

#include "GmatFunction_defs.hpp"
#include "Factory.hpp"

/**
 * Factory that creates commands used in GMAT functions
 */
class GMATFUNCTION_API GmatFunctionCommandFactory : public Factory
{
public:
   GmatFunctionCommandFactory();
   virtual ~GmatFunctionCommandFactory();
   GmatFunctionCommandFactory(const GmatFunctionCommandFactory& elf);
   GmatFunctionCommandFactory& operator=(const GmatFunctionCommandFactory& elf);

   virtual GmatCommand*     CreateCommand(const std::string &ofType,
                                          const std::string &withName = "");
};

#endif /* GmatFunctionCommandFactory_hpp */
