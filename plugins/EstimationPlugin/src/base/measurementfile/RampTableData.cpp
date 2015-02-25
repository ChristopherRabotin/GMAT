//$Id: RampTableData.cpp 1398 2013-07-01 20:39:37Z tdnguyen $
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
 * retrieved from a FrequencyTableType object.
 */
//------------------------------------------------------------------------------


#include "RampTableData.hpp"
#include "EstimationDefs.hpp"


//-----------------------------------------------------------------------------
// RampTableData()
//-----------------------------------------------------------------------------
/**
 * Default constructor
 */
//-----------------------------------------------------------------------------
RampTableData::RampTableData() :
   typeName          (""),
   type              (Gmat::UNKNOWN_MEASUREMENT),
   epochSystem       (TimeConverterUtil::A1MJD),
   epoch             (-1.0),
   uplinkBand        (0),
   rampType          (1),
   rampFrequency     (0.0),
   rampRate          (0.0),
   indexkey          ("")
{
	dataFormat = "GMAT_RampTable"; 
}


//-----------------------------------------------------------------------------
// ~RampTableData()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
RampTableData::~RampTableData()
{
}


//-----------------------------------------------------------------------------
// RampTableData(const RampTableData& rtd):
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param rtd The RampTableData object that sets the data for the new one
 */
//-----------------------------------------------------------------------------
RampTableData::RampTableData(const RampTableData& rtd):
   typeName                (rtd.typeName),
   type                    (rtd.type),
   epochSystem             (rtd.epochSystem),
   epoch                   (rtd.epoch),
   participantIDs          (rtd.participantIDs),
   uplinkBand              (rtd.uplinkBand),
   rampType                (rtd.rampType),
   rampFrequency           (rtd.rampFrequency),
   rampRate                (rtd.rampRate),
   indexkey                (rtd.indexkey)
{
   dataFormat = rtd.dataFormat;
}


//-----------------------------------------------------------------------------
// RampTableData& operator=(const RampTableData& rtd)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param rtd The RampTableData object that sets the data for the this one
 *
 * @return This object, configured to match rtd
 */
//-----------------------------------------------------------------------------
RampTableData& RampTableData::operator=(const RampTableData& rtd)
{
   if (&rtd != this)
   {
      dataFormat              = rtd.dataFormat;
	   typeName                = rtd.typeName;
	   type                    = rtd.type;
      epochSystem             = rtd.epochSystem;
      epoch                   = rtd.epoch;
      participantIDs          = rtd.participantIDs;
      uplinkBand              = rtd.uplinkBand;
	   rampType                = rtd.rampType;
      rampFrequency           = rtd.rampFrequency;
      rampRate                = rtd.rampRate;
      indexkey                = rtd.indexkey;
   }

   return *this;
}


//-----------------------------------------------------------------------------
// void Clear()
//-----------------------------------------------------------------------------
/**
 * Clears all data from this RampTableData object.
 */
//-----------------------------------------------------------------------------
void RampTableData::Clear()
{
   dataFormat              = "";
   typeName                = "";
   type                    = Gmat::UNKNOWN_MEASUREMENT;
   epochSystem             = TimeConverterUtil::A1MJD;
   epoch                   = 0.0;
   participantIDs.clear();
   uplinkBand              = 0;
   rampType                = 1;
   rampFrequency           = 0.0;
   rampRate                = 0.0;
   indexkey                = ""; 
}
