//$Id: GmatODType.cpp 1398 2013-06-04 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         GmatODType
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC
// Created: 2013/06/04
//
/**
 * ObType class used for the GMAT Internal observation data
 */
//------------------------------------------------------------------------------


#include "GmatODType.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "FileManager.hpp"
#include "StringUtil.hpp"
#include "MeasurementException.hpp"
#include <sstream>


//#define DEBUG_ODTYPE_CREATION_INITIALIZATION
//#define DEBUG_FILE_WRITE
//#define DEBUG_FILE_READ
//#define DEBUG_FILE_ACCESS

//-----------------------------------------------------------------------------
// GmatODType(const std::string withName)
//-----------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param withName The name of the new object
 */
//-----------------------------------------------------------------------------
GmatODType::GmatODType(const std::string withName) :
   ObType         ("GMAT_OD", withName),
   epochPrecision (16),
   dataPrecision  (6)
{
   #ifdef DEBUG_ODTYPE_CREATION_INITIALIZATION
	  MessageInterface::ShowMessage("GmatODType default constructor (a GMAT_OD obtype) <%s,%p>\n", GetName().c_str(), this);
   #endif

   header = "% GMAT OD Measurement Data File\n\n";
}

//-----------------------------------------------------------------------------
// ~GmatODType()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
GmatODType::~GmatODType()
{
}


//-----------------------------------------------------------------------------
// GmatODType(const GmatODType& ot) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ot The GmatODType that gets copied to this one
 */
//-----------------------------------------------------------------------------
GmatODType::GmatODType(const GmatODType& ot) :
   ObType         (ot),
   epochPrecision (ot.epochPrecision),
   dataPrecision  (ot.dataPrecision)
{
   #ifdef DEBUG_ODTYPE_CREATION_INITIALIZATION
	  MessageInterface::ShowMessage("GmatODType copy constructor (a GMAT_OD obtype) from <%s,%p> to <%s,%p>\n", ot.GetName().c_str(), &ot, GetName().c_str(), this);
   #endif
}


//-----------------------------------------------------------------------------
// GmatODType& operator=(const GmatODType& ot)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ot The GmatODType that gets copied to this one
 *
 * @return This GmatODType, configured to match ot
 */
//-----------------------------------------------------------------------------
GmatODType& GmatODType::operator=(const GmatODType& ot)
{
   #ifdef DEBUG_ODTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Assigning one GMAT_OD obtype to another\n");
   #endif

   if (this != &ot)
   {
      epochPrecision = ot.epochPrecision;
      dataPrecision  = ot.dataPrecision;
   }

   return *this;
}


//-----------------------------------------------------------------------------
// GmatBase* Clone() const
//-----------------------------------------------------------------------------
/**
 * Cloning method used to create a GmatODType from a GmatBase pointer
 *
 * @return A new GmatODType object matching this one
 */
//-----------------------------------------------------------------------------
GmatBase* GmatODType::Clone() const
{
   #ifdef DEBUG_ODTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Cloning a GMAT_OD obtype\n");
   #endif

   return new GmatODType(*this);
}


//-----------------------------------------------------------------------------
// bool Initialize()
//-----------------------------------------------------------------------------
/**
 * Prepares this GmatObType for use
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool GmatODType::Initialize()
{
#ifdef DEBUG_ODTYPE_CREATION_INITIALIZATION
   MessageInterface::ShowMessage("GmatODType::Initialize() Executing\n");
#endif
   
   ObType::Initialize();

   return true;
}


//-----------------------------------------------------------------------------
// bool Open(bool forRead, bool forWrite, bool append)
//-----------------------------------------------------------------------------
/**
 * Opens a GmatODType stream for processing
 *
 * The method manages GmatODType path and file extension defaults in addition to
 * performing the basic open operations.
 *
 * @param forRead True to open for reading, false otherwise
 * @param forWrite True to open for writing, false otherwise
 * @param append True if data being written should be appended, false if not
 *
 * @return true if the the stream was opened, false if not
 */
