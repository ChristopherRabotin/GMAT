//$Header$
//------------------------------------------------------------------------------
//                                  TextEphemFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun / NASA
// Created: 2006/03/29
//
/**
 * Writes the specified parameter value to a text ephemeris file.
 */
//------------------------------------------------------------------------------

#include "TextEphemFile.hpp"
#include "CubicSplineInterpolator.hpp"
#include "Moderator.hpp"
#include "MessageInterface.hpp"
#include <iomanip>

//#define DEBUG_EPHEMFILE 1
//#define DEBUG_EPHEMFILE_DATA 1
//#define DEBUG_RENAME 1

//---------------------------------
// static data
//---------------------------------
const std::string
TextEphemFile::PARAMETER_TEXT[TextEphemFileParamCount - ReportFileParamCount] =
{
   "HeaderFile",
   "EpochFormat",
   "Interval",
   "CoordinateSystem",
};

const Gmat::ParameterType
TextEphemFile::PARAMETER_TYPE[TextEphemFileParamCount - ReportFileParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
};


//------------------------------------------------------------------------------
// TextEphemFile(const std::string &type, const std::string &name,
//               const std::string &fileName)
//------------------------------------------------------------------------------
TextEphemFile::TextEphemFile(const std::string &type, const std::string &name,
                             const std::string &fileName, Parameter *firstVarParam) :
   ReportFile(type, name, fileName, firstVarParam)
{
   // create Interpolator
   mInterpolator = new CubicSplineInterpolator("InternalInterpolator", 6);
   
   mHeaderFileName = "";
   mEpochFormat = "";
   mCoordSysName = "";
   mIntervalInSec = 0.0;
   mCurrA1Mjd = 0.0;
   mOutputA1Mjd = 0.0;
   mStartA1Mjd = 0.0;
   mStopA1Mjd = 0.0;
   
   mNumValidPoints = 0;
   mBufferSize = mInterpolator->GetBufferSize();

   //MessageInterface::ShowMessage
   //   ("TextEphemFile() Constructor: mBufferSize=%d\n", mBufferSize);
}


//------------------------------------------------------------------------------
// ~TextEphemFile(void)
//------------------------------------------------------------------------------
TextEphemFile::~TextEphemFile(void)
{
   delete mInterpolator;
   dstream.flush();
   dstream.close();
}


//------------------------------------------------------------------------------
// TextEphemFile(const TextEphemFile &copy)
//------------------------------------------------------------------------------
TextEphemFile::TextEphemFile(const TextEphemFile &copy) :
   ReportFile(copy)
{
   mHeaderFileName = copy.mHeaderFileName;
   mEpochFormat = copy.mEpochFormat;
   mCoordSysName = copy.mCoordSysName;
   mIntervalInSec = copy.mIntervalInSec;
   mCurrA1Mjd = copy.mCurrA1Mjd;
   mOutputA1Mjd = copy.mOutputA1Mjd;
   mStartA1Mjd = copy.mStartA1Mjd;
   mStopA1Mjd = copy.mStopA1Mjd;
   
   // create Interpolator
   mInterpolator = (Interpolator*)(copy.mInterpolator->Clone());
   
   mNumValidPoints = 0;
   mBufferSize = mInterpolator->GetBufferSize();

   //MessageInterface::ShowMessage
   //   ("TextEphemFile() Copy Constructor: mBufferSize=%d\n", mBufferSize);   
}


//------------------------------------------------------------------------------
// TextEphemFile& TextEphemFile::operator=(const TextEphemFile& right)
//------------------------------------------------------------------------------
/**
 * The assignment operator
 */
