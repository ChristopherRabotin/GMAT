//$Id:  $
//------------------------------------------------------------------------------
//                              RelativisticCorrection
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: W. Shoan
// Created: 2011.07.06
///
/**
 * This is the RelativisticCorrection class.
 *
 * @note original prolog information included at end of file prolog.
 */
#include "gmatdefs.hpp"
#include "PhysicalModel.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "RealUtilities.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateConverter.hpp"
#include "RelativisticCorrection.hpp"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RELATIVISTIC_CORRECTION
//#define DEBUG_DERIVATIVES


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
const std::string RelativisticCorrection::PARAMETER_TEXT[RelativisticCorrectionParamCount - PhysicalModelParamCount] =
{
      "Radius",
      "Mu",
};
const Gmat::ParameterType RelativisticCorrection::PARAMETER_TYPE[RelativisticCorrectionParamCount - PhysicalModelParamCount] =
{
      Gmat::REAL_TYPE,
      Gmat::REAL_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// RelativisticCorrection(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Constructor for RelativisticCorrection model
 */
//------------------------------------------------------------------------------
RelativisticCorrection::RelativisticCorrection(const std::string &name, const std::string &forBodyName) :
  PhysicalModel          (Gmat::PHYSICAL_MODEL, "RelativisticCorrection", name),
  bodyRadius             (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]),
  bodyMu                 (GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH]),
  bodySpinRate           (0.0),
  bodyIsTheEarth         (true),
  satCount               (0),
  bodyInertial           (NULL),
  bodyFixed              (NULL),
  eop                    (NULL),
  sunMu                  (GmatSolarSystemDefaults::STAR_MU)
{
   objectTypeNames.push_back("RelativisticCorrection");
   parameterCount = RelativisticCorrectionParamCount;
   dimension      = 6 * satCount;
   body           = NULL;
   bodyName       = forBodyName;

   // create default body
   bodyName       = SolarSystem::EARTH_NAME;

   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   derivativeIds.push_back(Gmat::ORBIT_STATE_TRANSITION_MATRIX);
   derivativeIds.push_back(Gmat::ORBIT_A_MATRIX);
}

//------------------------------------------------------------------------------
// ~RelativisticCorrection()
//------------------------------------------------------------------------------
/**
 * Destructor for RelativisticCorrection model
 */
//------------------------------------------------------------------------------
RelativisticCorrection::~RelativisticCorrection()
{
   if (bodyInertial) delete bodyInertial;
   if (bodyFixed)    delete bodyFixed;
}

//------------------------------------------------------------------------------
// RelativisticCorrection(const RelativisticCorrection &rc)
//------------------------------------------------------------------------------
/**
 * Copy constructor for RelativisticCorrection model
 */
//------------------------------------------------------------------------------
RelativisticCorrection::RelativisticCorrection(const RelativisticCorrection &rc) :
   PhysicalModel(rc),
   theSun         (NULL),
   bodyRadius     (rc.bodyRadius),
   bodyMu         (rc.bodyMu),
   bodySpinRate   (rc.bodySpinRate),
   sunToCBVector  (rc.sunToCBVector),
   bodyIsTheEarth (rc.bodyIsTheEarth),
   sunrv          (rc.sunrv),
   cbrv           (rc.cbrv),
   now            (rc.now),
   satCount       (rc.satCount),
   bodyInertial   (NULL),
   bodyFixed      (NULL),
   eop            (rc.eop),
   sunMu          (rc.sunMu)
{
   objectTypeNames.push_back("RelativisticCorrection");
   parameterCount = RelativisticCorrectionParamCount;
   dimension      = rc.dimension;
   epoch          = rc.epoch;
   initialized    = false;

   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   derivativeIds.push_back(Gmat::ORBIT_STATE_TRANSITION_MATRIX);
   derivativeIds.push_back(Gmat::ORBIT_A_MATRIX);
}

//------------------------------------------------------------------------------
//  operator=(const RelativisticCorrection &rc)
//------------------------------------------------------------------------------
/**
 * operator= for RelativisticCorrection model
 */
