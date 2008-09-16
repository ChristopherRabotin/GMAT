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
#include "Keplerian.hpp"
#include "RealUtilities.hpp"
#include "UtilityException.hpp"
#include "StringUtil.hpp"
#include <sstream>

//#define DEBUG_ANOMALY 1

using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------
const std::string Anomaly::ANOMALY_LONG_TEXT[AnomalyTypeCount] =
{
   "True Anomaly", "Mean Anomaly", "Eccentric Anomaly", "Hyperbolic Anomaly",
};

const std::string Anomaly::ANOMALY_SHORT_TEXT[AnomalyTypeCount] =
{
   "TA", "MA", "EA", "HA",
};


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Anomaly() 
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
Anomaly::Anomaly() :
   mSma(0.0), mEcc(0.0), mAnomalyInRad (0.0), mType(TA)
{
}


//------------------------------------------------------------------------------
// Anomaly(Real sma, Real ecc, Real value, AnomalyType type = TA,
//         bool valueInRadians = false)
//------------------------------------------------------------------------------
/**
 * Constructor with parameters.
 * 
 * @param <sma>    Semimajor axis.
 * @param <ecc>    Eccentricity.
 * @param <value>  Anomaly value. 
 * @param <type>   Anomaly type.
 * @param <valueInRadians>  true if value is in radians
 */
//------------------------------------------------------------------------------
Anomaly::Anomaly(Real sma, Real ecc, Real value, AnomalyType type,
                 bool valueInRadians) :
   mSma(sma), mEcc(ecc), mAnomalyInRad (value), mType(type)
{
   if (!valueInRadians)
      mAnomalyInRad = value * RAD_PER_DEG;
   
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("Anomaly::Anomaly() mEcc=%f, mAnomalyInRad=%f, mType=%d\n",
       mEcc, mAnomalyInRad, mType);
   #endif
}


//------------------------------------------------------------------------------
// Anomaly(Real sma, Real ecc, Real value, const std::string &type = "TA",
//         bool valueInRadians = false)
//------------------------------------------------------------------------------
Anomaly::Anomaly(Real sma, Real ecc, Real value, const std::string &type,
                 bool valueInRadians)
{
   Anomaly(sma, ecc, value, GetType(type), valueInRadians);
}


//------------------------------------------------------------------------------
//   Anomaly(const Anomaly &anomaly)
//------------------------------------------------------------------------------
/**
 * Copy Constructor for base Anomaly structures.
 *
 * @param <anomaly> The original that is being copied.
 */
//------------------------------------------------------------------------------
Anomaly::Anomaly(const Anomaly &anomaly) :
   mSma          (anomaly.mSma),
   mEcc          (anomaly.mEcc),
   mAnomalyInRad (anomaly.mAnomalyInRad),
   mType         (anomaly.mType)
{
}


//------------------------------------------------------------------------------
//  Anomaly& operator=(const Anomaly &anomaly)
//------------------------------------------------------------------------------
/**
 * Assignment operator for Anomaly structures.
 *
 * @param <anomaly> The original that is being copied.
 *
 * @return Reference to this object.
 */
