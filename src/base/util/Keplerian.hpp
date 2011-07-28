//$Id$
//------------------------------------------------------------------------------
//                                  Keplerian
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2006/12/28
//
/**
 * Defines KeplerianElement class;  Performs conversions between mean, true and
 * eccentric anomaly.  Converts Cartesian Elements to Keplerian Elements.
 */
//------------------------------------------------------------------------------
#ifndef Keplerian_hpp
#define Keplerian_hpp

#include <iostream>
#include "gmatdefs.hpp"
#include "Anomaly.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "Anomaly.hpp"

class GMAT_API Keplerian            
{
public :
   
   //  Default constructors and destructor and Copy constructor
   Keplerian();
   Keplerian(Real sma, Real ecc, Radians inc, Radians raan, Radians aop,
             Radians ta);
   Keplerian(const Keplerian &keplerian);
   const Keplerian & operator=(const Keplerian &keplerian);
   virtual ~Keplerian();
   
   //  functions to access member data
   Real GetSemimajorAxis() const;
   Real GetEccentricity() const;
   Radians GetInclination() const;
   Radians GetRAAscendingNode() const;
   Radians GetArgumentOfPeriapsis() const;
   Radians GetTrueAnomaly() const;
   Radians GetMeanAnomaly() const;
   
   //  functions to set member data
   void SetSemimajorAxis(Real a);
   void SetEccentricity(Real e);
   void SetInclination(Radians i);
   void SetRAAscendingNode(Radians raan);
   void SetArgumentOfPeriapsis(Radians aop);
   void SetTrueAnomaly(Radians ta);
   void SetMeanAnomaly(Radians ma);
   void SetAll(Real a, Real e, Radians i, Radians raan, Radians aop,
               Radians ta);
   
   Integer GetNumData() const;
   const std::string* GetDataDescriptions() const;
   std::string* ToValueStrings();
   
   // conversion to Cartesian
   static Rvector6 KeplerianToCartesian(Real mu, const Rvector6 &state,
                                        const std::string &anomalyType = "TA");
   
   // computing keplerian elements
   static Real CartesianToSMA(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel);
   static Real CartesianToECC(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel);
   static Real CartesianToINC(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel, bool inRadian = false);
   static Real CartesianToRAAN(Real mu, const Rvector3 &pos,
                               const Rvector3 &vel, bool inRadian = false);
   static Real CartesianToAOP(Real mu, const Rvector3 &pos,
                              const Rvector3 &vel, bool inRadian = false);
   static Rvector3 CartesianToEccVector(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel);
   static Rvector3 CartesianToDirOfLineOfNode(const Rvector3 &pos,
                                              const Rvector3 &vel);
   static Rvector6 CartesianToAngularMomentum(Real mu, const Rvector3 &pos,
                                              const Rvector3 &vel);
   static Rvector6 CartesianToKeplerian(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel,
                                        Anomaly::AnomalyType anomalyType);
   static Rvector6 CartesianToKeplerian(Real mu, const Rvector3 &pos,
                                        const Rvector3 &vel,
                                        const std::string &anomalyType = "TA");
   static Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state,
                                        Anomaly::AnomalyType anomalyType);
   static Rvector6 CartesianToKeplerian(Real mu, const Rvector6 &state,
                                        const std::string &anomalyType = "TA");
   
   // anomaly conversion
   static Real CartesianToTA(Real mu, const Rvector3 &pos,
                             const Rvector3 &vel, bool inRadian = false);
   static Real CartesianToEA(Real mu, const Rvector3 &pos,
                             const Rvector3 &vel, bool inRadian = false);
   static Real CartesianToHA(Real mu, const Rvector3 &pos,
                             const Rvector3 &vel, bool inRadian = false);
   static Real CartesianToMA(Real mu, const Rvector3 &pos,
                             const Rvector3 &vel, bool inRadian = false);
   static Radians TrueToMeanAnomaly(Radians ta, Real ecc, bool modBy2Pi = false);
   static Radians TrueToEccentricAnomaly(Radians ta, Real ecc, bool modBy2Pi = false);
   static Radians TrueToHyperbolicAnomaly(Radians ta, Real ecc, bool modBy2Pi = false);
   
   static Real MeanToTrueAnomaly(Real maInDeg, Real ecc, Real tol = 1.0e-08);
   static Integer ComputeMeanToTrueAnomaly(Real maInDeg, Real ecc, Real tol,
                                           Real *ta, Integer *iter);
   
   // friend stream io
   friend std::ostream& operator<<(std::ostream& output, Keplerian& k);
   friend std::istream& operator>>(std::istream& input, Keplerian& k);
   

protected :

private :

   Real mSemimajorAxis;
   Real mEccentricity;
   Radians mInclination;
   Radians mRaOfAscendingNode;
   Radians mArgOfPeriapsis;
   Radians mTrueAnomaly;
   Radians mMeanAnomaly;
   
   static const Integer NUM_DATA = 6;
   static const std::string DATA_DESCRIPTIONS[NUM_DATA];
   std::string stringValues[NUM_DATA];

};
#endif // Keplerian_hpp
