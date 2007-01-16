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
// Arthor:  Joey Gurganus 
// Created: 2005/02/17 
//
/**
 * Definition for the Anomaly class to compute the true, mean, and eccentric
 * anomaly using semi-major axis and eccentricity.
 *
 */
//------------------------------------------------------------------------------
#ifndef Anomaly_hpp
#define Anomaly_hpp

#include "gmatdefs.hpp"
#include "RealTypes.hpp"          // for Radians
#include "GmatGlobal.hpp"         // for DATA_PRECISION

class GMAT_API Anomaly 
{
public:
   
   enum AnomalyType
   {
      TA,
      MA,
      EA,
      HA,
      AnomalyTypeCount
   };
   
   Anomaly();
   Anomaly(Real sma, Real ecc, Real value, AnomalyType type = TA,
           bool valueInRadians = false);
   Anomaly(Real sma, Real ecc, Real value, const std::string &type = "TA",
           bool valueInRadians = false);
   Anomaly(const Anomaly &anomaly);
   Anomaly& operator=(const Anomaly &anomaly);
   virtual ~Anomaly();
   
   void Set(Real sma, Real ecc, Real value, AnomalyType type,
            bool valueInRadians = false);
   void Set(Real sma, Real ecc, Real value, const std::string &type,
            bool valueInRadians = false);
   
   Real GetSMA() const { return mSma; }
   Real GetECC() const { return mEcc; }
   void SetSMA(const Real sma) { mSma = sma; }
   void SetECC(const Real ecc) { mEcc = ecc; }
   
   Real GetValue(bool valueInRadians = false) const;
   Real GetValue(AnomalyType type, bool inRadians = false) const;
   Real GetValue(const std::string &type, bool inRadians = false) const;
   void SetValue(Real value, bool valueInRadians = false);
   
   AnomalyType GetType() const { return mType; }
   AnomalyType GetType(const std::string &typeStr) const;
   void SetType(AnomalyType type) { mType = type; }
   std::string GetTypeString() const;
   void SetType(const std::string &type);
   
   Real GetTrueAnomaly(bool inRadians = false) const;
   Real GetMeanAnomaly(bool inRadians = false) const;
   Real GetEccentricAnomaly(bool inRadians = false) const;
   Real GetHyperbolicAnomaly(bool inRadians = false) const;
   
   bool IsInvalid(const std::string &typeStr) const;
   
   Real Convert(AnomalyType toType, bool inRadians = false) const;   
   Real Convert(const std::string &toType, bool inRadians = false) const;
   
   Anomaly ConvertToAnomaly(AnomalyType toType, bool inRadians = false);
   Anomaly ConvertToAnomaly(const std::string &toType, bool inRadians = false);
   
   std::string ToString(Integer precision = GmatGlobal::DATA_PRECISION);
   
   static AnomalyType GetAnomalyType(const std::string &typeStr);
   static std::string GetTypeString(const std::string &type);
   static std::string GetLongTypeString(const std::string &type);
   static Integer GetTypeCount() { return AnomalyTypeCount; }
   static const std::string* GetLongTypeNameList();
   
protected:
   
private:

   Real mSma;              // SemimajorAxis
   Real mEcc;              // Eccentricity
   Radians mAnomalyInRad;  // in radians
   AnomalyType mType;
   
   static const std::string ANOMALY_LONG_TEXT[AnomalyTypeCount];
   static const std::string ANOMALY_SHORT_TEXT[AnomalyTypeCount];
   
};
#endif // Anomaly_hpp
