//$Id$
//------------------------------------------------------------------------------
//                           ThrustFileReaderFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS 
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 14, 2016
/**
 * 
 */
//------------------------------------------------------------------------------

#ifndef ThrustFileReaderFactory_hpp
#define ThrustFileReaderFactory_hpp

#include "ThrustFileDefs.hpp"
#include "Factory.hpp"

class ThrustFileReaderFactory: public Factory
{
public:
   ThrustFileReaderFactory();
   virtual ~ThrustFileReaderFactory();
   ThrustFileReaderFactory(const ThrustFileReaderFactory& elf);
   ThrustFileReaderFactory& operator=(const ThrustFileReaderFactory& elf);

   virtual GmatBase* CreateObject(const std::string &ofType,
                                  const std::string &withName);
};

#endif /* ThrustFileReaderFactory_hpp */
