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
 * Declares Class for getting/setting global data used by GMAT.
 */
//------------------------------------------------------------------------------
#ifndef GmatGlobal_hpp
#define GmatGlobal_hpp

#include "gmatdefs.hpp"

class GMAT_API GmatGlobal
{
public:
   
   static GmatGlobal* Instance();
   
   // Real to string conversion precison
   static const Integer DATA_PRECISION;
   static const Integer TIME_PRECISION;
   static const Integer DATA_WIDTH;
   static const Integer TIME_WIDTH;
   static const Integer INTEGER_WIDTH;
   
   Integer GetDataPrecision() { return currentSetting.mDataPrecision; }
   Integer GetTimePrecision() { return currentSetting.mTimePrecision; }
   Integer GetDataWidth() { return currentSetting.mDataWidth; }
   Integer GetTimeWidth() { return currentSetting.mTimeWidth; }
   Integer GetIntegerWidth() { return currentSetting.mIntegerWidth; }
   std::string GetOutputPath() { return currentSetting.mOutputPath; }
   
   void SetDataPrecision(Integer p) { currentSetting.mDataPrecision = p; }
   void SetTimePrecision(Integer p) { currentSetting.mTimePrecision = p; }
   void SetDataWidth(Integer w) { currentSetting.mDataWidth = w; }
   void SetTimeWidth(Integer w) { currentSetting.mTimeWidth = w; }
   void SetIntegerWidth(Integer w) { currentSetting.mIntegerWidth = w; }
   void SetOutputPath(const std::string &path) { currentSetting.mOutputPath = path; }
   
   // Run mode
   bool IsBatchMode() { return isBatchMdoe; }
   void SetBatchMode(bool flag) { isBatchMdoe = flag; }
   
   // IO formatting
   bool IsScientific() { return actualFormat.mScientific; }
   bool IsHorizontal() { return actualFormat.mHorizontal; }
   bool IsBinaryIn() { return actualFormat.mBinaryIn; }
   bool IsBinaryOut() { return actualFormat.mBinaryOut; }
   Integer GetSpacing() { return currentFormat.mSpacing; }
   
   void SetScientific(bool flag) { actualFormat.mScientific = flag; }
   void SetHorizontal(bool flag) { actualFormat.mHorizontal = flag; }
   void SetBinaryIn(bool flag) { actualFormat.mBinaryIn = flag; }
   void SetBinaryOut(bool flag) { actualFormat.mBinaryOut = flag; }
   void SetSpacing(Integer sp) { actualFormat.mSpacing = sp; }
   
   void SetDefaultFormat();
   void SetCurrentFormat(bool scientific = false,
                         Integer width = GmatGlobal::DATA_WIDTH,
                         Integer precision = GmatGlobal::DATA_PRECISION,
                         bool horizontal = true, Integer spacing = 1, 
                         bool binaryIn = false, bool binaryOut = false);
   
   void GetActualFormat(bool &scientific, Integer &precision, Integer &width,
                        bool &horizontal, Integer &spacing); 
   
   void SetActualFormat(bool scientific, Integer precision, Integer width,
                        bool horizontal = true, Integer spacing = 1);
   
   void SetToDefaultFormat() { actualFormat = defaultFormat; }
   void SetToCurrentFormat() { actualFormat = currentFormat; }
   
   
private:

   // Global setting
   struct Setting
   {
      Setting(Integer dp = DATA_PRECISION, Integer tp = TIME_PRECISION,
              Integer dw = DATA_WIDTH, Integer tw = TIME_WIDTH,
              Integer iw = INTEGER_WIDTH, const std::string &op = "")
      {
         mDataPrecision = dp;
         mTimePrecision = tp;
         mDataWidth = dw;
         mTimeWidth = tw;
         mIntegerWidth = iw;
         mOutputPath = op;
      };
      
      Integer mDataPrecision;
      Integer mTimePrecision;
      Integer mDataWidth;
      Integer mTimeWidth;
      Integer mIntegerWidth;
      std::string mOutputPath;
   };
   
   // I/O formatting   
   struct IoFormat
   {
      IoFormat(bool scientific = false,
               Integer precision = GmatGlobal::DATA_PRECISION,
               Integer width = GmatGlobal::DATA_WIDTH,
               bool horizontal = true, Integer spacing = 1,
               bool binaryIn = false, bool binaryOut = false)
      {
         mScientific = scientific;
         mPrecision = precision;
         mWidth = width;
         mHorizontal = horizontal;
         mSpacing = spacing;
         mBinaryIn = binaryIn;
         mBinaryOut = binaryOut;
      };
      
      bool mScientific;    /// format using scientific notation
      Integer mPrecision;  /// the number of digits of precision
      Integer mWidth;      /// the field width
      bool mHorizontal;    /// format horizontally if true. Default is false
      Integer mSpacing;    /// determines number of spaces in between each element
      bool mBinaryIn;      /// read in binary if true. Default is false
      bool mBinaryOut;     /// print in binary if true. Default is false
      
   };
   
   Setting defaultSetting;
   Setting currentSetting;
   
   bool isBatchMdoe;
   
   IoFormat defaultFormat;
   IoFormat currentFormat;
   IoFormat actualFormat;
   
   /// The singleton instance
   static GmatGlobal *theGmatGlobal;
   
   GmatGlobal();
   ~GmatGlobal();
};

#endif // GmatGlobal_hpp

