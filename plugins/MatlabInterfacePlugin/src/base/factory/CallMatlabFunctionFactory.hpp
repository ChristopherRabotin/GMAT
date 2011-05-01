//$Id: CallMatlabFunctionFactory.hpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                            CallMatlabFunctionFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Linda Jun
// Created: 2010/03/30
//
/**
 *  Declaration code for the CallMatlabFunctionFactory class.
 */
//------------------------------------------------------------------------------
#ifndef CallMatlabFunctionFactory_hpp
#define CallMatlabFunctionFactory_hpp


#include "matlabinterface_defs.hpp"
#include "Factory.hpp"
#include "GmatCommand.hpp"

class MATLAB_API CallMatlabFunctionFactory : public Factory
{
public:
   virtual GmatCommand* CreateCommand(const std::string &ofType,
                                      const std::string &withName);
   
   // default constructor
   CallMatlabFunctionFactory();
   // constructor
   CallMatlabFunctionFactory(StringArray createList);
   // copy constructor
   CallMatlabFunctionFactory(const CallMatlabFunctionFactory& fact);
   // assignment operator
   CallMatlabFunctionFactory& operator=(const CallMatlabFunctionFactory& fact);
   
   virtual ~CallMatlabFunctionFactory();
   
};

#endif // CallMatlabFunctionFactory_hpp
