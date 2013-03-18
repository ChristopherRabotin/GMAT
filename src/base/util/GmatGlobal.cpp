//$Id$
//------------------------------------------------------------------------------
//                                 GmatGlobal
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
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/01/12
//
/**
 * Implements global data used by GMAT.
 */
//------------------------------------------------------------------------------

#include "GmatGlobal.hpp"
#include <algorithm>                    // Required for GCC 4.3

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
// Integer GetDataPrecision()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetDataPrecision()
{
   return currentSetting.mDataPrecision;
}

//------------------------------------------------------------------------------
// Integer GetTimePrecision()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetTimePrecision()
{
   return currentSetting.mTimePrecision;
}

//------------------------------------------------------------------------------
// Integer GetDataWidth()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetDataWidth()
{
   return currentSetting.mDataWidth;
}


//------------------------------------------------------------------------------
// Integer GetTimeWidth()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetTimeWidth()
{
   return currentSetting.mTimeWidth;
}


//------------------------------------------------------------------------------
// Integer GetIntegerWidth()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetIntegerWidth()
{
   return currentSetting.mIntegerWidth;
}


//------------------------------------------------------------------------------
// std::string GetOutputPath()
//------------------------------------------------------------------------------
std::string GmatGlobal::GetOutputPath()
{
   return currentSetting.mOutputPath;
}

//------------------------------------------------------------------------------
// void SetDataPrecision(Integer p)
//------------------------------------------------------------------------------
void GmatGlobal::SetDataPrecision(Integer p)
{
   currentSetting.mDataPrecision = p;
}

//------------------------------------------------------------------------------
// void SetTimePrecision(Integer p)
//------------------------------------------------------------------------------
void GmatGlobal::SetTimePrecision(Integer p)
{
   currentSetting.mTimePrecision = p;
}

//------------------------------------------------------------------------------
// void SetDataWidth(Integer w)
//------------------------------------------------------------------------------
void GmatGlobal::SetDataWidth(Integer w)
{
   currentSetting.mDataWidth = w;
}

//------------------------------------------------------------------------------
// void SetTimeWidth(Integer w)
//------------------------------------------------------------------------------
void GmatGlobal::SetTimeWidth(Integer w)
{
   currentSetting.mTimeWidth = w;
}

//------------------------------------------------------------------------------
// void SetIntegerWidth(Integer w)
//------------------------------------------------------------------------------
void GmatGlobal::SetIntegerWidth(Integer w)
{
   currentSetting.mIntegerWidth = w;
}

//------------------------------------------------------------------------------
// void SetOutputPath(const std::string &path)
//------------------------------------------------------------------------------
void GmatGlobal::SetOutputPath(const std::string &path)
{
   currentSetting.mOutputPath = path;
}

//------------------------------------------------------------------------------
// void SetMatlabFuncNameExt(const std::string &ext)
//------------------------------------------------------------------------------
void GmatGlobal::SetMatlabFuncNameExt(const std::string &ext)
{
   matlabExt = ext;
}

//------------------------------------------------------------------------------
// std::string GetMatlabFuncNameExt()
//------------------------------------------------------------------------------
std::string GmatGlobal::GetMatlabFuncNameExt()
{
   return matlabExt;
}

//------------------------------------------------------------------------------
// bool IsBatchMode()
//------------------------------------------------------------------------------
bool GmatGlobal::IsBatchMode()
{
   return isBatchMode;
}

//------------------------------------------------------------------------------
// void SetBatchMode(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetBatchMode(bool flag)
{
   isBatchMode = flag;
}

//------------------------------------------------------------------------------
// bool IsNitsClient()
//------------------------------------------------------------------------------
bool GmatGlobal::IsNitsClient()
{
   return isNitsClient;
}

//------------------------------------------------------------------------------
// void SetNitsClient(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetNitsClient(bool flag)
{
   isNitsClient = flag;
}

//------------------------------------------------------------------------------
// bool GetRunInterrupted()
//------------------------------------------------------------------------------
bool GmatGlobal::GetRunInterrupted()
{
   return runInterrupted;
}

