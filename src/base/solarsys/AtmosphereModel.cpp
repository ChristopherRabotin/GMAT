//$Header$
//------------------------------------------------------------------------------
//                              AtmosphereModel
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/02/21
//
/**
 * Base class for the atmosphere models
 */
//------------------------------------------------------------------------------

#include "AtmosphereModel.hpp"
#include "MessageInterface.hpp"


//---------------------------------
// static data
//---------------------------------

const std::string
AtmosphereModel::PARAMETER_TEXT[AtmosphereModelParamCount-GmatBaseParamCount] =
{
   "F107",
   "F107A",
   "MagneticIndex"
};

const Gmat::ParameterType
AtmosphereModel::PARAMETER_TYPE[AtmosphereModelParamCount-GmatBaseParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE
};

//------------------------------------------------------------------------------
//  AtmosphereModel()
//------------------------------------------------------------------------------
/**
 *  Constructor.
 */
//------------------------------------------------------------------------------
AtmosphereModel::AtmosphereModel(const std::string &typeStr) :
    GmatBase            (Gmat::ATMOSPHERE, typeStr),
    fileReader          (NULL),
    mCentralBody        (NULL),
    sunVector           (NULL),
    centralBody         ("Earth"),
    centralBodyLocation (NULL),
    cbRadius            (6378.14),
    newFile             (false),
    fileRead            (false),
    nominalF107         (150.0),
    nominalF107a        (150.0),
    nominalAp           (13.85)     // Corresponds to kp = 3, per Vallado (8-31)
{
    fileName = "";
}

//------------------------------------------------------------------------------
//  ~AtmosphereModel()
//------------------------------------------------------------------------------
/**
 *  Destructor.
 */
//------------------------------------------------------------------------------
AtmosphereModel::~AtmosphereModel()
{
}

//------------------------------------------------------------------------------
//  AtmosphereModel(const AtmosphereModel& am)
//------------------------------------------------------------------------------
/**
 *  Copy constructor.
 */
//------------------------------------------------------------------------------
AtmosphereModel::AtmosphereModel(const AtmosphereModel& am) :
   GmatBase            (am),
   fileReader          (NULL),
   sunVector           (NULL),
   centralBody         (am.centralBody),
   centralBodyLocation (NULL),
   cbRadius            (am.cbRadius),
   newFile             (am.newFile),
   fileRead            (am.fileRead),
   nominalF107         (am.nominalF107),
   nominalF107a        (am.nominalF107a),
   nominalAp           (am.nominalAp)
{
}

//------------------------------------------------------------------------------
// AtmosphereModel& operator=(const AtmosphereModel& am)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param am AtmosphereModel instance used as a template for this copy.
 * 
 * @return A reference to this class, with members set to match the template.
 */
//------------------------------------------------------------------------------
AtmosphereModel& AtmosphereModel::operator=(const AtmosphereModel& am)
{
    if (this == &am)
        return *this;
        
    fileReader = NULL;
    sunVector = NULL;
    centralBodyLocation = NULL;
    
    centralBody = am.centralBody;
    cbRadius = am.cbRadius;
    
    return *this;
}

//------------------------------------------------------------------------------
//  void SetSunVector(Real *sv)
//------------------------------------------------------------------------------
/**
 *  Sets the position vector for the Sun.
 * 
 * @param sv   The Sun vector.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSunVector(Real *sv)
{
    sunVector = sv;
}

//------------------------------------------------------------------------------
//  void SetCentralBodyVector(Real *cv)
//------------------------------------------------------------------------------
/**
 *  Sets the position vector for the body with the atmosphere.
 * 
 * @param cv   The body's position vector.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetCentralBodyVector(Real *cv)
{
    centralBodyLocation = cv;
}

//------------------------------------------------------------------------------
// void SetSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the solar system pointer
 * 
 * @param ss Pointer to the solar system used in the modeling.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSolarSystem(SolarSystem *ss)
{
   solarSystem = ss;
}

//------------------------------------------------------------------------------
// void SetCentralBody(CelestialBody *cb)
//------------------------------------------------------------------------------
/**
 * Sets the central body pointer
 *
 * @param cb Pointer to the central body used in the modeling.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetCentralBody(CelestialBody *cb)
{
   mCentralBody = cb;
}

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
std::string AtmosphereModel::GetParameterText(const Integer id) const
{
   if ((id >= GmatBaseParamCount) && (id < AtmosphereModelParamCount))
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer AtmosphereModel::GetParameterID(const std::string &str) const
{
   for (Integer i = GmatBaseParamCount; i < AtmosphereModelParamCount; ++i)
      if (str == PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   return GmatBase::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType AtmosphereModel::GetParameterType(const Integer id) const
{
   if ((id >= GmatBaseParamCount) && (id < AtmosphereModelParamCount))
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   return GmatBase::GetParameterType(id);
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
std::string AtmosphereModel::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}


//------------------------------------------------------------------------------
//  Real  GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the Real parameter value, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter value.
 *
 * @return  Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::GetRealParameter(const Integer id) const
{
   if (id == NOMINAL_FLUX)
      return nominalF107;
   if (id == NOMINAL_AVERGE_FLUX)
      return nominalF107a;
   if (id == NOMINAL_MAGNETIC_INDEX)
      return nominalAp;

   return GmatBase::GetRealParameter(id);
}

//------------------------------------------------------------------------------
//  Real  SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * This method sets the Real parameter value, given the input parameter ID.
 *
 * This method traps entries for F107, F107A, and the Magnetic index and ensures
 * that these parameters have positive values.
 *
 * @param <id> ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return  Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::SetRealParameter(const Integer id, const Real value)
{
   if (id == NOMINAL_FLUX)
   {
      if (value > 0.0)
         nominalF107 = value;
      return nominalF107;
   }

   if (id == NOMINAL_AVERGE_FLUX)
   {
      if (value > 0.0)
         nominalF107a = value;
      return nominalF107a;
   }

   if (id == NOMINAL_MAGNETIC_INDEX)
   {
      if (value > 0.0)
         nominalAp = value;
      return nominalAp;
   }

   return GmatBase::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
//  void SetSolarFluxFile(std::string file)
//------------------------------------------------------------------------------
/**
 * @param file The solar flux file
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSolarFluxFile(std::string file)
{
   if (strcmp(fileName.c_str(), file.c_str()) == 0)
      SetOpenFileFlag(true);
   else
   {
      fileName = file;
      SetOpenFileFlag(false);
   }   
}

//------------------------------------------------------------------------------
// void SetNewFileFlag(bool flag)
//------------------------------------------------------------------------------
/**
 * Sets the new file flag
 * 
 * @param flag
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetNewFileFlag(bool flag)
{
   newFile = flag;
}

//------------------------------------------------------------------------------
// void SetOpenFileFlag(bool flag)
//------------------------------------------------------------------------------
/**
 * Sets the file opened flag
 * 
 * @param flag
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetOpenFileFlag(bool flag)
{
   fileRead = flag;
}

//------------------------------------------------------------------------------
// void CloseFile()
//------------------------------------------------------------------------------
/**
 * Sets the new file flag
 * 
 * @param flag
 */
//------------------------------------------------------------------------------
void AtmosphereModel::CloseFile()
{
   if (fileReader->CloseSolarFluxFile(solarFluxFile))
       fileRead = false;
    else
       throw AtmosphereException("Error closing Atmosphere Model data file.\n");   
}
