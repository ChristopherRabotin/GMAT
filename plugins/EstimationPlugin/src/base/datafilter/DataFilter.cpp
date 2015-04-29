//$Id: DataFilter.cpp 1398 2015-03-03 14:10:37Z tdnguyen $
//------------------------------------------------------------------------------
//                                 DataFilter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2014 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2015/03/03
//
/**
 * Implementation for the DataFilter class
 */
//------------------------------------------------------------------------------


#include "DataFilter.hpp"
#include "GmatBase.hpp"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"
#include "MeasurementException.hpp"
#include "DateUtil.hpp" 
#include <sstream>


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION
//#define DEBUG_FILTER

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string DataFilter::PARAMETER_TEXT[] =
{
   "FileNames",
   "ObservedObjects",
   "Trackers",
   "DataTypes",
   "EpochFormat",
   "InitialEpoch",
   "FinalEpoch",
   "Strands",
};

const Gmat::ParameterType DataFilter::PARAMETER_TYPE[] =
{
   Gmat::STRINGARRAY_TYPE,			// FILENAMES
   Gmat::STRINGARRAY_TYPE,			// OBSERVED_OBJECTS
   Gmat::STRINGARRAY_TYPE,       // TRACKERS
   Gmat::STRINGARRAY_TYPE,       // DATA_TYPES
   Gmat::STRING_TYPE,			   // EPOCH_FORMAT
   Gmat::STRING_TYPE,            // INITIAL_EPOCH
   Gmat::STRING_TYPE,			   // FINIAL_EPOCH
   Gmat::STRINGARRAY_TYPE,       // STRANDS
};



//------------------------------------------------------------------------------
// DataFilter(const std::string name)
//------------------------------------------------------------------------------
/**
 * Constructor for DataFilter objects
 *
 * @param name The name of the object
 */
//------------------------------------------------------------------------------
DataFilter::DataFilter(const std::string name) :
   GmatBase          (Gmat::DATA_FILTER, "DataFilter", name),
   epochFormat       ("TAIModJulian"),
   initialEpoch      (DateUtil::EARLIEST_VALID_MJD), 
   finalEpoch        (DateUtil::LATEST_VALID_MJD),
   isChecked         (false)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("DataFilter default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::DATA_FILTER);
   objectTypeNames.push_back("DataFilter");

   parameterCount = DataFilterParamCount;
   
   epochStart = ConvertToRealEpoch(initialEpoch, epochFormat);
   epochEnd = ConvertToRealEpoch(finalEpoch, epochFormat);

   observers.push_back("All");
   fileNames.push_back("From_AddTrackingConfig");
   trackers.push_back("All");
   dataTypes.push_back("All");

   dataTypesMap["Range_KM"]          = "Range";
   dataTypesMap["Range_KU"]          = "DSNRange";
   dataTypesMap["Doppler_Hz"]        = "Doppler";
   dataTypesMap["Doppler_RangeRate"] = "RangeRate";
}


//------------------------------------------------------------------------------
// ~DataFilter()
//------------------------------------------------------------------------------
/**
 * DataFilter destructor
 */
//------------------------------------------------------------------------------
DataFilter::~DataFilter()
{
}


//------------------------------------------------------------------------------
// DataFilter(const DataFilter& saf)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a DataFilter
 *
 * @param saf        The DataFilter object that provides data for the new one
 */
//------------------------------------------------------------------------------
DataFilter::DataFilter(const DataFilter& saf) :
   GmatBase              (saf),
   fileNames             (saf.fileNames),
   observers             (saf.observers),
   trackers              (saf.trackers),
   dataTypes             (saf.dataTypes),
   epochFormat           (saf.epochFormat),
   initialEpoch          (saf.initialEpoch),
   finalEpoch            (saf.finalEpoch),
   epochStart            (saf.epochStart),
   epochEnd              (saf.epochEnd),
   strands               (saf.strands),
   dataTypesMap          (saf.dataTypesMap),
   isChecked             (false)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("DataFilter copy constructor from <%s,%p>  to  <%s,%p>\n", saf.GetName().c_str(), &saf, GetName().c_str(), this);
