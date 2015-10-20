//$Id: PolyhedronGravityModelFactory.hpp 9461 2012-08-17 16:19:15Z tuannguyen $
//------------------------------------------------------------------------------
//                            PolyhedronGravityModelFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen
// Created: 2012/08/17
//
/**
 *  Declaration code for the PolyhedronGravityModelFactory class.
 */
//------------------------------------------------------------------------------
#ifndef PolyhedronGravityModelFactory_hpp
#define PolyhedronGravityModelFactory_hpp


#include "polyhedrongravitymodel_defs.hpp"
#include "Factory.hpp"
#include "PhysicalModel.hpp"
#include "PolyhedronGravityModel.hpp"

class POLYHEDRONGRAVITYMODEL_API PolyhedronGravityModelFactory : public Factory
{
public:
   virtual PhysicalModel* CreatePhysicalModel(const std::string &ofType,
                                const std::string &withName);
   
   // default constructor
   PolyhedronGravityModelFactory();
   // constructor
   PolyhedronGravityModelFactory(StringArray createList);
   // copy constructor
   PolyhedronGravityModelFactory(const PolyhedronGravityModelFactory& fact);
   // assignment operator
   PolyhedronGravityModelFactory& operator=(const PolyhedronGravityModelFactory& fact);
   
   virtual ~PolyhedronGravityModelFactory();
   
};

#endif // PolyhedronGravityModelFactory_hpp