//------------------------------------------------------------------------------
// void SetRunInterrupted(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetRunInterrupted(bool flag)
{
   runInterrupted = flag;
}

//------------------------------------------------------------------------------
// Integer GetRunMode()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetRunMode()
{
   return runMode;
}

//------------------------------------------------------------------------------
// void SetRunMode(Integer mode)
//------------------------------------------------------------------------------
void GmatGlobal::SetRunMode(Integer mode)
{
   runMode = mode;
   if (runMode == EXIT_AFTER_RUN)
      isBatchMode = true;
}

//------------------------------------------------------------------------------
// Integer GetGuiMode()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetGuiMode()
{
   return guiMode;
}

//------------------------------------------------------------------------------
// void SetGuiMode(Integer mode)
//------------------------------------------------------------------------------
void GmatGlobal::SetGuiMode(Integer mode)
{
   guiMode = mode;
}

//------------------------------------------------------------------------------
// Integer GetGuiMode()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetPlotMode()
{
   return plotMode;
}

//------------------------------------------------------------------------------
// void SetPlotMode(Integer mode)
//------------------------------------------------------------------------------
void GmatGlobal::SetPlotMode(Integer mode)
{
   plotMode = mode;
}

//------------------------------------------------------------------------------
// Integer GetMatlabMode()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetMatlabMode()
{
   return matlabMode;
}

//------------------------------------------------------------------------------
// bool IsMatlabAvailable()
//------------------------------------------------------------------------------
bool GmatGlobal::IsMatlabAvailable()
{
   return isMatlabAvailable;
}

//------------------------------------------------------------------------------
// void SetMatlabAvailable(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetMatlabAvailable(bool flag)
{
   isMatlabAvailable = flag;
}

//------------------------------------------------------------------------------
// bool IsMatlabDebugOn()
//------------------------------------------------------------------------------
bool GmatGlobal::IsMatlabDebugOn()
{
   return isMatlabDebugOn;
}

//------------------------------------------------------------------------------
// void SetMatlabDebug(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetMatlabDebug(bool flag)
{
   isMatlabDebugOn = flag;
}

//------------------------------------------------------------------------------
// void SetEventLocationAvailable(bool flag)
//------------------------------------------------------------------------------
/**
 * Sets the event locator flag
 *
 * @param flag Flag that is true if there are event locators, false if not
 */
//------------------------------------------------------------------------------
void GmatGlobal::SetEventLocationAvailable(bool flag)
{
   isEventLocationAvailable = flag;
}

//------------------------------------------------------------------------------
// bool IsEventLocationAvailable()
//------------------------------------------------------------------------------
/**
 * Returns the event locator available flag
 *
 * @return The flag
 */
//------------------------------------------------------------------------------
bool GmatGlobal::IsEventLocationAvailable()
{
   return isEventLocationAvailable;
}


//------------------------------------------------------------------------------
// bool IsMissionTreeDebugOn()
//------------------------------------------------------------------------------
bool GmatGlobal::IsMissionTreeDebugOn()
{
   return isMissionTreeDebugOn;
}

//------------------------------------------------------------------------------
// void SetMissionTreeDebug(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetMissionTreeDebug(bool flag)
{
   isMissionTreeDebugOn = flag;
}

//------------------------------------------------------------------------------
// bool IsWritingParameterInfo()
//------------------------------------------------------------------------------
bool GmatGlobal::IsWritingParameterInfo()
{
   return isWritingParameterInfo;
}

//------------------------------------------------------------------------------
// void SetWriteParameterInfo(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetWriteParameterInfo(bool flag)
{
   isWritingParameterInfo = flag;
}

//------------------------------------------------------------------------------
// bool IsWritingGmatKeyword()
//------------------------------------------------------------------------------
bool GmatGlobal::IsWritingGmatKeyword()
{
   return isWritingGmatKeyword;
}

//------------------------------------------------------------------------------
// void SetWriteGmatKeyword(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetWriteGmatKeyword(bool flag)
{
   isWritingGmatKeyword = flag;
}

