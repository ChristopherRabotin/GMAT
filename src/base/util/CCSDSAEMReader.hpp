//------------------------------------------------------------------------------
//                                  CCSDSAEMReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2013.11.19
//
/**
 * Reads a CCSDS Attitude Ephemeris Message file, and manages segments.
 */
//------------------------------------------------------------------------------
#ifndef CCSDSAEMReader_hpp
#define CCSDSAEMReader_hpp

#include "gmatdefs.hpp"
#include "CCSDSEMReader.hpp"
#include "CCSDSEMSegment.hpp"
#include "Rmatrix33.hpp"

class GMAT_API CCSDSAEMReader : public CCSDSEMReader
{
public:
   /// class methods
   CCSDSAEMReader();
   CCSDSAEMReader(const CCSDSAEMReader &copy);
   CCSDSAEMReader& operator=(const CCSDSAEMReader &copy);

   virtual ~CCSDSAEMReader();

   virtual CCSDSAEMReader* Clone() const;

   /// Initialize will parse the file, initialize and validate each
   /// segment, and check to make sure the segments are ordered
   /// correctly by increasing time
   virtual void            Initialize();
   /// Returns the DCM from inertial-to-body given the input epoch
   virtual Rmatrix33       GetState(Real atEpoch);

protected:

   /// Required header fields
   /// none

   /// Other data
   /// none

   /// Create and return a new segment of the correct type
   virtual CCSDSEMSegment* CreateNewSegment(Integer segNum, const std::string &ofType = "ANY");
   /// Check to see if the version number on the file is valid for this type of
   // ephemeris message file
   virtual bool            IsValidVersion(const std::string &versionValue) const;
};

#endif // CCSDSAEMReader_hpp
