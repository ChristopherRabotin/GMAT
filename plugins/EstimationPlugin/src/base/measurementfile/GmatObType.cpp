//$Id: GmatObType.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                         GmatObType
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2009/07/06
//
/**
 * ObType class used for the GMAT Internal observation data
 */
//------------------------------------------------------------------------------


#include "GmatObType.hpp"
#include "MessageInterface.hpp"
#include "GmatConstants.hpp"
#include "FileManager.hpp"
#include <sstream>


//#define DEBUG_OBTYPE_CREATION_INITIALIZATION
//#define DEBUG_FILE_WRITE


//-----------------------------------------------------------------------------
// GmatObType(const std::string withName)
//-----------------------------------------------------------------------------
/**
 * Default constructor
 *
 * @param withName The name of the new object
 */
//-----------------------------------------------------------------------------
GmatObType::GmatObType(const std::string withName) :
   ObType         ("GMATInternal", withName),
   epochPrecision (16),
   dataPrecision  (6)
{
   #ifdef DEBUG_OBTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Creating a GMATInternal obtype\n");
   #endif

   header = "% GMAT Internal Measurement Data File\n\n";
}

//-----------------------------------------------------------------------------
// ~GmatObType()
//-----------------------------------------------------------------------------
/**
 * Destructor
 */
//-----------------------------------------------------------------------------
GmatObType::~GmatObType()
{
}


//-----------------------------------------------------------------------------
// GmatObType(const GmatObType& ot) :
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ot The GmatObType that gets copied to this one
 */
//-----------------------------------------------------------------------------
GmatObType::GmatObType(const GmatObType& ot) :
   ObType         (ot),
   epochPrecision (ot.epochPrecision),
   dataPrecision  (ot.dataPrecision)
{
   #ifdef DEBUG_OBTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Copying a GMATInternal obtype\n");
   #endif
}


//-----------------------------------------------------------------------------
// GmatObType& operator=(const GmatObType& ot)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ot The GmatObType that gets copied to this one
 *
 * @return This GmatObType, configured to match ot
 */
//-----------------------------------------------------------------------------
GmatObType& GmatObType::operator=(const GmatObType& ot)
{
   #ifdef DEBUG_OBTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Assigning one GMATInternal obtype to another\n");
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
 * Cloning method used to create a GmatObType from a GmatBase pointer
 *
 * @return A new GmatObType object matching this one
 */
//-----------------------------------------------------------------------------
GmatBase* GmatObType::Clone() const
{
   #ifdef DEBUG_OBTYPE_CREATION_INITIALIZATION
      MessageInterface::ShowMessage("Cloning a GMATInternal obtype\n");
   #endif

   return new GmatObType(*this);
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
bool GmatObType::Initialize()
{
#ifdef DEBUG_OBTYPE_CREATION_INITIALIZATION
   MessageInterface::ShowMessage("GmatObType::Initialize() Executing\n");
#endif

   bool retval = false;

   return retval;
}


//-----------------------------------------------------------------------------
// bool Open(bool forRead, bool forWrite, bool append)
//-----------------------------------------------------------------------------
/**
 * Opens a GmatObType stream for processing
 *
 * The method manages GmatObType path and file extension defaults in addition to
 * performing the basic open operations.
 *
 * @param forRead True to open for reading, false otherwise
 * @param forWrite True to open for writing, false otherwise
 * @param append True if data being written should be appended, false if not
 *
 * @return true if the the stream was opened, false if not
 */
//-----------------------------------------------------------------------------
bool GmatObType::Open(bool forRead, bool forWrite, bool append)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("GmatObType::Open(%s, %s, %s) Executing\n",
            (forRead ? "true" : "false"),
            (forWrite ? "true" : "false"),
            (append ? "true" : "false") );
   #endif
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
      UnsignedInt dotLoc = fullPath.find_last_of('.');
      UnsignedInt slashLoc = fullPath.find_last_of('/');
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
      MessageInterface::ShowMessage(
            "GMATInternal Data File %s could not be opened\n",
            streamName.c_str());
   return retval;
}

//-----------------------------------------------------------------------------
// bool IsOpen()
//-----------------------------------------------------------------------------
/**
 * Tests to see if the GmatObType data file has been opened
 *
 * @return true if the file is open, false if not.
 */
//-----------------------------------------------------------------------------
bool GmatObType::IsOpen()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatObType::IsOpen() Executing\n");
   #endif
   return theStream.is_open();
}


