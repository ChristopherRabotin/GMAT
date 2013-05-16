//$Id$
//------------------------------------------------------------------------------
//                                  Code500EphemerisFile
//------------------------------------------------------------------------------
#include "Code500EphemerisFile.hpp"
#include "TimeSystemConverter.hpp"   // For ConvertGregorianToMjd()
#include "Rvector6.hpp"
#include "DateUtil.hpp"              // For friend function UnpackDate(), ToHMSFromSecondsOfDay
#include "RealUtilities.hpp"         // For IsEqual()
#include "GregorianDate.hpp"         // For GregorianDate()
#include "GmatConstants.hpp"         // For GmatTimeConstants::A1_TAI_OFFSET
#include "MessageInterface.hpp"

//#define DEBUG_SET
//#define DEBUG_READ_DATA
//#define DEBUG_WRITE_HEADERS
//#define DEBUG_WRITE_DATA_SEGMENT
//#define DEBUG_WRITE_DATA_SEGMENT_MORE
//#define DEBUG_UNPACK_HEADERS

// static data
const double Code500EphemerisFile::DUL_TO_KM         = 10000.0;
const double Code500EphemerisFile::DUL_DUT_TO_KM_SEC = 10000.0 / 864.0;
const double Code500EphemerisFile::KM_TO_DUL         = 1.0 / 10000.0;
const double Code500EphemerisFile::KM_SEC_TO_DUL_DUT = 864.0 / 10000.0;
const double Code500EphemerisFile::SEC_TO_DUT        = 1.0 / 864.0;
const double Code500EphemerisFile::DAY_TO_DUT        = 86400.0 / 864.0;
const double Code500EphemerisFile::DUT_TO_DAY        = 864.0 / 86400.0;
const double Code500EphemerisFile::DUT_TO_SEC        = 864.0;

//------------------------------------------------------------------------------
// Code500EphemerisFile(double satId, const std::string &productId, ...)
//------------------------------------------------------------------------------
Code500EphemerisFile::Code500EphemerisFile(double satId, const std::string &productId,
                         const std::string &timeSystem, const std::string &tapeId,
                         const std::string &sourceId, const std::string &centralBody)
{
   #ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("Code500EphemerisFile() entered\n   satId=%f, productId='%s', timeSystem='%s', "
       "tapeId='%s', sourceId='%s', centralBody='%s'\n", satId, productId.c_str(),
       timeSystem.c_str(), tapeId.c_str(), sourceId.c_str(), centralBody.c_str());
   #endif
   
   mSatId = satId;
   CopyString(mProductId, productId, 8);
   mTimeSystem = timeSystem;
   // Time System Indicator: 0.0 = A.1, Atomic Time, 1.0 = UTC, Universal Time Coordinated
   mInTimeSystemIndicator = 0.0;
   mOutTimeSystemIndicator = 0.0;
   if (mTimeSystem == "A1")
      mOutTimeSystemIndicator = 1.0;
   else if (mTimeSystem == "UTC")
      mOutTimeSystemIndicator = 2.0;
   mCoordSystem = "2000";
   CopyString(mTapeId, tapeId, 8);
   CopyString(mSourceId, sourceId, 8);
   mCentralBody = centralBody;
   mDataRecWriteCounter = 2; // data record starts at 3
   mLastDataRecRead = 2;
   mLastStateIndexRead = -1;
   
   mSentinelData = 9.99999999999999e15;
   mSentinelsFound = false;
   mA1GregorianOfDUTRef = "18 Sep 1957 00:00:00.000";
   mRefTimeForDUT_YYMMDD = 570918.0;
   mA1MjdOfDUTRef = TimeConverterUtil::ConvertGregorianToMjd(mA1GregorianOfDUTRef);
   mTimeIntervalBetweenPointsSecs = 0.0;
   
   mSwapEndian = false;
   
   // Fill in some header and data record with initial values
   InitializeHeader1();
   InitializeHeader2();
   InitializeDataRecord();
   
   #ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("Code500EphemerisFile() leaving\n   size of header1 = %d\n   "
       "size of header2 = %d\n   size of data    = %d\n   mSentinelData    = % 1.15e\n",
       sizeof(mEphemHeader1), sizeof(mEphemHeader2), sizeof(mEphemData),
       mSentinelData);
   #endif
}

//------------------------------------------------------------------------------
// ~Code500EphemerisFile()
//------------------------------------------------------------------------------
Code500EphemerisFile::~Code500EphemerisFile()
{
   if (mEphemFileIn.is_open())
      mEphemFileIn.close();
   
   if (mEphemFileOut.is_open())
   {
      mEphemFileOut.flush();
      mEphemFileOut.close();
   }
}


//------------------------------------------------------------------------------
// Code500EphemerisFile(const Code500EphemerisFile &ef) 
//------------------------------------------------------------------------------
Code500EphemerisFile::Code500EphemerisFile(const Code500EphemerisFile &ef) 
{
}


//------------------------------------------------------------------------------
// Code500EphemerisFile& operator=(const Code500EphemerisFile& ef)
//------------------------------------------------------------------------------
Code500EphemerisFile& Code500EphemerisFile::operator=(const Code500EphemerisFile& ef)
{
   //if (this == &ef)
      return *this;
}


//------------------------------------------------------------------------------
// bool OpenForRead(const std::string &fileName)
//------------------------------------------------------------------------------
bool Code500EphemerisFile::OpenForRead(const std::string &fileName)
{
   #ifdef DEBUG_OPEN
   MessageInterface::ShowMessage("OpenForRead() entered, fileName='%s'\n", fileName.c_str());
   #endif
   
   // If input file is already open close it first
   // CloseForRead();
   
   mEphemFileIn.open(fileName.c_str(), std::ios_base::binary | std::ios_base::ate);
   if (!mEphemFileIn.is_open())
   {
      #ifdef DEBUG_OPEN
      MessageInterface::ShowMessage
         ("OpenForRead() just returning, cannot open ephemfile '%s'\n",
          fileName.c_str());
      #endif
      
      //@note Should exception be thrown here?
      return false;
   }
   
   std::streampos fsize = mEphemFileIn.tellg();
   
   return true;
}


