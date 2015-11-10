//$Id$
//------------------------------------------------------------------------------
//                                  Code500EphemerisFile
//------------------------------------------------------------------------------
#ifndef Code500EphemerisFile_hpp
#define Code500EphemerisFile_hpp

#include "gmatdefs.hpp"       // For type Byte
#include "A1Mjd.hpp"
#include <iostream>
#include <fstream>

class GMAT_API Code500EphemerisFile
{
public:
   Code500EphemerisFile(const std::string &fileName = "",
                        double satId = 123.0,
                        const std::string &timeSystem = "UTC",
                        const std::string &sourceId = "GMAT",
                        const std::string &centralBody = "Earth",
                        int fileMode = 2, int fileFormat = 1, 
                        int yearFormat = 1);
   
   ~Code500EphemerisFile();
   Code500EphemerisFile(const Code500EphemerisFile &ef);
   Code500EphemerisFile& operator=(const Code500EphemerisFile& ef);
   
   // methods for this class
   void Initialize();
   void Validate();
   bool OpenForRead(const std::string &filename, int fileFormat = 1,
                    int logOption = 0);
   bool OpenForWrite(const std::string &filename, int fileFormat = 1);
   void CloseForRead();
   void CloseForWrite();
   
   bool ReadHeader1(int logOption = 0);
   bool ReadHeader2(int logOption = 0);
   bool ReadDataAt(int dataRecNumber, int logOption = 0);
   bool ReadDataRecords(int numRecordsToRead = -999, int logOption = 0);
   
   void SetCentralBodyMu(double mu);
   void SetTimeIntervalBetweenPoints(double secs);
   void SetInitialEpoch(const A1Mjd &a1Mjd);
   void SetInitialState(Rvector6 *kmsec);
   void SetInitialCartesianState(const Rvector6 &cartState);
   void SetInitialKeplerianState(const Rvector6 &kepState);
   
   bool FinalizeHeaders();
   bool WriteHeader1();
   bool WriteHeader2();
   bool WriteDataAt(int recNumber);
   bool WriteDataSegment(const EpochArray &epochArray, const StateArray &stateArray, 
                         bool canFinalize = false);
   
   void SetSwapEndian(bool swapEndian, int fileMode);
   bool GetSwapEndian(int fileMode);
   double SwapDoubleEndian(double value);
   int    SwapIntegerEndian(int value);
   