//------------------------------------------------------------------------------
Anomaly& Anomaly::operator=(const Anomaly &anomaly)
{
   if (this != &anomaly)
   {
      mSma           = anomaly.mSma;
      mEcc           = anomaly.mEcc;
      mAnomalyInRad  = anomaly.mAnomalyInRad;
      mType          = anomaly.mType;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
//  ~Anomaly() 
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
Anomaly::~Anomaly() 
{
}


//------------------------------------------------------------------------------
// void Set(Real sma, Real ecc, Real value, AnomalyType type,
//          bool valueInRadians = false)
//------------------------------------------------------------------------------
/**
 * Set the whole anomaly with parameters.
 * 
 * @param <sma>    Semimajor axis
 * @param <ecc>    Eccentricity
 * @param <value>  Anomaly value
 * @param <type>   Anomaly type
 * @param <valueInRadians>  true if value is in radians
 */
//------------------------------------------------------------------------------
void Anomaly::Set(Real sma, Real ecc, Real value, AnomalyType type,
                  bool valueInRadians)
{
   mSma = sma;
   mEcc = ecc;
   mType = type;
   
   if (valueInRadians)
      mAnomalyInRad = value;
   else
      mAnomalyInRad = value * RAD_PER_DEG;
}


//------------------------------------------------------------------------------
// void Set(Real sma, Real ecc, Real value, const std::string &type,
//          bool valueInRadians = false)
//------------------------------------------------------------------------------
/**
 * Set the whole anomaly with parameters.
 * 
 * @param <sma>    Semimajor axis
 * @param <ecc>    Eccentricity
 * @param <value>  Anomaly value
 * @param <type>   Anomaly type string
 * @param <valueInRadians>  true if value is in radians
 */
//------------------------------------------------------------------------------
void Anomaly::Set(Real sma, Real ecc, Real value, const std::string &type,
                  bool valueInRadians)
{
   Set(sma, ecc, value, GetType(type), valueInRadians);
}


//------------------------------------------------------------------------------
// Real GetValue(bool inRadians = false) const
//------------------------------------------------------------------------------
/** 
 * Gets anomaly value.
 * 
 * @param <inRadians>  true if output value in radians is requested
 *
 * @return Anomaly value.
 */
//------------------------------------------------------------------------------
Real Anomaly::GetValue(bool inRadians) const
{
   Real value = mAnomalyInRad;
   
   if (!inRadians)
      value = mAnomalyInRad * DEG_PER_RAD;

   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("Anomaly::GetValue() returning %.18f\n", value);
   #endif
   
   return value;
}


//------------------------------------------------------------------------------
// Real GetValue(AnomalyType type, bool inRadians = false) const
//------------------------------------------------------------------------------
/** 
 * Gets anomaly value from the given anomaly type.
 * 
 * @param <type> type of anomaly
 * @param <inRadians>  true if output value in radians is requested
 * @return Anomaly value.
 */
//------------------------------------------------------------------------------
Real Anomaly::GetValue(AnomalyType type, bool inRadians) const
{
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("Anomaly::GetValue() type=%d, inRadians=%d, mType=%d, mAnomalyInRad=%f\n",
       type, inRadians, mType, mAnomalyInRad);
   #endif
   
   return Convert(type, inRadians);
}


//------------------------------------------------------------------------------
// Real GetValue(const std::string &type, bool valueInRadians = false) const
//------------------------------------------------------------------------------
/** 
 * Gets anomaly value from the given anomaly type.
 * 
 * @param <type> type of anomaly
 * @param <inRadians>  true if output value in radians is requested
 * @return Anomaly value.
 */
//------------------------------------------------------------------------------
Real Anomaly::GetValue(const std::string &type, bool valueInRadians) const
{
   return GetValue(GetType(type), valueInRadians);
}


//------------------------------------------------------------------------------
// void SetValue(Real value, bool valueInRadians = false)
//------------------------------------------------------------------------------
/** 
 * Sets anomaly value.
 * 
 * @param <value> Anomaly value.
 *
 */
//------------------------------------------------------------------------------
void Anomaly::SetValue(Real value, bool valueInRadians)
{
   if (valueInRadians)
      mAnomalyInRad = value;
   else
      mAnomalyInRad = value * RAD_PER_DEG;
}


//------------------------------------------------------------------------------
// AnomalyType GetType(const std::string &typeStr) const
//------------------------------------------------------------------------------
/** 
 * @return  AnomalyType of input type string.
 */
//------------------------------------------------------------------------------
Anomaly::AnomalyType Anomaly::GetType(const std::string &typeStr) const
{
   return GetAnomalyType(typeStr);
}


//------------------------------------------------------------------------------
// std::string GetTypeString() const
//------------------------------------------------------------------------------
/*
 * Returns internal type in string
 */
//------------------------------------------------------------------------------
std::string Anomaly::GetTypeString() const
{
   return ANOMALY_SHORT_TEXT[mType];
}


//------------------------------------------------------------------------------
// void SetType(const std::string &type)
//------------------------------------------------------------------------------
/** 
 * Sets anomaly type.
 * 
 * @param <type> Anomaly type string.
 */
//------------------------------------------------------------------------------
void Anomaly::SetType(const std::string &type)
{
   SetType(GetType(type));
}


//------------------------------------------------------------------------------
// Real GetTrueAnomaly(bool inRadians = false) const
//------------------------------------------------------------------------------
/** 
 * Gets true anomaly.
 *
 * @param <inRadians>  true if output value in radians is requested
 * @return  value of true anomaly.
 */
//------------------------------------------------------------------------------
Real Anomaly::GetTrueAnomaly(bool inRadians) const
{
   #if DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("Anomaly::GetTrueAnomaly() mEcc=%f, mAnomalyInRad=%f\n", mEcc, mAnomalyInRad);
   #endif
   
   Real ta = mAnomalyInRad;
   
   if (mType == MA)
   {
      try
      {
         ta = Keplerian::MeanToTrueAnomaly(mAnomalyInRad * DEG_PER_RAD, mEcc) * RAD_PER_DEG;
      }
      catch(UtilityException &ue)
      {
         std::string msg = "Anomaly::GetTrueAnomaly - " + ue.GetFullMessage();
         throw UtilityException(msg); 
      }
   }
   else if (mType == EA || mType == HA)
   {
      if (mSma >= 0.0 && mEcc <= 1.0)
      {
         std::string typeStr = GetTypeString();
         
         throw UtilityException
            ("Anomaly Type: \"" + typeStr + "\", SMA: \"" +
             GmatStringUtil::ToString(mSma) + "\",  and ECC: \"" +
             GmatStringUtil::ToString(mEcc) + "\" are incompatible.");
      }
      
      ta = Keplerian::MeanToTrueAnomaly(mAnomalyInRad * DEG_PER_RAD, mEcc) * RAD_PER_DEG;
   }
   
   if (!inRadians)
      ta = ta * DEG_PER_RAD;
   
   #if DEBUG_ANOMALY
   MessageInterface::ShowMessage("Anomaly::GetTrueAnomaly() returning %f\n", ta);
   #endif
   
   return ta;
}


//------------------------------------------------------------------------------
// Real GetMeanAnomaly(bool inRadians = false) const
//------------------------------------------------------------------------------
/** 
 * Gets mean anomaly.
 * 
 * @param <inRadians>  true if output value in radians is requested
 * @return  value of mean anomaly.
 *
 */
//------------------------------------------------------------------------------
Real Anomaly::GetMeanAnomaly(bool inRadians) const
{
   Real ma = mAnomalyInRad;
   
   if (mType == TA || mType == EA || mType == HA)
   {
      Real ta = GetTrueAnomaly(true);   
      ma = Keplerian::TrueToMeanAnomaly(ta, mEcc);
   }
   
   if (!inRadians)
      ma = ma * DEG_PER_RAD;
   
   #if DEBUG_ANOMALY
   MessageInterface::ShowMessage("Anomaly::GetMeanAnomaly() returning %f\n", ma);
   #endif

   return ma;
}


//------------------------------------------------------------------------------
// Real GetEccentricAnomaly(bool inRadians = false) const
//------------------------------------------------------------------------------
/** 
 * Gets eccentric anomaly.
 * 
 * @param <inRadians>  true if output value in radians is requested
 * @return  value of eccentric anomaly.
 *
 */
//------------------------------------------------------------------------------
Real Anomaly::GetEccentricAnomaly(bool inRadians) const
{
   Real ea = mAnomalyInRad;
   
   if (mType == TA || mType == MA || mType == HA)
   {
      Real ta = GetTrueAnomaly(true);   
      ea = Keplerian::TrueToEccentricAnomaly(ta, mEcc);
   }
   
   if (!inRadians)
      ea = ea * DEG_PER_RAD;
   
   #if DEBUG_ANOMALY
   MessageInterface::ShowMessage("Anomaly::GetEccentricAnomaly() returning %f\n", ea);
   #endif
   
   return ea;
}


//------------------------------------------------------------------------------
// Real GetHyperbolicAnomaly(bool inRadians = false) const
//------------------------------------------------------------------------------
/** 
 * Gets hyperbolic anomaly.
 * 
 * @param <inRadians>  true if output value in radians is requested
 * @return  value of hyperbolic anomaly.
 */
//------------------------------------------------------------------------------
Real Anomaly::GetHyperbolicAnomaly(bool inRadians) const
{
   Real ha = mAnomalyInRad;
   
   if (mType == MA || mType == EA || mType == TA)
   {
      Real ta = GetTrueAnomaly(true);
      ha = Keplerian::TrueToHyperbolicAnomaly(ta, mEcc);
   }
   
   if (!inRadians)
      ha = ha * DEG_PER_RAD;
   
   #if DEBUG_ANOMALY
   MessageInterface::ShowMessage("Anomaly::GetHyperbolicAnomaly() returning %f\n", ha);
   #endif
   
   return ha;
}


//------------------------------------------------------------------------------
// bool IsInvalid(const std::string &typeStr) const
//------------------------------------------------------------------------------
/** 
 * Determines if the anomlay type is invalid.
 * 
 * @return   true if invalid, false it is valid.
 */
//------------------------------------------------------------------------------
bool Anomaly::IsInvalid(const std::string &typeStr) const
{
   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (typeStr == ANOMALY_LONG_TEXT[i])
         return false;
   }
   
   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (typeStr == ANOMALY_SHORT_TEXT[i])
         return false;
   }
   
   return true;
}


