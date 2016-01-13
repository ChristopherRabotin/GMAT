//------------------------------------------------------------------------------
//                           BeginFileThrust
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 * Toggle command used to tell GMAT to use a thrust data file
 */
//------------------------------------------------------------------------------


#ifndef BeginFileThrust_hpp
#define BeginFileThrust_hpp

#include "ThrustFileDefs.hpp"
#include "GmatCommand.hpp"
#include "FileThrust.hpp"
#include "PhysicalModel.hpp"
#include "Spacecraft.hpp"


class BeginFileThrust: public GmatCommand
{
public:
   BeginFileThrust(const std::string &name = "");
   virtual ~BeginFileThrust();
   BeginFileThrust(const BeginFileThrust& bft);
   BeginFileThrust& operator=(const BeginFileThrust& bft);

   virtual GmatBase* Clone() const;
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);

   virtual void SetTransientForces(std::vector<PhysicalModel*> *tf);
   virtual bool Initialize();
   virtual bool Execute();

   DEFAULT_TO_NO_CLONES

protected:
   /// The FileThrust that is available for the force models
   FileThrust *burnForce;
   /// The vector of forces managed by the Sandbox
   std::vector<PhysicalModel*>   *transientForces;
   /// The names of the spacecraft that get maneuvered
   StringArray                   satNames;
   /// The spacecraft that get maneuvered
   std::vector<Spacecraft *>     sats;
};

#endif /* BeginFileThrust_hpp */
