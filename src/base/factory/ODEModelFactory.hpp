//$Id$
//------------------------------------------------------------------------------
//                         ODEModelFactory
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
// Created: 2003/10/14
//
/**
 *  This class is the factory class for ODEModels.  
 */
//------------------------------------------------------------------------------
#ifndef ODEModelFactory_hpp
#define ODEModelFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "ODEModel.hpp"

class GMAT_API ODEModelFactory : public Factory
{
public:
   ODEModel*  CreateObject(const std::string &ofType,
                           const std::string &withName = "");
   ODEModel*  CreateODEModel(const std::string &ofType,
                             const std::string &withName = "");
   
   // default constructor
   ODEModelFactory();
   // constructor
   ODEModelFactory(StringArray createList);
   // copy constructor
   ODEModelFactory(const ODEModelFactory& fact);
   // assignment operator
   ODEModelFactory& operator= (const ODEModelFactory& fact);

   // destructor
   ~ODEModelFactory();

protected:
   // protected data

private:
   // private data


};

#endif // ODEModelFactory_hpp




