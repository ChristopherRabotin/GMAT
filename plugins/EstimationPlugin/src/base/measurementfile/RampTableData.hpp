//$Id: RampTableData.hpp 1398 2013-07-01 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         RampTableData
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
// Created: 2013/07/01
//
/**
 * Class that contains ramp table record
 *
 * This class is essentially a struct designed to contain a record
 * retrieved from a RampTableType object.
 */
//------------------------------------------------------------------------------


#ifndef RampTableData_hpp
#define RampTableData_hpp

#include "estimation_defs.hpp"
#include "EstimationDefs.hpp"
#include "GmatData.hpp"
#include "TimeSystemConverter.hpp"


/**
 * Class used to set and retrieve data from ramp table
 */
class ESTIMATION_API RampTableData : public GmatData
{
public:
   RampTableData();
   virtual ~RampTableData();
   RampTableData(const RampTableData& rtd);
   RampTableData&  operator=(const RampTableData& rtd);

   void              Clear();

// Explicitly public so that this class acts like a struct
public:
   /// The text name of the data type, if available
   std::string       typeName;
   /// The type of measurement in this record
   Gmat::MeasurementType   
					      type;
   /// Enumerated ID for the epoch time system
   TimeConverterUtil::TimeSystemTypes  epochSystem;
   /// The start epoch in a ramp 
   GmatEpoch         epoch;						// unit: its unit is defined by epochSystem
   /// Who is involved in the measurement.  First one is the "anchor" node
   StringArray       participantIDs;
   /// Uplink band
   Integer           uplinkBand;				   //-1: invalid, 0:unknown, 1:L-band, 2:S-Band, 
   /// Ramp type
   Integer           rampType;					//0: snap, 1: start of new ramp, 2: media report, 3: periodic report, 4: end of ramp, 5: ramping terminated by operator, 6: invalid/unkown, 7: left blank in DSN file
   /// Ramp frequency
   Real              rampFrequency;				// unit: Hz
   /// Ramp rate
   Real              rampRate;					// unit: Hz/s

   /// Index key used for sorting records contains participantIDs and epoch
   std::string       indexkey;

};

#endif /* RampTableData_hpp */
