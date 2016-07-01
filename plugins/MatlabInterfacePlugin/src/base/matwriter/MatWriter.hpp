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

#ifndef MatWriter_hpp
#define MatWriter_hpp

#include <vector>
#include "mat.h"
#include "MatData.hpp"


/*
 * All data containers to be written to .mat files are derived from this class
 */

class MatWriter
{
    public: 
        MatWriter();
        virtual ~MatWriter();
        MatWriter(const MatWriter& mw);
        MatWriter& operator=(const MatWriter &mw);

        void Initialize(const char *filename, const char *mat_file_rev="w6");
        MATFile* OpenFile(const char *filename, const char *mat_file_rev="w6");
        bool CloseFile();
        void AddData(MatData * MatDataContainer);
        bool WriteData(const char *obj_name);
        void SetMxArray(std::vector<std::string> variable_list);

    private:
        // handle to .mat file
        MATFile *pmat;
        // vector of MatData objects (could be of StringMatData and RealMatData types)
        std::vector<MatData*> allData;

    protected:
        // vector of variable names to write to the .mat file
        std::vector<std::string> variable_names;
        // matlab data array type. mat_struct defines matlab structured array
        // in which data gets written. Many structure arrays can be written to 
        // a single file, by setting a new mat_struct array
        mxArray *mat_struct;

}; 

#endif
