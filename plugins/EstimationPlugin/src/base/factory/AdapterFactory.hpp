//$Id: AdapterFactory.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         AdapterFactory
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/06/18
//
/**
 * Factory used to create the MeasurementModel objects
 */
//------------------------------------------------------------------------------

#ifndef AdapterFactory_hpp
#define AdapterFactory_hpp


#include "estimation_defs.hpp"
#include "Factory.hpp"
#include "MeasurementModel.hpp"

class ESTIMATION_API AdapterFactory : public Factory
{
public:
   AdapterFactory();
   virtual ~AdapterFactory();

   AdapterFactory(StringArray createList);
   AdapterFactory(const AdapterFactory& fact);
   AdapterFactory& operator= (const AdapterFactory& fact);

   MeasurementModel *CreateMeasurementModel(const std::string &ofType,
         const std::string &withName = "");
};

#endif /* AdapterFactory_hpp */
