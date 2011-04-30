//$Id$
//------------------------------------------------------------------------------
//                            AssetFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: 2008/08/12
//
/**
 *  This class is the factory class for assets.  
 */
//------------------------------------------------------------------------------

#ifndef AssetFactory_hpp
#define AssetFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"


class GMAT_API AssetFactory : public Factory
{
public:
   AssetFactory();
   virtual ~AssetFactory();
   AssetFactory(StringArray createList);
   AssetFactory(const AssetFactory& af);
   AssetFactory& operator=(const AssetFactory& af);
   
   virtual SpacePoint*      CreateSpacePoint(const std::string &ofType,
                                             const std::string &withName = "");        

};

#endif /*AssetFactory_hpp*/
