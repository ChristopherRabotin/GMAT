//$Id$
//------------------------------------------------------------------------------
//                                  Code500EphemerisFile
//------------------------------------------------------------------------------
#include "Code500EphemerisFile.hpp"
#include "TimeSystemConverter.hpp"   // For ConvertGregorianToMjd()
#include "StateConversionUtil.hpp"   // For Convert()
#include "Rvector6.hpp"
#include "DateUtil.hpp"              // For friend function UnpackDate(), ToHMSFromSecondsOfDay
#include "RealUtilities.hpp"         // For IsEqual()
#include "GregorianDate.hpp"         // For GregorianDate()
#include "GmatConstants.hpp"         // For GmatTimeConstants::A1_TAI_OFFSET
#include "UtilityException.hpp"      // for UtilityException
#include "MessageInterface.hpp"

//#define DEBUG_OPEN
//#define DEBUG_SET
//#define DEBUG_READ_DATA
//#define DEBUG_WRITE_HEADERS
//#define DEBUG_WRITE_DATA_SEGMENT
//#define DEBUG_WRITE_DATA_SEGMENT_MORE
//#define DEBUG_WRITE_DATA
//#define DEBUG_UNPACK_HEADERS
//#define DEBUG_UNPACK_DATA
//#define DEBUG_TIME_CONVERSION

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
/**
 * Constructor
 *
 * @param  fileName     Filename to be opened
 * @param  satId        Spacecraft ID [123.0]
 * @param  timeSystem   Time system used "UTC" or "A1" ["UTC"]
 * @param  sourceId     Source ID ["GMAT"]
 * @param  centralBody  Central body name used ["Earth"]
 * @param  fileMode     1 = input, 2 = output [2]
 * @param  fileFormat   1 = PC, 2 = UNIX [1]
 */
//------------------------------------------------------------------------------
Code500EphemerisFile::Code500EphemerisFile(const std::string &fileName, double satId,
                                           const std::string &timeSystem,
                                           const std::string &sourceId,
                                           const std::string &centralBody,
                                           int fileMode, int fileFormat)
{
   #ifdef DEBUG_CONSTRUCTOR
   MessageInterface::ShowMessage
      ("Code500EphemerisFile() entered\n   fileName='%s', satId=%f, timeSystem='%s', sourceId='%s', "
       "centralBody='%s'\n", fileName.c_str(), satId, timeSystem.c_str(), sourceId.c_str(), centralBody.c_str());
   #endif
   
   mSatId = satId;
   mTimeSystem = timeSystem;
   mSourceId = sourceId;
   mCentralBody = centralBody;
   mFileMode = fileMode;
   mInputFileFormat = 1;
   mOutputFileFormat = 1;
   mInputFileName = "";
   mOutputFileName = "";
   if (mFileMode == 1)
   {
      mInputFileName = fileName;
      mInputFileFormat = fileFormat;
   }
   else
   {
      mOutputFileName = fileName;
      mOutputFileFormat = fileFormat;
   }
   
   Initialize();
   
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
   mSatId = ef.mSatId;
   mTimeSystem = ef.mTimeSystem;
   mSourceId = ef.mSourceId;
   mCentralBody = ef.mCentralBody;
   mFileMode = ef.mFileMode;
   mInputFileFormat = ef.mInputFileFormat;
   mOutputFileFormat = ef.mOutputFileFormat;
   mInputFileName = ef.mInputFileName;
   mOutputFileName = ef.mOutputFileName;
   
   Initialize();
}


//------------------------------------------------------------------------------
// Code500EphemerisFile& operator=(const Code500EphemerisFile& ef)
//------------------------------------------------------------------------------
Code500EphemerisFile& Code500EphemerisFile::operator=(const Code500EphemerisFile& ef)
{
   if (this == &ef)
   {
      return *this;
   }
   else
   {
      mSatId = ef.mSatId;
      mTimeSystem = ef.mTimeSystem;
      mSourceId = ef.mSourceId;
      mCentralBody = ef.mCentralBody;
      mFileMode = ef.mFileMode;
      mInputFileFormat = ef.mInputFileFormat;
      mOutputFileFormat = ef.mOutputFileFormat;
      mInputFileName = ef.mInputFileName;
      mOutputFileName = ef.mOutputFileName;
      
      Initialize();
      return *this;
   }
}


