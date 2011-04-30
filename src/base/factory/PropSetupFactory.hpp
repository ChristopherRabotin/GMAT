//$Id$
//------------------------------------------------------------------------------
//                         PropSetupFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/22
//
/**
*  This class is the factory class for PropSetups.
 */
//------------------------------------------------------------------------------
#ifndef PropSetupFactory_hpp
#define PropSetupFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "PropSetup.hpp"

class GMAT_API PropSetupFactory : public Factory
{
public:
   PropSetup*  CreatePropSetup(const std::string &ofType,
                               const std::string &withName = "");

   // default constructor
   PropSetupFactory();
   // constructor
   PropSetupFactory(StringArray createList);
   // copy constructor
   PropSetupFactory(const PropSetupFactory &fact);
   // assignment operator
   PropSetupFactory& operator= (const PropSetupFactory &fact);

   // destructor
   ~PropSetupFactory();

protected:
      // protected data

private:
      // private data


};

#endif // PropSetupFactory_hpp

