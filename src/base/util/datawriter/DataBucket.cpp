//$Id$
//------------------------------------------------------------------------------
//                           DataBucket
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 20, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#include "DataBucket.hpp"
#include <algorithm>

#define REAL_INITIAL_VALUE -1.0


DataBucket::DataBucket() :
   fillToMatch          (true)
{
}

DataBucket::~DataBucket()
{
}

DataBucket::DataBucket(const DataBucket& db) :
   elementStatus        (db.elementStatus),
   realNames            (db.realNames),
   realValues           (db.realValues),
   stringNames          (db.stringNames),
   stringValues         (db.stringValues),
   fillToMatch          (db.fillToMatch)
{
}

DataBucket& DataBucket::operator =(const DataBucket& db)
{
   if (this != &db)
   {
      elementStatus = db.elementStatus;
      realNames     = db.realNames;
      realValues    = db.realValues;
      stringNames   = db.stringNames;
      stringValues  = db.stringValues;
      fillToMatch   = db.fillToMatch;
   }

   return *this;
}

Integer DataBucket::AddRealContainer(const std::string& name)
{
   Integer retval = -1;

   if (find(realNames.begin(), realNames.end(), name) == realNames.end())
   {
      RealArray data;
      if (fillToMatch)
         data.assign(elementStatus.size(), -1.0);

      realNames.push_back(name);
      realValues.push_back(data);
      retval = realValues.size() - 1;
   }

   return retval;
}

Integer DataBucket::FindRealContainer(const std::string& name)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < realNames.size(); ++i)
   {
      if (realNames[i] == name)
      {
         retval = i;
         break;
      }
   }

   return retval;
}

Integer DataBucket::AddStringContainer(const std::string& name)
{
   Integer retval = -1;

   if (find(stringNames.begin(), stringNames.end(), name) == stringNames.end())
   {
      StringArray data;
      if (fillToMatch)
         data.assign(elementStatus.size(), "N/A");

      stringNames.push_back(name);
      stringValues.push_back(data);
      retval = stringValues.size() - 1;
   }

   return retval;
}

Integer DataBucket::FindStringContainer(const std::string& name)
{
   Integer retval = -1;

   for (UnsignedInt i = 0; i < stringNames.size(); ++i)
   {
      if (stringNames[i] == name)
      {
         retval = i;
         break;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer AddPoint()
//------------------------------------------------------------------------------
/**
 * Adds a data point to the member containers
 *
 * This method adds a new element to each data array in the bucket, initializing
 * Reals to REAL_INITIAL_VALUE and strings to "N/A"
 *
 * @return The index into the containers, built on the size of the elementStatus
 * container.
 */
//------------------------------------------------------------------------------
Integer DataBucket::AddPoint()
{
   elementStatus.push_back(REAL_INITIAL_VALUE);

   for (UnsignedInt i = 0; i < realValues.size(); ++i)
      realValues[i].push_back(REAL_INITIAL_VALUE);

   for (UnsignedInt i = 0; i < stringValues.size(); ++i)
      stringValues[i].push_back("N/A");

   return elementStatus.size() - 1;
}


//------------------------------------------------------------------------------
// void Clear()
//------------------------------------------------------------------------------
/**
 *
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
void DataBucket::Clear()
{
   elementStatus.clear();

   for (UnsignedInt i = 0; i < realValues.size(); ++i)
      realValues[i].clear();

   for (UnsignedInt i = 0; i < stringValues.size(); ++i)
      stringValues[i].clear();
}
