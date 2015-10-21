//$Id: MeasurementData.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         MeasurementData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/07/15
//
/**
 * Implementation of the MeasurementData class, used to manage calculated
 * measurements
 */
//------------------------------------------------------------------------------


#include "MeasurementData.hpp"
#include "MessageInterface.hpp"


//-----------------------------------------------------------------------------
// MeasurementData()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
MeasurementData::MeasurementData() :
   type             (Gmat::UNKNOWN_MEASUREMENT),
   typeName         ("Unknown"),
   uniqueID         (-1),
   epochSystem      (TimeConverterUtil::A1MJD),
   epoch            (0.0),
   isFeasible       (false),
   unfeasibleReason ("N"),
   feasibilityValue (0.0),
   covariance       (NULL),
   eventCount       (0),
   uplinkBand       (0),
   uplinkFreq       (0.0),
   rangeModulo      (1.0),
   dopplerCountInterval	(1.0e-10),
   tdrsNode4Freq    (0.0),                             // made changes by TUAN NGUYEN
   tdrsNode4Band    (0),                               // made changes by TUAN NGUYEN
   tdrsServiceID    ("SA1"),                           // made changes by TUAN NGUYEN
   tdrsSMARID       (0),                               // made changes by TUAN NGUYEN
   tdrsDataFlag     (0)                                // made changes by TUAN NGUYEN
{
}


//-----------------------------------------------------------------------------
// ~MeasurementData()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
MeasurementData::~MeasurementData()
{
}

void MeasurementData::CleanUp()
{
   if (covariance)
      delete covariance;
   covariance = NULL;

   participantIDs.clear();
}

//-----------------------------------------------------------------------------
// MeasurementData(const MeasurementData& md)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param md The measurement data that is copied into the new instance created
 *           here
 */
//-----------------------------------------------------------------------------
MeasurementData::MeasurementData(const MeasurementData& md) :
   type             (md.type),
   typeName         (md.typeName),
   uniqueID         (md.uniqueID),
   epochSystem      (md.epochSystem),
   epoch            (md.epoch),
   participantIDs   (md.participantIDs),
   value            (md.value),
   isFeasible       (md.isFeasible),
   unfeasibleReason (md.unfeasibleReason),
   feasibilityValue (md.feasibilityValue),
   covariance       (md.covariance),
   eventCount       (md.eventCount),
///// TBD: Do these go here?
   uplinkBand       (md.uplinkBand),
   uplinkFreq       (md.uplinkFreq),
   rangeModulo      (md.rangeModulo),
   dopplerCountInterval	(md.dopplerCountInterval),
   tdrsNode4Freq    (md.tdrsNode4Freq),                        // made changes by TUAN NGUYEN
   tdrsNode4Band    (md.tdrsNode4Band),                        // made changes by TUAN NGUYEN
   tdrsServiceID    (md.tdrsServiceID),                        // made changes by TUAN NGUYEN
   tdrsSMARID       (md.tdrsSMARID),                           // made changes by TUAN NGUYEN
   tdrsDataFlag     (md.tdrsDataFlag)                          // made changes by TUAN NGUYEN
{
}


//-----------------------------------------------------------------------------
// MeasurementData MeasurementData::operator=(const MeasurementData& md)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param md The measurement data that is copied into this instance
 *
 * @return This instance, configured to match md
 */
//-----------------------------------------------------------------------------
MeasurementData MeasurementData::operator=(const MeasurementData& md)
{

   if (&md != this)
   {
      type             = md.type;
      typeName         = md.typeName;
      uniqueID         = md.uniqueID;
      epochSystem      = md.epochSystem;
      epoch            = md.epoch;
      participantIDs   = md.participantIDs;
      value            = md.value;
      isFeasible       = md.isFeasible;
	   unfeasibleReason = md.unfeasibleReason;
	   feasibilityValue = md.feasibilityValue;
      covariance       = md.covariance;
      eventCount       = md.eventCount;
	   uplinkBand       = md.uplinkBand;
	   uplinkFreq       = md.uplinkFreq;
	   rangeModulo      = md.rangeModulo;
	   dopplerCountInterval = md.dopplerCountInterval;
      tdrsNode4Freq    = md.tdrsNode4Freq;                     // made changes by TUAN NGUYEN
      tdrsNode4Band    = md.tdrsNode4Band;                     // made changes by TUAN NGUYEN
      tdrsServiceID    = md.tdrsServiceID;                     // made changes by TUAN NGUYEN
      tdrsSMARID       = md.tdrsSMARID;                        // made changes by TUAN NGUYEN
      tdrsDataFlag     = md.tdrsDataFlag;                      // made changes by TUAN NGUYEN
   }

   return *this;
}
