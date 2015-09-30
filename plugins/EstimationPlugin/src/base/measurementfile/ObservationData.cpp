//$Id: ObservationData.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         ObservationData
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
// Created: 2009/08/06
//
/**
 * Class that contains observation data
 *
 * This class is essentially a struct designed to contain the observation data
 * retrieved from a ObType object.
 */
//------------------------------------------------------------------------------


#include "ObservationData.hpp"
#include "EstimationDefs.hpp"
#include <sstream>

//-----------------------------------------------------------------------------
// ObservationData()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
ObservationData::ObservationData() :
   inUsed            (true),
   removedReason     ("N"),
   typeName          (""),
   type              (Gmat::UNKNOWN_MEASUREMENT),
   uniqueID          (-1),
   epochSystem       (TimeConverterUtil::A1MJD),
   epoch             (-1.0),
   noiseCovariance   (NULL),
///// TBD: Determine if there is a more generic way to add these, and if they go here
   unit              ("km"),
   uplinkBand        (0),
   uplinkFreq        (0.0),
   rangeModulo       (1.0),
   dopplerCountInterval   (1.0e-10),
   tdrsServiceID     ("SA1"),                              // made changes by TUAN NGUYEN
   tdrsNode4Freq     (0.0),                                // made changes by TUAN NGUYEN
   tdrsNode4Band     (0),                                  // made changes by TUAN NGUYEN
   tdrsSMARID        (0)                                   // made changes by TUAN NGUYEN

{
///// TBD: This does not go here -- we'll be adding a new data type, so need to have a way to handle this
   dataFormat = "GMATInternal";
}


//-----------------------------------------------------------------------------
// ~ObservationData()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
ObservationData::~ObservationData()
{
}


//-----------------------------------------------------------------------------
// ObservationData(const ObservationData& od):
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param od The ObservationData object that sets the data for the new one
 */
//-----------------------------------------------------------------------------
ObservationData::ObservationData(const ObservationData& od):
   inUsed                  (od.inUsed),
   removedReason           (od.removedReason),
   typeName                (od.typeName),
   type                    (od.type),
   uniqueID                (od.uniqueID),
   epochSystem             (od.epochSystem),
   epoch                   (od.epoch),
   epochAtEnd              (od.epochAtEnd),
   epochAtIntegrationEnd   (od.epochAtIntegrationEnd),
   participantIDs          (od.participantIDs),
   strands                 (od.strands),
   value                   (od.value),
   dataMap                 (od.dataMap),
   value_orig              (od.value_orig),
   unit                    (od.unit),
   noiseCovariance         (od.noiseCovariance),
   extraDataDescriptions   (od.extraDataDescriptions),
   extraTypes              (od.extraTypes),
   extraData               (od.extraData),
//   dataFormat              (od.dataFormat),
   uplinkBand              (od.uplinkBand),
   uplinkFreq              (od.uplinkFreq),
   rangeModulo             (od.rangeModulo),
   dopplerCountInterval	   (od.dopplerCountInterval),
   tdrsServiceID           (od.tdrsServiceID),                    // made changes by TUAN NGUYEN
   tdrsNode4Freq           (od.tdrsNode4Freq),                    // made changes by TUAN NGUYEN
   tdrsNode4Band           (od.tdrsNode4Band),                    // made changes by TUAN NGUYEN
   tdrsSMARID              (od.tdrsSMARID)                        // made changes by TUAN NGUYEN
{
   dataFormat = od.dataFormat;
}


//-----------------------------------------------------------------------------
// ObservationData& operator=(const ObservationData& od)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param od The ObservationData object that sets the data for the this one
 *
 * @return This object, configured to match od
 */
//-----------------------------------------------------------------------------
ObservationData& ObservationData::operator=(const ObservationData& od)
{
   if (&od != this)
   {
      inUsed                  = od.inUsed;
      removedReason           = od.removedReason;
      typeName                = od.typeName;
      type                    = od.type;
      uniqueID                = od.uniqueID;
      epochSystem             = od.epochSystem;
      epoch                   = od.epoch;
      epochAtEnd              = od.epochAtEnd;
      epochAtIntegrationEnd   = od.epochAtIntegrationEnd;
      participantIDs          = od.participantIDs;
      strands                 = od.strands;
      value                   = od.value;
      dataMap                 = od.dataMap;
      value_orig              = od.value_orig;
      unit                    = od.unit;
      noiseCovariance         = od.noiseCovariance;
      extraDataDescriptions   = od.extraDataDescriptions;
      extraTypes              = od.extraTypes;
      extraData               = od.extraData;
      dataFormat              = od.dataFormat;
      uplinkBand              = od.uplinkBand;
      uplinkFreq              = od.uplinkFreq;
      rangeModulo             = od.rangeModulo;
      dopplerCountInterval    = od.dopplerCountInterval;
      tdrsServiceID           = od.tdrsServiceID;                    // made changes by TUAN NGUYEN
      tdrsNode4Freq           = od.tdrsNode4Freq;                    // made changes by TUAN NGUYEN
      tdrsNode4Band           = od.tdrsNode4Band;                    // made changes by TUAN NGUYEN
      tdrsSMARID              = od.tdrsSMARID;                       // made changes by TUAN NGUYEN
   }

   return *this;
}


//-----------------------------------------------------------------------------
// void Clear()
//-----------------------------------------------------------------------------
/**
 * Clears all data from this ObservationData object.
 */
//-----------------------------------------------------------------------------
void ObservationData::Clear()
{
   inUsed                  = true;
   removedReason           = "N";
   typeName                = "";
   type                    = Gmat::UNKNOWN_MEASUREMENT;
   uniqueID                = -1;
   epoch                   = 0.0;
   participantIDs.clear();
   value.clear();
   dataMap.clear();
   strands.clear();
   value_orig.clear();
   extraDataDescriptions.clear();
   extraTypes.clear();
   extraData.clear();
///// TBD: Determine if there is a more generic way to add these
   uplinkBand              = 0;
   uplinkFreq              = 0.0;
   rangeModulo             = 1.0;
   dopplerCountInterval    = 1.0e-10;
   tdrsServiceID           = "SA1";                    // made changes by TUAN NGUYEN
   tdrsNode4Freq           = 0.0;                      // made changes by TUAN NGUYEN
   tdrsNode4Band           = 0;                        // made changes by TUAN NGUYEN
   tdrsSMARID              = 0;                        // made changes by TUAN NGUYEN
}



std::string ObservationData::GetTrackingConfig()
{
   std::stringstream ss;
   ss << "{{";
   for (UnsignedInt i = 0; i < participantIDs.size(); ++i)
   {
      ss << participantIDs[i];
      if (i < participantIDs.size()-1)
         ss << ",";
   }
   ss << "}" << typeName << "}";

   return ss.str();
}
