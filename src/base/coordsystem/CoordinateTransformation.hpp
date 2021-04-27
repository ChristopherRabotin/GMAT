//$Id$
//------------------------------------------------------------------------------
//                           CoordinateTransformation
//------------------------------------------------------------------------------
// Copyright (c) 2002 - 2020 United States Government as represented by the
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
// contract, Task Order 08.
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: 2019.04.10
//
/**
* Original Python prototype:
* Author: N. Hatten
*
* Declares the CoordinateTransformation class.
*/
//------------------------------------------------------------------------------
#ifndef CoordianteTransformation_hpp
#define CoordinateTransformation_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"

class GMAT_API CoordinateTransformation
{
public:
   static Rvector6 TransformState(const A1Mjd &epoch, AxisSystem *oldFrame,
      const Rvector6 &stateWrtOldFrame, AxisSystem *newFrame);

private:
   CoordinateTransformation();
   CoordinateTransformation(const CoordinateTransformation &copy);
   CoordinateTransformation& operator=(const CoordinateTransformation &copy);
   ~CoordinateTransformation();
};
#endif