//-----------------------------------------------------------------------------
bool GmatODType::Open(bool forRead, bool forWrite, bool append)
{
   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("GmatODType::Open(%s, %s, %s) Executing\n",
            (forRead ? "true" : "false"),
            (forWrite ? "true" : "false"),
            (append ? "true" : "false") );
   #endif

   // when theStream open twice the content in theStream is always empty string. Therefore, it needs this verification.
   if (theStream.is_open())
      return true;

   bool retval = false;

   // temporary
   retval = true;
   std::ios_base::openmode mode = std::fstream::in;

   if (forRead && forWrite)
      mode = std::fstream::in | std::fstream::out;

   else
   {
      if (forRead)
         mode = std::fstream::in;
      if (forWrite)
         mode = std::fstream::out;
   }

   if (append)
      mode = mode | std::fstream::app;

   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("   Opening the stream %s, mode = %d\n",
            streamName.c_str(), mode);
   #endif

   if (streamName != "")
   {
      std::string fullPath = "";

      // If no path designation slash character is found, add the default path
      if ((streamName.find('/') == std::string::npos) &&
          (streamName.find('\\') == std::string::npos))
      {
         FileManager *fm = FileManager::Instance();
         fullPath = fm->GetPathname(FileManager::MEASUREMENT_PATH);
      }
      fullPath += streamName;

      // Add the .gmd extension if there is no extension in the file
      size_t dotLoc = fullPath.find_last_of('.');                     // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
      size_t slashLoc = fullPath.find_last_of('/');                   // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
      if (slashLoc == std::string::npos)
         slashLoc = fullPath.find_last_of('\\');

      if ((dotLoc == std::string::npos) ||
          (dotLoc < slashLoc))
      {
         fullPath += ".gmd";
      }

      #ifdef DEBUG_FILE_ACCESS
         MessageInterface::ShowMessage("   Full path is %s, mode = %d\n",
               fullPath.c_str(), mode);
      #endif
	  
      theStream.open(fullPath.c_str(), mode);
   }

   retval = theStream.is_open();
   if (retval && forWrite)
      theStream << header;

   if (retval == false)
   {
//      MessageInterface::ShowMessage(
//            "GMAT_OD Data File %s could not be opened\n",
//            streamName.c_str());
	  throw MeasurementException("GMAT_OD Data File " + streamName + " could not be opened\n");
   }
   return retval;
}

//-----------------------------------------------------------------------------
// bool IsOpen()
//-----------------------------------------------------------------------------
/**
 * Tests to see if the GmatODType data file has been opened
 *
 * @return true if the file is open, false if not.
 */
//-----------------------------------------------------------------------------
bool GmatODType::IsOpen()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatODType::IsOpen() Executing\n");
   #endif
   return theStream.is_open();
}


//-----------------------------------------------------------------------------
// bool AddMeasurement(MeasurementData *md)
//-----------------------------------------------------------------------------
/**
 * Adds a new measurement to the GmatODType data file
 *
 * This method takes the raw observation data passed in and formats it into a
 * string compatible with GmatInternal data files, and then writes that string
 * to the open data stream.
 *
 * @param md The measurement data containing the observation.
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool GmatODType::AddMeasurement(MeasurementData *md)
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatODType::AddMeasurement() Executing\n");
   #endif
   bool retval = false;

   std::stringstream dataLine;
   char databuffer[200];
   char epochbuffer[200];

   Real taiEpoch = (md->epochSystem == TimeConverterUtil::TAIMJD ? md->epoch :
         TimeConverterUtil::ConvertToTaiMjd(md->epochSystem, md->epoch,
               GmatTimeConstants::JD_NOV_17_1858));

   sprintf(epochbuffer, "%18.12lf", taiEpoch);
   dataLine << epochbuffer << "    " << md->typeName
            << "    " << md->type << "    ";
   for (UnsignedInt j = 0; j < md->participantIDs.size(); ++j)
      dataLine << md->participantIDs[j] << "    ";

   for (UnsignedInt k = 0; k < md->value.size(); ++k)
   {
	  Real dsnRange = GmatMathUtil::Mod(md->value[k],md->rangeModulo);			// value of observation has to be mod(fullrange, M) 
	  sprintf(databuffer, "%20.8lf", dsnRange);                                // increasing 6 decimal places to 8
      dataLine << databuffer;
      if (k < md->value.size()-1)
         dataLine << "    ";
   }
   
   //sprintf(databuffer, "    %d    %.15le    %.15le", md->uplinkBand, md->uplinkFreq, md->rangeModulo);
   sprintf(databuffer, "    %d    %.15le    %.15le", md->uplinkBand, md->uplinkFreqAtRecei, md->rangeModulo);
   dataLine << databuffer;
   theStream << dataLine.str() << "\n";


   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatODType::WriteMeasurement: \"%s\"\n",
            dataLine.str().c_str());
   #endif

   // temporary
   retval = true;

   return retval;
}


//-----------------------------------------------------------------------------
// ObservationData* ReadObservation()
//-----------------------------------------------------------------------------
/**
 * Retrieves an observation record
 *
 * This method reads an observation data set from a GmatOD data stream and
 * returns the data to the caller.
 *
 * @return The observation data from the stream.  If there is no more data in
 * the stream, a NULL pointer is returned.
 */
