//$Header: /cygdrive/p/dev/cvs/test/TestSpacecraft/TestFunction.hpp,v 1.2 2005/06/28 11:15:53 jgurgan Exp $
//------------------------------------------------------------------------------
//                              TestFunction
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurganus 
// Created: 2004/04/29
//
/**
 * File Header of Unit test program 
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "Spacecraft.hpp"
#include "TimeConverter.hpp"
#include "UtcDate.hpp"
#include "DateUtil.hpp"
 
// Declare functions 

void printEpoch(const Spacecraft *sc);

void printState(const std::string &title, const Real *state);

void printState(const std::string &title, const Rvector6 state);

void printState(const Spacecraft *s);

void printState(const std::string &title, const Spacecraft *s);
 
void printTime(const Real time);

void printTime(const Real time,const std::string &fromTime,
               const std::string &toTime);

Real julianDate(const Integer yr, const Integer mon, const Integer day, 
                const Integer hour, const Integer min, const Real sec);

std::string jdToGregorian(const Real jd);

std::string mjdToGregorian(const Real mjd);
