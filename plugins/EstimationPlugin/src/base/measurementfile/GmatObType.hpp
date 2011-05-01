//$Id: GmatObType.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         GmatObType
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
// Created: 2009/07/06
//
/**
 * ObType class used for the GMAT Internal observation data
 */
//------------------------------------------------------------------------------


#ifndef GMATOBTYPE_HPP_
#define GMATOBTYPE_HPP_

#include "estimation_defs.hpp"
#include "ObType.hpp"
#include <fstream>         // Should we have a file specific intermediate class?

/**
 * GmatObType is the observation data type used to represent GmatInternal
 * formatted observation data.
 */
class ESTIMATION_API GmatObType : public ObType
{
public:
   GmatObType(const std::string withName = "");
   virtual ~GmatObType();
   GmatObType(const GmatObType& ot);
   GmatObType& operator=(const GmatObType& ot);

   GmatBase*         Clone() const;

   virtual bool      Initialize();
   virtual bool      Open(bool forRead = true, bool forWrite= false,
                          bool append = false);
   virtual bool      IsOpen();
   virtual bool      AddMeasurement(MeasurementData *md);
   virtual ObservationData *
                     ReadObservation();
   virtual bool      Close();
   virtual bool      Finalize();

private:
   /// File stream that provides access to the observation data
   std::fstream      theStream;
   /// Precision used for epoch data
   Integer           epochPrecision;
   /// Precision used for the observation data
   Integer           dataPrecision;
   /// The most recently accessed observation data set
   ObservationData   currentObs;
};

#endif /* GMATOBTYPE_HPP_ */