   void ConvertAsciiToEbcdic(char *ascii, char *ebcdic, int numChars);
   void ConvertEbcdicToAscii(char *ebcdic, char *ascii, int numChars);
   
protected:
   static const int    RECORD_SIZE = 2800;
   static const int    NUM_STATES_PER_RECORD = 50;
   static const double DUL_TO_KM;
   static const double DUL_DUT_TO_KM_SEC;
   static const double KM_TO_DUL;
   static const double KM_SEC_TO_DUL_DUT;
   static const double SEC_TO_DUT;
   static const double DAY_TO_DUT;
   static const double DUT_TO_DAY;
   static const double DUT_TO_SEC;
   
#pragma pack(push, 1)
   struct GMAT_API EphemHeader1
   {
      char   productId[8];                          // 1-8
      double satId;                                 // 9-16
      double timeSystemIndicator;                   // 17-24
      double startDateOfEphem_YYYMMDD;              // 25-32
      double startDayCountOfYear;                   // 33-40
      double startSecondsOfDay;                     // 41-48
      double endDateOfEphem_YYYMMDD;                // 49-56
      double endDayCountOfYear;                     // 57-64
      double endSecondsOfDay;                       // 65-72
      double stepSize_SEC;                          // 73-80
      char   tapeId[8];                             // 81-88
      char   sourceId[8];                           // 89-96
      char   headerTitle[56];                       // 97-152
      double centralBodyIndicator;                  // 153-156
      double brouwerLyddame[6];                     // 161-208
      double refTimeForDUT_YYMMDD;                  // 209-216
      char   coordSystemIndicator1[4];              // 217-220
      int    coordSystemIndicator2;                 // 221-224
      char   orbitTheory[8];                        // 225-232
      char   spares1[16];                           // 233-248
      double dragCoefficient;                       // 249-256
      double scReflectivityConstant;                // 257-264
      char   atmosphericDensityModel[8];            // 265-272
      double areaOfSpacecraft;                      // 273-280
      double massOfSpacecraft;                      // 281-288
      double zonalTesseralHarmonicsIndicator;       // 289-296
      char   spares2[8];                            // 297-304
      double lunarGravPerturbIndicator;             // 305-312
      double solarRadiationPerturbIndicator;        // 313-320
      double solarGravPerturbIndicator;             // 321-328
      double atmosphericDragPerturbIndicator;       // 329-336
      double epochTimeOfElements_DUT;               // 337-344
      double yearOfEpoch_YYY;                       // 345-352
      double monthOfEpoch_MM;                       // 353-360
      double dayOfEpoch_DD;                         // 361-368
      double hourOfEpoch_HH;                        // 369-376
      double minuteOfEpoch_MM;                      // 377-384
      double secondsOfEpoch_MILSEC;                 // 385-392
      double keplerianElementsAtEpoch_RAD[6];       // 393-440
      double trueAnomalyAtEpoch;                    // 441-448
      double argOfLatitudeAtEpoch;                  // 449-456
      double flightPathAngleAtEpoch;                // 457-464
      double eccAnomalyAtEpoch;                     // 465-472
      double anomalisticPeriod_DUT;                 // 473-480
      double perigeeHeightAtEpoch;                  // 481-488
      double apogeeHeightAtEpoch;                   // 489-496
      double meanMotion;                            // 497-504
      double rateOfChangeOfArgOfPerigee;            // 505-512
      double reteOfChangeOfRAofAscendingNode;       // 513-520
      double cartesianElementsAtEpoch_DULT[6];      // 521-568
      double tSubQ[14];                             // 569-680
      char   spares3[48];                           // 681-728
      double rhoSub1;                               // 729-736
      double rhoSub2;                               // 737-744
      double rhoSub3;                               // 745-752
      double rhoSub4;                               // 753-760
      double brouwer1stOrderDragTerms[14];          // 761-872
      double brouwer2ndOrderDragTerms[14];          // 873-984
      char   spares4[40];                           // 985-1024
      double geocentricCoordOfSunAtEpoch[3];        // 1025-1048
      double totalNumberOfBrouwerDragTerms;         // 1049-1056
      char   spares5[480];                          // 1057-1536
      double startTimeOfEphemeris_DUT;              // 1537-1544
      double endTimeOfEphemeris_DUT;                // 1545-1552
      double timeIntervalBetweenPoints_DUT;         // 1553-1560
      double precessionNutationIndicator;           // 1561-1568
      double ghaAtEpoch;                            // 1569-1576
      double coordinateCenterIndicator;             // 1577-1584
      double dateOfInitiationOfEphemComp_YYYMMDD;   // 1585-1592
      double timeOfInitiationOfEphemComp_HHMMSS;    // 1593-1600
      double ghaAtEphemStart_RAD;                   // 1601-1608
      double ghaAtEphemerisEnd_RAD;                 // 1609-1616
      int    outputIntervalIndicator;               // 1617-1620
      int    leapSecondIndicator;                   // 1621-1624
      double dateOfLeapSeconds_YYYMMDD;             // 1625-1632
      double timeOfLeapSeconds_HHMMSS;              // 1633-1640
      double utcTimeAdjustment_SEC;                 // 1641-1648
      double dcObservationTimeSpan[4];              // 1649-1680
      int    trackingValidationIndicator;           // 1681-1684
      char   spares6[660];                          // 1685-2344
      char   harmonicsWithTitles1[456];             // 2345-2800
   };
   
   struct GMAT_API EphemHeader2
   {
      char   harmonicsWithTitles2[2800];              // 1-2800
   };
   
   struct GMAT_API EphemData
   {
      double dateOfFirstEphemPoint_YYYMMDD;           // 1-8
      double dayOfYearForFirstEphemPoint;             // 9-16
      double secsOfDayForFirstEphemPoint;             // 17-24
      double timeIntervalBetweenPoints_SEC;           // 25-32
      double firstStateVector_DULT[6];                // 33-80
      double stateVector2Thru50_DULT[49][6];          // 81-2432
      double timeOfFirstDataPoint_DUT;                // 2433-2440
      double timeIntervalBetweenPoints_DUT;           // 2441-2448
      double thrustIndicator;                         // 2449-2456
      char   spares1[344];                            // 2457-2800
   };
   
   // For swaping endianness
   struct DoubleByteType
   {
      unsigned byte1 : 8;
      unsigned byte2 : 8;
      unsigned byte3 : 8;
      unsigned byte4 : 8;
      unsigned byte5 : 8;
      unsigned byte6 : 8;
      unsigned byte7 : 8;
      unsigned byte8 : 8;
   };
   
   union DoubleUnionType
   {
      DoubleByteType doubleBytes;
      double         doubleValue;
   };
   
   struct IntByteType
   {
      unsigned byte1 : 8;
      unsigned byte2 : 8;
      unsigned byte3 : 8;
      unsigned byte4 : 8;
   };
   
   union IntUnionType
   {
      IntByteType intBytes;
      int         intValue;
   };
   
#pragma pack(pop)

   // Header_1 information
   double         mSatId;
   double         mInputTimeSystem;  // 1 = A1, 2 = UTC
   double         mOutputTimeSystem; // 1 = A1, 2 = UTC
   double         mCentralBodyIndicator; 
   double         mPrecNutIndicator; // hardcoded to 1
   std::string    mProductId;
   std::string    mTapeId;
   std::string    mSourceId;
   std::string    mCentralBody;
   std::string    mTimeSystem;
   std::string    mCoordSystem;
   
   // Header and data records
   EphemHeader1   mEphemHeader1;
   EphemHeader2   mEphemHeader2;
   EphemData      mEphemData;
   int            mDataRecWriteCounter;
   int            mLastDataRecRead;
   int            mLastStateIndexRead;
   std::string    mLastDataRecStartGreg;
   std::string    mLastDataRecEndGreg;
   
