//$Id$
//------------------------------------------------------------------------------
//                         PropagatorFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/14
//
/**
 *  This class is the factory class for propagators.  
 */
//------------------------------------------------------------------------------
#ifndef PropagatorFactory_hpp
#define PropagatorFactory_hpp

#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "Propagator.hpp"

class GMAT_API PropagatorFactory : public Factory
{
public:
   Propagator*  CreateObject(const std::string &ofType,
                             const std::string &withName = "");
   Propagator*  CreatePropagator(const std::string &ofType,
                                 const std::string &withName = "");

   // default constructor
   PropagatorFactory();
   // constructor
   PropagatorFactory(StringArray createList);
   // copy constructor
   PropagatorFactory(const PropagatorFactory &fact);
   // assignment operator
   PropagatorFactory& operator= (const PropagatorFactory &fact);

   // destructor
   ~PropagatorFactory();

protected:
   // protected data

private:
   // private data


};

#endif // PropagatorFactory_hpp

