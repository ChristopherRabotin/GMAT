//$Id: GmatODDopplerType.hpp 1398 2013-09-18 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         GmatODDopplerType
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
// Author: Tuan Dang Nguyen. NASA/GSFC
// Created: 2013/09/18
//
/**
 * GmatODDopplerType class used for the GMAT OD Doppler observation data
 */
//------------------------------------------------------------------------------


#ifndef GmatODDopplerType_hpp
#define GmatODDopplerType_hpp

#include "estimation_defs.hpp"
#include "ObType.hpp"
#include "ObservationData.hpp"
#include <fstream>         // Should we have a file specific intermediate class?

/**
 * GmatODDopplerType is the observation data type used to represent Gmat_ODDoppler
 * formatted observation data.
 */
class ESTIMATION_API GmatODDopplerType : public ObType
{
public:
   GmatODDopplerType(const std::string withName = "");
   virtual ~GmatODDopplerType();
   GmatODDopplerType(const GmatODDopplerType& ot);
   GmatODDopplerType& operator=(const GmatODDopplerType& ot);

   GmatBase*         Clone() const;

   virtual bool      Initialize();
   virtual bool      Open(bool forRead = true, bool forWrite= false,
                          bool append = false);
   virtual bool      IsOpen();
   virtual bool      AddMeasurement(MeasurementData *md);
   virtual ObservationData *
                     ReadObservation();

   /// GmatODDopplerType does not use ReadRampTableData() function
   virtual RampTableData *
	                 ReadRampTableData() {return NULL;};

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

#endif /* GmatODDopplerType_hpp */
