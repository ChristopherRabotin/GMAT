//$Id: Troposphere.cpp 65 2010-07-22 00:10:28Z  $
//------------------------------------------------------------------------------
//                         Troposphere Model
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
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
#include "Code500EphemerisFile.hpp"
#include "FileManager.hpp"

//#define DEBUG_TROPOSPHERE_CORRECTION

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

	month = 0;
	mariniData.clear();
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
   month     = tps.month;
   mariniData = tps.mariniData;
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

      month     = tps.month;
      mariniData = tps.mariniData;
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
// bool SetTime(GmatEpoch ep)
//------------------------------------------------------------------------------
/**
 * Set month based on epoch
 */
//------------------------------------------------------------------------------
bool Troposphere::SetTime(GmatEpoch ep)
{
   A1Mjd epoch = ep;
   month = epoch.ToA1Date().GetMonth();
   return true;
}


//------------------------------------------------------------------------------
// RealArray Correction()
//------------------------------------------------------------------------------
/** Compute refraction corrections.
*  Parameters required are determined by the Troposphere model used
*  Supported Troposphere models are HopfieldSaastamoinen and Marini
*  return double[] containing tropospheric refraction corrections for range (m)
*                             elevation (rad) measurements                     // made changes by TUAN NGUYEN
*                             media correction time delay (second)
*/
//------------------------------------------------------------------------------
RealArray Troposphere::Correction()
{
   RealArray out;

   if (modelTypeName == "HopfieldSaastamoinen")
   {
      out = CalculateHS();
   }
   else if (modelTypeName == "Marini")
   {
      out = CalculateMarini();
   }
   else
   {
      MessageInterface::ShowMessage("Troposphere::Correction: Unrecognized Troposphere model " + modelTypeName + " used\n"
                                    "Supported models are HopfieldSaastamoinen and Marini\n");
      throw MeasurementException("Troposphere::Correction: Unrecognized Troposphere model " + modelTypeName + " used\n"
                                 "Supported models are HopfieldSaastamoinen and Marini\n");
   }

#ifdef DEBUG_TROPOSPHERE_CORRECTION
   MessageInterface::ShowMessage(" Troposphere correction result:\n");
   MessageInterface::ShowMessage("   Range correction = %f m\n", out[0]);
   MessageInterface::ShowMessage("   Elevation angle correction = %f rad", out[1]);
   MessageInterface::ShowMessage("   Time correction = %f sec\n", out[2]); 
#endif

   return out;
}


//------------------------------------------------------------------------------
// RealArray CalculateHS()
//------------------------------------------------------------------------------
/** Compute refraction corrections using the HopfieldSaastamoinen model.
*  p double containing pressure in hPa
*  T double containing temperature in deg K
*  fh double containing relative humidity (0 <= fh <= 1)
*  E double containing elevation angle in radians
*  rho double containing range in m
*  return double[] containing tropospheric refraction corrections for range (m)
*                             elevation (rad) measurements                        // made changes by TUAN NGUYEN
*                             media correction time delay (second)
*/
//------------------------------------------------------------------------------
RealArray Troposphere::CalculateHS()
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
   Real Re = earth->GetEquatorialRadius()*GmatMathConstants::KM_TO_M;			// get Earth radius in meters

#ifdef DEBUG_TROPOSPHERE_CORRECTION
   MessageInterface::ShowMessage("Troposphere::Correction(): Using HopfieldSaastamoinen model\n");
   MessageInterface::ShowMessage("   temperature = %f K ,  pressure = %f hPa,  humidity = %f\n", temperature, pressure, humidityFraction);
   MessageInterface::ShowMessage("   range = %lfm ,  elevationAngle = %lf radian,  waveLength = %lfm\n", range, elevationAngle, waveLength);
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
	dE = Ce * 4.0 * cosE * dE/ rho;                          // unit: radian
	//dE = dE / GmatMathConstants::RAD_PER_ARCSEC;           // keep unit to be radian          // made changes by TUAN NGUYEN
	
	RealArray out;
	out.push_back(drho);
	out.push_back(dE);
	out.push_back(drho/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);

	return out;
}


