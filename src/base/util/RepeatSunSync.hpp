//$Id$
//------------------------------------------------------------------------------
//                           RepeatSunSync      
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
