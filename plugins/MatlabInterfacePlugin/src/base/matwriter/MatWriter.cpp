//$Id$
//------------------------------------------------------------------------------
//                              MatWriter
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
 * Defines the class used for to write MatData objects Matlab Data objects to
 *  .mat files
 */
//------------------------------------------------------------------------------
//
//

#include "MatWriter.hpp"
#include <stdlib.h>


//-------------------------------------------------------------------------------------
// MatWriter()
//-------------------------------------------------------------------------------------
/**
 * Constructs the MatWriter object (default constructor).
 *
 */
//-------------------------------------------------------------------------------------
MatWriter::MatWriter() :
   mat_struct(NULL),
   pmat(NULL)
{
}

//-------------------------------------------------------------------------------------
// MatWriter()
//-------------------------------------------------------------------------------------
/**
 * Destroys the mat_struct mxArray (destructor). 
 *
 */
//-------------------------------------------------------------------------------------
MatWriter::~MatWriter()
{
    mxDestroyArray(mat_struct);
}

//-------------------------------------------------------------------------------------
// MatWriter(const MatWriter& mw)
//-------------------------------------------------------------------------------------
/**
 * Constructs the MatWriter object (copy constructor)
 *
 * @param <mw> MatWriter object to copy
 *
 */
//-------------------------------------------------------------------------------------
MatWriter::MatWriter(const MatWriter& mw) :
    allData (mw.allData),
    variable_names (mw.variable_names)
{
}

//-------------------------------------------------------------------------------------
// MatWriter& operator=(const MatWriter &mw)
//-------------------------------------------------------------------------------------
/**
 * Sets one MatWriter object to match another
 *
 * @param <mw> The object that is copied
 *
 */
//-------------------------------------------------------------------------------------
MatWriter& MatWriter::operator=(const MatWriter &mw)
{
    if (this == &mw)
        return *this;

    allData = mw.allData;
    variable_names = mw.variable_names;

    return *this;
}

//-------------------------------------------------------------------------------------
// MatWriter::Initialize(const char *filename,  const char *mat_file_rev)
//-------------------------------------------------------------------------------------
/**
 * Initializes the MatWriter object, opens a file handle to the .mat file
 *
 * @param <filename> string name of the file you wish to open
 * @param <mat_file_rev> optional input arg for the type of .mat file to write. 
 *                       Defaults to version 6 
 *
 */
//-------------------------------------------------------------------------------------
void MatWriter::Initialize(const char *filename,  const char *mat_file_rev)
{
    // open file  
    OpenFile(filename, mat_file_rev);
}

//-------------------------------------------------------------------------------------
// MatWriter::OpenFile(const char *filename,  const char *mat_file_rev)
//-------------------------------------------------------------------------------------
/**
 * Opens a file handle to the .mat file
 *
 * @param <filename> string name of the file you wish to open
 * @param <mat_file_rev> optional input arg for the type of .mat file to write. 
 *                       Defaults to version 6 
 *
 * @return<pmat> File handle to .mat file
 */
//-------------------------------------------------------------------------------------
MATFile* MatWriter::OpenFile(const char *filename, const char *mat_file_rev)
{
    // check mat_file_rev
    if ( (strcmp(mat_file_rev,"w4")==0) || (strcmp(mat_file_rev,"w6")==0) || 
         (strcmp(mat_file_rev,"w7")==0) || (strcmp(mat_file_rev,"w7.3")==0))
    {
         printf("Writing .mat file using version %s\n", mat_file_rev);
    } 
    else
    {
         printf(".mat version %s invalid defaulting to w6\n", mat_file_rev);
         mat_file_rev = "w6";
    } 

    // open file
    pmat = matOpen(filename, mat_file_rev);

    if (pmat == NULL)
    { 
      printf("Error creating file %s\n", filename);
      printf("(Do you have write permission in this directory?)\n");
    }

    // return file handle to class
    return pmat;
}

//-------------------------------------------------------------------------------------
// MatWriter::AddData(MatData * MatDataContainer)
//-------------------------------------------------------------------------------------
/**
 * Adds data to a vector of MatData objects in preparation for writing
 *
 * @param <MatDataContainer> MatData object (RealMatData or StringMatData) 
 */
//-------------------------------------------------------------------------------------
void MatWriter::AddData(MatData * MatDataContainer)
{
	allData.push_back(MatDataContainer);
}

//-------------------------------------------------------------------------------------
// MatWriter::WriteData(const char *obj_name)
//-------------------------------------------------------------------------------------
/**
 * Writes data to .mat file, within the mat_struct structured array
 *
 * @param <obj_name> name of structured arry you want to write to
 *
 *
 * @return<exit code> Status code. Checks to see if mxArray associated with the object
 *                    requested to write to exists.
 */
//-------------------------------------------------------------------------------------
bool MatWriter::WriteData(const char *obj_name)
{
     if (mat_struct == NULL)
     {
        printf("MAT Structure array not created. Did you forget to call SetMxArray?\n");
        return (EXIT_FAILURE);
     }

     for (unsigned int i = 0; i < allData.size(); ++i)
     {
         allData[i]->WriteData(pmat, obj_name, mat_struct);

     }

     return (false);
}

//-------------------------------------------------------------------------------------
// MatWriter::SetMxArray(std::vector<std::string> variable_list)
//-------------------------------------------------------------------------------------
/**
 * Initializes the structured array that all the data will get written to.
 * data format will be mat_struct.variable 
 *
 * @param <variable_list> list of variable names to write to the structured array
 *
 */
//-------------------------------------------------------------------------------------
void MatWriter::SetMxArray(std::vector<std::string> variable_list)
{

    // get number of fields
    int number_of_fields = variable_list.size();

    // convert to cstrings
    const char *fields[number_of_fields];
    for (int i = 0; i < number_of_fields; i++)
    {
         fields[i] = variable_list[i].c_str();
    }
    
    // create structured array handle
    mat_struct = mxCreateStructMatrix(1, 1, number_of_fields, fields);

}

//-------------------------------------------------------------------------------------
// MatWriter::CloseFile()
//-------------------------------------------------------------------------------------
/**
 * Close the stream to the .mat file 
 *
 * @return<exit code> Status code. Checks to see if closing was successful
 */
//-------------------------------------------------------------------------------------
bool MatWriter::CloseFile()
{
    // close file
    if (matClose(pmat) != 0)
    {
        printf("Error closing .mat file \n"); 
        return (true);
    }
    else
    {
        return (false);
    }
}
