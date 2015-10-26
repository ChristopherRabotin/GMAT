//$Id: ObservationData.hpp 1398 2011-04-21 20:39:37Z  $
//------------------------------------------------------------------------------
//                         ObservationData
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
#include "Covariance.hpp"
#include "TimeSystemConverter.hpp"


/**
 * Class used to set and retrieve observation data
 */
class ESTIMATION_API ObservationData
{
public:
   ObservationData();
   virtual ~ObservationData();
   ObservationData(const ObservationData& od);
   ObservationData&  operator=(const ObservationData& od);

   void              Clear();

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
};

#endif /* ObservationData_hpp */
