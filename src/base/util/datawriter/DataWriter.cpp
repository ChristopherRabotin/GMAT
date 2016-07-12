//$Id$
//------------------------------------------------------------------------------
//                           DataWriter
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
// Created: Jul 11, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#include "DataWriter.hpp"

DataWriter::DataWriter() :
   filename    (""),
   format      ("")
{
}

DataWriter::~DataWriter()
{
}

DataWriter::DataWriter(const DataWriter& dw) :
   allData           (dw.allData),
   variable_names    (dw.variable_names),
   filename          (dw.filename),
   format            (dw.format)
{
}

DataWriter& DataWriter::operator=(const DataWriter& dw)
{
   if (this != &dw)
   {
      allData        = dw.allData;
      variable_names = dw.variable_names;
      filename       = dw.filename;
      format         = dw.format;
   }

   return *this;
}

bool DataWriter::Initialize(const std::string &fname, const std::string &rev)
{
   bool retval = true;

   filename = fname;
   format   = rev;

   return retval;
}

bool DataWriter::OpenFile()
{
   return false;
}

bool DataWriter::CloseFile()
{
   return false;
}

//-------------------------------------------------------------------------------------
// bool AddData(WriterData * newDataContainer)
//-------------------------------------------------------------------------------------
/**
 * Adds data to a vector of WriterData objects in preparation for writing
 *
 * @param newDataContainer WriterData object
 */
//-------------------------------------------------------------------------------------
bool DataWriter::AddData(WriterData * newDataContainer)
{
   bool retval = true;

   allData.push_back(newDataContainer);

   return retval;
}

bool DataWriter::WriteData(const std::string &obj_name)
{
   return false;
}
