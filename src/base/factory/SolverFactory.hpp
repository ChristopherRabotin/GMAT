//$Header$
//------------------------------------------------------------------------------
//                            SolverFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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
	SolverFactory();
    // constructor
    SolverFactory(StringArray createList);
    // copy constructor
    SolverFactory(const SolverFactory& fact);
    // assignment operator
    SolverFactory&              operator=(const SolverFactory& fact);

	virtual ~SolverFactory();

    virtual Solver*             CreateSolver(std::string ofType,
                                             std::string withName /* = "" */); 
};

#endif // SolverFactory_hpp
