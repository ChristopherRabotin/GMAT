//$Header$
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
   defaultFormat.mBinaryIn = false;
   defaultFormat.mBinaryOut = false;
}


//------------------------------------------------------------------------------
// void SetCurrentFormat(bool scientific = false, bool showPoint = false,
//                       Integer width = GmatGlobal::DATA_WIDTH,
//                       Integer precision = GmatGlobal::DATA_PRECISION,
//                       bool horizontal = false, Integer spacing = 1,
//                       const std::string &prefix = "", 
//                       bool binaryIn = false, bool binaryOut = false)
//------------------------------------------------------------------------------
void GmatGlobal::SetCurrentFormat(bool scientific, bool showPoint,
                                  Integer width, Integer precision,
                                  bool horizontal, Integer spacing,
                                  const std::string &prefix,
                                  bool binaryIn, bool binaryOut)
{
   currentFormat.mScientific = scientific;
   currentFormat.mShowPoint = scientific;
   currentFormat.mWidth = width;
   currentFormat.mPrecision = precision;
   currentFormat.mHorizontal = horizontal;
   currentFormat.mSpacing = spacing;
   currentFormat.mPrefix = prefix;   
   currentFormat.mBinaryIn = binaryIn;
   currentFormat.mBinaryOut = binaryOut;
}


//------------------------------------------------------------------------------
// void GetActualFormat(bool &scientific, bool &showPoint, Integer &precision,
//                      Integer &width, bool &horizontal, Integer &spacing,
//                      std::string &prefix)
//------------------------------------------------------------------------------
void GmatGlobal::GetActualFormat(bool &scientific, bool &showPoint,
                                 Integer &precision, Integer &width,
                                 bool &horizontal, Integer &spacing,
                                 std::string &prefix)
{
   scientific = actualFormat.mScientific;
   showPoint = actualFormat.mShowPoint;
   precision = actualFormat.mPrecision;
   width = actualFormat.mWidth;
   horizontal = actualFormat.mHorizontal;
   spacing = actualFormat.mSpacing;
   prefix = actualFormat.mPrefix;
}


//------------------------------------------------------------------------------
// void SetActualFormat(bool scientific, bool showPoint,
//                      Integer precision, Integer width,
//                      bool horizontal = false, Integer spacing = 1,
//                      const std::string &prefix = "")
//------------------------------------------------------------------------------
void GmatGlobal::SetActualFormat(bool scientific, bool showPoint, Integer precision,
                                 Integer width, bool horizontal, Integer spacing,
                                 const std::string &prefix)
{
   actualFormat.mScientific = scientific;
   actualFormat.mShowPoint = showPoint;
   actualFormat.mWidth = width;
   actualFormat.mPrecision = precision;
   actualFormat.mHorizontal = horizontal;
   actualFormat.mSpacing = spacing;
   actualFormat.mPrefix = prefix;
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

