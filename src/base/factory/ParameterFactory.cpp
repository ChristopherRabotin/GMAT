//$Header$
//------------------------------------------------------------------------------
//                            ParameterFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel Conway
// Created: 2003/10/28
//
/**
 *  Implementation code for the ParameterFactory class, responsible
 *  for creating Parameter objects.
 */
//------------------------------------------------------------------------------


#include "ParameterFactory.hpp"

#include "TimeParameters.hpp"
#include "CartesianParameters.hpp"
#include "KeplerianParameters.hpp"
#include "SphericalParameters.hpp"
#include "OrbitalParameters.hpp"
#include "AngularParameters.hpp"
#include "EnvParameters.hpp"
#include "PlanetParameters.hpp"
#include "Variable.hpp"
#include "StringVar.hpp"
#include "Array.hpp"
#include "BplaneParameters.hpp"
#include "BurnParameters.hpp"
#include "AttitudeParameters.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateParameter(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Parameter class
 *
 * @param <ofType> the Parameter object to create and return.
 * @param <withName> the name to give the newly-created Parameter object.
 */
//------------------------------------------------------------------------------
Parameter* ParameterFactory::CreateParameter(const std::string &ofType,
                                             const std::string &withName)
{
   // User defined parameters
   if (ofType == "Variable")
      return new Variable(withName);
   if (ofType == "String")
      return new StringVar(withName);
   if (ofType == "Array")
      return new Array(withName);
   
   // Time parameters
   if (ofType == "ElapsedDays")
      return new ElapsedDays(withName);
   if (ofType == "ElapsedSecs")
      return new ElapsedSecs(withName);
   if (ofType == "CurrA1MJD")
      return new CurrA1MJD(withName);
   if (ofType == "A1ModJulian")
      return new A1ModJulian(withName);
   if (ofType == "A1Gregorian")
      return new A1Gregorian(withName);
   if (ofType == "TAIModJulian")
      return new TAIModJulian(withName);
   if (ofType == "TAIGregorian")
      return new TAIGregorian(withName);
   if (ofType == "TTModJulian")
      return new TTModJulian(withName);
   if (ofType == "TTGregorian")
      return new TTGregorian(withName);
   if (ofType == "TDBModJulian")
      return new TDBModJulian(withName);
   if (ofType == "TDBGregorian")
      return new TDBGregorian(withName);
   if (ofType == "TCBModJulian")
      return new TCBModJulian(withName);
   if (ofType == "TCBGregorian")
      return new TCBGregorian(withName);
   if (ofType == "UTCModJulian")
      return new UTCModJulian(withName);
   if (ofType == "UTCGregorian")
      return new UTCGregorian(withName);

   // Cartesian parameters
   if (ofType == "X")
      return new CartX(withName);
   if (ofType == "Y")
      return new CartY(withName);
   if (ofType == "Z")
      return new CartZ(withName);
   if (ofType == "VX")
      return new CartVx(withName);
   if (ofType == "VY")
      return new CartVy(withName);
   if (ofType == "VZ")
      return new CartVz(withName);
   if (ofType == "Cartesian")
      return new CartState(withName);

    // Keplerian parameters
   if (ofType == "SMA")
      return new KepSMA(withName);
   if (ofType == "ECC")
      return new KepEcc(withName);
   if (ofType == "INC")
      return new KepInc(withName);
   if (ofType == "RAAN")
      return new KepRAAN(withName);
   if (ofType == "RADN")
      return new KepRADN(withName);
   if (ofType == "AOP")
      return new KepAOP(withName);
   if (ofType == "TA")
      return new KepTA(withName);
   if (ofType == "MA")
      return new KepMA(withName);
   if (ofType == "EA")
      return new KepEA(withName);
   if (ofType == "HA")
      return new KepHA(withName);
   if (ofType == "MM")
      return new KepMM(withName);
   if (ofType == "Keplerian")
      return new KepElem(withName);
   if (ofType == "ModKeplerian")
      return new ModKepElem(withName);

   // Spherical parameters
   if (ofType == "RMAG")
      return new SphRMag(withName);
   if (ofType == "RA")
      return new SphRA(withName);
   if (ofType == "DEC")
      return new SphDec(withName);
   if (ofType == "VMAG")
      return new SphVMag(withName);
   if (ofType == "RAV")
      return new SphRAV(withName);
   if (ofType == "DECV")
      return new SphDecV(withName);
   if (ofType == "AZI")
      return new SphAzi(withName);
   if (ofType == "FPA")
      return new SphFPA(withName);
   if (ofType == "SphericalRADEC") 
      return new SphRaDecElem(withName);
   if (ofType == "SphericalAZFPA") 
      return new SphAzFpaElem(withName);
   if (ofType == "Altitude")
      return new Altitude(withName);

   // Orbital parameters
   if (ofType == "VelApoapsis")
      return new VelApoapsis(withName);
   if (ofType == "VelPeriapsis")
      return new VelPeriapsis(withName);
   if (ofType == "Apoapsis")
      return new Apoapsis(withName);
   if (ofType == "Periapsis")
      return new Periapsis(withName);
   if (ofType == "OrbitPeriod")
      return new OrbitPeriod(withName);
   if (ofType == "RadApo")
      return new RadApoapsis(withName);
   if (ofType == "RadPer")
      return new RadPeriapsis(withName);
   if (ofType == "C3Energy")
      return new C3Energy(withName);
   if (ofType == "Energy")
      return new Energy(withName);

   // Angular parameters
   if (ofType == "SemilatusRectum")
      return new SemilatusRectum(withName);
   if (ofType == "HMAG")
      return new AngularMomentumMag(withName);
   if (ofType == "HX")
      return new AngularMomentumX(withName);
   if (ofType == "HY")
      return new AngularMomentumY(withName);
   if (ofType == "HZ")
      return new AngularMomentumZ(withName);

   // Environmental parameters
   if (ofType == "AtmosDensity")
      return new AtmosDensity(withName);
   
   // Planet parameters
   if (ofType == "MHA")
      return new MHA(withName);
   if (ofType == "Longitude")
      return new Longitude(withName);
   if (ofType == "Latitude")
      return new Latitude(withName);
   if (ofType == "LST")
      return new LST(withName);
   if (ofType == "BetaAngle")
      return new BetaAngle(withName);
   
   // B-Plane parameters
   if (ofType == "BdotT")
      return new BdotT(withName);
   if (ofType == "BdotR")
      return new BdotR(withName);
   if (ofType == "BVectorMag")
      return new BVectorMag(withName);
   if (ofType == "BVectorAngle")
      return new BVectorAngle(withName);
   
   // ImpulsiveBurn parameters
   if (ofType == "Element1")
      return new DeltaVDir1(withName, ofType);
   if (ofType == "Element2")
      return new DeltaVDir2(withName, ofType);
   if (ofType == "Element3")
      return new DeltaVDir3(withName, ofType);
   if (ofType == "V")
      return new DeltaVDir1(withName, ofType);
   if (ofType == "N")
      return new DeltaVDir2(withName, ofType);
   if (ofType == "B")
      return new DeltaVDir3(withName, ofType);
   
   // Attitude parameters
   if (ofType == "DCM11" || ofType == "DirectionCosineMatrix11")
      return new DCM11(withName);
   if (ofType == "DCM12" || ofType == "DirectionCosineMatrix12")
      return new DCM12(withName);
   if (ofType == "DCM13" || ofType == "DirectionCosineMatrix13")
      return new DCM13(withName);
   if (ofType == "DCM21" || ofType == "DirectionCosineMatrix21")
      return new DCM21(withName);
   if (ofType == "DCM22" || ofType == "DirectionCosineMatrix22")
      return new DCM22(withName);
   if (ofType == "DCM23" || ofType == "DirectionCosineMatrix23")
      return new DCM23(withName);
   if (ofType == "DCM31" || ofType == "DirectionCosineMatrix31")
      return new DCM31(withName);
   if (ofType == "DCM32" || ofType == "DirectionCosineMatrix32")
      return new DCM32(withName);
   if (ofType == "DCM33" || ofType == "DirectionCosineMatrix33")
      return new DCM33(withName);
   if (ofType == "EulerAngle1")
      return new EulerAngle1(withName);
   if (ofType == "EulerAngle2")
      return new EulerAngle2(withName);
   if (ofType == "EulerAngle3")
      return new EulerAngle3(withName);
   if (ofType == "Q1" || ofType == "q1")
      return new Quat1(withName);
   if (ofType == "Q2" || ofType == "q2")
      return new Quat2(withName);
   if (ofType == "Q3" || ofType == "q3")
      return new Quat3(withName);
   if (ofType == "Q4" || ofType == "q4")
      return new Quat4(withName);
      
   if (ofType == "AngularVelocityX" || ofType == "AngVelX")
      return new AngVelX(withName);
   if (ofType == "AngularVelocityY" || ofType == "AngVelY")
      return new AngVelY(withName);
   if (ofType == "AngularVelocityZ" || ofType == "AngVelZ")
      return new AngVelZ(withName);
   if (ofType == "EulerAngleRate1")
      return new EulerAngleRate1(withName);
   if (ofType == "EulerAngleRate2")
      return new EulerAngleRate2(withName);
   if (ofType == "EulerAngleRate3")
      return new EulerAngleRate3(withName);
   
   // add others here
   
   else
   {
      MessageInterface::ShowMessage
         ("**** ERROR **** Cannot create a parameter with unknown type \"%s\"\n",
          ofType.c_str());
      
      return NULL;
   }
}


