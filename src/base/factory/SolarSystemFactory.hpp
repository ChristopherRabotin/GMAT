//$Id$
//------------------------------------------------------------------------------
//                         SolarSystemFactory
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
// Created: 2004/01/29
//
/**
*  This class is the factory class for SolarSystems.
 */
//------------------------------------------------------------------------------
#ifndef SolarSystemFactory_hpp
#define SolarSystemFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "SolarSystem.hpp"

class GMAT_API SolarSystemFactory : public Factory
{
public:
   SolarSystem*  CreateSolarSystem(const std::string &ofType,
                                   const std::string &withName = "");

   // default constructor
   SolarSystemFactory();
   // constructor
   SolarSystemFactory(StringArray createList);
   // copy constructor
   SolarSystemFactory(const SolarSystemFactory& fact);
   // assignment operator
   SolarSystemFactory& operator= (const SolarSystemFactory& fact);

   // destructor
   ~SolarSystemFactory();

protected:
      // protected data

private:
      // private data


};

#endif // SolarSystemFactory_hpp




