//$Id$
//------------------------------------------------------------------------------
//                                    GmatTime
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Tuan Dang Nguyen
// Created: 2014/07/15
/**
 * This class is used to define GMAT time with a high precision. It has 2 parts:
 * The first part stores number of seconds. The second part stores fraction of 
 * seconds.
 */
//------------------------------------------------------------------------------
#ifndef GmatTime_hpp
#define GmatTime_hpp

#include "gmatdefs.hpp"

class GMAT_API GmatTime
{
public:
   GmatTime();
   virtual ~GmatTime();
   GmatTime(const GmatTime& gt);
   GmatTime(const GmatEpoch mjd);

   const GmatTime& operator=(const GmatTime& gt);
   GmatTime operator+(const GmatTime& gt) const;
   GmatTime operator-(const GmatTime& gt) const;
   const GmatTime& operator+=(const GmatTime& gt);
   const GmatTime& operator-=(const GmatTime& gt);
   const GmatTime& operator=(const GmatEpoch mjd);
   GmatTime operator+(const GmatEpoch mjd) const;
   GmatTime operator-(const GmatEpoch mjd) const;
   const GmatTime& operator+=(const GmatEpoch mjd);
   const GmatTime& operator-=(const GmatEpoch mjd);

   virtual GmatTime* Clone();

   virtual void SetTimeInSec(const Real sec);
   const GmatEpoch GetMjd();
   const Real GetTimeInSec();
   const Real GetSec();
   const Real GetFracSec();

protected:
   Real Sec;		// time in seconds
   Real FracSec;	// time in fraction of seconds

};

#endif //GmatTime_hpp
