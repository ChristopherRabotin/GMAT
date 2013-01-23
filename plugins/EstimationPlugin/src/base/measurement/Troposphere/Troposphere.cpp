//$Id: Troposphere.cpp 65 2010-07-22 00:10:28Z tdnguye2@NDC $
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
#include "Troposphere.hpp"
#include "CelestialBody.hpp"
#include "GmatConstants.hpp"
#include "MeasurementException.hpp"
#include "MessageInterface.hpp"

#define DEBUG_TROPOSPHERE_CORRECTION

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------

const Real Troposphere::term2 = 78.8828 / 77.624;



//------------------------------------------------------------------------------
// Troposphere(const std::string& nomme)
//------------------------------------------------------------------------------
/**
 * Standard constructor
 */
//------------------------------------------------------------------------------
Troposphere::Troposphere(const std::string& nomme) :
	MediaCorrection("Troposphere", nomme)
{
	objectTypeNames.push_back("Troposphere");
	model = 1;						// 1 for Troposphere model

	temperature = 295.1;		//286;		// 286K
	pressure = 1013.5;			//938.0;	// 938.0 hPa = 93800 Pa = 0.926 atm
	humidityFraction = 0.55;	//0.73;		// 73 %
}

//------------------------------------------------------------------------------
// ~Troposphere()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Troposphere::~Troposphere()
{
}

//------------------------------------------------------------------------------
// Troposphere(const Troposphere& tps)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
Troposphere::Troposphere(const Troposphere& tps):
		MediaCorrection(tps)
{
	pressure 			= tps.pressure;
	temperature 		= tps.temperature;
	humidityFraction 	= tps.humidityFraction;
	elevationAngle 	= tps.elevationAngle;
	range 				= tps.range;
	waveLength 			= tps.waveLength;
}


//-----------------------------------------------------------------------------
// Troposphere& operator=(const Troposphere& mc)
//-----------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param tps The Troposphere that is provides parameters for this one
 *
 * @return This Troposphere, configured to match tps
 */
//-----------------------------------------------------------------------------
Troposphere& Troposphere::operator=(const Troposphere& tps)
{
   if (this != &tps)
   {
      MediaCorrection::operator=(tps);

   	temperature 		= tps.temperature;
   	pressure 			= tps.pressure;
   	humidityFraction 	= tps.humidityFraction;
   	waveLength			= tps.waveLength;
   	elevationAngle		= tps.elevationAngle;
   	range					= tps.range;
   }

   return *this;
}


//------------------------------------------------------------------------------
// GmatBase* Clone()
//------------------------------------------------------------------------------
/**
 * Clone a Troposphere object
 */
//------------------------------------------------------------------------------
GmatBase* Troposphere::Clone() const
{
	return new Troposphere(*this);
}


//------------------------------------------------------------------------------
// bool SetTemperature(Real T)
//------------------------------------------------------------------------------
/**
 * Set temperature
 */
//------------------------------------------------------------------------------
bool Troposphere::SetTemperature(Real T)
{
	temperature = T;
	return true;
}

//------------------------------------------------------------------------------
// bool SetPressure(Real P)
//------------------------------------------------------------------------------
/**
 * Set pressure
 */
//------------------------------------------------------------------------------
bool Troposphere::SetPressure(Real P)
{
	pressure = P;
	return true;
}


//------------------------------------------------------------------------------
// bool SetHumidityFraction(Real humFr)
//------------------------------------------------------------------------------
/**
 * Set humidity fraction
 */
//------------------------------------------------------------------------------
bool Troposphere::SetHumidityFraction(Real humFr)
{
	humidityFraction = humFr;
	return true;
}

//------------------------------------------------------------------------------
// bool SetElevationAngle(Real elevation)
//------------------------------------------------------------------------------
/**
 * Set elevation angle
 */
//------------------------------------------------------------------------------
bool Troposphere::SetElevationAngle(Real elevation)
{
	elevationAngle = elevation;
	return true;
}

//------------------------------------------------------------------------------
// bool SetRange(Real r)
//------------------------------------------------------------------------------
/**
 * Set range
 */
//------------------------------------------------------------------------------
bool Troposphere::SetRange(Real r)
{
	range = r;
	return true;
}

//------------------------------------------------------------------------------
// bool SetWaveLength(Real lambda)
//------------------------------------------------------------------------------
/**
 * Set wave length
 */
//------------------------------------------------------------------------------
bool Troposphere::SetWaveLength(Real lambda)
{
	waveLength = lambda;
	return true;
}