//------------------------------------------------------------------------------
// bool Code500EphemerisFile::OpenForWrite(const std::string &fileName)
//------------------------------------------------------------------------------
bool Code500EphemerisFile::OpenForWrite(const std::string &fileName)
{
   #ifdef DEBUG_OPEN
   MessageInterface::ShowMessage
      ("OpenForWrite() entered, fileName='%s'\n", fileName.c_str());
   #endif
   
   mEphemFileOut.open(fileName.c_str(), std::ios_base::binary);
   if (!mEphemFileOut.is_open())
   {
      #ifdef DEBUG_OPEN
      MessageInterface::ShowMessage
         ("OpenForWrite() just returning, cannot open ephemfile '%s'\n",
          fileName.c_str());
      #endif
      
      //@note Should exception be thrown here?
      return false;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// void CloseForRead()
//------------------------------------------------------------------------------
void Code500EphemerisFile::CloseForRead()
{
   if (mEphemFileIn.is_open())
      mEphemFileIn.close();
   
   mSentinelsFound = false;
}


//------------------------------------------------------------------------------
// void CloseForWrite()
//------------------------------------------------------------------------------
void Code500EphemerisFile::CloseForWrite()
{
   if (mEphemFileOut.is_open())
      mEphemFileOut.close();
}


//------------------------------------------------------------------------------
// bool ReadHeader1(int logOption = 0)
//------------------------------------------------------------------------------
/**
 * Unpacks heder 1 record and log values to log file on option.
 *
 * @param  logOption  0 = no log, 1 = log all header data
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::ReadHeader1(int logOption)
{
   #ifdef DEBUG_READ_HEADERS
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::ReadHeader1() entered, logOption=%d\n", logOption);
   #endif
   
   if (!mEphemFileIn.is_open())
   {
      #ifdef DEBUG_READ_HEADERS
      MessageInterface::ShowMessage
         ("ReadHeader1() returning false, input ephem file is not opened\n");
      #endif
      
      //@note Should exception be thrown here?
      return false;
   }
   
   // Read the first header
   mEphemFileIn.seekg(std::ios_base::beg);
   mEphemFileIn.read((char*)(&mEphemHeader1), RECORD_SIZE);
   
   // Save infile time system used
   mInTimeSystemIndicator = mEphemHeader1.timeSystemIndicator;
   
   if (logOption == 1)
      UnpackHeader1();
   
   #ifdef DEBUG_READ_HEADERS
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::ReadHeader1() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool ReadHeader2(int logOption = 0)
//------------------------------------------------------------------------------
/**
 * Unpacks heder 2 record and log values to log file on option.
 *
 * @param  logOption  0 = no log, 1 = log all header data
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::ReadHeader2(int logOption)
{
   if (!mEphemFileIn.is_open())
   {
      #ifdef DEBUG_READ_HEADERS
      MessageInterface::ShowMessage
         ("ReadHeader2() returning false, input ephem file is not opened\n");
      #endif
      
      //@note Should exception be thrown here?
      return false;
   }
   
   // Read the second header
   mEphemFileIn.seekg(RECORD_SIZE, std::ios_base::beg);
   mEphemFileIn.read((char*)&mEphemHeader2, RECORD_SIZE);
   
   if (logOption == 1)
      UnpackHeader2();
   
   return true;
}


//------------------------------------------------------------------------------
// bool ReadDataAt(int dataRecNumber, int logOption = 0)
//------------------------------------------------------------------------------
/**
 * Reads data record at requested data record number. It will add the size of
 * 2 header records before positioning the file.
 *
 * @param  dataRecNumber  Data record number
 * @param  logOption      = 0, no log
 *                        = 1, log first state vector of only first and last record
 *                        = 2, log first and last state vector of all records
 *                        = 3, log all state vectors of all records
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::ReadDataAt(int dataRecNumber, int logOption)
{
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage
      ("ReadDataAt() entered, dataRecNumber = %d, logOption = %d\n", dataRecNumber,
       logOption);
   #endif
   
   if (!mEphemFileIn.is_open())
   {
      #ifdef DEBUG_READ_DATA
      MessageInterface::ShowMessage
         ("ReadDataAt() returning false, input ephem file is not opened\n");
      #endif
      
      //@note Should exception be thrown here?
      return false;
   }
   
   int filePos = (dataRecNumber + 1) * RECORD_SIZE;
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage("   filePos = %d\n", filePos);
   #endif
   // Position file at requested data record number
   mEphemFileIn.seekg(filePos, std::ios_base::beg);
   
   // Check for eof
   if (mEphemFileIn.eof())
   {
      #ifdef DEBUG_READ_DATA
      MessageInterface::ShowMessage
         ("ReadDataAt() returning false, input ephem file reached eof\n");
      #endif
      return false;
   }
   
   mEphemFileIn.read((char*)&mEphemData, RECORD_SIZE);
   
   // Always unpack data to find sentinel data for end of state vector
   UnpackDataRecord(dataRecNumber, logOption);
   
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage
      ("ReadDataAt() returning true, dataRecNumber = %d, mLastStateIndexRead = %d\n",
       dataRecNumber, mLastStateIndexRead);
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// bool ReadDataRecords(int numRecordsToRead = -1, int logOption = 0)
//------------------------------------------------------------------------------
/**
 * Reads data record at requested data record number. It will add the size of
 * 2 header records before positioning the file.
 *
 * @param  numRecordsToRead = -999, Read entire file
 *                          > 0, Reads number of records
 * @param  logOption        = 0, no log
 *                          = 1, log first state vector of only first and last record
 *                          = 2, log first state vector of all specified number of records
 *                          = 3, log all state vectors of all specified number of records
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::ReadDataRecords(int numRecordsToRead, int logOption)
{
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage
      ("ReadDataRecords() entered, numRecordsToRead = %d, logOption = %d, "
       "mSentinelsFound=%d\n", numRecordsToRead, logOption, mSentinelsFound);
   #endif
   
   if (!mEphemFileIn.is_open())
   {
      #ifdef DEBUG_READ_DATA
      MessageInterface::ShowMessage
         ("ReadDataRecords() returning false, input ephem file is not opened\n");
      #endif
      
      //@note Should exception be thrown here?
      return false;
   }
   
   int recCount = 1;
   
   // Position file to first data record
   int filePos = RECORD_SIZE * 2;
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage("   filePos = %d\n", filePos);
   #endif
   mEphemFileIn.seekg(filePos, std::ios_base::beg);
   
   // Read the data until eof or 10 sentinels found, or numRecordsToRead reached
   bool continueRead = true;
   while (continueRead && !mSentinelsFound)
   {
      if (mEphemFileIn.eof())
      {
         #ifdef DEBUG_READ_DATA
         MessageInterface::ShowMessage("End of file reached\n");
         #endif
         break;
      }
      else
      {
         ReadDataAt(recCount, logOption);
         
         if ((numRecordsToRead != -999) && (recCount >= numRecordsToRead))
         {
            #ifdef DEBUG_READ_DATA
            MessageInterface::ShowMessage("Read requested %d records\n", numRecordsToRead);
            #endif
            break;
         }
         else if (mSentinelsFound)
         {
            #ifdef DEBUG_READ_DATA
            MessageInterface::ShowMessage
               ("Sentinels found at record # %d, mLastStateIndexRead = %d\n", recCount, mLastStateIndexRead);
            #endif
            
            // Log last data record
            if (logOption > 0)
               UnpackDataRecord(recCount, logOption);
            break;
         }
         recCount++;
      }
   }
   
   #ifdef DEBUG_READ_DATA
   MessageInterface::ShowMessage("ReadDataRecords() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool FinalizeHeaders()
//------------------------------------------------------------------------------
bool Code500EphemerisFile::FinalizeHeaders()
{
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::FinalizeHeaders() entered\n");
   #endif
   
   // Anything to finalize header 1?
   
   // Write final header 1 record
   WriteHeader1();
   
   #ifdef DEBUG_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::FinalizeHeaders() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool WriteHeader1()
//------------------------------------------------------------------------------
bool Code500EphemerisFile::WriteHeader1()
{
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteHeader1() entered\n");
   #endif
   
   if (!mEphemFileOut.is_open())
   {
      #ifdef DEBUG_WRITE_HEADERS
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteHeader1() returning false, output ephem file is not opened\n");
      #endif
      return false;
   }
   
   mEphemFileOut.seekp(std::ios_base::beg);
   mEphemFileOut.write((char*)&mEphemHeader1, RECORD_SIZE);
   
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteHeader1() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool WriteHeader2()
//------------------------------------------------------------------------------
bool Code500EphemerisFile::WriteHeader2()
{
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteHeader2() entered\n");
   #endif
   
   if (!mEphemFileOut.is_open())
   {
      #ifdef DEBUG_WRITE_HEADERS
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteHeader2() returning false, output ephem file is not opened\n");
      #endif
      return false;
   }
   
   mEphemFileOut.seekp(RECORD_SIZE, std::ios_base::beg);
   mEphemFileOut.write((char*)&mEphemHeader2, RECORD_SIZE);
   
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteHeader2() returning true\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool WriteDataAt(int recNumber)
//------------------------------------------------------------------------------
bool Code500EphemerisFile::WriteDataAt(int recNumber)
{
   #ifdef DEBUG_WRITE_DATA
   MessageInterface::ShowMessage
      ("============================= Code500EphemerisFile::WriteDataAt() entered, recNumber=%d\n", recNumber);
   #endif
   
   if (!mEphemFileOut.is_open())
   {
      #ifdef DEBUG_WRITE_DATA
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteDataAt() returning false, output ephem file is not opened\n");
      #endif
      
      //@note Should exception thron here?
      return false;
   }
   
   if (recNumber <=2)
   {
      #ifdef DEBUG_WRITE_DATA
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteDataAt() returning false, data records starts at 3, but requsted at %d\n",
          recNumber);
      #endif
      return false;
   }
   
   #ifdef DEBUG_WRITE_DATA
   MessageInterface::ShowMessage
      ("Writing data record at %d\n", RECORD_SIZE * (recNumber-1));
   #endif
   
   // Set to no thrust
   mEphemData.thrustIndicator = 2.0; // 1 = thrust, 2 = free flight
   
   mEphemFileOut.seekp(RECORD_SIZE * (recNumber-1), std::ios_base::beg);
   mEphemFileOut.write((char*)&mEphemData, RECORD_SIZE);
   
   #ifdef DEBUG_WRITE_DATA
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteDataAt() leaving\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool WriteDataSegment(const A1Mjd &start, const A1Mjd &end,
//                       const StateArray &stateArray, const EpochArray &epochArray,
//                       bool canFinalize = false)
//------------------------------------------------------------------------------
bool Code500EphemerisFile::WriteDataSegment(const A1Mjd &start, const A1Mjd &end,
                           const StateArray &stateArray, const EpochArray &epochArray,
                           bool canFinalize)
{
   std::string startGreg = ToA1Gregorian(start);
   std::string endGreg = ToA1Gregorian(end);
   Integer numPoints = stateArray.size();

   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("==================================================\nCode500EphemerisFile::WriteDataSegment() entered, "
       "start=%f '%s', end=%f '%s'\n", start.GetReal(), startGreg.c_str(), end.GetReal(), endGreg.c_str());
   MessageInterface::ShowMessage
      ("stateArray.size()=%d, epochArray.size()=%d, canFinalize=%d\n", stateArray.size(), epochArray.size(),
       canFinalize);
   #endif
   
   if (numPoints == 0)
   {
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteDataSegment() just returning true, data size is zero\n");
      #endif
      return true;
   }
   
   // Write data record
   mDataRecWriteCounter++;
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("mDataRecWriteCounter=%d\n", mDataRecWriteCounter);
   #endif
   
   // Set start date of ephemeris if first data record
   // Header1 = record1,  Header2 = record2,  Data1 = record3
   if (mDataRecWriteCounter == 3)
   {
      SetEphemerisStartTime(start);
      // Write initial headers
      WriteHeader1();
      WriteHeader2();
   }
   
   if (canFinalize)
      SetEphemerisEndTime(end);
   
   mEphemData.dateOfFirstEphemPoint_YYYMMDD = ToYYYMMDD(start);
   mEphemData.dayOfYearForFirstEphemPoint = ToDayOfYear(start);
   mEphemData.secsOfDayForFirstEphemPoint = ToSecondsOfDay(start);
   mEphemData.timeIntervalBetweenPoints_SEC = mTimeIntervalBetweenPointsSecs;
   
   // Write first state vector
   double stateDULT[6];
   ConvertStateKmSecToDULT(stateArray[0], stateDULT);
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   DebugWriteState(stateArray[0], stateDULT, 2);
   #endif
   mEphemData.firstStateVector_DULT[0] = stateDULT[0];
   mEphemData.firstStateVector_DULT[1] = stateDULT[1];
   mEphemData.firstStateVector_DULT[2] = stateDULT[2];
   mEphemData.firstStateVector_DULT[3] = stateDULT[3];
   mEphemData.firstStateVector_DULT[4] = stateDULT[4];
   mEphemData.firstStateVector_DULT[5] = stateDULT[5];
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("mEphemData.firstStateVector_DULT[0]  =% 1.15e\n", mEphemData.firstStateVector_DULT[0]);
   #endif
   
   // Write state 2 through numPoints
   for (int i = 1; i < numPoints; i++)
   {
      ConvertStateKmSecToDULT(stateArray[i], stateDULT);
      for (int j = 0; j < 6; j++)
         mEphemData.stateVector2Thru50_DULT[i][j] = stateDULT[j];
      
      #ifdef DEBUG_WRITE_DATA_SEGMENT_MORE
      MessageInterface::ShowMessage
         ("mEphemData.stateVector2Thru50_DULT[%2d][0]=% 1.15e\n", i, mEphemData.stateVector2Thru50_DULT[i][0]);
      #endif
   }
   
   // If data points received is less than NUM_STATES_PER_RECORD, write sentinel flag
   if (numPoints < NUM_STATES_PER_RECORD)
   {
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      MessageInterface::ShowMessage
         ("===> Writing %d sentinel data\n", NUM_STATES_PER_RECORD-numPoints);
      #endif
      
      for (int i = numPoints + 1; i < NUM_STATES_PER_RECORD; i++)
      {
         for (int j = 0; j < 6; j++)
            mEphemData.stateVector2Thru50_DULT[i][j] = mSentinelData;
         
         #ifdef DEBUG_WRITE_DATA_SEGMENT_MORE
         MessageInterface::ShowMessage
            ("mEphemData.stateVector2Thru50_DULT[%2d][0]=% 1.15e\n", i,
             mEphemData.stateVector2Thru50_DULT[i][0]);
         #endif
      }
   }
   
   // Write time of first data point and time interval in DUT
   double timeInDUT = ToDUT(start);
   mEphemData.timeOfFirstDataPoint_DUT = timeInDUT;
   mEphemData.timeIntervalBetweenPoints_SEC = mTimeIntervalBetweenPointsSecs;
   mEphemData.timeIntervalBetweenPoints_DUT = mTimeIntervalBetweenPointsSecs * SEC_TO_DUT;
   
   // Write data record
   WriteDataAt(mDataRecWriteCounter);
   
   // If last data record contains NUM_STATES_PER_RECORD valid states and writing
   // final data, write next record with 10 sentinels.
   if (numPoints == NUM_STATES_PER_RECORD && canFinalize)
   {
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      MessageInterface::ShowMessage
         ("   Writing last data record with first 10 sentinel data\n");
      #endif
      mEphemData.dateOfFirstEphemPoint_YYYMMDD = mSentinelData;
      mEphemData.dayOfYearForFirstEphemPoint = mSentinelData;
      mEphemData.secsOfDayForFirstEphemPoint = mSentinelData;
      mEphemData.timeIntervalBetweenPoints_SEC = mSentinelData;
      
      for (int j = 0; j < 6; j++)
         mEphemData.firstStateVector_DULT[j] = mSentinelData;
   }
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteDataSegment() leavng\n");
   #endif
   return true;
}

//---------------------------------
// protected methods
//---------------------------------

//------------------------------------------------------------------------------
// void PpackHeader1Info()
//------------------------------------------------------------------------------
void Code500EphemerisFile::InitializeHeader1()
{
   #ifdef DEBUG_HEADRES
   MessageInterface::ShowMessage("InitializeHeader1() entered\n");
   #endif
   
   // Initialize all character arrays
   BlankOut(mEphemHeader1.productId, 8);
   BlankOut(mEphemHeader1.tapeId, 8);
   BlankOut(mEphemHeader1.sourceId, 8);
   BlankOut(mEphemHeader1.headerTitle, 56);
   BlankOut(mEphemHeader1.coordSystemIndicator1, 4);
   BlankOut(mEphemHeader1.orbitTheory, 8);
   BlankOut(mEphemHeader1.spares1, 16);
   BlankOut(mEphemHeader1.atmosphericDensityModel, 8);
   BlankOut(mEphemHeader1.spares2, 8);
   BlankOut(mEphemHeader1.spares3, 48);
   BlankOut(mEphemHeader1.spares4, 40);
   BlankOut(mEphemHeader1.spares5, 480);
   BlankOut(mEphemHeader1.spares6, 660);
   BlankOut(mEphemHeader1.harmonicsWithTitles1, 456);
   
   CopyString(mEphemHeader1.productId, mProductId, 8);
   mEphemHeader1.satId = mSatId;
   CopyString(mEphemHeader1.tapeId, mTapeId, 8);
   CopyString(mEphemHeader1.sourceId, mSourceId, 8);
   mEphemHeader1.timeSystemIndicator = mOutTimeSystemIndicator;
   mEphemHeader1.refTimeForDUT_YYMMDD = mRefTimeForDUT_YYMMDD;
   CopyString(mEphemHeader1.coordSystemIndicator1, mCoordSystem, 4); // "2000" for J2000
   mEphemHeader1.coordSystemIndicator2 = 4; // 2 = Mean of 1950, 3 = True of reference, 4 = J2000
   
   std::string str = mEphemHeader1.tapeId;
   #ifdef DEBUG_HEADRES
   MessageInterface::ShowMessage("tapeId = '%s'\n", str.substr(0,8).c_str());
   #endif
   
   str = mEphemHeader1.sourceId;
   #ifdef DEBUG_HEADRES
   MessageInterface::ShowMessage("sourceId = '%s'\n", str.substr(0,8).c_str());
   #endif
   
   // Set Orbit Theory to COWELL
   str = "COWELL  ";
   CopyString(mEphemHeader1.orbitTheory, str, 8);
   
   #ifdef DEBUG_HEADRES   
   MessageInterface::ShowMessage("InitializeHeader1() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void InitializeHeader2()
//------------------------------------------------------------------------------
void Code500EphemerisFile::InitializeHeader2()
{
   #ifdef DEBUG_HEADRES
   MessageInterface::ShowMessage("InitializeHeader2() entered\n");
   #endif
   
   BlankOut(mEphemHeader2.harmonicsWithTitles2, RECORD_SIZE);
   
   #ifdef DEBUG_HEADRES
   MessageInterface::ShowMessage("InitializeHeader2() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void SetEphemerisStartTime(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
/**
 * Sets start time of ephemeris in the header record 1.
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetEphemerisStartTime(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::SetEphemerisStartTime() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   
   double yyymmdd, hhmmss;
   ToYYYMMDDHHMMSS(a1Mjd, yyymmdd, hhmmss);
   
   double doy = ToDayOfYear(a1Mjd);
   double secsOfDay = ToSecondsOfDay(a1Mjd);
   
   mEphemHeader1.startDateOfEphem_YYYMMDD = yyymmdd;
   mEphemHeader1.startDayCountOfYear = doy;
   mEphemHeader1.startSecondsOfDay = secsOfDay;
   double startDut = ToDUT(a1Mjd);
   mEphemHeader1.startTimeOfEphemeris_DUT = startDut;
   
   // Should I set initiation time to start time of ephemeris?
   mEphemHeader1.dateOfInitiationOfEphemComp_YYYMMDD = yyymmdd;
   mEphemHeader1.timeOfInitiationOfEphemComp_HHMMSS = hhmmss;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::SetEphemerisStartTime() leaving, "
       "mEphemHeader1.startTimeOfEphemeris_DUT=%f\n", startDut);
   #endif
}


//------------------------------------------------------------------------------
// void SetEphemerisEndTime(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
/**
 * Sets end time of ephemeris in the header record 1.
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetEphemerisEndTime(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::SetEphemerisEndTime() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   
   double yyymmdd = ToYYYMMDD(a1Mjd);
   double doy = ToDayOfYear(a1Mjd);
   double secsOfDay = ToSecondsOfDay(a1Mjd);
   
   mEphemHeader1.endDateOfEphem_YYYMMDD = yyymmdd;
   mEphemHeader1.endDayCountOfYear = doy;
   mEphemHeader1.endSecondsOfDay = secsOfDay;
   double endDut = ToDUT(a1Mjd);
   mEphemHeader1.endTimeOfEphemeris_DUT = endDut;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::SetEphemerisEndTime() leaving, mEphemHeader1.endTimeOfEphemeris_DUT=%f\n", endDut);
   #endif
}


//------------------------------------------------------------------------------
// void SetTimeIntervalBetweenPoints(double secs)
//------------------------------------------------------------------------------
/**
 * Sets time interval between ephemeris points.
 *
 * @param secs  Time interval in secconds (-999.999 indicates variable interval)
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetTimeIntervalBetweenPoints(double secs)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("SetTimeIntervalBetweenPoints() entered, secs=%f\n", secs);
   #endif
   
   if (secs == -999.999)
   {
      mTimeIntervalBetweenPointsSecs = 0.0;//@note What should be the default value?
      mEphemHeader1.outputIntervalIndicator = 2; // 1 = fixed step, 2 = variable step
   }
   else
   {
      mTimeIntervalBetweenPointsSecs = secs;
      mEphemHeader1.outputIntervalIndicator = 1; // 1 = fixed step, 2 = variable step
   }
   
   mEphemHeader1.stepSize_SEC = mTimeIntervalBetweenPointsSecs;
   mEphemHeader1.timeIntervalBetweenPoints_DUT = mTimeIntervalBetweenPointsSecs * SEC_TO_DUT;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetTimeIntervalBetweenPoints() leaving, mTimeIntervalBetweenPointsSecs=%f\n",
       mTimeIntervalBetweenPointsSecs);
   #endif
}


//------------------------------------------------------------------------------
// void SetInitialEpoch(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetInitialEpoch(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetInitialEpoch() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   
   A1Mjd tempA1Mjd = a1Mjd;
   double epochDUT = ToDUT(a1Mjd);
   Real year, month, day, hour, min, sec;
   A1Date a1Date = tempA1Mjd.ToA1Date();
   a1Date.ToYearMonthDayHourMinSec(year, month, day, hour, min, sec);
   
   mEphemHeader1.epochTimeOfElements_DUT = epochDUT;
   mEphemHeader1.yearOfEpoch_YYY = year - 1900;
   mEphemHeader1.monthOfEpoch_MM = month;
   mEphemHeader1.dayOfEpoch_DD = day;
   mEphemHeader1.hourOfEpoch_HH = hour;
   mEphemHeader1.minuteOfEpoch_MM = min;
   mEphemHeader1.secondsOfEpoch_MILSEC = sec * 1000.0;
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetInitialEpoch() leaving, epochDUT=%f\n", epochDUT);
   #endif
}


//------------------------------------------------------------------------------
// void SetInitialCartesianState(const Rvector6 &cartState)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetInitialCartesianState(const Rvector6 &cartState)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetInitialCartesianState() entered, cartState[0]=%f\n", cartState[0]);
   #endif
   
   Rvector6 cartStateDULT = cartState;
   for (int i = 0; i < 3; i++)
      cartStateDULT[i] = cartState[i] * KM_TO_DUL;
   for (int i = 3; i < 6; i++)
      cartStateDULT[i] = cartState[i] * KM_SEC_TO_DUL_DUT;
   
   for (int i = 0; i < 6; i++)
      mEphemHeader1.cartesianElementsAtEpoch_DULT[i] = cartStateDULT[i];
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetKeplerianElements() leaving, cartStateDULT[0]=%f\n", cartStateDULT[0]);
   #endif
}


//------------------------------------------------------------------------------
// void SetInitialKeplerianState(const Rvector6 &kepState)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetInitialKeplerianState(const Rvector6 &kepState)
{
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetInitialKeplerianState() entered, kepState[2]=%f\n", kepState[2]);
   #endif
   
   Rvector6 kepStateRad = kepState;
   // Keplerian elements:
   // [SMA, ECC, INC, RAAN, AOP, TA]
   // @todo It should write MA instead of TA
   for (int i = 2; i < 6; i++)
      kepStateRad[i] = kepState[i] * GmatMathConstants::RAD_PER_DEG;
   
   for (int i = 0; i < 6; i++)
      mEphemHeader1.keplerianElementsAtEpoch_RAD[i] = kepStateRad[i];
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("SetKeplerianElements() leaving, kepStateRad[2]=%f\n", kepStateRad[2]);
   #endif
}


//------------------------------------------------------------------------------
// void InitializeDataRecord()
//------------------------------------------------------------------------------
void Code500EphemerisFile::InitializeDataRecord()
{
   // Set to no thrust
   mEphemData.thrustIndicator = 0.0;
   // Blank out spares
   BlankOut(mEphemData.spares1, 344);
}


//------------------------------------------------------------------------------
// void UnpackHeader1()
//------------------------------------------------------------------------------
void Code500EphemerisFile::UnpackHeader1()
{
   MessageInterface::ShowMessage("======================================== Begin of Header1\n");
   
   std::string str(mEphemHeader1.productId);
   MessageInterface::ShowMessage("productId                           = '%s'\n", str.c_str());   
   MessageInterface::ShowMessage("satId                               = % f\n", mEphemHeader1.satId);
   MessageInterface::ShowMessage("timeSystemIndicator                 = % f\n", mEphemHeader1.timeSystemIndicator);
   MessageInterface::ShowMessage("StartDateOfEphem_YYYMMDD            = % f\n", mEphemHeader1.startDateOfEphem_YYYMMDD);
   MessageInterface::ShowMessage("startDayCountOfYear                 = % f\n", mEphemHeader1.startDayCountOfYear);
   MessageInterface::ShowMessage("startSecondsOfDay                   = % f\n", mEphemHeader1.startSecondsOfDay);
   MessageInterface::ShowMessage("endDateOfEphem_YYYMMDD              = % f\n", mEphemHeader1.endDateOfEphem_YYYMMDD);
   MessageInterface::ShowMessage("endDayCountOfYear                   = % f\n", mEphemHeader1.endDayCountOfYear);
   MessageInterface::ShowMessage("endSecondsOfDay                     = % f\n", mEphemHeader1.endSecondsOfDay);
   MessageInterface::ShowMessage("stepSize_SEC                        = % f\n", mEphemHeader1.stepSize_SEC);
   
   std::string tapeIdStr = mEphemHeader1.tapeId;
   std::string sourceIdStr = mEphemHeader1.sourceId;
   std::string headerTitleStr = mEphemHeader1.headerTitle;
   std::string coordSystemStr = mEphemHeader1.coordSystemIndicator1;
   std::string orbitTheoryStr = mEphemHeader1.orbitTheory;
   MessageInterface::ShowMessage("tapeId                              = '%s'\n", tapeIdStr.substr(0,8).c_str());
   MessageInterface::ShowMessage("sourceId                            = '%s'\n", sourceIdStr.substr(0,8).c_str());
   MessageInterface::ShowMessage("headerTitle                         = '%s'\n", headerTitleStr.substr(0,56).c_str());
   MessageInterface::ShowMessage("centralBodyIndicator                = % f\n", mEphemHeader1.centralBodyIndicator);
   MessageInterface::ShowMessage("refTimeForDUT_YYMMDD                = % f\n", mEphemHeader1.refTimeForDUT_YYMMDD);
   MessageInterface::ShowMessage("coordSystemIndicator1               = '%s'\n", coordSystemStr.substr(0,4).c_str());
   MessageInterface::ShowMessage("coordSystemIndicator2               = %d\n", mEphemHeader1.coordSystemIndicator2);
   MessageInterface::ShowMessage("orbitTheory                         = '%s'\n", orbitTheoryStr.substr(0,8).c_str());
   MessageInterface::ShowMessage("timeIntervalBetweenPoints_DUT       = % f\n", mEphemHeader1.timeIntervalBetweenPoints_DUT);
   MessageInterface::ShowMessage("timeIntervalBetweenPoints_SEC.      = % f\n", mEphemHeader1.timeIntervalBetweenPoints_DUT * DUT_TO_SEC);
   MessageInterface::ShowMessage("outputIntervalIndicator             = %d\n", mEphemHeader1.outputIntervalIndicator);
   MessageInterface::ShowMessage("epochTimeOfElements_DUT             = % f\n", mEphemHeader1.epochTimeOfElements_DUT);
   MessageInterface::ShowMessage("epochTimeOfElements_DAY.            = % f\n", mEphemHeader1.epochTimeOfElements_DUT * DUT_TO_DAY);
   
   double dutTime = mEphemHeader1.epochTimeOfElements_DUT;
   std::string a1Greg = ToA1Gregorian(dutTime, (int)mInTimeSystemIndicator);
   MessageInterface::ShowMessage("a1Greg  = '%s'\n", a1Greg.c_str());
   std::string utcGreg = ToUtcGregorian(dutTime, (int)mInTimeSystemIndicator);
   MessageInterface::ShowMessage("utcGreg = '%s'\n", utcGreg.c_str());
   
   MessageInterface::ShowMessage("yearOfEpoch_YYY                     = % f\n", mEphemHeader1.yearOfEpoch_YYY);
   MessageInterface::ShowMessage("monthOfEpoch_MM                     = % f\n", mEphemHeader1.monthOfEpoch_MM);
   MessageInterface::ShowMessage("dayOfEpoch_DD                       = % f\n", mEphemHeader1.dayOfEpoch_DD);
   MessageInterface::ShowMessage("hourOfEpoch_HH                      = % f\n", mEphemHeader1.hourOfEpoch_HH);
   MessageInterface::ShowMessage("minuteOfEpoch_MM                    = % f\n", mEphemHeader1.minuteOfEpoch_MM);
   MessageInterface::ShowMessage("secondsOfEpoch_MILSEC               = % f\n", mEphemHeader1.secondsOfEpoch_MILSEC);
   
   MessageInterface::ShowMessage
      ("keplerianElementsAtEpoch_RAD[0]     = % f\n", mEphemHeader1.keplerianElementsAtEpoch_RAD[0]);
   for (int i = 1; i < 6; i++)
   {
      MessageInterface::ShowMessage
         ("keplerianElementsAtEpoch_RAD[%d]     = % f\n", i, mEphemHeader1.keplerianElementsAtEpoch_RAD[i]);
      MessageInterface::ShowMessage
         ("keplerianElementsAtEpoch_DEG[%d].    = % f\n", i, mEphemHeader1.keplerianElementsAtEpoch_RAD[i] * GmatMathConstants::DEG_PER_RAD);
   }
   for (int i = 0; i < 3; i++)
   {
      MessageInterface::ShowMessage
         ("cartesianElementsAtEpoch_DULT[%d]    = % f\n", i, mEphemHeader1.cartesianElementsAtEpoch_DULT[i]);
      MessageInterface::ShowMessage
         ("cartesianElementsAtEpoch_KMSE[%d].   = % f\n", i, mEphemHeader1.cartesianElementsAtEpoch_DULT[i] * DUL_TO_KM);
   }
   for (int i = 3; i < 6; i++)
   {
      MessageInterface::ShowMessage
         ("cartesianElementsAtEpoch_DULT[%d]    = % f\n", i, mEphemHeader1.cartesianElementsAtEpoch_DULT[i]);
      MessageInterface::ShowMessage
         ("cartesianElementsAtEpoch_KMSE[%d].   = % f\n", i, mEphemHeader1.cartesianElementsAtEpoch_DULT[i] * DUL_DUT_TO_KM_SEC);
   }
   
   MessageInterface::ShowMessage("startTimeOfEphemeris_DUT            = % f\n", mEphemHeader1.startTimeOfEphemeris_DUT);
   MessageInterface::ShowMessage("startTimeOfEphemeris_DAY.           = % f\n", mEphemHeader1.startTimeOfEphemeris_DUT * DUT_TO_DAY);
   MessageInterface::ShowMessage("endTimeOfEphemeris_DUT              = % f\n", mEphemHeader1.endTimeOfEphemeris_DUT);
   MessageInterface::ShowMessage("endTimeOfEphemeris_DAY.             = % f\n", mEphemHeader1.endTimeOfEphemeris_DUT * DUT_TO_DAY);
   MessageInterface::ShowMessage("timeIntervalBetweenPoints_DUT       = % f\n", mEphemHeader1.timeIntervalBetweenPoints_DUT);
   MessageInterface::ShowMessage("timeIntervalBetweenPoints_SEC.      = % f\n", mEphemHeader1.timeIntervalBetweenPoints_DUT * DUT_TO_SEC);
   MessageInterface::ShowMessage("dateOfInitiationOfEphemComp_YYYMMDD = % f\n", mEphemHeader1.dateOfInitiationOfEphemComp_YYYMMDD);
   MessageInterface::ShowMessage("timeOfInitiationOfEphemComp_HHMMSS  = % f\n", mEphemHeader1.timeOfInitiationOfEphemComp_HHMMSS);
   
   MessageInterface::ShowMessage("======================================== End of Header1\n");
}


//------------------------------------------------------------------------------
// void UnpackHeader2()
//------------------------------------------------------------------------------
void Code500EphemerisFile::UnpackHeader2()
{
   #ifdef DEBUG_UNPACK_HEADERS
   MessageInterface::ShowMessage
      ("======================================== Begin of Header2\n  ... todo ...");
   #endif
}


//------------------------------------------------------------------------------
// void UnpackDataRecord(int recNum, int logOption)
//------------------------------------------------------------------------------
/**
 * Unpacks data fields in the data record.
 *
 * @param  recNum         Data record number (must start from 3)
 * @param  logOption      = 0, no log
 *                        = 1, log first state vector of only first and last record
 *                        = 2, log first and last state vector of all records
 *                        = 3, log all state vectors of all records
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::UnpackDataRecord(int recNum, int logOption)
{
   if ((logOption > 1) || (logOption == 1 && recNum == 1) || (logOption == 1 && mSentinelsFound))
      MessageInterface::ShowMessage
         ("======================================== Begin of data record %d\n", recNum);
   
   double sentinels[50];
   sentinels[0] = mEphemData.dateOfFirstEphemPoint_YYYMMDD;
   sentinels[1] = mEphemData.dayOfYearForFirstEphemPoint;
   sentinels[2] = mEphemData.secsOfDayForFirstEphemPoint;
   sentinels[3] = mEphemData.timeIntervalBetweenPoints_SEC;
   mLastDataRecRead = recNum;
   
   if ((logOption > 1) || (logOption == 1 && recNum == 1) || (logOption == 1 && mSentinelsFound))
   {
      int year, month, day, hour, min;
      double sec;
      std::string ymdhmsStr =
         ToYearMonthDayHourMinSec(mEphemData.dateOfFirstEphemPoint_YYYMMDD,
                                  mEphemData.secsOfDayForFirstEphemPoint);
      
      MessageInterface::ShowMessage("timeOfFirstEphemPoint.          =  %s\n", ymdhmsStr.c_str());
      MessageInterface::ShowMessage("dateOfFirstEphemPoint_YYYMMDD   = % f\n", mEphemData.dateOfFirstEphemPoint_YYYMMDD);
      MessageInterface::ShowMessage("dayOfYearForFirstEphemPoint     = % f\n", mEphemData.dayOfYearForFirstEphemPoint);
      MessageInterface::ShowMessage("secsOfDayForFirstEphemPoint     = % f\n", mEphemData.secsOfDayForFirstEphemPoint);
      MessageInterface::ShowMessage("timeIntervalBetweenPoints_SEC   = % f\n", mEphemData.timeIntervalBetweenPoints_SEC);
   }
   
   // First position vector
   for (int j = 0; j < 3; j++)
   {
      sentinels[4+j] = mEphemData.firstStateVector_DULT[j];
      if ((logOption > 1) || (logOption == 1 && recNum == 1) || (logOption == 1 && mSentinelsFound))
         MessageInterface::ShowMessage("firstStateVector_DULT[%d]        = % 1.15e\n", j, mEphemData.firstStateVector_DULT[j]);
         MessageInterface::ShowMessage("firstStateVector_KMSE[%d].       = % 1.15e\n", j, mEphemData.firstStateVector_DULT[j] * DUL_TO_KM);
   }
   
   // First velocity vector
   for (int j = 3; j < 6; j++)
   {
      sentinels[7+j] = mEphemData.firstStateVector_DULT[j];
      if ((logOption > 1) || (logOption == 1 && recNum == 1) || (logOption == 1 && mSentinelsFound))
         MessageInterface::ShowMessage("firstStateVector_DULT[%d]        = % 1.15e\n", j, mEphemData.firstStateVector_DULT[j]);
         MessageInterface::ShowMessage("firstStateVector_KMSE[%d].       = % 1.15e\n", j, mEphemData.firstStateVector_DULT[j] * DUL_DUT_TO_KM_SEC);
   }
   
   // If sentinels already found, log data and return
   if (mSentinelsFound)
   {
      int i = mLastStateIndexRead;
      
      for (int j = 0; j < 3; j++)
      {
         if (logOption >= 2)
         {
            MessageInterface::ShowMessage
               ("stateVector2Thru50_DULT[%2d][%2d] = % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j]);
            MessageInterface::ShowMessage
               ("stateVector2Thru50_KMSE[%2d][%2d].= % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j] * DUL_TO_KM);
         }
      }
      
      for (int j = 3; j < 6; j++)
      {
         sentinels[j] = mEphemData.stateVector2Thru50_DULT[i][j];
         if (logOption > 2 || (logOption == 2 && i == mLastStateIndexRead))
         {
            MessageInterface::ShowMessage
               ("stateVector2Thru50_DULT[%2d][%2d] = % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j]);
            MessageInterface::ShowMessage
               ("stateVector2Thru50_KMSE[%2d][%2d].= % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j] * DUL_DUT_TO_KM_SEC);
         }
      }
      return;
   }

   
   // Check for sentinels
   int sentinelCount = 0;
   for (int k = 0; k < 10; k++)
   {
      if (GmatMathUtil::IsEqual(sentinels[k], mSentinelData, 10.0))
         sentinelCount++;
   }
   
   if (sentinelCount == 10)
   {
      #ifdef DEBUG_UNPACK_DATA
      MessageInterface::ShowMessage("First 10 sentinels found\n");
      #endif
      mSentinelsFound = true;
      mLastStateIndexRead = -1;
      return;
   }
   
   // Initially set last state index to 49
   mLastStateIndexRead = 49;
   
   // State vector 2 through 50
   for (int i = 1; i < 50; i++)
   {
      sentinelCount = 0;
      for (int j = 0; j < 3; j++)
      {
         sentinels[j] = mEphemData.stateVector2Thru50_DULT[i][j];
         if (logOption > 2 || (logOption == 2 && i == mLastStateIndexRead))
         {
            MessageInterface::ShowMessage
               ("stateVector2Thru50_DULT[%2d][%2d] = % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j]);
            MessageInterface::ShowMessage
               ("stateVector2Thru50_KMSE[%2d][%2d].= % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j] * DUL_TO_KM);
         }
      }
      
      for (int j = 3; j < 6; j++)
      {
         sentinels[j] = mEphemData.stateVector2Thru50_DULT[i][j];
         if (logOption > 2 || (logOption == 2 && i == mLastStateIndexRead))
         {
            MessageInterface::ShowMessage
               ("stateVector2Thru50_DULT[%2d][%2d] = % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j]);
            MessageInterface::ShowMessage
               ("stateVector2Thru50_KMSE[%2d][%2d].= % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j] * DUL_DUT_TO_KM_SEC);
         }
      }
      
      #ifdef DEBUG_UNPACK_DATA
      MessageInterface::ShowMessage("Checking for sentinels\n");
      #endif
      
      // Check for sentinels
      for (int k = 0; k < 6; k++)
      {
         if (GmatMathUtil::IsEqual(sentinels[k], mSentinelData, 1.0e-10))
            sentinelCount++;
      }
      
      #ifdef DEBUG_UNPACK_DATA
      MessageInterface::ShowMessage("sentinelCount = %d\n", sentinelCount);
      #endif
      
      if (sentinelCount > 5)
      {
         #ifdef DEBUG_UNPACK_DATA
         MessageInterface::ShowMessage("State sentinels found\n");
         #endif
         mSentinelsFound = true;
         mLastStateIndexRead = i - 1;
         break;
      }
      
      #ifdef DEBUG_UNPACK_DATA
      MessageInterface::ShowMessage("Checking for zero state vector\n");
      #endif
      
      // Check for zero vector
      sentinelCount = 0;
      for (int k = 0; k < 6; k++)
      {
         #ifdef DEBUG_UNPACK_DATA
         MessageInterface::ShowMessage("sentinels[%d] = % 1.15e\n", k, sentinels[k]);
         #endif
         // Check for zero state vector
         if (GmatMathUtil::IsZero(sentinels[k], 1e-12))
            sentinelCount++;
      }
      
      #ifdef DEBUG_UNPACK_DATA
      MessageInterface::ShowMessage("sentinelCount = %d\n", sentinelCount);
      #endif
      
      if (sentinelCount > 5)
      {
         #ifdef DEBUG_UNPACK_DATA
         MessageInterface::ShowMessage("State zero vector found\n");
         #endif
         mSentinelsFound = true;
         mLastStateIndexRead = i - 1;
         break;
      }
   }
   
   if ((logOption > 1) || (logOption == 1 && recNum == 1) || (logOption == 1 && mSentinelsFound))
   {
      MessageInterface::ShowMessage("timeOfFirstDataPoint_DUT        = % f\n", mEphemData.timeOfFirstDataPoint_DUT);
      MessageInterface::ShowMessage("timeIntervalBetweenPoints_DUT   = % f\n", mEphemData.timeIntervalBetweenPoints_DUT);
      MessageInterface::ShowMessage("thrustIndicator                 = % f\n", mEphemData.thrustIndicator);
      MessageInterface::ShowMessage
         ("======================================== End of data record %d\n", recNum);
   }
}


//------------------------------------------------------------------------------
// void ConvertStateKmSecToDULT(Rvector6 *kmsec, double *dult)
//------------------------------------------------------------------------------
void Code500EphemerisFile::ConvertStateKmSecToDULT(Rvector6 *kmsec, double *dult)
{
   // DUT = 864 seconds
   // DUL = 10000 km

   dult[0] = kmsec->Get(0) * KM_TO_DUL;
   dult[1] = kmsec->Get(1) * KM_TO_DUL;
   dult[2] = kmsec->Get(2) * KM_TO_DUL;
   dult[3] = kmsec->Get(3) * KM_SEC_TO_DUL_DUT;
   dult[4] = kmsec->Get(4) * KM_SEC_TO_DUL_DUT;
   dult[5] = kmsec->Get(5) * KM_SEC_TO_DUL_DUT;
}


//------------------------------------------------------------------------------
// void ConvertAsciiToEbcdic(char *ascii, char *ebcdic, int numChars)
//------------------------------------------------------------------------------
void Code500EphemerisFile::ConvertAsciiToEbcdic(char *ascii, char *ebcdic, int numChars)
{
   unsigned char asc;
   unsigned char ebc;
   
   for (int i = 0; i < numChars; i++)
   {
      asc = (unsigned char)ascii[i];
      ebc = AsciiToEbcdic(asc);
      ebcdic[i] = (char)ebc;
   }
}


//------------------------------------------------------------------------------
// void ConvertEbcdicToAscii(char *ebcdic, char *ascii, int numChars)
//------------------------------------------------------------------------------
void Code500EphemerisFile::ConvertEbcdicToAscii(char *ebcdic, char *ascii, int numChars)
{
   unsigned char asc;
   unsigned char ebc;
   
   for (int i = 0; i < numChars; i++)
   {
      ebc = (unsigned char)ebcdic[i];
      asc = EbcdicToAscii(ebc);
      ascii[i] = (char)asc;
   }
}


//------------------------------------------------------------------------------
// void ToYearMonthDayHourMinSec(double yyymmdd, double secsOfDay, ...
//------------------------------------------------------------------------------
void Code500EphemerisFile::ToYearMonthDayHourMinSec(double yyymmdd, double secsOfDay,
                              int &year, int &month, int &day,
                              int &hour, int &min, double &sec)
{
   ToYearMonthDay(yyymmdd, year, month, day);
   ToHMSFromSecondsOfDay(secsOfDay, hour, min, sec);
}


//------------------------------------------------------------------------------
// std::string ToYearMonthDayHourMinSec(double yyymmdd, double secsOfDay)
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToYearMonthDayHourMinSec(double yyymmdd, double secsOfDay)
{
   int year, month, day, hour, min;
   double sec;
   ToYearMonthDayHourMinSec(yyymmdd, secsOfDay, year, month, day, hour, min, sec);
   char buffer[30];
   sprintf(buffer, "%d-%02d-%02d %02d:%02d:%f", year, month, day, hour, min, sec);
   return std::string(buffer);
}


//------------------------------------------------------------------------------
// void ToYearMonthDay(double yyymmdd, int &year, int &month, int &day)
//------------------------------------------------------------------------------
void Code500EphemerisFile::ToYearMonthDay(double yyymmdd, int &year, int &month, int &day)
{
   double yyyymmdd = yyymmdd + 19000000.0;
   UnpackDate(yyyymmdd, year, month, day);
}


//------------------------------------------------------------------------------
// void ToYYYMMDDHHMMSS(const A1Mjd &a1Mjd, double &ymd, double &hms)
//------------------------------------------------------------------------------
void Code500EphemerisFile::ToYYYMMDDHHMMSS(const A1Mjd &a1Mjd, double &ymd, double &hms)
{
   A1Mjd tempA1Mjd = a1Mjd;
   A1Date a1Date = tempA1Mjd.ToA1Date();
   a1Date.ToYearMonthDayHourMinSec(ymd, hms);
}


//------------------------------------------------------------------------------
// double ToDUT(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToDUT(const A1Mjd &a1Mjd)
{
   double a1mjdReal = a1Mjd.GetReal();
   double dut = (a1mjdReal - mA1MjdOfDUTRef) * DAY_TO_DUT;
   return dut;
}


//------------------------------------------------------------------------------
// double ToYYYMMDD(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToYYYMMDD(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToYYYMMDD() entered, a1Mjd.GetReal()=% f\n", a1Mjd.GetReal());
   #endif
   
   A1Mjd tempA1Mjd = a1Mjd;
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double yyymmdd = a1Date.ToPackedYYYMMDD();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToYYYMMDD() returning % f\n", yyymmdd);
   #endif
   
   return yyymmdd;
}


//------------------------------------------------------------------------------
// double ToHHMMSS(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToHHMMSS(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToHHMMSS() entered, a1Mjd.GetReal()=% f\n", a1Mjd.GetReal());
   #endif
   
   A1Mjd tempA1Mjd = a1Mjd;
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double yyymmdd = a1Date.ToPackedHHMMSS();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToHHMMSS() returning % f\n", yyymmdd);
   #endif
   
   return yyymmdd;
}


//------------------------------------------------------------------------------
// double ToDayOfYear(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToDayOfYear(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToDayOfYear() entered, a1Mjd.GetReal()=% f\n", a1Mjd.GetReal());
   #endif
   
   A1Mjd tempA1Mjd = a1Mjd;
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double doy = a1Date.ToDayOfYear();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToDayOfYear() returning % f\n", doy);
   #endif
   
   return doy;
}


//------------------------------------------------------------------------------
// double ToSecondsOfDay(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToSecondsOfDay(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToSecondsOfDay() entered, a1Mjd.GetReal()=% f\n", a1Mjd.GetReal());
   #endif
   
   A1Mjd tempA1Mjd = a1Mjd;
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double secs = a1Date.GetSecondsOfDay();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToSecondsOfDay() returning % f\n", secs);
   #endif
   
   return secs;
}


//------------------------------------------------------------------------------
// A1Mjd  ToA1Mjd(double dutTime, int timeSystem = 1)
//------------------------------------------------------------------------------
/**
 *  Converts DUT time in given time system to A1Mjd.
 *
 * @param  dutTime     Input time in DUT unit(centiday)
 * @param  timeSystem  Input time system of input value
 *                     = 1, A.1
 *                     = 2, UTC
 */
//------------------------------------------------------------------------------
A1Mjd Code500EphemerisFile::ToA1Mjd(double dutTime, int timeSystem)
{
   Real timeOffset = 0.0;
   
   if (timeSystem == 1)
      timeOffset = 21.0 / GmatTimeConstants::SECS_PER_DAY;
   
   double a1mjdReal = (dutTime * DUT_TO_DAY) + mA1MjdOfDUTRef + timeOffset;
   A1Mjd a1Mjd = A1Mjd(a1mjdReal);
   return a1Mjd;
}


//------------------------------------------------------------------------------
// std::string ToA1Gregorian(double dutTime, int timeSystem = 2)
//------------------------------------------------------------------------------
/**
 * Converts DUT time to A1 Gregorian time system (no leap seconds)
 *
 * @param  dutTime  time in DUT unit (centiday)
 * @param  timeSystem  = 1, A.1
 *                     = 2, UTC
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToA1Gregorian(double dutTime, int timeSystem)
{
   // format 1 = "01 Jan 2000 11:59:28.000"
   //        2 = "2000-01-01T11:59:28.000"
   //Integer format = 1;
   A1Mjd tempA1Mjd = ToA1Mjd(dutTime, timeSystem);
   std::string epochStr = ToA1Gregorian(tempA1Mjd);
   return epochStr;
}


//------------------------------------------------------------------------------
// std::string ToA1Gregorian(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
/**
 * Converts A1 mod julian time to A1 Gregorian time system (no leap seconds)
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToA1Gregorian(const A1Mjd &a1Mjd)
{
   // format 1 = "01 Jan 2000 11:59:28.000"
   //        2 = "2000-01-01T11:59:28.000"
   #ifdef DEBUG_TIME_SYSTEM
   MessageInterface::ShowMessage("ToA1Gregorian() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   Integer format = 1;
   A1Mjd tempA1Mjd = a1Mjd;
   A1Date a1Date = tempA1Mjd.ToA1Date();
   GregorianDate gregorianDate(&a1Date, format);
   std::string epochStr = gregorianDate.GetDate();
   
   return epochStr;
}


//------------------------------------------------------------------------------
// std::string ToUtcGregorian(double dutTime, int timeSystem = 2)
//------------------------------------------------------------------------------
/**
 * Converts DUT time to UTC Gregorian time system.
 *
 * @param  dutTime  time in DUT unit (centiday)
 * @param  timeSystem  = 1, A.1
 *                     = 2, UTC
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToUtcGregorian(double dutTime, int timeSystem)
{
   int actualTimeSystem = timeSystem;
   // if time system is UTC, leap seconds is already applied
   if (timeSystem == 2)
      actualTimeSystem = 1;
   
   A1Mjd tempA1Mjd = ToA1Mjd(dutTime, actualTimeSystem);
   std::string epochStr = ToUtcGregorian(tempA1Mjd, timeSystem);
   return epochStr;
}


//------------------------------------------------------------------------------
// std::string ToUtcGregorian(const A1Mjd &a1Mjd, int timeSystem = 2)
//------------------------------------------------------------------------------
/**
 * Converts A1 mod julian time to UTC Gregorian time system (with leap seconds)
 *
 * @param  a1Mjd  time in A1 mod julian
 * @param  timeSystem  = 1, A.1
 *                     = 2, UTC
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToUtcGregorian(const A1Mjd &a1Mjd, int timeSystem)
{
   #ifdef DEBUG_TIME_SYSTEM
   MessageInterface::ShowMessage("ToUtcGregorian() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   
   // format 1 = "01 Jan 2000 11:59:28.000"
   //        2 = "2000-01-01T11:59:28.000"
   Integer format = 1;
   std::string epochStr;
   if (timeSystem == 1)
   {
      Real toMjd;
      Real epochInDays = a1Mjd.GetReal();
      
      // Need to apply leap seconds, so use TimeConverterUtil
      // Convert current epoch to specified format
      TimeConverterUtil::Convert("A1ModJulian", epochInDays, "", "UTCGregorian",
                                 toMjd, epochStr, format);
      
      if (epochStr == "")
      {
         MessageInterface::ShowMessage
            ("**** ERROR **** Code500EphemerisFile::ToUtcGregorian() Cannot convert epoch %.10f "
             "to UTCGregorian\n", a1Mjd.GetReal());
         
         epochStr = "EpochError";
      }
   }
   else
   {
      // No extra leap seconds applied
      epochStr = ToA1Gregorian(a1Mjd);
   }
   
   return epochStr;
}


//------------------------------------------------------------------------------
// void CopyString(char *to, const std::string &from, int numChars)
//------------------------------------------------------------------------------
void Code500EphemerisFile::CopyString(char *to, const std::string &from, int numChars)
{
   #ifdef DEBUG_COPY_STRING
   MessageInterface::ShowMessage
      ("CopyString() entered, from='%s', numChars=%d\n", from.c_str(), numChars);
   #endif
   
   int fromSize = from.size();
   
   for (int i = 0; i < fromSize; i++)
      to[i] = from[i];
   
   if (fromSize < numChars)
      for (int i = fromSize; i < numChars; i++)
         to[i] = ' ';

   std::string toStr(to);

   #ifdef DEBUG_COPY_STRING
   MessageInterface::ShowMessagememcopoy
      ("   from = '%s'\n   to   = '%s'\n", from.c_str(), toStr.substr(0,numChars).c_str());
   MessageInterface::ShowMessage("CopyString() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void CopyString(char *to, char *from, int numChars)
//------------------------------------------------------------------------------
void Code500EphemerisFile::CopyString(char *to, char *from, int numChars)
{
   memcpy(to, from, numChars);
}


//------------------------------------------------------------------------------
// void BlankOut(char *str, int numChars)
//------------------------------------------------------------------------------
void Code500EphemerisFile::BlankOut(char *str, int numChars)
{
   for (int i = 0; i < numChars; i++)
      str[i] = ' ';
}


//------------------------------------------------------------------------------
// unsigned char AsciiToEbcdic(unsigned char ascii)
//------------------------------------------------------------------------------
unsigned char Code500EphemerisFile::AsciiToEbcdic(unsigned char ascii)
{
    unsigned char ebcd;
    static unsigned char ascToEbcTable[256]=
    {
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x4F,0x7F,0x7B,0x5B,0x6C,0x50,0x7D,    /*  !"#$%&' */
       0x4D,0x5D,0x5C,0x4E,0x6B,0x60,0x4B,0x61,    /* ()*+,-./ */
       0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,    /* 01234567 */
       0xF8,0xF9,0x7A,0x5E,0x4C,0x7E,0x6E,0x6F,    /* 89:;<=>? */
       0x7C,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,    /* @ABCDEFG */
       0xC8,0xC9,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,    /* HIJKLMNO */
       0xD7,0xD8,0xD9,0xE2,0xE3,0xE4,0xE5,0xE6,    /* PQRSTUVW */
       0xE7,0xE8,0xE9,0x4A,0xE0,0x5A,0x5F,0x6D,    /* XYZ[\]^_ */
       0x79,0x81,0x82,0x83,0x84,0x85,0x86,0x87,    /* `abcdefg */
       0x88,0x89,0x91,0x92,0x93,0x94,0x95,0x96,    /* hijklmno */
       0x97,0x98,0x99,0xA2,0xA3,0xA4,0xA5,0xA6,    /* pqrstuvw */
       0xA7,0xA8,0xA9,0xC0,0x6A,0xD0,0xA1,0x40,    /* xyz{|}~  */
       0xB9,0xBA,0xED,0xBF,0xBC,0xBD,0xEC,0xFA,    /*          */
       0xCB,0xCC,0xCD,0xCE,0xCF,0xDA,0xDB,0xDC,    /*          */
       0xDE,0xDF,0xEA,0xEB,0xBE,0xCA,0xBB,0xFE,    /*          */
       0xFB,0xFD,0x7d,0xEF,0xEE,0xFC,0xB8,0xDD,    /*          */
       0x77,0x78,0xAF,0x8D,0x8A,0x8B,0xAE,0xB2,    /*          */
       0x8F,0x90,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0xAA,0xAB,0xAC,0xAD,0x8C,0x8E,0x80,0xB6,    /*          */
       0xB3,0xB5,0xB7,0xB1,0xB0,0xB4,0x76,0xA0,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,    /*          */
       0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40     /*          */
    };
    
    ebcd = ascToEbcTable[ascii];
    return (ebcd);
}


