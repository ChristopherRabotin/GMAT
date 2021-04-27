//$Id: VernerFactory.hpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            VernerFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: 2008/07/03
//
/**
 * Declaration code for the VernerFactory class, which creates objects that extend
 * the SRP model for solar sailing.
 *
 * This is sample code demonstrating GMAT's plug-in capabilities.
 */
//------------------------------------------------------------------------------


#ifndef EphemPropFactory_hpp
#define EphemPropFactory_hpp


#include "ephempropagator_defs.hpp"
#include "Factory.hpp"


/**
 * VernerFactory is a factory plug-in that creates SolarSailForce objects for use
 * in GMAT's force model.
 */
class EPHEM_PROPAGATOR_API EphemPropFactory : public Factory
{
public:
   virtual GmatBase* CreateObject(const std::string &ofType,
                                const std::string &withName /* = "" */);
   virtual Propagator* CreatePropagator(const std::string &ofType,
                                const std::string &withName /* = "" */);

   // default constructor
   EphemPropFactory();
   // constructor
   EphemPropFactory(StringArray createList);
   // copy constructor
   EphemPropFactory(const EphemPropFactory& fact);
   // assignment operator
   EphemPropFactory& operator=(const EphemPropFactory& fact);

   virtual ~EphemPropFactory();

};

#endif // EphemPropFactory_hpp
