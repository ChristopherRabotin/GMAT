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
#include "MessageInterface.hpp"
#include <math.h>          // for atan2(double y, double x) & atanh(double)

//#define DEBUG_ANOMALY 1
//#define DEBUG_ANOMALY_GET_SET

//---------------------------------
//  static data
//---------------------------------

const Real Anomaly::ANOMALY_TOL = 1.0e-30;

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
   #ifdef DEBUG_ANOMALY_GET_SET
   MessageInterface::ShowMessage("Entering Anomaly::GetValue, returning %.18f\n",
   anomalyValue);
   #endif
   return anomalyValue;
}

//------------------------------------------------------------------------------
// Real Anomaly::GetValue(const std::string &mType) const
//------------------------------------------------------------------------------
/** 
 * Gets anomaly value from the given anomaly type.
 * 
 @ @param <mType> type of anomaly
 * @return Anomaly value.
 *
 */
Real Anomaly::GetValue(const std::string &mType) const
{
   #ifdef DEBUG_ANOMALY_GET_SET
   MessageInterface::ShowMessage("Entering Anomaly::GetValue with type &s\n",
   mType.c_str());
   #endif
   if (IsInvalid(mType))
      throw UtilityException("Anomaly::GetValue() - invalid input type");

   if (mType == type) return GetValue();

   if (mType == "TA")
      return GetTrueAnomaly();
   else if (mType == "MA") 
      return GetMeanAnomaly();
   else if (mType == "EA")
      return GetEccentricAnomaly();
   else // mType == "HA"
      return GetHyperbolicAnomaly();

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
/* Joey's code commented out(3/28/06) by LTR 
 * Set type should only set the anomaly type NOT convert  	
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
*/ 
   type = t;
}

//------------------------------------------------------------------------------
// Real Anomaly::GetTrueAnomaly() const
//------------------------------------------------------------------------------
/** 
 * Gets true anomaly.
 * 
 * @return    value of true anomaly.
 *
 */
Real Anomaly::GetTrueAnomaly() const
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
   else if (type == "EA" || type == "HA")
   {
      Real ma = GetMeanAnomaly();
      ta = CoordUtil::MeanToTrueAnomaly(ma, eccentricity);
      return ta;
   }
   return anomalyValue;   
}

//------------------------------------------------------------------------------
// Real Anomaly::GetMeanAnomaly() const
//------------------------------------------------------------------------------
/** 
 * Gets mean anomaly.
 * 
 * @return    value of mean anomaly.
 *
 */
