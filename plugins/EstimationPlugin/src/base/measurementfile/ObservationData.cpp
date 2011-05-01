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
   typeName          (""),
   type              (Gmat::UNKNOWN_MEASUREMENT),
   uniqueID          (-1),
   epochSystem       (TimeConverterUtil::A1MJD),
   epoch             (-1.0),
   noiseCovariance   (NULL)
{
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
   typeName                (od.typeName),
   type                    (od.type),
   uniqueID                (od.uniqueID),
   epochSystem             (od.epochSystem),
   epoch                   (od.epoch),
   participantIDs          (od.participantIDs),
   value                   (od.value),
   noiseCovariance         (od.noiseCovariance),
   extraDataDescriptions   (od.extraDataDescriptions),
   extraTypes              (od.extraTypes),
   extraData               (od.extraData)
{
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
      typeName                = od.typeName;
      type                    = od.type;
      uniqueID                = od.uniqueID;
      epochSystem             = od.epochSystem;
      epoch                   = od.epoch;
      participantIDs          = od.participantIDs;
      value                   = od.value;
      noiseCovariance         = od.noiseCovariance;
      extraDataDescriptions   = od.extraDataDescriptions;
      extraTypes              = od.extraTypes;
      extraData               = od.extraData;
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
   typeName                = "";
   type                    = Gmat::UNKNOWN_MEASUREMENT;
   uniqueID                = -1;
   epoch                   = 0.0;
   participantIDs.clear();
   value.clear();
   extraDataDescriptions.clear();
   extraTypes.clear();
   extraData.clear();
}
