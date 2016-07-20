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


//------------------------------------------------------------------------------
// DataWriter() :
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
DataWriter::DataWriter() :
   filename    (""),
   format      ("")
{
}

//------------------------------------------------------------------------------
// ~DataWriter()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
DataWriter::~DataWriter()
{
}

//------------------------------------------------------------------------------
// DataWriter(const DataWriter& dw) :
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param dw The DataWriter providing settings for this new one
 */
//------------------------------------------------------------------------------
DataWriter::DataWriter(const DataWriter& dw) :
   allData           (dw.allData),
   variable_names    (dw.variable_names),
   filename          (dw.filename),
   format            (dw.format)
{
}

//------------------------------------------------------------------------------
// DataWriter& operator=(const DataWriter& dw)
//------------------------------------------------------------------------------
/**
 * Assignment Operator
 *
 * @param dw The writer setting values here
 *
 * @return This DataWriter, configured like dw
 */
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
// bool DataWriter::Initialize(const std::string &fname, const std::string &rev)
//------------------------------------------------------------------------------
/**
 * Prepares the data structures for use
 *
 * @param fname Name of the output file
 * @param rev Extra information, if needed, to open the file
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool DataWriter::Initialize(const std::string &fname, const std::string &rev)
{
   bool retval = true;

   filename = fname;
   format   = rev;

   return retval;
}

//------------------------------------------------------------------------------
// bool OpenFile()
//------------------------------------------------------------------------------
/**
 * Opens the data file for writing
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataWriter::OpenFile()
{
   return false;
}

//------------------------------------------------------------------------------
// bool CloseFile()
//------------------------------------------------------------------------------
/**
 * Closes the data file after writing
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
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


//------------------------------------------------------------------------------
// bool DataWriter::DescribeData (const StringArray &variableList)
//------------------------------------------------------------------------------
/**
 * Method used to prepare teh writer, if needed, for incoming data
 *
 * @param variableList The names of the incoming data
 *
 * @return true on success
 */
//------------------------------------------------------------------------------
bool DataWriter::DescribeData (const StringArray &variableList)
{
   return true;
}
