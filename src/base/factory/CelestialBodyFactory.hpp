//$Id$
//------------------------------------------------------------------------------
//                         CelestialBodyFactory
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
*  This class is the factory class for CelestialBodys.
 */
//------------------------------------------------------------------------------
#ifndef CelestialBodyFactory_hpp
#define CelestialBodyFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "CelestialBody.hpp"

class GMAT_API CelestialBodyFactory : public Factory
{
public:
   CelestialBody*  CreateCelestialBody(const std::string &ofType,
                                       const std::string &withName = "");

   // default constructor
   CelestialBodyFactory();
   // constructor
   CelestialBodyFactory(const StringArray &createList);
   // copy constructor
   CelestialBodyFactory(const CelestialBodyFactory &fact);
   // assignment operator
   CelestialBodyFactory& operator= (const CelestialBodyFactory &fact);

   // destructor
   ~CelestialBodyFactory();

protected:
      // protected data

private:
      // private data


};

#endif // CelestialBodyFactory_hpp




