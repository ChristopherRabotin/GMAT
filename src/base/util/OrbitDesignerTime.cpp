//
//------------------------------------------------------------------------------
//                             OrbitDesignerTime
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
// Created: 2011/07/25
//
/**
 * Implements the orbit designer time base class.
 */
//------------------------------------------------------------------------------

#include "OrbitDesignerTime.hpp"
#include "math.h"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// OrbitDesignerTime()
//------------------------------------------------------------------------------
// A constructor.
//------------------------------------------------------------------------------
OrbitDesignerTime::OrbitDesignerTime(std::string epochFormat, std::string epoch,
	                                 double RAAN, std::string startTime)
{
}

OrbitDesignerTime::OrbitDesignerTime(std::string epochStr, 
	                                 std::string epochFormatStr, bool raanVal, 
									 Real raan, bool startTimeVal, 
									 std::string startTimeStr)
{
   epoch = epochStr;
   epochFormat = epochFormatStr;
   if (raanVal)	
      RAAN = raan;
   else 
	  RAAN = 306.6148021947984100;
   if (startTimeVal) 
	  startTime = startTimeStr;
   else 
	  startTime = "12:00:00.0";
}

//------------------------------------------------------------------------------
// ~OrbitDesignerTime()
//------------------------------------------------------------------------------
OrbitDesignerTime::~OrbitDesignerTime()
{
}

