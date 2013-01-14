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
 * Proxy code for Formation objects, used to define interfaces implemented in 
 * the Formation plugin. 
 */
//------------------------------------------------------------------------------

#ifndef FormationInterface_hpp
#define FormationInterface_hpp

#include "SpaceObject.hpp"

/**
 * Interface definition for Formations, implemented in libFormation.
 */
class GMAT_API FormationInterface : public SpaceObject
{
public:
   FormationInterface(Gmat::ObjectType typeId, const std::string &typeStr, 
             const std::string &instName);
   virtual ~FormationInterface();
   FormationInterface(const FormationInterface& fi);
   FormationInterface& operator=(const FormationInterface& fi);

   virtual void         BuildState() = 0;
   virtual void         UpdateElements() = 0;
   virtual void         UpdateState() = 0;
};

#endif // FormationInterface_hpp
