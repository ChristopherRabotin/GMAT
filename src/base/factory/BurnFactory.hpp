//$Id$
//------------------------------------------------------------------------------
//                              BurnFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Darrel Conway, Thinking Systems, Inc.
// Created: 2004/1/14
//
/**
 * Implementation code for the BurnFactory class, responsible for creating 
 * Burn objects.
 */
//------------------------------------------------------------------------------


#ifndef BurnFactory_hpp
#define BurnFactory_hpp

#include "Factory.hpp"
#include "Burn.hpp"

class GMAT_API BurnFactory : public Factory
{
public:
   Burn* CreateBurn(const std::string &ofType,
                    const std::string &withName = "");

   BurnFactory();
   virtual ~BurnFactory();
   BurnFactory(StringArray createList);
   BurnFactory(const BurnFactory &fact);
   BurnFactory& operator= (const BurnFactory &fact);
};


#endif // BurnFactory_hpp
