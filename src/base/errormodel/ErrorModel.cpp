//$Id: ErrorModel.cpp 1398 2015-01-07 20:39:37Z tdnguyen $
//------------------------------------------------------------------------------
//                                 ErrorModel
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
// Created: 2015/01/07
//
/**
 * Implementation for the ErrorModel class
 */
//------------------------------------------------------------------------------


#include "ErrorModel.hpp"
#include "GmatBase.hpp"
#include "MessageInterface.hpp"
//#include "MeasurementException.hpp"


//#define DEBUG_CONSTRUCTION
//#define DEBUG_INITIALIZATION

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const std::string ErrorModel::PARAMETER_TEXT[] =
{
   "Type",
   "Trip",
   "Strand",
   "NoiseSigma",
   "NoiseModel",
   "Bias",
   "SolveMode",
};

const Gmat::ParameterType ErrorModel::PARAMETER_TYPE[] =
{
   Gmat::STRING_TYPE,			// TYPE                  // Its value will be "Range_KM", "Range_RU", "Doppler_RangeRate", "Doppler_Hz"
   Gmat::INTEGER_TYPE,			// TRIP                  // Its value is 1 for one-way, 2 for two-way, 3 for three-way, and so on
   Gmat::STRING_TYPE,		   // STRAND                // Its value is a string containing signal path of measurement
   Gmat::REAL_TYPE,			   // NOISE_SIGMA           // Measurement noise sigma value
   Gmat::STRING_TYPE,         // NOISE_MODEL           // Specify model of error. It is "RandomConstant" for Gausian distribution 
   Gmat::REAL_TYPE,			   // BIAS                  // Measurement bias
   Gmat::STRING_TYPE,			// SOLVEMODE             // Specify what mode that bias used for. If SolveMode = 'Estimation', bias is used as a solve-for variable. If SolveMode = 'Model', bias is used as a consider
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
   measurementType   ("Range_KM"),
   measurementTrip   (2),
   strand            (""),
   noiseSigma        (0.01),                   // 0.01 Km
   noiseModel        ("NoiseConstant"), 
   bias              (0.0),                    // 0.0 Km
   solveMode         ("Model")
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel default constructor <%s,%p>\n", GetName().c_str(), this);
#endif

   objectTypes.push_back(Gmat::ERROR_MODEL);
   objectTypeNames.push_back("ErrorModel");

   parameterCount = ErrorModelParamCount;
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
   GmatBase          (em),
   measurementType   (em.measurementType),
   measurementTrip   (em.measurementTrip),
   strand            (em.strand),
   noiseSigma        (em.noiseSigma),
   noiseModel        (em.noiseModel),
   bias              (em.bias),
   solveMode         (em.solveMode)
{
#ifdef DEBUG_CONSTRUCTION
	MessageInterface::ShowMessage("ErrorModel copy constructor from <%s,%p>  to  <%s,%p>\n", em.GetName().c_str(), &em, GetName().c_str(), this);
#endif

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

      measurementType = em.measurementType;
      measurementTrip = em.measurementTrip;
      strand          = em.strand;
      noiseSigma      = em.noiseSigma;
      noiseModel      = em.noiseModel;
      bias            = em.bias;
      solveMode       = em.solveMode;
   }

   return *this;
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

   bool retval = false;

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
std::string ErrorModel::GetStringParameter(const Integer id) const
{
   if (id == TYPE)
      return measurementType;

   if (id == STRAND)
      return strand;

   if (id == NOISE_MODEL)
      return noiseModel;

   if (id == SOLVE_MODE)
      return solveMode;

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
   if (id == TYPE)
   {
      measurementType = value;
      return true;
   }

   if (id == STRAND)
   {
      strand = value;
      return true;
   }

   if (id == NOISE_MODEL)
   {
      if (value != "RandomConstant")
      {
         //throw MeasurementException("Error: " + GetName() + "." + GetParameterText(id) + " cannot accept '" + value +"'\n");
         throw GmatBaseException("Error: " + GetName() + "." + GetParameterText(id) + " cannot accept '" + value +"'\n");
         return false;
      }
      else
      {
         noiseModel = value;    
	      return true;
      }
   }

   if (id == SOLVE_MODE)
   {
      if ((value != "Estimation")&&(value != "Model"))
      {
         //throw MeasurementException("Error: " + GetName() + "." + GetParameterText(id) + " cannot accept '" + value +"'\n");
         throw GmatBaseException("Error: " + GetName() + "." + GetParameterText(id) + " cannot accept '" + value +"'\n");
         return false;
      }
      else
      {
         this->solveMode = value;
         return true;
      }
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


Real ErrorModel::GetRealParameter(const Integer id) const
{
   if (id == NOISE_SIGMA)
      return noiseSigma;

   if (id == BIAS)
      return bias;

   return GmatBase::GetRealParameter(id);
}


Real ErrorModel::SetRealParameter(const Integer id, const Real value)
{
   if (id == NOISE_SIGMA)
   {
      if (value <= 0.0)
         //throw MeasurementException("Error: value of "+ GetName() +".NoiseSigma has to be a positive number.\n");
         throw GmatBaseException("Error: value of "+ GetName() +".NoiseSigma has to be a positive number.\n");

	  noiseSigma = value;
	  return noiseSigma;
   }

   if (id == BIAS)
   {
	  bias = value;
	  return bias;
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
   if (id == TRIP)
      return measurementTrip;

   return GmatBase::GetIntegerParameter(id);
}


Integer ErrorModel::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == TRIP)
   {
      if (value < 0)
         //throw MeasurementException("Error: value of "+ GetName() +".Trip has to be a non-positive integer.\n");
         throw GmatBaseException("Error: value of "+ GetName() +".Trip has to be a non-positive integer.\n");

	   measurementTrip = value;
	   return measurementTrip;
   }

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