#endif

}


//------------------------------------------------------------------------------
// DataFilter& operator=(const DataFilter& saf)
//------------------------------------------------------------------------------
/**
 * DataFilter assignment operator
 *
 * @param saf    The DataFilter object that provides data for the this one
 *
 * @return This object, configured to match saf
 */
//------------------------------------------------------------------------------
DataFilter& DataFilter::operator=(const DataFilter& saf)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("DataFilter operator = <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &saf)
   {
      GmatBase::operator=(saf);

      fileNames    = saf.fileNames;
      observers    = saf.observers;
      trackers     = saf.trackers;
      dataTypes    = saf.dataTypes;
      epochFormat  = saf.epochFormat;
      initialEpoch = saf.initialEpoch;
      finalEpoch   = saf.finalEpoch;
      epochStart   = saf.epochStart;
      epochEnd     = saf.epochEnd;
      strands      = saf.strands;
      dataTypesMap = saf.dataTypesMap;
      isChecked    = false;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone method for DataFilter
 *
 * @return A clone of this object.
 */
//------------------------------------------------------------------------------
GmatBase* DataFilter::Clone() const
{
   return new DataFilter(*this);
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
bool DataFilter::Initialize()
{
#ifdef DEBUG_INITIALIZATION
	MessageInterface::ShowMessage("DataFilter<%s,%p>::Initialize()   entered\n", GetName().c_str(), this);
#endif

   bool retval = true;

   if (epochStart > epochEnd)
      throw MeasurementException("Error: " + GetName() + ".InitialEpoch (" + initialEpoch + ") is greater than " + GetName() +".FinalEpoch (" + finalEpoch +")\n");

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("DataFilter<%s,%p>::Initialize()   exit\n", GetName().c_str(), this);
#endif

   isInitialized = retval;
   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script a DataFilter property
 *
 * @param id The ID of the property
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string DataFilter::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < DataFilterParamCount)
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
std::string DataFilter::GetParameterUnit(const Integer id) const
{
   // @Todo: It needs to add code to specify unit used for each parameter
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
Integer DataFilter::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < DataFilterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
      {
         if (IsParameterReadOnly(i))
            throw MeasurementException("Error: " + str + "parameter was not defined in DataFilter.\n");
         return i;
      }
   }

   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the parameter type for a property
 *
 * @param id The ID of the property
 *
 * @return The ParameterType of the property
 */
//------------------------------------------------------------------------------
Gmat::ParameterType DataFilter::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < DataFilterParamCount)
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
std::string DataFilter::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


bool DataFilter::IsParameterReadOnly(const Integer id) const
{
   if (id == STRANDS)
      return true;

   return GmatBase::IsParameterReadOnly(id);
}


bool DataFilter::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}



//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property
 *
 * @param id The ID of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string DataFilter::GetStringParameter(const Integer id) const
{
   if (id == EPOCH_FORMAT)
      return epochFormat;

   if (id == INITIAL_EPOCH)
      return initialEpoch;

   if (id == FINAL_EPOCH)
      return finalEpoch;

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
bool DataFilter::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == EPOCH_FORMAT)
   {
      StringArray nameList = GetListOfValidEpochFormats();
      if (find(nameList.begin(), nameList.end(), value) == nameList.end())
         throw MeasurementException("Error: Value '" + value + "' set to " + GetName() + ".EpochFormat parameter is invalid.\n");

      epochFormat = value;
      return true;
   }

   if (id == INITIAL_EPOCH)
   {
      initialEpoch = value;
      // Convert to a.1 time for internal processing
      epochStart = ConvertToRealEpoch(initialEpoch, epochFormat);
      return true;
   }

   if (id == FINAL_EPOCH)
   {
      finalEpoch = value;
      // Convert to a.1 time for internal processing
      epochEnd = ConvertToRealEpoch(finalEpoch, epochFormat);
      return true;
   }

   return GmatBase::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property of a ErrorModel
 *
 * @param label The text description of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string DataFilter::GetStringParameter(const std::string &label) const
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
bool DataFilter::SetStringParameter(const std::string &label,
      const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from a StringArray object
 *
 * @param id      The ID of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string DataFilter::GetStringParameter(const Integer id, const Integer index) const
{
   if (id == FILENAMES)
   {
      if ((index < 0)||(index >= (Integer)fileNames.size()))
      {
         std::stringstream ss;
         ss << "Error: file name's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return fileNames[index];
   }

   if (id == OBSERVED_OBJECTS)
   {
      if ((index < 0)||(index >= (Integer)observers.size()))
      {
         std::stringstream ss;
         ss << "Error: observer name's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return observers[index];
   }

   if (id == TRACKERS)
   {
      if ((index < 0)||(index >= (Integer)trackers.size()))
      {
         std::stringstream ss;
         ss << "Error: tracker name's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return trackers[index];
   }

   if (id == DATA_TYPES)
   {
      if ((index < 0)||(index >= (Integer)dataTypes.size()))
      {
         std::stringstream ss;
         ss << "Error: data type's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return dataTypes[index];
   }

   if (id == STRANDS)
   {
      if ((index < 0)||(index >= (Integer)strands.size()))
      {
         std::stringstream ss;
         ss << "Error: strand's index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return strands[index];
   }

   return GmatBase::GetStringParameter(id, index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value to a property of StringArray type
 *
 * @param id      The ID of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return true if the setting successes and false otherwise
 */
//------------------------------------------------------------------------------
bool DataFilter::SetStringParameter(const Integer id, const std::string &value,
                                           const Integer index)
{
   if (id == FILENAMES)
   {
      if (index == -1)
      {
         fileNames.clear();
         return true;
      } 
      else if ((0 <= index)&&(index <= (Integer)fileNames.size()))
      {
         if (value == "")
            throw MeasurementException("Error: cannot assign an empty string to file name.\n");

         if (index == fileNames.size())
            fileNames.push_back(value);
         else
            fileNames[index] = value;
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: file name's index (" << index << ") is out of bound.\n"; 
         throw GmatBaseException(ss.str());
      }
   }

   if (id == OBSERVED_OBJECTS)
   {
      if (index == -1)
      {
         observers.clear();
         return true;
      } 
      else if ((0 <= index)&&(index <= (Integer)observers.size()))
      {
         if (value == "")
            throw MeasurementException("Error: cannot assign an empty string to observer ID.\n");

         if (index == observers.size())
            observers.push_back(value);
         else
            observers[index] = value;
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: observer's index (" << index << ") is out of bound.\n"; 
         throw GmatBaseException(ss.str());
      }
   }

   if (id == TRACKERS)
   {
      if (index == -1)
      {
         trackers.clear();
         return true;
      } 
      else if ((0 <= index)&&(index <= (Integer)trackers.size()))
      {
         if (value == "")
            throw MeasurementException("Error: cannot accept an empty string to a tracker ID.\n");

         if (index == trackers.size())
            trackers.push_back(value);
         else
            trackers[index] = value;
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: tracker's index (" << index << ") is out of bound.\n"; 
         throw GmatBaseException(ss.str());
      }
   }

   if (id == DATA_TYPES)
   {
      if (index == -1)
      {
         dataTypes.clear();
         return true;
      } 
      else if ((0 <= index)&&(index <= (Integer)dataTypes.size()))
      {
         StringArray nameList = GetListOfMeasurementTypes();
         if (find(nameList.begin(), nameList.end(), value) == nameList.end())
            throw MeasurementException("Error: Value '" + value + "' set to " + GetName() + ".DataTypes parameter is invalid.\n");

         if (index == dataTypes.size())
            dataTypes.push_back(value);
         else
            dataTypes[index] = value;
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: data type's index (" << index << ") is out of bound.\n"; 
         throw GmatBaseException(ss.str());
      }
   }


   if (id == STRANDS)
   {
      if (index == -1)
      {
         strands.clear();
         return true;
      } 
      else if ((0 <= index)&&(index <= (Integer)strands.size()))
      {
         if (value == "")
            throw MeasurementException("Error: cannot assign an empty string to strand.\n");

         if (index == strands.size())
            strands.push_back(value);
         else
            strands[index] = value;
         return true;
      }
      else
      {
         std::stringstream ss;
         ss << "Error: strand's index (" << index << ") is out of bound.\n"; 
         throw GmatBaseException(ss.str());
      }
   }

   return GmatBase::SetStringParameter(id, value, index);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label, const Integer index) const
//------------------------------------------------------------------------------
/**
 * Retrieves a string property from a StringArray object
 *
 * @param labe    The name of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
std::string DataFilter::GetStringParameter(const std::string &label, const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Set value to a property of StringArray type
 *
 * @param label   The name of the property
 * @param index   The index of the property of StringArray type 
 *
 * @return true if the setting successes and false otherwise
 */
//------------------------------------------------------------------------------
bool DataFilter::SetStringParameter(const std::string &label, const std::string &value,
                                           const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}


//------------------------------------------------------------------------------
// const std::string GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray property
 *
 * @param id The ID of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
const StringArray& DataFilter::GetStringArrayParameter(const Integer id) const
{
   if (id == FILENAMES)
      return fileNames;

   if (id == OBSERVED_OBJECTS)
      return observers;

   if (id == TRACKERS)
      return trackers;

   if (id == DATA_TYPES)
      return dataTypes;

   if (id == STRANDS)
      return strands;

   return GmatBase::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const std::string GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Retrieves a StringArray property
 *
 * @param label   The name of the property
 *
 * @return The property value
 */
//------------------------------------------------------------------------------
const StringArray& DataFilter::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// Real ConvertToRealEpoch(const std::string &theEpoch,
//                         const std::string &theFormat)
//------------------------------------------------------------------------------
/**
 * Converts an epoch string is a specified format into
 *
 * @param theEpoch The input epoch
 * @param theFormat The format of the input epoch
 *
 * @return
 */
//------------------------------------------------------------------------------
Real DataFilter::ConvertToRealEpoch(const std::string &theEpoch,
                                   const std::string &theFormat)
{
   Real fromMjd = -999.999;
   Real retval = -999.999;
   std::string outStr;

   TimeConverterUtil::Convert(theFormat, fromMjd, theEpoch, "A1ModJulian",
         retval, outStr);

   if (retval == -999.999)
      throw GmatBaseException("Error converting the time string \"" + theEpoch +
            "\"; please check the format for the input string.");
   return retval;
}


StringArray DataFilter::GetListOfMeasurementTypes()
{
   StringArray typeList;

   //@todo: need to add any new measurement type here
   //typeList.push_back("Range");
   //typeList.push_back("DSNRange");
   //typeList.push_back("Doppler");
   //typeList.push_back("RangeRate");

   typeList.push_back("Range_KM");
   typeList.push_back("Range_RU");
   typeList.push_back("Doppler_Hz");
   typeList.push_back("Doppler_RangeRate");
   return typeList;
}


ObjectArray DataFilter::GetListOfSpacecrafts()
{
   // StringArray nameList = Moderator::Instance()->GetListOfObjects(Gmat::SPACECRAFT);
   StringArray nameList = GetListOfObjects(Gmat::SPACECRAFT);
   ObjectArray objectList;
   for (UnsignedInt i = 0; i < nameList.size(); ++i)
      objectList.push_back(GetConfiguredObject(nameList[i]));
      //objectList.push_back(Moderator::Instance()->GetConfiguredObject(nameList[i]));

   return objectList;
}


ObjectArray DataFilter::GetListOfGroundStations()
{
   //StringArray nameList = Moderator::Instance()->GetListOfObjects(Gmat::GROUND_STATION);
   StringArray nameList = GetListOfObjects(Gmat::GROUND_STATION);

   ObjectArray objectList;
   for (UnsignedInt i = 0; i < nameList.size(); ++i)
      objectList.push_back(GetConfiguredObject(nameList[i]));
      //objectList.push_back(Moderator::Instance()->GetConfiguredObject(nameList[i]));

   return objectList;
}


StringArray DataFilter::GetListOfValidEpochFormats()
{
   return TimeConverterUtil::GetListOfTimeSystemTypes();
}


bool DataFilter::ValidateInput()
{
   if (isChecked)
      return true;

#ifdef DEBUG_FILTER
   MessageInterface::ShowMessage("DataFilter<%s,%p>::ValidateInput()  enter\n", GetName().c_str(), this);
#endif

   ObjectArray objectList, objectList1;
   StringArray nameList, nameList1;
   bool found;

   bool retval = true;
   
   // 1. Verify observers:
   //MessageInterface::ShowMessage("Validate observers\n");
   objectList = GetListOfSpacecrafts();
   for (UnsignedInt i = 0; i < observers.size(); ++i)
   {
      // validate observers[i]
      found = false;
      for (UnsignedInt j = 0; j < objectList.size(); ++j)
      {
         if (observers[i] == objectList[j]->GetStringParameter("Id"))
         {
            found = true;
            break;
         }
      }

      if (!found)
      {
         //MessageInterface::ShowMessage("Data filter %s:\n%s\n", GetName().c_str(), this->GetGeneratingString().c_str());
         throw GmatBaseException("Error: observer with ID '" + observers[i] + "' set to parameter " + GetName() + ".ObservedObjects was not defined in script\n"); 
      }
   }

   // 2. Verify trackers:
   //MessageInterface::ShowMessage("Validate trackers\n");
   objectList1 = GetListOfGroundStations();
   for (UnsignedInt i = 0; i < trackers.size(); ++i)
   {
      // validate of trackers[i]
      found = false;
      for (UnsignedInt j = 0; j < objectList1.size(); ++j)
      {
         if (trackers[i] == objectList1[j]->GetStringParameter("Id"))
         {
            found = true;
            break;
         }
      }

      if (!found)
         throw GmatBaseException("Error: tracker '" + trackers[i] + "' which is set to parameter " + GetName() + ".Trackers was not defined in script\n"); 
   }

   // 3. Verify strands:
   //MessageInterface::ShowMessage("Validate strands\n");
   // 3.1. Get a list of all spacecrafts and ground stations
   for (UnsignedInt i = 0; i < objectList1.size(); ++i)
      objectList.push_back(objectList1[i]);

   // 3.2. Verify all strands
   for (UnsignedInt i = 0; i < strands.size(); ++i)
   {
      // 3.2.1. Get a strand in strand list:
      std::string strand = strands[i];
      
      //3.2.2. Get a partcipant from the strand and verify it in valid participant list  
      std::string::size_type pos = strand.find_first_of('-');
      while (pos != std::string::npos)
      {
         // Get participant
         std::string participant = "";
         if (pos > 0)
            participant = strand.substr(0, pos);

         // Verify particicpant
         found = false;
         for (UnsignedInt j = 0; j < objectList.size(); ++j)
         {
            if (participant == objectList[j]->GetStringParameter("Id"))
            {
               found = true;
               break;
            }
         }
         if (!found)
            throw GmatBaseException("Error: participant '" + participant + "' which is set to parameter " + GetName() + ".Strands was not defined in script\n"); 

         // Reset value of strand
         strand.substr(pos+1);
         pos = strand.find_first_of('-');
      }
   }

   // 6. Verify InitialEpoch:

   // 7. Verify FinalEpoch:

   isChecked = true;

#ifdef DEBUG_FILTER
   MessageInterface::ShowMessage("DataFilter<%s,%p>::ValidateInput()  exit\n", GetName().c_str(), this);
#endif

   return retval;
}


ObservationData* DataFilter::FilteringData(ObservationData* dataObject, Integer& rejectedReason)
{
   throw MeasurementException("Error: Do not allow to run DataFilter::FilteringData()\n");
   return NULL;
}



