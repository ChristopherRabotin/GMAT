//$Id$
//------------------------------------------------------------------------------
//                                  Barycenter
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
#include "MessageInterface.hpp"
#include "GmatDefaults.hpp"
#include "SolarSystem.hpp"

//#define DEBUG_BARYCENTER
//#define DEBUG_BARYCENTER_STATE
//#define DEBUG_BARYCENTER_BODIES

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
   CalculatedPoint("Barycenter", itsName),
   isBuiltIn      (false),
   builtInType    (""),
   builtInSP      (NULL)
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
   CalculatedPoint          (bary),
   isBuiltIn                (bary.isBuiltIn),
   builtInType              (bary.builtInType),
   builtInSP                (NULL)
{
   parameterCount  = bary.parameterCount;
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
   parameterCount  = bary.parameterCount;
   isBuiltIn       = bary.isBuiltIn;
   builtInType     = builtInType;
   builtInSP       = bary.builtInSP;

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
   // if it's built-in, get the state from the SpacePoint
   if (isBuiltIn)
   {
      #ifdef DEBUG_BARYCENTER_STATE
         MessageInterface::ShowMessage("Computing state for Barycenter %s, whose builtInSP is %s\n",
               instanceName.c_str(), (builtInSP->GetName()).c_str());
      #endif
      return builtInSP->GetMJ2000State(atTime);
   }
   // otherwise, sum the masses and states
   CheckBodies();
   #ifdef DEBUG_BARYCENTER
      MessageInterface::ShowMessage("Entering BaryCenter;:GetMJ2000EqState at time %12.10f\n",
            atTime.Get());
   #endif
   Real     bodyMass = 0.0;
   Rvector3 bodyPos(0.0,0.0,0.0);
   Rvector3 bodyVel(0.0,0.0,0.0);

//   Real     sumMass  = 0.0;
   Real     weight   = 0.0;
   Rvector3 sumMassPos(0.0,0.0,0.0);
   Rvector3 sumMassVel(0.0,0.0,0.0);
   Rvector6 bodyState;
   Real      sumMass = GetMass();
//   for (unsigned int ii = 0; ii < bodyList.size(); ii++)
//      sumMass +=  ((CelestialBody*) (bodyList.at(ii)))->GetMass();

   for (unsigned int i = 0; i < bodyList.size() ; i++)
   {
      bodyMass    = ((CelestialBody*) (bodyList.at(i)))->GetMass();
      bodyState   = (bodyList.at(i))->GetMJ2000State(atTime);
      bodyPos     = bodyState.GetR();
      bodyVel     = bodyState.GetV();
      weight      = bodyMass/sumMass;
      #ifdef DEBUG_BARYCENTER
         MessageInterface::ShowMessage("Mass (and weight) of body %s = %12.10f (%12.10f)\n",
               ((bodyList.at(i))->GetName()).c_str(), bodyMass, weight);
         MessageInterface::ShowMessage("    pos = %s\n", (bodyPos.ToString()).c_str());
         MessageInterface::ShowMessage("    vel = %s\n", (bodyVel.ToString()).c_str());
      #endif
      sumMassPos += (weight * bodyPos);
      sumMassVel += (weight * bodyVel);
   }
   #ifdef DEBUG_BARYCENTER
      MessageInterface::ShowMessage("sumMassPos = %s\n",
            (sumMassPos.ToString()).c_str());
   #endif
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


//---------------------------------------------------------------------------
//  Real GetMass()
//---------------------------------------------------------------------------
/**
 * Method returning the total mass of the celestial bodies included in
 * this Barycenter.
 *
 * @return total mass of the celestial bodies included in this Barycenter.
 */
//---------------------------------------------------------------------------
Real Barycenter::GetMass() 
{
   // if it's built-in, get the mass from the SpacePoint
   if (isBuiltIn)
      return ((CelestialBody*)builtInSP)->GetMass();
   // otherwise, sum the masses of the bodies
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


//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Method that initializes this Barycenter.
 *
 * @return success flag for initializing
 */
//---------------------------------------------------------------------------
bool Barycenter::Initialize()
{
   #ifdef DEBUG_BARYCENTER_BODIES
      MessageInterface::ShowMessage("Entering Barycenter::Initialize\n   bodyNames:\n");
      for (unsigned int ii = 0; ii < bodyNames.size(); ii++)
         MessageInterface::ShowMessage("   %d    %s\n", ii, (bodyNames.at(ii)).c_str());
      MessageInterface::ShowMessage("and defaultBodies:\n");
      for (unsigned int ii = 0; ii < defaultBodies.size(); ii++)
         MessageInterface::ShowMessage("   %d    %s\n", ii, (defaultBodies.at(ii)).c_str());
   #endif
   if (isBuiltIn)
   {
      // create the builtInSP here
      if ((builtInType == "SSB") || (builtInType == GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME))
      {
         builtInSP = theSolarSystem->GetSpecialPoint(GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME);
         if (!builtInSP)
         {
            std::string errmsg = "No special point \"";
            errmsg += GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME + "\" found in Solar System.\n";
            throw SolarSystemException(errmsg);
         }
      }
      else // no others currently available
      {
         throw SolarSystemException("Unknown built-in barycenter\n");
      }
   }
   else
   {
      if ((bodyNames.empty()) && !(defaultBodies.empty()))
      {
         for (unsigned int ii = 0; ii < defaultBodies.size(); ii++)
            bodyNames.push_back(defaultBodies.at(ii));
      }
      if (bodyNames.empty())
      {
         std::string errmsg = "No celestial body specified for Barycenter ";
         errmsg += instanceName + "\n";
         throw SolarSystemException(errmsg);
      }
      #ifdef DEBUG_BARYCENTER_BODIES
         MessageInterface::ShowMessage("at end of Barycenter::Initialize\n   bodyNames:\n");
         for (unsigned int ii = 0; ii < bodyNames.size(); ii++)
            MessageInterface::ShowMessage("   %d    %s\n", ii, (bodyNames.at(ii)).c_str());
      #endif
   }
   return CalculatedPoint::Initialize();
}


bool Barycenter::IsBuiltIn()
{
   return isBuiltIn;
}

void Barycenter::SetIsBuiltIn(bool builtIn, const std::string &ofType)
{
   isBuiltIn   = builtIn;
   builtInType = ofType;
}

StringArray Barycenter::GetBuiltInNames()
{
   StringArray spNames;
   if (builtInType ==  "SSB")
      spNames.push_back("SolarSystemBarycenter");
   return spNames;
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
   if (bodyNames.empty())
      throw SolarSystemException("Attempting to use Barycenter with no bodies set ...\n");

   for (unsigned int i = 0; i < bodyList.size() ; i++)
      if ((bodyList.at(i))->GetType() != Gmat::CELESTIAL_BODY)
         throw SolarSystemException(
         "Barycenter defined incorrectly with non-Celestial Body components");
}


