//$Id: MeasurementData.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         MeasurementData
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
// Created: 2009/07/15
//
/**
 * Implementation of the MeasurementData class, used to manage calculated
 * measurements
 */
//------------------------------------------------------------------------------


#include "MeasurementData.hpp"


//-----------------------------------------------------------------------------
// MeasurementData()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
MeasurementData::MeasurementData() :
   type        (Gmat::UNKNOWN_MEASUREMENT),
   typeName    ("Unknown"),
   uniqueID    (-1),
   epochSystem (TimeConverterUtil::A1MJD),
   epoch       (0.0),
   isFeasible  (false),
   covariance  (NULL),
   eventCount  (0)
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
   type           (md.type),
   typeName       (md.typeName),
   uniqueID       (md.uniqueID),
   epochSystem    (md.epochSystem),
   epoch          (md.epoch),
   participantIDs (md.participantIDs),
   value          (md.value),
   isFeasible     (md.isFeasible),
   covariance     (md.covariance),
   eventCount     (md.eventCount)
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
      type            = md.type;
      typeName        = md.typeName;
      uniqueID        = md.uniqueID;
      epochSystem     = md.epochSystem;
      epoch           = md.epoch;
      participantIDs  = md.participantIDs;
      value           = md.value;
      isFeasible      = md.isFeasible;
      covariance      = md.covariance;
      eventCount      = md.eventCount;
   }

   return *this;
}