//------------------------------------------------------------------------------
// RealArray CalculateMarini()
//------------------------------------------------------------------------------
/** Compute refraction corrections using the Marini model.
*  Based on GTDS TROPOA.F
*  
*  latitude double containing Ground Station latitude in rad
*  longitude double containing Ground Station longitude in rad
*  month integer containing month of the year
*  rho double containing range in m
*  E double containing elevation angle in radians
*  return double[] containing tropospheric refraction corrections for range (m)
*                             elevation (rad) measurements                            // made changes by TUAN NGUYEN
*                             media correction time delay (second)
*/
//------------------------------------------------------------------------------
RealArray Troposphere::CalculateMarini()
{
#ifdef DEBUG_TROPOSPHERE_CORRECTION
   MessageInterface::ShowMessage("Troposphere::Correction(): Using Marini model\n");
   MessageInterface::ShowMessage("   latitude = %f deg ,  longitude = %f deg, month %d\n", latitude*GmatMathConstants::DEG_PER_RAD, longitude*GmatMathConstants::DEG_PER_RAD, month);
   MessageInterface::ShowMessage("   range = %lf m ,  elevationAngle = %lf radian\n", range, elevationAngle);
#endif

   if (mariniData.size() == 0)
   {
      LoadMariniDataFile();
   }

   // Specify inputs:
   double LATITUDE = latitude;
   double LONGITUDE = longitude;
   Integer MONTH = month - 1; // January = 0
   double ELEVATION = elevationAngle;
   double RGN = range;

   // Specify intermediate variables:
   Integer NS;
   Real HT;
   Real RHO, RS, P, Q, SINEA, COSEA, XIO, XI1, XII1, XII2;
   Real XKO, XMO, XM1, XMM1, XMM2, I, L, M;

   // Specify outputs:
   Real drho, dE;

   /*      SUBROUTINE TROPOA 

   C  PURPOSE:  TO COMPUTE CORRECTIONS DUE TO THE TROPOSPHERE              
                                                                    
   C  REFERENCE:  "CLOSED FORM SATELLITE TRACKING DATA CORRECTIONS FOR     
   C        AN ARBITRARY TROPOSPHERIC PROFILE",JOHN W. MARINI,MARCH 1971,  
   C         GSFC,X-551-71-122.                      */                      

   //C     GET THE MONTHLY MEAN VALUE OF REFRACTIVITY AND SCALE HEIGHT
   TROGET(LATITUDE, LONGITUDE, MONTH, NS, HT);

   //C     SLANT RANGE
   RHO = RGN;

   //C     SOME EQUATORIAL RADIUS
   RS = 6369.96;

   //C     Eq 7-197a p7-83;
   //P = DSQRT(2.0 * HT / RS);
   P = GmatMathUtil::Sqrt(2.0 * HT / RS);

   //C     Eq 7-197b p7-84
   //Q = 1.0D-6 * NS * RS / HT;
   Q = 1.0E-6 * NS * RS / HT;

   //C     SIN AND COS OF ELEVATION
   //SINEA = DSIN(ELEVATION);
   //COSEA = DCOS(ELEVATION);
   SINEA = GmatMathUtil::Sin(ELEVATION);
   COSEA = GmatMathUtil::Cos(ELEVATION);

   //C     Eq 7-203c p7-85
   //XIO = SQRT(PI) / (1.0 - 0.9206 * Q) ** 0.4468
   XIO = GmatMathUtil::Sqrt(GmatMathConstants::PI) / GmatMathUtil::Pow(1.0 - 0.9206 * Q, 0.4468);

   //C     Eq 7-203d p7-86
   XI1 = 2.0 / (1.0 - Q);

   //C     Eq 7-203a p7-85
   //XII = 0.5 - 0.25 * Q;
   XII1 = 0.5 - 0.25 * Q;

   //C     Eq 7-203b p7-85
   //XII2 = 0.75 - 0.5625 * Q + 0.125 * Q**2;
   XII2 = 0.75 - 0.5625 * Q + 0.125 * Q*Q;

   //C     Eq 7-205 p7-86
   //XKO = SQRT(2 * PI) / (1.0 - 0.9408 * Q) ** 0.4759;
   XKO = GmatMathUtil::Sqrt(2 * GmatMathConstants::PI) / GmatMathUtil::Pow(1.0 - 0.9408 * Q, 0.4759);

   //C     Eq 7-204c p7-86
   //XMO = XIO * (1.0 + Q + Q**2 * XIO**2 / 12.0) - 0.5 * Q * XKO;
   XMO = XIO * (1.0 + Q + Q*Q * XIO*XIO / 12.0) - 0.5 * Q * XKO;

   //C     Eq 7-204d p7-86
   XM1 = (2.0 + 0.5 * Q * XIO * XIO) / (1.0 - Q);

   //C     Eq 7-204a p7-86
   XMM1 = 0.5 - 0.375* Q;

   //C     Eq 7-204b p7-86
   //XMM2 = 0.75 * (1.0 - 25.0 / 24.0 * Q + 11.0 / 36.0 * Q**2);
   XMM2 = 0.75 * (1.0 - 25.0 / 24.0 * Q + 11.0 / 36.0 * Q*Q);

   //C     Eq 7-200a p7-84 WHERE F = Eq 7-201 p7-85
   I = BendingIntegral(SINEA, XII1, XII2, XIO, XI1, P);

   //C     Eq 7-199 p7-84
   //L = 1.D0 - I * SINEA + 0.5D-6 * NS * I**2;
   L = 1.0 - I * SINEA + 0.5E-6 * NS * I*I;

   //C     Eq 7-200b p7-84 WHERE F = Eq 7-201 p7-85
   M = BendingIntegral(SINEA, XMM1, XMM2, XMO, XM1, P);

   //C     Range correction in km  = Eq 7-198a p7-84
   //DRANGE = 1.D - 6 * NS * HT * (M - 0.5D - 6 * NS * (RS * COSEA * L) ** 2 / (RHO * HT))
   drho = 1.0E-6 * NS * HT * (M - 0.5E-6 * NS * GmatMathUtil::Pow(RS * COSEA * L, 2) / (RHO * HT));

   dE = 1.0E-6 * NS * COSEA * (I - RS * L / RHO);         // unit: radian

   drho *= GmatMathConstants::KM_TO_M;
   //dE /= GmatMathConstants::RAD_PER_ARCSEC;         // keep unit to be radian     // made changes by TUAN NGUYEN

   RealArray out;
   out.push_back(drho);
   out.push_back(dE);
   out.push_back(drho/GmatPhysicalConstants::SPEED_OF_LIGHT_VACUUM);

   return out;
}


