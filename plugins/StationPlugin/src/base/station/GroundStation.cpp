//$Id$
//------------------------------------------------------------------------------
//                            GroundStation
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
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2008/08/01

// Modified:
//    2010.06.03 Tuan Nguyen
//      - Add AddHardware parameter and verify added hardware
//    2010.03.23 Steve Hughes/Thomas Grubb
//      - Changed initialize method to use statetype = "Spherical" instead
//        of deprecated "geographical" state type
//    2010.03.19 Thomas Grubb
//      - Overrode Copy method
//
/**
 * Implements the Groundstation class used to model ground based tracking stations.
 */
//------------------------------------------------------------------------------

#include "GroundStation.hpp"
#include "MessageInterface.hpp"
#include "AssetException.hpp"
#include "ColorTypes.hpp"               // for namespace GmatColor::


//#define DEBUG_OBJECT_MAPPING
//#define DEBUG_CONSTRUCTION
//#define DEBUG_INIT
//#define DEBUG_HARDWARE
//#define TEST_GROUNDSTATION
//#define DEBUG_AZEL_CONSTRAINT

//---------------------------------
// static data
//---------------------------------

/// Labels used for the ground station parameters.
const std::string
GroundStation::PARAMETER_TEXT[GroundStationParamCount - BodyFixedPointParamCount] =
   {
      "Id",
      "AddHardware",
      "IonosphereModel",
      "TroposphereModel",
      "DataSource",
      "Temperature",            // K- degree
      "Pressure",               // hPa
      "Humidity",               // percentage
      "MinimumElevationAngle",  // degree
      "ErrorModels",                   // ERROR_MODELS                    // made changes by TUAN NGUYEN
      //"RangeNoiseSigma",               // RANGE_NOISESIGMA
      //"RangeErrorModel",               // RANGE_ERRORMODEL
      //"DSNRangeNoiseSigma",            // DSNRANGE_NOISESIGMA
      //"DSNRangeErrorModel",            // DSNRANGE_ERRORMODEL
      //"DopplerNoiseSigma",             // DOPPLER_NOISESIGMA
      //"DopplerErrorModel",             // DOPPLER_ERRORMODEL
      //"RangeBias",                     // RANGE_BIAS
      //"DSNRangeBias",                  // DSNRANGE_BIAS
      //"DopplerBias",                   // DOPPLER_BIAS
   };

const Gmat::ParameterType
GroundStation::PARAMETER_TYPE[GroundStationParamCount - BodyFixedPointParamCount] =
   {
      Gmat::STRING_TYPE,
      Gmat::OBJECTARRAY_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::STRING_TYPE,
      Gmat::REAL_TYPE,      // Temperature
      Gmat::REAL_TYPE,      // Pressure
      Gmat::REAL_TYPE,      // Humidity
      Gmat::REAL_TYPE,      // MinimumElevationAngle
      Gmat::OBJECTARRAY_TYPE,      // ERROR_MODEL                          // made changes by TUAN NGUYEN
      //Gmat::REAL_TYPE,             // RANGE_NOISESIGMA
      //Gmat::STRING_TYPE,           // RANGE_ERROR_MODEL
      //Gmat::REAL_TYPE,             // DSNRANGE_NOISESIGMA
      //Gmat::STRING_TYPE,           // DSNRANGE_ERROR_MODEL
      //Gmat::REAL_TYPE,             // DOPPLER_NOISESIGMA
      //Gmat::STRING_TYPE,           // DOPPLER_ERROR_MODEL
      //Gmat::REAL_TYPE,             // RANGE_BIAS
      //Gmat::REAL_TYPE,             // DSNRANGE_BIAS
      //Gmat::REAL_TYPE,             // DOPPLER_BIAS
   };



//---------------------------------
// public methods
//---------------------------------


//---------------------------------------------------------------------------
//  GroundStation(const std::string &itsName)
//---------------------------------------------------------------------------
/**
 * Constructs a GroundStation object (default constructor).
 *
 * @param <itsName> Optional name for the object.  Defaults to "".
 */
//---------------------------------------------------------------------------
GroundStation::GroundStation(const std::string &itsName) :
   GroundstationInterface    ("GroundStation", itsName),
   stationId                 ("StationId"),
   temperature               (295.1),                // 295.1 K-degree
   pressure                  (1013.5),               // 1013.5 hPa
   humidity                  (55.0),                 // 55%
   dataSource                ("Constant"),
   minElevationAngle         (7.0),                  // 7 degree

   //rangeNoiseSigma           (1.0),                  // 1 Km            // This will be removed when code for ErrorModel is completed
   //rangeErrorModel           ("RandomConstant"),                        // This will be removed when code for ErrorModel is completed
   //dsnrangeNoiseSigma        (1.0),                  // 1 RU            // This will be removed when code for ErrorModel is completed
   //dsnrangeErrorModel        ("RandomConstant"),                        // This will be removed when code for ErrorModel is completed
   //dopplerNoiseSigma         (1.0),                  // 1 Hz            // This will be removed when code for ErrorModel is completed
   //dopplerErrorModel         ("RandomConstant"),                        // This will be removed when code for ErrorModel is completed
   //rangeBias                 (0.0),                  // unit: km        // This will be removed when code for ErrorModel is completed
   //dsnrangeBias              (0.0),                  // unit: RU        // This will be removed when code for ErrorModel is completed
   //dopplerBias               (0.0),                  // unit: Hz        // This will be removed when code for ErrorModel is completed

   troposphereModel          ("None"),
   ionosphereModel           ("None")
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GroundStation default constructor\n");
#endif

   objectTypeNames.push_back("GroundStation");
   parameterCount = GroundStationParamCount;
   
   // Set default colors
   SetDefaultColors(GmatColor::THISTLE, GmatColor::DARK_GRAY);
   
   bfcsName   = "EarthFixed";
   mj2kcsName = "EarthMJ2000Eq";
}

