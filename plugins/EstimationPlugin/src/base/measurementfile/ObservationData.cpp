//$Id: ObservationData.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         ObservationData
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


//-----------------------------------------------------------------------------
// ObservationData()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
ObservationData::ObservationData() :
   inUsed            (true),				// made changes by TUAN NGUYEN
   removedReason     ("N"),					// made changes by TUAN NGUYEN
   typeName          (""),
   type              (Gmat::UNKNOWN_MEASUREMENT),
   uniqueID          (-1),
   epochSystem       (TimeConverterUtil::A1MJD),
   epoch             (-1.0),
   noiseCovariance   (NULL),
///// TBD: Determine if there is a more generic way to add these, and if they go here
   unit              ("km"),				// made changes by TUAN NGUYEN
   uplinkBand        (0),					// made changes by TUAN NGUYEN
   uplinkFreq        (0.0),					// made changes by TUAN NGUYEN
   rangeModulo       (1.0),					// made changes by TUAN NGUYEN
   dopplerCountInterval   (1.0e-10)			// made changes by TUAN NGUYEN
{
///// TBD: This does not go here -- we'll be adding a new data type, so need to have a way to handle this
   dataFormat = "GMATInternal";				// made changes by TUAN NGUYEN
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
   inUsed                  (od.inUsed),					// made changes by TUAN NGUYEN
   removedReason           (od.removedReason),			// made changes by TUAN NGUYEN
   typeName                (od.typeName),
   type                    (od.type),
   uniqueID                (od.uniqueID),
   epochSystem             (od.epochSystem),
   epoch                   (od.epoch),
   participantIDs          (od.participantIDs),
   value                   (od.value),
   unit					   (od.unit),					// made changes by TUAN NGUYEN
   noiseCovariance         (od.noiseCovariance),
   extraDataDescriptions   (od.extraDataDescriptions),
   extraTypes              (od.extraTypes),
   extraData               (od.extraData),
//   dataFormat              (od.dataFormat),			// made changes by TUAN NGUYEN
   uplinkBand              (od.uplinkBand),				// made changes by TUAN NGUYEN
   uplinkFreq              (od.uplinkFreq),				// made changes by TUAN NGUYEN
   rangeModulo             (od.rangeModulo),			// made changes by TUAN NGUYEN
   dopplerCountInterval	   (od.dopplerCountInterval)	// made changes by TUAN NGUYEN
{
   dataFormat = od.dataFormat;							// made changes by TUAN NGUYEN
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
	  inUsed                  = od.inUsed;			// made changes by TUAN NGUYEN
	  removedReason           = od.removedReason;	// made changes by TUAN NGUYEN
      typeName                = od.typeName;
      type                    = od.type;
      uniqueID                = od.uniqueID;
      epochSystem             = od.epochSystem;
      epoch                   = od.epoch;
      participantIDs          = od.participantIDs;
      value                   = od.value;
	  unit                    = od.unit;			// made changes by TUAN NGUYEN
      noiseCovariance         = od.noiseCovariance;
      extraDataDescriptions   = od.extraDataDescriptions;
      extraTypes              = od.extraTypes;
      extraData               = od.extraData;
	  dataFormat              = od.dataFormat;		// made changes by TUAN NGUYEN
	  uplinkBand              = od.uplinkBand;		// made changes by TUAN NGUYEN
	  uplinkFreq              = od.uplinkFreq;		// made changes by TUAN NGUYEN
	  rangeModulo             = od.rangeModulo;		// made changes by TUAN NGUYEN
	  dopplerCountInterval    = od.dopplerCountInterval;	// made changes by TUAN NGUYEN
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
   inUsed                  = true;					// made changes by TUAN NGUYEN
   removedReason           = "N";					// made changes by TUAN NGUYEN
   typeName                = "";
   type                    = Gmat::UNKNOWN_MEASUREMENT;
   uniqueID                = -1;
   epoch                   = 0.0;
   participantIDs.clear();
   value.clear();
   extraDataDescriptions.clear();
   extraTypes.clear();
   extraData.clear();
///// TBD: Determine if there is a more generic way to add these
   uplinkBand              = 0;						// made changes by TUAN NGUYEN
   uplinkFreq              = 0.0;					// made changes by TUAN NGUYEN
   rangeModulo             = 1.0;					// made changes by TUAN NGUYEN
   dopplerCountInterval    = 1.0e-10;				// made changes by TUAN NGUYEN
}
