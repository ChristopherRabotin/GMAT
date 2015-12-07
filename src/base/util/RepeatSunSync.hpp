//$Id$
//------------------------------------------------------------------------------
//                           RepeatSunSync      
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Evelyn Hull
// Created: 2011/07/28
//
/**
 * Definition of the Repeat Sun Sync Orbit Class
 */
//------------------------------------------------------------------------------

#ifndef RepeatSunSync_hpp
#define RepeatSunSync_hpp

#include "StringUtil.hpp"

class GMAT_API RepeatSunSync
{
public:
   RepeatSunSync();
   ~RepeatSunSync();

   void CalculateRepeatSunSync(bool eccVal, Real ECC, bool dtrVal, 
                               Real daysToRepeat, bool rtrVal, Real revsToRepeat,
                               bool rpdVal, Real revsPerDay);

   //accessor methods
   Real GetSMA();
   Real GetALT();
   Real GetECC();
   Real GetINC();
   Real GetROP();
   Real GetROA();
   Real GetP();
   bool IsError();
   std::string GetError();

private:
   struct orbitElements 
   {
      Real SMA;
      Real ECC;
      Real INC;
      Real daysToRepeat;
      Real revsToRepeat;
      Real revsPerDay;
   };

   orbitElements elements;

   std::string errormsg;
   bool isError;
};

#endif
