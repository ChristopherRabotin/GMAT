//$Header$
//------------------------------------------------------------------------------
//                             Anomaly 
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Joey Gurganus 
// Created: 2005/02/17
//
/**
 * Implements Anomaly class to compute the true, mean, and eccentric anomaly 
 * using the semi-major axis and eccentricity.
 *
 */
//------------------------------------------------------------------------------

#include "Anomaly.hpp"

#define __DEBUG_ANOMALY__ 0

#if __DEBUG_ANOMALY__
#include <iostream>
#include <iomanip>
#endif

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Anomaly::Anomaly() 
//------------------------------------------------------------------------------
/**
 * Creates default constructor.
 */
Anomaly::Anomaly() :
    semiMajorAxis     (0.0),
    eccentricity      (0.0),
    anomalyValue      (0.0),
    type              ("TA")
{
}

//------------------------------------------------------------------------------
//  Anomaly::Anomaly(const std::string &mType)
//------------------------------------------------------------------------------
/**
 * Creates constructor with a parameter.
 * 
 * @param <mType>  Anomaly type.
 *
 */
Anomaly::Anomaly(const std::string &mType) :
    semiMajorAxis     (0.0),
    eccentricity      (0.0),
    anomalyValue      (0.0)
{
   // check for invalid type then use default
   if (IsInvalid(mType))
      type = "TA";
   else
      type = mType;
}

//------------------------------------------------------------------------------
//  Anomaly::Anomaly(const Real a,  const Real e, const Real value)
//------------------------------------------------------------------------------
/**
 * Creates constructor with parameters.
 * 
 * @param <a>      Semi-major axis.
 * @param <e>      Eccentricity.
 * @param <value>  Anomaly value.
 *
 */
Anomaly::Anomaly(const Real a,  const Real e, const Real value) :
    semiMajorAxis     (a),
    eccentricity      (e),
    anomalyValue      (value),
    type              ("TA")
{
}

//------------------------------------------------------------------------------
//  Anomaly::Anomaly(const Real a,  const Real e, Real value, 
//                   const std::string &mType)
//------------------------------------------------------------------------------
/**
 * Creates constructor with parameters.
 * 
 * @param <a>      Semi-major axis.
 * @param <e>      Eccentricity.
 * @param <value>  Anomaly value. 
 * @param <mType>  Anomaly type.
 *
 */
Anomaly::Anomaly(const Real a,  const Real e, Real value, 
                 const std::string &mType) :
    semiMajorAxis     (a),
    eccentricity      (e),
    anomalyValue      (value)
{
   // Check for invalid type then use default
   if (IsInvalid(mType))
      type = "TA";
   else
      type = mType;
}

//------------------------------------------------------------------------------
//   Anomaly::Anomaly(const Anomaly &anomaly)
//------------------------------------------------------------------------------
/**
 * Copy Constructor for base Anomaly structures.
 *
 * @param <anomaly> The original that is being copied.
 */
Anomaly::Anomaly(const Anomaly &anomaly) :
    semiMajorAxis  (anomaly.semiMajorAxis),
    eccentricity   (anomaly.eccentricity),
    anomalyValue   (anomaly.anomalyValue),
    type           (anomaly.type)
{
}

//------------------------------------------------------------------------------
//  Anomaly& Anomaly::operator=(const Anomaly &anomaly)
//------------------------------------------------------------------------------
/**
 * Assignment operator for Anomaly structures.
 *
 * @param <anomaly> The original that is being copied.
 *
 * @return Reference to this object.
 */
Anomaly& Anomaly::operator=(const Anomaly &anomaly)
{
   if (this != &anomaly)
   {
      semiMajorAxis = anomaly.GetSMA();
      eccentricity  = anomaly.GetECC();
      anomalyValue  = anomaly.GetValue();
      type          = anomaly.GetType();
   }
   return *this;
}

//------------------------------------------------------------------------------
//  Anomaly::~Anomaly() 
//------------------------------------------------------------------------------
/**
 * Creates destructor.
 */
Anomaly::~Anomaly() 
{
}

//------------------------------------------------------------------------------
//  void Anomaly::Set(const Real a,  const Real e, Real value, 
//                    const std::string &mType)
//------------------------------------------------------------------------------
/**
 * Set the whole anomaly with parameters.
 * 
 * @param <a>      Semi-major axis.
 * @param <e>      Eccentricity.
 * @param <value>  Anomaly value. 
 * @param <mType>  Anomaly type.
 *
 */
void Anomaly::Set(const Real a,  const Real e, Real value, 
                  const std::string &mType)
{
    SetSMA(a);
    SetECC(e);
    SetValue(value);
    SetType(mType);
}

//------------------------------------------------------------------------------
// Real Anomaly::GetSMA() const
//------------------------------------------------------------------------------
/** 
 * Gets semi-major axis.
 * 
 * @return  Semi-major axis.
 *
 */
