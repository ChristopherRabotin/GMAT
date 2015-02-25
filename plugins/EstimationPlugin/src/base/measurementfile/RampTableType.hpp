//$Id: RampTableType.hpp 1398 2013-07-02 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         RampTableType
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
// Created: 2013/07/02
//
/**
 * RampTableType class used for GMAT_RampTable data
 */
//------------------------------------------------------------------------------


#ifndef RampTableType_HPP_
#define RampTableType_HPP_

#include "estimation_defs.hpp"
#include "ObType.hpp"
#include "RampTableData.hpp"
#include <fstream>         // Should we have a file specific intermediate class?

/**
 * RampTableType is the frequency table data type used to represent GmatRampTable
 * formatted observation data.
 */
class ESTIMATION_API RampTableType : public ObType
{
public:
   RampTableType(const std::string withName = "");
   virtual ~RampTableType();
   RampTableType(const RampTableType& frt);
   RampTableType& operator=(const RampTableType& ot);

   GmatBase*         Clone() const;

   virtual bool      Initialize();
   virtual bool      Open(bool forRead = true, bool forWrite= false,
                          bool append = false);
   virtual bool      IsOpen();

   virtual bool      AddMeasurement(MeasurementData *md){ return true;};		// This function is not used in ramp table
   virtual ObservationData *
					 ReadObservation(){ return NULL;};							         // This function is not used in ramp table
   virtual RampTableData *
                     ReadRampTableData();

   virtual bool      Close();
   virtual bool      Finalize();

private:
   /// File stream that provides access to the observation data
   std::fstream      theStream;
   /// Precision used for epoch data
   Integer           epochPrecision;
   /// Precision used for the observation data
   Integer           dataPrecision;
   /// The most recently accessed record of frequency ramp table
   RampTableData     currentRecord;
   /// Ramp table
   std::vector<RampTableData>
	                 rampTable;
};

#endif /* RampTableType_HPP_ */
