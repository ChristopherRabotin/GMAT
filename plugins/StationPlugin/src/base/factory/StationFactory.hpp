//$Id$
//------------------------------------------------------------------------------
//                          AvroInterfaceFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28.
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: January 10, 2013
//
//
/**
 * Definition of the factory used to create message writing commands in this
 * plugin
 */
//------------------------------------------------------------------------------


#ifndef SampleFactory_hpp
#define SampleFactory_hpp

#include "StationDefs.hpp"
#include "Factory.hpp"


/**
 * Factory class that creates station objects
 */
class STATION_API StationFactory : public Factory
{
public:
   StationFactory();
   virtual ~StationFactory();
   StationFactory(const StationFactory& elf);
   StationFactory& operator=(const StationFactory& elf);

   virtual SpacePoint*      CreateSpacePoint(const std::string &ofType,
                                             const std::string &withName = "");        
};

#endif /* SampleFactory_hpp */
