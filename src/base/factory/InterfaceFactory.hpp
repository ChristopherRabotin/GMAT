//$Id$
//------------------------------------------------------------------------------
//                            InterfaceFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Linda Jun
// Created: 2010/04/02
//
/**
 *  Implementation code for the InterfaceFactory class, responsible for creating
 *  targeters, optimizers, and other parameteric scanning objects.
 */
//------------------------------------------------------------------------------
#ifndef InterfaceFactory_hpp
#define InterfaceFactory_hpp


#include "Factory.hpp"


class InterfaceFactory : public Factory
{
public:
   virtual Interface* CreateInterface(const std::string &ofType,
                                      const std::string &withName);
   // default constructor
   InterfaceFactory();
   // constructor
   InterfaceFactory(StringArray createList);
   // copy constructor
   InterfaceFactory(const InterfaceFactory& fact);
   // assignment operator
   InterfaceFactory& operator=(const InterfaceFactory& fact);
   
   virtual ~InterfaceFactory();
   
};

#endif // InterfaceFactory_hpp
