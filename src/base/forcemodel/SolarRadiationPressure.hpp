//$Id$
//------------------------------------------------------------------------------
//                              SolarRadiationPressure
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
// Author: D.Conway
// Created: 2003/03/14
// Modified: 2003/03/30 Waka Waktola GSFC Code 583
//           Updated for GMAT style, standards.
///
/**
 * This is the SolarRadiationPressure class.
 *
 * @note original prolog information included at end of file prolog.
 */
// *** File Name : SolarRadiationPressure.cpp
// *** Created   : February 27, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// **************************************************************************

#ifndef SolarRadiationPressure_hpp
#define SolarRadiationPressure_hpp

#include "PhysicalModel.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "Rvector6.hpp"
#include "gmatdefs.hpp"

/** 
 * Solar radiation pressure model -- currently incomplete
 * 
 * This class models solar radiation pressure.  The current implementation 
 * builds a force directed along the x-axis with magnitude equal to a nominal
 * SRP acceleration.  The force toggles on and off at 40 minute intervals.
 * 
 * Note that this is <B>NOT</B> an implementation of Solar Radiation Pressure
 * in its current form because (1) the force is not applied in the correct 
 * direction and (2) the magnitude of the force does bot change with distance
 * from the Sun.  The current implementation is used solely to test 
 * discontinuities in the force model.
 */
class GMAT_API SolarRadiationPressure : public PhysicalModel
{
public:
   SolarRadiationPressure(const std::string &name = ""); //loj: 5/28/04 added default
   virtual ~SolarRadiationPressure();
   SolarRadiationPressure(const SolarRadiationPressure &srp);
   SolarRadiationPressure& operator=(const SolarRadiationPressure &srp);

   virtual bool IsUnique(const std::string &forBody = "");

   virtual bool Initialize();
   virtual bool SetCentralBody();
   virtual bool GetDerivatives(Real *state, Real dt = 0.0, Integer order = 1, 
         const Integer id = -1);
   virtual Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc);

   // inherited from GmatBase
   virtual GmatBase* Clone() const;

   // Parameter access methods - overridden from GmatBase
   virtual std::string         GetParameterText(const Integer id) const;
   virtual Integer             GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string         GetParameterTypeString(const Integer id) const;
   virtual bool                IsParameterReadOnly(const Integer id) const;

   virtual Real                GetRealParameter(const Integer id) const;
   virtual Real                SetRealParameter(const Integer id, const Real value);
   virtual bool                GetBooleanParameter(const Integer id) const;
   virtual bool                SetBooleanParameter(const Integer id, const bool value);
   virtual Integer             GetIntegerParameter(const Integer id) const;
   virtual Integer             SetIntegerParameter(const Integer id, const Integer value);
   
   virtual void SetSatelliteParameter(const Integer i, 
                                      const std::string parmName, 
                                      const Real parm,
                                      const Integer parmID = -1);
   virtual void SetSatelliteParameter(const Integer i,
                                      Integer parmID,
                                      const Real parm);
   virtual void ClearSatelliteParameters(const std::string parmName = "");
   
   // Methods used by the ODEModel to set the state indexes, etc
   virtual bool SupportsDerivative(Gmat::StateElementId id);
   virtual bool SetStart(Gmat::StateElementId id, Integer index, 
                         Integer quantity);

   DEFAULT_TO_NO_CLONES
   DEFAULT_TO_NO_REFOBJECTS


protected:
   // Parameter IDs
   enum
   {
      SHADOWMODELTYPE,   /// ID for the type of shadow model to use
      VECTORTYPE        /// ID for the force orientation model to use
//      ParamCount         /// Count of the parameters for this class waw: 04/19/04
   };
    
   /// Enumeration to ease pain of figuring out which shadow model is active
   enum ShadowModel 
   {
      CYLINDRICAL_MODEL = 1, 
      CONICAL_MODEL
   };
    
   /// Enumeration to ease pain of determining which force direction is active
   enum ForceDirection 
   {
      SUN_PARALLEL = 1,
      ATTITUDE_BASED
   };
            
   /// Pointer to the source of planetary ephemerides
   CelestialBody *theSun;

   /// Flag used to indicate using an analytic model to locate the Sun
   bool useAnalytic;
   /// Shadow model type indicator: 1 for cylinder, 2 for cones
   Integer shadowModel;
   /// Vector model: 1 for (anti)parallel to Sun, 2 attitude based
   Integer vectorModel;
   /// Radius of the body casting the shadow
   Real bodyRadius;
   /// Vector to the Sun from the central body -- eventually from DE file
   Real *cbSunVector;
   /// Vector to the Sun from the spacecraft
   Real *forceVector;
   /// Radius of the Sun
   Real sunRadius;
   /// Flag used to indicate that there are other bodies casting shadows
   bool hasMoons;

   /// Basically the reflectivity of the body experiencing the force
   std::vector<Real> cr;
   /// Reflective surface area of the body, in square meters
   std::vector<Real> area;
   /// Mass of the body, in kg
   std::vector<Real> mass;
   /// Solar flux, in W/m^2
   Real flux;
   /// Solar flux, in N/m^2
   Real fluxPressure;
   /// Distance from the Sun, currently set to a dummy value
   Real sunDistance;
   /// Nominal distance to the Sun used in the model: 1 AU
   Real nominalSun;
   /// Flag used to detect if the central body is Sol
   bool bodyIsTheSun;

   // Conical shadow model parameters that are used in multiple methods
   /// Conical model: projected sun radius
   Real psunrad;
   /// Conical model: projected central body radius
   Real pcbrad;
   /// Weighting for partial lighting
   Real percentSun;
   /// Central body ID for the model
   Integer bodyID;
   
   Rvector6 sunrv;
   Rvector6 cbrv;

   /// Number of spacecraft in the state vector that use CartesianState
   Integer              satCount;
//   /// Start index for the Cartesian state
//   Integer              cartIndex;
//   /// Flag indicating if the Cartesian state should be populated
//   bool                 fillCartesian;
//
//   /// Number of spacecraft in the state vector that use OrbitSTM
//   Integer              stmCount;
//   /// Start index for the OrbitSTM
//   Integer              stmIndex;
//   /// Flag indicating if the OrbitSTM should be populated
//   bool                 fillSTM;

   Integer massID;
   Integer crID;
   Integer areaID;


   void FindShadowState(bool &lit, bool &dark, Real *state);
   Real ShadowFunction(Real *state);

private:

   /// Parameter IDs
   enum
   {
      USE_ANALYTIC = PhysicalModelParamCount,
      SHADOW_MODEL,
      VECTOR_MODEL,
      BODY_RADIUS,
      SUN_RADIUS,
      HAS_MOONS,
      CR,
      AREA,
      MASS,
      FLUX,
      FLUX_PRESSURE,
      SUN_DISTANCE,
      NOMINAL_SUN,
      PSUNRAD,
      PCBRAD,
      PERCENT_SUN,
      SRPParamCount  // Count of the parameters for this class
   };
   
   static const std::string PARAMETER_TEXT[SRPParamCount - PhysicalModelParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[SRPParamCount - PhysicalModelParamCount];

};

#endif

