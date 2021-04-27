//$Id: ErrorModel.cpp 1398 2015-01-07 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                                 ErrorModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Tuan Dang Nguyen, NASA/GSFC.
// Created: 2015/01/07
//
/**
 * Implementation for the ErrorModel class
 */
//------------------------------------------------------------------------------


#include "ErrorModel.hpp"
#include "StringUtil.hpp"
#include "GmatBase.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"
#include <sstream>


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string ErrorModel::PARAMETER_TEXT[] =
{
   "Type",
//   "Trip",
//   "Strand",
   "NoiseSigma",
//   "NoiseModel",
   "Bias",
   "BiasSigma",
   "SolveFors",
   "Id",
};

const Gmat::ParameterType ErrorModel::PARAMETER_TYPE[] =
{
   Gmat::STRING_TYPE,			// TYPE                  // Its value will be ("Range_KM") "Range", "SN_Range", "DSN_Seq_Range", ("Doppler_RangeRate") "RangeRate", "DSN_TCP", ("TDRSDoppler_HZ") "SN_Doppler"
//   Gmat::INTEGER_TYPE,			// TRIP                  // Its value is 1 for one-way, 2 for two-way, 3 for three-way, and so on
//   Gmat::STRINGARRAY_TYPE,    // STRAND                // containing a name list of participants along signal path
   Gmat::REAL_TYPE,			   // NOISE_SIGMA           // Measurement noise sigma value
//   Gmat::STRING_TYPE,         // NOISE_MODEL           // Specify model of error. It is "RandomConstant" for Gausian distribution 
   Gmat::REAL_TYPE,			   // BIAS                  // Measurement bias
   Gmat::REAL_TYPE,			   // BIAS_SIGMA            // Measurement bias sigma
   Gmat::STRINGARRAY_TYPE,    // SOLVEFORS             // Contains a list of all solve-for parameters in ErrorModel 
   Gmat::STRING_TYPE,         // MODEL_ID              // The id of the error model, which is the instanceName, but using objects' ids instead of names 
};



//------------------------------------------------------------------------------
// ErrorModel(const std::string name)
//------------------------------------------------------------------------------
/**
 * Constructor for ErrorModel objects
 *
 * @param name The name of the object
 */
//------------------------------------------------------------------------------
ErrorModel::ErrorModel(const std::string name) :
   GmatBase          (Gmat::ERROR_MODEL, "ErrorModel", name),
   measurementType   ("DSN_SeqRange"),
// measurementTrip   (2),
// strand            (""),
   noiseSigma        (103.0),                 // (0.01),                  // 0.01 measurement unit (km, RU, Km/s, or Hz)
// noiseModel        ("NoiseConstant"), 
   bias              (0.0),                   // 0.0 measurement unit (km, RU, Km/s, or Hz)
   biasSigma         (1.0e70),                // 0.0 measurement unit (km, RU, Km/s, or Hz)
   modelId           ("ErrorModelId")
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::ERROR_MODEL);
   objectTypeNames.push_back("ErrorModel");

   parameterCount = ErrorModelParamCount;

   covariance.AddCovarianceElement("Bias", this);
   covariance(0, 0) = biasSigma*biasSigma;

//   // define deprecated type map
//   depTypeMap["Range_RU"]   = "DSN_SeqRange";
//   depTypeMap["Doppler_HZ"] = "DSN_TCP";
}


//------------------------------------------------------------------------------
// ~ErrorModel()
//------------------------------------------------------------------------------
/**
 * ErrorModel destructor
 */
//------------------------------------------------------------------------------
ErrorModel::~ErrorModel()
{
}


//------------------------------------------------------------------------------
// ErrorModel(const ErrorModel& em)
//------------------------------------------------------------------------------
/**
 * Copy constructor for a ErrorModel
 *
 * @param em        The ErrorModel object that provides data for the new one
 */
//------------------------------------------------------------------------------
ErrorModel::ErrorModel(const ErrorModel& em) :
   GmatBase              (em),
   measurementType       (em.measurementType),
//   measurementTrip       (em.measurementTrip),
//   participantNameList   (em.participantNameList),
   noiseSigma            (em.noiseSigma),
