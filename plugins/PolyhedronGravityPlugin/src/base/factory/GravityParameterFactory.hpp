//$Id$
//------------------------------------------------------------------------------
//                           GravityParameterFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 31, 2016
/**
 * Factory used to create parameters that reference gravity model data
 */
//------------------------------------------------------------------------------

#ifndef GravityParameterFactory_hpp
#define GravityParameterFactory_hpp

#include "polyhedrongravitymodel_defs.hpp"
#include "Factory.hpp"

/**
 * The factory used for gravity model based parameters
 */
class POLYHEDRONGRAVITYMODEL_API GravityParameterFactory: public Factory
{
public:
   GravityParameterFactory();
   GravityParameterFactory(StringArray createList);
   GravityParameterFactory(const GravityParameterFactory& gpf);
   GravityParameterFactory& operator=(const GravityParameterFactory& gpf);

   virtual ~GravityParameterFactory();

   virtual bool DoesObjectTypeMatchSubtype(const std::string &theType,
                                           const std::string &theSubtype);
   virtual StringArray      GetListOfCreatableObjects(
                                  const std::string &qualifier = "");
   
   virtual GmatBase*  CreateObject(const std::string &ofType,
                               const std::string &withName = "");
   virtual Parameter*  CreateParameter(const std::string &ofType,
                               const std::string &withName = "");

private:
   /// Flag used to show that ParameterInfo has been registered
   bool registrationComplete;
};

#endif /* GravityParameterFactory_hpp */
