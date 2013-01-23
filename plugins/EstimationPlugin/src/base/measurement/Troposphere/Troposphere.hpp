//$Id: Troposphere.hpp 65 2010-07-22 00:10:28Z tdnguye2@NDC $
//------------------------------------------------------------------------------
//                         Troposphere Model
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Tuan Dang Nguyen
// Created: 2010/07/22
//
/**
 * Troposphere media correction model.
 */
//------------------------------------------------------------------------------
#ifndef Troposphere_hpp
#define Troposphere_hpp

#include "MediaCorrection.hpp"

#include "math.h"

class Troposphere : public MediaCorrection
{
public:
	Troposphere(const std::string& nomme);
	virtual ~Troposphere();
    Troposphere(const Troposphere& tps);
    Troposphere& operator=(const Troposphere& tps);
    virtual GmatBase*    Clone() const;

	bool SetTemperature(Real T);
	bool SetPressure(Real P);
	bool SetHumidityFraction(Real humFr);
	bool SetWaveLength(Real lambda);
	bool SetElevationAngle(Real elevation);
	bool SetRange(Real r);

	virtual RealArray Correction();		// specify the changes of range, angle, and time

private:
	Real temperature;					// unit: K
	Real pressure;						// unit: hPa
	Real humidityFraction;				// unit: no unit. It's range is from 0 to 1
	Real waveLength;					// unit: m
	Real elevationAngle;				// unit: radian
	Real range;							// unit: m

   static const Real term2;

};

#endif /* Troposphere_hpp */