//------------------------------------------------------------------------------
// bool IsScientific()
//------------------------------------------------------------------------------
bool GmatGlobal::IsScientific()
{
   return actualFormat.mScientific;
}

//------------------------------------------------------------------------------
// bool ShowPoint()
//------------------------------------------------------------------------------
bool GmatGlobal::ShowPoint()
{
   return actualFormat.mShowPoint;
}

//------------------------------------------------------------------------------
// bool IsHorizontal()
//------------------------------------------------------------------------------
bool GmatGlobal::IsHorizontal()
{
   return actualFormat.mHorizontal;
}

//------------------------------------------------------------------------------
// bool IsBinaryIn()
//------------------------------------------------------------------------------
bool GmatGlobal::IsBinaryIn()
{
   return actualFormat.mBinaryIn;
}

//------------------------------------------------------------------------------
// bool IsBinaryOut()
//------------------------------------------------------------------------------
bool GmatGlobal::IsBinaryOut()
{
   return actualFormat.mBinaryOut;
}

//------------------------------------------------------------------------------
// Integer GetSpacing()
//------------------------------------------------------------------------------
Integer GmatGlobal::GetSpacing()
{
   return currentFormat.mSpacing;
}

//------------------------------------------------------------------------------
// void SetScientific(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetScientific(bool flag)
{
   actualFormat.mScientific = flag;
}

//------------------------------------------------------------------------------
// void SetShowPoint(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetShowPoint(bool flag)
{
   actualFormat.mShowPoint = flag;
}

//------------------------------------------------------------------------------
// void SetHorizontal(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetHorizontal(bool flag)
{
   actualFormat.mHorizontal = flag;
}

//------------------------------------------------------------------------------
// void SetBinaryIn(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetBinaryIn(bool flag)
{
   actualFormat.mBinaryIn = flag;
}

//------------------------------------------------------------------------------
// void SetBinaryOut(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetBinaryOut(bool flag)
{
   actualFormat.mBinaryOut = flag;
}

//------------------------------------------------------------------------------
// void SetSpacing(Integer sp)
//------------------------------------------------------------------------------
void GmatGlobal::SetSpacing(Integer sp)
{
   actualFormat.mSpacing = sp;
}

//------------------------------------------------------------------------------
// void SetPrefix(const std::string &prefix)
//------------------------------------------------------------------------------
void GmatGlobal::SetPrefix(const std::string &prefix)
{
   actualFormat.mPrefix = prefix;
}

//------------------------------------------------------------------------------
// void SetAppendEol(bool flag)
//------------------------------------------------------------------------------
void GmatGlobal::SetAppendEol(bool flag)
{
   actualFormat.mAppendEol = flag;
}

