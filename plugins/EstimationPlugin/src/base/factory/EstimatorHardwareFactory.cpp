//$Id: EstimatorHardwareFactory.cpp 1398 2011-04-21 20:39:37Z ljun@NDC $
//------------------------------------------------------------------------------
//                             EstimatorHardwareFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/02/22 by Darrel Conway (Thinking Systems)
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task 28
//
/**
 * Implementation for the EstimatorHardwareFactory class used by the estimation
 * subsystem.
 */
//------------------------------------------------------------------------------


#include "EstimatorHardwareFactory.hpp"

// Specific hardware types this factory can create
#include "Antenna.hpp"
#include "Transmitter.hpp"
#include "Receiver.hpp"
#include "Transponder.hpp"


//------------------------------------------------------------------------------
// EstimatorHardwareFactory()
//------------------------------------------------------------------------------
/**
 * Default constructor
 */
//------------------------------------------------------------------------------
EstimatorHardwareFactory::EstimatorHardwareFactory() :
   Factory        (Gmat::HARDWARE)
{
   if (creatables.empty())
   {
   	creatables.push_back("Antenna");
      creatables.push_back("Transmitter");
      creatables.push_back("Receiver");
      creatables.push_back("Transponder");
   }
}


//------------------------------------------------------------------------------
// ~EstimatorHardwareFactory()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
EstimatorHardwareFactory::~EstimatorHardwareFactory()
{
}


//------------------------------------------------------------------------------
// EstimatorHardwareFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * Constructor using a new creation list
 *
 * @param createList The new creation list
 */
//------------------------------------------------------------------------------
EstimatorHardwareFactory::EstimatorHardwareFactory(StringArray createList) :
   Factory        (createList)
{
   if (creatables.empty())
   {
   	creatables.push_back("Antenna");
      creatables.push_back("Transmitter");
      creatables.push_back("Receiver");
      creatables.push_back("Transponder");
   }
}


//------------------------------------------------------------------------------
// EstimatorHardwareFactory(const EstimatorHardwareFactory& fact)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param fact The factory that is copied to this new one.
 */
//------------------------------------------------------------------------------
EstimatorHardwareFactory::EstimatorHardwareFactory(
         const EstimatorHardwareFactory& fact) :
   Factory        (fact)
{
   if (creatables.empty())
   {
   	creatables.push_back("Antenna");
      creatables.push_back("Transmitter");
      creatables.push_back("Receiver");
      creatables.push_back("Transponder");
   }
}


//------------------------------------------------------------------------------
// EstimatorHardwareFactory& operator=(const EstimatorHardwareFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param fact The factory that sets properties for this one
 *
 * @return This factory set to match fact
 */
//------------------------------------------------------------------------------
EstimatorHardwareFactory& EstimatorHardwareFactory::operator=(
         const EstimatorHardwareFactory& fact)
{
   if (this != &fact)
   {
      Factory::operator=(fact);
      if (creatables.empty())
      {
      	creatables.push_back("Antenna");
         creatables.push_back("Transmitter");
         creatables.push_back("Receiver");
         creatables.push_back("Transponder");
      }

   }

   return *this;
}



//------------------------------------------------------------------------------
// Hardware* CreateHardware(const std::string& ofType,
//          const std::string& withName)
//------------------------------------------------------------------------------
/**
 * Creates a new hardware element
 *
 * @param ofType The scripted type name for the new object
 * @param withName The name of the new object
 *
 * @return The new object
 */
//------------------------------------------------------------------------------
Hardware* EstimatorHardwareFactory::CreateHardware(const std::string& ofType,
         const std::string& withName)
{
   Hardware *retval = NULL;

   if (ofType == "Antenna")
   	retval = new Antenna(ofType, withName);
   else if (ofType == "Transmitter")
      retval = new Transmitter(withName);
   else if (ofType == "Receiver")
   	retval = new Receiver(withName);
   else if (ofType == "Transponder")
   	retval = new Transponder(withName);

   return retval;
}
