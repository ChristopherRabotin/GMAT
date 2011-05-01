//$Id: MeasurementData.hpp 1398 2011-04-21 20:39:37Z ljun@NDC $
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
 * Definition of the MeasurementData class, used to manage calculated
 * measurements
 */
//------------------------------------------------------------------------------


#ifndef MeasurementData_hpp
#define MeasurementData_hpp

#include "estimation_defs.hpp"
#include "EstimationDefs.hpp"
#include "Covariance.hpp"
#include "TimeSystemConverter.hpp"

/**
 *  The measurement data structure
 *
 *  This class acts mostly as a structure which provides the core set of
 *  information used for calculated measurement data.  Each measurement can be
 *  captured in this structure.
 *
 *  This struct presents as a class because it includes code for the assignment
 *  operator and copy constructor.  To me, it's just bad form to call something
 *  a struct and include those pieces.
 */
class ESTIMATION_API MeasurementData
{
public:
   MeasurementData();
   virtual ~MeasurementData();
   MeasurementData(const MeasurementData& md);
   MeasurementData operator=(const MeasurementData& md);


// Explicitly public so that this class acts like a struct
public:
   /// The type of measurement in this record
   Gmat::MeasurementType   type;
   /// String value for type of measurement in this record
   std::string       typeName;
   /// Unique ID for associated model.  This number can change from run to run.
   Integer           uniqueID;
   /// Enumerated ID for the epoch time system
   TimeConverterUtil::TimeSystemTypes
                     epochSystem;
   /// The epoch of the measurement
   GmatEpoch         epoch;
   /// Who is involved in the measurement.  First one is the "anchor" node
   StringArray       participantIDs;
   /// The measured value.  Array to handle more than one value, like AZ_EL
   RealArray         value;
   /// Flag indicating if the measurement could be made when it was attempted
   bool              isFeasible;
   /// Value used for root finding
   Real              feasibilityValue;
   /// Measurement error covariance matrix
   Covariance        *covariance;
   /// Number of events associated with this measurement
   Integer           eventCount;
};

#endif /* MeasurementData_hpp */