//------------------------------------------------------------------------------
// void Troposphere::TROGET(Real FLATD, Real FLOND, Integer MON, Integer &NS, Real &HT)
//------------------------------------------------------------------------------
/** Load the troposphere data from a data file for the Marini model.
*  Based on GTDS TROGET.F
*  
*  FLATD double containing Ground Station latitude in rad
*  FLOND double containing Ground Station longitude in rad
*  MON integer containing month of the year January = 0
*  NS integer output containing the refractivity
*  HT double output containing the scale height
*/
//------------------------------------------------------------------------------
void Troposphere::TROGET(Real FLATD, Real FLOND, Integer MON, Integer &NS, Real &HT)
{
   Integer NZHMON = 37068;

   for (UnsignedInt ii = 0U; ii < mariniData.size(); ii++)
   {
      if (abs(FLATD*GmatMathConstants::DEG_PER_RAD - mariniData[ii].latitude) < 1.0)
      {
         Real DLON = abs(FLOND*GmatMathConstants::DEG_PER_RAD - mariniData[ii].longitude);

         if (DLON < 1.0 || (360.0 - DLON) < 1.0)
         {
            NZHMON = mariniData[ii].refractivity[MON];
            break;
         }
      }
   }
   
   NS = NZHMON / 100;
   HT = Real(GmatMathUtil::Mod(NZHMON, 100)) * 1.E-1;
}


//------------------------------------------------------------------------------
// void Troposphere::BendingIntegral(Real ALPHA, Real FF1_Tropo, Real FF2_Tropo,
//                                   Real FO, Real F1, Real P)
//------------------------------------------------------------------------------
/** A FUNCTION WHICH APPROXIMATES THE BENDING INTEGRAL, I(ALPHA)
*  Based on GTDS F.F
*  NOTE: 'FF1_Tropo' and 'FF2_Tropo' are necessary because FF1 is an OS X
*        Macro and the code will not compile with original argument name 'FF1'
*/
//------------------------------------------------------------------------------
Real Troposphere::BendingIntegral(Real ALPHA, Real FF1_Tropo, Real FF2_Tropo,
                                  Real FO, Real F1, Real P)
{
   Real PSQ, X1, X2, X3, X4, F;
   Real Q1 = 1.0;

   PSQ = P * P;
   X1 = FF1_Tropo * PSQ;
   X2 = FF2_Tropo * PSQ / FF1_Tropo - X1;
   X3 = FO * FO * FF1_Tropo * (Q1 + X1 / X2) - (Q1 + F1*FF1_Tropo);
   X4 = FO * X1 / X3 / P*1.21313;
   X3 = X2 / X3 * 1.320903;
   X2 = X2 * 1.08885;
   F = X2 / (ALPHA + X3 / (ALPHA + X4));
   F = Q1 / (ALPHA + X1 / (ALPHA + F));

   return F;
}


//------------------------------------------------------------------------------
// void LoadMariniDataFile()
//------------------------------------------------------------------------------
/** Load the data file containing refractivity data used by the Marini model
*/
//------------------------------------------------------------------------------
void Troposphere::LoadMariniDataFile()
{
   FileManager *fm = FileManager::Instance();
   std::string filename = fm->GetFullPathname(FileManager::MARINI_TROPO_FILE);

   std::ifstream tropoFile(filename.c_str());
   if (!tropoFile)
      throw MeasurementException("Error opening troposphere data file \"" + filename + "\"\n");
   
   std::string theLine;
   
   MariniDataStruct mariniLine;
   mariniData.clear();

   while(!tropoFile.eof())
   {
      std::getline(tropoFile,theLine);
      
      std::stringstream ss;
      ss.str(theLine);
      ss >> mariniLine.latitude >> mariniLine.longitude;
      
      for (UnsignedInt jj = 0U; jj < 12; jj++)
      {
         ss >> mariniLine.refractivity[jj];
      }
      
      mariniData.push_back(mariniLine);
   }
      
   if (mariniData.size() == 0)
      throw MeasurementException("Unable to load refractivity data from troposphere data file \"" + filename + "\"\n");
}