Real Anomaly::GetMeanAnomaly() const
{
#if DEBUG_ANOMALY
   MessageInterface::ShowMessage("\nAnomaly::GetMeanAnomaly()..."
             "\n type = %s\n a    = %f\n e    = %f\n value = %f\n", 
             type.c_str(), semiMajorAxis, eccentricity, anomalyValue);
#endif
   Real eccAnomaly, meanAnomaly, hyperAnomaly;
   Real E; // Eccentric anomaly in radians
   Real H; // Hyperbolic anomaly in radians
   Real M; // Mean anomaly in radians

   if (type == "TA")
   {
      // Determine it is ellipse or hyperbola for computing the mean anomaly
      // Ellipse
      if (eccentricity < (1.0 - 1.0e-12))
      {
         try
         {
			// Get the eccentric anamoly
			eccAnomaly = GetEccentricAnomaly();

		 	// Convert the eccentric anomaly from degrees to radians
         	E = GmatMathUtil::DegToRad(eccAnomaly);

			// Compute the mean anamoly
            meanAnomaly = E - eccentricity * GmatMathUtil::Sin(E);
			
            if (meanAnomaly < 0.0) 
            	meanAnomaly = meanAnomaly + GmatMathUtil::TWO_PI;
			return (GmatMathUtil::RadToDeg(meanAnomaly));
         }
         catch(RealUtilitiesExceptions::ArgumentError &rue)
         {
            throw UtilityException("Anomaly::GetMeanAnomaly() - "
                                   "unable to get the mean anomay due to "
                                   "failure of use Sin"); 
         }
      }
      // Hyperbola
      else if (eccentricity > (1.0 + 1.0e-12))
      {
         try
         {
			// Get the hyperbolic anamoly
			hyperAnomaly = GetHyperbolicAnomaly();
			
		 	// Convert the hyperbolic anomaly from degrees to radians
         	H = GmatMathUtil::DegToRad(hyperAnomaly);

			// Compute the mean anamoly 
			meanAnomaly = eccentricity * GmatMathUtil::Sinh(H) - H;

   			if ((meanAnomaly < 0.0) || (meanAnomaly > GmatMathUtil::TWO_PI))
   			     meanAnomaly = GmatMathUtil::Mod(meanAnomaly,GmatMathUtil::TWO_PI);
			return (GmatMathUtil::RadToDeg(meanAnomaly));
         } 
         catch(RealUtilitiesExceptions::ArgumentError &rue)
         {
            throw UtilityException("Anomaly::GetMeanAnomaly() - "
                                   "unable to get the mean anomaly due to "
                                   "failure of use Sinh"); 
         }
      }
      else
      {
         meanAnomaly = 0.0;	
         MessageInterface::ShowMessage
               ("Anomaly::GetMeanAnomaly() "
                "Warning: Orbit is near parabolic in mean "
                "anomaly calculation. Setting MA = 0.0\n");
         MessageInterface::PopupMessage
            (Gmat::WARNING_, "Warning: Orbit is near parabolic in mean "
                             " anomaly calculation. Setting MA = 0.0");
         return (meanAnomaly);
      }
   }
   else if (type == "EA" || type == "HA")
   {
      E = GmatMathUtil::DegToRad(anomalyValue);
      H = E;
      if (type == "EA")
         M = E - eccentricity * GmatMathUtil::Sin(E);
      else
         M = eccentricity * sinh(H) - H;
      
      return GmatMathUtil::RadToDeg(M);
   }
   
   return anomalyValue; 
}

//------------------------------------------------------------------------------
// Real Anomaly::GetEccentricAnomaly() const
//------------------------------------------------------------------------------
/** 
 * Gets eccentric anomaly.
 * 
 * @return   value of eccentric anomaly.
 *
 */
Real Anomaly::GetEccentricAnomaly() const
{
   Real eccAnomaly; // eccentric anomaly
   Real nu;         // true anomaly in radians
   Real ta;         // true anomaly in degrees

   if (eccentricity > (1.0 - 1.0e-12))
   {
   		eccAnomaly = 0.0;
   }
   else
   {
      try
      {
         if (type == "MA" || type == "EA" || type == "HA")
            ta = GetTrueAnomaly();
         else 
			ta = anomalyValue;
			
		 // Convert the true anomaly from degrees to radians
         nu = GmatMathUtil::DegToRad(ta);
         
		 // Compute the eccentric anamoly
         Real numerator1, numerator2, denominator, sinE, cosE;
		 
		 // sin(E)
         numerator1 = GmatMathUtil::Sqrt(1.0 - eccentricity * eccentricity) *
                      GmatMathUtil::Sin(nu);
		 denominator = 1.0 + eccentricity * GmatMathUtil::Cos(nu);
		 sinE = numerator1 / denominator;

		 // cos(E)
		 numerator2 = eccentricity + GmatMathUtil::Cos(nu);
		 cosE = numerator2 / denominator;

		 eccAnomaly = atan2(sinE, cosE);
      }
      catch(RealUtilitiesExceptions::ArgumentError &rue)
      {
         throw UtilityException("Anomaly::GetEccentricAnomaly() - "
                                "unable to get the eccentric anomaly due to "
                                "failure of use Cos, Sin or atan2");
      }
   }

   return (GmatMathUtil::RadToDeg(eccAnomaly));
}

//------------------------------------------------------------------------------
// Real Anomaly::GetHyperbolicAnomaly() const
//------------------------------------------------------------------------------
/** 
 * Gets hyperbolic anomaly.
 * 
 * @return   value of hyperbolic anomaly.
 *
 */
