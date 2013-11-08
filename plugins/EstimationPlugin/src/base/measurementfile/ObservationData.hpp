//$Id: ObservationData.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
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


#ifndef ObservationData_hpp
#define ObservationData_hpp

#include "estimation_defs.hpp"
#include "EstimationDefs.hpp"
///// TBD: Determine if there is a more generic way to add these
#include "GmatData.hpp"										// made changes by TUAN NGUYEN
#include "Covariance.hpp"
#include "TimeSystemConverter.hpp"


/**
 * Class used to set and retrieve observation data
 */
class ESTIMATION_API ObservationData : public GmatData		// made changes by TUAN NGUYEN
{
public:
   ObservationData();
   virtual ~ObservationData();
   ObservationData(const ObservationData& od);
   ObservationData&  operator=(const ObservationData& od);

   virtual void      Clear();									// made changes by TUAN NGUYEN

// Explicitly public so that this class acts like a struct
public:
   /// The text name of the data type, if available
   std::string       typeName;
   /// The type of measurement in this record
   Gmat::MeasurementType
                     type;
   /// Unique ID for associated data stream.
   Integer           uniqueID;
   /// Enumerated ID for the epoch time system
   TimeConverterUtil::TimeSystemTypes
                     epochSystem;
   /// The epoch of the measurement
   GmatEpoch         epoch;
   /// Who is involved in the measurement.  First one is the "anchor" node
   StringArray       participantIDs;
   /// The observed value.  Array to handle more than one value, like AZ_EL
   RealArray         value;
   /// The associated noise covariance matrix
   Covariance*       noiseCovariance;


   /// Strings describing any ancillary data in the observation source
   StringArray       extraDataDescriptions;
   /// Types for any ancillary data in the observation source
   IntegerArray      extraTypes;
   /// Ancillary data from the observation source, in string format
   StringArray       extraData;

///// TBD: Determine if there is a more generic way to add these
//   // This secsion added by TUAN NGUYEN
//   std::string dataFormat;			// Flag indicating which data format is used. Its value is either "GMATInternal" or "GMAT_OD"
   /// Uplink band
   Integer uplinkBand;
   /// Uplink frequency
   Real uplinkFreq;
   /// Range modulo
   Real rangeModulo;

   // This section is added by TUAN NGUYEN for Gmat_ODDoppler observation data
   /// Time difference between the reception time at station for start path and end path  
   Real dopplerCountInterval;

};

#endif /* ObservationData_hpp */
