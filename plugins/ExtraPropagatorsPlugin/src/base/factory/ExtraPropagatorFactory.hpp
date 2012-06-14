//$Id: ExtraPropagatorFactory.hpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            ExtraPropagatorFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: 2012/06/14
//
/**
 * Declaration code for the ExtraPropagatorFactory class, which creates
 * propagators that are not part of the core GMAT system.
 */
//------------------------------------------------------------------------------


#ifndef ExtraPropagatorFactory_hpp
#define ExtraPropagatorFactory_hpp


#include "ExtraPropagatorDefs.hpp"
#include "Factory.hpp"


/**
 * ExtraPropagatorFactory is a factory plug-in that creates Propagator objects
 * for use in GMAT's propagation subsystem.
 */
class PROPAGATOR_API ExtraPropagatorFactory : public Factory
{
public:
   virtual Propagator* CreatePropagator(const std::string &ofType,
                                const std::string &withName /* = "" */);

   // default constructor
   ExtraPropagatorFactory();
   // constructor
   ExtraPropagatorFactory(StringArray createList);
   // copy constructor
   ExtraPropagatorFactory(const ExtraPropagatorFactory& fact);
   // assignment operator
   ExtraPropagatorFactory& operator=(const ExtraPropagatorFactory& fact);

   virtual ~ExtraPropagatorFactory();

};

#endif // ExtraPropagatorFactory_hpp