//-----------------------------------------------------------------------------
// bool AddMeasurement(MeasurementData *md)
//-----------------------------------------------------------------------------
/**
 * Adds a new measurement to the GmatObType data file
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
bool GmatObType::AddMeasurement(MeasurementData *md)
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatObType::AddMeasurement() Executing\n");
   #endif
   bool retval = false;

   std::stringstream dataLine;
   char databuffer[20];
   char epochbuffer[20];

   Real taiEpoch = (md->epochSystem == TimeConverterUtil::TAIMJD ? md->epoch :
         TimeConverterUtil::ConvertToTaiMjd(md->epochSystem, md->epoch,
               GmatTimeConstants::JD_NOV_17_1858));

   sprintf(epochbuffer, "%17.11lf", taiEpoch);
   dataLine << epochbuffer << "    " << md->typeName
            << "    " << md->type << "    ";
   for (UnsignedInt j = 0; j < md->participantIDs.size(); ++j)
      dataLine << md->participantIDs[j] << "    ";

   for (UnsignedInt k = 0; k < md->value.size(); ++k)
   {
      sprintf(databuffer, "%9.5lf", md->value[k]);
      dataLine << databuffer;
      if (k < md->value.size()-1)
         dataLine << "    ";
   }

   theStream << dataLine.str() << "\n";

   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatObType::WriteMeasurement: \"%s\"\n",
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
 * This method reads an observation data set from a GmatInternal data stream and
 * returns the data to the caller.
 *
 * @return The observation data from the stream.  If there is no more data in
 * the stream, a NULL pointer is returned.
 */
//-----------------------------------------------------------------------------
ObservationData* GmatObType::ReadObservation()
{
   std::string str;
   std::stringstream theLine;

   Integer participantSize;
   Integer dataSize;

   Real defaultNoiseCovariance = 0.1;

   // Skip header and comment lines
   std::getline (theStream, str);

   while ((str[0] == '%') || (!theStream.eof() && (str == "")))
   {
      std::getline(theStream, str);
   }

   if (theStream.eof())
   {
      return NULL;
   }

   theLine << str;
   currentObs.Clear();

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

   switch (currentObs.type)
   {
      case Gmat::GEOMETRIC_RANGE:
      case Gmat::GEOMETRIC_RANGE_RATE:
      case Gmat::USN_TWOWAYRANGE:
      case Gmat::USN_TWOWAYRANGERATE:
      case Gmat::DSN_TWOWAYRANGE:
      case Gmat::DSN_TWOWAYDOPPLER:
         participantSize = 2;
         dataSize = 1;
         break;

      case Gmat::TDRSS_TWOWAYRANGE:
      case Gmat::TDRSS_TWOWAYRANGERATE:
         participantSize = 3;
         dataSize = 1;
         break;

      case Gmat::GEOMETRIC_AZ_EL:
      case Gmat::GEOMETRIC_RA_DEC:
      case Gmat::OPTICAL_AZEL:
      case Gmat::OPTICAL_RADEC:
         participantSize = 2;
         dataSize = 2;
         defaultNoiseCovariance = 0.1;
         break;

      default:
         participantSize = 0;
         dataSize = 0;
         break;
   }

   for (Integer i = 0; i < participantSize; ++i)
   {
      theLine >> str;
      currentObs.participantIDs.push_back(str);
   }

   for (Integer i = 0; i < dataSize; ++i)
   {
      theLine >> value;
      currentObs.value.push_back(value);
   }

   Covariance *noise = new Covariance();
   noise->SetDimension(dataSize);
   for (Integer i = 0; i < dataSize; ++i)
   {
      /// @todo Measurement noise covariance is hard-coded; this must be fixed
      (*noise)(i,i) = defaultNoiseCovariance;
   }
   currentObs.noiseCovariance = noise;

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
bool GmatObType::Close()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatObType::Close() Executing\n");
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
// bool GmatObType::Finalize()
//-----------------------------------------------------------------------------
/**
 * Completes operations on this GmatObType.
 *
 * @return true always -- there is no GmatObType specific finalization needed.
 */
//-----------------------------------------------------------------------------
bool GmatObType::Finalize()
{
   #ifdef DEBUG_FILE_WRITE
      MessageInterface::ShowMessage("GmatObType::Finalize() Executing\n");
   #endif
   bool retval = true;

   return retval;
}
