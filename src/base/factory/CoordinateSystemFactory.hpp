//$Id$
//------------------------------------------------------------------------------
//                         CoordinateSystemFactory
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
// Author: Linda Jun
// Created: 2005/01/18
//
/**
 *  This class is the factory class for CoordinateSystems.  
 */
//------------------------------------------------------------------------------
#ifndef CoordinateSystemFactory_hpp
#define CoordinateSystemFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "CoordinateSystem.hpp"

class GMAT_API CoordinateSystemFactory : public Factory
{
public:
   CoordinateSystem*  CreateCoordinateSystem(const std::string &ofType,
                                             const std::string &withName = "");

   // default constructor
   CoordinateSystemFactory();
   // constructor
   CoordinateSystemFactory(StringArray createList);
   // copy constructor
   CoordinateSystemFactory(const CoordinateSystemFactory& fact);
   // assignment operator
   CoordinateSystemFactory& operator= (const CoordinateSystemFactory& fact);

   // destructor
   ~CoordinateSystemFactory();

protected:
   // protected data

private:
   // private data


};

#endif // CoordinateSystemFactory_hpp