Real Anomaly::GetHyperbolicAnomaly() const
{
   Real hyperAnomaly; // hyperbolic anomaly
   Real nu;           // true anomaly in radians
   Real ta;           // true anomaly in degrees
   
   if (eccentricity < (1.0 + 1.0e-12))
   {
   		hyperAnomaly = 0.0;
   }
   else
   {
      try
      {
         if (type == "MA" || type == "EA" || type == "HA")
            ta = GetTrueAnomaly();
         else 
			ta = anomalyValue;

		 // Convert the true anomaly from degrees to radians
         nu = GmatMathUtil::DegToRad(ta);
         
		 // Compute the hyperbolic anamoly
         Real numerator1, numerator2, denominator, sinhH, coshH, tanhH;
		 
		 // Hyperbolic sin(H)
		 numerator1 = GmatMathUtil::Sin(nu) * 
		    GmatMathUtil::Sqrt(eccentricity * eccentricity - 1.0);
		 denominator = 1.0 + eccentricity * GmatMathUtil::Cos(nu);
		 sinhH = numerator1 / denominator;

		 // Hyperbolic cos(H)
		 numerator2 = eccentricity + GmatMathUtil::Cos(nu);
		 coshH = numerator2 / denominator;

         tanhH = sinhH / coshH;
		 hyperAnomaly = atanh(tanhH);		
      }
      catch(RealUtilitiesExceptions::ArgumentError &rue)
      {
         throw UtilityException("Anomaly::GetHyperbolicAnomaly() - "
                                "unable to get the eccentric anomaly due to "
                                "failure of use Cos or atanh");
      }
   }
   return (GmatMathUtil::RadToDeg(hyperAnomaly));
}

//------------------------------------------------------------------------------
// bool Anomaly::IsInvalid(const std::string &inputType) const
//------------------------------------------------------------------------------
/** 
 * Determines if the anomlay type is invalid.
 * 
 * @return   true if invalid, false it is valid.
 *
 */
bool Anomaly::IsInvalid(const std::string &inputType) const
{
   if (inputType != "EA" && inputType != "HA" && 
       inputType != "MA" && inputType != "TA")
      return true;

   return false;
}

//------------------------------------------------------------------------------
// void Anomaly::Convert(const std::string &t) const
//------------------------------------------------------------------------------
/** 
 * Converts anomaly value.
 * 
 * @param <fromType> Anomaly type to convert from.
 * @param <toType>   Anomaly type to convert to.
 *
 * @return Converted anomaly type 
 */
Real Anomaly::Convert(const std::string &fromType, 
                      const std::string &toType) const
{
   if (IsInvalid(toType))
      throw UtilityException("Anomaly::Convert() - invalid input type");

   Real value = GetValue();
   
   try
   {
      // Convert from TA to MA, EA or HA
      if (fromType == "TA") 
      {
	     if (toType == "MA")
	        value = GetMeanAnomaly();   
	     else if (toType == "EA")
	        value = GetEccentricAnomaly();   
	     else if (toType == "HA")
	        value = GetHyperbolicAnomaly(); 
      }  
      // Convert from MA to TA, EA or HA
	  else if (fromType == "MA") 
	  {
	     if (toType == "TA")
	        value = GetTrueAnomaly(); 
	     else if (toType == "EA")
	        value = GetEccentricAnomaly();   
	     else if (toType == "HA")
	        value = GetHyperbolicAnomaly();   
	  }
      // Convert from EA to MA, TA or HA
	  else if (fromType == "EA") 
	  {
	     if (toType == "MA")
	        value = GetMeanAnomaly(); 
	     else if (toType == "TA")
	        value = GetTrueAnomaly();   
	     else if (toType == "HA")
	        value = GetHyperbolicAnomaly();   
	  }
      // Convert from HA to MA, TA or EA
	  else if (fromType == "HA") 
	  {
	     if (toType == "MA")
	        value = GetMeanAnomaly(); 
	     else if (toType == "TA")
	        value = GetTrueAnomaly();   
	     else if (toType == "EA")
	        value = GetEccentricAnomaly();   
	  }
   }
   catch(RealUtilitiesExceptions::ArgumentError &rue)
   {
      throw UtilityException("Anomaly::GetEccentricAnomaly() - "
                             "unable to get the eccentric anomaly due to "
                             "failure of use Cos, Sin or atan2");
   }
   
   return value;
}