//------------------------------------------------------------------------------
RelativisticCorrection& RelativisticCorrection::operator=(const RelativisticCorrection &rc)
{
   if (&rc == this)
      return *this;

   PhysicalModel::operator=(rc);

   theSun         = NULL;
   bodyRadius     = rc.bodyRadius;
   bodyMu         = rc.bodyMu;
   bodySpinRate   = rc.bodySpinRate;
   sunToCBVector  = rc.sunToCBVector;
   bodyIsTheEarth = rc.bodyIsTheEarth;
   sunrv          = rc.sunrv;
   cbrv           = rc.cbrv;
   now            = rc.now;
   satCount       = rc.satCount;
   bodyInertial   = (CoordinateSystem*) rc.bodyInertial->Clone();
   bodyFixed      = (CoordinateSystem*) rc.bodyInertial->Clone();
   eop            = rc.eop;
   sunMu          = rc.sunMu;

   return *this;
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Method to initialize the RelativisticCorrection object
 */
//------------------------------------------------------------------------------
bool RelativisticCorrection::Initialize()
{
   PhysicalModel::Initialize();

   if (!eop) throw ODEModelException(
             "EOP file is undefined for RelativisticCorrection " + instanceName);

   if (solarSystem != NULL)
   {
      body    = solarSystem->GetBody(bodyName);
      theSun  = solarSystem->GetBody(SolarSystem::SUN_NAME);

      if (body != NULL)
      {
         bodyMu       = body->GetGravitationalConstant();
         sunMu        = theSun->GetGravitationalConstant();

         #ifdef DEBUG_RELATIVISTIC_CORRECTION
             MessageInterface::ShowMessage
                ("RelativisticCorrection::Initialize() setting mu=%f for type=%s, "
                 "name=%s\n", bodyMu, body->GetTypeName().c_str(),
                 body->GetName().c_str());
         #endif

      }
      else
      {
         initialized = false;
         throw ODEModelException("RelativisticCorrection::Initialize() body \"" +
            bodyName + "\" is not in the solar system\n");
      }
   }
   else
   {
      initialized = false;
      throw ODEModelException(
         "RelativisticCorrection::Initialize() solarSystem is NULL\n");
   }

   // Create the coordinate systems needed (Earth only)
   if (body->GetName() == GmatSolarSystemDefaults::EARTH_NAME)
   {
      bodyInertial = CoordinateSystem::CreateLocalCoordinateSystem("bodyInertial", "MJ2000Eq", body,
                                       NULL, NULL, body->GetJ2000Body(), solarSystem);
      bodyFixed    = CoordinateSystem::CreateLocalCoordinateSystem("bodyFixed", "BodyFixed", body,
                                       NULL, NULL, body->GetJ2000Body(), solarSystem);
   }

   return true;
}


//------------------------------------------------------------------------------
//  bool GetDerivatives(Real *state, Real dt = 0.0, Integer order = 1,
//                      const Integer id = -1)
//------------------------------------------------------------------------------
/**
 * Method to return the derivative array
 */
//------------------------------------------------------------------------------
bool RelativisticCorrection::GetDerivatives(Real *state, Real dt, Integer order, const Integer id)
{
   if ((order > 2) || (order < 1))
      return false;

   if ((cartesianCount < 1)  && (stmCount < 1) && (aMatrixCount < 1))
      throw ODEModelException(
         "RelativisticCorrection requires at least one spacecraft.");

   now         = epoch + dt/GmatTimeConstants::SECS_PER_DAY;

   if (fillCartesian)
   {
      Rvector6  stateWRTSun, dummy, dummyResult;
      Real      r, v, s1, posMag, rvDotvvX4, s2_1, lt1, lt2;
      Real      muSunc2r3;
      Real      threeOver2 = 3.0 / 2.0;
      Rmatrix33 RB_IF;
      Real      ar[3], geodesic[3], s2[3], s3[3], schwarzschild[3], J1[3], J[3], rvCrossvv[3], vvCrossJ[3];
      Real      rv[3], vv[3], omega[3], vel[3], pos[3], lenseThirring[3], posWRTSun[3], velWRTSun[3];
      Real      c      = GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM * GmatMathConstants::M_TO_KM;

      J[0]             = J[1]             = J[2]             = 0.0;
      omega[0]         = omega[1]         = omega[2]         = 0.0;
      geodesic[0]      = geodesic[1]      = geodesic[2]      = 0.0;
      lenseThirring[0] = lenseThirring[1] = lenseThirring[2] = 0.0;
      if (body->GetName() == GmatSolarSystemDefaults::EARTH_NAME)
      {
         // compute quantities needed for geodesic and lense-thirring terms, for Earth ONLY
         stateWRTSun  = - theSun->GetMJ2000State(now);  // since state of Earth wrt Earth = (0.0 0.0 0.0 0.0 0.0 0.0)
         posWRTSun[0] = stateWRTSun[0];
         posWRTSun[1] = stateWRTSun[1];
         posWRTSun[2] = stateWRTSun[2];
         velWRTSun[0] = stateWRTSun[3];
         velWRTSun[1] = stateWRTSun[4];
         velWRTSun[2] = stateWRTSun[5];
         posMag       = GmatMathUtil::Sqrt(posWRTSun[0] * posWRTSun[0] + posWRTSun[1] * posWRTSun[1] + posWRTSun[2] * posWRTSun[2]);
         muSunc2r3    = sunMu / (c * c * posMag * posMag * posMag);
         vel[0]       = threeOver2 * velWRTSun[0];
         vel[1]       = threeOver2 * velWRTSun[1];
         vel[2]       = threeOver2 * velWRTSun[2];
         pos[0]       = -muSunc2r3 * posWRTSun[0];
         pos[1]       = -muSunc2r3 * posWRTSun[1];
         pos[2]       = -muSunc2r3 * posWRTSun[2];
         omega[0]     = vel[1]*pos[2] - vel[2]*pos[1];
         omega[1]     = vel[2]*pos[0] - vel[0]*pos[2];
         omega[2]     = vel[0]*pos[1] - vel[1]*pos[0];
         cc.Convert(now, dummy, bodyFixed, dummyResult, bodyInertial);
         bodyRadius   = body->GetEquatorialRadius();
         // Get xp and yp from the EOP file
//         Real xp, yp;
//         Real lod = 0.0;   // Set this to 0.0 for now
//         Real utcmjd  = TimeConverterUtil::Convert(now.Get(), TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD,
//                       GmatTimeConstants::JD_JAN_5_1941);
//         eop->GetPolarMotionAndLod(utcmjd, xp, yp, lod);
         bodySpinRate = 7.29211514670698e-5; //  * (1.0 - (lod / GmatTimeConstants::SECS_PER_DAY));
         RB_IF        = cc.GetLastRotationMatrix();
         J1[0] = 0.0;
         J1[1] = 0.0;
         J1[2] = (2.0 / 5.0) * bodyRadius * bodyRadius * bodySpinRate;
         J[0] = RB_IF(0,0)*J1[0] + RB_IF(0,1)*J1[1] + RB_IF(0,2)*J1[2];
         J[1] = RB_IF(1,0)*J1[0] + RB_IF(1,1)*J1[1] + RB_IF(1,2)*J1[2];
         J[2] = RB_IF(2,0)*J1[0] + RB_IF(2,1)*J1[1] + RB_IF(2,2)*J1[2];

         #ifdef DEBUG_RELATIVISTIC_CORRECTION
            MessageInterface::ShowMessage("posWRTSun = %12.10f   %12.10f   %12.10f\n", posWRTSun[0], posWRTSun[1], posWRTSun[2]);
            MessageInterface::ShowMessage("velWRTSun = %12.10f   %12.10f   %12.10f\n", velWRTSun[0], velWRTSun[1], velWRTSun[2]);
            MessageInterface::ShowMessage("big Omega = %12.10f   %12.10f   %12.10f\n", omega[0], omega[1], omega[2]);
            MessageInterface::ShowMessage("J         = %12.10f   %12.10f   %12.10f\n", J[0], J[1], J[2]);
            MessageInterface::ShowMessage("c         = %12.10f\n", c);
            MessageInterface::ShowMessage("sunMu     = %12.10f\n", sunMu);
            MessageInterface::ShowMessage("bodyMu    = %12.10f\n", bodyMu);
         #endif
      }
      Integer nOffset;
      for (Integer n = 0; n < cartesianCount; ++n)
      {
         nOffset = cartesianStart + n * 6;   // stateSize;
         for (Integer i = 0; i < 3; ++i)
         {
            rv[i] = state[i+nOffset];
            vv[i] = state[i+nOffset+3];
         }

         // Compute the Schwarzschild solution
         r             = GmatMathUtil::Sqrt(rv[0] * rv[0] + rv[1] * rv[1] + rv[2] * rv[2]);
         v             = GmatMathUtil::Sqrt(vv[0] * vv[0] + vv[1] * vv[1] + vv[2] * vv[2]);
         s1            = bodyMu / (c * c * r * r * r);
         s2_1          = (4.0 * bodyMu / r) - (v * v);
         s2[0]         = s2_1 * rv[0];
         s2[1]         = s2_1 * rv[1];
         s2[2]         = s2_1 * rv[2];
         rvDotvvX4     = 4.0 * (rv[0] * vv[0] + rv[1] * vv[1] + rv[2] * vv[2]);
         s3[0]         = rvDotvvX4 * vv[0];
         s3[1]         = rvDotvvX4 * vv[1];
         s3[2]         = rvDotvvX4 * vv[2];
         schwarzschild[0] = s1 * (s2[0] + s3[0]);
         schwarzschild[1] = s1 * (s2[1] + s3[1]);
         schwarzschild[2] = s1 * (s2[2] + s3[2]);
         // ONLY IF the body is the Earth, compute the other terms
         if (body->GetName() == GmatSolarSystemDefaults::EARTH_NAME)
         {
            // Compute the geodesic precession
            geodesic[0]       =  2.0 * (omega[1]*vv[2] - omega[2]*vv[1]);
            geodesic[1]       =  2.0 * (omega[2]*vv[0] - omega[0]*vv[2]);
            geodesic[2]       =  2.0 * (omega[0]*vv[1] - omega[1]*vv[0]);

            // Compute the Lense-Thirring precession
//            J = 11.8 * omega.GetUnitVector();  // ******************** TEMPORARY - Steve's value
            rvCrossvv[0] = rv[1]*vv[2] - rv[2]*vv[1];
            rvCrossvv[1] = rv[2]*vv[0] - rv[0]*vv[2];
            rvCrossvv[2] = rv[0]*vv[1] - rv[1]*vv[0];
            vvCrossJ[0]  = vv[1]*J[2]  - vv[2]*J[1];
            vvCrossJ[1]  = vv[2]*J[0]  - vv[0]*J[2];
            vvCrossJ[2]  = vv[0]*J[1]  - vv[1]*J[0];
            lt1          = 2.0 * s1;
            lt2          = (3.0 / (r * r)) * (rv[0] * J[0] + rv[1] * J[1] + rv[2] * J[2]);

            lenseThirring[0]  =  lt1 * ((lt2 * rvCrossvv[0]) + vvCrossJ[0]);
            lenseThirring[1]  =  lt1 * ((lt2 * rvCrossvv[1]) + vvCrossJ[1]);
            lenseThirring[2]  =  lt1 * ((lt2 * rvCrossvv[2]) + vvCrossJ[2]);

            //Add the terms together
            ar[0]  = schwarzschild[0] + geodesic[0] + lenseThirring[0];
            ar[1]  = schwarzschild[1] + geodesic[1] + lenseThirring[1];
            ar[2]  = schwarzschild[2] + geodesic[2] + lenseThirring[2];
         }
         else
         {
            ar[0] = schwarzschild[0];
            ar[1] = schwarzschild[1];
            ar[2] = schwarzschild[2];
         }
         #ifdef DEBUG_RELATIVISTIC_CORRECTION
            MessageInterface::ShowMessage("schwarzschild = %12.10f   %12.10f   %12.10f\n", schwarzschild[0], schwarzschild[1], schwarzschild[2]);
            MessageInterface::ShowMessage("geodesic      = %12.10f   %12.10f   %12.10f\n", geodesic[0], geodesic[1], geodesic[2]);
            MessageInterface::ShowMessage("lenseThirring = %12.10f   %12.10f   %12.10f\n", lenseThirring[0], lenseThirring[1], lenseThirring[2]);
         #endif

         // Fill Derivatives
         switch (order)
         {
            case 1:
               deriv[0+nOffset] = 0.0;
               deriv[1+nOffset] = 0.0;
               deriv[2+nOffset] = 0.0;
               deriv[3+nOffset] = ar[0];
               deriv[4+nOffset] = ar[1];
               deriv[5+nOffset] = ar[2];
               break;

            case 2:
               deriv[0+nOffset] = ar[0];
               deriv[1+nOffset] = ar[1];
               deriv[2+nOffset] = ar[2];
               deriv[3+nOffset] = 0.0;
               deriv[4+nOffset] = 0.0;
               deriv[5+nOffset] = 0.0;
               break;
         }

      }
   }   // fillCartesian


   if (fillSTM)
   {
      // Setting all zeroes for now
      Real aTilde[36];
      Integer element;
      for (Integer i = 0; i < stmCount; ++i)
      {
         Integer i6 = stmStart + i * 36;

         // Calculate A-tilde
         aTilde[ 0] = aTilde[ 1] = aTilde[ 2] =
         aTilde[ 3] = aTilde[ 4] = aTilde[ 5] =
         aTilde[ 6] = aTilde[ 7] = aTilde[ 8] =
         aTilde[ 9] = aTilde[10] = aTilde[11] =
         aTilde[12] = aTilde[13] = aTilde[14] =
         aTilde[15] = aTilde[16] = aTilde[17] =
         aTilde[18] = aTilde[19] = aTilde[20] =
         aTilde[21] = aTilde[22] = aTilde[23] =
         aTilde[24] = aTilde[25] = aTilde[26] =
         aTilde[27] = aTilde[28] = aTilde[29] =
         aTilde[30] = aTilde[31] = aTilde[32] =
         aTilde[33] = aTilde[34] = aTilde[35] = 0.0;

         for (Integer j = 0; j < 6; j++)
         {
            for (Integer k = 0; k < 6; k++)
            {
               element = j * 6 + k;
               #ifdef DEBUG_DERIVATIVES
                  MessageInterface::ShowMessage("------ deriv[%d] = %12.10f\n", (i6+element), aTilde[element]);
               #endif
               deriv[i6+element] = aTilde[element];
            }
         }
      }
   }
   if (fillAMatrix)
   {
      // Setting all zeroes for now
      Real aTilde[36];
      Integer element;
      for (Integer i = 0; i < aMatrixCount; ++i)
      {
         Integer i6 = aMatrixStart + i * 36;

         // Calculate A-tilde
         aTilde[ 0] = aTilde[ 1] = aTilde[ 2] =
         aTilde[ 3] = aTilde[ 4] = aTilde[ 5] =
         aTilde[ 6] = aTilde[ 7] = aTilde[ 8] =
         aTilde[ 9] = aTilde[10] = aTilde[11] =
         aTilde[12] = aTilde[13] = aTilde[14] =
         aTilde[15] = aTilde[16] = aTilde[17] =
         aTilde[18] = aTilde[19] = aTilde[20] =
         aTilde[21] = aTilde[22] = aTilde[23] =
         aTilde[24] = aTilde[25] = aTilde[26] =
         aTilde[27] = aTilde[28] = aTilde[29] =
         aTilde[30] = aTilde[31] = aTilde[32] =
         aTilde[33] = aTilde[34] = aTilde[35] = 0.0;

         for (Integer j = 0; j < 6; j++)
         {
            for (Integer k = 0; k < 6; k++)
            {
               element = j * 6 + k;
               #ifdef DEBUG_DERIVATIVES
                  MessageInterface::ShowMessage("------ deriv[%d] = %12.10f\n", (i6+element), aTilde[element]);
               #endif
               deriv[i6+element] = aTilde[element];
            }
         }
      }
   }

   return true;
}

//------------------------------------------------------------------------------
//  void SetEopFile(EopFile *eopF)
//------------------------------------------------------------------------------
/**
 * Method to set EopFile for this system.
 *
 * @param eopF  the eopFile.
 *
 */
//------------------------------------------------------------------------------
void RelativisticCorrection::SetEopFile(EopFile *eopF)
{
   eop = eopF;
}


//---------------------------------
// inherited methods from GmatBase
//---------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
 /**
* This method returns a clone of the RelativisticCorrection.
 *
 * @return clone of the RelativisticCorrection.
 *
 */
//------------------------------------------------------------------------------
GmatBase* RelativisticCorrection::Clone() const
{
   return (new RelativisticCorrection(*this));
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string RelativisticCorrection::GetParameterText(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < RelativisticCorrectionParamCount))
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer RelativisticCorrection::GetParameterID(const std::string &str) const
{
   for (Integer i = PhysicalModelParamCount; i < RelativisticCorrectionParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - PhysicalModelParamCount])
         return i;
   }
   return PhysicalModel::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType RelativisticCorrection::GetParameterType(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < RelativisticCorrectionParamCount))
      return PARAMETER_TYPE[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string RelativisticCorrection::GetParameterTypeString(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < RelativisticCorrectionParamCount))
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// bool IsParameterReadOnly(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool RelativisticCorrection::IsParameterReadOnly(const Integer id) const
{
   return true;  // all read-only for now
}

