//$Id$
//------------------------------------------------------------------------------
//                          MeasurementModelBase
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/06/24
//
/**
 * MeasurementModelBase declaration used in GMAT's estimator and simulator
 * factory code
 */
//------------------------------------------------------------------------------

#ifndef MeasurementModelBase_hpp
#define MeasurementModelBase_hpp

#include "estimation_defs.hpp"
#include "EstimationDefs.hpp"
#include "GmatBase.hpp"
#include "TimeSystemConverter.hpp"   // for the TimeSystemConverter singleton

/**
 * Base class for measurement models and tracking file sets so the Factories 
 * can manage them as the same core type
 */
class ESTIMATION_API MeasurementModelBase : public GmatBase
{
public:
   MeasurementModelBase(const std::string &nomme,
         const std::string& typeName);
   virtual ~MeasurementModelBase();
   MeasurementModelBase(const MeasurementModelBase &mm);
   MeasurementModelBase& operator=(const MeasurementModelBase &mm);

protected:
   enum
   {
      MeasurementModelBaseParamCount = GmatBaseParamCount,
   };

   /// Time converter singleton
   TimeSystemConverter *theTimeConverter;

   Integer GetParmIdFromEstID(Integer id, GmatBase *obj);
};

#endif /* MeasurementModelBase_hpp */