//------------------------------------------------------------------------------
// Real Convert(AnomalyType toType, bool inRadians = false) const
//------------------------------------------------------------------------------
/** 
 * Converts anomaly value.
 * 
 * @param <toType>   Anomaly type to convert to.
 * @param <inRadians>  true if output value in radians is requested
 *
 * @return Converted anomaly type 
 */
//------------------------------------------------------------------------------
Real Anomaly::Convert(AnomalyType toType, bool inRadians) const
{
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage
      ("Anomaly::Convert() toType=%d, inRadians=%d, mType=%d, mAnomalyInRad=%f\n",
       toType, inRadians, mType, mAnomalyInRad);
   #endif
   
   Real value;
   
   if (toType == mType)
      value = mAnomalyInRad;
   else if (toType == TA)
      value = GetTrueAnomaly(true);
   else if (toType == MA) 
      value = GetMeanAnomaly(true);
   else if (toType == EA)
      value = GetEccentricAnomaly(true);
   else if (toType == HA)
      value = GetHyperbolicAnomaly(true);
   else
      throw UtilityException("Anomaly::Convert() - invalid input type");
   
   if (!inRadians)
      value = value * DEG_PER_RAD;
   
   #ifdef DEBUG_ANOMALY
   MessageInterface::ShowMessage("Anomaly::Convert() returning %f\n", value);
   #endif
   
   return value;
}