//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param id    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real RelativisticCorrection::GetRealParameter(const Integer id) const
{
   if (id == BODY_RADIUS)
      return bodyRadius;

   if (id == BODY_MU)
      return bodyMu;

   return PhysicalModel::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
Real RelativisticCorrection::SetRealParameter(const Integer id, const Real value)
{
   if (id == BODY_RADIUS)
   {
      bodyRadius = value;
      return true;
   }

   if (id == BODY_MU)
   {
      bodyMu = value;
      return true;
   }

   return PhysicalModel::SetRealParameter(id, value);
}

//   virtual bool RelativisticCorrection::GetBooleanParameter(const Integer id) const;
//   virtual bool RelativisticCorrection::SetBooleanParameter(const Integer id, const bool value);
//   virtual Integer RelativisticCorrection::GetIntegerParameter(const Integer id) const;
//   virtual Integer RelativisticCorrection::SetIntegerParameter(const Integer id, const Integer value);


// Methods used by the ODEModel to set the state indexes, etc
//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative
 * information for a specified type.
 *
 * @param id State Element ID for the derivative type
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool RelativisticCorrection::SupportsDerivative(Gmat::StateElementId id)
{
   if (id == Gmat::CARTESIAN_STATE)
      return true;

   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
      return true;

   if (id == Gmat::ORBIT_A_MATRIX)
      return true;

   return PhysicalModel::SupportsDerivative(id);
}

//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Function used to set the start point and size information for the state
 * vector, so that the derivative information can be placed in the correct place
 * in the derivative vector.
 *
 * @param id State Element ID for the derivative type
 * @param index Starting index in the state vector for this type of derivative
 * @param quantity Number of objects that supply this type of data
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool RelativisticCorrection::SetStart(Gmat::StateElementId id, Integer index,
                                      Integer quantity)
{
   bool retval = false;

   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
         satCount = quantity;
         cartesianStart = index;
         cartesianCount = quantity;
         fillCartesian = true;
         retval = true;
         break;

      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
         stmCount = quantity;
         stmStart = index;
         fillSTM = true;
         retval = true;
         break;

      case Gmat::ORBIT_A_MATRIX:
         aMatrixCount = quantity;
         aMatrixStart = index;
         fillAMatrix = true;
         retval = true;
         break;

      default:
         break;
   }

   return retval;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// N/A

//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------
// N/A