//------------------------------------------------------------------------------
// unsigned char EbcdicToAscii(unsigned char ebcd)
//------------------------------------------------------------------------------
unsigned char Code500EphemerisFile::EbcdicToAscii(unsigned char ebcd)
{
    unsigned char asc;
    
    static int ebcToAscTable[256]=
    {
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 91, 46, 60, 40, 43, 33,
            38,  32, 32, 32, 32, 32, 32, 32,
            32,  32, 93, 36, 42, 41, 59, 94,
            45,  47, 32, 32, 32, 32, 32, 32,
            32,  32,124, 44, 37, 95, 62, 63,
            32,  32, 32, 32, 32, 32,238,160,
            161, 96, 58, 35, 64, 39, 61, 34,
            230, 97, 98, 99,100,101,102,103,
            104,105,164,165,228,163,229,168,
            169,106,107,108,109,110,111,112,
            113,114,170,171,172,173,174,175,
            239,126,115,116,117,118,119,120,
            121,122,224,225,226,227,166,162,
            236,235,167,232,237,233,231,234,
            158,128,129,150,132,133,148,131,
            123, 65, 66, 67, 68, 69, 70, 71,
            72,  73,149,136,137,138,139,140,
            125, 74, 75, 76, 77, 78, 79, 80,
            81,  82,141,142,143,159,144,145,
            92,  32, 83, 84, 85, 86, 87, 88,
            89,  90,146,147,134,130,156,155,
            48,  49, 50, 51, 52, 53, 54, 55,
            56,  57,135,152,157,153,151, 32
    };
    
    asc = ebcToAscTable[ebcd];
    return (asc);
}


