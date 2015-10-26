//------------------------------------------------------------------------------
//                              Load3ds
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
// ** Legal **
//
// Author: Phillip Silvia, Jr.
// Created: 2009/06/24
/**
 * Loads a .3ds file and stores the information in a ModelObject
 */
//------------------------------------------------------------------------------

#ifndef _LOAD_3DS_H
#define _LOAD_3DS_H

#include "ModelObject.hpp"

// Constants
#define LOAD3DS_DEBUG 0

// Functions
extern char Load3DS(ModelObject *p_object, const std::string &p_filename);

#endif
