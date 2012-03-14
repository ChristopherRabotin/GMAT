//$Id$
//------------------------------------------------------------------------------
//                            File: HardwareParameters
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. 
//
// Author: Linda Jun
// Created: 2009.03.20
//
/**
 * Declares Hardware related Parameter classes.
 *    FuelTank: FuelMass, Pressure, Temperature, RefTemperature, Volume,
 *              FuelDensity,
 *    Thruster: DutyCycle, ThrustScaleFactor, GravitationalAccel, C1-C16,
 *              K1-K16, ThrustDirections
 */
//------------------------------------------------------------------------------
#ifndef HardwareParameters_hpp
#define HardwareParameters_hpp

#include "gmatdefs.hpp"
#include "HardwareReal.hpp"

class GMAT_API FuelMass : public HardwareReal
{
public:
   
   FuelMass(const std::string &name = "", GmatBase *obj = NULL);
   FuelMass(const FuelMass &copy);
   FuelMass& operator=(const FuelMass &right);
   virtual ~FuelMass();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API Pressure : public HardwareReal
{
public:

   Pressure(const std::string &name = "", GmatBase *obj = NULL);
   Pressure(const Pressure &copy);
   Pressure& operator=(const Pressure &right);
   virtual ~Pressure();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API Temperature : public HardwareReal
{
public:

   Temperature(const std::string &name = "", GmatBase *obj = NULL);
   Temperature(const Temperature &copy);
   Temperature& operator=(const Temperature &right);
   virtual ~Temperature();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API RefTemperature : public HardwareReal
{
public:

   RefTemperature(const std::string &name = "", GmatBase *obj = NULL);
   RefTemperature(const RefTemperature &copy);
   RefTemperature& operator=(const RefTemperature &right);
   virtual ~RefTemperature();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API Volume : public HardwareReal
{
public:

   Volume(const std::string &name = "", GmatBase *obj = NULL);
   Volume(const Volume &copy);
   Volume& operator=(const Volume &right);
   virtual ~Volume();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API FuelDensity : public HardwareReal
{
public:

   FuelDensity(const std::string &name = "", GmatBase *obj = NULL);
   FuelDensity(const FuelDensity &copy);
   FuelDensity& operator=(const FuelDensity &right);
   virtual ~FuelDensity();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API DutyCycle : public HardwareReal
{
public:

   DutyCycle(const std::string &name = "", GmatBase *obj = NULL);
   DutyCycle(const DutyCycle &copy);
   DutyCycle& operator=(const DutyCycle &right);
   virtual ~DutyCycle();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API ThrustScaleFactor : public HardwareReal
{
public:

   ThrustScaleFactor(const std::string &name = "", GmatBase *obj = NULL);
   ThrustScaleFactor(const ThrustScaleFactor &copy);
   ThrustScaleFactor& operator=(const ThrustScaleFactor &right);
   virtual ~ThrustScaleFactor();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API GravitationalAccel : public HardwareReal
{
public:

   GravitationalAccel(const std::string &name = "", GmatBase *obj = NULL);
   GravitationalAccel(const GravitationalAccel &copy);
   GravitationalAccel& operator=(const GravitationalAccel &right);
   virtual ~GravitationalAccel();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:

};

class GMAT_API ThrustCoefficients : public HardwareReal
{
public:

   ThrustCoefficients(const std::string &type = "",
                      const std::string &name = "",
                      GmatBase *obj = NULL);
   ThrustCoefficients(const ThrustCoefficients &copy);
   ThrustCoefficients& operator=(const ThrustCoefficients &right);
   virtual ~ThrustCoefficients();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   Integer mThrustCoeffId;
   
};

class GMAT_API ImpulseCoefficients : public HardwareReal
{
public:

   ImpulseCoefficients(const std::string &type = "",
                       const std::string &name = "",
                       GmatBase *obj = NULL);
   ImpulseCoefficients(const ImpulseCoefficients &copy);
   ImpulseCoefficients& operator=(const ImpulseCoefficients &right);
   virtual ~ImpulseCoefficients();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   Integer mImpulseCoeffId;
   
};

class GMAT_API ThrustDirections : public HardwareReal
{
public:

   ThrustDirections(const std::string &type = "",
                    const std::string &name = "",
                    GmatBase *obj = NULL);
   ThrustDirections(const ThrustDirections &copy);
   ThrustDirections& operator=(const ThrustDirections &right);
   virtual ~ThrustDirections();
   
   // methods inherited from Parameter
   virtual bool Evaluate();
   virtual void SetReal(Real val);
   
   // methods inherited from GmatBase
   virtual GmatBase* Clone(void) const;
   
protected:
   Integer mThrustDirectionId;
   
};

#endif //HardwareParameters_hpp
