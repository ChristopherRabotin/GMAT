//$Id$
//------------------------------------------------------------------------------
//                           Frozen      
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
// Created: 2011/07/29
//
/**
 * Definition of the Frozen Orbit Class
 */
//------------------------------------------------------------------------------

#ifndef Frozen_hpp
#define Frozen_hpp

#include "StringUtil.hpp"

class GMAT_API Frozen
{
public:
   Frozen();
   ~Frozen();

   void CalculateFrozen(Real ALT, bool altVal, Real INC, bool eccVal);

   //accesor functions
   Real GetSMA();
   Real GetALT();
   Real GetECC();
   Real GetINC();
   Real GetPALT();
   Real GetAALT();
   bool IsError();
   std::string GetError();

private:
   struct orbitElements 
   {
      Real SMA;
      Real ALT;
      Real ECC;
      Real INC;
      Real AALT;
      Real PALT;
   };

   orbitElements elements;
   std::string errormsg;
   bool isError;
};

#endif
