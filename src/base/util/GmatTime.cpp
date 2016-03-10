//$Id$
//------------------------------------------------------------------------------
//                                  GmatTime
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Author: Tuan Dang Nguyen
// Created: 2014/07/15   GSFC-NASA
//
/**
 * This class is used to define GMAT time with a high precision. It has 2 parts:
 * The first part stores number of seconds. The second part stores fraction of 
 * seconds.
 */
//------------------------------------------------------------------------------
#include "GmatTime.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp" 

//---------------------------------
//  public
//---------------------------------

//------------------------------------------------------------------------------
//  GmatTime()										OK
//------------------------------------------------------------------------------
GmatTime::GmatTime():
   Sec			(21545.0*GmatTimeConstants::SECS_PER_DAY),
   FracSec		(0.0)
{
}


//------------------------------------------------------------------------------
//  ~GmatTime()
//------------------------------------------------------------------------------
GmatTime::~GmatTime()
{
}


//------------------------------------------------------------------------------
//  GmatTime(const GmatTime& gt)					OK
//------------------------------------------------------------------------------
GmatTime::GmatTime(const GmatTime& gt):
   Sec			(gt.Sec),
   FracSec		(gt.FracSec)
{
}


//------------------------------------------------------------------------------
//  GmatTime(const GmatEpoch mjd)					OK
//------------------------------------------------------------------------------
GmatTime::GmatTime(const GmatEpoch mjd)
{
   Real ts = mjd*GmatTimeConstants::SECS_PER_DAY;
   Sec = GmatMathUtil::Floor(ts);
   FracSec = ts - Sec;
}


//------------------------------------------------------------------------------
//  GmatTime& operator=(const GmatTime& gt)			OK
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator=(const GmatTime& gt)
{
   if (this != &gt)
   {
	   Sec		= gt.Sec;
	   FracSec	= gt.FracSec;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatTime& operator=(const Real mjd)			OK
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator=(const Real mjd)
{
   Real ts = mjd*GmatTimeConstants::SECS_PER_DAY;
   Sec = GmatMathUtil::Floor(ts);
   FracSec = ts - Sec;

   return *this;
}


//------------------------------------------------------------------------------
//  GmatTime operator+(const GmatTime& gt) const
//------------------------------------------------------------------------------
GmatTime GmatTime::operator+(const GmatTime& gt) const
{
   GmatTime gt1(*this);

   gt1.FracSec = gt1.FracSec + gt.FracSec;
   if (FracSec >= 1.0)
   {
	  gt1.FracSec = gt1.FracSec - 1.0;
      gt1.Sec = gt1.Sec + gt.Sec + 1.0;
   }
   else
	  gt1.Sec = gt1.Sec + gt.Sec;

   return gt1;
}


//------------------------------------------------------------------------------
// GmatTime operator+(const Real mjd) const
//------------------------------------------------------------------------------
GmatTime GmatTime::operator+(const Real mjd) const
{
   GmatTime gt(mjd);
   GmatTime gt1((*this) + gt);

   return gt1;
}


//------------------------------------------------------------------------------
//  const GmatTime& operator+=(const GmatTime& gt)
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator+=(const GmatTime& gt)
{
   (*this) = (*this) + gt;

   return *this;
}


//------------------------------------------------------------------------------
// const GmatTime& ::operator+=(const Real mjd)
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator+=(const Real mjd)
{
   (*this) = ((*this) + mjd);

   return *this;
}


//------------------------------------------------------------------------------
//  GmatTime operator-(const GmatTime& gt) const
//------------------------------------------------------------------------------
GmatTime GmatTime::operator-(const GmatTime& gt) const
{
   GmatTime gt1(*this);

   gt1.FracSec = gt1.FracSec - gt.FracSec;
   if (gt1.FracSec < 0.0)
   {
	  gt1.FracSec = gt1.FracSec + 1.0;
      gt1.Sec = gt1.Sec - gt.Sec - 1.0;
   }
   else
	  gt1.Sec = gt1.Sec - gt.Sec;

   return gt1;
}


//------------------------------------------------------------------------------
// GmatTime operator-(const Real mjd) const
//------------------------------------------------------------------------------
GmatTime GmatTime::operator-(const Real mjd) const
{
   GmatTime gt(mjd);
   GmatTime gt1(*this - gt);

   return gt1;
}


//------------------------------------------------------------------------------
//  const GmatTime& operator-=(const GmatTime& gt)
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator-=(const GmatTime& gt)
{
   (*this) = ((*this) - gt);

   return *this;
}


//------------------------------------------------------------------------------
// const GmatTime& ::operator-=(const Real mjd)
//------------------------------------------------------------------------------
const GmatTime& GmatTime::operator-=(const Real mjd)
{
   (*this) = ((*this) - mjd);

   return *this;
}


GmatTime* GmatTime::Clone()
{
   return new GmatTime(*this); 
}


const GmatEpoch GmatTime::GetMjd()
{
   return (Sec+FracSec)/GmatTimeConstants::SECS_PER_DAY;  
}


const Real GmatTime::GetSec()
{
   return Sec;
}


const Real GmatTime::GetFracSec()
{
   return FracSec;
}


void GmatTime::SetTimeInSec(const Real sec)
{
   Sec = GmatMathUtil::Floor(sec);
   FracSec = sec - Sec;      
}


const Real GmatTime::GetTimeInSec() 
{ 
   return Sec+FracSec;
}

