//$Header$
//------------------------------------------------------------------------------
//                            SolverException
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/1/4
//
/**
 * Definition for exceptions thrown by the Solver subsystem. 
 */
//------------------------------------------------------------------------------


#ifndef SolverException_hpp
#define SolverException_hpp

#include "BaseException.hpp"

class SolverException : public BaseException{
public:

    SolverException(const std::string &details,
                  const std::string &message = "Solver subsystem exception: ");
	virtual ~SolverException();
};

#endif // SolverException_hpp
