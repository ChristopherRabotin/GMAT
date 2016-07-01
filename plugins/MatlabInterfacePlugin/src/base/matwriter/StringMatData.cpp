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
 * Defines the string data class used to define a container to hold strings in a way 
 * that the MathWorks mat writer understands
 *
 */
//------------------------------------------------------------------------------
#include "StringMatData.hpp"

//-------------------------------------------------------------------------------------
// StringMatData(const char * variable_name)
//-------------------------------------------------------------------------------------
/**
 * Constructs the StringMatData object (default constructor).
 *
 * @param <variable_name> variable name of the data to be written to the .mat file structured
 *                        array
 *
 */
//-------------------------------------------------------------------------------------
StringMatData::StringMatData(const char * variable_name)
{
    variable = variable_name;
}

//-------------------------------------------------------------------------------------
// StringMatData()
//-------------------------------------------------------------------------------------
/**
 * Doesnt currently do anything(default destructor).
 *
 */
//-------------------------------------------------------------------------------------
StringMatData::~StringMatData()
{
}

//-------------------------------------------------------------------------------------
// StringMatWriter(const StringMatData& sd)
//-------------------------------------------------------------------------------------
/**
 * Constructs the StringMatData object (copy constructor)
 *
 * @param <sd> StringMatData object to copy
 *
 */
//-------------------------------------------------------------------------------------
StringMatData::StringMatData(const StringMatData &sd)
{
}

//-------------------------------------------------------------------------------------
// StringMatData& operator=(const StringMatData &sd)
//-------------------------------------------------------------------------------------
/**
 * Sets one string mat data object to match another
 *
 * @param <sd> The object that is copied
 *
 */
//-------------------------------------------------------------------------------------
StringMatData& StringMatData::operator=(const StringMatData &sd)
{
    if (this ==&sd)
       return *this;

    return *this;
}

//-------------------------------------------------------------------------------------
// StringMatData::AddData(StringMatrix data))
//-------------------------------------------------------------------------------------
/**
 * Sets up an array of input strings in a mxArray for writing.
 *
 * @params <data> string data to write to the .mat file
 *
 */
//-------------------------------------------------------------------------------------
bool StringMatData::AddData(StringMatrix data)
{
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
        subs[0] = (mwIndex)i;

        for (int j=0; j<n_size; j++)
        {
             tmp_str = mxCreateString(data[i][j].c_str());
             subs[1]=(mwIndex)j;
             index = mxCalcSingleSubscript(pa_string, m_size, subs);
             mxSetCell(pa_string, index, tmp_str );
        }
    }
}

//-------------------------------------------------------------------------------------
// StringMatDatia::WriteData(MATFile *pmat, const char *obj_name, mxArray *mat_struct)
//-------------------------------------------------------------------------------------
/**
 * Writes string data to the open .mat file stream.
 *
 * @params <pmat> file handle to .mat file
 * @params <obj_name> name of the structured array in which to write the data
 * @params <mat_struct> mxArray of structured array in which to write
 *
 */
//-------------------------------------------------------------------------------------
void StringMatData::WriteData(MATFile *pmat, const char *obj_name, mxArray *mat_struct)
{
    int data_field_number = mxGetFieldNumber(mat_struct, variable);
    mxSetFieldByNumber(mat_struct, 0, data_field_number, pa_string);
    matPutVariable(pmat, obj_name, mat_struct); 

}
