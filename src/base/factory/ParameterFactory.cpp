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
#include "AngularParameters.hpp"

//loj: 3/18/04 grouped relevant classes into one file
//  #include "ElapsedDaysParam.hpp" 
//  #include "ElapsedSecsParam.hpp" 
//  #include "CurrentA1MjdParam.hpp"
//  #include "CartXParam.hpp"
//  #include "CartYParam.hpp"
//  #include "CartZParam.hpp"
//  #include "CartVxParam.hpp"
//  #include "CartVyParam.hpp"
//  #include "CartVzParam.hpp"
//  #include "KepSmaParam.hpp"
//  #include "KepEccParam.hpp"
//  #include "KepIncParam.hpp"
//  #include "KepRaanParam.hpp"
//  #include "KepAopParam.hpp"
//  #include "KepTaParam.hpp"

#include "SMA.hpp"
#include "Ecc.hpp"
#include "Inc.hpp"


// add others here for future builds

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
    if (ofType == "CurrA1Mjd")
        return new CurrA1Mjd(withName);

    // Cartesian paramters
    if (ofType == "CartX")
        return new CartX(withName);
    if (ofType == "CartY")
        return new CartY(withName);
    if (ofType == "CartZ")
        return new CartZ(withName);
    if (ofType == "CartVx")
        return new CartVx(withName);
    if (ofType == "CartVy")
        return new CartVy(withName);
    if (ofType == "CartVz")
        return new CartVz(withName);

    // Keplerian parameters
    if (ofType == "KepSMA")
        return new KepSMA(withName);
    if (ofType == "KepEcc")
        return new KepEcc(withName);
    if (ofType == "KepInc")
        return new KepInc(withName);
    if (ofType == "KepRAAN")
        return new KepRAAN(withName);
    if (ofType == "KepAOP")
        return new KepAOP(withName);
    if (ofType == "KepTA")
        return new KepTA(withName);
    if (ofType == "KepMA")
        return new KepMA(withName);
    if (ofType == "KepMM")
        return new KepMM(withName);
    if (ofType == "VelApoapsis")
        return new VelApoapsis(withName);
    if (ofType == "VelPeriapsis")
        return new VelPeriapsis(withName);
    if (ofType == "SMA")
        return new SMA(withName);
    if (ofType == "Ecc")
        return new Ecc(withName);
    if (ofType == "Inc")
        return new Inc(withName);

    // Spherical parameters
    if (ofType == "SphRA")
        return new SphRA(withName);
    if (ofType == "SphDec")
        return new SphDec(withName);
    
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
        creatables.push_back("CurrA1Mjd");

        // Cartesian parameters
        creatables.push_back("CartX");
        creatables.push_back("CartY");
        creatables.push_back("CartZ");
        creatables.push_back("CartVx");
        creatables.push_back("CartVy");
        creatables.push_back("CartVz");

        // Keplerian parameters
        creatables.push_back("KepSMA");
        creatables.push_back("KepEcc");
        creatables.push_back("KepInc");
        creatables.push_back("KepRAAN");
        creatables.push_back("KepAOP");
        creatables.push_back("KepTA");
        creatables.push_back("SMA");
        creatables.push_back("Ecc");
        creatables.push_back("Inc");
        creatables.push_back("KepMA");
        creatables.push_back("KepMM");
        creatables.push_back("VelApoapsis");
        creatables.push_back("VelPeriapsis");

        // Spherical parameters
        creatables.push_back("SphRA");
        creatables.push_back("SphDec");

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

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------