//------------------------------------------------------------------------------
// RealArray Correction()
//------------------------------------------------------------------------------
/** Compute refraction corrections.
*  p double containing pressure in hPa
*  T double containing temperature in deg K
*  fh double containing relative humidity (0 <= fh <= 1)
*  E double containing elevation angle in radians
*  rho double containing range in m
*  return double[] containing tropospheric refraction corrections for range (m)
*                             elevation (arcsec) measurements
*                             media correction time delay (second)
*/
//------------------------------------------------------------------------------
RealArray Troposphere::Correction()
{
   // Determine Re value
   if (!solarSystem)
   {
	  MessageInterface::ShowMessage("Troposphere::Correction: Solar System is NULL; Cannot obtain Earth radius\n");
      throw MeasurementException("Troposphere::Correction: Solar System is NULL; Cannot obtain Earth radius\n");
   }
   CelestialBody *earth= solarSystem->GetBody(GmatSolarSystemDefaults::EARTH_NAME);
   if (!earth)
   {
	  MessageInterface::ShowMessage("Troposphere::Correction: Cannot obtain Earth radius\n");
      throw MeasurementException("Troposphere::Correction: Cannot obtain Earth radius\n");
   }
   Real Re = earth->GetEquatorialRadius()*GmatMathConstants::KM_TO_M;			// get Erath radius in meters

#ifdef DEBUG_TROPOSPHERE_CORRECTION
   MessageInterface::ShowMessage("Troposphere::Correction():\n");
   MessageInterface::ShowMessage("   temperature = %f K ,  pressure = %f hPa,  humidity = %f\n", temperature, pressure, humidityFraction);
   MessageInterface::ShowMessage("   range = %lfm ,  elevationAngle = %lf radian,  waveLenght = %lfm\n", range, elevationAngle, waveLength);
   MessageInterface::ShowMessage("   earth radius = %lf m\n",Re);
#endif

	// Specify Ce and Crho:
	double lambda = waveLength;
	double lp2_inv = 1.0 / ((lambda * 1.0E+06)*(lambda * 1.0E+06));
	double denom = (173.3 - lp2_inv);
	double term1 = 170.2649 / denom;
	double Ce = term1*term2;
	double term3 = (173.3 + lp2_inv)/denom;
	double Crho = Ce * term3;

#ifdef DEBUG_TROPOSPHERE_CORRECTION
   MessageInterface::ShowMessage("   Ce = %lf ,  Crho = %lf\n", Ce, Crho);
#endif

	// Specify inputs:
	double p = pressure;
	double T = temperature;
	double fh = humidityFraction;
	double E = elevationAngle;
	double rho = range;

	// refractivities
	double N[2];
	// compute dry component refractivity
	N[0] = 77.624 * p / T;

	// compute wet component refractivity
   double Tc = T + GmatPhysicalConstants::ABSOLUTE_ZERO_C;
	double e = 6.10 * fh * exp(17.15 * Tc /(234.7 + Tc));
	N[1] = 371900.0 * e / (T*T) - 12.92 * e/T;


	// troposphere heights
	double h[2];
	// compute dry troposphere height
	h[0] = 5.0 * 0.002277 * p / (N[0] * 1.0E-06);

	// compute wet troposphere height
	h[1] = 5.0 * 0.002277 * e * (1255.0/T + 0.05) / (N[1] * 1.0E-06);

	// distances to top of the troposphere
	double r[2];
	double alpha[9][2];
	double beta[7][2];
	double cosE = cos(E);
	double cosE2 = cosE * cosE;
	double sinE = sin(E);

	for (int j = 0; j < 2; j++)
	{
		r[j] = sqrt((Re + h[j])*(Re + h[j]) - (Re*Re*cosE2)) - Re*sinE;
		double aj = -1.0 * sinE/h[j];
		double bj = - 1.0 * cosE2/(2.0 * h[j] * Re);
		alpha[0][j] = 1.0;
		alpha[1][j] = 4.0*aj;
		alpha[2][j] = 6.0*aj*aj + 4.0*bj;
		alpha[3][j] = 4.0*aj*(aj*aj + 3.0*bj);
		alpha[4][j] = pow(aj, 4) + 12.0*aj*aj*bj + 6.0*bj*bj;
		alpha[5][j] = 4.0*aj*bj*(aj*aj + 3.0*bj);
		alpha[6][j] = bj*bj*(6.0*aj*aj + 4.0*bj);
		alpha[7][j] = 4.0 * aj * bj*bj*bj;
		alpha[8][j] = pow(bj,4);
		beta[0][j] = 1.0;
		beta[1][j] = 3.0*aj;
		beta[2][j] = 3.0*(aj*aj + bj);
		beta[3][j] = aj*(aj*aj + 6.0*bj);
		beta[4][j] = 3.0*bj*(aj*aj + bj);
		beta[5][j] = 3.0 * aj * bj*bj;
		beta[6][j] = pow(bj,3);
	}

	double drho = 0.0;
	double dE = 0.0;
	for (int j = 0; j < 2; j++)
	{
		double sum1 = 0.0;
		for (int i = 0; i < 9; i++)
		{
			double ii = (double)i;
			double temp1 = alpha[i][j]*pow(r[j],(i+1))/(ii+1.0);
			sum1 = sum1 + temp1;
		}
		double sum2 = 0.0;
		for (int k = 0; k < 7; k++)
		{
			double kk = (double)k;
			double temp2 = beta[k][j]*pow(r[j],(k+2))/((kk+1.0)*(kk+2.0)) + beta[k][j]*pow(r[j],(k+1))*(rho - r[j])/(kk+1);
			sum2 = sum2 + temp2;
		}
		drho = drho + N[j] * 1.0E-06 * sum1;
		dE = dE + N[j] * 1.0E-06 * sum2 / h[j];
	}
	drho = Crho * drho;
	dE = Ce * 4.0 * cosE * dE/ rho;
	dE = dE / GmatMathConstants::RAD_PER_ARCSEC;
	
	RealArray out;
	out.push_back(drho);
	out.push_back(dE);
	out.push_back(drho/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);

#ifdef DEBUG_TROPOSPHERE_CORRECTION
	MessageInterface::ShowMessage(" Troposphere correction result:\n");
	MessageInterface::ShowMessage("   Range correction = %f m\n", drho);
	MessageInterface::ShowMessage("   Elevation angle correction = %f arcsec", dE);
	MessageInterface::ShowMessage("   Time correction = %f sec\n", drho/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM); 
#endif

	return out;
}
