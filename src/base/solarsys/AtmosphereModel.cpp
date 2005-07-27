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
//#include "RealUtilities.hpp"        // Inadequate for my needs here, so...
#include <cmath>                    // for exp


// #define CHECK_KP2AP

//---------------------------------
// static data
//---------------------------------

const std::string
AtmosphereModel::PARAMETER_TEXT[AtmosphereModelParamCount-GmatBaseParamCount] =
{
   "F107",
   "F107A",
   "MagneticIndex"                  // In GMAT, the "published" value is K_p.
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
 *
 *  @param <typeStr> The type of the derived atmosphere model.
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
   nominalKp           (3.0)
{
   objectTypes.push_back(Gmat::ATMOSPHERE);
   objectTypeNames.push_back("AtmosphereModel");

   fileName = "";
   parameterCount = AtmosphereModelParamCount;
   nominalAp = ConvertKpToAp(nominalKp);

   #ifdef CHECK_KP2AP
      MessageInterface::ShowMessage("K_p to A_p conversions:\n");
      for (Integer i = 0; i < 28; ++i)
         MessageInterface::ShowMessage("   %12.9lf -> %12.9lf\n", i/3.0,
            ConvertKpToAp(i/3.0));
   #endif
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
 *
 *  @param <am> The atmosphere model that is copied.
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
   nominalKp           (am.nominalKp)
{
   fileName = am.fileName;
   parameterCount = AtmosphereModelParamCount;
   nominalAp = ConvertKpToAp(nominalKp);
}

//------------------------------------------------------------------------------
// AtmosphereModel& operator=(const AtmosphereModel& am)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 * 
 * @param <am> AtmosphereModel instance used as a template for this copy.
 * 
 * @return A reference to this class, with members set to match the template.
 */
//------------------------------------------------------------------------------
AtmosphereModel& AtmosphereModel::operator=(const AtmosphereModel& am)
{
   if (this == &am)
      return *this;
        
   GmatBase::operator=(am);
   
   fileReader          = NULL;
   sunVector           = NULL;
   centralBodyLocation = NULL;
   fileName            = am.fileName;
   centralBody         = am.centralBody;
   cbRadius            = am.cbRadius;
   newFile             = am.newFile;
   fileRead            = am.fileRead;
   nominalF107         = am.nominalF107;
   nominalF107a        = am.nominalF107a;
   nominalKp           = am.nominalKp;
   nominalAp = ConvertKpToAp(nominalKp);

   return *this;
}

//------------------------------------------------------------------------------
//  void SetSunVector(Real *sv)
//------------------------------------------------------------------------------
/**
 *  Sets the position vector for the Sun.
 * 
 * @param <sv> The Sun vector.
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
 *  @param <cv> The body's position vector.
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
 * @param <ss> Pointer to the solar system used in the modeling.
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
 * @param <cb> Pointer to the central body used in the modeling.
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
 * @return Real value of the requested parameter.
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::GetRealParameter(const Integer id) const
{
   if (id == NOMINAL_FLUX)
      return nominalF107;
   if (id == NOMINAL_AVERAGE_FLUX)
      return nominalF107a;
   if (id == NOMINAL_MAGNETIC_INDEX)
      return nominalKp;

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
 * @param <id>    ID for the parameter whose value to change.
 * @param <value> value for the parameter.
 *
 * @return Real value of the requested parameter.
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

   if (id == NOMINAL_AVERAGE_FLUX)
   {
      if (value > 0.0)
         nominalF107a = value;
      return nominalF107a;
   }

   if (id == NOMINAL_MAGNETIC_INDEX)
   {
      if (value > 0.0)
      {
         nominalKp = value;
         nominalAp = ConvertKpToAp(nominalKp);
      }
      return nominalKp;
   }
   
   /// @todo Throw exceptions when the values are unphysical here.

   return GmatBase::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
//  void SetSolarFluxFile(const std::string &file)
//------------------------------------------------------------------------------
/**
 * @param <file> The solar flux file
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetSolarFluxFile(const std::string &file)
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
 * @param <flag> The value for the flag.
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
 * @param <flag> The value for the flag.
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
 * Closes the solar flux file.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::CloseFile()
{
   if (fileReader->CloseSolarFluxFile(solarFluxFile))
      fileRead = false;
   else
      throw AtmosphereException("Error closing Atmosphere Model data file.\n");
}


//------------------------------------------------------------------------------
// Real ConvertKpToAp(const Real kp)
//------------------------------------------------------------------------------
/**
 * Routine to convert Kp values into Ap values, using a secant method.
 * 
 * This method solves the transcendental equation
 * 
 *    \f[28 K_p + 0.03 e^{K_p} = A_p + 100 (1 - e^{-0.08 A_p})\f]
 * 
 * using an iterative secant method.
 * 
 * @param <kp> The geomagnetic index, Kp.
 * 
 * @return the corresponding geomagnetic amplitude, Ap. 
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::ConvertKpToAp(const Real kp)
{
   Real r = 28.0 * kp + 0.03 * exp(kp) - 100.0;
   
   Real x[3], y[2];
   x[0] = 0.0;
   x[1] = 500.0;
   
   Real epsilon = 1.0e-6;
   Integer maxIterations = 15, i = 0;
   
   do {
      y[0] = 100.0 * exp(-0.08 * x[0]) + r - x[0];
      y[1] = 100.0 * exp(-0.08 * x[1]) + r - x[1];
      
      x[2] = x[1] - y[1] * (x[1] - x[0]) / (y[1] - y[0]);
      x[0] = x[1];
      x[1] = x[2];
      
      if (i++ > maxIterations)
         throw AtmosphereException("ConvertKpToAp failed; too many iterations");
   } while (fabs(y[1]) > epsilon);
   
   return x[2];
}
