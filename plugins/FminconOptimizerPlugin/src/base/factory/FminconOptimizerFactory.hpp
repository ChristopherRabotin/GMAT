//$Id: FminconOptimizerFactory.hpp 9461 2011-04-21 22:10:15Z lindajun $
//------------------------------------------------------------------------------
//                            FminconOptimizerFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
// Author: Linda Jun
// Created: 2010/03/30
//
/**
 *  Declaration code for the FminconOptimizerFactory class.
 */
//------------------------------------------------------------------------------
#ifndef FminconOptimizerFactory_hpp
#define FminconOptimizerFactory_hpp


#include "fmincon_defs.hpp"
#include "Factory.hpp"
#include "Solver.hpp"

class FMINCON_API FminconOptimizerFactory : public Factory
{
public:
   virtual Solver* CreateSolver(const std::string &ofType,
                                const std::string &withName);
   
   // default constructor
   FminconOptimizerFactory();
   // constructor
   FminconOptimizerFactory(StringArray createList);
   // copy constructor
   FminconOptimizerFactory(const FminconOptimizerFactory& fact);
   // assignment operator
   FminconOptimizerFactory& operator=(const FminconOptimizerFactory& fact);
   
   virtual ~FminconOptimizerFactory();
   
};

#endif // FminconOptimizerFactory_hpp
