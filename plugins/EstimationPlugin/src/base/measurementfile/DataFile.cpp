//$Id: DataFile.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                                 DataFile
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
// Created: 2009/07/16
//
/**
 * Implementation for the DataFile class used in GMAT measurement simulator and
 * estimators
 */
//------------------------------------------------------------------------------


#include "DataFile.hpp"
#include "GmatBase.hpp"
#include "MessageInterface.hpp"
#include <sstream>

//#define DEBUG_FILE_ACCESS
//#define DEBUG_INITIALIZATION
//#define DEBUG_OBSERVATION_READ
//#define DEBUG_OBSERVATION_DATA
//#define DEBUG_RAMP_TABLE_READ
//#define DEBUG_RAMP_TABLE_DATA

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string DataFile::PARAMETER_TEXT[] =
{
   "Filename",
   "Format"
};

const Gmat::ParameterType DataFile::PARAMETER_TYPE[] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE			// Gmat::OBJECT_TYPE			// made changes by TUAN NGUYEN
};



//------------------------------------------------------------------------------
// DataFile(const std::string name)
//------------------------------------------------------------------------------
/**
 * Constructor for DataFile objects
 *
 * @param name The name of the object
 */
//------------------------------------------------------------------------------
DataFile::DataFile(const std::string name) :
   GmatBase          (Gmat::DATA_FILE, "DataFile", name),
   theDatastream     (NULL),
   streamName        ("ObsData.gmd"),
   obsType           ("GMATInternal")
{
   objectTypes.push_back(Gmat::DATA_FILE);
   objectTypes.push_back(Gmat::DATASTREAM);
   objectTypeNames.push_back("DataFile");

   parameterCount = DataFileParamCount;
}


//------------------------------------------------------------------------------
// ~DataFile()
//------------------------------------------------------------------------------
/**
 * DataFile destructor
 */
//------------------------------------------------------------------------------
DataFile::~DataFile()
{
   if (theDatastream)
      delete theDatastream;
}


//------------------------------------------------------------------------------
// DataFile(const DataFile& df)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a DataFile
 *
 * @param df The DataFile object that provides data for the new one
 */
//------------------------------------------------------------------------------
DataFile::DataFile(const DataFile& df) :
   GmatBase          (df),
   streamName        (df.streamName),
   obsType           (df.obsType)
{
   if (df.theDatastream != NULL)
      theDatastream = (ObType*)df.theDatastream->Clone();
   else
      theDatastream = NULL;
}


//------------------------------------------------------------------------------
// DataFile& operator=(const DataFile& df)
//------------------------------------------------------------------------------
/**
 * DataFile assignment operator
 *
 * @param df The DataFile object that provides data for the this one
 *
 * @return This object, configured to match df
 */