//   noiseModel            (em.noiseModel),
   bias                  (em.bias),
   biasSigma             (em.biasSigma),
   solveforNames         (em.solveforNames),
   modelId               (em.modelId)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel copy constructor from <%s,%p>  to  <%s,%p>\n", em.GetName().c_str(), &em, GetName().c_str(), this);
#endif

   Integer locationStart = covariance.GetSubMatrixLocationStart("Bias");
   covariance(locationStart, locationStart) = biasSigma*biasSigma;

}


//------------------------------------------------------------------------------
// ErrorModel& operator=(const ErrorModel& em)
//------------------------------------------------------------------------------
/**
 * ErrorModel assignment operator
 *
 * @param em    The ErrorModel object that provides data for the this one
 *
 * @return This object, configured to match em
 */
//------------------------------------------------------------------------------
ErrorModel& ErrorModel::operator=(const ErrorModel& em)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel operator = <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &em)
   {
      GmatBase::operator=(em);

      measurementType     = em.measurementType;
//      measurementTrip     = em.measurementTrip;
//      participantNameList = em.participantNameList;
      noiseSigma          = em.noiseSigma;
//      noiseModel          = em.noiseModel;
      bias                = em.bias;
      biasSigma           = em.biasSigma;
      solveforNames       = em.solveforNames;
      modelId             = em.modelId;

      Integer locationStart = covariance.GetSubMatrixLocationStart("Bias");
      covariance(locationStart, locationStart) = biasSigma * biasSigma;

   }

   return *this;
}


//------------------------------------------------------------------------------
// bool operator==(const ErrorModel& em)
//------------------------------------------------------------------------------
/**
 * ErrorModel equal comparing operator
 *
 * @param em    The ErrorModel object that provides data for the this one
 *
 * @return      true if both error model objects having the same measurement 
 *              type, trip, and strand; false otherwise
 */
//------------------------------------------------------------------------------
bool ErrorModel::operator==(const ErrorModel& em)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel operator == <%s,%p>\n", GetName().c_str(), this);
#endif

   if (this != &em)
   {
      //retVal = GmatBase::operator==(em);

      if ((GetName() != em.GetName())||
          (measurementType != em.measurementType)   //||
//          (measurementTrip != em.measurementTrip)||
//          (participantNameList != em.participantNameList)
         )
         return false;
   }
   return true;
}


//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone method for ErrorModel
 *
 * @return A clone of this object.
 */
//------------------------------------------------------------------------------
GmatBase* ErrorModel::Clone() const
{
   return new ErrorModel(*this);
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
bool ErrorModel::Initialize()
{
#ifdef DEBUG_INITIALIZATION
	MessageInterface::ShowMessage("ErrorModel<%s,%p>::Initialize()   entered\n", GetName().c_str(), this);
#endif
   if (isInitialized)
      return true;

   bool retval = true;

#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("ErrorModel<%s,%p>::Initialize()   exit\n", GetName().c_str(), this);
#endif

   isInitialized = retval;
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
bool ErrorModel::Finalize()
{
   bool retval = false;

   return retval;
}


//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Retrieves the text string used to script a ErrorModel property
 *
 * @param id The ID of the property
 *
 * @return The string
 */
//------------------------------------------------------------------------------
std::string ErrorModel::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ErrorModelParamCount)
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
std::string ErrorModel::GetParameterUnit(const Integer id) const
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
Integer ErrorModel::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < ErrorModelParamCount; i++)
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
 * Retrieves the parameter type for a property
 *
 * @param id The ID of the property
 *
 * @return The ParameterType of the property
 */
//------------------------------------------------------------------------------
Gmat::ParameterType ErrorModel::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < ErrorModelParamCount)
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
std::string ErrorModel::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false if not,
 */
 //---------------------------------------------------------------------------
