//$Id: MeasurementData.cpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         MeasurementData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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
   type                       (Gmat::UNKNOWN_MEASUREMENT),
   typeName                   ("Unknown"),
   uniqueID                   (-1),
   epochSystem                (TimeConverterUtil::A1MJD),
   epoch                      (0.0),
   isFeasible                 (false),
   unfeasibleReason           ("N"),
   feasibilityValue           (0.0),
   covariance                 (NULL),
   eventCount                 (0),
   uplinkBand                 (0),
   uplinkFreq                 (0.0),
   uplinkFreqAtRecei          (0.0),
   rangeModulo                (1.0),
   dopplerCountInterval	      (1.0e-10),
   tdrsNode4Freq              (0.0),
   tdrsNode4Band              (0),
   tdrsServiceID              ("SA1"),
   tdrsSMARID                 (0),
   tdrsDataFlag               (0),
   isTropoCorrectWarning      (false),
   isIonoCorrectWarning       (false),
   tropoCorrectWarningValue   (0.0),
   ionoCorrectWarningValue    (0.0)
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
   type                     (md.type),
   typeName                 (md.typeName),
   uniqueID                 (md.uniqueID),
   epochSystem              (md.epochSystem),
   epoch                    (md.epoch),
   participantIDs           (md.participantIDs),
   value                    (md.value),
   isFeasible               (md.isFeasible),
   unfeasibleReason         (md.unfeasibleReason),
   feasibilityValue         (md.feasibilityValue),
   covariance               (md.covariance),
   eventCount               (md.eventCount),
///// TBD: Do these go here?
   uplinkBand               (md.uplinkBand),
   uplinkFreq               (md.uplinkFreq),
   uplinkFreqAtRecei        (md.uplinkFreqAtRecei),
   rangeModulo              (md.rangeModulo),
   dopplerCountInterval	    (md.dopplerCountInterval),
   tdrsNode4Freq            (md.tdrsNode4Freq),
   tdrsNode4Band            (md.tdrsNode4Band),
   tdrsServiceID            (md.tdrsServiceID),
   tdrsSMARID               (md.tdrsSMARID),
   tdrsDataFlag             (md.tdrsDataFlag),
   isTropoCorrectWarning    (md.isTropoCorrectWarning),
   isIonoCorrectWarning     (md.isIonoCorrectWarning),
   tropoCorrectWarningValue (md.tropoCorrectWarningValue),
   ionoCorrectWarningValue  (md.ionoCorrectWarningValue)
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
      type                     = md.type;
      typeName                 = md.typeName;
      uniqueID                 = md.uniqueID;
      epochSystem              = md.epochSystem;
      epoch                    = md.epoch;
      participantIDs           = md.participantIDs;
      value                    = md.value;
      isFeasible               = md.isFeasible;
	   unfeasibleReason         = md.unfeasibleReason;
	   feasibilityValue         = md.feasibilityValue;
      covariance               = md.covariance;
      eventCount               = md.eventCount;
	   uplinkBand               = md.uplinkBand;
	   uplinkFreq               = md.uplinkFreq;
      uplinkFreqAtRecei        = md.uplinkFreqAtRecei;
	   rangeModulo              = md.rangeModulo;
	   dopplerCountInterval     = md.dopplerCountInterval;
      tdrsNode4Freq            = md.tdrsNode4Freq;
      tdrsNode4Band            = md.tdrsNode4Band;
      tdrsServiceID            = md.tdrsServiceID;
      tdrsSMARID               = md.tdrsSMARID;
      tdrsDataFlag             = md.tdrsDataFlag;

      isTropoCorrectWarning    = md.isTropoCorrectWarning;
      isIonoCorrectWarning     = md.isIonoCorrectWarning;
      tropoCorrectWarningValue = md.tropoCorrectWarningValue;
      ionoCorrectWarningValue  = md.ionoCorrectWarningValue;

   }

   return *this;
}
