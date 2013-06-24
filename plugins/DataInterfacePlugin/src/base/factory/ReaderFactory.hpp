//$Id$
//------------------------------------------------------------------------------
//                             ReaderFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: September 20, 2012
//
//
/**
 * Definition of the factory used to create DataReaders
 */
//------------------------------------------------------------------------------


#ifndef ReaderFactory_hpp
#define ReaderFactory_hpp

#include "DataInterfaceDefs.hpp"
#include "Factory.hpp"


/**
 * Factory class that creates one class of your plugin's objects
 *
 * Rename this class from SaveCommandFactory( to something more descriptive of your 
 * plugin object(s).
 */
class DATAINTERFACE_API ReaderFactory : public Factory
{
public:
   ReaderFactory();
   virtual ~ReaderFactory();
   ReaderFactory(const ReaderFactory& elf);
   ReaderFactory& operator=(const ReaderFactory& elf);

   virtual GmatBase* CreateObject(const std::string &ofType,
                                  const std::string &withName);
};

#endif /* ReaderFactory_hpp */