//------------------------------------------------------------------------------
// Real Convert(const std::string &toType, bool inRadians = false) const
//------------------------------------------------------------------------------
/** 
 * Converts anomaly value.
 * 
 * @param <toType>   Anomaly type to convert to.
 * @param <inRadians>  true if output value in radians is requested
 *
 * @return Converted anomaly type 
 */
//------------------------------------------------------------------------------
Real Anomaly::Convert(const std::string &toType,  bool inRadians) const
{
   return Convert(GetType(toType), inRadians);
}


//------------------------------------------------------------------------------
// Anomaly ConvertToAnomaly(AnomalyType toType, bool inRadians = false)
//------------------------------------------------------------------------------
/*
 * Converts internal anomaly using toType and returns new Anomaly.
 *
 * @param <toType>     AnomalyType to convert internal anomaly to
 * @param <inRadians>  true if output value in radians is requested
 */
//------------------------------------------------------------------------------
Anomaly Anomaly::ConvertToAnomaly(AnomalyType toType, bool inRadians)
{
   Anomaly temp = *this;
   Real value = temp.Convert(toType, inRadians);
   temp.SetValue(value);
   return temp;
}


//------------------------------------------------------------------------------
// Anomaly ConvertToAnomaly(const std::string &toType, bool inRadians = false)
//------------------------------------------------------------------------------
/*
 * Converts internal anomaly using toType and returns new Anomaly.
 *
 * @param <toType>     AnomalyType to convert internal anomaly to
 * @param <inRadians>  true if output value in radians is requested
 */
