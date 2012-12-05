//$Id$
//------------------------------------------------------------------------------
//                           SunSync      
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
// Created: 2011/07/20
//
/**
 * Definition of the Sun Sync Orbit Class
 */
//------------------------------------------------------------------------------

#ifndef SunSync_hpp
#define SunSync_hpp

#include "StringUtil.hpp"

class GMAT_API SunSync
{
public:
   SunSync();
   SunSync(Real a, Real alt, Real e, Real i, Real rop, Real roa, Real p);
   ~SunSync();

   void CalculateSunSync(bool aVal, Real a, bool altVal, Real alt, bool eVal, 
                         Real e, bool iVal, Real i, bool ropVal, Real RoP, 
                         bool roaVal, Real RoA, bool pVal, Real P);
   
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

   Real SolveA(Real e, Real i, bool flag);
   Real SolveE(Real a, Real i, bool flag, bool altFlag = false);
   Real SolveI(Real a, Real e, bool flag, bool roaFlag = false, bool ropFlag = false,
               bool eFlag = true, bool aFlag = true, bool pFlag = false, 
               bool altFlag = false);
   void SolvePAE(Real P, Real i, Real &a, Real &e);
   void SolveRoAAE(Real RoA, Real i, Real &a, Real &e);
   void SolveRoPAE(Real RoP, Real i, Real &a, Real &e);

private:
   struct orbitElements 
   {
      Real SMA;
      Real ALT;
      Real ECC;
      Real INC;
      Real ROP;
      Real ROA;
      Real P;
   };

   orbitElements elements;
   std::string errormsg;
   bool isError;
};

#endif
