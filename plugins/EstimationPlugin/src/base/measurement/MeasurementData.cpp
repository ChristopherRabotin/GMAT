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
   unfeasibleReason ("N"),		      // made changes by TUAN NGUYEN
   feasibilityValue (0.0),		      // made changes by TUAN NGUYEN
   covariance       (NULL),
   eventCount       (0),
   uplinkBand       (0),				// made changes by TUAN NGUYEN
   uplinkFreq       (0.0),			   // made changes by TUAN NGUYEN
   rangeModulo      (1.0),		   	// made changes by TUAN NGUYEN
   dopplerCountInterval	(1.0e-10)	// made changes by TUAN NGUYEN
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
   //if (covariance)
   //   delete covariance;
   /*MessageInterface::ShowMessage("MeasurementData destruction before!!!!\n");
   CleanUp();
   MessageInterface::ShowMessage("MeasurementData destruction !!!!\n");*/
}

void MeasurementData::CleanUp()
{
   if (covariance)
      delete covariance;
   covariance = NULL;

   participantIDs.clear();
//   MessageInterface::ShowMessage("MeasurementData CleanUp #####\n");
   //type             = Gmat::UNKNOWN_MEASUREMENT;
   //typeName         = "Unknown";
   //uniqueID         = -1;
   //epochSystem      = TimeConverterUtil::A1MJD;
   //epoch            = 0.0;
   //isFeasible       = false;
   //unfeasibleReason = "N";
   //feasibilityValue = 0.0;
   //eventCount       = 0;
   //uplinkBand       = 0;
   //uplinkFreq       = 0.0;
   //rangeModulo      = 1.0;
   //dopplerCountInterval	=1.0e-10;
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
   unfeasibleReason (md.unfeasibleReason),           // made changes by TUAN NGUYEN
   feasibilityValue (md.feasibilityValue),	        // made changes by TUAN NGUYEN
   covariance       (md.covariance),
   eventCount       (md.eventCount),
///// TBD: Do these go here?
   uplinkBand       (md.uplinkBand),			        // made changes by TUAN NGUYEN
   uplinkFreq       (md.uplinkFreq),			        // made changes by TUAN NGUYEN
   rangeModulo      (md.rangeModulo),			        // made changes by TUAN NGUYEN
   dopplerCountInterval	(md.dopplerCountInterval)	  // made changes by TUAN NGUYEN
{
      //if (covariance != md.covariance)
      //{
      //   if (covariance)
      //      delete covariance;
      //   covariance = NULL;

      //   if (md.covariance)
      //   {
      //      covariance = new Covariance();
      //      covariance->SetDimension(md.covariance->GetDimension());
      //      *covariance = *md.covariance;
      //   }
      //}

   //if (md.covariance != NULL)                        // made changes by TUAN NGUYEN
   //{                                                 // made changes by TUAN NGUYEN
   //   covariance = new Covariance();                 // made changes by TUAN NGUYEN
   //   *covariance = *(md.covariance);                // made changes by TUAN NGUYEN
   //}                                                 // made changes by TUAN NGUYEN
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
	   unfeasibleReason = md.unfeasibleReason;         // made changes by TUAN NGUYEN
	   feasibilityValue = md.feasibilityValue;	      // made changes by TUAN NGUYEN
      covariance       = md.covariance;
      eventCount       = md.eventCount;
	   uplinkBand       = md.uplinkBand;			      // made changes by TUAN NGUYEN
	   uplinkFreq       = md.uplinkFreq;			      // made changes by TUAN NGUYEN
	   rangeModulo      = md.rangeModulo;			      // made changes by TUAN NGUYEN
	   dopplerCountInterval = md.dopplerCountInterval;	// made changes by TUAN NGUYEN

      //if (covariance != md.covariance)
      //{
      //   if (covariance)
      //      delete covariance;
      //   covariance = NULL;

      //   if (md.covariance)
      //   {
      //      covariance = new Covariance();
      //      covariance->SetDimension(md.covariance->GetDimension());
      //      *covariance = *md.covariance;
      //   }
      //}

      //if (md.covariance)
      //{
      //   if (covariance == NULL)
      //      covariance = new Covariance();
      //   (*covariance) = *(md.covariance);
      //}
   }

   return *this;
}
