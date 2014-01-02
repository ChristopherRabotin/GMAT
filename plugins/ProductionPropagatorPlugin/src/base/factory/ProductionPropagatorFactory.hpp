//$Id: ProductionPropagatorFactory.hpp,v 1.1 2008/07/03 19:15:33 djc Exp $
//------------------------------------------------------------------------------
//                            ProductionPropagatorFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Darrel Conway
// Created: Nov 6, 2013
//
/**
 * Declaration code for the ProductionPropagatorFactory class, which creates
 * propagators that are not part of the core GMAT system.
 */
//------------------------------------------------------------------------------


#ifndef ProductionPropagatorFactory_hpp
#define ProductionPropagatorFactory_hpp


#include "ProductionPropagatorDefs.hpp"
#include "Factory.hpp"


/**
 * ProductionPropagatorFactory is a factory plug-in that creates Propagator objects
 * for use in GMAT's propagation subsystem.
 */
class PRODUCTIONPROPAGATOR_API ProductionPropagatorFactory : public Factory
{
public:
   virtual Propagator* CreatePropagator(const std::string &ofType,
                                const std::string &withName /* = "" */);

   // default constructor
   ProductionPropagatorFactory();
   // constructor
   ProductionPropagatorFactory(StringArray createList);
   // copy constructor
   ProductionPropagatorFactory(const ProductionPropagatorFactory& fact);
   // assignment operator
   ProductionPropagatorFactory& operator=(const ProductionPropagatorFactory& fact);

   virtual ~ProductionPropagatorFactory();

};

#endif // ProductionPropagatorFactory_hpp
