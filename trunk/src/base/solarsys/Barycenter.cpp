//$Id$
//------------------------------------------------------------------------------
//                                  Barycenter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan
// Created: 2005/04/05
//
/**
 * Implementation of the Barycenter class.
 *
 */
//------------------------------------------------------------------------------

#include <vector>
#include "gmatdefs.hpp"
#include "CalculatedPoint.hpp"
#include "Barycenter.hpp"
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"

//---------------------------------
// static data
//---------------------------------
//const std::string
//Barycenter::PARAMETER_TEXT[BarycenterParamCount - CalculatedPointParamCount] =
//{
//   "",
//};

//const Gmat::ParameterType
//Barycenter::PARAMETER_TYPE[BarycenterParamCount - CalculatedPointParamCount] =
//{
//   Gmat::,
//};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  Barycenter(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Barycenter class
 * (default constructor).
 *
 * @param <itsName> parameter indicating the name of the barycenter.
 */
//------------------------------------------------------------------------------
Barycenter::Barycenter(const std::string &itsName) :
CalculatedPoint("Barycenter", itsName)
{
   objectTypes.push_back(Gmat::BARYCENTER);
   objectTypeNames.push_back("Barycenter");
   parameterCount = BarycenterParamCount;
}

//------------------------------------------------------------------------------
//  Barycenter(const Barycenter &bary)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Barycenter class as a copy of the
 * specified Barycenter class (copy constructor).
 *
 * @param <bary> Barycenter object to copy.
 */
//------------------------------------------------------------------------------
Barycenter::Barycenter(const Barycenter &bary) :
CalculatedPoint          (bary)
{
   parameterCount = bary.parameterCount;
}

//------------------------------------------------------------------------------
//  Barycenter& operator= (const Barycenter& bary)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Barycenter class.
 *
 * @param <bary> the Barycenter object whose data to assign to "this"
 *             calculated point.
 *
 * @return "this" Barycenter with data of input Barycenter bary.
 */
//------------------------------------------------------------------------------
Barycenter& Barycenter::operator=(const Barycenter &bary)
{
   if (&bary == this)
      return *this;

   CalculatedPoint::operator=(bary);
   parameterCount = bary.parameterCount;
   
   return *this;
}

//------------------------------------------------------------------------------
//  ~Barycenter()
//------------------------------------------------------------------------------
/**
 * Destructor for the Barycenter class.
 */
//------------------------------------------------------------------------------
Barycenter::~Barycenter()
{
   // nothing to do here ..... hm .. hm .. hm
}

//---------------------------------------------------------------------------
//  const Rvector6 GetMJ2000State(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000 state of the Barycenter at the time atTime.
 *
 * @param <atTime> Time for which the state is requested.
 *
 * @return state of the Barycenter at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector6 Barycenter::GetMJ2000State(const A1Mjd &atTime)
{
   CheckBodies();
   Real     bodyMass = 0.0;
   Rvector3 bodyPos(0.0,0.0,0.0);
   Rvector3 bodyVel(0.0,0.0,0.0);
   
   Real     sumMass  = 0.0;
   Rvector3 sumMassPos(0.0,0.0,0.0);
   Rvector3 sumMassVel(0.0,0.0,0.0);
   for (unsigned int i = 0; i < bodyList.size() ; i++)
   {
      bodyMass    = ((CelestialBody*) (bodyList.at(i)))->GetMass();
      bodyPos     = (bodyList.at(i))->GetMJ2000Position(atTime);
      bodyVel     = (bodyList.at(i))->GetMJ2000Velocity(atTime);
      sumMass    += bodyMass;
      sumMassPos += (bodyMass * bodyPos);
      sumMassVel += (bodyMass * bodyVel);
   }
   sumMassPos    /= sumMass;
   sumMassVel    /= sumMass;
   return Rvector6(sumMassPos(0), sumMassPos(1), sumMassPos(2),
                   sumMassVel(0), sumMassVel(1), sumMassVel(2));
}

//---------------------------------------------------------------------------
//  const Rvector3 GetMJ2000Position(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000 position of the Barycenter at the time atTime.
 *
 * @param <atTime> Time for which the position is requested.
 *
 * @return position of the Barycenter at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector3 Barycenter::GetMJ2000Position(const A1Mjd &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetR());
}

//---------------------------------------------------------------------------
//  const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000 velocity of the Barycenter at the time atTime.
 *
 * @param <atTime> Time for which the velocity is requested.
 *
 * @return velocity of the Barycenter at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector3 Barycenter::GetMJ2000Velocity(const A1Mjd &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetV());
}

Real Barycenter::GetMass() 
{
   Real     sumMass  = 0.0;
   for (unsigned int i = 0; i < bodyList.size() ; i++)
   {
      sumMass    += ((CelestialBody*) (bodyList.at(i)))->GetMass();
   }
   return sumMass;
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
/*std::string Barycenter::GetParameterText(const Integer id) const
{
   if (id >= CalculatedPointParamCount && id < BarycenterParamCount)
      return PARAMETER_TEXT[id - CalculatedPointParamCount];
   return CalculatedPoint::GetParameterText(id);
}*/

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
/*Integer     Barycenter::GetParameterID(const std::string &str) const
{
   for (Integer i = CalculatedPointParamCount; i < BarycenterParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - CalculatedPointParamCount])
         return i;
   }
   
   return CalculatedPoint::GetParameterID(str);
}*/

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
/*Gmat::ParameterType Barycenter::GetParameterType(const Integer id) const
{
   if (id >= CalculatedPointParamCount && id < BarycenterParamCount)
      return PARAMETER_TYPE[id - CalculatedPointParamCount];
      
   return CalculatedPoint::GetParameterType(id);
}*/

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
/*std::string Barycenter::GetParameterTypeString(const Integer id) const
{
   return CalculatedPoint::PARAM_TYPE_STRING[GetParameterType(id)];
}*/

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Barycenter.
 *
 * @return clone of the Barycenter.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Barycenter::Clone() const
{
   return (new Barycenter(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void Barycenter::Copy(const GmatBase* orig)
{
   // We don't want to copy instanceName
   std::string name = instanceName;
   operator=(*((Barycenter *)(orig)));
   instanceName = name;
}


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  void CheckBodies()
//------------------------------------------------------------------------------
/**
 * Method for the Barycenter class that checks to make sure the bodyList has
 * been defined appropriately (i.e. all CelestialBody objects).
 *
 */
//------------------------------------------------------------------------------
void Barycenter::CheckBodies()
{
   for (unsigned int i = 0; i < bodyList.size() ; i++)
      if ((bodyList.at(i))->GetType() != Gmat::CELESTIAL_BODY)
         throw SolarSystemException(
         "Barycenter defined incorrectly with non-Celestial Body components");
}