//------------------------------------------------------------------------------
DataFile& DataFile::operator=(const DataFile& df)
{
   if (this != &df)
   {
      GmatBase::operator=(df);

      streamName = df.streamName;
      obsType    = df.obsType;

      if (df.theDatastream)
         theDatastream = (ObType*)df.theDatastream->Clone();
      else
         theDatastream = NULL;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone method for DataFiles
 *
 * @return A clone of this object.
 */
//------------------------------------------------------------------------------
GmatBase* DataFile::Clone() const
{
   return new DataFile(*this);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Code fired in the Sandbox when the Sandbox initializes objects prior to a run
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFile::Initialize()
{
#ifdef DEBUG_INITIALIZATION
	MessageInterface::ShowMessage("DataFile<%s>::Initialize()\n", GetName().c_str());
#endif

   bool retval = false;

   if (theDatastream)
   {
      retval = theDatastream->Initialize();
	  obsType = theDatastream->GetTypeName();			// made changes by TUAN NGUYEN
      #ifdef DEBUG_INITIALIZATION
	     MessageInterface::ShowMessage("DataFile::Initialize():   obsType = '%s'\n", obsType.c_str());
      #endif

   }

   return retval;
}


//------------------------------------------------------------------------------
// bool Finalize()
//------------------------------------------------------------------------------
/**
 * Code that executes after a run completes
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFile::Finalize()
{
   bool retval = false;

   if (theDatastream)
   {
      retval = theDatastream->Finalize();
   }

   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script a DataFile property
 *
 * @param id The ID of the property
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string DataFile::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < DataFileParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterUnit(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the units used for a property
 *
 * @param id The ID of the property
 *
 * @return The text string specifying the property's units
 */
//------------------------------------------------------------------------------
std::string DataFile::GetParameterUnit(const Integer id) const
{
   return GmatBase::GetParameterUnit(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * Retrieves the ID associated with a scripted property string
 *
 * @param str The scripted string used for the property
 *
 * @return The associated ID
 */
//------------------------------------------------------------------------------
Integer DataFile::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < DataFileParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter type for a DataFile property
 *
 * @param id The ID of the property
 *
 * @return The ParameterType of the property
 */
//------------------------------------------------------------------------------
Gmat::ParameterType DataFile::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < DataFileParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];

   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string describing the type of a property
 *
 * @param id The ID of the property
 *
 * @return The text description of the property type
 */
//------------------------------------------------------------------------------
std::string DataFile::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property of a DataFile
 *
 * @param id The ID of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string DataFile::GetStringParameter(const Integer id) const
{
   if (id == ObsType)
      return obsType;

   if (id == StreamName)
      return streamName;

   return GmatBase::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string property
 *
 * @param id The ID of the property
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFile::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == ObsType)
   {
      obsType = value;
      return true;
   }

   if (id == StreamName)
   {
      streamName = value;
      return true;
   }

   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property of a DataFile contained in an array
 *
 * @note This method is provided to keep the base class version visible to the
 *       compiler.
 *
 * @param id The ID of the property
 * @param index The array index for the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string DataFile::GetStringParameter(const Integer id,
      const Integer index) const
{
   return GmatBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string property of a DataFile contained in an array
 *
 * @note This method is provided to keep the base class version visible to the
 *       compiler.
 *
 * @param id The ID of the property
 * @param value The new property value
 * @param index The array index for the property
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFile::SetStringParameter(const Integer id, const std::string &value,
      const Integer index)
{
   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property of a DataFile
 *
 * @param label The text description of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string DataFile::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Sets a string property
 *
 * @param label The text description of the property
 * @param value The new value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFile::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label,
//                                const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property of a DataFile contained in an array
 *
 * @note This method is provided to keep the base class version visible to the
 *       compiler.
 *
 * @param label The text description of the property
 * @param index The array index for the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string DataFile::GetStringParameter(const std::string &label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value,
//                         const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a string property of a DataFile contained in an array
 *
 * @note This method is provided to keep the base class version visible to the
 *       compiler.
 *
 * @param label The text description of the property
 * @param value The new property value
 * @param index The array index for the property
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFile::SetStringParameter(const std::string &label,
      const std::string &value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// bool SetStream(ObType *thisStream)
//------------------------------------------------------------------------------
/**
 * Sets the data stream used for the measurement data
 *
 * @param thisStream The ObType that provides the stream interfaces
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFile::SetStream(ObType *thisStream)
{
   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("Setting ObType to a %s object\n",
            thisStream->GetTypeName().c_str());
   #endif

   bool retval = false;

   if (thisStream->IsOfType(Gmat::OBTYPE))
   {
      theDatastream = thisStream;
      retval = true;
   }
   return retval;
}


///// TBD: This method needs to be documented
bool DataFile::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                  const std::string &name)
{
   #ifdef DEBUG_FILE_ACCESS
      MessageInterface::ShowMessage("DataFile<'%s'>::SetRefObject: Setting object of "
		  "type %s <%d> named '%s'\n", GetName().c_str(), obj->GetTypeName().c_str(), type,
            obj->GetName().c_str());
   #endif

   if (obj->IsOfType(Gmat::OBTYPE))
   {
      theDatastream = (ObType*)obj;
      return true;
   }

   return GmatBase::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
// bool OpenStream(bool simulate)
//------------------------------------------------------------------------------
/**
 * Opens the data stream used for the measurement data
 *
 * @param simulate A flag indicating if the stream should be opened to receive
 *                 simulated data
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFile::OpenStream(bool simulate)
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("Entered DataFile<%s>::OpenStream(%s)\n", 
		  GetName().c_str(), (simulate ? "true" : "false"));
   #endif

   bool retval = false;

   if (theDatastream)
   {
      theDatastream->SetStreamName(streamName);
	  obsType = theDatastream->GetTypeName();							// made changes by TUAN NGUYEN
	  #ifdef DEBUG_INITIALIZATION
		    MessageInterface::ShowMessage("DataFile::OpenStream():   obsType = '%s'\n", obsType.c_str());
      #endif
      // todo: Currently opens either to simulate or to estimate, but not both
      // at the same time.
	  // For ramp table, it is opened for read only
///// TBD: Determine if there is a more generic way to add this
	  if (obsType == "GMAT_RampTable")									// made changes by TUAN NGUYEN
	  {
         #ifdef DEBUG_INITIALIZATION
		    MessageInterface::ShowMessage("DataFile::OpenStream():   open ramp table '%s' for reading\n", GetName().c_str());
         #endif
	     retval = theDatastream->Open(true, false);						// made changes by TUAN NGUYEN
	  }
	  else																// made changes by TUAN NGUYEN
	  {
         if (simulate)
		 {
            #ifdef DEBUG_INITIALIZATION
		       MessageInterface::ShowMessage("DataFile::OpenStream():   open observation data file '%s' for writing\n", GetName().c_str());
            #endif
            retval = theDatastream->Open(false, true);
		 }
         else
		 {
            #ifdef DEBUG_INITIALIZATION
		       MessageInterface::ShowMessage("DataFile::OpenStream():   open observation data file '%s' for reading\n", GetName().c_str());
            #endif
            retval = theDatastream->Open(true, false);
		 }
	  }
   }

   return retval;
}

//------------------------------------------------------------------------------
// bool IsOpen()
//------------------------------------------------------------------------------
/**
 * Reports the status of a datastream
 *
 * @return true if the stream is open, false if not
 */
//------------------------------------------------------------------------------
bool DataFile::IsOpen()
{
   if (theDatastream)
      return theDatastream->IsOpen();

   return false;
}

//------------------------------------------------------------------------------
// void WriteMeasurement(MeasurementData* theMeas)
//------------------------------------------------------------------------------
/**
 * Sends a measurement to a data stream so it can be written
 *
 * This method is used during simulation to pass a calculated measurement to
 * the measurement stream.
 *
 * @param theMeas The measurement that needs to be written
 */
//------------------------------------------------------------------------------
void DataFile::WriteMeasurement(MeasurementData* theMeas)
{
   if (theDatastream)
      theDatastream->AddMeasurement(theMeas);
}


//------------------------------------------------------------------------------
// MeasurementData* ReadMeasurement()
//------------------------------------------------------------------------------
/**
 * Retrieves an observation from a data stream so it can be processed
 *
 * This method is used during estimation to retrieve the measurement
 * observations from the measurement stream.
 *
 * @return The measurement observation from the file, or NULL if no more
 *         observations are available
 */
//------------------------------------------------------------------------------
ObservationData* DataFile::ReadObservation()
{
   #ifdef DEBUG_OBSERVATION_READ
	  MessageInterface::ShowMessage("Entered DataFile<%s>::ReadObservation()\n", GetName().c_str());
   #endif

   ObservationData *theObs = NULL;
   if (theDatastream)
   {
      theObs = theDatastream->ReadObservation();

      #ifdef DEBUG_OBSERVATION_DATA
         if (theObs)
         {
            MessageInterface::ShowMessage("Observation:\n");
            MessageInterface::ShowMessage("   Epoch:          %.12lf\n",
                  theObs->epoch);
            MessageInterface::ShowMessage("   Type:           %s\n",
                  theObs->typeName.c_str());
            MessageInterface::ShowMessage("   TypeID:         %d\n",
                  theObs->type);
            for (UnsignedInt i = 0; i < theObs->participantIDs.size(); ++i)
               MessageInterface::ShowMessage("   Participant %d: %s\n", i,
                     theObs->participantIDs[i].c_str());
            for (UnsignedInt i = 0; i < theObs->value.size(); ++i)
               MessageInterface::ShowMessage("   Value[%d]:         %.12lf\n",
                     i, theObs->value[i]);

            // Now the extra data
            for (UnsignedInt i = 0; i < theObs->extraData.size(); ++i)
            {
               MessageInterface::ShowMessage("   ExtraData[%d]:   %s (type %d)"
                     " = %s\n", i, theObs->extraDataDescriptions[i].c_str(),
                     theObs->extraTypes[i], theObs->extraData[i].c_str());
            }
         }
         else
            MessageInterface::ShowMessage("*** Reached End of Observations\n");
      #endif
   }

   #ifdef DEBUG_OBSERVATION_READ
	  MessageInterface::ShowMessage("Exit DataFile::ReadObservation()\n");
   #endif

   return theObs;
}


///// TBD: Determine if there is a more generic way to add these
//------------------------------------------------------------------------------
// RampTableData* ReadRampTableData()
//------------------------------------------------------------------------------
/**
 * Retrieves a frequency ramp table data from a data stream so it can be processed
 *
 * This method is used during simualation to simulate a frequency ramp measurement.
 *
 * @return The frequency ramp table data from the file, or NULL if no more
 *         ramp table data are available
 */
//------------------------------------------------------------------------------
RampTableData* DataFile::ReadRampTableData()
{
   #ifdef DEBUG_RAMP_TABLE_READ
	  MessageInterface::ShowMessage("Entered DataFile<%s>::ReadRampTableData()\n", GetName().c_str());
   #endif

   RampTableData *theData = NULL;
   if (theDatastream)
   {
      theData = theDatastream->ReadRampTableData();

      #ifdef DEBUG_RAMP_TABLE_DATA
         if (theData)
         {
            MessageInterface::ShowMessage("Ramp table data:\n");
            MessageInterface::ShowMessage("   Epoch:          %.12lf\n",
                  theData->epoch);
            MessageInterface::ShowMessage("   Type:           %s\n",
                  theData->typeName.c_str());
            MessageInterface::ShowMessage("   TypeID:         %d\n",
                  theData->type);
            for (UnsignedInt i = 0; i < theData->participantIDs.size(); ++i)
               MessageInterface::ShowMessage("   Participant %d: %s\n", i,
                     theData->participantIDs[i].c_str());
            MessageInterface::ShowMessage("ramp type = %d     ramp frequency = %.12le    ramp rate = %.12le\n",
				theData->rampType, theData->rampFrequency, theData->rampRate);
         }
         else
            MessageInterface::ShowMessage("*** Reached End of ramp table data\n");
      #endif
   }

   #ifdef DEBUG_RAMP_TABLE_READ
	  MessageInterface::ShowMessage("Exit DataFile::ReadRampTableData()\n");
   #endif

   return theData;
}


//------------------------------------------------------------------------------
// bool CloseStream()
//------------------------------------------------------------------------------
/**
 * Closes the data stream
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool DataFile::CloseStream()
{
   #ifdef DEBUG_FILE_ACCESS
	  MessageInterface::ShowMessage("Enetered DataFile::CloseStream()\n");
   #endif

   bool retval = false;

   if (theDatastream)
      retval = theDatastream->Close();

   #ifdef DEBUG_FILE_ACCESS
	  MessageInterface::ShowMessage("Exit DataFile::CloseStream()\n");
   #endif

   return retval;
}
