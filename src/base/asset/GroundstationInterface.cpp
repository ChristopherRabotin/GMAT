//$Id$
//------------------------------------------------------------------------------
//                           FormationInterface
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CI63P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2013/01/09
/**
 * Proxy code for Groundstation objects, used to define interfaces implemented
 * in the Groundstation plugin.
 */
//------------------------------------------------------------------------------

#include "GroundstationInterface.hpp"


//------------------------------------------------------------------------------
// GroundstationInterface(const std::string &itsTypeName,
//       const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * Constructor
 *
 * @param itsTypeName The type name of the new object
 * @param itsName The name of the new object
 */
//------------------------------------------------------------------------------
GroundstationInterface::GroundstationInterface(const std::string &itsTypeName,
       const std::string &itsName) :
   BodyFixedPoint    (itsTypeName, itsName, Gmat::GROUND_STATION)
{
   objectTypes.push_back(Gmat::GROUND_STATION);
}


//------------------------------------------------------------------------------
// ~GroundstationInterface()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
GroundstationInterface::~GroundstationInterface()
{
}


//------------------------------------------------------------------------------
// GroundstationInterface(const GroundstationInterface& gsi)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 */
//------------------------------------------------------------------------------
GroundstationInterface::GroundstationInterface(
		const GroundstationInterface& gsi) :
   BodyFixedPoint          (gsi)
{
}



//------------------------------------------------------------------------------
// GroundstationInterface& operator=(const GroundstationInterface& gsi)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 */
//------------------------------------------------------------------------------
GroundstationInterface& GroundstationInterface::operator=(
		const GroundstationInterface& gsi)
{
   if (this != &gsi)
   {
      BodyFixedPoint::operator=(gsi);
   }
   return *this;
}
