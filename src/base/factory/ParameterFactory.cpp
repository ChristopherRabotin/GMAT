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
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "ParameterFactory.hpp"

#include "TimeParameters.hpp"
#include "CartesianParameters.hpp"
#include "KeplerianParameters.hpp"
#include "SphericalParameters.hpp"
#include "OrbitalParameters.hpp"
#include "AngularParameters.hpp"

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CreateParameter(std::string ofType, std::string withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Parameter class
 *
 * @param <ofType> the Parameter object to create and return.
 * @param <withName> the name to give the newly-created Parameter object.
 */
//------------------------------------------------------------------------------
Parameter* ParameterFactory::CreateParameter(std::string ofType,
                                             std::string withName)
{
   // Time parameters
   if (ofType == "ElapsedDays")
      return new ElapsedDays(withName);
   if (ofType == "ElapsedSecs")
      return new ElapsedSecs(withName);
   if (ofType == "CurrA1MJD")
      return new CurrA1MJD(withName);

    // Cartesian paramters
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
   if (ofType == "CartState")  //loj: 9/2/04 added
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
   if (ofType == "AOP")
      return new KepAOP(withName);
   if (ofType == "TA")
      return new KepTA(withName);
   if (ofType == "MA")
      return new KepMA(withName);
   if (ofType == "MM")
      return new KepMM(withName);
   if (ofType == "KepElem")  //loj: 9/2/04 added
      return new KepElem(withName);

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
   if (ofType == "SphElem")  //loj: 9/2/04 added
      return new SphElem(withName);

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

   // Angular parameters
   if (ofType == "SemilatusRectum")
      return new SemilatusRectum(withName);

   // add others here
   else
      return NULL;
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
      // Time parameters
      creatables.push_back("ElapsedDays");
      creatables.push_back("ElapsedSecs");
      creatables.push_back("CurrA1MJD");

      // Cartesian parameters
      creatables.push_back("X");
      creatables.push_back("Y");
      creatables.push_back("Z");
      creatables.push_back("VX");
      creatables.push_back("VY");
      creatables.push_back("VZ");
      creatables.push_back("CartState"); //loj: 9/2/04 added

      // Keplerian parameters
      creatables.push_back("SMA");
      creatables.push_back("ECC");
      creatables.push_back("INC");
      creatables.push_back("RAAN");
      creatables.push_back("AOP");
      creatables.push_back("TA");
      creatables.push_back("MA");
      creatables.push_back("MM");
      creatables.push_back("KepElem"); //loj: 9/2/04 added

      // Spherical parameters
      creatables.push_back("RMAG");
      creatables.push_back("RA");
      creatables.push_back("DEC");
      creatables.push_back("VMAG");
      creatables.push_back("RAV");
      creatables.push_back("DECV");
      creatables.push_back("SphElem"); //loj: 9/2/04 added

      // Orbital parameters
      creatables.push_back("VelApoapsis");
      creatables.push_back("VelPeriapsis");
      creatables.push_back("Apoapsis");
      creatables.push_back("Periapsis");
      creatables.push_back("OrbitPeriod");

      // Angular parameters
      creatables.push_back("SemilatusRectum");
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
//  PropagatorFactory(const PropagatorFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ParameterFactory
 * (copy constructor).
   *
   * @param <fact> the factory object to copy to "this" factory.
   */
//------------------------------------------------------------------------------
ParameterFactory::ParameterFactory(const ParameterFactory& fact) :
   Factory(fact)
{
}

//------------------------------------------------------------------------------
//  ParameterFactory& operator= (const ParameterFactory& fact)
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
ParameterFactory& ParameterFactory::operator= (const ParameterFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~ParameterFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the ParameterFactory base class.
   *
   */
//------------------------------------------------------------------------------
ParameterFactory::~ParameterFactory()
{
   // deletes handled by Factory destructor
}

