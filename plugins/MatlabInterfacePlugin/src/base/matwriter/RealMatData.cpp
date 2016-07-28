//$Id$
//------------------------------------------------------------------------------
//                              RealMatData
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
 * Defines the string data class used to define a container to hold strings in a way 
 * that the MathWorks mat writer understands
 *
 */

#include "RealMatData.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// RealMatData(const std::string &variable_name)
//------------------------------------------------------------------------------
/**
 * Constructs the RealMatData object (default constructor).
 *
 * @param variable_name Variable name of the data to be written to the .mat 
 *                      file structured array
 */
//------------------------------------------------------------------------------
RealMatData::RealMatData(const std::string &variable_name) :
   MatData        (variable_name)
{
}

//------------------------------------------------------------------------------
// ~RealMatData()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
RealMatData::~RealMatData()
{
}

//------------------------------------------------------------------------------
// RealMatWriter(const RealMatData& rd)
//------------------------------------------------------------------------------
/**
 * Constructs the RealMatData object (copy constructor)
 *
 * @param rd RealMatData object to copy
 */
//------------------------------------------------------------------------------
RealMatData::RealMatData(const RealMatData &rd) :
   MatData        (rd)
{
}

//------------------------------------------------------------------------------
// RealMatData& operator=(const RealMatData &rd)
//------------------------------------------------------------------------------
/**
 * Sets one string mat data object to match another
 *
 * @param sd The object that is copied
 */
//------------------------------------------------------------------------------
RealMatData& RealMatData::operator=(const RealMatData &rd)
{
   if (this != &rd)
   {
      MatData::operator=(rd);
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool AddData(std::vector< RealArray> data))
//------------------------------------------------------------------------------
/**
 * Sets up an array of input reals for writing to an mxArray/.mat file.
 *
 * @params data string data to write to the .mat file.
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool RealMatData::AddData(const Matrix &data)
{
   bool retval = false;
   
   // get matrix size
   m_size = data.size();
   n_size = data[0].size();

   // set data
   data_tmp = (double *)mxMalloc(m_size*n_size* sizeof(double));

   for (int i = 0; i < m_size; i++)
   {
      for (int j = 0; j < n_size; j++)
      {
         data_tmp[i + j*m_size ] = data[i][j];
         retval = true;
      }
   }

   return retval;
}

//------------------------------------------------------------------------------
// void WriteData(MATFile *pmat, const std::string &obj_name, 
//       mxArray *mat_struct)
//------------------------------------------------------------------------------
/**
 * Writes real data to the open .mat file stream.
 *
 * @params pmat       File handle to .mat file
 * @params obj_name   Name of the structured array in which to write the data
 * @params mat_struct mxArray of structured array in which to write
 */
//------------------------------------------------------------------------------
void RealMatData::WriteData(MATFile *pmat, const std::string &obj_name,
      mxArray *mat_struct)
{
   // find place to write the data within the struct
   int data_field_number = mxGetFieldNumber(mat_struct, varName.c_str());

   // create number matrix to house the data
   mxArray *pa1;
   pa1 = mxCreateNumericMatrix(0, 0, mxDOUBLE_CLASS, mxREAL);

   // write the data
   mxSetData(pa1, data_tmp);
   mxSetM(pa1, m_size);
   mxSetN(pa1, n_size);
   mxSetFieldByNumber(mat_struct, 0, data_field_number, pa1);
   matPutVariable(pmat, obj_name.c_str(), mat_struct);
}


//------------------------------------------------------------------------------
// bool WriteData()
//------------------------------------------------------------------------------
/**
 * Writes real data to the open .mat file stream.
 *
 * @return false (currently does nothing)
 */
//------------------------------------------------------------------------------
bool RealMatData::WriteData()
{
   return false;
}
