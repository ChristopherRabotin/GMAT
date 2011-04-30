//$Id$
//------------------------------------------------------------------------------
//                         SpacecraftFactory
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
 *  This class is the factory class for spacecraft.  
 */
//------------------------------------------------------------------------------
#ifndef SpacecraftFactory_hpp
#define SpacecraftFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Spacecraft.hpp"

class GMAT_API SpacecraftFactory : public Factory
{
public:
   SpaceObject* CreateSpacecraft(const std::string &ofType,
                                 const std::string &withName = "");

   // default constructor
   SpacecraftFactory();
   // constructor
   SpacecraftFactory(StringArray createList);
   // copy constructor
   SpacecraftFactory(const SpacecraftFactory &fact);
   // assignment operator
   SpacecraftFactory& operator= (const SpacecraftFactory &fact);

   // destructor
   ~SpacecraftFactory();

protected:
   // protected data

private:
   // private data


};

#endif // SpacecraftFactory_hpp
