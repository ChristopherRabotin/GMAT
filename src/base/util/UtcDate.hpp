//$Header$
//------------------------------------------------------------------------------
//                                UtcDate
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 1995/10/18 for GSS project
// Modified: 2003/09/12 Linda Jun - Added member data: descs, stringValues
//
/**
 * Provides conversions among various ways representing UTC calendar
 * dates and times.
 */
//------------------------------------------------------------------------------
#ifndef UtcDate_hpp
#define UtcDate_hpp

#include "gmatdefs.hpp"
#include "TimeTypes.hpp"
#include "Date.hpp"
//#include "A1Date.hpp"

//class A1Date;

class UtcDate : public Date
{
public:
    UtcDate();
    UtcDate(Integer year, Integer month, Integer day, Integer hour, 
            Integer minute, Real second);
    UtcDate(Integer year, Integer doy, Integer hour, Integer minute,
            Real second);
    UtcDate(Integer year, Integer month, Integer day, Real secondsOfDay);
    UtcDate(const GmatTimeUtil::CalDate &date);
    UtcDate(const std::string& time);            // "YYYYMMDD.HHMMSSmmm"
    UtcDate(const UtcDate &date);
    UtcDate operator= (const UtcDate &date);
    ~UtcDate();

//loj: Do this later
//      Real     operator- (const UtcDate &date) const;
//      UtcDate  operator+ (const Real seconds) const;
//      UtcDate& operator+=(const Real seconds);
//      UtcDate  operator- (const Real seconds) const;
//      UtcDate& operator-=(const Real seconds);
//      A1Date ToA1Date();

protected:
private:
};
#endif // UtcDate_hpp