//------------------------------------------------------------------------------
TextEphemFile& TextEphemFile::operator=(const TextEphemFile& right)
{
   if (this == &right)
      return *this;

   ReportFile::operator=(right);
   
   mHeaderFileName = right.mHeaderFileName;
   mEpochFormat = right.mEpochFormat;
   mCoordSysName = right.mCoordSysName;
   mIntervalInSec = right.mIntervalInSec;
   mCurrA1Mjd = right.mCurrA1Mjd;
   mOutputA1Mjd = right.mOutputA1Mjd;
   mStartA1Mjd = right.mStartA1Mjd;
   mStopA1Mjd = right.mStopA1Mjd;
   
   // create Interpolator
   mInterpolator = (Interpolator*)(right.mInterpolator->Clone());
   
   mNumValidPoints = 0;
   mBufferSize = mInterpolator->GetBufferSize();

   //MessageInterface::ShowMessage
   //   ("TextEphemFile() = operator: mBufferSize=%d\n", mBufferSize);
   
   return *this;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the TextEphemFile.
 *
 * @return clone of the TextEphemFile.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TextEphemFile::Clone(void) const
{
   return (new TextEphemFile(*this));
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer TextEphemFile::GetParameterID(const std::string &str) const
{
   for (Integer i = ReportFileParamCount; i < TextEphemFileParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ReportFileParamCount])
         return i;
   }

   return ReportFile::GetParameterID(str);
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string TextEphemFile::GetParameterText(const Integer id) const
{
    if (id >= ReportFileParamCount && id < TextEphemFileParamCount)
        return PARAMETER_TEXT[id - ReportFileParamCount];
    else
        return ReportFile::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType TextEphemFile::GetParameterType(const Integer id) const
{
    if (id >= ReportFileParamCount && id < TextEphemFileParamCount)
        return PARAMETER_TYPE[id - ReportFileParamCount];
    else
        return ReportFile::GetParameterType(id);

}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string TextEphemFile::GetParameterTypeString(const Integer id) const
{
   if (id >= ReportFileParamCount && id < TextEphemFileParamCount)
      return ReportFile::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return ReportFile::GetParameterTypeString(id);

}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real TextEphemFile::GetRealParameter(const Integer id) const
{
   switch (id)
   {
   case INTERVAL:
      return mIntervalInSec;
   default:
      return ReportFile::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real TextEphemFile::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real TextEphemFile::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
   case INTERVAL:
      mIntervalInSec = value;
      return value;
   default:
      return ReportFile::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
Real TextEphemFile::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string TextEphemFile::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case HEADER_FILE:
      return mHeaderFileName;
   case EPOCH_FORMAT:
      return mEpochFormat;
   case COORD_SYSTEM:
      return mEpochFormat;
   default:
      return ReportFile::GetStringParameter(id);
   }
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
std::string TextEphemFile::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool TextEphemFile::SetStringParameter(const Integer id, const std::string &value)
{
   switch (id)
   {
   case HEADER_FILE:
      mHeaderFileName = value;
      return true;
   case EPOCH_FORMAT:
      mEpochFormat = value;
      return true;
   case COORD_SYSTEM:
      mCoordSysName = value;
      return true;
   default:
      return ReportFile::SetStringParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
bool TextEphemFile::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//--------------------------------------
// protected methods
//--------------------------------------


//--------------------------------------
// methods inherited from Subscriber
//--------------------------------------

//------------------------------------------------------------------------------
// bool Distribute(const Real * dat, Integer len)
//------------------------------------------------------------------------------
bool TextEphemFile::Distribute(const Real * dat, Integer len)
{
   if (!active)
      return true;

   if (isEndOfRun)
      WriteEphemHeader();
   
   if (len == 0)
      return false;
   
   if (mNumVarParams > 0)
   {      
      if (!dstream.is_open())
         if (!OpenReportFile())
            return false;
      
      if (initial)
      {
         WriteColumnTitle();
         //ReportFile::WriteHeaders();
         
         // Compute next output time, first data is time
         mCurrA1Mjd = dat[0];
         mOutputA1Mjd = dat[0] + mIntervalInSec/86400.0;
         
         // Write first data
         WriteFirstData();
         
         #if DEBUG_EPHEMFILE_DATA
         MessageInterface::ShowMessage
         ("TextEphemFile::Distribute() first dat=%f %f %f %f %g %g %g\n", dat[0], dat[1],
          dat[2], dat[3], dat[4], dat[5], dat[6]);
         MessageInterface::ShowMessage
            ("   mOutputA1Mjd=%f, mNumVarParams=%d\n", mOutputA1Mjd, mNumVarParams);
         #endif
      }
      
      if (!dstream.good())
         dstream.clear();

      mCurrA1Mjd = dat[0];
      WriteToBuffer();
      
      if (IsTimeToWrite())
      {
         WriteData();

         // try one more time for step size > interval
         if (IsTimeToWrite())
            WriteData();
      }
      
      #if DEBUG_EPHEMFILE_DATA > 1
      MessageInterface::ShowMessage
         ("TextEphemFile::Distribute() dat=%f %f %f %f %g %g %g\n", dat[0], dat[1],
          dat[2], dat[3], dat[4], dat[5], dat[6]);
      #endif
   }
   
   return true;
}


//------------------------------------------------------------------------------
// void WriteEphemHeader()
//------------------------------------------------------------------------------
void TextEphemFile::WriteEphemHeader()
{
   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("===> TextEphemFile::WriteEphemHeader() mHeaderFileName=%s\n",
       mHeaderFileName.c_str());
   #endif

   // Need to write MATLAB format scripts for Spacecraft and PropSetup
   // but subscriber doesn't know about these objects, so use the Moderator
   Moderator *theModerator = Moderator::Instance();
   theModerator->SaveScript(mHeaderFileName, Gmat::EPHEM_HEADER);

   // Write additional parameters
   std::ofstream ofs(mHeaderFileName.c_str(), std::ios::app);

   std::string ephemSource = theModerator->GetCurrentPlanetarySource();
   ofs << "\n\n";
   ofs << "PlanetaryEphemerisSource = '" << ephemSource << "';\n";
   ofs << "\n";
   
   ofs << "Output.EpochType = '" << mEpochFormat << "';\n";
   ofs << "Output.StartEpoch = " << mStartA1Mjd << ";\n";
   ofs << "Output.StopEpoch = " << mStartA1Mjd << ";\n";
   ofs << "Output.IntervalType = 'Second';\n";
   ofs << "Output.Interval = " << mIntervalInSec << ";\n";
   ofs << "Output.CoordinateSystem = '" << mCoordSysName << "';\n";
   ofs << "Output.StateType = 'Cartesian';\n";
   ofs << "\n";
   
   ofs << "Time.Unit = 'Day';\n";
   ofs << "X.Unit = 'Km';\n";
   ofs << "Y.Unit = 'Km';\n";
   ofs << "Z.Unit = 'Km';\n";
   ofs << "VX.Unit = 'Km/Sec';\n";
   ofs << "VY.Unit = 'Km/Sec';\n";
   ofs << "VZ.Unit = 'Km/Sec';\n";
   ofs << "\n";

   ofs.close();
}


//------------------------------------------------------------------------------
// void WriteColumnTitle()
//------------------------------------------------------------------------------
void TextEphemFile::WriteColumnTitle()
{
   if (writeHeaders)
   {
      if (!dstream.is_open())
         return;

      // write heading for each item
      for (int i=0; i < mNumVarParams; i++)
      {
          if (!dstream.good())
             dstream.clear();
          
          // set longer width of param names or columnWidth
          Integer width = (Integer)mVarParamNames[i].length() > columnWidth ?
             mVarParamNames[i].length() : columnWidth;

          // parameter name has Gregorian, minimum width is 24
          if (mVarParamNames[i].find("Gregorian") != mVarParamNames[i].npos)
             if (width < 24)
                width = 24;
          
          dstream.width(width); // sets miminum field width
          
          //dstream.width(columnWidth);
          dstream.fill(' ');
          
          if (leftJustify)
             dstream.setf(std::ios::left);

          dstream << mVarParamNames[i] << "   ";

          // save time width for writing data
          mColWidth[i] = width;
      }
      
      dstream << std::endl;
   }
   
   initial = false;
}


//------------------------------------------------------------------------------
// void WriteToBuffer()
//------------------------------------------------------------------------------
void TextEphemFile::WriteToBuffer()
{
   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::WriteToBuffer() mNumValidPoints=%d, mBufferSize=%d\n",
       mNumValidPoints, mBufferSize);
   #endif
   
   // buffer up
   mNumValidPoints = (mNumValidPoints < BUFFER_SIZE) ?
      mNumValidPoints + 1 : mNumValidPoints;

   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::WriteToBuffer() mNumValidPoints=%d, BUFFER_SIZE=%d\n",
       mNumValidPoints, BUFFER_SIZE);
   #endif
   
   // shift values to make room for newest value
   for (int i=0; i<BUFFER_SIZE-1; i++)
   {
      mTimeBuffer[i] = mTimeBuffer[i+1];
      mXposBuffer[i] = mXposBuffer[i+1];
      mYposBuffer[i] = mYposBuffer[i+1];
      mZposBuffer[i] = mZposBuffer[i+1];
      mXvelBuffer[i] = mXvelBuffer[i+1];
      mYvelBuffer[i] = mYvelBuffer[i+1];
      mZvelBuffer[i] = mZvelBuffer[i+1];
   }
   
   Real rval = -9999.999;

   mTimeBuffer[BUFFER_SIZE - 1] = mCurrA1Mjd;
   
   for (int i=1; i < mNumVarParams; i++)
   {
      // assuming all real parameters

      rval = mVarParams[i]->EvaluateReal();

      #if DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("TextEphemFile::WriteToBuffer() i=%d, rval=%f\n", i, rval);
      #endif
      
      if (i == 1) //(mVarParams[i]->GetName() == "X")
      {
         mXposBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[0] = rval;
      }
      else if (i == 2) //(mVarParams[i]->GetName() == "Y")
      {
         mYposBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[1] = rval;
      }
      else if (i == 3) //(mVarParams[i]->GetName() == "Z")
      {
         mZposBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[2] = rval;
      }
      else if (i == 4) //(mVarParams[i]->GetName() == "VX")
      {
         mXvelBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[3] = rval;
      }
      else if (i == 5) //(mVarParams[i]->GetName() == "VY")
      {
         mYvelBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[4] = rval;
      }
      else if (i == 6) //(mVarParams[i]->GetName() == "VZ")
      {
         mZvelBuffer[BUFFER_SIZE - 1] = rval;
         mOutputVals[5] = rval;
      }
      
   }
}


//------------------------------------------------------------------------------
// bool IsTimeToWrite()
//------------------------------------------------------------------------------
bool TextEphemFile::IsTimeToWrite()
{
   #if DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::IsTimeToWrite() mNumValidPoints=%d, mCurrA1Mjd=%f, "
       "mOutputA1Mjd=%f\n", mNumValidPoints,  mCurrA1Mjd, mOutputA1Mjd);
   #endif
   
   // if output time is between the points and has BUFFER_SIZE points
   if (mNumValidPoints == BUFFER_SIZE)
   {
      // find mid point
      Integer midIndex = -1;
      for (int i=1; i<BUFFER_SIZE-1; i++)
      {
         if (mOutputA1Mjd >= mTimeBuffer[i] && mOutputA1Mjd <= mTimeBuffer[i+1])
         {
            midIndex = i;
            break;
         }
      }

      if (midIndex < 1)
         return false;

      
      #if DEBUG_EPHEMFILE_DATA
      MessageInterface::ShowMessage
         ("==> mTimeBuffer[%d]=%f\n", midIndex, mTimeBuffer[midIndex]);
      #endif
      
      Real vals[6];
      
      for (int i=midIndex-2; i<=midIndex+2; i++)
      {
         vals[0] = mXposBuffer[i];
         vals[1] = mYposBuffer[i];
         vals[2] = mZposBuffer[i];
         vals[3] = mXvelBuffer[i];
         vals[4] = mYvelBuffer[i];
         vals[5] = mZvelBuffer[i];
         
         mInterpolator->AddPoint(mTimeBuffer[i], vals);
      }
      
      if (mInterpolator->Interpolate(mOutputA1Mjd, mOutputVals))
      
      //@todo: Do some value check here
      return true;
   }

   return false;
}


//------------------------------------------------------------------------------
// void WriteData()
//------------------------------------------------------------------------------
void TextEphemFile::WriteData()
{
   // Write time
   //@todo: Convert epoch to epoch format (mEpochStr)
   //dstream << mOutputA1Mjd*86400.0 << "   ";
   dstream.width(mColWidth[0]);
   dstream << mOutputA1Mjd << "   ";
   
   for (int i=1; i<mNumVarParams; i++)
   {
      dstream.width(mColWidth[i]);
      dstream.fill(' ');
      
      if (leftJustify)
      {
         dstream.setf(std::ios::left);
         if (zeroFill)
            dstream.fill('0');                 
      }
      
      dstream.precision(precision);
      dstream << mOutputVals[i-1] << "   "; 
   }
   
   dstream << std::endl;
   
   // Compute new output time
   mOutputA1Mjd = mOutputA1Mjd + mIntervalInSec/86400.0;

   #if #DEBUG_EPHEMFILE_DATA
   MessageInterface::ShowMessage
      ("TextEphemFile::WriteData() new mOutputA1Mjd=%f\n\n", mOutputA1Mjd);
   #endif
   
}


//------------------------------------------------------------------------------
// void WriteFirstData()
//------------------------------------------------------------------------------
void TextEphemFile::WriteFirstData()
{
   Real rval = -999.999;

   // Write time
   //@todo: Convert epoch to epoch format (mEpochStr)
   //dstream << mOutputA1Mjd*86400.0 << "   ";
   dstream.width(mColWidth[0]);
   dstream << mCurrA1Mjd << "   ";
   
   for (int i=1; i<mNumVarParams; i++)
   {
      rval = mVarParams[i]->EvaluateReal();
      
      dstream.width(mColWidth[i]);
      dstream.fill(' ');
      
      if (leftJustify)
      {
         dstream.setf(std::ios::left);
         if (zeroFill)
            dstream.fill('0');                 
      }
      
      dstream.precision(precision);
      dstream << rval << "   "; 
   }
   
   dstream << std::endl;
   
}


