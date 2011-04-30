//$Id$
//------------------------------------------------------------------------------
//                         ParameterFactory
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
// Author: Darrel Conway
// Created: 2003/10/28
//
/**
 *  This class is the factory class for parameters.
 */
//------------------------------------------------------------------------------
#ifndef ParameterFactory_hpp
#define ParameterFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Parameter.hpp"

class GMAT_API ParameterFactory : public Factory
{
public:
   Parameter*  CreateParameter(const std::string &ofType,
                               const std::string &withName = "");

   // default constructor
   ParameterFactory();
   // constructor
   ParameterFactory(StringArray createList);
   // copy constructor
   ParameterFactory(const ParameterFactory &fact);
   // assignment operator
   ParameterFactory& operator= (const ParameterFactory &fact);

   // destructor
   ~ParameterFactory();

protected:
   // protected data

private:
   // private data

};

#endif // ParameterFactory_hpp
