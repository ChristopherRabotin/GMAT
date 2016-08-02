//$Id: GmatODDopplerType.cpp 1398 2013-09-18 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                         GmatODDopplerType
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
// Author: Tuan Dang Nguyen. NASA/GSFC
// Created: 2013/09/18
//
/**
 * GmatODDopplerType class used for the GMAT OD doppler observation data
 */
//------------------------------------------------------------------------------


#include "GmatODDopplerType.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "FileManager.hpp"
#include "StringUtil.hpp"
#include "MeasurementException.hpp"
#include <sstream>


//#define DEBUG_ODDOPPLERTYPE_CREATION_INITIALIZATION
//#define DEBUG_FILE_WRITE
//#define DEBUG_FILE_READ
//#define DEBUG_FILE_ACCESS

//-----------------------------------------------------------------------------
// GmatODDopperType(const std::string withName)
//-----------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param withName The name of the new object
 */
//-----------------------------------------------------------------------------
GmatODDopplerType::GmatODDopplerType(const std::string withName) :
   ObType         ("GMAT_ODDoppler", withName),
   epochPrecision (16),
   dataPrecision  (6)
{
   #ifdef DEBUG_ODDOPPLERTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Creating a GMAT_ODDoppler object type\n");
   #endif

   header = "% GMAT OD Doppler Measurement Data File\n\n";
}

//-----------------------------------------------------------------------------
// ~GmatODDopplerType()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
GmatODDopplerType::~GmatODDopplerType()
{
}


//-----------------------------------------------------------------------------
// GmatODDopplerType(const GmatODDopplerType& ot) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ot The GmatODDopplerType that gets copied to this one
 */
//-----------------------------------------------------------------------------
GmatODDopplerType::GmatODDopplerType(const GmatODDopplerType& ot) :
   ObType         (ot),
   epochPrecision (ot.epochPrecision),
   dataPrecision  (ot.dataPrecision)
{
   #ifdef DEBUG_ODDOPPLERTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Copying a GMAT_ODDoppler object type\n");
   #endif
}


//-----------------------------------------------------------------------------
// GmatODDopplerType& operator=(const GmatODDopplerType& ot)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ot The GmatODDopplerType that gets copied to this one
 *
 * @return This GmatODDopplerType, configured to match ot
 */
//-----------------------------------------------------------------------------
GmatODDopplerType& GmatODDopplerType::operator=(const GmatODDopplerType& ot)
{
   #ifdef DEBUG_ODDOPPLERTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Assigning one GMAT_ODDoppler object type to another\n");
   #endif

   if (this != &ot)
   {
	  ObType::operator= (ot);

      epochPrecision = ot.epochPrecision;
      dataPrecision  = ot.dataPrecision;
   }

   return *this;
}


//-----------------------------------------------------------------------------
// GmatBase* Clone() const
//-----------------------------------------------------------------------------
/**
 * Cloning method used to create a GmatODDopplerType from a GmatBase pointer
 *
 * @return A new GmatODDopplerType object matching this one
 */
//-----------------------------------------------------------------------------
GmatBase* GmatODDopplerType::Clone() const
{
   #ifdef DEBUG_ODDOPPLERTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Cloning a GMAT_ODDoppler object type\n");
   #endif

   return new GmatODDopplerType(*this);
}


//-----------------------------------------------------------------------------
// bool Initialize()
//-----------------------------------------------------------------------------
/**
 * Prepares this GmatODDopplerType for use
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool GmatODDopplerType::Initialize()
{
#ifdef DEBUG_ODDOPPLERTYPE_CREATION_INITIALIZATION
   MessageInterface::ShowMessage("GmatODDopplerType::Initialize() Executing\n");
#endif

   ObType::Initialize();

   return true;
}


//-----------------------------------------------------------------------------
// bool Open(bool forRead, bool forWrite, bool append)
//-----------------------------------------------------------------------------
/**
 * Opens a GmatODDopplerType stream for processing
 *
 * The method manages GmatODDopplerType path and file extension defaults in addition to
 * performing the basic open operations.
 *
 * @param forRead True to open for reading, false otherwise
 * @param forWrite True to open for writing, false otherwise
 * @param append True if data being written should be appended, false if not
 *
 * @return true if the the stream was opened, false if not
 */