   // Epoch and state buffer
   EpochArray     mA1MjdArray;
   StateArray     mStateArray;
   
   // Sentinel data and flag
   double         mSentinelData;
   bool           mSentinelsFound;
   
   // DUT reference date
   std::string    mGregorianOfDUTRef;
   double         mRefTimeForDUT_YYMMDD;
   double         mMjdOfDUTRef;
   
   // Time information
   double mTimeIntervalBetweenPointsSecs;
   double mLeapSecsStartOutput;
   double mLeapSecsEndOutput;
   double mLeapSecsInput;
   
   // For cartesian to keplerian state conversion
   double mCentralBodyMu;
   
   // File mode, format, and name (read or write)
   int  mFileMode;  // 1 = input, 2 = output
   int  mInputFileFormat;
   int  mOutputFileFormat;
   std::string mInputFileName;
   std::string mOutputFileName;
   
   // File input/output streams
   std::ifstream  mEphemFileIn;
   std::ofstream  mEphemFileOut;
   
   // YearMonthDay format (1 = YYY, 2 = YYYY)
   int  mInputYearFormat;
   int  mOutputYearFormat;
   
   // Endianness
   bool mSwapInputEndian;
   bool mSwapOutputEndian;
   DoubleUnionType mDoubleOriginBytes;
   DoubleUnionType mDoubleSwappedBytes;
   IntUnionType mIntOriginBytes;
   IntUnionType mIntSwappedBytes;
   
   // Initialization
   void InitializeHeaderRecord1();
   void InitializeHeaderRecord2();
   void InitializeDataRecord();
   
   void SetEphemerisStartTime(const A1Mjd &a1Mjd);
   void SetEphemerisEndTime(const A1Mjd &a1Mjd);

   // Reading
   double ReadDoubleField(double *field);
   int    ReadIntegerField(int *field);
   
   // Writing
   void WriteDataRecord(bool canFinalize);
   void WriteDoubleField(double *field, double value);
   void WriteIntegerField(int *field, int value);
   
   // Data buffering
   void ClearBuffer();
   
   // Unpacking
   void UnpackHeader1();
   void PackHeader2();
   void UnpackHeader2();
   void UnpackDataRecord(int recNum, int logOption);
   
   // Unit Conversion
   void ConvertStateKmSecToDULT(Rvector6 *kmsec, double *dult);
   
   // Time Conversion
   void   ToYearMonthDayHourMinSec(double yyymmdd, double secsOfDay,
                                   int &year, int &month, int &day,
                                   int &hour, int &min, double &sec);
   void   ToYearMonthDay(double yyymmdd, int &year, int &month, int &day);
   void   ToYYYMMDDHHMMSS(double mjd, double &ymd, double &hms);
   double ToDUT(double mjd);
   double ToUtcModJulian(const A1Mjd &a1Mjd);
   double ToYYYMMDD(double mjd);
   double ToHHMMSS(double mjd);
   double ToDayOfYear(double mjd);
   double ToSecondsOfDay(double mjd);
   A1Mjd  ToA1Mjd(double dutTime, bool forOutput = true);
   std::string ToA1Gregorian(double dutTime, bool forOutput = true);
   std::string ToA1Gregorian(const A1Mjd &a1Mjd);
   std::string ToUtcGregorian(double dutTime, bool forOutput = true);
   std::string ToUtcGregorian(const A1Mjd &a1Mjd, bool forOutput = true);
   std::string ToYearMonthDayHourMinSec(double yyymmdd, double secsOfDay);
   
   // String functions
   void CopyString(char *to, const std::string &from, int numChars);
   void CopyString(char *to, char *from, int numChars);
   void BlankOut(char *str, int numChars);
   
   // Platform related
   unsigned char AsciiToEbcdic(unsigned char ascii);
   unsigned char EbcdicToAscii(unsigned char ebc);
   double SwapDouble();
   double SwapDoubleBytes(const Byte byte1, const Byte byte2, const Byte byte3,
                          const Byte byte4, const Byte byte5, const Byte byte6,
                          const Byte byte7, const Byte byte8);
   int SwapInteger();
   int SwapIntegerBytes(const Byte byte1, const Byte byte2, const Byte byte3, const Byte byte4);
   
   // Debug
   void DebugDouble(const std::string &fieldAndFormat, double value, bool swapEndian = false);
   void DebugDouble(const std::string &fieldAndFormat, int index, double value, bool swapEndian = false);
   void DebugDouble(const std::string &fieldAndFormat, int index1, int index2, double value, bool swapEndian = false);
   void DebugInteger(const std::string &fieldAndFormat, int value, bool swapEndian = false);
   void DebugWriteStateVector(double *stateDULT, int i, int numElem = 1, bool swapEndian = false);
   void DebugWriteState(Rvector6 *stateKmSec, double *stateDULT, int option = 1);
};

#endif // Code500EphemerisFile_hpp
