//------------------------------------------------------------------------------
//                                  CCSDSAEMReader
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
// Created: 2013.11.19
//
/**
 * Reads a CCSDS Attitude Ephemeris Message file, and manages segments.
 */
//------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include "CCSDSAEMReader.hpp"
#include "CCSDSAEMQuaternionSegment.hpp"
#include "CCSDSAEMEulerAngleSegment.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_AEM_READER_GET_STATE

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

// -----------------------------------------------------------------------------
// public methods
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// default constructor
// -----------------------------------------------------------------------------
CCSDSAEMReader::CCSDSAEMReader() :
   CCSDSEMReader()
{
   versionFieldName  = "CCSDS_AEM_VERS";
   metaSpecifiesType = true;
   metaDataTypeField = "ATTITUDE_TYPE";
}

// -----------------------------------------------------------------------------
// copy constructor
// -----------------------------------------------------------------------------
CCSDSAEMReader::CCSDSAEMReader(const CCSDSAEMReader &copy) :
   CCSDSEMReader(copy)
{
}

// -----------------------------------------------------------------------------
// operator=
// -----------------------------------------------------------------------------
CCSDSAEMReader& CCSDSAEMReader::operator=(const CCSDSAEMReader &copy)
{
   if (&copy == this)
      return *this;

   CCSDSEMReader::operator=(copy);

   return *this;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
CCSDSAEMReader::~CCSDSAEMReader()
{
}

// -----------------------------------------------------------------------------
// CCSDSEMReader* Clone()
// -----------------------------------------------------------------------------
/**
 * This method returns a clone of the CCSDSAEMReader.
 *
 * @return clone of the CCSDSAEMReader.
 */
//------------------------------------------------------------------------------
CCSDSEMReader* CCSDSAEMReader::Clone() const
{
   return (new CCSDSAEMReader(*this));
}

// -----------------------------------------------------------------------------
// void Initialize()
// -----------------------------------------------------------------------------
void CCSDSAEMReader::Initialize()
{
   CCSDSEMReader::Initialize();
   // nothing else to do here at this time
}

// -----------------------------------------------------------------------------
// Rmatrix33 GetState(Real atEpoch)
// -----------------------------------------------------------------------------
Rmatrix33 CCSDSAEMReader::GetState(Real atEpoch)
{
   #ifdef DEBUG_AEM_READER_GET_STATE
      MessageInterface::ShowMessage(
            "Entering AEMReader::GetState, with atEpoch = %lf\n",
            atEpoch);
      MessageInterface::ShowMessage("numSegments = %d\n", numSegments);
      for (Integer ii = 0; ii < numSegments; ii++)
         MessageInterface::ShowMessage("   segment[%d] is at <%p>\n",
               ii, segments.at(ii));
   #endif
   CCSDSEMSegment *theSegment = GetSegment(atEpoch);
   if (!theSegment)
   {
      #ifdef DEBUG_AEM_READER_GET_STATE
         MessageInterface::ShowMessage(
               "In AEMReader::GetState, segment not found!\n");
      #endif
      std::stringstream errmsg("");
      errmsg.precision(16);
      errmsg << "Time ";
      errmsg << atEpoch;
      errmsg << " is not contained in any segment present in file " << emFile << ".\n";
      throw UtilityException(errmsg.str());
   }
   #ifdef DEBUG_AEM_READER_GET_STATE
      MessageInterface::ShowMessage(
            "Entering AEMReader::GetState, segmentToUse = %p\n",
            theSegment);
   #endif
   CCSDSAEMSegment *theAEMSegment = (CCSDSAEMSegment*) theSegment;
   #ifdef DEBUG_AEM_READER_GET_STATE
      if (theAEMSegment == NULL) MessageInterface::ShowMessage("the segment to use is NULL!!!!\n");
   #endif
   return theAEMSegment->GetState(atEpoch);
}


// -----------------------------------------------------------------------------
// protected methods
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CCSDSEMSegment* CreateNewSegment(Integer segNum,
//                                  const std::string &ofType = "ANY")
// -----------------------------------------------------------------------------
CCSDSEMSegment* CCSDSAEMReader::CreateNewSegment(Integer segNum,
                                                 const std::string &ofType)
{
   if (ofType == "QUATERNION")
   {
      return new CCSDSAEMQuaternionSegment(segNum);
   }
   else if (ofType == "EULER_ANGLE")
   {
      return new CCSDSAEMEulerAngleSegment(segNum);
   }
   else
   {
      std::string errmsg = "Error reading ephemeris message file \"";
      errmsg += emFile + "\".  ";
      errmsg += "Attitude type \"" + ofType;
      errmsg += "\" is invalid or not supported.\n";
      throw UtilityException(errmsg);
   }
   return NULL;  // for testing
}

// -----------------------------------------------------------------------------
// bool IsValidVersion(const std::string &versionValue) const
// -----------------------------------------------------------------------------
bool CCSDSAEMReader::IsValidVersion(const std::string &versionValue) const
{
   // Only version allowed right now is 1.0
   if (versionValue == "1.0") return true;
   return false;
}