//------------------------------------------------------------------------------
// findRAAN()
//------------------------------------------------------------------------------
Real OrbitDesignerTime::FindRAAN()
{
   Real c1 = 6.697374558;
   Real c2 = 0.06570982441908;
   Real c3 = 1.00273790935;
   Real c4 = 0.000026;
   Real mon, day, year, hour, min, sec, epochJD, epochJDN, JD, JDN, D, argJD0;
   Real JD0, H, D0, T, GMST, argGMST, a, y, m;
   std::string months[12] = 
      {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
   errormsg = "";
   isError = false;

   if ((epochFormat == "TAIModJulian") || (epochFormat == "UTCModJulian") || 
	   (epochFormat == "A1ModJulian") || (epochFormat == "TTModJulian"))
   {
	  GmatStringUtil::ToReal(epoch,epochJD);
	  epochJDN = floor(epochJD);
   }
   else if ((epochFormat == "UTCGregorian") || (epochFormat == "A1Gregorian") ||
	   (epochFormat == "TAIGregorian") || (epochFormat == "TTGregorian"))
   {
	  ////////////////////////////////////////////////////
	  //find epoch in decimal form
	  ////////////////////////////////////////////////////
	  //epoch Format = "DD Mon YYYY HH:MM:SS.ss"
	  //convert date to day, mon, year doubles
	  std::string epochTemp = epoch;
	  GmatStringUtil::ToReal(epochTemp.substr(0,2),day);
	  epochTemp.erase(0,3);
	  std::string monthStr = epochTemp.substr(0,3);
	  for (int i=0; i<12; i++)
	  {
	     if (monthStr == months[i])
	     {
		    mon = i+1;
			break;
	     }
	  }
	  epochTemp.erase(0,4);
	  GmatStringUtil::ToReal(epochTemp.substr(0,4),year);
	  epochTemp.erase(0,5);
	  //convert time to hour, min, sec doubles
	  GmatStringUtil::ToReal(epochTemp.substr(0,2),hour);
	  epochTemp.erase(0,3);
	  GmatStringUtil::ToReal(epochTemp.substr(0,2),min);
	  epochTemp.erase(0,3);
	  GmatStringUtil::ToReal(epochTemp,sec);
	  a = floor((14-mon)/12);
	  y = year+4800-a;
	  m = mon+(12*a)-3;
	  epochJDN = 
	     day + floor((153*m + 2)/5) + 365*y + floor(y/4) - floor(y/100) + 
		 floor(y/400) - 32045;
      epochJD = epochJDN + (hour-12)/24 + min/1440 + sec/86400;
   }
   /////////////////////////////////////////////////////
   //convert start time into decimal form
   /////////////////////////////////////////////////////
   //startTimeFormat = "HH:MM:SS.ss"
   std::string startTimeTemp = startTime;
   if (GmatStringUtil::IsNumber(startTimeTemp.substr(0,2)))
	  GmatStringUtil::ToReal(startTimeTemp.substr(0,2),hour);
   else
      isError = true;
   if (startTimeTemp.substr(2,1) != ":")
	  isError = true;
   startTimeTemp.erase(0,3);
   if (GmatStringUtil::IsNumber(startTimeTemp.substr(0,2)))
	  GmatStringUtil::ToReal(startTimeTemp.substr(0,2),min);
   else
      isError = true;
   if (startTimeTemp.substr(2,1) != ":")
	   isError = true;
   startTimeTemp.erase(0,3);
   if (GmatStringUtil::IsNumber(startTimeTemp.substr(0,2)))
	  GmatStringUtil::ToReal(startTimeTemp.substr(0,2),sec);
   else
      isError = true;
   if (isError)
   {
	  errormsg = 
	     "The value of \"" + startTime + 
		 "\" for field \"Initial Local Sidereal Time\" is not an allowed value.\n"
		 + "The allowed values are: [HH:MM:SS.sss]\n";
	  return -1;
   }
   JDN = epochJDN;
   JD = JDN + (hour-12)/24 + min/1440 + sec/86400;
   D = JD - epochJD;

   ///////////////////////////////////////////////////////////////////////
   //calculate RAAN from the given epoch and time
   ///////////////////////////////////////////////////////////////////////
   argJD0 = JD - floor(JD);
   if (argJD0 < .5)
      JD0 = JD - (argJD0 + .5);
   else if (argJD0 >.5)
      JD0 = JD - argJD0;
   else
      JD0 = JD;
   H = 24*(JD - JD0);
   D0 = JD0 - epochJD;
   T = D/36525;
   argGMST = c1 + c2*D0 + c3*H +c4*pow(T,2);
   if (argGMST >= 24)
      GMST = argGMST- 24*floor(argGMST/24);
   else 
      GMST = argGMST;

   RAAN = (GMST/24)*360;
  
   return RAAN;
}

//------------------------------------------------------------------------------
// FindStartTime()
//------------------------------------------------------------------------------
std::string OrbitDesignerTime::FindStartTime(bool flag, Real lon)
{
   Real GMST, epochJDN, epochJD, JD, JDN, timeArg, mon, day, year, hour;
   Real min, sec, diff = 1, a, y, m, JD0;
   Real tol = .0000001;
   int count = 0;
   Real c1 = 6.697374558;
   Real c2 = 0.06570982441908;
   Real c3 = 1.00273790935;
   Real c4 = 0.000026;
   errormsg = "";
   isError = false;

   std::string months[12] = 
      {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
	   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

   if (flag)
   {
      if (lon<-180)
	  {
	     errormsg = 
	        "Longitude out of range, please choose longitude greater than or equal to -180";
	     isError = true;
	     return "-1";
	  }
	  else if (lon>180)
	  {
	     errormsg = 
		    "Longitude out of range, please choose longitude less than or equal to 180";
	     isError = true;
	     return "-1";
	  }
   }
   else
   {
	  if (RAAN<0)
	  {
	     errormsg = 
		    "RAAN out of range, please choose RAAN than or equal to 0";
	     isError = true;
	     return "-1";
	  }
	  else if (RAAN>=360)
	  {
	     errormsg = 
		    "RAAN out of range, please choose RAAN less than 360";
	     isError = true;
	     return "-1";
	  }
   }

   if (flag)
      if (lon<0)
	     GMST = ((360+lon)/360)*24;
	  else
         GMST = (lon/360)*24;
   else
      GMST = (RAAN/360)*24;

   if ((epochFormat == "TAIModJulian") || (epochFormat == "UTCModJulian") ||
	   (epochFormat == "A1ModJulian") || (epochFormat == "TTModJulian"))
   {
	  GmatStringUtil::ToReal(epoch,epochJD);
	  epochJDN = floor(epochJD);
   }
   else if ((epochFormat == "UTCGregorian") || (epochFormat == "A1Gregorian") ||
	        (epochFormat == "TAIGregorian") || (epochFormat == "TTGregorian"))
   {
	  ////////////////////////////////////////////////////
	  //find epoch in decimal form
	  ////////////////////////////////////////////////////
      //epoch Format = "DD Mon YYYY HH:MM:SS.ss"
      //convert date to day, mon, year doubles
	  std::string epochTemp = epoch;
	  GmatStringUtil::ToReal(epochTemp.substr(0,2),day);
	  epochTemp.erase(0,3);
	  std::string monthStr = epochTemp.substr(0,3);
	  for (int i=0; i<12; i++)
	  {
	     if (monthStr == months[i])
		 {
		    mon = i+1;
			break;
		 }
	  }
	  epochTemp.erase(0,4);
	  GmatStringUtil::ToReal(epochTemp.substr(0,4),year);
	  epochTemp.erase(0,5);
	  //convert time to hour, min, sec doubles
	  GmatStringUtil::ToReal(epochTemp.substr(0,2),hour);
	  epochTemp.erase(0,3);
	  GmatStringUtil::ToReal(epochTemp.substr(0,2),min);
	  epochTemp.erase(0,3);
	  GmatStringUtil::ToReal(epochTemp,sec);
	  a = floor((14-mon)/12);
	  y = year+4800-a;
	  m = mon+(12*a)-3;
	  epochJDN = 
	     day + floor((153*m + 2)/5) + 365*y + floor(y/4) - floor(y/100) + 
		 floor(y/400) - 32045;
      epochJD = epochJDN + (hour-12)/24 + min/1440 + sec/86400;
   }

   JDN = epochJDN;
   JD0 = JDN-.5;
   
   JD = epochJD;
   diff = 
      c1 + c2*(JD0-epochJD) + 24*c3*(JD-JD0) + c4*pow((JD-epochJD)/36525,2)
	  - GMST;

   while ((abs(diff)>tol) && (count<10000))
   {
      if (diff>tol)
         JD = JD - diff*.01;
      else if (diff<tol)
         JD = JD + abs(diff*.01);
	  diff = 
	     c1 + c2*(JD0-epochJD) + 24*c3*(JD-JD0) + 
		 c4*pow((JD-epochJD)/36525,2.0) - GMST;
	  count++;
   }

   startTime = "";
   timeArg = JD-JDN;
   hour = floor(timeArg*24+12);
   if ((hour<10)&&(hour>=0))
      startTime += "0";
   std::string hourStr = GmatStringUtil::ToString(hour);
   hourStr.resize(2);
   startTime += hourStr;
   startTime += ":";
   min = floor((timeArg-(hour-12)/24)*1440);
   if (min<10)
      startTime += "0";
   std::string minStr = GmatStringUtil::ToString(min);
   minStr.resize(2);
   startTime += minStr;
   startTime += ":";
   sec = (timeArg - (hour-12)/24 - min/1440)*86400;
   std::string secStr = GmatStringUtil::ToString(sec);
   if (sec <10)
   {
      startTime += "0";
      secStr.resize(5);
   }
   else
   {
      secStr.resize(6);
   }
   startTime += secStr;

   return startTime;
}

void OrbitDesignerTime::SetRAAN(Real val)
{
   RAAN = val;
}

void OrbitDesignerTime::SetEpoch(std::string val)
{
   epoch = val;
}

void OrbitDesignerTime::SetStart(std::string val)
{
   startTime = val;
}

//////////////////////////////////////////////////////////////////////////////
//accessor functions
//////////////////////////////////////////////////////////////////////////////
double OrbitDesignerTime::GetRAAN()
{
   return RAAN;
}

std::string OrbitDesignerTime::GetStartTime()
{
   return startTime;
}

std::string OrbitDesignerTime::GetEpoch()
{
   return epoch;
}

std::string OrbitDesignerTime::GetEpochFormat()
{
   return epochFormat;
}

bool OrbitDesignerTime::IsError()
{
   return isError;
}

std::string OrbitDesignerTime::GetError()
{
   return errormsg;
}