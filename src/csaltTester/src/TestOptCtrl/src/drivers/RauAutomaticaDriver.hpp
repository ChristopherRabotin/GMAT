//------------------------------------------------------------------------------
//                           RauAutomaticaDriver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef SRC_RAUAUTOMATICADRIVER_HPP_
#define SRC_RAUAUTOMATICADRIVER_HPP_

#include "CsaltTestDriver.hpp"

/**
 * Driver for the RauAutomatica problem
 */
class RauAutomaticaDriver : public CsaltTestDriver
{
public:
   RauAutomaticaDriver();
   virtual ~RauAutomaticaDriver();
protected:
   virtual void SetPointPathAndProperties();
   virtual void SetupPhases();
};

#endif /* SRC_RAUAUTOMATICADRIVER_HPP_ */