bool ErrorModel::IsParameterReadOnly(const Integer id) const
{
   if (id == MODEL_ID)
      return true;

   return GmatBase::IsParameterReadOnly(id);
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
bool ErrorModel::IsParameterReadOnly(const std::string &label) const
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
std::string ErrorModel::GetStringParameter(const Integer id) const
{
   if (id == TYPE)
      return measurementType;

   //if (id == NOISE_MODEL)
   //   return noiseModel;

   if (id == MODEL_ID)
      return modelId;

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
bool ErrorModel::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == SOLVEFORS)
   {
      // if it is an empty list, then clear the solve-for list
      if (GmatStringUtil::RemoveSpaceInBrackets(value, "{}") == "{}")
      {
         solveforNames.clear();
         return true;
      }

      // If is not "{}", it is a string containing solve-for variable  
      // Check for valid identity
      if (value != "Bias")
         throw GmatBaseException("Error: '" + value + "' is an invalid value. "
         + GetName() + ".SolveFors parameter only accepts Bias as a solve-for.\n");

      if (find(solveforNames.begin(), solveforNames.end(), value) == solveforNames.end())
         solveforNames.push_back(value);
      else
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".SolveFors parameter is replicated.\n");

      return true;
   }

   if (id == TYPE)
   {
      
      std::string value1 = value;
      //value1 = CheckTypeDeprecation(value1);

      // Get a list of all available types
      StringArray typesList = GetAllAvailableTypes();
      bool found = false;
      for (Integer i = 0; i < typesList.size(); ++i)
      {
         if (typesList[i] == value1)
         {
            found = true;
            break;
         }
      }
      if (!found)
         throw MeasurementException("Error: '" + value + "' set to " + GetName() + ".Type parameter is an invalid measurement type.\n");

      measurementType = value1;
      return true;
   }

   //if (id == NOISE_MODEL)
   //{
   //   if (value != "RandomConstant")
   //   {
   //      throw GmatBaseException("Error: " + GetName() + "." + GetParameterText(id) + " cannot accept '" + value +"'\n");
   //      return false;
   //   }
   //   else
   //   {
   //      noiseModel = value;    
	  //    return true;
   //   }
   //}

   if (id == MODEL_ID)
   {
      modelId = value;    
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
std::string ErrorModel::GetStringParameter(const std::string &label) const
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
bool ErrorModel::SetStringParameter(const std::string &label,
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
std::string ErrorModel::GetStringParameter(const Integer id, const Integer index) const
{
   //if (id == STRAND)
   //{
   //   if ((index < 0)||(index >= (Integer)participantNameList.size()))
   //   {
   //      std::stringstream ss;
   //      ss << "Error: participant index (" << index << ") is out of bound.\n";
   //      throw GmatBaseException(ss.str());
   //   }

   //   return participantNameList[index];
   //}

   if (id == SOLVEFORS)
   {
      if ((index < 0)||(index >= (Integer)solveforNames.size()))
      {
         std::stringstream ss;
         ss << "Error: solve-for index (" << index << ") is out of bound.\n";
         throw GmatBaseException(ss.str());
      }

      return solveforNames[index];
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
bool ErrorModel::SetStringParameter(const Integer id, const std::string &value,
                                           const Integer index)
{
   //if (id == STRAND)
   //{
   //   if ((0 <= index)&&(index < (Integer)participantNameList.size()))
   //   {
   //      participantNameList[index] = value;
   //      return true;
   //   }
   //   else
   //   {
   //      if (index == -1)
   //      {
   //         participantNameList.clear();
   //         return true;
   //      }
   //      else if (index == participantNameList.size())
   //      {
   //         participantNameList.push_back(value);
   //         return true;
   //      }
   //      else
   //      {
   //         std::stringstream ss;
   //         ss << "Error: participant name's index (" << index << ") is out of bound.\n"; 
   //         throw GmatBaseException(ss.str());
   //      }
   //   }
   //}

   if (id == SOLVEFORS)
   {
      // an empty list is set to SolveFors parameter when index == -1
      if (index == -1)
      {
         solveforNames.clear();
         return true;
      }

      if (value != "Bias")
         throw GmatBaseException("Error: '" + value + "' is an invalid value. "
         + GetName() + ".SolveFors parameter only accepts Bias as a solve-for.\n");

      if ((0 <= index)&&(index < (Integer)solveforNames.size()))
      {
         solveforNames[index] = value;
         return true;
      }
      else
      {
         if (index == -1)
         {
            solveforNames.clear();
            return true;
         }
         else if (index == solveforNames.size())
         {
            solveforNames.push_back(value);
            return true;
         }
         else
         {
            std::stringstream ss;
            ss << "Error: solve-for's index (" << index << ") is out of bound.\n";
            throw GmatBaseException(ss.str());
         }
      }
      return true;
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
std::string ErrorModel::GetStringParameter(const std::string &label, const Integer index) const
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
bool ErrorModel::SetStringParameter(const std::string &label, const std::string &value,
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
const StringArray& ErrorModel::GetStringArrayParameter(const Integer id) const
{
   //if (id == STRAND)
   //   return participantNameList;

   if (id == SOLVEFORS)
      return solveforNames;

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
const StringArray& ErrorModel::GetStringArrayParameter(const std::string &label) const
{
   return GetStringArrayParameter(GetParameterID(label));
}



Real ErrorModel::GetRealParameter(const Integer id) const
{
   if (id == NOISE_SIGMA)
      return noiseSigma;

   if (id == BIAS)
      return bias;

   if (id == BIAS_SIGMA)
      return biasSigma;

   return GmatBase::GetRealParameter(id);
}


Real ErrorModel::SetRealParameter(const Integer id, const Real value)
{
#ifdef DEBUG_INITIALIZATION
   MessageInterface::ShowMessage("ErrorModel::SetRealParameter(id = %d, value = %le\n", id, value);
#endif
   if (id == NOISE_SIGMA)
   {
      if (value <= 0.0)
         throw GmatBaseException("Error: value set to "+ GetName() +".NoiseSigma is a non positive number. It has to be a positive number.\n");

	  noiseSigma = value;
	  return noiseSigma;
   }

   if (id == BIAS)
   {
	  bias = value;
	  return bias;
   }

   if (id == BIAS_SIGMA)
   {
      if (value <= 0.0)
         throw GmatBaseException("Error: value set to " + GetName() + ".BiasSigma is a nonpositive number. It has to be a positive number.\n");

      biasSigma = value;
      Integer locationStart = covariance.GetSubMatrixLocationStart("Bias");
      covariance(locationStart, locationStart) = biasSigma * biasSigma;

      return biasSigma;
   }

   return GmatBase::SetRealParameter(id, value);
}


Real ErrorModel::GetRealParameter(const std::string& label) const
{
	return GetRealParameter(GetParameterID(label));
}


Real ErrorModel::SetRealParameter(const std::string& label, const Real value)
{
	return SetRealParameter(GetParameterID(label), value);
}



Integer ErrorModel::GetIntegerParameter(const Integer id) const
{
   //if (id == TRIP)
   //   return measurementTrip;

   return GmatBase::GetIntegerParameter(id);
}


Integer ErrorModel::SetIntegerParameter(const Integer id, const Integer value)
{
   //if (id == TRIP)
   //{
   //   if (value < 0)
   //      throw GmatBaseException("Error: value of "+ GetName() +".Trip has to be a non-positive integer.\n");

	  // measurementTrip = value;
	  // return measurementTrip;
   //}

   return GmatBase::SetIntegerParameter(id, value);
}


Integer ErrorModel::GetIntegerParameter(const std::string& label) const
{
	return GetIntegerParameter(GetParameterID(label));
}


Integer ErrorModel::SetIntegerParameter(const std::string& label, const Integer value)
{
	return SetIntegerParameter(GetParameterID(label), value);
}



//------------------------------------------------------------------------------
// bool IsEstimationParameterValid(const Integer item)
//------------------------------------------------------------------------------
/**
* This function is used to verify an estimation paramter is either valid or not
*
* @param item      Estimation parameter ID (Note that: it is defferent from object ParameterID)
*
* return           true if it is valid, false otherwise 
*/
//------------------------------------------------------------------------------
bool ErrorModel::IsEstimationParameterValid(const Integer item)
{
   bool retval = false;

   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;    // convert Estimation ID to object parameter ID

   switch (id)
   {
      case BIAS:
         retval = true;
         break;

      // All other values call up the hierarchy
      default:
         retval = GmatBase::IsEstimationParameterValid(item);
   }

   return retval;
}


//------------------------------------------------------------------------------
// Integer GetEstimationParameterSize(const Integer item)
//------------------------------------------------------------------------------
Integer ErrorModel::GetEstimationParameterSize(const Integer item)
{
   Integer retval = 1;


   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   #ifdef DEBUG_ESTIMATION
      MessageInterface::ShowMessage("ErrorModel::GetEstimationParameterSize(%d)"
            " called; parameter ID is %d\n", item, id);
   #endif


   switch (id)
   {
      case BIAS:
         retval = 1;
         break;

      // All other values call up the hierarchy
      default:
         retval = GmatBase::GetEstimationParameterSize(item);
   }

   return retval;
}

//------------------------------------------------------------------------------
// Real* GetEstimationParameterValue(const Integer item)
//------------------------------------------------------------------------------
Real* ErrorModel::GetEstimationParameterValue(const Integer item)
{
   Real* retval = NULL;

   Integer id = item - type * ESTIMATION_TYPE_ALLOCATION;

   switch (id)
   {
      case BIAS:
         retval = &bias;
         break;

      // All other values call up the class heirarchy
      default:
         retval = GmatBase::GetEstimationParameterValue(item);
   }

   return retval;
}


StringArray ErrorModel::GetAllAvailableTypes()
{
   static StringArray sa;
   sa.clear();

   sa.push_back("DSN_SeqRange");
   sa.push_back("DSN_TCP");
   sa.push_back("GPS_PosVec");
   sa.push_back("Range");
   sa.push_back("Range_Skin");
   sa.push_back("RangeRate");
   sa.push_back("Azimuth");
   sa.push_back("Elevation");
   sa.push_back("XEast");
   sa.push_back("YNorth");
   sa.push_back("XSouth");
   sa.push_back("YEast");
   //sa.push_back("RightAscension");                           // made changes by TUAN NGUYEN
   //sa.push_back("Declination");                              // made changes by TUAN NGUYEN

   Integer runmode = GmatGlobal::Instance()->GetRunModeStartUp();        // fix bug: GMT-5955
   if (runmode == GmatGlobal::TESTING)                                   // fix bug: GMT-5955
   {
      sa.push_back("RightAscension");                          // made changes by TUAN NGUYEN
      sa.push_back("Declination");                             // made changes by TUAN NGUYEN
      sa.push_back("SN_Range");
      sa.push_back("SN_Doppler");                                        // fix bug: GMT-5955
   }

   return sa;
}


//-------------------------------------------------------------------------
// Integer Spacecraft::HasParameterCovariances(Integer parameterId)
//-------------------------------------------------------------------------
/**
* This function is used to verify whether a parameter (with ID specified by
* parameterId) having a covariance or not.
*
* @param parameterId      ID of a parameter
* @return                 size of covarian matrix associated with the parameter
*                         return -1 when the parameter has no covariance
*/
//-------------------------------------------------------------------------
Integer ErrorModel::HasParameterCovariances(Integer parameterId)
{
   if (parameterId == BIAS)
      return 1;

   return GmatBase::HasParameterCovariances(parameterId);
}


//------------------------------------------------------------------------------
// Rmatrix* GetParameterCovariances(Integer parameterId)
//------------------------------------------------------------------------------
/**
* Get covariance matrix for a given parameter.
*
* @param parameterId      ID of a parameter
* @return                 a pointer to covariance matrix associated with the parameter
*                         return NULL when the parameter has no covariance
*/
//-------------------------------------------------------------------------
Rmatrix* ErrorModel::GetParameterCovariances(Integer parameterId)
{
   if (isInitialized)
      return covariance.GetCovariance(parameterId);
   else
      throw GmatBaseException("Error: cannot get " + GetName() + " spacecraft's covariance when it is not initialized.\n");
   return NULL;
}


//std::string ErrorModel::CheckTypeDeprecation(const std::string datatype)
//{
//   std::string typeName = datatype;
//   for (std::map<std::string, std::string>::iterator i = depTypeMap.begin();
//      i != depTypeMap.end(); ++i)
//   {
//      if ((*i).first == datatype)
//      {
//         MessageInterface::ShowMessage("Warning: " + GetName() + ".Type name '" 
//            + datatype + "' is deprecated and will be removed in a future release. Use '"
//            + (*i).second + "' instead.\n");
//         typeName = (*i).second;
//         break;
//      }
//   }
//
//   return typeName;
//}

