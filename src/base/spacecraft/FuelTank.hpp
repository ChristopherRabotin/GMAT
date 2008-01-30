//$Id$
//------------------------------------------------------------------------------
//                               FuelTank
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
 * Class definition for the Fuel Tanks.
 */
//------------------------------------------------------------------------------


#ifndef FUELTANK_HPP
#define FUELTANK_HPP

#include "Hardware.hpp"
#include "MessageInterface.hpp"


/**
 * Basic fuel tank model used in finite maneuvers.
 */ 
class GMAT_API FuelTank : public Hardware
{
public:

   FuelTank(std::string nomme);
   virtual ~FuelTank();
   FuelTank(const FuelTank& ft);
   FuelTank&            operator=(const FuelTank& ft);
   
   // Parameter access methods - overridden from GmatBase
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual bool         IsParameterReadOnly(const Integer id) const;
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual bool         GetBooleanParameter(const Integer id) const;
   virtual bool         SetBooleanParameter(const Integer id,
                                            const bool value);
   
   
   // required method for all subclasses
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         Initialize();
   
protected:
   /// Mass of the fuel in the tank
   Real                 fuelMass;
   /// Tank pressure
   Real                 pressure;
   /// Fuel temperature
   Real                 temperature;
   /// Reference temperature
   Real                 refTemperature;
   /// Tank volume
   Real                 volume;
   /// Fuel density
   Real                 density;
   /// Flag indicating is the tank is pressure regulated or blow-down
   bool                 pressureRegulated;
   
   // Parameters used for internal calculations
   
   /// Pressurant volume, a calculated parameter
   Real                 gasVolume;
   /// Baseline product of the pressure and temperature
   Real                 pvBase;
   /// Flag used to force an update to the pressure and temperature calculations
   bool                 initialized;
   
   virtual void         UpdateTank();
   virtual void         DepleteFuel(Real dm);
   
   /// Published parameters for generic fuel tanks
   enum
   {
      FUEL_MASS = HardwareParamCount, 
      PRESSURE, 
      TEMPERATURE,
      REFERENCE_TEMPERATURE,
      VOLUME,
      FUEL_DENSITY,
      PRESSURE_REGULATED,
      FuelTankParamCount
   };
   
   /// Parameter labels
   static const std::string 
                  PARAMETER_TEXT[FuelTankParamCount - HardwareParamCount];
   /// Parameter types
   static const Gmat::ParameterType 
                  PARAMETER_TYPE[FuelTankParamCount - HardwareParamCount];
};

#endif // FUELTANK_HPP