//------------------------------------------------------------------------------
// void SwapEndian(char *input, int numBytes)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SwapEndian(char *input, int numBytes)
{
   char *temp = NULL;
   
   temp = new char[numBytes];
   memcpy( temp, input, numBytes);
   for (int i = 0; i < numBytes; i++)
      input[(numBytes - 1) - i] = temp[i];
   
   delete [] temp;
}

//------------------------------------------------------------------------------
// void DebugWriteState(Rvector6 *stateKmSec, double *stateDULT int option = 1)
//------------------------------------------------------------------------------
void Code500EphemerisFile::DebugWriteState(Rvector6 *stateKmSec, double *stateDULT, int option)
{
   if (option == 1 || option == 2)
   {
      MessageInterface::ShowMessage
         ("stateKmSec = %g %g %g %g %g %g\n", stateKmSec->Get(0), stateKmSec->Get(1),
          stateKmSec->Get(2), stateKmSec->Get(3), stateKmSec->Get(4), stateKmSec->Get(5));
      if (option == 2)
         MessageInterface::ShowMessage
            ("stateDULT  = %g %g %g %g %g %g\n", stateDULT[0], stateDULT[1],
             stateDULT[2], stateDULT[3], stateDULT[4], stateDULT[5]);
   }
   else
      MessageInterface::ShowMessage
         ("stateDULT  = %g %g %g %g %g %g\n", stateDULT[0], stateDULT[1],
          stateDULT[2], stateDULT[3], stateDULT[4], stateDULT[5]);
}