//------------------------------------------------------------------------------
Anomaly Anomaly::ConvertToAnomaly(const std::string &toType, bool inRadians)
{
   return ConvertToAnomaly(GetType(toType), inRadians);
}


//------------------------------------------------------------------------------
//  std::string ToString(Integer precision = GmatIO::DATA_PRECISION)
//------------------------------------------------------------------------------
/**
 * @return data value string
 */
//------------------------------------------------------------------------------
std::string Anomaly::ToString(Integer precision)
{
   std::stringstream ss("");
   ss.precision(precision);
   
   ss << "Anomaly Type: " << GetTypeString();
   ss << ", SMA: " << mSma;
   ss << ", ECC: " << mEcc;
   ss << ", Value: " << GetValue();
      
   return ss.str();
}

//---------------------------------
// static functions
//---------------------------------

//------------------------------------------------------------------------------
// AnomalyType GetAnomalyType(const std::string &typeStr)
//------------------------------------------------------------------------------
/** 
 * @return  AnomalyType of input type string.
 */
//------------------------------------------------------------------------------
Anomaly::AnomalyType Anomaly::GetAnomalyType(const std::string &typeStr)
{
   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (typeStr == ANOMALY_LONG_TEXT[i])
         return (AnomalyType)i;
   }
   
   for (int i=0; i<AnomalyTypeCount; i++)
   {
      if (typeStr == ANOMALY_SHORT_TEXT[i])
         return (AnomalyType)i;
   }
   
   throw UtilityException
      ("Invalid Anomaly Type \"" + typeStr + "\"\nAllowed "
       "are \"TA\", \"MA\", \"EA\", \"HA\" or \n\"True Anomaly\", "
       "\"Mean Anomaly\", \"Eccentric Anomaly\", \"Hyperbolic Anomaly\"");
}


//------------------------------------------------------------------------------
// static std::string GetTypeString(const std::string &type)
//------------------------------------------------------------------------------
/*
 * Returns matching short type string of input type string.
 */
//------------------------------------------------------------------------------
std::string Anomaly::GetTypeString(const std::string &type)
{
   if (type == "True Anomaly" || type == "TA")
      return "TA";
   else if (type == "Mean Anomaly" || type == "MA") 
      return "MA";
   else if (type == "Eccentric Anomaly" || type == "EA")
      return "EA";
   else if (type == "Hyperbolic Anomaly" || type == "HA")
      return "HA";
   else
      throw UtilityException
         ("Invalid Anomaly Type \"" + type + "\"\nAllowed "
          "are \"TA\", \"MA\", \"EA\", \"HA\" or \n\"True Anomaly\", "
          "\"Mean Anomaly\", \"Eccentric Anomaly\", \"Hyperbolic Anomaly\"");
   
}


//------------------------------------------------------------------------------
// static std::string GetLongTypeString(const std::string &type)
//------------------------------------------------------------------------------
/*
 * Returns matching long type string of input type string.
 */
//------------------------------------------------------------------------------
std::string Anomaly::GetLongTypeString(const std::string &type)
{
   if (type == "True Anomaly" || type == "TA")
      return "True Anomaly";
   else if (type == "Mean Anomaly" || type == "MA") 
      return "Mean Anomaly";
   else if (type == "Eccentric Anomaly" || type == "EA")
      return "Eccentric Anomaly";
   else if (type == "Hyperbolic Anomaly" || type == "HA")
      return "Hyperbolic Anomaly";
   else
      throw UtilityException
         ("Invalid Anomaly Type \"" + type + "\"\nAllowed "
          "are \"TA\", \"MA\", \"EA\", \"HA\" or \n\"True Anomaly\", "
          "\"Mean Anomaly\", \"Eccentric Anomaly\", \"Hyperbolic Anomaly\"");
   
}


//------------------------------------------------------------------------------
// static const std::string* GetLongTypeNameList()
//------------------------------------------------------------------------------
const std::string* Anomaly::GetLongTypeNameList()
{
   return ANOMALY_LONG_TEXT;
}

