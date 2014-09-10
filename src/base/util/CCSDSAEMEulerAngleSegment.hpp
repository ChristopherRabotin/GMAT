//------------------------------------------------------------------------------
//                                  CCSDSAEMEulerAngleSegment
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2013.12.11
//
/**
 * Stores, validates, and manages meta data and Euler Angle real data read from,
 * or to be written to, a CCSDS Attitude Ephemeris Message file.
 * Currently, the meta data fields handled in this base class are:
 *    EULER_ROT_SEQ
 *
 */
//------------------------------------------------------------------------------
#ifndef CCSDSAEMEulerAngleSegment_hpp
#define CCSDSAEMEulerAngleSegment_hpp

#include "gmatdefs.hpp"
#include "Rmatrix33.hpp"
#include "Rvector.hpp"
#include "CCSDSAEMSegment.hpp"

class GMAT_API CCSDSAEMEulerAngleSegment : public CCSDSAEMSegment
{
public:
   /// class methods
   CCSDSAEMEulerAngleSegment(Integer segNum);
   CCSDSAEMEulerAngleSegment(const CCSDSAEMEulerAngleSegment &copy);
   CCSDSAEMEulerAngleSegment& operator=(const CCSDSAEMEulerAngleSegment &copy);

   virtual ~CCSDSAEMEulerAngleSegment();

   virtual CCSDSEMSegment*  Clone() const;

   virtual bool             Validate(bool checkData = true);
   virtual Rmatrix33        GetState(Real atEpoch);

   virtual bool             SetMetaData(const std::string &fieldName,
                                        const std::string &value);
   virtual bool             AddData(Real epoch, Rvector data);

protected:
   // Required metadata fields for an Euler Angle Attitude Ephemeris Message
   std::string eulerRotSeq;

   /// Optional meta data fields
   // none at this level

   /// Optional (stored but not currently used) fields
   /// none at this level

   /// other data
   Integer     euler1, euler2, euler3;

   virtual Rvector          Interpolate(Real atEpoch);
   bool                     ValidateEulerAngles(const Rvector &eAngles);
};

#endif // CCSDSAEMEulerAngleSegment_hpp
