//$Id: MatlabInterfaceFactory.hpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                            MatlabInterfaceFactory
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
 *  Declaration code for the MatlabInterfaceFactory class.
 */
//------------------------------------------------------------------------------
#ifndef MatlabInterfaceFactory_hpp
#define MatlabInterfaceFactory_hpp

#include "matlabinterface_defs.hpp"
#include "Factory.hpp"
#include "Interface.hpp"

class MATLAB_API MatlabInterfaceFactory : public Factory
{
public:
   virtual Interface* CreateInterface(const std::string &ofType,
                                      const std::string &withName);
   
   /// default constructor
   MatlabInterfaceFactory();
   /// constructor
   MatlabInterfaceFactory(StringArray createList);
   /// copy constructor
   MatlabInterfaceFactory(const MatlabInterfaceFactory& fact);
   /// assignment operator
   MatlabInterfaceFactory& operator=(const MatlabInterfaceFactory& fact);
   
   virtual ~MatlabInterfaceFactory();
   
};

#endif // MatlabInterfaceFactory_hpp
