//------------------------------------------------------------------------------
//                                  CCSDSEMSegment
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2013.11.19
//
/**
 * Stores, validates, and manages meta data and real data read from, or
 * to be written to, a CCSDS Ephemeris Message file.
 * This is the abstract base class from which other classes must derive (e.g.
 * a class to handle attitude segments).
 * Currently, the common meta data fields handled in this base class are:
 *    COMMENT *
 *    OBJECT_NAME *
 *    OBJECT_ID *
 *    CENTER_NAME **
 *    TIME_SYSTEM
 *    START_TIME
 *    USEABLE_START_TIME
 *    USEABLE_STOP_TIME
 *    STOP_TIME
 *    INTERPOLATION_DEGREE
 *       * GMAT saves but ignores these
 *       ** GMAT saves this but validates in child classes
 *
 * In addition, the data fields handled here are:
 *    COMMENT
 *
 *    All others needed must be handled in the appropriate child class.
 *
 */
//------------------------------------------------------------------------------
#ifndef CCSDSEMSegment_hpp
#define CCSDSEMSegment_hpp

#include "gmatdefs.hpp"
#include "Rvector.hpp"

class GMAT_API CCSDSEMSegment
{
public:
   // static methods
   /// Parse a time string read from the EM file and convert it to
   /// a Real (A1Mjd) epoch
   static Real ParseEpoch(const std::string &epochString);

   /// class methods
   CCSDSEMSegment(Integer segNum);
   CCSDSEMSegment(const CCSDSEMSegment &copy);
   CCSDSEMSegment& operator=(const CCSDSEMSegment &copy);

   virtual ~CCSDSEMSegment();

   virtual CCSDSEMSegment* Clone() const = 0;

   virtual bool    Validate(bool checkData = true);

   // A GetState method must be added to child classes, returning the
   // requested state data in the required format/representation

   /// Sets a meta data field - assumes fields are all Caps
   virtual bool    SetMetaData(const std::string &fieldName,
                               const std::string &value);
   virtual bool    AddData(Real epoch, Rvector data);
   virtual bool    AddDataComment(const std::string& comment);

   virtual Integer GetDataSize() const;
   virtual bool    CoversEpoch(Real theEpoch);
   virtual Real    GetStartTime() const;
   virtual Real    GetStopTime() const;

protected:
   // struct to hold epochs and data
   struct EpochAndData
   {
      Real    epoch;
      // data vector will be allocated/set in the child classes according to
      // the size needed, e.g. an attitude quaternion segment would need
      // an Rvector of size 4
      Rvector data;
   };

   /// Store the data in a vector
   std::vector<EpochAndData*> dataStore;

   // The number of the segment (its position in the file)
   Integer     segmentNumber;
   /// Actual size of the data needed
   Integer     dataSize;
   /// What is the type of data for this segment?  (set in child classes)
   std::string dataType;
   /// Standard part of an error message for the segment
   std::string segError;

   // Required meta data fields
   std::string timeSystem;
   Real        startTime;
   Real        stopTime;

   /// Optional meta data fields
   Real        usableStartTime;
   Real        usableStopTime;
   // The keyword associated with this item is different depending on the
   // type of Ephmeris Message file; therefore, it is handled in the
   // appropriate child class
   std::string interpolationMethod;
   Integer     interpolationDegree;

   /// Unused (but stored for possible future use) meta data fields
   StringArray metaComments;
   std::string objectName;
   std::string objectID;
   std::string centerName;

   StringArray dataComments;

   // other data
   /// Does the segment contain usable start and stop times?
   bool        usesUsableTimes;

   bool        checkLagrangeOrder;

   Integer     firstUsable;
   Integer     lastUsable;

   // static data

   /// Tolerance to use when looking for an exact epoch match
   static const Real        EPOCH_MATCH_TOLERANCE;
   /// Initial value of a string field that GMAT does not currently use
   static const std::string UNUSED_STRING;
   /// Initial value of a string field that GMAT does use indicating
   /// that it has not yet been set
   static const std::string UNSET_STRING;
   /// Initial value of a Real field that GMAT does use indicating
   /// that it has not yet been set
   static const Real        UNSET_REAL;
   /// Initial value of an Integer field that GMAT does use indicating
   /// that it has not yet been set
   static const Integer     UNSET_INTEGER;

   // Look for an exact epoch match
   virtual Rvector      DetermineState(Real atEpoch);
   virtual bool         GetUsableIndexRange(Integer &first, Integer &last);
   /// Interpolate the data if necessary
   virtual Rvector      Interpolate(Real atEpoch) = 0;
   virtual Rvector      InterpolateLagrange(Real atEpoch);
   virtual Rvector      InterpolateSLERP(Real atEpoch);
};

#endif // CCSDSEMSegment_hpp