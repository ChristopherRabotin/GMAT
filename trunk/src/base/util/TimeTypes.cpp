//$Id$
//------------------------------------------------------------------------------
//                                TimeTypes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun, NASA/GSFC
// Created: 2006/05/05
//
/**
 * Provides declarations for date & time types.
 */
//------------------------------------------------------------------------------

#include "TimeTypes.hpp"
#include "StringUtil.hpp"
#include "UtilityException.hpp"
#include <ctime>                   // for time()


//------------------------------------------------------------------------------
// bool IsValidMonthName(const std::string &str)
//------------------------------------------------------------------------------
/*
 * Checks if input string is valid month short name. Input month name should
 *   be month name abbreviation.  For example, "Jan", "Feb", etc.
 *
 * @param  str  input month string
 *
 * @return return true if input string is valid month name
 */
//------------------------------------------------------------------------------ 
bool GmatTimeUtil::IsValidMonthName(const std::string &str)
{
   for (int i=0; i<12; i++)
   {
      if (str == MONTH_NAME_TEXT[i])
         return true;
   }
   
   return false;
}


//------------------------------------------------------------------------------
// std::string GetMonthName(Integer month)
//------------------------------------------------------------------------------
/*
 * Return corresponding Month Name for integer month.
 *
 * @param  month  input month
 *
 * @return corresponding Month Name for integer month.
 * 
 * @exception <TimeException> thrown if input month is out of range
 */
//------------------------------------------------------------------------------
std::string GmatTimeUtil::GetMonthName(Integer month)
{
   if (month < 1 || month > 12) 
   {
      throw TimeException
         ("Cannot get Month Name for " + GmatStringUtil::ToString(month));
   }
   
   return GmatTimeUtil::MONTH_NAME_TEXT[month-1];
}


//------------------------------------------------------------------------------
// Integer GetMonth(const std::string &monthName)
//------------------------------------------------------------------------------
/*
 * Return corresponding month for input Month Name. Input month name should
 *   be month name abbreviation.  For example, "Jan", "Feb", etc.
 *
 * @param  monthName  input month name
 *
 * @return corresponding Integer month for Month Name.
 *         -1, if name not found
 * 
 * @exception <TimeException> thrown if input month name is out of range
 */
//------------------------------------------------------------------------------
Integer GmatTimeUtil::GetMonth(const std::string &monthName)
{
   for (int i=0; i<12; i++)
   {
      if (monthName == GmatTimeUtil::MONTH_NAME_TEXT[i])
         return i + 1;
   }
   
   return -1;
}


//------------------------------------------------------------------------------
// std::string GetCurrentTime()
//------------------------------------------------------------------------------
/*
 * Returns the current time in "Wed Apr 16 12:30:22 2008" format.
 */
//------------------------------------------------------------------------------
std::string GmatTimeUtil::GetCurrentTime()
{
   time_t currTime = time(NULL);
   char *currTimeStr = ctime(&currTime);
   return currTimeStr;
}

