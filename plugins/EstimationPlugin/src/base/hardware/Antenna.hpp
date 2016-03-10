/*
 * Antenna.h
 *
 *  Created on: Mar 31, 2010
 *      Author: tdnguye2
 */

#ifndef Antenna_hpp
#define Antenna_hpp

#include "estimation_defs.hpp"
#include "Hardware.hpp"

class ESTIMATION_API Antenna : public Hardware
{
public:
   Antenna(const std::string &type, const std::string &name);
   virtual ~Antenna();
   Antenna(const Antenna& ant);
   Antenna& operator=(const Antenna& ant);
   virtual GmatBase* Clone() const;
   virtual void      Copy(const GmatBase* ant);

   // Access methods derived classes can override
   virtual std::string  GetParameterText(const Integer id) const;
   virtual std::string  GetParameterUnit(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;

   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;


   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);

   DEFAULT_TO_NO_REFOBJECTS

protected:
   Real       antennaDelay;
   Real       phaseCenterLocation1;
   Real       phaseCenterLocation2;
   Real       phaseCenterLocation3;

   /// Published parameters for the RF hardware

   enum
   {
      ANTENNA_DELAY = HardwareParamCount,
      PHASE_CENTER_LOCATION1,
      PHASE_CENTER_LOCATION2,
      PHASE_CENTER_LOCATION3,
      AntennaParamCount,
   };

   static const std::string
      PARAMETER_TEXT[AntennaParamCount - HardwareParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[AntennaParamCount - HardwareParamCount];
};

#endif /* Antenna_hpp */