Real Anomaly::GetSMA() const
{
   return semiMajorAxis;
}

//------------------------------------------------------------------------------
// void Anomaly::SetSMA(const Real a)
//------------------------------------------------------------------------------
/** 
 * Sets semi-major axis.
 * 
 * @param <a>  Semi-major axis.
 *
 */
void Anomaly::SetSMA(const Real a)
{
   semiMajorAxis = a;
}

//------------------------------------------------------------------------------
// Real Anomaly::GetECC() const
//------------------------------------------------------------------------------
/** 
 * Gets eccentricity.
 * 
 * @return Eccentricity.
 *
 */
Real Anomaly::GetECC() const
{
   return eccentricity;
}

//------------------------------------------------------------------------------
// void Anomaly::SetECC(const Real e)
//------------------------------------------------------------------------------
/** 
 * Sets eccentricity.
 * 
 * @param <e> Eccentricity.
 *
 */
void Anomaly::SetECC(const Real e)
{
   eccentricity = e;
}

//------------------------------------------------------------------------------
// Real Anomaly::GetValue() const
//------------------------------------------------------------------------------
/** 
 * Gets anomaly value.
 * 
 * @return Anomaly value.
 *
 */
Real Anomaly::GetValue() const
{
   return anomalyValue;
}

//------------------------------------------------------------------------------
// void Anomaly::SetValue(const Real value)
//------------------------------------------------------------------------------
/** 
 * Sets anomaly value.
 * 
 * @param <value> Anomaly value.
 *
 */
void Anomaly::SetValue(const Real value)
{
   anomalyValue = value;
}

//------------------------------------------------------------------------------
// std::string Anomaly::GetType() const
//------------------------------------------------------------------------------
/** 
 * Gets anomaly type.
 * 
 * @return Anomaly type.
 *
 */
std::string Anomaly::GetType() const
{
   return type;
}

//------------------------------------------------------------------------------
// void Anomaly::SetType(const std::string &t)
//------------------------------------------------------------------------------
/** 
 * Sets anomaly value.
 * 
 * @param <t> Anomaly type.
 *
 */
void Anomaly::SetType(const std::string &t)
{
   if (IsInvalid(t))
      throw UtilityException("Anomaly::SetType() - invalid input type");

   if (type == "TA" && t == "MA")
      anomalyValue = GetMeanAnomaly();   

   else if (type == "TA" && t == "EA")
      anomalyValue = GetEccentricAnomaly();   

   else if (type == "MA" && t == "EA")
      anomalyValue = GetEccentricAnomaly();   

   else if (type == "MA" && t == "TA")
      anomalyValue = GetTrueAnomaly();   

   else if (type == "EA" && t == "MA")
      anomalyValue = GetMeanAnomaly();   

   else if (type == "EA" && t == "TA")
      anomalyValue = GetTrueAnomaly();   
 
   type = t;
}

//------------------------------------------------------------------------------
// Real Anomaly::GetTrueAnomaly()
//------------------------------------------------------------------------------
/** 
 * Gets true anomaly.
 * 
 * @return    value of true anomaly.
 *
 */
Real Anomaly::GetTrueAnomaly()
{
   Real ta;
   if (type == "MA")
   {
      try
      {
         ta = CoordUtil::MeanToTrueAnomaly(anomalyValue,eccentricity);
         return ta;
      }
      catch(UtilityException &ue)
      {
         std::string msg = "Anomaly::GetTrueAnomaly - " + ue.GetMessage();
         throw UtilityException(msg); 
      }
   } 
   else if (type == "EA")
   {
      Real ma = GetMeanAnomaly();
      ta = CoordUtil::MeanToTrueAnomaly(ma, eccentricity);
      return ta;
   }
   return anomalyValue;   
}

//------------------------------------------------------------------------------
// Real Anomaly::GetMeanAnomaly()
//------------------------------------------------------------------------------
/** 
 * Gets mean anomaly.
 * 
 * @return    value of mean anomaly.
 *
 */