//-----------------------------------------------------------------------------
bool GmatODDopplerType::Open(bool forRead, bool forWrite, bool append)
{
   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("GmatODDopplerType::Open(%s, %s, %s) Executing\n",
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
      size_t dotLoc = fullPath.find_last_of('.');                      // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
      size_t slashLoc = fullPath.find_last_of('/');                    // change from std::string::size_type to size_t in order to compatible with C++98 and C++11
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
	  throw MeasurementException("GMAT_ODDoppler Data File " + streamName + " could not be opened\n");

   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("Exit GmatODDopplerType::Open()\n");
   #endif

   return retval;
}

//-----------------------------------------------------------------------------
// bool IsOpen()
//-----------------------------------------------------------------------------
/**
 * Tests to see if the GmatODDopplerType data file has been opened
 *
 * @return true if the file is open, false if not.
 */
//-----------------------------------------------------------------------------
bool GmatODDopplerType::IsOpen()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatODDopplerType::IsOpen() Executing\n");
   #endif
   return theStream.is_open();
}


//-----------------------------------------------------------------------------
// bool AddMeasurement(MeasurementData *md)
//-----------------------------------------------------------------------------
/**
 * Adds a new measurement to the GmatODDopplerType data file
 *
 * This method takes the raw observation data passed in and formats it into a
 * string compatible with Gmat_ODDoppler data files, and then writes that string
 * to the open data stream.
 *
 * @param md The measurement data containing the observation.
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool GmatODDopplerType::AddMeasurement(MeasurementData *md)
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatODDopplerType::AddMeasurement() Executing\n");
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

   // store value of uplink band
   dataLine << md->uplinkBand << "    ";
   // store value of doppler count interval
   dataLine << md->dopplerCountInterval << "    ";

   for (UnsignedInt k = 0; k < md->value.size(); ++k)
   {
      sprintf(databuffer, "%20.8lf", md->value[k]);           // increasing 6 decimal places to 8
      dataLine << databuffer;
      if (k < md->value.size()-1)
         dataLine << "    ";
   }

   theStream << dataLine.str() << "\n";

   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatODDopplerType::WriteMeasurement: \"%s\"\n",
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
 * This method reads an observation data set from a Gmat_ODDoppler data stream and
 * returns the data to the caller.
 *
 * @return The observation data from the stream.  If there is no more data in
 * the stream, a NULL pointer is returned.
 */
//-----------------------------------------------------------------------------
ObservationData* GmatODDopplerType::ReadObservation()
{
   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage("GmatODDopplerType::ReadObservation() Executing\n");
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
   currentObs.dataFormat = "GMAT_ODDoppler";

   // format: 21545.05439854615    Range    7000    GS2ID    ODSatID    2713.73185
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
   currentObs.unit = "Hz";

   participantSize = 2;
   dataSize = 1;
   for (Integer i = 0; i < participantSize; ++i)
   {
      theLine >> str;
      currentObs.participantIDs.push_back(str);
   }

   
   // load value of uplink band
   theLine >> currentObs.uplinkBand; 
   // load value of doppler count interval
   theLine >> currentObs.dopplerCountInterval;


   for (Integer i = 0; i < dataSize; ++i)
   {
      theLine >> value;
      currentObs.value.push_back(value);
	  currentObs.value_orig.push_back(value);
   }
/*
   Covariance *noise = new Covariance();
   noise->SetDimension(dataSize);
   for (Integer i = 0; i < dataSize; ++i)
   {
      /// @todo Measurement noise covariance is hard-coded; this must be fixed
      (*noise)(i,i) = defaultNoiseCovariance;
   }
   currentObs.noiseCovariance = noise;
*/
   #ifdef DEBUG_FILE_READ
      MessageInterface::ShowMessage(" %.12lf    %s    %d    ", currentObs.epoch, currentObs.typeName.c_str(), currentObs.type);
      for (Integer i = 0; i < participantSize; ++i)
		  MessageInterface::ShowMessage("%s    ", currentObs.participantIDs.at(i).c_str());

	  MessageInterface::ShowMessage(" %d    %.12lf    ", currentObs.uplinkBand, currentObs.dopplerCountInterval);
      
	  for (Integer i = 0; i < dataSize; ++i)
		  MessageInterface::ShowMessage("%.12lf    ", currentObs.value.at(i));

	  MessageInterface::ShowMessage("\n");
      MessageInterface::ShowMessage("GmatObType::ReadObservation() End\n");
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
bool GmatODDopplerType::Close()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatODDopplerType::Close() Executing\n");
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
// bool GmatODDopplerType::Finalize()
//-----------------------------------------------------------------------------
/**
 * Completes operations on this GmatObType.
 *
 * @return true always -- there is no GmatObType specific finalization needed.
 */
//-----------------------------------------------------------------------------
bool GmatODDopplerType::Finalize()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatODDopplerType::Finalize() Executing\n");
   #endif
   bool retval = true;

   return retval;
}
