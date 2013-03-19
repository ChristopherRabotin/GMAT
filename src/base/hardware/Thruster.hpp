//$Id$
//------------------------------------------------------------------------------
//                               Thruster
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2004/11/08
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class definition for the thrusters.
 */
//------------------------------------------------------------------------------


#ifndef THRUSTER_HPP
#define THRUSTER_HPP

#include "Hardware.hpp"
#include "FuelTank.hpp"
#include "CoordinateSystem.hpp"
#include "CelestialBody.hpp"

// Declare forward reference since Spacecraft owns Thruster
class Spacecraft;

/**
 * Thruster model used for finite maneuvers
 */
class GMAT_API Thruster : public Hardware
{
public:
   
   static const Integer COEFFICIENT_COUNT = 16;
   static const Integer AXES_COUNT = 4;
   
   Thruster(std::string nomme);
   virtual ~Thruster();
   Thruster(const Thruster& th);
   Thruster&            operator=(const Thruster& th);
   
   // required method for all subclasses
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* inst);
   
   // Parameter access methods - overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterCommandModeSettable(const Integer id) const;
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   
   virtual const StringArray&
                        GetStringArrayParameter(const Integer id) const; 
   
   // for enumerated strings
   virtual const StringArray&
                        GetPropertyEnumStrings(const Integer id) const;
   virtual const StringArray&
                        GetPropertyEnumStrings(const std::string &label) const;
   virtual Gmat::ObjectType
                        GetPropertyObjectType(const Integer id) const;
   
   // Ref. object access methods - overridden from GmatBase
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const ObjectTypeArray&
                        GetRefObjectTypeArray();
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual bool         SetRefObject(GmatBase *obj, 
                                     const Gmat::ObjectType type,
                                     const std::string &name = "");
   virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type);
   virtual ObjectArray& GetRefObjectArray(const std::string& typeString);
   
   
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   virtual void         SetSolarSystem(SolarSystem *ss);
   virtual bool         Initialize();
   Real                 CalculateMassFlow();
   
protected:
   /// Finite burn instances access thruster data directly
   friend class FiniteBurn;
   
   /// Solar system used to find the J2000 body, etc.
   SolarSystem                *solarSystem;
   /// Local Coordinate system
   CoordinateSystem           *localCoordSystem;
   /// Coordinate system
   CoordinateSystem           *coordSystem;
   /// Origin object pointer if coordinate system is set to Local
   CelestialBody              *localOrigin;
   /// J2000body pointer
   CelestialBody              *j2000Body;
   /// Secondary Spacecraft object if coordinate system is set to Local
   Spacecraft                 *spacecraft;
   /// Coordinate system name
   std::string                coordSystemName;
   /// Origin name if coordinate system is set to Local
   std::string                localOriginName;
   /// Axes name if coordinate system is set to Local
   std::string                localAxesName;
   /// Name of the J2000 body
   std::string                j2000BodyName;
   /// Name of the Spacecraft that has thruster
   std::string                satName;
   /// Acceleration due to gravity, used to specify Isp in seconds
   Real                       gravityAccel;   
   /// Thrust duty cycle for this thruster
   Real                       dutyCycle;
   /// Thrust scale factor for this thruster
   Real                       thrustScaleFactor;
   /// Current tank pressure
   Real                       pressure;
   /// Current tank temperature divided by reference temperature
   Real                       temperatureRatio;
   /// Most recently calculated thrust
   Real                       thrust;
   /// Most recently calculated specific impulse
   Real                       impulse;
   /// Most recently calculated mass flow rate
   Real                       mDot;
   /// Thrust direction projected into the inertial coordinate system
   Real                       inertialDirection[3];
   /// Array of thrust coefficients
   Real                       cCoefficients[COEFFICIENT_COUNT];
   /// Array of specific impulse coefficients
   Real                       kCoefficients[COEFFICIENT_COUNT];
   /// Decrement mass flag
   bool                       decrementMass;
   /// Flag used to turn thruster on or off
   bool                       thrusterFiring;
   /// Flag used for constant thrust and Isp
   bool                       constantExpressions;
   /// Flag used for thrust and Isp that only use the first 3 coefficients
   bool                       simpleExpressions;
   /// Flag indicating if local coordinate system is used
   bool                       usingLocalCoordSys;
   /// Flag indicating if axes is MJ2000Eq
   bool                       isMJ2000EqAxes;
   /// Flag indicating if axes is SpacecrftBody
   bool                       isSpacecraftBodyAxes;
   /// Tank names
   StringArray                tankNames;
   /// The tanks
   std::vector<FuelTank *>    tanks;
   /// Temporary buffer used to get ref objects
   ObjectArray                tempArray;
   
   /// Available local axes labels
   static  StringArray        localAxesLabels;
   /// C-coefficient units
   static  StringArray        cCoefUnits;
   /// K-coefficient units
   static  StringArray        kCoefUnits;
   
   /// Published parameters for thrusters
   enum
   {
      THRUSTER_FIRING = HardwareParamCount, 
      COORDINATE_SYSTEM,
      ORIGIN,
      AXES,
      DUTY_CYCLE,
      THRUST_SCALE_FACTOR,
      DECREMENT_MASS,
      TANK,
      GRAVITATIONAL_ACCELERATION,
      C1,    C2,    C3,    C4,    C5,    C6,    C7,    C8,    
      C9,   C10,   C11,   C12,   C13,   C14,   C15,   C16, 
      K1,    K2,    K3,    K4,    K5,    K6,    K7,    K8,    
      K9,   K10,   K11,   K12,   K13,   K14,   K15,   K16,
      C_UNITS,
      K_UNITS,
      ThrusterParamCount
   };
   
   /// Thruster parameter labels
   static const std::string
                        PARAMETER_TEXT[ThrusterParamCount - HardwareParamCount];
   /// Thruster parameter types
   static const Gmat::ParameterType
                        PARAMETER_TYPE[ThrusterParamCount - HardwareParamCount];
   
   bool                 CalculateThrustAndIsp();
   bool                 SetSpacecraft(Spacecraft *sat);
   
   CoordinateSystem*    CreateLocalCoordinateSystem();
   void                 ConvertDirectionToInertial(Real *dv, Real *dvInertial,
                                                   Real epoch);
   void                 ComputeInertialDirection(Real epoch);
   void                 WriteDeprecatedMessage(const std::string &oldProp,
                                               const std::string &newProp) const;
   
};

#endif // THRUSTER_HPP
