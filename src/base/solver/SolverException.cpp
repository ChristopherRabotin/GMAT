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
 * Implementation for exceptions thrown by the Solver subsystem. 
 */
//------------------------------------------------------------------------------


#include "SolverException.hpp"

SolverException::SolverException(const std::string &details,
                                 const std::string &message) :
    BaseException       (message, details)
{}


SolverException::~SolverException()
{}
