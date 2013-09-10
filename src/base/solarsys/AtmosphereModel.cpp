//$Id$
//------------------------------------------------------------------------------
//                              AtmosphereModel
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "CelestialBody.hpp"        // To retrieve radius, flattening factor
#include "SolarSystemException.hpp"
#include "CoordinateSystem.hpp"
#include "GmatConstants.hpp"
#include "AngleUtil.hpp"            // For lat, long range setting
#include "CoordinateConverter.hpp"


#include <cmath>                    // for exp


//#define CHECK_KP2AP
//#define DEBUG_ANGVEL
//#define DEBUG_COORDINATE_TRANSFORMS
//#define DEBUG_CALCULATE_GEOCENTRICS
//#define DEBUG_CALCULATE_GEODETICS

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
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
//  AtmosphereModel(const std::string &typeStr, const std::string &name = "")
//------------------------------------------------------------------------------
/**
 *  Default Constructor.
 *
 *  @param <typeStr> The type of the derived atmosphere model.
 *  @param <name>    The name of the derived atmosphere model.
 */
//------------------------------------------------------------------------------
AtmosphereModel::AtmosphereModel(const std::string &typeStr, const std::string &name) :
   GmatBase             (Gmat::ATMOSPHERE, typeStr, name),
   fileReader           (NULL),
   solarSystem          (NULL),
   mCentralBody         (NULL),
   solarFluxFile        (NULL),
   fileName             (""),        // Set to a default when working
   sunVector            (NULL),
   centralBody          ("Earth"),
   centralBodyLocation  (NULL),
   cbRadius             (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]),
   cbFlattening         (0.0),       // Default is spherical
   newFile              (false),
   fileRead             (false),
   nominalF107          (150.0),
   nominalF107a         (150.0),
   nominalKp            (3.0),
   kpApConversion       (0),
   mInternalCoordSystem (NULL),
   cbJ2000              (NULL),
   cbFixed              (NULL),
   wUpdateInterval      (0.0),     // Default to always update
   wUpdateEpoch         (0.0),
   geoHeight            (0.0),
   geoLat               (0.0),
   geoLong              (0.0),
   useGeodetic          (false),							// made changes by TUAN NGUYEN
   gha                  (0.0),
   ghaEpoch             (0.0)
{
   objectTypes.push_back(Gmat::ATMOSPHERE);
   objectTypeNames.push_back("AtmosphereModel");

   parameterCount = AtmosphereModelParamCount;
   nominalAp = ConvertKpToAp(nominalKp);

   // Default to nominal Earth angular velocity
   angVel[0]      = 0.0;
   angVel[1]      = 0.0;
   angVel[2]      = 7.29211585530e-5;

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
   GmatBase             (am),
   fileReader           (NULL),
   solarSystem          (am.solarSystem),
   mCentralBody         (am.mCentralBody),
   solarFluxFile        (NULL),
   fileName             (am.fileName),
   sunVector            (NULL),
   centralBody          (am.centralBody),
   centralBodyLocation  (NULL),
   cbRadius             (am.cbRadius),
   cbFlattening         (am.cbFlattening),
   newFile              (false),
   fileRead             (false),
   nominalF107          (am.nominalF107),
   nominalF107a         (am.nominalF107a),
   nominalKp            (am.nominalKp),
   kpApConversion       (am.kpApConversion),
   mInternalCoordSystem (am.mInternalCoordSystem),
   cbJ2000              (am.cbJ2000),
   cbFixed              (am.cbFixed),
   wUpdateInterval      (am.wUpdateInterval),
   wUpdateEpoch         (am.wUpdateEpoch),
   geoHeight            (0.0),
   geoLat               (0.0),
   geoLong              (0.0),
   useGeodetic          (am.useGeodetic),						// made changes by TUAN NGUYEN
   gha                  (0.0),
   ghaEpoch             (0.0)
{
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
   
   fileReader           = NULL;
   solarSystem          = am.solarSystem;
   mCentralBody         = am.mCentralBody;
   solarFluxFile        = NULL;
   fileName             = am.fileName;
   sunVector            = NULL;
   centralBody          = am.centralBody;
   centralBodyLocation  = NULL;
   cbRadius             = am.cbRadius;
   cbFlattening         = am.cbFlattening;
   newFile              = false;
   fileRead             = false;
   nominalF107          = am.nominalF107;
   nominalF107a         = am.nominalF107a;
   nominalKp            = am.nominalKp;
   nominalAp            = ConvertKpToAp(nominalKp);
   kpApConversion       = am.kpApConversion;
   mInternalCoordSystem = am.mInternalCoordSystem;
   cbJ2000              = am.cbJ2000;
   cbFixed              = am.cbFixed;
   wUpdateInterval      = am.wUpdateInterval;
   wUpdateEpoch         = am.wUpdateEpoch;
   geoHeight            = 0.0;
   geoLat               = 0.0;
   geoLong              = 0.0;
   useGeodetic          = am.useGeodetic;							// made changes by TUAN NGUYEN
   gha                  = 0.0;
   ghaEpoch             = 0.0;

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
// void SetUpdateParameters(Real interval, GmatEpoch epoch)
//------------------------------------------------------------------------------
/**
 * Sets the update interval and, if selected, the epoch to apply
 *
 * @param interval The update interval, in days
 * @param epoch    An update epoch
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetUpdateParameters(Real interval, GmatEpoch epoch)
{
   wUpdateInterval = interval;
   if ((epoch >= 0.0) && (cbFixed != NULL))
      UpdateAngularVelocity(epoch);
}


//------------------------------------------------------------------------------
// void SetInternalCoordSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets the internal coordinate system used to construct the angular momentum
 * vector
 *
 * @param cs The coordinate system
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetInternalCoordSystem(CoordinateSystem *cs)
{
   mInternalCoordSystem = cs;
}

//------------------------------------------------------------------------------
// void SetCbJ2000CoordinateSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets the body centered J2000 coordinate system used to calculate geodetics 
 * when the CB is not the Earth.
 *
 * @param cs The coordinate system
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetCbJ2000CoordinateSystem(CoordinateSystem *cs)
{
   cbJ2000 = cs;
}

//------------------------------------------------------------------------------
// void SetFixedCoordinateSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Sets the body fixed coordinate system used to construct the angular momentum
 * vector
 *
 * @param cs The coordinate system
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetFixedCoordinateSystem(CoordinateSystem *cs)
{
   if (!cs->AreAxesOfType("BodyFixedAxes"))
      throw SolarSystemException(
            "AtmosphereModel: coordinate system is not of type BodyFixed.\n");
   cbFixed = cs;
}

//------------------------------------------------------------------------------
// Real* GetAngularVelocity(const GmatEpoch when)
//------------------------------------------------------------------------------
/**
 * Retrieves the angular momentum vector, optionally at a specified epoch
 *
 * @note This method return the pointer to a data member.  If that pointer is
 * stored, subsequent updates will change the data pointed to so that the user
 * need not refresh this data pointer.
 *
 * @param when The epoch for the desired vector.  If not specified or less than
 * 0.0, the last calculated vector is returned.
 *
 * @return A pointer to the angular momentum vector
 */
//------------------------------------------------------------------------------
Real* AtmosphereModel::GetAngularVelocity(const GmatEpoch when)
{
   if (when >= 0.0)
      UpdateAngularVelocity(when);
   return angVel;
}


//------------------------------------------------------------------------------
// void BuildAngularVelocity(const GmatEpoch when)
//------------------------------------------------------------------------------
/**
 * Constructs the angular velocity vector at the specified epoch.
 *
 * @param when The epoch
 */
//------------------------------------------------------------------------------
void AtmosphereModel::BuildAngularVelocity(const GmatEpoch when)
{
    Real in[3];
    Rmatrix33 rotMat = cbFixed->GetLastRotationMatrix();
    Rmatrix33 rotDotMat = cbFixed->GetLastRotationDotMatrix();

    // Build angVel from R' * Rdot
    in[0] = rotMat(0,2)*rotDotMat(0,1) + rotMat(1,2)*rotDotMat(1,1) +
          rotMat(2,2)*rotDotMat(2,1);
    in[1] = rotMat(0,0)*rotDotMat(0,2) + rotMat(1,0)*rotDotMat(1,2) +
          rotMat(2,0)*rotDotMat(2,2);
    in[2] = rotMat(0,1)*rotDotMat(0,0) + rotMat(1,1)*rotDotMat(1,0) +
          rotMat(2,1)*rotDotMat(2,0);

    // Rotate into the J2000 frame
    angVel[0] = rotMat(0,0)*in[0] + rotMat(0,1)*in[1] + rotMat(0,2)*in[2];
    angVel[1] = rotMat(1,0)*in[0] + rotMat(1,1)*in[1] + rotMat(1,2)*in[2];
    angVel[2] = rotMat(2,0)*in[0] + rotMat(2,1)*in[1] + rotMat(2,2)*in[2];

    wUpdateEpoch = when;

    #ifdef DEBUG_ANGVEL
       MessageInterface::ShowMessage("Rotation Matrix:\n");
       for (Integer m = 0; m < 3; ++m)
       {
          MessageInterface::ShowMessage("   [");
          for (Integer n = 0; n < 3; ++n)
             MessageInterface::ShowMessage(" %.14lf ",rotMat(m,n));
          if (m < 2)
             MessageInterface::ShowMessage(";\n");
          else
             MessageInterface::ShowMessage("]\n");
       }

       MessageInterface::ShowMessage("Rotation Dot Matrix:\n");
       for (Integer m = 0; m < 3; ++m)
       {
          MessageInterface::ShowMessage("   [");
          for (Integer n = 0; n < 3; ++n)
             MessageInterface::ShowMessage(" %.14lf ",rotDotMat(m,n));
          if (m < 2)
             MessageInterface::ShowMessage(";\n");
          else
             MessageInterface::ShowMessage("]\n");
       }

       MessageInterface::ShowMessage("AtmosphereModel::"
             "UpdateAngularVelocity(%.12lf) -> [%.12le %.12le %.12le]\n",
             when, angVel[0], angVel[1], angVel[2]);
    #endif
}


//------------------------------------------------------------------------------
// void UpdateAngularVelocity(const GmatEpoch when)
//------------------------------------------------------------------------------
/**
 * Updates the angular momentum vector based on the input epoch.
 *
 * Updates are only performed if the update epoch is outside of the update
 * interval period.
 *
 * @param when The epoch (in a.1 ModJulian format) of the desired update.
 */
//------------------------------------------------------------------------------
void AtmosphereModel::UpdateAngularVelocity(const GmatEpoch when)
{
   if (wUpdateInterval >= 0.0)
   {
      if (fabs(when - wUpdateEpoch) > wUpdateInterval)
      {
         if (cbFixed == NULL)
            throw AtmosphereException("The body-fixed coordinate system is "
                  "not set");
         Real in[3], out[3];
         cbFixed->ToBaseSystem(when, in, out, true, true);  // @todo - do we need ToMJ2000Eq here?
         BuildAngularVelocity(when);
      }
   }
}


//------------------------------------------------------------------------------
// void SetKpApConversionMethod(Integer method)
//------------------------------------------------------------------------------
/**
 * Selects the Kp -> Ap conversion method
 *
 * @param <method>  Method to use (see next method prolog)
 */
//------------------------------------------------------------------------------
void AtmosphereModel::SetKpApConversionMethod(Integer method)
{
   kpApConversion = method;
   nominalAp = ConvertKpToAp(nominalKp);
}


//------------------------------------------------------------------------------
// Real ConvertKpToAp(const Real kp)
//------------------------------------------------------------------------------
/**
 * Routine to convert Kp values into Ap values
 *
 * This method provides several different Kp->Ap, conversion methods.  The
 * method used is selected by the value of kpApConversion.  The implemented
 * approaches are as follows:
 *
 *   kpApConversion = 0: Table lookup using table 8-3 from Vallado,  3rd edition
 *
 *   kpApConversion = 1: Exponential approximation from Vallado 2nd Edition,
 *   (eq 8-31).  Ap is calculated using
 *
 *       \f[a_p = \exp\left(\frac{k_p + 1.6} {1.75}\right)\f]
 *
 *   kpApConversion = 2: A secant method that solves the transcendental equation
 *
 *       \f[28 K_p + 0.03 e^{K_p} = A_p + 100 (1 - e^{-0.08 A_p})\f]
 *
 *   using an iterative secant method.
 *
 * The default method is the table lookup
 *
 * @param <kp> The geomagnetic index, Kp.
 *
 * @return the corresponding geomagnetic amplitude, Ap.
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::ConvertKpToAp(const Real kp)
{
   Real ap = 15.0;

   switch (kpApConversion)
   {
      case 0:  // Table lookup
         {
            Integer index = (Integer)((kp + .01) * 3);
            switch (index)
            {
               case 0:
                  ap = 0.0;
                  break;
               case 1:
                  ap = 2.0;
                  break;
               case 2:
                  ap = 3.0;
                  break;
               case 3:
                  ap = 4.0;
                  break;
               case 4:
                  ap = 5.0;
                  break;
               case 5:
                  ap = 6.0;
                  break;
               case 6:
                  ap = 7.0;
                  break;
               case 7:
                  ap = 9.0;
                  break;
               case 8:
                  ap = 12.0;
                  break;
               /// case 9 is the default, below
               case 10:
                  ap = 18.0;
                  break;
               case 11:
                  ap = 22.0;
                  break;
               case 12:
                  ap = 27.0;
                  break;
               case 13:
                  ap = 32.0;
                  break;
               case 14:
                  ap = 39.0;
                  break;
               case 15:
                  ap = 48.0;
                  break;
               case 16:
                  ap = 56.0;
                  break;
               case 17:
                  ap = 67.0;
                  break;
               case 18:
                  ap = 80.0;
                  break;
               case 19:
                  ap = 94.0;
                  break;
               case 20:
                  ap = 111.0;
                  break;
               case 21:
                  ap = 132.0;
                  break;
               case 22:
                  ap = 154.0;
                  break;
               case 23:
                  ap = 179.0;
                  break;
               case 24:
                  ap = 207.0;
                  break;
               case 25:
                  ap = 236.0;
                  break;
               case 26:
                  ap = 300.0;
                  break;
               case 27:
                  ap = 400.0;
                  break;
               case 9:
               default:
                  ap = 15.0;
                  break;
            }
         }
         break;

      case 1:  // Vallado, 2nd edition
         ap = exp((kp + 1.6) / 1.75);
         break;

      case 2:
      default:
      {
         Real r = 28.0 * kp + 0.03 * exp(kp) - 100.0;

         Real x[3], y[2];
         x[0] = 0.0;
         x[1] = 500.0;

         Real epsilon = 1.0e-6;
         Integer maxIterations = 15, i = 0;

         do
         {
            y[0] = 100.0 * exp(-0.08 * x[0]) + r - x[0];
            y[1] = 100.0 * exp(-0.08 * x[1]) + r - x[1];

            x[2] = x[1] - y[1] * (x[1] - x[0]) / (y[1] - y[0]);
            x[0] = x[1];
            x[1] = x[2];

            if (i++ > maxIterations)
               throw AtmosphereException("ConvertKpToAp failed; too "
                     "many iterations");
         } while (fabs(y[1]) > epsilon);

         ap = x[2];
      }
   }
   return ap;
}


//-----------------------------------------------------------------------------
// bool AtmosphereModel::HasWindModel()
//-----------------------------------------------------------------------------
/**
 * Checks to see if a local wind model exists for the atmosphere.
 *
 * @return true if the model supports wind calculation, false if not
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::HasWindModel()
{
   return false;
}

//-----------------------------------------------------------------------------
// bool Wind(Real *position, Real* wind, Real ep, Integer count = 1)
//-----------------------------------------------------------------------------
/**
 * Retrieves the local wind in MJ2000Eq coordinates.
 *
 * @param position The location of the spacecraft
 * @param wind The local wind, as set by the method
 * @param ep The epoch of the calculations
 * @param count The number of spacecraft contained in pos
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::Wind(Real *position, Real* wind, Real ep,
						   Integer count)
{
   return false;
}

//-----------------------------------------------------------------------------
// bool HasTemperatureModel()
//-----------------------------------------------------------------------------
/**
 * Checks to see if a local temperature exists for the atmosphere.
 *
 * @return true if the model supports temperature calculation, false if not
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::HasTemperatureModel()
{
   return false;
}

//-----------------------------------------------------------------------------
// bool Temperature(Real *position, Real *temperature, Real epoch, 
//                  Integer count)
//-----------------------------------------------------------------------------
/**
 * Retrieves the temperature.
 *
 * @param position The location of the spacecraft
 * @param temperature The temperature, as set by the method
 * @param ep The epoch of the calculations
 * @param count The number of spacecraft contained in position
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::Temperature(Real *position, Real *temperature, 
                                  Real epoch, Integer count)
{
   return false;
}

//-----------------------------------------------------------------------------
// bool HasPressureModel()
//-----------------------------------------------------------------------------
/**
 * Checks to see if a local pressure exists for the atmosphere.
 *
 * @return true if the model supports pressure calculation, false if not
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::HasPressureModel()
{
   return false;
}

//-----------------------------------------------------------------------------
// bool Pressure(Real *position, Real *pressure, Real epoch, Integer count)
//-----------------------------------------------------------------------------
/**
 * Retrieves the pressure.
 *
 * @param position The location of the spacecraft
 * @param pressure The pressure, as set by the method
 * @param ep The epoch of the calculations
 * @param count The number of spacecraft contained in position
 *
 * @return true on success, false on failure
 */
//-----------------------------------------------------------------------------
bool AtmosphereModel::Pressure(Real *position, Real *pressure, 
                               Real epoch, Integer count)
{
   return false;
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
   if (mCentralBody != cb)
   {
      mCentralBody = cb;
      cbRadius     = mCentralBody->GetEquatorialRadius();
      cbFlattening = mCentralBody->GetFlattening();

      #ifdef DEBUG_CB_PROPERTIES
         MessageInterface::ShowMessage("Body set to %s; radius = %.12lf, "
               "flattening = %.12lf\n", mCentralBody->GetName().c_str(),
               cbRadius, cbFlattening);
      #endif
   }
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool AtmosphereModel::IsParameterReadOnly(const Integer id) const
{
   // Since these parameters are handled in the DragForce, make them
   // read only 
   if ((id == NOMINAL_FLUX) || (id == NOMINAL_AVERAGE_FLUX) ||
       (id == NOMINAL_MAGNETIC_INDEX))
      return true;
   
   return GmatBase::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool AtmosphereModel::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
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
// Real CalculateGeodetics(Real *position, GmatEpoch when, bool includeLatLong)
//------------------------------------------------------------------------------
/**
 * Calculates the geodetic height, latitude and longitude for the input state
 *
 * The method used here is the same as used in the parameter code.  We may want
 * to refactor so that both call a common, low level source.
 *
 * @param position       The cb-centered MJ2000 Cartesian state for the
 *                       calculations
 * @param when           Epoch for the lat/long calculations
 * @param includeLatLong true to calculate latitude and longitude, false to skip
 *
 * @return The geodetic height.  The latitude and longitude are filled in to
 *         class variables
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::CalculateGeodetics(Real *position, GmatEpoch when,
                                         bool includeLatLong)
{
   Rvector6 instate(position), state;

   CoordinateSystem *j2000ToUse = (cbJ2000 == NULL ? mInternalCoordSystem : cbJ2000);

#ifdef DEBUG_CALCULATE_GEODETICS
   if (cbJ2000 == NULL)
	   MessageInterface::ShowMessage("cbJ2000 == NULL, mInternalCoordSystem <%p,%s>\n", mInternalCoordSystem, mInternalCoordSystem->GetName().c_str());
   else
	   MessageInterface::ShowMessage("cbJ2000 <%p,%s>\n", cbJ2000, cbJ2000->GetName().c_str());
   MessageInterface::ShowMessage("j2000ToUse cs<%p,%s>: origin = %s    axes = %s\n", j2000ToUse, j2000ToUse->GetName().c_str(), j2000ToUse->GetOrigin()->GetName().c_str(), j2000ToUse->GetStringParameter("Axes").c_str());
#endif

   if (when == -1.0)
      when = wUpdateEpoch;

   #ifdef DEBUG_COORDINATE_TRANSFORMS
      MessageInterface::ShowMessage("Geodetic calculations at epoch %.12lf\n",
            when);
      MessageInterface::ShowMessage("Internal CS:\n%s\nFixed:\n%s\n",
            mInternalCoordSystem->GetGeneratingString(
                  Gmat::NO_COMMENTS).c_str(),
            cbFixed->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      if (cbJ2000 != NULL)
         MessageInterface::ShowMessage("cbJ2000 CS:\n%s\n",
            cbJ2000->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      MessageInterface::ShowMessage("Position: %lf, %lf, %lf\n", position[0], position[1], position[2]);
   #endif

   CoordinateConverter mCoordConverter;
   mCoordConverter.Convert(A1Mjd(when), instate, j2000ToUse,
                           state, cbFixed);

   // Build angular momentum
   if (wUpdateEpoch != when)
      BuildAngularVelocity(when);

   // Get the body fixed geodetic height
   // Vallado algorithm 12 (Vallado, 2nd ed, p. 177)
   Real rxy = sqrt(state[0]*state[0] + state[1]*state[1]);
   geoLat = atan2(state[2], rxy);

   Real delta = 1.0;
   Real geodeticTolerance = 1.0e-7;    // Better than 0.0001 degrees
   Real ecc2 = cbFlattening * (2.0 - cbFlattening);

   Real cFactor, oldlat, sinlat;
   while (delta > geodeticTolerance)
   {
      oldlat = geoLat;
      sinlat = sin(oldlat);
      cFactor = cbRadius / sqrt(1.0 - ecc2 * sinlat * sinlat);
      geoLat = atan2(state[2] + cFactor*ecc2*sinlat, rxy);
      delta = fabs(geoLat - oldlat);
   }

   sinlat = sin(geoLat);
   cFactor = cbRadius / sqrt(1.0 - ecc2 * sinlat * sinlat);
   geoHeight = rxy / cos(geoLat) - cFactor;

   // Only do lat/long (in degrees) if needed
   if (includeLatLong)
   {
      geoLat = geoLat * GmatMathConstants::DEG_PER_RAD;
      geoLat = AngleUtil::PutAngleInDegRange(geoLat, -90.0, 90.0);
      geoLong = atan2(state[1], state[0]) * GmatMathConstants::DEG_PER_RAD;
      geoLong = AngleUtil::PutAngleInDegRange(geoLong, -180.0, 180.0);
   }

   #ifdef DEBUG_COORDINATE_TRANSFORMS
      MessageInterface::ShowMessage("Geodetics:\n   Height = %.6lf\n   "
            "Latitude = %.6lf\n   Longitude = %.6lf\n", geoHeight, geoLat,
            (includeLatLong ? geoLong : -999999.999999));
   #endif

   return geoHeight;

}


//------------------------------------------------------------------------------
// Real CalculateGeocentrics(Real *position, GmatEpoch when, bool includeLatLong)
//------------------------------------------------------------------------------
/**
 * Calculates the geocentric height, latitude and longitude for the input state
 *
 * The method used here is the same as used in the parameter code.  We may want
 * to refactor so that both call a common, low level source.
 *
 * @param position       The cb-centered MJ2000 Cartesian state for the
 *                       calculations
 * @param when           Epoch for the lat/long calculations
 * @param includeLatLong true to calculate latitude and longitude, false to skip
 *
 * @return The geocentrics height.  The latitude and longitude are filled in to
 *         class variables
 */
//------------------------------------------------------------------------------
Real AtmosphereModel::CalculateGeocentrics(Real *position, GmatEpoch when,
                                           bool includeLatLong)
{
   Rvector6 instate(position), state;

   CoordinateSystem *j2000ToUse = (cbJ2000 == NULL ? mInternalCoordSystem : cbJ2000);

#ifdef DEBUG_CALCULATE_GEOCENTRICS
   if (cbJ2000 == NULL)
	   MessageInterface::ShowMessage("cbJ2000 == NULL, mInternalCoordSystem <%p,%s>\n", mInternalCoordSystem, mInternalCoordSystem->GetName().c_str());
   else
	   MessageInterface::ShowMessage("cbJ2000 <%p,%s>\n", cbJ2000, cbJ2000->GetName().c_str());
   MessageInterface::ShowMessage("j2000ToUse cs<%p,%s>: origin = %s    axes = %s\n", j2000ToUse, j2000ToUse->GetName().c_str(), j2000ToUse->GetOrigin()->GetName().c_str(), j2000ToUse->GetStringParameter("Axes").c_str());
#endif

   if (when == -1.0)
      when = wUpdateEpoch;

   #ifdef DEBUG_COORDINATE_TRANSFORMS
      MessageInterface::ShowMessage("Geocentric calculations at epoch %.12lf\n",
            when);
      MessageInterface::ShowMessage("Internal CS:\n%s\nFixed:\n%s\n",
            mInternalCoordSystem->GetGeneratingString(
                  Gmat::NO_COMMENTS).c_str(),
            cbFixed->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      if (cbJ2000 != NULL)
         MessageInterface::ShowMessage("cbJ2000 CS:\n%s\n",
            cbJ2000->GetGeneratingString(Gmat::NO_COMMENTS).c_str());
      MessageInterface::ShowMessage("Position: %lf, %lf, %lf\n", position[0], position[1], position[2]);
   #endif

   CoordinateConverter mCoordConverter;
   mCoordConverter.Convert(A1Mjd(when), instate, j2000ToUse,
                           state, cbFixed);

   // Build angular momentum
   if (wUpdateEpoch != when)
      BuildAngularVelocity(when);

   // Get the body fixed geocentric height
   //
   Real rxy = sqrt(state[0]*state[0] + state[1]*state[1]);
   geoLat = atan2(state[2], rxy);
   geoHeight = rxy / cos(geoLat) - cbRadius;

   // Only do lat/long (in degrees) if needed
   if (includeLatLong)
   {
      geoLat = geoLat * GmatMathConstants::DEG_PER_RAD;
      geoLat = AngleUtil::PutAngleInDegRange(geoLat, -90.0, 90.0);
      geoLong = atan2(state[1], state[0]) * GmatMathConstants::DEG_PER_RAD;
      geoLong = AngleUtil::PutAngleInDegRange(geoLong, -180.0, 180.0);
   }

   #ifdef DEBUG_COORDINATE_TRANSFORMS
      MessageInterface::ShowMessage("Geocentrics:\n   Height = %.6lf\n   "
            "Latitude = %.6lf\n   Longitude = %.6lf\n", geoHeight, geoLat,
            (includeLatLong ? geoLong : -999999.999999));
   #endif

   return geoHeight;
}