//-----------------------------------------------------------------------------
ObservationData* GmatODType::ReadObservation()
{
   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage("GmatODType::READObservation() Executing\n");
   #endif

   std::string str;
   std::stringstream theLine;

   Integer participantSize;
   Integer dataSize;

   Real defaultNoiseCovariance = 0.1;


   // Do nothing when it is at the end of file
   if (theStream.eof())
      return NULL;

   // Read a line when it is not end of file
   std::getline (theStream, str);
   
   // Skip header and comment lines or empty lines
   while ((str[0] == '%') || (GmatStringUtil::RemoveAllBlanks(str) == "") ||
          (str.length() < 2))
   {
      std::getline(theStream, str);

      // Do nothing when it is at the end of file
      if (theStream.eof())
         return NULL;
   }


   // Processing data in the line
   theLine << str;
   currentObs.Clear();
   currentObs.dataFormat = "GMAT_OD";

   // old format: 21545.05439854615       Range    7000    GS2ID    ODSatID    2713.73185
   // new format: 21545.05439854615    DSNRange    7050    GS2ID    ODSatID    2713.73185		Uplink Band		Uplink Frequency	Range Modulo 
   Real value; 

   GmatEpoch taiEpoch;
   theLine >> taiEpoch;

   currentObs.epoch = (currentObs.epochSystem == TimeConverterUtil::TAIMJD ?
         taiEpoch :
         TimeConverterUtil::ConvertFromTaiMjd(currentObs.epochSystem, taiEpoch,
               GmatTimeConstants::JD_NOV_17_1858));

   theLine >> currentObs.typeName;

   Integer type;
   theLine >> type;
   currentObs.type = (Gmat::MeasurementType)type;

   //switch (currentObs.type)
   //{
   //   case Gmat::GEOMETRIC_RANGE:
   //   case Gmat::GEOMETRIC_RANGE_RATE:
   //   case Gmat::USN_TWOWAYRANGE:
   //   case Gmat::USN_TWOWAYRANGERATE:
   //   case Gmat::DSN_TWOWAYRANGE:
   //   case Gmat::DSN_TWOWAYDOPPLER:
   //      participantSize = 2;
   //      dataSize = 1;
   //      break;

   //   case Gmat::TDRSS_TWOWAYRANGE:
   //   case Gmat::TDRSS_TWOWAYRANGERATE:
   //      participantSize = 3;
   //      dataSize = 1;
   //      break;

   //   case Gmat::GEOMETRIC_AZ_EL:
   //   case Gmat::GEOMETRIC_RA_DEC:
   //   case Gmat::OPTICAL_AZEL:
   //   case Gmat::OPTICAL_RADEC:
   //      participantSize = 2;
   //      dataSize = 2;
   //      defaultNoiseCovariance = 0.1;
   //      break;

   //   default:
   //      participantSize = 0;
   //      dataSize = 0;
   //      break;
   //}

   // Set measurement unit 
   currentObs.unit = "RU";

   participantSize = 2;
   dataSize = 1;
   for (Integer i = 0; i < participantSize; ++i)
   {
      theLine >> str;
      currentObs.participantIDs.push_back(str);
   }

   for (Integer i = 0; i < dataSize; ++i)
   {
      theLine >> value;
      currentObs.value.push_back(value);
	  currentObs.value_orig.push_back(value);
	  if (value == -1.0)
		  break;
   }

   if (value != -1.0)
   {
      // Read uplink band, uplink frequency, and range modulo:
      Integer uplinkBand;
//      Real uplinkFreq;
      Real uplinkFreqAtRecei;
      Real rangeModulo;

      theLine >> uplinkBand;
//      theLine >> uplinkFreq;
      theLine >> uplinkFreqAtRecei;
      theLine >> rangeModulo;
      currentObs.uplinkBand  = uplinkBand;
//      currentObs.uplinkFreq  = uplinkFreq;
      currentObs.uplinkFreqAtRecei = uplinkFreqAtRecei;
      currentObs.rangeModulo = rangeModulo;
   }

//   Covariance *noise = new Covariance();
//   noise->SetDimension(dataSize);
//   for (Integer i = 0; i < dataSize; ++i)
//   {
//      /// @todo Measurement noise covariance is hard-coded; this must be fixed
//      (*noise)(i,i) = defaultNoiseCovariance;
//   }
//   currentObs.noiseCovariance = noise;

   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage(" %.12lf    %s    %d    ", currentObs.epoch, currentObs.typeName.c_str(), currentObs.type);
      for (Integer i = 0; i < participantSize; ++i)
		  MessageInterface::ShowMessage("%s    ", currentObs.participantIDs.at(i).c_str());

      for (Integer i = 0; i < dataSize; ++i)
		  MessageInterface::ShowMessage("%.12lf    ", currentObs.value.at(i));

	  MessageInterface::ShowMessage(" %d    %.12le    %.12le\n",currentObs.uplinkBand, currentObs.uplinkFreq, currentObs.rangeModulo);
      MessageInterface::ShowMessage("GmatODType::READObservation() End\n");
   #endif

   return &currentObs;
}


//-----------------------------------------------------------------------------
// bool Close()
//-----------------------------------------------------------------------------
/**
 * Closes the data stream
 *
 * This method flushes the data stream, and then closes it.
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool GmatODType::Close()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatODType::Close() Executing\n");
   #endif
   bool retval = false;

   if (theStream.is_open())
   {
      theStream.flush();
      theStream.close();
      retval = !(theStream.is_open());
   }

   return retval;
}


//-----------------------------------------------------------------------------
// bool GmatODType::Finalize()
//-----------------------------------------------------------------------------
/**
 * Completes operations on this GmatODType.
 *
 * @return true always -- there is no GmatObType specific finalization needed.
 */
//-----------------------------------------------------------------------------
bool GmatODType::Finalize()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatODType::Finalize() Executing\n");
   #endif
   bool retval = true;

   return retval;
}