//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
void Code500EphemerisFile::Initialize()
{
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("Code500EphemerisFile::Initialize() entered\n");
   #endif
   
   mProductId = "EPHEM   ";
   mCoordSystem = "2000";
   mTapeId = "STANDARD";
   
   // Set central body indicator
   // 1.0 = Earth,  2.0 = Moon,    3.0 = Sun,     4.0 = Mars,     5.0 = Jupiter, 6.0 = Saturn
   // 7.0 = Uranus, 8.0 = Neptune, 9.0 = Pluto, 10.0 = Mercury, 11.0 = Venus
   if (mCentralBody == "Earth")
      mCentralBodyIndicator = 1.0;
   if (mCentralBody == "Luna")
      mCentralBodyIndicator = 2.0;
   else if (mCentralBody == "Sun")
      mCentralBodyIndicator = 3.0;
   else if (mCentralBody == "Mars")
      mCentralBodyIndicator = 4.0;
   else if (mCentralBody == "Jupiter")
      mCentralBodyIndicator = 5.0;
   else if (mCentralBody == "Saturn")
      mCentralBodyIndicator = 6.0;
   else if (mCentralBody == "Uranus")
      mCentralBodyIndicator = 7.0;
   else if (mCentralBody == "Neptune")
      mCentralBodyIndicator = 8.0;
   else if (mCentralBody == "Pluto")
      mCentralBodyIndicator = 9.0;
   else if (mCentralBody == "Mercury")
      mCentralBodyIndicator = 10.0;
   else if (mCentralBody == "Venus")
      mCentralBodyIndicator = 11.0;
   else
      mCentralBodyIndicator = -99.99;
   
   // Time System Indicator: 0.0 = A.1, Atomic Time, 1.0 = UTC, Universal Time Coordinated
   mInputTimeSystem = 0.0;
   mOutputTimeSystem = 0.0;
   if (mTimeSystem == "A1")
      mOutputTimeSystem = 1.0;
   else if (mTimeSystem == "UTC")
      mOutputTimeSystem = 2.0;
   
   mDataRecWriteCounter = 2; // data record starts at 3
   mLastDataRecRead = 2;
   mLastStateIndexRead = -1;
   
   mSentinelData = 9.99999999999999e15;
   mSentinelsFound = false;
   mGregorianOfDUTRef = "18 Sep 1957 00:00:00.000";
   mRefTimeForDUT_YYMMDD = 570918.0;
   mMjdOfDUTRef = TimeConverterUtil::ConvertGregorianToMjd(mGregorianOfDUTRef);
   mTimeIntervalBetweenPointsSecs = 0.0;
   mLeapSecsStartOutput = 0.0;
   mLeapSecsEndOutput = 0.0;
   mLeapSecsInput = 0.0;
   
   //mCentralBodyMu = 0.0;
   //@todo Add GetOriginMu() to Spacecraft to get central body mu to
   // do cartesian to keplerian state conversion
   mCentralBodyMu = 398600.4415; // Set to earth mu for now for testing
   mSwapInputEndian = false;
   mSwapOutputEndian = false;
   
   // Fill in some header and data record with initial values
   InitializeHeaderRecord1();
   InitializeHeaderRecord2();
   InitializeDataRecord();
   
   // Open file
   if (mFileMode == 1)
      OpenForRead(mInputFileName, mInputFileFormat);
   else if (mFileMode == 2)
      OpenForWrite(mOutputFileName, mOutputFileFormat);
   
   #ifdef DEBUG_INIT
   MessageInterface::ShowMessage("Code500EphemerisFile::Initialize() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void Validate()
//------------------------------------------------------------------------------
/**
 * Validates header data. This method is usually called after Initialize.
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::Validate()
{
   // If file mode is writing, check for some header value
   if (mFileMode == 1)
   {
      // Check for uninitialized central body indicator
      if (mCentralBodyIndicator == -99.99)
         throw UtilityException
            ("Code 500 ephemeris header field error: central body indicator "
             "is uninitialized for the file \"" + mOutputFileName + "\"");
   }
}


//------------------------------------------------------------------------------
// bool OpenForRead(const std::string &fileName, int fileFormat = 1)
//------------------------------------------------------------------------------
/**
 * Opens ephemeris file for reading.
 *
 * @param  fileName  File name to be open for reading
 * @param  fileFormat  Input file format (1 = PC, 2 = UNIX)
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::OpenForRead(const std::string &fileName, int fileFormat)
{
   #ifdef DEBUG_OPEN
   MessageInterface::ShowMessage
      ("OpenForRead() entered, fileName='%s', fileFormat=%d\n", fileName.c_str(), fileFormat);
   #endif
   
   if (fileName == "")
   {
      #ifdef DEBUG_OPEN
      MessageInterface::ShowMessage
         ("OpenForRead() returning false, file name is blank\n");
      #endif
      return false;
   }
   
   if (mInputFileName != fileName)
   {
      // Close it first
      CloseForRead();      
      mEphemFileIn.open(fileName.c_str(), std::ios_base::binary);
      
      if (!mEphemFileIn.is_open())
         throw UtilityException("Cannot open code 500 ephemeris file \"" + fileName + "\" for reading");
      
      mInputFileName = fileName;
      mSwapInputEndian = false;
      if (fileFormat == 2)
         mSwapInputEndian = true;
   }
   
   #ifdef DEBUG_OPEN
   std::streampos fsize = mEphemFileIn.tellg();
   mEphemFileIn.seekg(0, std::ios::end);
   fsize = mEphemFileIn.tellg() - fsize;
   mEphemFileIn.seekg(std::ios_base::beg);
   MessageInterface::ShowMessage("OpenForRead() returning true, fsize=%d\n", fsize);
   #endif
   
   #ifdef DEBUG_OPEN
   MessageInterface::ShowMessage
      ("OpenForRead() returning true, ephemeris file successfully opened for reading\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool OpenForWrite(const std::string &fileName, int fileFormat = 1)
//------------------------------------------------------------------------------
/**
 * Opens ephemeris file for writing.
 *
 * @param  fileName  File name to be open for writing
 * @param  fileFormat  Output file format (1 = PC, 2 = UNIX)
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::OpenForWrite(const std::string &fileName, int fileFormat)
{
   #ifdef DEBUG_OPEN
   MessageInterface::ShowMessage
      ("OpenForWrite() entered, fileName='%s', fileFormat=%d\n", fileName.c_str(), fileFormat);
   #endif
   
   if (fileName == "")
   {
      #ifdef DEBUG_OPEN
      MessageInterface::ShowMessage
         ("OpenForWrite() returning false, file name is blank\n");
      #endif
      return false;
   }
   
   if (mInputFileName != fileName)
   {
      // Close it firat
      CloseForWrite();
      mEphemFileOut.open(fileName.c_str(), std::ios_base::binary);
      
      if (!mEphemFileOut.is_open())
         throw UtilityException("Cannot open code 500 ephemeris file \"" + fileName + "\" for writing");
      
      mOutputFileName != fileName;
      mSwapOutputEndian = false;
      if (fileFormat == 2)
         mSwapOutputEndian = true;
   }
   
   #ifdef DEBUG_OPEN
   MessageInterface::ShowMessage
      ("OpenForWrite() returning true, ephemeris file successfully opened for writing\n");
   #endif
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
      throw UtilityException("Cannot open code 500 ephemeris file \"" + mInputFileName + "\" for reading");
   
   // Read the first header
   mEphemFileIn.seekg(std::ios_base::beg);
   mEphemFileIn.read((char*)(&mEphemHeader1), RECORD_SIZE);
   
   // Save infile time system used
   mInputTimeSystem = mEphemHeader1.timeSystemIndicator;
   
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
      throw UtilityException("Cannot open code 500 ephemeris file \"" + mInputFileName + "\" for reading");
   
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
      throw UtilityException("Cannot open code 500 ephemeris file \"" + mInputFileName + "\" for reading");
   
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
      throw UtilityException("Cannot open code 500 ephemeris file \"" + mInputFileName + "\" for reading");
   
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
   mEphemFileOut.flush();
   
   #ifdef DEBUG_WRITE_HEADERS
   MessageInterface::ShowMessage
      ("mEphemHeader1.endDateOfEphem_YYYMMDD = %f\n", mEphemHeader1.endDateOfEphem_YYYMMDD);
   MessageInterface::ShowMessage
      ("mEphemHeader1.endTimeOfEphemeris_DUT = %f\n", mEphemHeader1.endTimeOfEphemeris_DUT);
   #endif
   
   #ifdef DEBUG_WRITE_HEADERS
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
// bool WriteDataSegment(const EpochArray &epochArray, const StateArray &stateArray, 
//                       bool canFinalize = false)
//------------------------------------------------------------------------------
/**
 * Handles writing state vector to data record.
 *
 * @param  epochArray  Array of epoch pointer
 * @param  stateArray  Array of state vector pointer
 *
 * @note It assumes the maximum number of state vector receiving is NUM_STATES_PER_RECORD (50).
 */
//------------------------------------------------------------------------------
bool Code500EphemerisFile::WriteDataSegment(const EpochArray &epochArray,
                                            const StateArray &stateArray, 
                                            bool canFinalize)
{
   Integer numPoints = stateArray.size();
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("============================================================\n"
       "Code500EphemerisFile::WriteDataSegment() entered, numPoints=%d, canFinalize=%d\n",
       numPoints, canFinalize);
   #endif
   
   if (numPoints == 0)
   {
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      MessageInterface::ShowMessage
         ("Code500EphemerisFile::WriteDataSegment() just returning true, data size is zero\n");
      #endif
      return true;
   }
   else if (numPoints > NUM_STATES_PER_RECORD)
   {
      UtilityException ue;
      ue.SetDetails("Code500EphemerisFile::WriteDataSegment() received too many state vector: %d, "
                    "The maximum it can handle is 50\n", numPoints);
      throw ue;
   }

   // For multiple segments, end epoch of previous segment may be the same as
   // beginning epoch of new segmnet, so check for duplicate epoch and use the
   // state of new epoch since any maneuver or some spacecraft property update
   // may happened.
   if (!mA1MjdArray.empty())
   {
      A1Mjd *end = mA1MjdArray.back();
      A1Mjd *newStart = epochArray.front();
      if (end->GetReal() == newStart->GetReal())
      {
         #ifdef DEBUG_WRITE_DATA_SEGMENT
         MessageInterface::ShowMessage
            ("Duplicate epoch found, so removing end dadta, end=%f, newStart=%f\n",
             end->GetReal(), newStart->GetReal());
         #endif
         
         // Remove the last dadta
         mA1MjdArray.pop_back();
         mStateArray.pop_back();
      }
   }
   
   // Fill the buffer
   for (int i = 0; i < numPoints; i++)
   {
      // Push cloned epoch and state since epoch and state POINTER array is cleared in
      // the EphemerisFile
      mA1MjdArray.push_back(epochArray[i]->Clone());
      mStateArray.push_back(stateArray[i]->Clone());
      
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      MessageInterface::ShowMessage("i=%2d, mA1MjdArray.size()=%2d\n", i, mA1MjdArray.size());
      #endif
      // If buffer is full, write data
      if ((int)mA1MjdArray.size() == NUM_STATES_PER_RECORD)
      {
         // Write data record and clear buffer
         bool isEndOfData = false;
         if (canFinalize && i == numPoints - 1)
            isEndOfData = true;
         WriteDataRecord(isEndOfData);
         ClearBuffer();
      }
   }
   
   // If final data segment received, write data
   if (canFinalize)
   {
      WriteDataRecord(canFinalize);
      ClearBuffer();
   }
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::WriteDataSegment() leavng, mA1MjdArray.size()=%d\n", mA1MjdArray.size());
   #endif
   return true;
}

//---------------------------------
// protected methods
//---------------------------------


//------------------------------------------------------------------------------
// void InitializeHeaderRecord1()
//------------------------------------------------------------------------------
void Code500EphemerisFile::InitializeHeaderRecord1()
{
   #ifdef DEBUG_HEADRES
   MessageInterface::ShowMessage("InitializeHeaderRecord1() entered\n");
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
   mEphemHeader1.timeSystemIndicator = mOutputTimeSystem;
   mEphemHeader1.refTimeForDUT_YYMMDD = mRefTimeForDUT_YYMMDD;
   CopyString(mEphemHeader1.coordSystemIndicator1, mCoordSystem, 4); // "2000" for J2000
   mEphemHeader1.coordSystemIndicator2 = 4; // 2 = Mean of 1950, 3 = True of reference, 4 = J2000
   mEphemHeader1.centralBodyIndicator = mCentralBodyIndicator;
   
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
   
   // Set leap second info
   mEphemHeader1.leapSecondIndicator = 1; // 1 = no leap sec occurrs, 2 = leap second occurrs
   mEphemHeader1.dateOfLeapSeconds_YYYMMDD = 0.0;
   mEphemHeader1.timeOfLeapSeconds_HHMMSS = 0.0;
   mEphemHeader1.utcTimeAdjustment_SEC = 0.0;
   
   #ifdef DEBUG_HEADRES   
   MessageInterface::ShowMessage("InitializeHeaderRecord1() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void InitializeHeaderRecord2()
//------------------------------------------------------------------------------
void Code500EphemerisFile::InitializeHeaderRecord2()
{
   #ifdef DEBUG_HEADRES
   MessageInterface::ShowMessage("InitializeHeaderRecord2() entered\n");
   #endif
   
   BlankOut(mEphemHeader2.harmonicsWithTitles2, RECORD_SIZE);
   
   #ifdef DEBUG_HEADRES
   MessageInterface::ShowMessage("InitializeHeaderRecord2() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// void InitializeDataRecord()
//------------------------------------------------------------------------------
void Code500EphemerisFile::InitializeDataRecord()
{
   // Set to no thrust
   mEphemData.thrustIndicator = 0.0;
   
   // Set all states to sentinels
   mEphemData.firstStateVector_DULT[0] = mSentinelData;
   mEphemData.firstStateVector_DULT[1] = mSentinelData;
   mEphemData.firstStateVector_DULT[2] = mSentinelData;
   mEphemData.firstStateVector_DULT[3] = mSentinelData;
   mEphemData.firstStateVector_DULT[4] = mSentinelData;
   mEphemData.firstStateVector_DULT[5] = mSentinelData;
   
   for (int i = 0; i < NUM_STATES_PER_RECORD - 1; i++)
   {
      for (int j = 0; j < 6; j++)
         mEphemData.stateVector2Thru50_DULT[i-1][j] = mSentinelData;
   }
   
   // Blank out spares
   BlankOut(mEphemData.spares1, 344);
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
   
   double startMjd = 0.0;
   if (mOutputTimeSystem == 1) // A1 time system
      startMjd = a1Mjd.GetReal();
   else  // UTC time system
      startMjd = ToUtcModJulian(a1Mjd);
   
   mLeapSecsStartOutput = TimeConverterUtil::NumberOfLeapSecondsFrom(startMjd);
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("mLeapSecsStartOutput = %f\n", mLeapSecsStartOutput);
   #endif
   
   double yyymmdd, hhmmss;
   ToYYYMMDDHHMMSS(startMjd, yyymmdd, hhmmss);
   
   double doy = ToDayOfYear(startMjd);
   double secsOfDay = ToSecondsOfDay(startMjd);
   
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage
      ("yyymmdd=%f, hhmmss=%f, doy=%f, secsOfDay=%f\n", yyymmdd, hhmmss, doy, secsOfDay);
   #endif
   
   mEphemHeader1.startDateOfEphem_YYYMMDD = yyymmdd;
   mEphemHeader1.startDayCountOfYear = doy;
   mEphemHeader1.startSecondsOfDay = secsOfDay;
   double startDut = ToDUT(startMjd);
   mEphemHeader1.startTimeOfEphemeris_DUT = startDut;
   
   // Should I set initiation time to start time of ephemeris?
   mEphemHeader1.dateOfInitiationOfEphemComp_YYYMMDD = yyymmdd;
   mEphemHeader1.timeOfInitiationOfEphemComp_HHMMSS = hhmmss;
   
   #ifdef DEBUG_SET
   std::string ymdhms = ToYearMonthDayHourMinSec
      (mEphemHeader1.startDateOfEphem_YYYMMDD, mEphemHeader1.startSecondsOfDay);
   MessageInterface::ShowMessage("startYYYYMMDDHHMMSSsss = %s\n", ymdhms.c_str());
   MessageInterface::ShowMessage("mEphemHeader1.startTimeOfEphemeris_DUT = %f\n", startDut);
   MessageInterface::ShowMessage("Code500EphemerisFile::SetEphemerisStartTime() leaving\n");
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
   
   double endMjd = 0.0;
   if (mOutputTimeSystem == 1) // A1 time system
      endMjd = a1Mjd.GetReal();
   else  // UTC time system
      endMjd = ToUtcModJulian(a1Mjd);
   
   // Leap seconds info
   mLeapSecsEndOutput = TimeConverterUtil::NumberOfLeapSecondsFrom(endMjd);
   #ifdef DEBUG_SET
   MessageInterface::ShowMessage("mLeapSecsEndOutput = %f\n", mLeapSecsEndOutput);
   #endif
   
   if ((mLeapSecsEndOutput - mLeapSecsStartOutput) > 0)
   {
      mEphemHeader1.leapSecondIndicator = 1;
      mEphemHeader1.utcTimeAdjustment_SEC = mLeapSecsEndOutput - mLeapSecsStartOutput;
      // @todo Compute time of leap second occurrs using LeapSecsFileReader
   }
   
   double yyymmdd = ToYYYMMDD(endMjd);
   double doy = ToDayOfYear(endMjd);
   double secsOfDay = ToSecondsOfDay(endMjd);
   
   mEphemHeader1.endDateOfEphem_YYYMMDD = yyymmdd;
   mEphemHeader1.endDayCountOfYear = doy;
   mEphemHeader1.endSecondsOfDay = secsOfDay;
   double endDut = ToDUT(endMjd);
   mEphemHeader1.endTimeOfEphemeris_DUT = endDut;
   
   #ifdef DEBUG_SET
   std::string ymdhms = ToYearMonthDayHourMinSec
      (mEphemHeader1.endDateOfEphem_YYYMMDD, mEphemHeader1.endSecondsOfDay);
   MessageInterface::ShowMessage("endYYYYMMDDHHMMSSsss. = %s\n", ymdhms.c_str());
   MessageInterface::ShowMessage("mEphemHeader1.endDateOfEphem_YYYMMDD = %f\n", mEphemHeader1.endDateOfEphem_YYYMMDD);
   MessageInterface::ShowMessage("mEphemHeader1.endTimeOfEphemeris_DUT = %f\n", mEphemHeader1.endTimeOfEphemeris_DUT);
   MessageInterface::ShowMessage("Code500EphemerisFile::SetEphemerisEndTime() leaving\n");
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
// void SetCentralBodyMu(double mu)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetCentralBodyMu(double mu)
{
   mCentralBodyMu = mu;
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
   
   std::string initialGreg;
   double initialMjd;
   
   if (mOutputTimeSystem == 1) // A1 time system
   {
      initialGreg = ToA1Gregorian(a1Mjd);
      initialMjd = a1Mjd.GetReal();
   }
   else  // UTC time system
   {
      initialGreg = ToUtcGregorian(a1Mjd);
      initialMjd = ToUtcModJulian(a1Mjd);
   }
   
   A1Mjd tempA1Mjd = initialMjd;
   double epochDUT = ToDUT(initialMjd);
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
// void SetInitialState(Rvector6 *kmsec)
//------------------------------------------------------------------------------
void Code500EphemerisFile::SetInitialState(Rvector6 *kmsec)
{
   Rvector6 cartState = *kmsec;
   SetInitialCartesianState(cartState);
   
   Rvector6 kepState =
      StateConversionUtil::Convert(mCentralBodyMu, cartState, "Cartesian", "Keplerian", "TA");
   SetInitialKeplerianState(kepState);
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
// void WriteDataRecord(bool canFinalize)
//------------------------------------------------------------------------------
void Code500EphemerisFile::WriteDataRecord(bool canFinalize)
{
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("==================================================\n"
       "Code500EphemerisFile::WriteDataRecord() entered, canFinalize=%d, "
       "mA1MjdArray.size()=%d, mStateArray.size()=%d\n", canFinalize,
       mA1MjdArray.size(), mStateArray.size());
   #endif
   
   int numPoints = mA1MjdArray.size();
   A1Mjd *start = mA1MjdArray.front();
   A1Mjd *end = mA1MjdArray.back();
   
   std::string startGreg;
   std::string endGreg;
   double startMjd = 0.0;
   double endMjd = 0.0;
   if (mOutputTimeSystem == 1) // A1 time system
   {
      startGreg = ToA1Gregorian(*start);
      endGreg = ToA1Gregorian(*end);
      startMjd = start->GetReal();
      endMjd = end->GetReal();
   }
   else  // UTC time system
   {
      startGreg = ToUtcGregorian(*start);
      endGreg = ToUtcGregorian(*end);
      startMjd = ToUtcModJulian(*start);
      endMjd = ToUtcModJulian(*end);
   }
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage
      ("start=%f '%s', end=%f '%s'\n", start->GetReal(), startGreg.c_str(), end->GetReal(), endGreg.c_str());
   #endif
   
   
   // Write data record
   mDataRecWriteCounter++;
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("mDataRecWriteCounter=%d\n", mDataRecWriteCounter);
   #endif
   
   // Set start date of ephemeris if first data record
   // Header1 = record1,  Header2 = record2,  Data1 = record3
   if (mDataRecWriteCounter == 3)
   {
      SetInitialEpoch(*start);
      SetInitialState(mStateArray[0]);
      SetEphemerisStartTime(*start);
      // Write initial headers
      WriteHeader1();
      WriteHeader2();
   }
   
   if (canFinalize)
      SetEphemerisEndTime(*end);
   
   mEphemData.dateOfFirstEphemPoint_YYYMMDD = ToYYYMMDD(startMjd);
   mEphemData.dayOfYearForFirstEphemPoint = ToDayOfYear(startMjd);
   mEphemData.secsOfDayForFirstEphemPoint = ToSecondsOfDay(startMjd);
   mEphemData.timeIntervalBetweenPoints_SEC = mTimeIntervalBetweenPointsSecs;
   
   // Write first state vector
   double stateDULT[6];
   ConvertStateKmSecToDULT(mStateArray[0], stateDULT);
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("----- State vector 1\n");
   DebugWriteState(mStateArray[0], stateDULT, 2);
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
   
   // Write state 2 through numPoints,
   // data record state vector index starts from 0 and ends at numPoints - 1
   for (int i = 1; i < numPoints; i++)
   {
      ConvertStateKmSecToDULT(mStateArray[i], stateDULT);      
      for (int j = 0; j < 6; j++)
         mEphemData.stateVector2Thru50_DULT[i-1][j] = stateDULT[j];
      
      #ifdef DEBUG_WRITE_DATA_SEGMENT_MORE
      MessageInterface::ShowMessage("----- State vector %d\n", i+1);
      DebugWriteState(mStateArray[i], stateDULT, 2);
      DebugWriteStateVector(mEphemData.stateVector2Thru50_DULT[i-1], i-1, 1);
      #endif
   }
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("Check if sentinel data needs to be written out\n");
   #endif
   // If data points received is less than NUM_STATES_PER_RECORD, write sentinel flag
   if ((numPoints < NUM_STATES_PER_RECORD) && canFinalize)
   {
      #ifdef DEBUG_WRITE_DATA_SEGMENT
      MessageInterface::ShowMessage
         ("===> Writing %d sentinel data\n", NUM_STATES_PER_RECORD-numPoints);
      #endif
      
      for (int i = numPoints - 1; i < NUM_STATES_PER_RECORD - 1; i++)
      {
         for (int j = 0; j < 6; j++)
            mEphemData.stateVector2Thru50_DULT[i][j] = mSentinelData;
         
         #ifdef DEBUG_WRITE_DATA_SEGMENT_MORE
         DebugWriteStateVector(mEphemData.stateVector2Thru50_DULT[i], i, 1);
         #endif
      }
   }
   
   // Write time of first data point and time interval in DUT
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("Writing time of first data point and time interval\n");
   #endif
   double timeInDUT = ToDUT(startMjd);
   mEphemData.timeOfFirstDataPoint_DUT = timeInDUT;
   mEphemData.timeIntervalBetweenPoints_SEC = mTimeIntervalBetweenPointsSecs;
   mEphemData.timeIntervalBetweenPoints_DUT = mTimeIntervalBetweenPointsSecs * SEC_TO_DUT;
   
   // Write data record
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("Calling WriteDataAt(%d)\n", mDataRecWriteCounter);
   #endif
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
   
   // Flush data to file
   mEphemFileOut.flush();
   
   #ifdef DEBUG_WRITE_DATA_SEGMENT
   MessageInterface::ShowMessage("Code500EphemerisFile::WriteDataRecord() leavng\n");
   #endif
}


//------------------------------------------------------------------------------
// void ClearBuffer()
//------------------------------------------------------------------------------
void Code500EphemerisFile::ClearBuffer()
{
   #ifdef DEBUG_CLEAR_BUFFER
   MessageInterface::ShowMessage
      ("Code500EphemerisFile::ClearBuffer() entered, mA1MjdArray.size()=%d, "
       "mStateArray.size()=%d\n", mA1MjdArray.size(), mStateArray.size());
   #endif
   
   EpochArray::iterator ei;
   for (ei = mA1MjdArray.begin(); ei != mA1MjdArray.end(); ++ei)
      delete (*ei);
   
   StateArray::iterator si;
   for (si = mStateArray.begin(); si != mStateArray.end(); ++si)
      delete (*si);
   
   mA1MjdArray.clear();
   mStateArray.clear();
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
   mInputTimeSystem = mEphemHeader1.timeSystemIndicator;
   MessageInterface::ShowMessage("StartDateOfEphem_YYYMMDD            = % f\n", mEphemHeader1.startDateOfEphem_YYYMMDD);
   MessageInterface::ShowMessage("startDayCountOfYear                 = % f\n", mEphemHeader1.startDayCountOfYear);
   MessageInterface::ShowMessage("startSecondsOfDay                   = % f\n", mEphemHeader1.startSecondsOfDay);
   MessageInterface::ShowMessage("endDateOfEphem_YYYMMDD              = % f\n", mEphemHeader1.endDateOfEphem_YYYMMDD);
   MessageInterface::ShowMessage("endDayCountOfYear                   = % f\n", mEphemHeader1.endDayCountOfYear);
   MessageInterface::ShowMessage("endSecondsOfDay                     = % f\n", mEphemHeader1.endSecondsOfDay);
   MessageInterface::ShowMessage("stepSize_SEC                        = % f\n", mEphemHeader1.stepSize_SEC);
   
   std::string ymdhms = ToYearMonthDayHourMinSec(mEphemHeader1.startDateOfEphem_YYYMMDD, mEphemHeader1.startSecondsOfDay);
   MessageInterface::ShowMessage("startYYYYMMDDHHMMSSsss.             = %s\n", ymdhms.c_str());
   ymdhms = ToYearMonthDayHourMinSec(mEphemHeader1.endDateOfEphem_YYYMMDD, mEphemHeader1.endSecondsOfDay);
   MessageInterface::ShowMessage("endYYYYMMDDHHMMSSsss.               = %s\n", ymdhms.c_str());
   
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
   std::string epochA1Greg = ToA1Gregorian(dutTime, false);
   MessageInterface::ShowMessage("epochA1Greg  = '%s'\n", epochA1Greg.c_str());
   std::string epochUtcGreg = ToUtcGregorian(dutTime, false);
   MessageInterface::ShowMessage("epochUtcGreg = '%s'\n", epochUtcGreg.c_str());
   
   MessageInterface::ShowMessage("yearOfEpoch_YYY                     = % f\n", mEphemHeader1.yearOfEpoch_YYY);
   MessageInterface::ShowMessage("monthOfEpoch_MM                     = % f\n", mEphemHeader1.monthOfEpoch_MM);
   MessageInterface::ShowMessage("dayOfEpoch_DD                       = % f\n", mEphemHeader1.dayOfEpoch_DD);
   MessageInterface::ShowMessage("hourOfEpoch_HH                      = % f\n", mEphemHeader1.hourOfEpoch_HH);
   MessageInterface::ShowMessage("minuteOfEpoch_MM                    = % f\n", mEphemHeader1.minuteOfEpoch_MM);
   MessageInterface::ShowMessage("secondsOfEpoch_MILSEC               = % f\n", mEphemHeader1.secondsOfEpoch_MILSEC);
   
   MessageInterface::ShowMessage
      ("keplerianElementsAtEpoch_RAD[0]     = % 1.15e\n", mEphemHeader1.keplerianElementsAtEpoch_RAD[0]);
   for (int i = 1; i < 6; i++)
   {
      MessageInterface::ShowMessage
         ("keplerianElementsAtEpoch_RAD[%d]     = % 1.15e\n", i, mEphemHeader1.keplerianElementsAtEpoch_RAD[i]);
      MessageInterface::ShowMessage
         ("keplerianElementsAtEpoch_DEG[%d].    = % 1.15e\n", i, mEphemHeader1.keplerianElementsAtEpoch_RAD[i] * GmatMathConstants::DEG_PER_RAD);
   }
   for (int i = 0; i < 3; i++)
   {
      MessageInterface::ShowMessage
         ("cartesianElementsAtEpoch_DULT[%d]    = % 1.15e\n", i, mEphemHeader1.cartesianElementsAtEpoch_DULT[i]);
      MessageInterface::ShowMessage
         ("cartesianElementsAtEpoch_KMSE[%d].   = % 1.15e\n", i, mEphemHeader1.cartesianElementsAtEpoch_DULT[i] * DUL_TO_KM);
   }
   for (int i = 3; i < 6; i++)
   {
      MessageInterface::ShowMessage
         ("cartesianElementsAtEpoch_DULT[%d]    = % 1.15e\n", i, mEphemHeader1.cartesianElementsAtEpoch_DULT[i]);
      MessageInterface::ShowMessage
         ("cartesianElementsAtEpoch_KMSE[%d].   = % 1.15e\n", i, mEphemHeader1.cartesianElementsAtEpoch_DULT[i] * DUL_DUT_TO_KM_SEC);
   }
   
   MessageInterface::ShowMessage("startTimeOfEphemeris_DUT            = % f\n", mEphemHeader1.startTimeOfEphemeris_DUT);
   MessageInterface::ShowMessage("startTimeOfEphemeris_DAY.           = % f\n", mEphemHeader1.startTimeOfEphemeris_DUT * DUT_TO_DAY);
   MessageInterface::ShowMessage("endTimeOfEphemeris_DUT              = % f\n", mEphemHeader1.endTimeOfEphemeris_DUT);
   MessageInterface::ShowMessage("endTimeOfEphemeris_DAY.             = % f\n", mEphemHeader1.endTimeOfEphemeris_DUT * DUT_TO_DAY);
   MessageInterface::ShowMessage("timeIntervalBetweenPoints_DUT       = % f\n", mEphemHeader1.timeIntervalBetweenPoints_DUT);
   MessageInterface::ShowMessage("timeIntervalBetweenPoints_SEC.      = % f\n", mEphemHeader1.timeIntervalBetweenPoints_DUT * DUT_TO_SEC);
   MessageInterface::ShowMessage("dateOfInitiationOfEphemComp_YYYMMDD = % f\n", mEphemHeader1.dateOfInitiationOfEphemComp_YYYMMDD);
   MessageInterface::ShowMessage("timeOfInitiationOfEphemComp_HHMMSS  = % f\n", mEphemHeader1.timeOfInitiationOfEphemComp_HHMMSS);
   MessageInterface::ShowMessage("utcTimeAdjustment_SEC               = % f\n", mEphemHeader1.utcTimeAdjustment_SEC);
   
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
   
   // Initially set last state index to 48, index starts from 0 to 48
   mLastStateIndexRead = 48;
   
   // State vector 2 through NUM_STATES_PER_RECORD
   for (int i = 0; i < NUM_STATES_PER_RECORD - 1; i++)
   {
      sentinelCount = 0;
      for (int j = 0; j < 3; j++)
         sentinels[j] = mEphemData.stateVector2Thru50_DULT[i][j];
      
      for (int j = 3; j < 6; j++)
         sentinels[j] = mEphemData.stateVector2Thru50_DULT[i][j];
      
      if (logOption > 2 || (logOption == 2 && i == mLastStateIndexRead))
         DebugWriteStateVector(mEphemData.stateVector2Thru50_DULT[i], i, 6);
      
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
   
   //#ifdef DEBUG_UNPACK_DATA
   MessageInterface::ShowMessage("mLastStateIndexRead = %d\n", mLastStateIndexRead);
   
   //#endif
   
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
// void ToYYYMMDDHHMMSS(double mjd, double &ymd, double &hms)
//------------------------------------------------------------------------------
void Code500EphemerisFile::ToYYYMMDDHHMMSS(double mjd, double &ymd, double &hms)
{
   A1Mjd tempA1Mjd(mjd);
   A1Date a1Date = tempA1Mjd.ToA1Date();
   a1Date.ToYearMonthDayHourMinSec(ymd, hms);
   // Since ymd is in yyyymmdd, subtract 19000000
   ymd = ymd - 19000000.0;
}


//------------------------------------------------------------------------------
// double ToDUT(double mjd)
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToDUT(double mjd)
{
   double dut = (mjd - mMjdOfDUTRef) * DAY_TO_DUT;
   return dut;
}


//------------------------------------------------------------------------------
// double ToUtcModJulian(const A1Mjd &a1Mjd)
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToUtcModJulian(const A1Mjd &a1Mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToUtcModJulian() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   
   Real a1MjdReal = a1Mjd.GetReal();
   
   //std::string epochStr;
   //Integer format = 1;
   // Need to apply leap seconds, so use TimeConverterUtil
   // Convert current epoch to specified format
   //TimeConverterUtil::Convert("A1ModJulian", mjd, "", "UTCModJulian",
   //                           utcMjd, epochStr, format);
   
   Real utcMjd = TimeConverterUtil::Convert(a1MjdReal, TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD);
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToUtcModJulian() returning utcMjd=%f\n", utcMjd);
   #endif
   return utcMjd;
}


//------------------------------------------------------------------------------
// double ToYYYMMDD(double mjd)
//------------------------------------------------------------------------------
/**
 * Converts mod julian days to YYYMMDD format. Assuming input mjd has
 * leap seconds already applied for UTC time system.
 *
 * @param  mjd  Modified julian days in A1 or UTC, or other time system
 */
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToYYYMMDD(double mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToYYYMMDD() entered, mjd=% f\n", mjd);
   #endif
   
   A1Mjd tempA1Mjd(mjd);
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double yyymmdd = a1Date.ToPackedYYYMMDD();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToYYYMMDD() returning % f\n", yyymmdd);
   #endif
   
   return yyymmdd;
}


//------------------------------------------------------------------------------
// double ToHHMMSS(double mjd)
//------------------------------------------------------------------------------
/**
 * Converts mod julian days to HHMMSS.mmm format. Assuming input mjd has
 * leap seconds already applied for UTC time system.
 *
 * @param  mjd  Modified julian days in A1 or UTC, or other time system
 */
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToHHMMSS(double mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToHHMMSS() entered, mjd=f\n", mjd);
   #endif
   
   A1Mjd tempA1Mjd(mjd);
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double hhmmss = a1Date.ToPackedHHMMSS();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToHHMMSS() returning % f\n", hhmmss);
   #endif
   
   return hhmmss;
}


//------------------------------------------------------------------------------
// double ToDayOfYear(double mjd)
//------------------------------------------------------------------------------
/**
 * Converts mod julian days to day of year. Assuming input mjd has
 * leap seconds already applied for UTC time system.
 *
 * @param  mjd  Modified julian days in A1 or UTC, or other time system
 */
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToDayOfYear(double mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToDayOfYear() entered, mjd=%f\n", mjd);
   #endif
   
   A1Mjd tempA1Mjd(mjd);
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double doy = a1Date.ToDayOfYear();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToDayOfYear() returning % f\n", doy);
   #endif
   
   return doy;
}


//------------------------------------------------------------------------------
// double ToSecondsOfDay(double mjd)
//------------------------------------------------------------------------------
/**
 * Converts mod julian days to seconds of day part. Assuming input mjd has
 * leap seconds already applied for UTC time system.
 *
 * @param  mjd  Modified julian days in A1 or UTC, or other time system
 */
//------------------------------------------------------------------------------
double Code500EphemerisFile::ToSecondsOfDay(double mjd)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToSecondsOfDay() entered, mjd=%f\n", mjd);
   #endif
   
   A1Mjd tempA1Mjd(mjd);
   A1Date a1Date = tempA1Mjd.ToA1Date();
   double secs = a1Date.GetSecondsOfDay();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToSecondsOfDay() returning % f\n", secs);
   #endif
   
   return secs;
}


//------------------------------------------------------------------------------
// A1Mjd  ToA1Mjd(double dutTime, bool forOutput = true)
//------------------------------------------------------------------------------
/**
 *  Converts DUT time in given time system to A1Mjd.
 *
 * @param  dutTime     Input time in DUT unit(centiday)
 * @param  forOutput   true if converting for writing
 */
//------------------------------------------------------------------------------
A1Mjd Code500EphemerisFile::ToA1Mjd(double dutTime, bool forOutput)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToA1Mjd() entered, dutTime=%f, forOutput=%d, mInputTimeSystem=%f\n",
       dutTime, forOutput, mInputTimeSystem);
   #endif
   
   Real timeOffset = 0.0;
   
   #if 0
   if (!forOutput && mInputTimeSystem == 2)
      timeOffset = 21.0 / GmatTimeConstants::SECS_PER_DAY;
   #endif
   
   double a1MjdReal = (dutTime * DUT_TO_DAY) + mMjdOfDUTRef + timeOffset;
   A1Mjd a1Mjd = A1Mjd(a1MjdReal);
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToA1Mjd() returning a1Mjd=%f for dutTime=%f\n", a1MjdReal, dutTime);
   #endif
   
   return a1Mjd;
}


//------------------------------------------------------------------------------
// std::string ToA1Gregorian(double dutTime, bool forOutput = true)
//------------------------------------------------------------------------------
/**
 * Converts DUT time to A1 Gregorian time system (no leap seconds)
 *
 * @param  dutTime  time in DUT unit (centiday)
 * @param  forOutput  true if converting for writing 
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToA1Gregorian(double dutTime, bool forOutput)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToA1Gregorian() entered, dutTime=%f, forOutput=%d\n", dutTime, forOutput);
   #endif
   
   A1Mjd tempA1Mjd = ToA1Mjd(dutTime, forOutput);
   std::string epochStr = ToA1Gregorian(tempA1Mjd);
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToA1Gregorian() returning '%s' for dutTime=%f\n", epochStr.c_str(), dutTime);
   #endif
   
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
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage("ToA1Gregorian() entered, a1Mjd=%f\n", a1Mjd.GetReal());
   #endif
   Integer format = 1;
   A1Mjd tempA1Mjd = a1Mjd;
   A1Date a1Date = tempA1Mjd.ToA1Date();
   GregorianDate gregorianDate(&a1Date, format);
   std::string epochStr = gregorianDate.GetDate();
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToA1Gregorian() returning '%s' for a1Mjd=%f\n", epochStr.c_str(), a1Mjd.GetReal());
   #endif
   
   return epochStr;
}


//------------------------------------------------------------------------------
// std::string ToUtcGregorian(double dutTime, bool forOutput = true)
//------------------------------------------------------------------------------
/**
 * Converts DUT time to UTC Gregorian time system.
 *
 * @param  dutTime  time in DUT unit (centiday)
 * @param  forOutput  true if converting for writing
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToUtcGregorian(double dutTime, bool forOutput)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToUtcGregorian() entered, dutTime=%f, forOutput=%d\n", dutTime, forOutput);
   #endif
   
   A1Mjd tempA1Mjd = ToA1Mjd(dutTime, forOutput);
   std::string epochStr = ToUtcGregorian(tempA1Mjd, forOutput);
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToUtcGregorian() returning '%s' for dutTime=%f\n", epochStr.c_str(), dutTime);
   #endif
   
   return epochStr;
}


//------------------------------------------------------------------------------
// std::string ToUtcGregorian(const A1Mjd &a1Mjd, bool forOutput = true)
//------------------------------------------------------------------------------
/**
 * Converts A1 mod julian time to UTC Gregorian time system (with leap seconds)
 *
 * @param  a1Mjd  time in A1 mod julian
 * @param  forOutput  true if converting for writing
 */
//------------------------------------------------------------------------------
std::string Code500EphemerisFile::ToUtcGregorian(const A1Mjd &a1Mjd, bool forOutput)
{
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToUtcGregorian() entered, a1Mjd=%f, forOutput=%d\n", a1Mjd.GetReal(), forOutput);
   #endif
   
   // format 1 = "01 Jan 2000 11:59:28.000"
   //        2 = "2000-01-01T11:59:28.000"
   Integer format = 1;
   std::string epochStr;
   if (forOutput)
   {
      Real utcMjd;
      Real epochInDays = a1Mjd.GetReal();
      
      // Need to apply leap seconds, so use TimeConverterUtil
      // Convert current epoch to specified format
      TimeConverterUtil::Convert("A1ModJulian", epochInDays, "", "UTCGregorian",
                                 utcMjd, epochStr, format);
      
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
   
   #ifdef DEBUG_TIME_CONVERSION
   MessageInterface::ShowMessage
      ("ToUtcGregorian() returning '%s' for a1Mjd=%f\n", epochStr.c_str(), a1Mjd.GetReal());
   #endif
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
// void DebugWriteStateVector(double *stateDULT, int i, int numElem = 1)
//------------------------------------------------------------------------------
void Code500EphemerisFile::DebugWriteStateVector(double *stateDULT, int i, int numElem)
{
   for (int j = 0; j < 3; j++)
   {
      if (numElem < j)
         break;
      
      MessageInterface::ShowMessage
         ("stateVector2Thru50_DULT[%2d][%2d] = % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j]);
      MessageInterface::ShowMessage
         ("stateVector2Thru50_KMSE[%2d][%2d].= % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j] * DUL_TO_KM);
   }
   
   for (int j = 3; j < 6; j++)
   {
      if (numElem < j)
         break;
      
      MessageInterface::ShowMessage
         ("stateVector2Thru50_DULT[%2d][%2d] = % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j]);
      MessageInterface::ShowMessage
         ("stateVector2Thru50_KMSE[%2d][%2d].= % 1.15e\n", i, j, mEphemData.stateVector2Thru50_DULT[i][j] * DUL_DUT_TO_KM_SEC);
   }
}


//------------------------------------------------------------------------------
// void DebugWriteState(Rvector6 *stateKmSec, double *stateDULT, int option = 1)
//------------------------------------------------------------------------------
/**
 * Write state in km/sec or in DULT.
 *
 * @param option 1 = Write only in km/sec
 *               2 = Write in km/sec and in DUL/DUT
 *               other = Write only in DUL/DUT
 */
//------------------------------------------------------------------------------
void Code500EphemerisFile::DebugWriteState(Rvector6 *stateKmSec,
                                           double *stateDULT, int option)
{
   if (option == 1 || option == 2)
   {
      MessageInterface::ShowMessage
         ("stateKmSec = % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e\n", 
          stateKmSec->Get(0), stateKmSec->Get(1), stateKmSec->Get(2),
          stateKmSec->Get(3), stateKmSec->Get(4), stateKmSec->Get(5));
      if (option == 2)
         MessageInterface::ShowMessage
            ("stateDULT  = % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e\n",
             stateDULT[0], stateDULT[1], stateDULT[2], stateDULT[3], stateDULT[4], stateDULT[5]);
   }
   else
      MessageInterface::ShowMessage
         ("stateDULT  = % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e % 1.15e\n", 
          stateDULT[0], stateDULT[1], stateDULT[2], stateDULT[3], stateDULT[4], stateDULT[5]);
}