//---------------------------------------------------------------------------
// ~GroundStation()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
GroundStation::~GroundStation()
{
   // delete all hardware
   for (UnsignedInt i = 0; i < hardwareList.size(); ++i)
      if (hardwareList[i] != NULL)                          // made changes by TUAN NGUYEN
         delete hardwareList[i];

   // delete all error models                               // made changes by TUAN NGUYEN
   for (UnsignedInt i = 0; i < errorModels.size(); ++i)     // made changes by TUAN NGUYEN
      if (errorModels[i] != NULL)                           // made changes by TUAN NGUYEN
         delete errorModels[i];                             // made changes by TUAN NGUYEN

}

//---------------------------------------------------------------------------
//  GroundStation(const GroundStation& gs)
//---------------------------------------------------------------------------
/**
 * Constructs a new GroundStation by copying the input instance (copy
 * constructor).
 *
 * @param gs  GroundStation instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
GroundStation::GroundStation(const GroundStation& gs) :
   GroundstationInterface        (gs),
   stationId             (gs.stationId),
   temperature           (gs.temperature),
   pressure              (gs.pressure),
   humidity              (gs.humidity),
   dataSource            (gs.dataSource),
   minElevationAngle     (gs.minElevationAngle),

   errorModelNames       (gs.errorModelNames),             // made changes by TUAN NGUYEN

   //rangeNoiseSigma       (gs.rangeNoiseSigma),             // This will be removed when code for ErrorModel is completed
   //rangeErrorModel       (gs.rangeErrorModel),             // This will be removed when code for ErrorModel is completed
   //dsnrangeNoiseSigma    (gs.dsnrangeNoiseSigma),          // This will be removed when code for ErrorModel is completed
   //dsnrangeErrorModel    (gs.dsnrangeErrorModel),          // This will be removed when code for ErrorModel is completed
   //dopplerNoiseSigma     (gs.dopplerNoiseSigma),           // This will be removed when code for ErrorModel is completed
   //dopplerErrorModel     (gs.dopplerErrorModel),           // This will be removed when code for ErrorModel is completed
   //rangeBias             (gs.rangeBias),                   // This will be removed when code for ErrorModel is completed
   //dsnrangeBias          (gs.dsnrangeBias),                // This will be removed when code for ErrorModel is completed
   //dopplerBias           (gs.dopplerBias),                 // This will be removed when code for ErrorModel is completed
   
   ionosphereModel       (gs.ionosphereModel),
   troposphereModel      (gs.troposphereModel)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GroundStation copy constructor start\n");
#endif

   hardwareNames       = gs.hardwareNames;
// hardwareList        = gs.hardwareList;       // should it be cloned ????

#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GroundStation copy constructor end\n");
#endif
}


//---------------------------------------------------------------------------
//  GroundStation& operator=(const GroundStation& gs)
//---------------------------------------------------------------------------
/**
 * Assignment operator for GroundStations.
 *
 * @param gs The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
GroundStation& GroundStation::operator=(const GroundStation& gs)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GroundStation operator =\n");
#endif

   if (&gs != this)
   {
      GroundstationInterface::operator=(gs);

      stationId       = gs.stationId;
      hardwareNames   = gs.hardwareNames;
//    hardwareList    = gs.hardwareList;       // should it be cloned ????

      temperature     = gs.temperature;
      pressure        = gs.pressure;
      humidity        = gs.humidity;
      dataSource      = gs.dataSource;

      minElevationAngle  = gs.minElevationAngle;

      errorModelNames    = gs.errorModelNames;                // made changes by TUAN NGUYEN

      //rangeNoiseSigma    = gs.rangeNoiseSigma;                // This will be removed when code for ErrorModel is completed
      //rangeErrorModel    = gs.rangeErrorModel;                // This will be removed when code for ErrorModel is completed
      //dsnrangeNoiseSigma = gs.dsnrangeNoiseSigma;             // This will be removed when code for ErrorModel is completed
      //dsnrangeErrorModel = gs.dsnrangeErrorModel;             // This will be removed when code for ErrorModel is completed
      //dopplerNoiseSigma  = gs.dopplerNoiseSigma;              // This will be removed when code for ErrorModel is completed
      //dopplerErrorModel  = gs.dopplerErrorModel;              // This will be removed when code for ErrorModel is completed
      //rangeBias          = gs.rangeBias;                      // This will be removed when code for ErrorModel is completed
      //dsnrangeBias       = gs.dsnrangeBias;                   // This will be removed when code for ErrorModel is completed
      //dopplerBias        = gs.dopplerBias;                    // This will be removed when code for ErrorModel is completed

      troposphereModel  = gs.troposphereModel;
      ionosphereModel   = gs.ionosphereModel;

	  minElevationAngle = gs.minElevationAngle;
   }

   return *this;
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 *
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void GroundStation::Copy(const GmatBase* orig)
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GroundStation::Copy()\n");
#endif

   operator=(*((GroundStation *)(orig)));
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Makes a copy of this instance and returns it as a GmatBase pointer.
 *
 * @return The clone of the GroundStation, as a GmatBase pointer.
 */
//------------------------------------------------------------------------------
GmatBase* GroundStation::Clone() const
{
#ifdef DEBUG_CONSTRUCTION
   MessageInterface::ShowMessage("GroundStation::Clone()\n");
#endif

   return new GroundStation(*this);
}



