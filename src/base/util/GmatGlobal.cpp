//$Id$
//------------------------------------------------------------------------------
//                                 GmatGlobal
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/01/12
//
/**
 * Implements global data used by GMAT.
 */
//------------------------------------------------------------------------------

#include "GmatGlobal.hpp"

//---------------------------------
// static members
//---------------------------------

GmatGlobal* GmatGlobal::theGmatGlobal = NULL;

const Integer GmatGlobal::DATA_PRECISION = 16;
const Integer GmatGlobal::TIME_PRECISION = 16;
const Integer GmatGlobal::DATA_WIDTH = 16;
const Integer GmatGlobal::TIME_WIDTH = 16;
const Integer GmatGlobal::INTEGER_WIDTH = 4;

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// GmatGlobal* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain the singleton.
 *
 * @return the singleton instance of the GmatGlobal class.
 */
//------------------------------------------------------------------------------
GmatGlobal* GmatGlobal::Instance()
{
   if (!theGmatGlobal)
      theGmatGlobal = new GmatGlobal;
        
   return theGmatGlobal;
}


//------------------------------------------------------------------------------
// void SetDefaultFormat()
//------------------------------------------------------------------------------
void GmatGlobal::SetDefaultFormat()
{
   defaultFormat.mScientific = false;
   defaultFormat.mShowPoint = false;
   defaultFormat.mWidth = DATA_WIDTH;
   defaultFormat.mPrecision = DATA_PRECISION;
   defaultFormat.mHorizontal = true;
   defaultFormat.mSpacing = 1;
   defaultFormat.mPrefix = "";
   defaultFormat.mAppendEol = true;
   defaultFormat.mBinaryIn = false;
   defaultFormat.mBinaryOut = false;
}


//------------------------------------------------------------------------------
// void SetCurrentFormat(bool scientific, bool showPoint, Integer width,
//                       Integer precision, bool horizontal, Integer spacing,
//                       const std::string &prefix, bool appendEol,
//                       bool binaryIn, bool binaryOut)
//------------------------------------------------------------------------------
/*
 * Sets current Real data input/output formatting.
 *
 * @param  scientific  Formats using scientific notation if true (false)
 * @param  showPoint  Formats using ios::showpoint if true (false)
 * @param  width  Width to be used in formatting (GmatGlobal::DATA_WIDTH)
 * @param  precision  Precision to be used in formatting (GmatGlobal::DATA_PRECISION)
 * @param  horizontal  Format horizontally if true (false)
 * @param  spacing  Spacing to be used in formatting (1)
 * @param  appendEol  Appends eol if true (true)
 * @param  binaryIn  Reads binary data if true (false)
 * @param  binaryOut  Writes binary data if true (false) 
 */
//------------------------------------------------------------------------------
void GmatGlobal::SetCurrentFormat(bool scientific, bool showPoint, Integer width,
                                  Integer precision, bool horizontal, Integer spacing,
                                  const std::string &prefix, bool appendEol,
                                  bool binaryIn, bool binaryOut)
{
   currentFormat.mScientific = scientific;
   currentFormat.mShowPoint = scientific;
   currentFormat.mWidth = width;
   currentFormat.mPrecision = precision;
   currentFormat.mHorizontal = horizontal;
   currentFormat.mSpacing = spacing;
   currentFormat.mPrefix = prefix;   
   currentFormat.mAppendEol = appendEol;   
   currentFormat.mBinaryIn = binaryIn;
   currentFormat.mBinaryOut = binaryOut;
}


//------------------------------------------------------------------------------
// void GetActualFormat(bool &scientific, bool &showPoint, Integer &precision,
//                      Integer &width, bool &horizontal, Integer &spacing,
//                      std::string &prefix, bool &appendEol)
//------------------------------------------------------------------------------
void GmatGlobal::GetActualFormat(bool &scientific, bool &showPoint,
                                 Integer &precision, Integer &width,
                                 bool &horizontal, Integer &spacing,
                                 std::string &prefix, bool &appendEol)
{
   scientific = actualFormat.mScientific;
   showPoint = actualFormat.mShowPoint;
   precision = actualFormat.mPrecision;
   width = actualFormat.mWidth;
   horizontal = actualFormat.mHorizontal;
   spacing = actualFormat.mSpacing;
   prefix = actualFormat.mPrefix;
   appendEol = actualFormat.mAppendEol;
}


//------------------------------------------------------------------------------
// void SetActualFormat(bool scientific, bool showPoint, Integer precision,
//                      Integer width, bool horizontal = false, Integer spacing = 1,
//                      const std::string &prefix = "", bool appendEol = true)
//------------------------------------------------------------------------------
void GmatGlobal::SetActualFormat(bool scientific, bool showPoint, Integer precision,
                                 Integer width, bool horizontal, Integer spacing,
                                 const std::string &prefix, bool appendEol)
{
   actualFormat.mScientific = scientific;
   actualFormat.mShowPoint = showPoint;
   actualFormat.mWidth = width;
   actualFormat.mPrecision = precision;
   actualFormat.mHorizontal = horizontal;
   actualFormat.mSpacing = spacing;
   actualFormat.mPrefix = prefix;
   actualFormat.mAppendEol = appendEol;
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// GmatGlobal()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
GmatGlobal::GmatGlobal()
{
   isBatchMdoe = false;
   matlabExt = "__m__";
}


//------------------------------------------------------------------------------
// ~GmatGlobal()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GmatGlobal::~GmatGlobal()
{
}

