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

#ifndef GroundstationInterface_hpp
#define GroundstationInterface_hpp

#include "BodyFixedPoint.hpp"

/**
 * Defines interfaces used in the Groundstation, contained in libStation.
 */
class GMAT_API GroundstationInterface : public BodyFixedPoint
{
public:
   GroundstationInterface(const std::string &itsTypeName, 
      const std::string &itsName);

   // Set d'tor pure abstract even though implementation exists to prevent instantiation
   virtual ~GroundstationInterface() = 0;

   GroundstationInterface(const GroundstationInterface& gsi);
   GroundstationInterface& operator=(const GroundstationInterface& gsi);
};

#endif
