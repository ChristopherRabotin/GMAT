//$Id$
//------------------------------------------------------------------------------
//                           Brent
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Sep 20, 2011
//
/**
 * Definition of the Brent's method root finder
 */
//------------------------------------------------------------------------------


#ifndef BRENT_HPP_
#define BRENT_HPP_

#include "RootFinder.hpp"

class Brent: public RootFinder
{
public:
   Brent();
   virtual ~Brent();
   Brent(const Brent& b);
   Brent& operator=(const Brent& b);

   virtual bool Initialize(GmatEpoch t0, Real f0, GmatEpoch t1, Real f1);
   virtual bool SetValue(GmatEpoch forEpoch, Real withValue);

protected:
   /// Flag used to tell which method was used most recently
   bool bisectionUsed;
   /// Epoch to be achieved at the step to be taken
   GmatEpoch epochOfStep;
   /// Step to be taken
   Real step;
   /// Value used in the step selection logic
   GmatEpoch oldCValue;


//   virtual bool FindRoot(Integer whichOne, GmatEpoch roots[2]);
   virtual Real FindStep(const GmatEpoch currentEpoch);
};

#endif /* BRENT_HPP_ */