//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer GroundStation::GetParameterID(const std::string & str) const
{
   //if ((str == "RangeErrorModel")||(str == "DSNRangeErrorModel")||(str == "DopplerErrorModel")||
   //    (str == "RangeBias")||(str == "DSNRangeBias")||(str == "DopplerBias")||
   //    (str == "RangeNoiseSigma")||(str == "DSNRangeNoiseSigma")||(str == "DopplerNoiseSigma")
   //   )
   //   throw GmatBaseException("Error: GroundStation has no parameter with name '" + str + "'\n");

   for (Integer i = BodyFixedPointParamCount; i < GroundStationParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - BodyFixedPointParamCount])
         return i;
   }

   return GroundstationInterface::GetParameterID(str);
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string GroundStation::GetParameterText(const Integer id) const
{
   if (id >= BodyFixedPointParamCount && id < GroundStationParamCount)
      return PARAMETER_TEXT[id - BodyFixedPointParamCount];
   return GroundstationInterface::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string GroundStation::GetParameterTypeString(const Integer id) const
{
   return GroundstationInterface::PARAM_TYPE_STRING[GetParameterType(id)];
}

//---------------------------------------------------------------------------
//  std::string GetParameterUnit(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the unit for the parameter.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return unit for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string GroundStation::GetParameterUnit(const Integer id) const
{
   return GroundstationInterface::GetParameterUnit(id);
}

//---------------------------------------------------------------------------
//  Gmat::ParameterType GetParameterType(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Retrieve the enumerated type of the object.
 *
 * @param <id> The integer ID for the parameter.
 *
 * @return The enumeration for the type of the parameter, or
 *         UNKNOWN_PARAMETER_TYPE.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType GroundStation::GetParameterType(const Integer id) const
{
   if (id >= BodyFixedPointParamCount && id < GroundStationParamCount)
      return PARAMETER_TYPE[id - BodyFixedPointParamCount];

   return GroundstationInterface::GetParameterType(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool GroundStation::IsParameterReadOnly(const Integer id) const
{
   return GroundstationInterface::IsParameterReadOnly(id);
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool GroundStation::IsParameterReadOnly(const std::string & label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter
 *
 * @param id The ID for the parameter
 *
 * @return The string parameter
 */
//------------------------------------------------------------------------------
std::string GroundStation::GetStringParameter(const Integer id) const
{
   if (id == STATION_ID)
      return stationId;

   if (id == TROPOSPHERE_MODEL)
      return troposphereModel;

   if (id == IONOSPHERE_MODEL)
      return ionosphereModel;

   if (id == DATA_SOURCE)
      return dataSource;

   //if (id == RANGE_ERRORMODEL)                 // This will be removed when code for ErrorModel is completed
   //   return rangeErrorModel;                  // This will be removed when code for ErrorModel is completed

   //if (id == DSNRANGE_ERRORMODEL)              // This will be removed when code for ErrorModel is completed
   //   return dsnrangeErrorModel;               // This will be removed when code for ErrorModel is completed

   //if (id == DOPPLER_ERRORMODEL)               // This will be removed when code for ErrorModel is completed
   //   return dopplerErrorModel;                // This will be removed when code for ErrorModel is completed

   return GroundstationInterface::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string & value)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter
 *
 * @param id The ID for the parameter
 * @param value The new string value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool GroundStation::SetStringParameter(const Integer id,
      const std::string & value)
{
   if (id == STATION_ID)
   {
 //   if (IsValidID(value))
 //   {
         stationId = value;
         return true;
 //   }
 //   else
 //   {
 //      AssetException ae;
 //      ae.SetDetails(errorMessageFormat.c_str(), value.c_str(), "Id",
 //                       "Must begin with a letter; may contain letters, integers, dashes, underscores");
 //      throw ae;
 //   }
   }

   if (id == ADD_HARDWARE)
   {
      // Only add the hardware if it is not in the list already
      if (find(hardwareNames.begin(), hardwareNames.end(), value) ==
                  hardwareNames.end())
      {
         hardwareNames.push_back(value);
      }
      return true;
   }

   if (id == TROPOSPHERE_MODEL)
   {
      if ((value != "HopfieldSaastamoinen")&&(value != "None"))
         throw AssetException("Error: '" + value +"' is not a valid name for TroposphereModel.\n"
         +"Currently only 'HopfieldSaastamoinen' and 'None' are allowed for Troposphere.\n");

      troposphereModel = value;
      return true;
   }

   if (id == IONOSPHERE_MODEL)
   {
      if ((value != "IRI2007")&&(value != "None"))
         throw AssetException("Error: '" + value + "' is not a valid name for IonosphereModel.\n"
         +"Currently only 'IRI2007' and 'None' are allowed for Ionosphere.\n");

      ionosphereModel = value;
      return true;
   }

   if (id == DATA_SOURCE)
   {
      if (dataSource != "Constant")
         throw AssetException("Error: '"+ value + "' is not a valid value for DataSource.\n");

      dataSource = value;
      return true;
   }

   if (id == ERROR_MODELS)                                                      // made changes by TUAN NGUYEN
   {                                                                            // made changes by TUAN NGUYEN
      // Only add error model if it is not in the list already                  // made changes by TUAN NGUYEN
      if (find(errorModelNames.begin(), errorModelNames.end(), value) ==        // made changes by TUAN NGUYEN
                  errorModelNames.end())                                        // made changes by TUAN NGUYEN
      {                                                                         // made changes by TUAN NGUYEN
         errorModelNames.push_back(value);                                      // made changes by TUAN NGUYEN
      }                                                                         // made changes by TUAN NGUYEN
      return true;                                                              // made changes by TUAN NGUYEN
   }                                                                            // made changes by TUAN NGUYEN

   //// @todo: This section will be removed when code for ErrorModel is completed
   //if (id == RANGE_ERRORMODEL)
   //{                                                                                                                    
   //   if (value != "RandomConstant")
   //      throw GmatBaseException("Error: '"+ value + "' is invalid value for " + GetName() + ".RangeErrorModel\n");
   //   rangeErrorModel = value;
   //   return true;
   //}

   //if (id == DSNRANGE_ERRORMODEL)
   //{
   //   if (value != "RandomConstant")
   //      throw GmatBaseException("Error: '"+ value + "' is invalid value for " + GetName() + ".DSNRangeErrorModel\n");
   //   dsnrangeErrorModel = value;
   //   return true;
   //}

   //if (id == DOPPLER_ERRORMODEL)
   //{
   //   if (value != "RandomConstant")
   //      throw GmatBaseException("Error: '" + value + "' is invalid value for " + GetName() + ".DopplerErrorModel\n");
   //   dopplerErrorModel = value;
   //   return true;
   //}


   return GroundstationInterface::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter
 *
 * @param label The string label for the parameter
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string GroundStation::GetStringParameter(const std::string & label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label, const std::string & value)
//------------------------------------------------------------------------------
/**
 * This method sets a string parameter
 *
 * @param label The string label for the parameter
 * @param value The new string value
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool GroundStation::SetStringParameter(const std::string & label,
      const std::string & value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param id The ID of the parameter
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string GroundStation::GetStringParameter(const Integer id,
      const Integer index) const
{
   if (index < 0)
   {
      AssetException ex;
      ex.SetDetails("The index %d is out-of-range for field \"%s\"", index,
                    GetParameterText(id).c_str());
      throw ex;
   }

   switch (id)
   {
      case ADD_HARDWARE:
         if ((0 <= index)&&(index < (Integer)hardwareNames.size()))
            return hardwareNames[index];
         else
            return "";
         break;

      case ERROR_MODELS:
         if ((0 <= index)&&(index < (Integer)errorModelNames.size()))
            return errorModelNames[index];
         else
            return "";
         break;

      default:
         break;      // intentional drop through
   }

   return GroundstationInterface::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string & label,
//       const Integer index) const
//------------------------------------------------------------------------------
/**
 * This method retrieves a string parameter from a StringArray
 *
 * @param label The string label for the parameter
 *
 * @return The parameter
 */
//------------------------------------------------------------------------------
std::string GroundStation::GetStringParameter(const std::string & label,
      const Integer index) const
{
   return GetStringParameter(GetParameterID(label), index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string & label,
//       const std::string & value, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method calls the base class method.  It is provided for overload
 * compatibility.  See the base class description for a full description.
 */
//------------------------------------------------------------------------------
bool GroundStation::SetStringParameter(const std::string & label,
      const std::string & value, const Integer index)
{
   return SetStringParameter(GetParameterID(label), value, index);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id,
//       const std::string & value, const Integer index)
//------------------------------------------------------------------------------
/**
 * Sets a specific string in a StringArray
 *
 * This method changes a specific string in a StringArray if a string has been
 * set at the location selected by the index value.  If the index exceeds the
 * size of the name array, the participant name is added to the end of the list.
 *
 * @param id The ID for the StringArray parameter that is being changed
 * @param value The string that needs to be placed in the StringArray
 * @param index The location for the string in the list.  If index exceeds the
 *              size of the StringArray, the string is added to the end of the
 *              array
 *
 * @return true If the string was processed
 */
//------------------------------------------------------------------------------
bool GroundStation::SetStringParameter(const Integer id,
      const std::string & value, const Integer index)
{
   if (index < 0)
   {
      AssetException ex;
      ex.SetDetails("The index %d is out-of-range for field \"%s\"", index,
                    GetParameterText(id).c_str());
      throw ex;
   }

   switch (id)
   {
   case ADD_HARDWARE:
      // Only add the hardware if it is not in the list already
      if (find(hardwareNames.begin(), hardwareNames.end(), value) ==
                  hardwareNames.end())
      {
         hardwareNames.push_back(value);
      }
      return true;

   case ERROR_MODELS:                                                           // made changes by TUAN NGUYEN
      // Only add the error model if it is not in the list already              // made changes by TUAN NGUYEN
      if (find(errorModelNames.begin(), errorModelNames.end(), value) ==        // made changes by TUAN NGUYEN
                  errorModelNames.end())                                        // made changes by TUAN NGUYEN
      {                                                                         // made changes by TUAN NGUYEN
         errorModelNames.push_back(value);                                      // made changes by TUAN NGUYEN
      }                                                                         // made changes by TUAN NGUYEN
      return true;                                                              // made changes by TUAN NGUYEN

   default:
      break;      // intentional drop through
   }

   return GroundstationInterface::SetStringParameter(id, value, index);
}

//---------------------------------------------------------------------------
//  const StringArray& GetStringArrayParameter(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Accesses StringArray parameters.
 *
 * @param id The integer ID for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//---------------------------------------------------------------------------
const StringArray& GroundStation::GetStringArrayParameter(const Integer id) const
{
   if (id == ADD_HARDWARE)
      return hardwareNames;

   if (id == ERROR_MODELS)                                                // made changes by TUAN NGUYEN
      return errorModelNames;                                             // made changes by TUAN NGUYEN

   return GroundstationInterface::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accesses StringArray parameters.
 *
 * @param label The script string for the parameter.
 *
 * @return The requested StringArray; throws if the parameter is not a
 *         StringArray.
 */
//------------------------------------------------------------------------------
const StringArray& GroundStation::GetStringArrayParameter(
      const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method gets a real parameter based on parameter id.
 *
 * @param <id>    id of a real parameter
 *
 * @return value of a real parameter.
 */
//------------------------------------------------------------------------------
Real GroundStation::GetRealParameter(const Integer id) const
{
   if (id == TEMPERATURE)
      return temperature;
   if (id == PRESSURE)
      return pressure;
   if (id == HUMIDITY)
      return humidity;

   if (id == MINIMUM_ELEVATION_ANGLE)
      return minElevationAngle;

   //if (id == RANGE_NOISESIGMA)                       // This will be removed when code for ErrorModel is completed
   //   return rangeNoiseSigma;                        // This will be removed when code for ErrorModel is completed

   //if (id == DSNRANGE_NOISESIGMA)                    // This will be removed when code for ErrorModel is completed
   //   return dsnrangeNoiseSigma;                     // This will be removed when code for ErrorModel is completed

   //if (id == DOPPLER_NOISESIGMA)                     // This will be removed when code for ErrorModel is completed
   //   return dopplerNoiseSigma;                      // This will be removed when code for ErrorModel is completed

   //if (id == RANGE_BIAS)                             // This will be removed when code for ErrorModel is completed
   //   return rangeBias;                              // This will be removed when code for ErrorModel is completed

   //if (id == DSNRANGE_BIAS)                          // This will be removed when code for ErrorModel is completed
   //   return dsnrangeBias;                           // This will be removed when code for ErrorModel is completed

   //if (id == DOPPLER_BIAS)                           // This will be removed when code for ErrorModel is completed
   //   return dopplerBias;                            // This will be removed when code for ErrorModel is completed

   return GroundstationInterface::GetRealParameter(id);
}


//------------------------------------------------------------------------------
//  Real SetRealParameter(const Integer id, const Real value) const
//------------------------------------------------------------------------------
/**
 * This method sets value to a real parameter based on parameter id.
 *
 * @param <id>       id of a real parameter
 * @param <value>    value used to set
 *
 * @return value of a real parameter.
 */
//------------------------------------------------------------------------------
Real GroundStation::SetRealParameter(const Integer id,
                                      const Real value)
{
   if (id == TEMPERATURE)
   {
      if ( value < 0.0)
         throw AssetException("Temperature set to " + GetName() + " does not allow to be a negative number\n");
      temperature = value;
      return temperature;
   }
   if (id == PRESSURE)
   {
      if ( value < 0.0)
         throw AssetException("Pressure set to " + GetName() + " does not allow to be a negative number\n");
      pressure = value;
      return pressure;
   }
   if (id == HUMIDITY)
   {
      if (( value < 0.0)||( value > 100.0))
         throw AssetException("Huminity set to " + GetName() + " is not in range [0.0, 100.0]\n");
      humidity = value;
      return humidity;
   }

   if (id == MINIMUM_ELEVATION_ANGLE)
   {
      if (( value < -90.0)||( value > 90.0))
         throw AssetException("Minimum elevation angle set to " + GetName() + " is not in range [-90.0o, 90.0o]\n");
      minElevationAngle = value;
      return minElevationAngle;
   }
   

   //// @todo: This section will be removed when code for ErrorModel is completed
   //if (id == RANGE_NOISESIGMA)
   //{
   //   if (value <= 0.0)
   //   {
   //      std::stringstream ss;
   //      ss << "Error: value of " << GetName() << ".RangeNoiseSigma (" << value << ") is invalid. It value should be a positive number.\n";
   //      throw GmatBaseException(ss.str());
   //   }
   //   rangeNoiseSigma = value;
   //   return rangeNoiseSigma;
   //}

   //if (id == DSNRANGE_NOISESIGMA)
   //{
   //   if (value <= 0.0)
   //   {
   //      std::stringstream ss;
   //      ss << "Error: value of " << GetName() << ".DSNRangeNoiseSigma (" << value << ") is invalid. It value should be a positive number.\n";
   //      throw GmatBaseException(ss.str());
   //   }
   //   dsnrangeNoiseSigma = value;
   //   return dsnrangeNoiseSigma;
   //}

   //if (id == DOPPLER_NOISESIGMA)
   //{
   //   if (value <= 0.0)
   //   {
   //      std::stringstream ss;
   //      ss << "Error: value of " << GetName() << ".DopplerNoiseSigma (" << value << ") is invalid. It value should be a positive number.\n";
   //      throw GmatBaseException(ss.str());
   //   }
   //   dopplerNoiseSigma = value;
   //   return dopplerNoiseSigma;
   //}


   //if (id == RANGE_BIAS)
   //{
   //   rangeBias = value;
   //   return rangeBias;
   //}

   //if (id == DSNRANGE_BIAS)
   //{
   //   dsnrangeBias = value;
   //   return dsnrangeBias;
   //}

   //if (id == DOPPLER_BIAS)
   //{
   //   dopplerBias = value;
   //   return dopplerBias;
   //}


   return GroundstationInterface::SetRealParameter(id, value);
}


Real GroundStation::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}

Real GroundStation::SetRealParameter(const std::string &label,
                                      const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
bool GroundStation::RenameRefObject(const Gmat::ObjectType type,
                                 const std::string &oldName,
                                 const std::string &newName)
{
   switch (type)                                            // made changes by TUAN NGUYEN
   {                                                        // made changes by TUAN NGUYEN
   case Gmat::HARDWARE:                                     // made changes by TUAN NGUYEN
      for (UnsignedInt i=0; i<hardwareNames.size(); i++)
      {
         if (hardwareNames[i] == oldName)
         {
            hardwareNames[i] = newName;
            break;
         }
      }
      return true;
      break;

   case Gmat::ERROR_MODEL:                                  // made changes by TUAN NGUYEN
      for (UnsignedInt i=0; i<errorModelNames.size(); i++)  // made changes by TUAN NGUYEN
      {                                                     // made changes by TUAN NGUYEN
         if (errorModelNames[i] == oldName)                 // made changes by TUAN NGUYEN
         {                                                  // made changes by TUAN NGUYEN
            errorModelNames[i] = newName;                   // made changes by TUAN NGUYEN
            break;                                          // made changes by TUAN NGUYEN
         }                                                  // made changes by TUAN NGUYEN
      }                                                     // made changes by TUAN NGUYEN
      return true;                                          // made changes by TUAN NGUYEN
      break;                                                // made changes by TUAN NGUYEN
   }

   return GroundstationInterface::RenameRefObject(type, oldName, newName);
}


//------------------------------------------------------------------------------
// const StringArray&
// GroundStation::GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Gets an array of referenced object names
 *
 * @param type The type of referenced objects
 *
 * @return a StringArray containing a list of the referenced object names
 */
//------------------------------------------------------------------------------
const StringArray&
GroundStation::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("GroundStation::GetRefObjectNameArray(%d)",
            type);
   #endif

   static StringArray retval;
   retval.clear();

   // for hardware
   if ((type == Gmat::UNKNOWN_OBJECT)||(type == Gmat::HARDWARE))
   {
      // Add all hardware names to retval array
      for (UnsignedInt i = 0; i < hardwareNames.size(); ++i)
      {
         if (find(retval.begin(), retval.end(), hardwareNames[i]) == retval.end())        // made changes by TUAN NGUYEN
            retval.push_back(hardwareNames[i]);
      }
   }
   
   // for error model
   if ((type == Gmat::UNKNOWN_OBJECT)||(type == Gmat::ERROR_MODEL))                       // made changes by TUAN NGUYEN
   {                                                                                      // made changes by TUAN NGUYEN
      // Add all error model names to retval array                                        // made changes by TUAN NGUYEN
      for (UnsignedInt i = 0; i < errorModelNames.size(); ++i)                            // made changes by TUAN NGUYEN
      {                                                                                   // made changes by TUAN NGUYEN
         if (find(retval.begin(), retval.end(), errorModelNames[i]) == retval.end())      // made changes by TUAN NGUYEN
            retval.push_back(errorModelNames[i]);                                         // made changes by TUAN NGUYEN
      }                                                                                   // made changes by TUAN NGUYEN
   }                                                                                      // made changes by TUAN NGUYEN


   // Now pick up the objects that the base classes need
   StringArray baseNames = GroundstationInterface::GetRefObjectNameArray(type);
   for (UnsignedInt i = 0; i < baseNames.size(); ++i)
   {
      if (find(retval.begin(), retval.end(), baseNames[i]) == retval.end())              // made changes by TUAN NGUYEN
         retval.push_back(baseNames[i]);
   }

   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("Groundstation Ref Object Name Array:\n");
      for (UnsignedInt i = 0; i < retval.size(); ++i)
         MessageInterface::ShowMessage("   %s\n", retval[i].c_str());
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// GmatBase* GroundStation::GetRefObject(const Gmat::ObjectType type,
//                                     const std::string &name)
//------------------------------------------------------------------------------
/**
 * Gets the referenced object specified by type and its name.
 *
 * @param type The type of the referenced object
 * @param name The name of the referenced object
 *
 * @return the referenced object match with type and name.
 */
//------------------------------------------------------------------------------
GmatBase* GroundStation::GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name)
{
   if ((type == Gmat::UNKNOWN_OBJECT)||(type == Gmat::HARDWARE))
   {
      for(ObjectArray::iterator i = hardwareList.begin();
               i < hardwareList.end(); ++i)
      {
         if ((*i)->GetName() == name)
            return (*i);
      }
   }

   if ((type == Gmat::UNKNOWN_OBJECT)||(type == Gmat::ERROR_MODEL))                 // made changes by TUAN NGUYEN
   {                                                                                // made changes by TUAN NGUYEN
      for(ObjectArray::iterator i = errorModels.begin();                            // made changes by TUAN NGUYEN
               i < errorModels.end(); ++i)                                          // made changes by TUAN NGUYEN
      {                                                                             // made changes by TUAN NGUYEN
         if ((*i)->GetName() == name)                                               // made changes by TUAN NGUYEN
            return (*i);                                                            // made changes by TUAN NGUYEN
      }                                                                             // made changes by TUAN NGUYEN
   }                                                                                // made changes by TUAN NGUYEN

   return GroundstationInterface::GetRefObject(type, name);
}

//------------------------------------------------------------------------------
// bool GroundStation::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                                     const std::string &name = "")
//------------------------------------------------------------------------------
/**
 * Sets the referenced object to a given object specified by type and its name.
 *
 * @param obj The object which is assigned to the referenced object.
 * @param type The type of the referenced object
 * @param name The name of the referenced object
 *
 * @return true when the assignment is successful.
 */
//------------------------------------------------------------------------------
bool GroundStation::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name)
{
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("GroundStation::SetRefObject(obj, "
            "type, name):  obj.name = %s  type = %d  name = %s\n",
            obj->GetName().c_str(), type, name.c_str());
   #endif

   if (obj == NULL)
      return false;

   switch (type)                                                                           // made changes by TUAN NGUYEN
   {                                                                                       // made changes by TUAN NGUYEN
   case Gmat::HARDWARE:   // work for hardware                                             // made changes by TUAN NGUYEN
      if (obj->GetType() == Gmat::HARDWARE)
      {
         // Don't add if it's already there
         bool hardwareRegistered = false;
         for (UnsignedInt i=0; i < hardwareList.size(); ++i)
         {
            if (hardwareList[i]->GetName() == obj->GetName())
            {
               hardwareRegistered = true;
               break;
            }
         }
         if (!hardwareRegistered)
         {
            hardwareList.push_back(obj->Clone());   // a hardware needs to be cloned
         }
         return true;
      }
      return false;      // <-- throw here; It was supposed to be hardware, but isn't.
      break;

   case Gmat::ERROR_MODEL:   // work for error model                                        // made changes by TUAN NGUYEN
      if (obj->GetType() == Gmat::ERROR_MODEL)                                              // made changes by TUAN NGUYEN
      {                                                                                     // made changes by TUAN NGUYEN
         // Don't add if it's already there                                                 // made changes by TUAN NGUYEN
         bool errormodelRegistered = false;                                                 // made changes by TUAN NGUYEN
         for (UnsignedInt i=0; i < errorModels.size(); ++i)                                 // made changes by TUAN NGUYEN
         {                                                                                  // made changes by TUAN NGUYEN
            if (errorModels[i]->GetName() == obj->GetName())                                // made changes by TUAN NGUYEN
            {                                                                               // made changes by TUAN NGUYEN
               errormodelRegistered = true;                                                 // made changes by TUAN NGUYEN
               break;                                                                       // made changes by TUAN NGUYEN
            }                                                                               // made changes by TUAN NGUYEN
         }                                                                                  // made changes by TUAN NGUYEN
         if (!errormodelRegistered)                                                         // made changes by TUAN NGUYEN
         {                                                                                  // made changes by TUAN NGUYEN
            errorModels.push_back(obj->Clone());   // a error model needs to be cloned      // made changes by TUAN NGUYEN
         }                                                                                  // made changes by TUAN NGUYEN
         return true;                                                                       // made changes by TUAN NGUYEN
      }                                                                                     // made changes by TUAN NGUYEN
      return false;      // <-- throw here; It was supposed to be error model, but isn't.   // made changes by TUAN NGUYEN
      break;                                                                                // made changes by TUAN NGUYEN
   }

   return GroundstationInterface::SetRefObject(obj, type, name);
}

ObjectArray& GroundStation::GetRefObjectArray(const Gmat::ObjectType type)
{
   switch(type)
   {
   case Gmat::HARDWARE:
      return hardwareList;
   
   case Gmat::ERROR_MODEL:                   // made changes by TUAN NGUYEN
      return errorModels;                    // made changes by TUAN NGUYEN
   
   default:
      break;
   }

   return GroundstationInterface::GetRefObjectArray(type);
}


ObjectArray& GroundStation::GetRefObjectArray(const std::string& typeString)
{
   if (typeString == "Hardware")
   {
      return hardwareList;
   }

   if (typeString == "ErrorModel")                // made changes by TUAN NGUYEN
   {                                              // made changes by TUAN NGUYEN
      return errorModels;                         // made changes by TUAN NGUYEN
   }                                              // made changes by TUAN NGUYEN

   return GroundstationInterface::GetRefObjectArray(typeString);
}



//------------------------------------------------------------------------------
// virtual bool HasRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool GroundStation::HasRefObjectTypeArray()
{
   return true;
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 *
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& GroundStation::GetRefObjectTypeArray()
{
   refObjectTypes.clear();

   GroundstationInterface::GetRefObjectTypeArray();

   refObjectTypes.push_back(Gmat::HARDWARE);
   refObjectTypes.push_back(Gmat::ERROR_MODEL);                            // made changes by TUAN NGUYEN
   
   return refObjectTypes;
}



//-------------------------------------------------------------------------
// This function is used to verify GroundStation's added hardware.
//
// return true if there is no error, false otherwise.
//-------------------------------------------------------------------------
bool GroundStation::VerifyAddHardware()
{
   Gmat::ObjectType type;
   std::string subTypeName;
   GmatBase* obj;

   // 1. Verify all hardware in hardwareList are not NULL:
   for(ObjectArray::iterator i= hardwareList.begin(); i != hardwareList.end(); ++i)
   {
      obj = (*i);
      if (obj == NULL)
      {
         MessageInterface::ShowMessage("***Error***:One element of hardwareList = NULL\n");
         return false;
      }
   }

   // 2. Verify primary antenna to be in hardwareList:
   // 2.1. Create antenna list from hardwareList for searching:
   // extract all antenna from hardwareList and store to antennaList
   ObjectArray antennaList;
   for(ObjectArray::iterator i= hardwareList.begin(); i != hardwareList.end(); ++i)
   {
      obj = (*i);
      subTypeName = obj->GetTypeName();
      if (subTypeName == "Antenna")
         antennaList.push_back(obj);
   }

   // 2.2. Verify primary antenna of Receiver, Transmitter, and Transponder:
   GmatBase* antenna;
   GmatBase* primaryAntenna;
   std::string primaryAntennaName;
   bool verify = true;
   for(ObjectArray::iterator i= hardwareList.begin(); i != hardwareList.end(); ++i)
   {
      obj = (*i);
      type = obj->GetType();
      if (type == Gmat::HARDWARE)
      {
         subTypeName = obj->GetTypeName();
         if ((subTypeName == "Transmitter")||
            (subTypeName == "Receiver")||
            (subTypeName == "Transponder"))
         {
            // Get primary antenna:
            primaryAntennaName = obj->GetRefObjectName(Gmat::HARDWARE);
            primaryAntenna = obj->GetRefObject(Gmat::HARDWARE,primaryAntennaName);

            bool check;
            if (primaryAntenna == NULL)
            {
               MessageInterface::ShowMessage
                ("***Error***:primary antenna of %s in %s's AddHardware list is not set \n",
                 obj->GetName().c_str(), this->GetName().c_str());
               check = false;
            }
            else
            {
               // Check primary antenna of transmitter, receiver, or transponder is in antenna list:
               check = false;
               for(ObjectArray::iterator j= antennaList.begin(); j != antennaList.end(); ++j)
               {
                  antenna = (*j);
                  if (antenna == primaryAntenna)
                  {
                     check = true;
                     break;
                  }
                  else if (antenna->GetName() == primaryAntenna->GetName())
                  {
                     MessageInterface::ShowMessage
                       ("Primary antenna %s of %s is a clone of an antenna in %s's AddHardware\n",
                       primaryAntenna->GetName().c_str(), obj->GetName().c_str(), this->GetName().c_str());
                  }
               }
               if (check == false)
               {
                  // Display error message:
                  MessageInterface::ShowMessage
                      ("***Error***:primary antenna of %s is not in %s's AddHardware\n",
                       obj->GetName().c_str(), this->GetName().c_str());
               }

            }

            verify = verify && check;
         }
      }
   }

   return verify;
}


//------------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------------
bool GroundStation::Initialize()
{
   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("GroundStation::Initializing %s\n", instanceName.c_str());
   #endif

   // Call the parent class to initialize its data
   GroundstationInterface::Initialize();

   // Initialize hardware data
   #ifdef DEBUG_HARDWARE
      MessageInterface::ShowMessage("Hardware list names:\n");
      for (UnsignedInt i = 0; i < hardwareNames.size(); ++i)
      {
         MessageInterface::ShowMessage("   %s\n", hardwareNames[i].c_str());
      }

      MessageInterface::ShowMessage("Hardware list objects:\n");
      for (UnsignedInt i = 0; i < hardwareList.size(); ++i)
      {
         MessageInterface::ShowMessage("   %s\n", hardwareList[i]->GetName().c_str());
      }
   #endif

   // Set the hardware interconnections
   for (ObjectArray::iterator i=hardwareList.begin(); i!=hardwareList.end(); ++i)
   {
      if ((*i)->IsOfType(Gmat::HARDWARE))
      {
         Hardware *current = (Hardware*)(*i);

         // Get the hardware reference list
         StringArray refs = current->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
         for (UnsignedInt j = 0; j < refs.size(); ++j)
         {
            #ifdef DEBUG_HARDWARE
               MessageInterface::ShowMessage("Connecting up %s for %s\n",
                     refs[j].c_str(), current->GetName().c_str());
            #endif

            for (UnsignedInt k = 0; k < hardwareList.size(); ++k)
            {
               if (hardwareList[k]->GetName() == refs[j])
                  current->SetRefObject(hardwareList[k],
                        hardwareList[k]->GetType(), hardwareList[k]->GetName());
            }
         }
      }
   }


   // verify GroundStation's referenced objects
   if (VerifyAddHardware() == false)   // verify add hardware
      return false;

   #ifdef DEBUG_INIT
      MessageInterface::ShowMessage("GroundStation::Initializing %s  exit\n", instanceName.c_str());
   #endif

   return true;
}


bool GroundStation::IsEstimationParameterValid(const Integer id)
{
   bool retval = false;

   return retval;
}

Integer GroundStation::GetEstimationParameterSize(const Integer item)
{
   Integer retval = 1;

//   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   retval = GroundstationInterface::GetEstimationParameterSize(item);

   return retval;
}


Real* GroundStation::GetEstimationParameterValue(const Integer item)
{
   Real *retval = NULL;

//   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   retval = GroundstationInterface::GetEstimationParameterValue(item);

   return retval;
}

//------------------------------------------------------------------------------
// bool  IsValidID(const std::string &id)
//------------------------------------------------------------------------------
/**
 * Checks to see if the input string represents a valid station ID.
 *
 * As of 2010.12.23, valid IDs must:
 *    - start with a letter
 * As of 2010.12.23, valid IDs may:
 *    - contain letters & integers
 *    - contain only the following special characters:  dashes, underscores
 *
 * @param id string for the station ID.
 *
 * @return true if input is a valid ID; false otherwise.
 */
//------------------------------------------------------------------------------
// Note: stationID can be any string.
bool GroundStation::IsValidID(const std::string &id)
{
//   // first character must be a letter
//   if (!isalpha(id[0]))  return false;

//   // each character must be a letter, an integer, a dash, or an underscore
//   unsigned int sz = id.size();
//   for (unsigned int ii = 0; ii < sz; ii++)
//      if (!isalnum(id[ii]) && id[ii] != '-' && id[ii] != '_') return false;

   return true;
}


//------------------------------------------------------------------------------
// Real* IsValidElevationAngle(const Rvector6 &state_sez)
//------------------------------------------------------------------------------
/**
 * Performs the elevation angle check at a groundstation
 *
 * @param state_sez The outgoing topocentric vector in the direction tested
 *
 * @return a vector of el, az, and the elevation above the minimum
 */
//------------------------------------------------------------------------------
Real* GroundStation::IsValidElevationAngle(const Rvector6 &state_sez)
{
   // Get topocentric range and rangerate
   Rvector3 rho_sez = state_sez.GetR();
   Rvector3 rhodot_sez = state_sez.GetV();

   //// Compute satellite elevation
   //Real rho_sez_mag = rho_sez.GetMagnitude();
   //az_el_visible[0] = GmatMathUtil::ATan2(rho_sez[2],rho_sez_mag) *
   //      GmatMathConstants::DEG_PER_RAD;

   //// c=cos s=sin compute azimuth, protect against 90 deg elevation
   //Real c_rho =  rho_sez[1]/sqrt(GmatMathUtil::Pow(rho_sez[0],2) +
   //      GmatMathUtil::Pow(rho_sez[1],2));
   //Real s_rho = -rho_sez[0]/sqrt(GmatMathUtil::Pow(rho_sez[0],2) +
   //      GmatMathUtil::Pow(rho_sez[1],2));
   //Real c_rhodot =  rhodot_sez[1]/sqrt(GmatMathUtil::Pow(rhodot_sez[0],2) +
   //      GmatMathUtil::Pow(rhodot_sez[1],2));
   //Real s_rhodot = -rhodot_sez[0]/sqrt(GmatMathUtil::Pow(rhodot_sez[0],2) +
   //      GmatMathUtil::Pow(rhodot_sez[1],2));

   //// Compute azimuth
   //if (az_el_visible[0] != 90)
   //{
   //   az_el_visible[1] = GmatMathUtil::ATan2(s_rho,c_rho) *
   //         GmatMathConstants::DEG_PER_RAD;
   //}
   //else if (az_el_visible[0] == 90)
   //{
   //   az_el_visible[1] = GmatMathUtil::ATan2(s_rhodot,c_rhodot)*
   //       GmatMathConstants::DEG_PER_RAD;
   //}

   Rvector3 rho_sez_unit = rho_sez.GetUnitVector();
   Rvector3 rhodot_sez_unit = rhodot_sez.GetUnitVector();
   az_el_visible[0] = asin(rho_sez_unit[2])*GmatMathConstants::DEG_PER_RAD;
   az_el_visible[1] = asin(rhodot_sez_unit[2])*GmatMathConstants::DEG_PER_RAD;
   az_el_visible[2] = az_el_visible[0] - minElevationAngle;

   #ifdef DEBUG_AZEL_CONSTRAINT
      MessageInterface::ShowMessage("Satellite az = %9.4lf degs, "
            "satellite el = %8.4lf degs. Is visible = %8.4lf\n",
            az_el_visible[1], az_el_visible[0], az_el_visible[2]);
   #endif

   return az_el_visible;
}

