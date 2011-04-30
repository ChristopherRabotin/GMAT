//$Id$
//------------------------------------------------------------------------------
//                            SolverFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel Conway
// Created: 2004/02/02  (Groundhog Day)
//
/**
 *  Implementation code for the SolverFactory class, responsible for creating
 *  targeters, optimizers, and other parameteric scanning objects.
 */
//------------------------------------------------------------------------------


#ifndef SolverFactory_hpp
#define SolverFactory_hpp


#include "Factory.hpp"


class SolverFactory : public Factory
{
public:
   virtual Solver* CreateSolver(const std::string &ofType,
                                const std::string &withName /* = "" */);
   
   // default constructor
   SolverFactory();
   // constructor
   SolverFactory(StringArray createList);
   // copy constructor
   SolverFactory(const SolverFactory& fact);
   // assignment operator
   SolverFactory& operator=(const SolverFactory& fact);

   virtual ~SolverFactory();
   
   virtual bool DoesObjectTypeMatchSubtype(const std::string &theType,
                                           const std::string &theSubtype);
};

#endif // SolverFactory_hpp
