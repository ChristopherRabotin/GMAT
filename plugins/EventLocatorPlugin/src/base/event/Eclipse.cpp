//$Id: Eclipse.cpp 2182 2012-03-01 22:20:38Z djconway@NDC $
//------------------------------------------------------------------------------
//                           Eclipse
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under NASA Prime
// Contract NNG10CP02C, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jul 6, 2011
//
/**
 * Implementation of the Eclipse event function.
 */
//------------------------------------------------------------------------------


#include "Eclipse.hpp"
#include "CelestialBody.hpp"
#include "MessageInterface.hpp"
#include "EventException.hpp"


//#define DEBUG_EVENTFUNCTIONS
//#define DEBUG_ORIGIN

//------------------------------------------------------------------------------
// Eclipse(const std::string &typeStr)
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
Eclipse::Eclipse(const std::string &typeStr) :
   EventFunction        (typeStr),
   body                 (NULL),
   sol                  (NULL),
   starRadius           (0.0),
   bodyRadius           (0.0),
   eqRadiusID           (-1)
{
}


//------------------------------------------------------------------------------
// ~Eclipse()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Eclipse::~Eclipse()
{
}


//------------------------------------------------------------------------------
// Eclipse(const Eclipse& ef)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param ef The original Eclipse function being copied
 */
//------------------------------------------------------------------------------
Eclipse::Eclipse(const Eclipse& ef) :
   EventFunction        (ef),
   body                 (ef.body),
   sol                  (ef.sol),
   starRadius           (ef.starRadius),
   bodyRadius           (ef.bodyRadius),
   eqRadiusID           (ef.eqRadiusID)
{
}

//------------------------------------------------------------------------------
// Eclipse& operator=(const Eclipse& ef)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param ef The Eclipse function providing parameters for this one
 *
 * @return this Eclipse EventFunction, set to match ef
 */
//------------------------------------------------------------------------------
Eclipse& Eclipse::operator=(const Eclipse& ef)
{
   if (this != &ef)
   {
      EventFunction::operator=(ef);

      body       = ef.body;
      sol        = ef.sol;
      starRadius = ef.starRadius;
      bodyRadius = ef.bodyRadius;
      eqRadiusID = ef.eqRadiusID;
   }

   return *this;
}

//------------------------------------------------------------------------------
// bool SetSol(CelestialBody *sun)
//------------------------------------------------------------------------------
/**
 * Sets the sun pointer
 *
 * @param sun The pointer
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Eclipse::SetSol(CelestialBody *sun)
{
   sol = sun;
   return (sol != NULL);
}


//------------------------------------------------------------------------------
// bool SetBody(CelestialBody *bod)
//------------------------------------------------------------------------------
/**
 * Sets the eclipsing body pointer
 *
 * @param bod The eclipsing body
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Eclipse::SetBody(CelestialBody *bod)
{
   body = bod;
   return (body != NULL);
}




//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Prepared the Eclipse function for use
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool Eclipse::Initialize()
{
   bool retval = EventFunction::Initialize();

   if (retval)
   {
      if ((sol == NULL) || (body == NULL))
         throw EventException("Unable to initialize the " + typeName +
               " EventFunction; the solar system bodies are not set.");

      if (eqRadiusID < 0)
         eqRadiusID = body->GetParameterID("EquatorialRadius");

      starRadius = sol->GetRealParameter(eqRadiusID);
      bodyRadius = body->GetRealParameter(eqRadiusID);

      #ifdef DEBUG_EVENTFUNCTIONS
         MessageInterface::ShowMessage("The %s's radius is %12.4lf; the %s's, "
               "%12.4lf\n", sol->GetName().c_str(), starRadius,
               body->GetName().c_str(), bodyRadius);
      #endif

      instanceName = primary->GetName();
      instanceName += " - ";
      instanceName += body->GetName();
   }

   return retval;
}