Real Anomaly::GetMeanAnomaly()
{
#if __DEBUG_ANOMALY__
    std::cout << "\nAnomaly::GetMeanAnomlay() with type = " << type 
              << "\na = " << semiMajorAxis
              << ", and e = " << eccentricity << std::endl;
#endif

   if (type == "TA")
   {
      Real q = GmatMathUtil::DegToRad(anomalyValue);

      // Determine it is ellipse or hyperbola for computing the mean anomaly
      if (semiMajorAxis >= 0.0)
      {
         try
         {
            Real div = 1.0 + eccentricity * GmatMathUtil::Cos(q);
        
            // Check if it is tolerance then send the message and discontinue
            if (GmatMathUtil::Abs(div) < ANOMALY_TOL)
              throw UtilityException("Anomaly::GetMeanAnomaly() - "
                                    "unable to get the mean anomaly");

             Real cosea, sinea, ea, ava;
             cosea = (eccentricity + GmatMathUtil::Cos(q))/div;
             sinea = GmatMathUtil::Sqrt(1.0 - eccentricity*eccentricity) * 
                     GmatMathUtil::Sin(q)/div;
             ea = GmatMathUtil::ATan(sinea,cosea); 
             ava = ea - eccentricity * GmatMathUtil::Sin(ea);

             if (ava < 0.0) 
                ava = ava + GmatMathUtil::TWO_PI;

             return (GmatMathUtil::RadToDeg(ava));
         }
         catch(RealUtilitiesExceptions::ArgumentError &rue)
         {
            throw UtilityException("Anomaly::GetMeanAnomaly() - "
                                    "unable to get the mean anomay due to "
                                    "failure of use Cos, Sin, or ATan"); 
         }
      }
      else          // for hyperbolic orbit
      {
         try
         {
            Real denom = GmatMathUtil::Cos(q/2.0);
        
            // Check if it is tolerance then send the message and discontinue
            if (GmatMathUtil::Abs(denom) < ANOMALY_TOL)
              throw UtilityException("Anomaly::GetMeanAnomaly() - "
                                    "unable to get the mean anomaly");

            Real tang = GmatMathUtil::Sqrt((eccentricity - 1.0) / 
                        (eccentricity + 1.0)) *
                        GmatMathUtil::Sin(q/2.0) / denom;
            denom = 1.0 - tang*tang;

            // Check if it is tolerance then send the message and discontinue
            if (GmatMathUtil::Abs(denom) < ANOMALY_TOL)
              throw UtilityException("Anomaly::GetMeanAnomaly() - "
                                    "unable to get the mean anomaly");

            Real sinhf, auxf;
            sinhf = (2.0 * tang)/denom;
            auxf = GmatMathUtil::Log(sinhf + 
                   GmatMathUtil::Sqrt(sinhf * sinhf + 1.0));

            return (GmatMathUtil::RadToDeg(eccentricity * sinhf - auxf));
         } 
         catch(RealUtilitiesExceptions::ArgumentError &rue)
         {
            throw UtilityException("Anomaly::GetMeanAnomaly() - "
                                    "unable to get the mean anomaly due to "
                                    "failure of use Cos, Sin, or ATan"); 
         }
      }
   }
   else if (type == "EA")
   {
      Real ea = GmatMathUtil::DegToRad(anomalyValue);
      Real ma = ea - eccentricity * GmatMathUtil::Sin(ea);
      return GmatMathUtil::RadToDeg(ma);
   }
   
   return anomalyValue; 
}

//------------------------------------------------------------------------------
// Real Anomaly::GetEccentricAnomaly()
//------------------------------------------------------------------------------
/** 
 * Gets eccentric anomaly.
 * 
 * @return   value of eccentric anomaly.
 *
 */
Real Anomaly::GetEccentricAnomaly()
{
#if __DEBUG_ANOMALY__
   std::cout << std::setprecision(30);
   std::cout << "\nAnomaly::GetEccentricAnomaly() enters...\n";
#endif

   Real ma;     // Mean Anomaly

   if (type == "TA")
      ma = GetMeanAnomaly();
   else if (type == "MA")
      ma = anomalyValue;
   else
      return anomalyValue;

   // Check if it is circular orbit then return the mean anomaly 
   if (eccentricity == 0.0)
      return ma;

   // Covernt Mean anomaly to radians and initialize the eccentric anomaly 
   ma = GmatMathUtil::DegToRad(ma);
   
   Real E0, E1;
   E0 = ma;

   try
   {
      for (Integer i = 0; i < MAX_ITERATIONS; i++)
      {
         E1 = E0 - (E0 - eccentricity * GmatMathUtil::Sin(E0) - ma) /
              (1.0 - eccentricity * GmatMathUtil::Cos(E0));  

#if __DEBUG_ANOMALY__
   std::cout << "\nE0 = " << E0 << ", E1 = " << E1 << std::endl;
#endif
        if (E0 == E1)
            break;

        E0 = E1;
      }
   }
   catch(RealUtilitiesExceptions::ArgumentError &rue)
   {
      throw UtilityException("Anomaly::GetEccentricAnomaly() - "
                             "unable to get the eccentric anomaly due to "
                             "failure of use Cos or Sin");
   }

#if __DEBUG_ANOMALY__
   std::cout << "\nAnomaly::GetEccentricAnomaly() exits\n";
#endif
   return GmatMathUtil::RadToDeg(E0);
}

//------------------------------------------------------------------------------
// bool Anomaly::IsInvalid(const std::string &inputType)
//------------------------------------------------------------------------------
/** 
 * Determines if the anomlay type is invalid.
 * 
 * @return   true if invalid, false it is valid.
 *
 */
bool Anomaly::IsInvalid(const std::string &inputType)
{
   if (inputType != "EA" && inputType != "MA" && inputType != "TA")
      return true;

   return false;
}
