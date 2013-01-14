//$Id$
//------------------------------------------------------------------------------
//                          FormationFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2012 Thinking Systems, Inc.
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: January 10, 2013
//
/**
 * Definition of the factory used to create Formations
 */
//------------------------------------------------------------------------------


#ifndef FormationFactory_hpp
#define FormationFactory_hpp

#include "FormationDefs.hpp"
#include "Factory.hpp"


/**
 * Factory class that creates Formation objects
 */
class FORMATION_API FormationFactory : public Factory
{
public:
   FormationFactory();
   virtual ~FormationFactory();
   FormationFactory(const FormationFactory& elf);
   FormationFactory& operator=(const FormationFactory& elf);

   virtual SpaceObject*     CreateSpacecraft(const std::string &ofType,
                                             const std::string &withName = "");
};

#endif /* FormationFactory_hpp */
