//$Id$
//------------------------------------------------------------------------------
//                           DataBucket
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 20, 2016
/**
 * Helper class used to collect data to be written using a DataWriter
 */
//------------------------------------------------------------------------------

#ifndef DataBucket_hpp
#define DataBucket_hpp

#include "utildefs.hpp"


/**
 * A generic container used to store data for use by a DataWriter
 *
 * All members here are public to facilitate fast access
 */
class GMATUTIL_API DataBucket
{
public:
   DataBucket();
   virtual ~DataBucket();
   DataBucket(const DataBucket& db);
   DataBucket& operator=(const DataBucket& db);

   Integer AddRealContainer(const std::string &name);
   Integer FindRealContainer(const std::string &name);
   Integer AddStringContainer(const std::string &name);
   Integer FindStringContainer(const std::string &name);

   Integer AddPoint();
   void Clear();

   /// Vector to track state for the data, so empty elements can be detected
   RealArray   elementStatus;
   /// Names of the real data collected
   StringArray realNames;
   /// The real data
   std::vector<RealArray> realValues;
   /// Names of the string data collected
   StringArray stringNames;
   /// The string data
   std::vector<StringArray> stringValues;

   bool fillToMatch;
};

#endif /* DataBucket_hpp */
