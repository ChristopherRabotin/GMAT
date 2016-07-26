//$Id$
//------------------------------------------------------------------------------
//                              StringMatData
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2016 United States Government as represented by the
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Michael A. Barrucco, Thinking Systems, Inc.
// Created: 2016/6/24
//
/**
 * Defines the string data class used to define a container to hold strings in a 
 * way that the MathWorks mat writer understands
 */
//------------------------------------------------------------------------------

#include "StringMatData.hpp"
#include "UtilityException.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// StringMatData(const char * variable_name)
//------------------------------------------------------------------------------
/**
 * Constructs the StringMatData object (default constructor).
 *
 * @param variable_name Variable name of the data to be written to the .mat 
 *                      file structured array
 */
//------------------------------------------------------------------------------
StringMatData::StringMatData(const std::string &variable_name) :
   MatData           (variable_name),
   m_string          (0),
   m_size            (0),
   n_size            (0),
   pa_string         (NULL)
{
   dataType = Gmat::STRING_TYPE;
}

//------------------------------------------------------------------------------
// ~StringMatData()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
StringMatData::~StringMatData()
{
}

//------------------------------------------------------------------------------
// StringMatWriter(const StringMatData& sd)
//------------------------------------------------------------------------------
/**
 * Constructs a StringMatData object (copy constructor)
 *
 * @param sd StringMatData object to copy
 *
 */
//------------------------------------------------------------------------------
StringMatData::StringMatData(const StringMatData &sd) :
   MatData           (sd),
   m_string          (sd.m_string),
   m_size            (sd.m_size),
   n_size            (sd.n_size),
   pa_string         (NULL)
{
}

//------------------------------------------------------------------------------
// StringMatData& operator=(const StringMatData &sd)
//------------------------------------------------------------------------------
/**
 * Sets one string mat data object to match another
 *
 * @param sd The object that is copied
 *
 * @return This object, configured to match sd
 */
//------------------------------------------------------------------------------
StringMatData& StringMatData::operator=(const StringMatData &sd)
{
   if (this !=&sd)
   {
      MatData::operator=(sd);

      m_string   = sd.m_string;
      m_size     = sd.m_size;
      n_size     = sd.n_size;
      pa_string  = NULL;
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool AddData(StringMatrix data))
//------------------------------------------------------------------------------
/**
 * Sets up an array of input strings in a mxArray for writing.
 *
 * @params data String data to write to the .mat file
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool StringMatData::AddData(const StringMatrix &data)
{
   bool retval = false;

   #ifdef DEBUG_ADD_DATA
      MessageInterface::ShowMessage("Adding string data; matrix is %d "
            "arrays big\n", data.size());
   #endif

   // size of array
   m_size = data.size();
   n_size = data[0].size();

   // init data
   pa_string = mxCreateCellMatrix(m_size, n_size);
   m_string = mxGetNumberOfDimensions(pa_string);
   mwIndex *subs = (mwIndex*)mxCalloc(m_string,sizeof(mwIndex));
   mwIndex index;

   for (int i=0; i<m_size; i++)
   {
      mxArray *tmp_str;

      for (int j=0; j<n_size; j++)
      {
         #ifdef DEBUG_ADD_DATA
            MessageInterface::ShowMessage("[%d][%d] = \"%s\"\n", i, j,
                  data[i][j].c_str());
         #endif

         tmp_str = mxCreateString(data[i][j].c_str());
         subs[1]=(mwIndex)(i+j*m_size);
         index = mxCalcSingleSubscript(pa_string, m_size, subs);
         mxSetCell(pa_string, subs[1], tmp_str );

         retval = true;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// void StringMatDatia::WriteData(MATFile *pmat, const char *obj_name,
//       mxArray *mat_struct)
//------------------------------------------------------------------------------
/**
 * Writes string data to the open .mat file stream.
 *
 * @params pmat       File handle to .mat file
 * @params obj_name   Name of the structured array in which to write the data
 * @params mat_struct mxArray of structured array in which to write
 *
 */
//------------------------------------------------------------------------------
void StringMatData::WriteData(MATFile *matfile, const std::string &objectName,
      mxArray *mx_struct)
{
   pmat      = matfile;
   obj_name  = objectName;
   mat_struct = mx_struct;

   int data_field_number = mxGetFieldNumber(mat_struct, varName.c_str());
   mxSetFieldByNumber(mat_struct, 0, data_field_number, pa_string);
   matPutVariable(pmat, obj_name.c_str(), mat_struct);
}

//------------------------------------------------------------------------------
// bool StringMatData::WriteData()
//------------------------------------------------------------------------------
/**
 * Override for abstract method in the base class. Currently just returns false.
 *
 * @return false always.  MatWriters don't use this base class version.
 */
//------------------------------------------------------------------------------
bool StringMatData::WriteData()
{
   return false;
}
