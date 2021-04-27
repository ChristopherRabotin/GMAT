//------------------------------------------------------------------------------
//                        ThrustFileCommandFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 13, 2016
/**
 *
 */
//------------------------------------------------------------------------------


#ifndef ThrustFileCommandFactory_hpp
#define ThrustFileCommandFactory_hpp

#include "ThrustFileDefs.hpp"
#include "Factory.hpp"


/**
 * Factory class that creates one class of your plugin's objects
 *
 * Rename this class from ThrustFileCommandFactory( to something more descriptive of your
 * plugin object(s).
 *
 * @note: The "SAMPLE_API" descriptor needs to match the definition in your 
 * ...Defs.hpp file
 */
class THRUSTFILE_API ThrustFileCommandFactory : public Factory
{
public:
   ThrustFileCommandFactory();
   virtual ~ThrustFileCommandFactory();
   ThrustFileCommandFactory(const ThrustFileCommandFactory& elf);
   ThrustFileCommandFactory& operator=(const ThrustFileCommandFactory& elf);

   virtual GmatCommand*     CreateCommand(const std::string &ofType,
                                          const std::string &withName = "");
};

#endif /* ThrustFileCommandFactory_hpp */