//------------------------------------------------------------------------------
//  ParameterFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ParameterFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
ParameterFactory::ParameterFactory()
   : Factory(Gmat::PARAMETER)
{
   if (creatables.empty())
   {
      // User defined parameters
      creatables.push_back("Variable");
      creatables.push_back("String");
      creatables.push_back("Array");
      
      // Time parameters
      creatables.push_back("ElapsedDays");
      creatables.push_back("ElapsedSecs");
      creatables.push_back("CurrA1MJD");
      creatables.push_back("A1ModJulian");
      creatables.push_back("A1Gregorian");
      creatables.push_back("TAIModJulian");
      creatables.push_back("TAIGregorian");
      creatables.push_back("TTModJulian");
      creatables.push_back("TTGregorian");
      creatables.push_back("TDBModJulian");
      creatables.push_back("TDBGregorian");
      creatables.push_back("TCBModJulian");
      creatables.push_back("TCBGregorian");
      creatables.push_back("UTCModJulian");
      creatables.push_back("UTCGregorian");

      // Cartesian parameters
      creatables.push_back("X");
      creatables.push_back("Y");
      creatables.push_back("Z");
      creatables.push_back("VX");
      creatables.push_back("VY");
      creatables.push_back("VZ");
      creatables.push_back("Cartesian");

      // Keplerian parameters
      creatables.push_back("SMA");
      creatables.push_back("ECC");
      creatables.push_back("INC");
      creatables.push_back("RAAN");
      creatables.push_back("RADN");
      creatables.push_back("AOP");
      creatables.push_back("TA");
      creatables.push_back("MA");
      creatables.push_back("EA");
      creatables.push_back("HA");
      creatables.push_back("MM");
      creatables.push_back("Keplerian");
      creatables.push_back("ModKeplerian");

      // Spherical parameters
      creatables.push_back("RMAG");
      creatables.push_back("RA");
      creatables.push_back("DEC");
      creatables.push_back("VMAG");
      creatables.push_back("RAV");
      creatables.push_back("DECV");
      creatables.push_back("AZI");
      creatables.push_back("FPA");
      creatables.push_back("SphericalRADEC");
      creatables.push_back("SphericalAZFPA");
      creatables.push_back("Altitude");

      // Orbital parameters
      creatables.push_back("VelApoapsis");
      creatables.push_back("VelPeriapsis");
      creatables.push_back("Apoapsis");
      creatables.push_back("Periapsis");
      creatables.push_back("OrbitPeriod");
      creatables.push_back("RadApo");
      creatables.push_back("RadPer");
      creatables.push_back("C3Energy");
      creatables.push_back("Energy");

      // Angular parameters
      creatables.push_back("SemilatusRectum");
      creatables.push_back("HMAG");
      creatables.push_back("HX");
      creatables.push_back("HY");
      creatables.push_back("HZ");
      
      // Environmental parameters
      creatables.push_back("AtmosDensity");
      
      // Planet parameters
      creatables.push_back("MHA");
      creatables.push_back("Longitude");
      creatables.push_back("Latitude");
      creatables.push_back("LST");
      creatables.push_back("BetaAngle");
      
      // B-Plane parameters
      creatables.push_back("BdotT");
      creatables.push_back("BdotR");
      creatables.push_back("BVectorMag");
      creatables.push_back("BVectorAngle");
      
      // Burn parameters
      creatables.push_back("Element1");
      creatables.push_back("Element2");
      creatables.push_back("Element3");
      creatables.push_back("V");
      creatables.push_back("N");
      creatables.push_back("B");
      
      // Attitude parameters
      creatables.push_back("DCM11");
      creatables.push_back("DCM12");
      creatables.push_back("DCM13");
      creatables.push_back("DCM21");
      creatables.push_back("DCM22");
      creatables.push_back("DCM23");
      creatables.push_back("DCM31");
      creatables.push_back("DCM32");
      creatables.push_back("DCM33");
      creatables.push_back("EulerAngle1");
      creatables.push_back("EulerAngle2");
      creatables.push_back("EulerAngle3");
      creatables.push_back("Q1");
      creatables.push_back("Q2");
      creatables.push_back("Q3");
      creatables.push_back("Q4");
      creatables.push_back("AngularVelocityX");
      creatables.push_back("AngularVelocityY");
      creatables.push_back("AngularVelocityZ");
      creatables.push_back("EulerAngleRate1");
      creatables.push_back("EulerAngleRate2");
      creatables.push_back("EulerAngleRate3");
   }
}


//------------------------------------------------------------------------------
//  ParameterFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ParameterFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects
 *
 */
//------------------------------------------------------------------------------
ParameterFactory::ParameterFactory(StringArray createList) :
   Factory(createList, Gmat::PARAMETER)
{
}


//------------------------------------------------------------------------------
//  ParameterFactory(const ParameterFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ParameterFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
ParameterFactory::ParameterFactory(const ParameterFactory &fact) :
   Factory(fact)
{
}


//------------------------------------------------------------------------------
//  ParameterFactory& operator= (const ParameterFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ParameterFactory base class.
 *
 * @param <fact> the ParameterFactory object whose data to assign to "this"
 *  factory.
 *
 * @return "this" ParameterFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
ParameterFactory& ParameterFactory::operator= (const ParameterFactory &fact)
{
   Factory::operator=(fact);
   return *this;
}


//------------------------------------------------------------------------------
// ~ParameterFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the ParameterFactory base class.
 */
//------------------------------------------------------------------------------
ParameterFactory::~ParameterFactory()
{
   // deletes handled by Factory destructor
}

