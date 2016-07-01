//$Id$
//------------------------------------------------------------------------------
//                              MatData
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
 * Defines the base class used for Matlab Data objects used to 
 * write .mat output files (string or numerical data) 
 */
//------------------------------------------------------------------------------

#include <stdio.h>
#include "mat.h"
#include <iostream>
#include <string.h>
#include <vector>

#ifndef MatData_hpp
#define MatData_hpp

typedef std::vector<std::vector<double> > Matrix;
typedef std::vector<std::string> StringArray;
typedef std::vector<StringArray> StringMatrix;

class MatData  
{
    public:
        MatData();
        virtual ~MatData();
        MatData(const MatData &md);
        virtual void WriteData(MATFile *pmat, const char *obj_name, mxArray *mat_struct) = 0;
};

#endif
