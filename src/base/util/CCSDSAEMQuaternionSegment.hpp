//------------------------------------------------------------------------------
//                                  CCSDSAEMQuaternionSegment
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Author: Wendy Shoan / NASA
// Created: 2013.11.21
//
/**
 * Stores, validates, and manages meta data and Quaternion real data read from,
 * or to be written to, a CCSDS Attitude Ephemeris Message file.
 * Currently, the meta data fields handled in this base class are:
 *    QUATERNION_TYPE
 *
 */
//------------------------------------------------------------------------------
#ifndef CCSDSAEMQuaternionSegment_hpp
#define CCSDSAEMQuaternionSegment_hpp

#include "gmatdefs.hpp"
#include "Rmatrix33.hpp"
#include "Rvector.hpp"
#include "CCSDSAEMSegment.hpp"

class GMAT_API CCSDSAEMQuaternionSegment : public CCSDSAEMSegment
{
public:
   /// class methods
   CCSDSAEMQuaternionSegment(Integer segNum);
   CCSDSAEMQuaternionSegment(const CCSDSAEMQuaternionSegment &copy);
   CCSDSAEMQuaternionSegment& operator=(const CCSDSAEMQuaternionSegment &copy);

   virtual ~CCSDSAEMQuaternionSegment();

   virtual CCSDSEMSegment*  Clone() const;

   virtual bool             Validate(bool checkData = true);
   virtual Rmatrix33        GetState(Real atEpoch);

   virtual bool             SetMetaData(const std::string &fieldName,
                                        const std::string &value);
   virtual bool             AddData(Real epoch, Rvector data);

protected:
   // Required metadata fields for a Quaternion Attitude Ephemeris Message
   std::string quaternionType;

   /// Optional meta data fields
   // none at this level

   /// Optional (stored but not currently used) fields
   /// none at this level

   /// other data
   /// none at this level

   virtual Rvector          Interpolate(Real atEpoch);
   bool                     ValidateQuaternion(const Rvector &quat);
};

#endif // CCSDSAEMQuaternionSegment_hpp
