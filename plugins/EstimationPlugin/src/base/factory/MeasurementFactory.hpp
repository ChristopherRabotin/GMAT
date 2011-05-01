//$Id: MeasurementFactory.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         MeasurementFactory
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
// Created: 2009/07/09
//
/**
 * Factory class used to create core measurement objects
 */
//------------------------------------------------------------------------------


#ifndef MeasurementFactory_hpp
#define MeasurementFactory_hpp

#include "estimation_defs.hpp"
#include "Factory.hpp"
#include "CoreMeasurement.hpp"


class ESTIMATION_API MeasurementFactory : public Factory
{
public:
   MeasurementFactory();
   virtual ~MeasurementFactory();
   MeasurementFactory(StringArray createList);
   MeasurementFactory(const MeasurementFactory& fact);
   MeasurementFactory& operator= (const MeasurementFactory& fact);

   CoreMeasurement *CreateMeasurement(const std::string &ofType,
         const std::string &withName = "");
};

#endif /* MeasurementFactory_hpp */
