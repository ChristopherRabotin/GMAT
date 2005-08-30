//$Header$
//------------------------------------------------------------------------------
//                               Thruster
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
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
#include "MessageInterface.hpp"


/**
 * Thruster model used for finite maneuvers
 */
class GMAT_API Thruster : public Hardware
{
public:

   Thruster(std::string nomme);
   virtual ~Thruster();
   Thruster(const Thruster& th);
   Thruster&                  operator=(const Thruster& th);
   
   // Parameter access methods - overridden from GmatBase
   virtual bool               SetRefObject(GmatBase *obj, 
                                    const Gmat::ObjectType type,
                                    const std::string &name = "");
   virtual ObjectArray&       GetRefObjectArray(const Gmat::ObjectType type);
   virtual ObjectArray&       GetRefObjectArray(const std::string& typeString);
   
   virtual std::string        GetParameterText(const Integer id) const;
   virtual Integer            GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                              GetParameterType(const Integer id) const;
   virtual std::string        GetParameterTypeString(const Integer id) const;
   virtual bool               IsParameterReadOnly(const Integer id) const;
   
   virtual Real               GetRealParameter(const Integer id) const;
   virtual Real               SetRealParameter(const Integer id,
                                               const Real value);
   virtual std::string        GetStringParameter(const Integer id) const;
   virtual bool               SetStringParameter(const Integer id, 
                                                 const std::string &value);
//   virtual std::string GetStringParameter(const Integer id,
//                                          const Integer index) const;
//   virtual bool        SetStringParameter(const Integer id, 
//                                          const std::string &value,
//                                          const Integer index);
   virtual bool               GetBooleanParameter(const Integer id) const;
   virtual bool               SetBooleanParameter(const Integer id,
                                                  const bool value);
   
   virtual const StringArray& GetStringArrayParameter(const Integer id) const; 

   virtual bool               TakeAction(const std::string &action,  
                                 const std::string &actionData = "");
   

   // required method for all subclasses
   virtual GmatBase*          Clone() const;
   virtual void               Copy(const GmatBase* inst);
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
protected:
   /// Finite burn instances access thruster data directly
   friend class FiniteBurn;
   
   /// Tank names
   StringArray                tankNames;
   /// The tanks
   std::vector<FuelTank *>    tanks;
   /// Coordinate system name
   std::string                coordinateName;
   // /// Coordinate system -- currently a placeholder, waiting for build 4 code
   // CoordinateSystem     theCoordinates
   /// Array of thrust coefficients
   Real                       cCoefficients[14];
   /// Array of specific impulse coefficients
   Real                       kCoefficients[14];
   
   /// Flag used to turn thruster on or off
   bool                       thrusterFiring;
   /// Thrust scale factor for this thruster
   Real                       thrustScaleFactor;
   /// Thrust direction projected into the specified coordinate system
   Real                       thrustDirection[3];
   /// Current tank pressure
   Real                       pressure;
   /// Current tank temperature divided by reference temperature
   Real                       temperatureRatio;
   /// Most recently calculated thrust
   Real                       thrust;
   /// Most recently calculated specific impulse
   Real                       impulse;
   /// Flag used for constant thrust and Isp
   bool                       constantExpressions;
   /// Flag used for thrust and Isp that only use the first 3 coefficients
   bool                       simpleExpressions;
   /// Most recently calculated mass flow rate
   Real                       mDot;

   /// Published parameters for thrusters
   enum
   {
      TANK = HardwareParamCount, 
      C1,    C2,    C3,    C4,    C5,    C6,    C7, 
      C8,    C9,   C10,   C11,   C12,   C13,   C14, 
      K1,    K2,    K3,    K4,    K5,    K6,    K7, 
      K8,    K9,   K10,   K11,   K12,   K13,   K14,
      THRUSTER_FIRING,
      COORDINATE_SYSTEM, 
      THRUST_SCALE_FACTOR,
      ThrusterParamCount
   };
   
   /// Thruster parameter labels
   static const std::string 
                        PARAMETER_TEXT[ThrusterParamCount - HardwareParamCount];
   /// Thruster parameter types
   static const Gmat::ParameterType 
                        PARAMETER_TYPE[ThrusterParamCount - HardwareParamCount];
                        
   bool                       CalculateThrustAndIsp();
   Real                       CalculateMassFlow();
   
};

#endif // THRUSTER_HPP
