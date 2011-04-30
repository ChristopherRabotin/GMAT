//$Id$
//------------------------------------------------------------------------------
//                                    DateUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 1995/10/18
//
/**
 * This class provides conversions among various ways of representing calendar
 * dates and times.
 */
//------------------------------------------------------------------------------
#ifndef DateUtil_hpp
#define DateUtil_hpp

#include "TimeTypes.hpp" // for ElapsedDays, YearNumber, MonthOfYear, DayOfMonth

class GMAT_API DateUtil
{
public:
   static Integer JulianDay(YearNumber year, MonthOfYear month,
                            DayOfMonth day);
   
   static std::string FormatGregorian(YearNumber year, MonthOfYear month,
                                      DayOfMonth day, Integer hour, Integer minute,
                                      Real second);
   
   friend Real GMAT_API JulianDate(YearNumber year, MonthOfYear month,
                          DayOfMonth day, Integer hour, Integer minute,
                          Real second);
   
   friend Real GMAT_API ModifiedJulianDate(YearNumber year, MonthOfYear month,
                                  DayOfMonth day, Integer hour, Integer minute,
                                  Real second);

   friend Real GMAT_API ModifiedJulianDate(YearNumber year, MonthOfYear month,
                                  DayOfMonth day, Integer hour, Integer minute,
                                  Real second, Real refEpochJD);
   
   friend void GMAT_API UnpackDate(Real packedDate, Integer& year, Integer& month, 
                          Integer& day);        // input in YYYYMMDD. 
   friend void GMAT_API UnpackDateWithDOY(Real packedDate, Integer& year, Integer& day);
   
   // input in YYYYDDD. 
   friend void GMAT_API UnpackTime(Real packedTime, Integer& hour, Integer& minute, 
                          Real& second);        // input in hhmmssnnn.
   friend void GMAT_API ToMonthDayFromYearDOY(Integer year, Integer dayOfYear,
                                     Integer& month, Integer& day);
   friend Integer GMAT_API ToDOYFromYearMonthDay(Integer year, Integer month,
                                        Integer day);
   friend Real GMAT_API ToSecondsOfDayFromHMS(Integer hour, Integer minute,
                                     Real second);
   friend void GMAT_API ToHMSFromSecondsOfDay(Real secsOfDay, Integer& hour, 
                                     Integer& minute, Real& second);
   friend bool GMAT_API IsValidTime(Integer year, Integer month, Integer day, 
                           Integer hour, Integer minute, Real second);
   static bool IsValidGregorian(const std::string &str, bool checkDate = false);
   friend bool GMAT_API IsLeapYear(Integer year);
  
};

#endif
