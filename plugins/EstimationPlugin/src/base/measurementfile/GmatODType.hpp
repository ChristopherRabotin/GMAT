//$Id: GmatODType.hpp 1398 2013-06-04 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         GmatODType
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
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: 2013/06/04
//
/**
 * ObType class used for the GMAT Internal observation data
 */
//------------------------------------------------------------------------------


#ifndef GMATODTYPE_HPP_
#define GMATODTYPE_HPP_

#include "estimation_defs.hpp"
#include "ObType.hpp"
#include <fstream>         // Should we have a file specific intermediate class?

/**
 * GmatODType is the observation data type used to represent GmatInternal
 * formatted observation data.
 */
class ESTIMATION_API GmatODType : public ObType
{
public:
   GmatODType(const std::string withName = "");
   virtual ~GmatODType();
   GmatODType(const GmatODType& ot);
   GmatODType& operator=(const GmatODType& ot);

   GmatBase*         Clone() const;

   virtual bool      Initialize();
   virtual bool      Open(bool forRead = true, bool forWrite= false,
                          bool append = false);
   virtual bool      IsOpen();
   virtual bool      AddMeasurement(MeasurementData *md);
   virtual ObservationData *
                     ReadObservation();

   /// GmatODType does not use ReadRampTableData() function
   virtual RampTableData *
	   ReadRampTableData(){return NULL;};

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

#endif /* GMATODTYPE_HPP_ */
