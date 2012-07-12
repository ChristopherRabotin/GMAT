//$Id$
//------------------------------------------------------------------------------
//                         AtmosphereFactory
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
// Created: 2004/08/12
//
/**
*  This class is the factory class for Atmospheres.
 */
//------------------------------------------------------------------------------
#ifndef AtmosphereFactory_hpp
#define AtmosphereFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "AtmosphereModel.hpp"

class GMAT_API AtmosphereFactory : public Factory
{
public:
   AtmosphereModel*  CreateAtmosphereModel(const std::string &ofType,
                                           const std::string &withName = "",
                                           const std::string &forBody = "Earth");

   // method to return list of types of objects that this factory can create
   virtual StringArray      GetListOfCreatableObjects(
                                  const std::string &qualifier = "Earth");

   // default constructor
   AtmosphereFactory();
   // constructor
   AtmosphereFactory(StringArray createList);
   // copy constructor
   AtmosphereFactory(const AtmosphereFactory& fact);
   // assignment operator
   AtmosphereFactory& operator= (const AtmosphereFactory& fact);

   // destructor
   ~AtmosphereFactory();

protected:
      // protected data

private:


};

#endif // AtmosphereFactory_hpp




