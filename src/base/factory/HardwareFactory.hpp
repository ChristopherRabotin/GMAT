//$Id$
//------------------------------------------------------------------------------
//                         HardwareFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// order 124
//
// Author: Darrel Conway (Based on SpacecraftFactory)
// Created: 2004/11/10
//
/**
 *  This class is the factory class for hardware.  
 */
//------------------------------------------------------------------------------
#ifndef HardwareFactory_hpp
#define HardwareFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Hardware.hpp"

class GMAT_API HardwareFactory : public Factory
{
public:
   Hardware* CreateHardware(const std::string &ofType,
                            const std::string &withName = "");

   // default constructor
   HardwareFactory();
   // constructor
   HardwareFactory(StringArray createList);
   // copy constructor
   HardwareFactory(const HardwareFactory& fact);
   // assignment operator
   HardwareFactory& operator= (const HardwareFactory& fact);

   // destructor
   ~HardwareFactory();

protected:
   // protected data

private:
   // private data
};

#endif // HardwareFactory_hpp