//------------------------------------------------------------------------------
// void SetMatlabMode(Integer mode)
//------------------------------------------------------------------------------
void GmatGlobal::SetMatlabMode(Integer mode)
{
   matlabMode = mode;
   if (mode == NO_MATLAB)
      isMatlabAvailable = false;
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

//------------------------------------------------------------------------------
// void SetToDefaultFormat()
//------------------------------------------------------------------------------
void GmatGlobal::SetToDefaultFormat()
{
   actualFormat = defaultFormat;
}

//------------------------------------------------------------------------------
// void SetToCurrentFormat()
//------------------------------------------------------------------------------
void GmatGlobal::SetToCurrentFormat()
{
   actualFormat = currentFormat;
}

//------------------------------------------------------------------------------
// EopFile* GetEopFile()
//------------------------------------------------------------------------------
EopFile* GmatGlobal::GetEopFile()
{
   return theEopFile;
}


//------------------------------------------------------------------------------
// ItrfCoefficientsFile* GetItrfCoefficientsFile()
//------------------------------------------------------------------------------
ItrfCoefficientsFile* GmatGlobal::GetItrfCoefficientsFile()
{
   return theItrfFile;
}


//------------------------------------------------------------------------------
// void SetEopFile(EopFile *eop)
//------------------------------------------------------------------------------
void GmatGlobal::SetEopFile(EopFile *eop)
{
   theEopFile = eop;
}


//------------------------------------------------------------------------------
// void SetItrfCoefficientsFile(ItrfCoefficientsFile *itrf)
//------------------------------------------------------------------------------
void GmatGlobal::SetItrfCoefficientsFile(ItrfCoefficientsFile *itrf)
{
   theItrfFile = itrf;
}


//------------------------------------------------------------------------------
// void  AddHiddenCommand(const std::string &cmd)
//------------------------------------------------------------------------------
/*
 * Add command to hide in menu
 *
 * @param  cmd  command name to be added
 */
//------------------------------------------------------------------------------
void GmatGlobal::AddHiddenCommand(const std::string &cmd)
{
   #ifdef DEBUG_HIDDEN_COMMAND
   MessageInterface::ShowMessage
      ("FileManager::AddHiddenCommand() Adding %s to HiddenCommands\n",
       cmd.c_str());
   #endif

   mHiddenCommands.push_back(cmd);

   #ifdef DEBUG_HIDDEN_COMMAND
   std::list<std::string>::iterator pos;
   pos = mHiddenCommands.begin();
   while (pos != mHiddenCommands.end())
   {
      MessageInterface::ShowMessage
         ("   mHiddenCommands=%s\n",(*pos).c_str());
      ++pos;
   }
   #endif
}


//------------------------------------------------------------------------------
// const StringArray& GetHiddenCommands()
//------------------------------------------------------------------------------
const StringArray& GmatGlobal::GetHiddenCommands()
{
   mHiddenCommandsArray.clear();

   std::list<std::string>::iterator listpos = mHiddenCommands.begin();
   while (listpos != mHiddenCommands.end())
   {
      mHiddenCommandsArray.push_back(*listpos);
      ++listpos;
   }

   return mHiddenCommandsArray;
}


//------------------------------------------------------------------------------
// void ClearHiddenCommands()
//------------------------------------------------------------------------------
void GmatGlobal::ClearHiddenCommands()
{
   mHiddenCommands.clear();
}


//------------------------------------------------------------------------------
// void  IsHiddenCommand(const std::string &cmd)
//------------------------------------------------------------------------------
/*
 * returns true if this command should not be shown in menu
 *
 * @return  true to hide
 */
//------------------------------------------------------------------------------
bool GmatGlobal::IsHiddenCommand(const std::string &cmd)
{
   std::list<std::string>::iterator pos =
      find(mHiddenCommands.begin(), mHiddenCommands.end(), cmd);
   return pos != mHiddenCommands.end();
}



//------------------------------------------------------------------------------
// void  RemoveHiddenCommand(const std::string &cmd)
//------------------------------------------------------------------------------
/*
 * Remove command so that it is shown in menu
 *
 * @param  cmd  command name to be removed
 */
//------------------------------------------------------------------------------
void GmatGlobal::RemoveHiddenCommand(const std::string &cmd)
{
   #ifdef DEBUG_HIDDEN_COMMAND
   MessageInterface::ShowMessage
      ("FileManager::RemoveHiddenCommand() Removing %s to HiddenCommands\n",
       cmd.c_str());
   #endif

   std::list<std::string>::iterator pos =
      find(mHiddenCommands.begin(), mHiddenCommands.end(), cmd);
   if (pos != mHiddenCommands.end())
	mHiddenCommands.erase(pos);

   #ifdef DEBUG_HIDDEN_COMMAND
   pos = mHiddenCommands.begin();
   while (pos != mHiddenCommands.end())
   {
      MessageInterface::ShowMessage
         ("   mHiddenCommands=%s\n",(*pos).c_str());
      ++pos;
   }
   #endif
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
   isBatchMode = false;
   isNitsClient = false;
   runInterrupted = false;
   isMatlabAvailable = false;
   isMatlabDebugOn = false;
   isEventLocationAvailable = false;
   isMissionTreeDebugOn = false;
   isWritingParameterInfo = false;
   isWritingGmatKeyword = true;
   runMode = NORMAL;
   guiMode = NORMAL_GUI;
   plotMode = NORMAL_PLOT;
   matlabMode = SHARED;
   matlabExt = "__m__";
   
   SetDefaultFormat();
   SetToDefaultFormat();
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

